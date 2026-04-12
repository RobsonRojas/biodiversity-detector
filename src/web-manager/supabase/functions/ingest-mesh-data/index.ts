import { serve } from "https://deno.land/std@0.168.0/http/server.ts"
import { createClient } from "https://esm.sh/@supabase/supabase-js@2"

serve(async (req: Request) => {
  try {
    const payload = await req.json();
    const { node_id, status, battery_mv, rssi_dbm, confidence, latitude, longitude } = payload;
    
    const supabase = createClient(
      // @ts-ignore: Deno global
      Deno.env.get('SUPABASE_URL') || '',
      // @ts-ignore: Deno global
      Deno.env.get('SUPABASE_SERVICE_ROLE_KEY') || ''
    );

    // 1. Upsert node information
    const { error: nodeError } = await supabase
      .from('nodes')
      .upsert({ 
        id: node_id, 
        status: status || 'online', 
        battery_mv, 
        rssi_dbm, 
        latitude, 
        longitude,
        last_seen: new Date().toISOString()
      });
    if (nodeError) throw nodeError;

    // 2. If it's a detection, run Weighted Centroid Localization (WCL)
    if (confidence !== undefined) {
      // a. Insert the raw detection
      const { data: currentDetection, error: detectionError } = await supabase
        .from('detections')
        .insert([{ 
          node_id, 
          confidence, 
          pushed_at: new Date().toISOString(),
          last_rssi: rssi_dbm,
          sound_class: payload.sound_class || 'chainsaw'
        }])
        .select()
        .single();
      
      if (detectionError) throw detectionError;

      // b. Correlation: Find other detections within 10s of now
      const tenSecondsAgo = new Date(Date.now() - 10000).toISOString();
      const { data: recentDetections, error: correlateError } = await supabase
        .from('detections')
        .select(`
          node_id,
          confidence,
          nodes:node_id (latitude, longitude)
        `)
        .eq('sound_class', payload.sound_class || 'chainsaw')
        .gt('pushed_at', tenSecondsAgo);
      
      if (!correlateError && recentDetections && recentDetections.length >= 1) {
        // c. Multi-node Weighted Centroid Calculation
        let sumWeights = 0;
        let weightedLat = 0;
        let weightedLon = 0;
        const observerNodes = [];

        for (const det of recentDetections) {
           if (det.nodes && det.nodes.latitude && det.nodes.longitude) {
              const weight = det.confidence;
              sumWeights += weight;
              weightedLat += det.nodes.latitude * weight;
              weightedLon += det.nodes.longitude * weight;
              observerNodes.push(det.node_id);
           }
        }

        if (sumWeights > 0) {
           const eventLat = weightedLat / sumWeights;
           const eventLon = weightedLon / sumWeights;

           // d. Upsert the localized acoustic event
           await supabase
             .from('acoustic_events')
             .upsert({
                sound_class: payload.sound_class || 'chainsaw',
                latitude: eventLat,
                longitude: eventLon,
                avg_confidence: sumWeights / recentDetections.length,
                observer_nodes: observerNodes,
                last_detected_at: new Date().toISOString()
             }, { onConflict: 'sound_class,latitude,longitude' }); // Simplified conflict logic
        }
      }
    }

    return new Response(JSON.stringify({ success: true }), {
      headers: { "Content-Type": "application/json" },
    });
  } catch (err: any) {
    return new Response(JSON.stringify({ error: err.message }), { status: 500 });
  }
});

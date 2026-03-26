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

    // 2. If it's a detection, insert record
    if (confidence !== undefined) {
      const { error: detectionError } = await supabase
        .from('detections')
        .insert([{ 
          node_id, 
          confidence, 
          timestamp: new Date().toISOString() 
        }]);
      if (detectionError) throw detectionError;
    }

    return new Response(JSON.stringify({ success: true }), {
      headers: { "Content-Type": "application/json" },
    });
  } catch (err: any) {
    return new Response(JSON.stringify({ error: err.message }), { status: 500 });
  }
});

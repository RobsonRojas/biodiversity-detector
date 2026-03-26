import { serve } from "https://deno.land/std@0.168.0/http/server.ts"

serve(async (req: Request) => {
  try {
    const { target_node_id, parameters } = await req.json();
    
    // In a real scenario, this would push a message to an IoT hub or 
    // update a 'commands' table where the Gateway pulls from.
    // For now, we simulate broadcasting.
    console.log(`Broadcasting config to ${target_node_id}:`, parameters);

    return new Response(JSON.stringify({ 
      status: "queued", 
      message: `Configuration broadcast queued for node ${target_node_id}` 
    }), {
      headers: { "Content-Type": "application/json" },
    });
  } catch (err: any) {
    return new Response(JSON.stringify({ error: err.message }), { status: 500 });
  }
});

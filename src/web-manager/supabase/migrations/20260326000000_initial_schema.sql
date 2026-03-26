-- Table for mesh nodes
CREATE TABLE IF NOT EXISTS public.nodes (
    id TEXT PRIMARY KEY,
    role TEXT NOT NULL CHECK (role IN ('Leaf', 'Router', 'Gateway')),
    status TEXT DEFAULT 'offline',
    battery_mv INTEGER,
    rssi_dbm INTEGER,
    last_seen TIMESTAMPTZ DEFAULT NOW(),
    latitude DOUBLE PRECISION,
    longitude DOUBLE PRECISION,
    parent_id TEXT REFERENCES public.nodes(id)
);

-- Table for detection events
CREATE TABLE IF NOT EXISTS public.detections (
    id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
    node_id TEXT NOT NULL REFERENCES public.nodes(id),
    timestamp TIMESTAMPTZ DEFAULT NOW(),
    confidence DOUBLE PRECISION NOT NULL,
    audio_clip_url TEXT,
    processed BOOLEAN DEFAULT FALSE
);

-- Enable Realtime for these tables
ALTER PUBLICATION supabase_realtime ADD TABLE public.nodes;
ALTER PUBLICATION supabase_realtime ADD TABLE public.detections;

-- Basic RLS (enable anon read for demo, restricted write)
ALTER TABLE public.nodes ENABLE ROW LEVEL SECURITY;
ALTER TABLE public.detections ENABLE ROW LEVEL SECURITY;

CREATE POLICY "Allow public read" ON public.nodes FOR SELECT USING (true);
CREATE POLICY "Allow public read" ON public.detections FOR SELECT USING (true);

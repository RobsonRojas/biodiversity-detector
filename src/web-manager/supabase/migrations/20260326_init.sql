-- Enable Realtime for detections
alter publication supabase_realtime add table detections;

-- Create detections table
CREATE TABLE IF NOT EXISTS detections (
    id UUID PRIMARY KEY DEFAULT uuid_generate_v4(),
    node_id TEXT NOT NULL,
    confidence DOUBLE PRECISION NOT NULL,
    pushed_at TIMESTAMP WITH TIME ZONE DEFAULT timezone('utc'::text, now()) NOT NULL,
    trail JSONB DEFAULT '[]'::jsonb
);

-- RLS (Row Level Security) - Allow all for demo purposes
ALTER TABLE detections ENABLE ROW LEVEL SECURITY;
CREATE POLICY "Allow public read access" ON detections FOR SELECT USING (true);
CREATE POLICY "Allow service_role insert" ON detections FOR INSERT WITH CHECK (true);

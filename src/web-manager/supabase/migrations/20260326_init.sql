-- Enable Realtime for detections
alter publication supabase_realtime add table detections;

-- Create detections table
CREATE TABLE IF NOT EXISTS detections (
    id UUID PRIMARY KEY DEFAULT uuid_generate_v4(),
    node_id TEXT NOT NULL,
    confidence DOUBLE PRECISION NOT NULL,
    sound_class TEXT DEFAULT 'chainsaw',
    audio_url TEXT,
    pushed_at TIMESTAMP WITH TIME ZONE DEFAULT timezone('utc'::text, now()) NOT NULL,
    trail JSONB DEFAULT '[]'::jsonb
);

-- RLS (Row Level Security) - Allow all for demo purposes
ALTER TABLE detections ENABLE ROW LEVEL SECURITY;
CREATE POLICY "Allow public read access" ON detections FOR SELECT USING (true);

-- Create storage bucket for audio clips
INSERT INTO storage.buckets (id, name, public) 
VALUES ('detection-audio', 'detection-audio', true)
ON CONFLICT (id) DO NOTHING;

-- RLS for storage (allow public read)
CREATE POLICY "Public Read Audio" ON storage.objects FOR SELECT USING (bucket_id = 'detection-audio');
CREATE POLICY "Allow service_role insert" ON detections FOR INSERT WITH CHECK (true);

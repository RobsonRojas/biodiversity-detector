-- Align detections table with frontend and C++ requirements
ALTER TABLE public.detections RENAME COLUMN timestamp TO pushed_at;
ALTER TABLE public.detections RENAME COLUMN audio_clip_url TO audio_url;

ALTER TABLE public.detections ADD COLUMN IF NOT EXISTS sound_class TEXT DEFAULT 'chainsaw';
ALTER TABLE public.detections ADD COLUMN IF NOT EXISTS battery_mv INTEGER;
ALTER TABLE public.detections ADD COLUMN IF NOT EXISTS last_rssi INTEGER;
ALTER TABLE public.detections ADD COLUMN IF NOT EXISTS trail JSONB DEFAULT '[]'::jsonb;

-- Ensure Realtime is enabled for the updated schema
-- (Already added in previous migrations)

-- Update policies if needed
DROP POLICY IF EXISTS "Allow public read access" ON public.detections;
CREATE POLICY "Allow public read access" ON public.detections FOR SELECT USING (true);

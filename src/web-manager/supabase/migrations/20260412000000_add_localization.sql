-- Table for localized acoustic events (correlated detections)
CREATE TABLE IF NOT EXISTS public.acoustic_events (
    id UUID PRIMARY KEY DEFAULT uuid_generate_v4(),
    sound_class TEXT NOT NULL,
    avg_confidence DOUBLE PRECISION,
    latitude DOUBLE PRECISION,
    longitude DOUBLE PRECISION,
    observer_nodes TEXT[], -- Array of node IDs that detected this event
    first_detected_at TIMESTAMP WITH TIME ZONE DEFAULT timezone('utc'::text, now()) NOT NULL,
    last_detected_at TIMESTAMP WITH TIME ZONE DEFAULT timezone('utc'::text, now()) NOT NULL
);

-- Enable Realtime for acoustic_events
alter publication supabase_realtime add table acoustic_events;

-- Update nodes table to include accuracy if not present
ALTER TABLE public.nodes ADD COLUMN IF NOT EXISTS location_accuracy DOUBLE PRECISION;

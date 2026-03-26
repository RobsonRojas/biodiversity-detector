-- Table for remote node configuration
CREATE TABLE IF NOT EXISTS nodes_config (
    node_id TEXT PRIMARY KEY,
    detection_threshold DOUBLE PRECISION DEFAULT 0.85,
    heartbeat_interval_seconds INTEGER DEFAULT 30,
    status TEXT DEFAULT 'Applied', -- 'Pending', 'Applied', 'Failed'
    updated_at TIMESTAMP WITH TIME ZONE DEFAULT timezone('utc'::text, now()) NOT NULL
);

-- Table for historical telemetry (analytics)
CREATE TABLE IF NOT EXISTS telemetry_history (
    id UUID PRIMARY KEY DEFAULT uuid_generate_v4(),
    node_id TEXT NOT NULL,
    battery_mv INTEGER NOT NULL,
    rssi INTEGER NOT NULL,
    recorded_at TIMESTAMP WITH TIME ZONE DEFAULT timezone('utc'::text, now()) NOT NULL
);

-- Index for analytics performance
CREATE INDEX IF NOT EXISTS idx_telemetry_node_time ON telemetry_history (node_id, recorded_at DESC);

-- Enable Realtime
alter publication supabase_realtime add table nodes_config;
alter publication supabase_realtime add table telemetry_history;

-- Seeds for the 5 nodes
INSERT INTO nodes_config (node_id, detection_threshold, heartbeat_interval_seconds)
VALUES 
('1', 0.85, 30),
('2', 0.85, 30),
('3', 0.85, 30),
('4', 0.85, 30),
('5', 0.85, 60)
ON CONFLICT (node_id) DO NOTHING;

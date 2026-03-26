import { useEffect, useState } from 'react';
import { 
  Table, TableBody, TableCell, TableContainer, 
  TableHead, TableRow, Paper, Slider, Button, Chip, Typography, Box 
} from '@mui/material';
import { supabase } from '../lib/supabase';

interface NodeConfigParams {
  node_id: string;
  detection_threshold: number;
  heartbeat_interval_seconds: number;
  status: string;
}

const NodeConfig = () => {
    const [configs, setConfigs] = useState<NodeConfigParams[]>([]);

    useEffect(() => {
        const fetchConfigs = async () => {
            const { data } = await supabase.from('nodes_config').select('*').order('node_id');
            if (data) setConfigs(data);
        };
        fetchConfigs();

        const channel = supabase.channel('config-updates')
            .on('postgres_changes', { event: '*', schema: 'public', table: 'nodes_config' }, payload => {
                setConfigs(prev => prev.map(c => c.node_id === (payload.new as any).node_id ? (payload.new as any) : c));
            })
            .subscribe();

        return () => { supabase.removeChannel(channel); };
    }, []);

    const handleUpdate = async (nodeId: string, updates: Partial<NodeConfigParams>) => {
        await supabase.from('nodes_config').update({ ...updates, status: 'Pending' }).eq('node_id', nodeId);
    };

    return (
        <TableContainer component={Paper} sx={{ bgcolor: 'background.paper' }}>
            <Table>
                <TableHead>
                    <TableRow>
                        <TableCell>Node ID</TableCell>
                        <TableCell>Threshold</TableCell>
                        <TableCell>Interval (s)</TableCell>
                        <TableCell>Status</TableCell>
                        <TableCell>Action</TableCell>
                    </TableRow>
                </TableHead>
                <TableBody>
                    {configs.map(c => (
                        <TableRow key={c.node_id}>
                            <TableCell>Node 0x{parseInt(c.node_id).toString(16)}</TableCell>
                            <TableCell>
                                <Slider 
                                    defaultValue={c.detection_threshold} 
                                    min={0.5} max={1.0} step={0.05} size="small"
                                    valueLabelDisplay="auto"
                                    onChangeCommitted={(_, v) => handleUpdate(c.node_id, { detection_threshold: v as number })}
                                    sx={{ width: 100 }}
                                />
                            </TableCell>
                            <TableCell>
                                <Slider 
                                    defaultValue={c.heartbeat_interval_seconds} 
                                    min={10} max={300} step={10} size="small"
                                    valueLabelDisplay="auto"
                                    onChangeCommitted={(_, v) => handleUpdate(c.node_id, { heartbeat_interval_seconds: v as number })}
                                    sx={{ width: 100 }}
                                />
                            </TableCell>
                            <TableCell>
                                <Chip label={c.status} color={c.status === 'Applied' ? 'success' : 'warning'} size="small" />
                            </TableCell>
                            <TableCell>
                                <Button size="small" onClick={() => handleUpdate(c.node_id, { status: 'Applied' })}>Force Sync</Button>
                            </TableCell>
                        </TableRow>
                    ))}
                </TableBody>
            </Table>
        </TableContainer>
    );
};

export default NodeConfig;

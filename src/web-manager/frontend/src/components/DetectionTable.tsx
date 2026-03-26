import { useEffect, useState } from 'react';
import { 
  Table, TableBody, TableCell, TableContainer, 
  TableHead, TableRow, Chip, Typography, Box 
} from '@mui/material';
import { supabase } from '../lib/supabase';

interface Detection {
  id: string;
  node_id: string;
  confidence: number;
  battery_mv: number;
  last_rssi: number;
  pushed_at: string;
}

const DetectionTable = () => {
  const [detections, setDetections] = useState<Detection[]>([]);

  useEffect(() => {
    // Initial fetch
    const fetchDetections = async () => {
      const { data, error } = await supabase
        .from('detections')
        .select('*')
        .order('pushed_at', { ascending: false })
        .limit(10);
      
      if (!error && data) setDetections(data as Detection[]);
    };

    fetchDetections();

    // Real-time subscription
    const channel = supabase
      .channel('schema-db-changes')
      .on(
        'postgres_changes',
        { event: 'INSERT', schema: 'public', table: 'detections' },
        (payload) => {
          setDetections(prev => [payload.new as Detection, ...prev].slice(0, 10));
        }
      )
      .subscribe();

    return () => {
      supabase.removeChannel(channel);
    };
  }, []);

  return (
    <TableContainer component={Box} sx={{ maxHeight: 400, overflow: 'auto' }}>
      <Table stickyHeader size="small">
        <TableHead>
          <TableRow>
            <TableCell>Source</TableCell>
            <TableCell>Confidence</TableCell>
            <TableCell>Battery</TableCell>
            <TableCell>RSSI</TableCell>
            <TableCell>Time</TableCell>
          </TableRow>
        </TableHead>
        <TableBody>
          {detections.length === 0 ? (
             <TableRow>
               <TableCell colSpan={5} align="center">
                 <Typography variant="body2" sx={{ py: 2, color: 'text.secondary' }}>
                   No detections reported yet
                 </Typography>
               </TableCell>
             </TableRow>
          ) : (
            detections.map((d) => (
              <TableRow key={d.id}>
                <TableCell>Node 0x{parseInt(d.node_id).toString(16)}</TableCell>
                <TableCell>
                  <Chip 
                    label={`${(d.confidence * 100).toFixed(1)}%`}
                    color={d.confidence > 0.9 ? 'error' : 'warning'}
                    size="small"
                  />
                </TableCell>
                <TableCell>
                  <Typography variant="body2" color={d.battery_mv < 3700 ? 'error' : 'inherit'}>
                    {d.battery_mv}mV
                  </Typography>
                </TableCell>
                <TableCell>
                  <Typography variant="body2" color={d.last_rssi < -110 ? 'error' : 'inherit'}>
                    {d.last_rssi}dBm
                  </Typography>
                </TableCell>
                <TableCell>{new Date(d.pushed_at).toLocaleTimeString()}</TableCell>
              </TableRow>
            ))
          )}
        </TableBody>
      </Table>
    </TableContainer>
  );
};

export default DetectionTable;

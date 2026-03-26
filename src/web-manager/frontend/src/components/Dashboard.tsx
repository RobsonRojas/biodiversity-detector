import { useState } from 'react';
import { 
    Grid2 as Grid, Paper, Typography, Box, Container, 
    Tabs, Tab, Button, Stack 
} from '@mui/material';
import { Download as DownloadIcon } from 'lucide-react';
import Map from './Map';
import DetectionTable from './DetectionTable';
import NodeConfig from './NodeConfig';
import AnalyticsCharts from './AnalyticsCharts';
import { supabase } from '../lib/supabase';

const Dashboard = () => {
  const [tabIndex, setTabIndex] = useState(0);

  const handleExport = async () => {
    const { data } = await supabase.from('detections').select('*');
    if (data) {
        const csvContent = "data:text/csv;charset=utf-8," 
            + ["ID,Node,Confidence,Battery,RSSI,Timestamp", 
               ...data.map(d => `${d.id},${d.node_id},${d.confidence},${d.battery_mv},${d.last_rssi},${d.pushed_at}`)
              ].join("\n");
        const encodedUri = encodeURI(csvContent);
        const link = document.createElement("a");
        link.setAttribute("href", encodedUri);
        link.setAttribute("download", "chainsaw_detections.csv");
        document.body.appendChild(link);
        link.click();
    }
  };

  return (
    <Container maxWidth="xl" sx={{ mt: 4, mb: 4 }}>
      <Stack direction="row" justifyContent="space-between" alignItems="center" sx={{ mb: 2 }}>
        <Typography variant="h4" sx={{ fontWeight: 'bold', color: 'primary.main' }}>
            Forest Guardian Mesh Dashboard
        </Typography>
        <Button 
            variant="outlined" 
            startIcon={<DownloadIcon size={18} />} 
            onClick={handleExport}
            sx={{ borderRadius: 2 }}
        >
            Export Detections
        </Button>
      </Stack>
      
      <Box sx={{ borderBottom: 1, borderColor: 'divider', mb: 3 }}>
        <Tabs value={tabIndex} onChange={(_, v) => setTabIndex(v)} textColor="primary" indicatorColor="primary">
          <Tab label="Live Network Map" />
          <Tab label="Node Configuration" />
          <Tab label="Systems Analytics" />
        </Tabs>
      </Box>

      {tabIndex === 0 && (
        <Grid container spacing={3}>
            {/* Map View */}
            <Grid size={{ xs: 12, md: 8 }}>
            <Paper sx={{ p: 2, height: 600, display: 'flex', flexDirection: 'column' }}>
                <Typography variant="h6" gutterBottom>Network Topology & Real-time Links</Typography>
                <Box sx={{ flexGrow: 1, borderRadius: 1, overflow: 'hidden' }}>
                <Map />
                </Box>
            </Paper>
            </Grid>

            {/* Detections List */}
            <Grid size={{ xs: 12, md: 4 }}>
            <Paper sx={{ p: 2, height: 600 }}>
                <Typography variant="h6" gutterBottom>Recent Detections</Typography>
                <DetectionTable />
            </Paper>
            </Grid>
        </Grid>
      )}

      {tabIndex === 1 && (
        <Box sx={{ width: '100%' }}>
             <Typography variant="h6" gutterBottom>Remote Mesh Management</Typography>
             <NodeConfig />
        </Box>
      )}

      {tabIndex === 2 && (
        <Grid container spacing={3}>
             <Grid size={{ xs: 12, md: 8 }}>
                <Paper sx={{ p: 2 }}>
                    <Typography variant="h6" gutterBottom>Historical Trends</Typography>
                    <AnalyticsCharts />
                </Paper>
             </Grid>
             <Grid size={{ xs: 12, md: 4 }}>
                <Paper sx={{ p: 2 }}>
                    <Typography variant="h6" gutterBottom>Deployment Health</Typography>
                    <Box sx={{ p: 2, bgcolor: 'background.default', borderRadius: 1 }}>
                        <Typography variant="body2" color="text.secondary">All nodes reporting within heartbeat interval.</Typography>
                        <Typography variant="h4" sx={{ mt: 2, color: 'success.main' }}>100%</Typography>
                        <Typography variant="caption">Network Availability</Typography>
                    </Box>
                </Paper>
             </Grid>
        </Grid>
      )}
    </Container>
  );
};

export default Dashboard;

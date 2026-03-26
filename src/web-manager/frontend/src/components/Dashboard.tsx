import Grid from '@mui/material/Grid';
import { Paper, Typography, Box, Container } from '@mui/material';
import Map from './Map';
import DetectionTable from './DetectionTable';

const Dashboard = () => {
  return (
    <Container maxWidth="xl" sx={{ mt: 4, mb: 4 }}>
      <Typography variant="h4" gutterBottom sx={{ fontWeight: 'bold', color: 'primary.main' }}>
        Forest Guardian Mesh Dashboard
      </Typography>
      
      <Grid container spacing={3}>
        {/* Map View */}
        <Grid size={{ xs: 12, md: 8 }}>
          <Paper sx={{ p: 2, height: 500, display: 'flex', flexDirection: 'column' }}>
            <Typography variant="h6" gutterBottom>
              Network Topology Map
            </Typography>
            <Box sx={{ flexGrow: 1, borderRadius: 1, overflow: 'hidden' }}>
              <Map />
            </Box>
          </Paper>
        </Grid>

        {/* Node Status Summary */}
        <Grid size={{ xs: 12, md: 4 }}>
          <Paper sx={{ p: 2, height: 500 }}>
             <Typography variant="h6" gutterBottom>
              Recent Detections
            </Typography>
            <DetectionTable />
          </Paper>
        </Grid>
      </Grid>
    </Container>
  );
};

export default Dashboard;

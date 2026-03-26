import React from 'react';
import { Box, Paper, Typography, AppBar, Toolbar } from '@mui/material';
import { MapContainer, TileLayer, Marker, Popup, Polyline } from 'react-leaflet';
import 'leaflet/dist/leaflet.css';
import Sidebar from './Sidebar';

const Dashboard: React.FC = () => {
  // Mock data for nodes
  const nodes = [
    { id: 'GW-01', type: 'Gateway', lat: -23.5505, lng: -46.6333, status: 'online', battery: 100 },
    { id: 'RT-01', type: 'Router', lat: -23.5550, lng: -46.6350, status: 'online', battery: 85 },
    { id: 'LF-01', type: 'Leaf', lat: -23.5600, lng: -46.6400, status: 'online', battery: 70 },
  ];

  const connections: [number, number][][] = [
    [[-23.5505, -46.6333], [-23.5550, -46.6350]],
    [[-23.5550, -46.6350], [-23.5600, -46.6400]],
  ];

  return (
    <Box sx={{ flexGrow: 1, height: '100vh', display: 'flex', flexDirection: 'column' }}>
      <AppBar position="static" color="primary">
        <Toolbar>
          <Typography variant="h6" component="div">
            Guardian Network Manager
          </Typography>
        </Toolbar>
      </AppBar>
      
      <Box sx={{ display: 'flex', flexGrow: 1, overflow: 'hidden' }}>
        <Sidebar nodes={nodes} />
        
        <Box component="main" sx={{ flexGrow: 1, p: 2, position: 'relative' }}>
          <Paper sx={{ height: '100%', overflow: 'hidden', borderRadius: 2 }}>
            <MapContainer center={[-23.555, -46.635]} zoom={14} style={{ height: '100%', width: '100%' }}>
              <TileLayer
                url="https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png"
                attribution='&copy; <a href="https://www.openstreetmap.org/copyright">OpenStreetMap</a> contributors'
              />
              {nodes.map(node => (
                <Marker key={node.id} position={[node.lat, node.lng]}>
                  <Popup>
                    <strong>{node.id}</strong><br />
                    Type: {node.type}<br />
                    Battery: {node.battery}%
                  </Popup>
                </Marker>
              ))}
              {connections.map((coords, index) => (
                <Polyline key={index} positions={coords} color="#2e7d32" weight={3} opacity={0.6} />
              ))}
            </MapContainer>
          </Paper>
        </Box>
      </Box>
    </Box>
  );
};

export default Dashboard;

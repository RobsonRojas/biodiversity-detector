import React from 'react';
import { Box, Paper, Typography, Grid } from '@mui/material';
import { LineChart, Line, XAxis, YAxis, CartesianGrid, Tooltip, ResponsiveContainer, BarChart, Bar } from 'recharts';

const data = [
  { name: 'Mon', detections: 4, confidence: 0.82 },
  { name: 'Tue', detections: 7, confidence: 0.88 },
  { name: 'Wed', detections: 2, confidence: 0.75 },
  { name: 'Thu', detections: 12, confidence: 0.92 },
  { name: 'Fri', detections: 6, confidence: 0.85 },
  { name: 'Sat', detections: 1, confidence: 0.70 },
  { name: 'Sun', detections: 3, confidence: 0.78 },
];

const Analytics: React.FC = () => {
  return (
    <Box sx={{ p: 3 }}>
      <Typography variant="h4" gutterBottom>Detection Analytics</Typography>
      
      <Grid container spacing={3}>
        <Grid size={{ xs: 12, md: 8 }}>
          <Paper sx={{ p: 3, borderRadius: 2, bgcolor: 'background.paper', height: 400 }}>
            <Typography variant="h6" gutterBottom>Weekly Detection Trend</Typography>
            <ResponsiveContainer width="100%" height="90%">
              <LineChart data={data}>
                <CartesianGrid strokeDasharray="3 3" stroke="#333" />
                <XAxis dataKey="name" stroke="#888" />
                <YAxis stroke="#888" />
                <Tooltip 
                  contentStyle={{ backgroundColor: '#1a1f1a', border: 'none', borderRadius: '8px' }}
                  itemStyle={{ color: '#2e7d32' }}
                />
                <Line type="monotone" dataKey="detections" stroke="#2e7d32" strokeWidth={3} dot={{ fill: '#2e7d32' }} />
              </LineChart>
            </ResponsiveContainer>
          </Paper>
        </Grid>

        <Grid size={{ xs: 12, md: 4 }}>
          <Paper sx={{ p: 3, borderRadius: 2, bgcolor: 'background.paper', height: 400 }}>
            <Typography variant="h6" gutterBottom>Avg. Confidence Level</Typography>
            <ResponsiveContainer width="100%" height="90%">
              <BarChart data={data}>
                <XAxis dataKey="name" hide />
                <YAxis hide domain={[0, 1]} />
                <Tooltip 
                  contentStyle={{ backgroundColor: '#1a1f1a', border: 'none' }}
                  labelStyle={{ display: 'none' }}
                />
                <Bar dataKey="confidence" fill="#ffa000" radius={[4, 4, 0, 0]} />
              </BarChart>
            </ResponsiveContainer>
          </Paper>
        </Grid>
      </Grid>
    </Box>
  );
};

export default Analytics;

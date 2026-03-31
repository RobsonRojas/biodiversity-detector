import { 
    LineChart, Line, XAxis, YAxis, CartesianGrid, 
    Tooltip, ResponsiveContainer 
} from 'recharts';
import { Typography, Paper, Box } from '@mui/material';

const mockData = [
  { name: '12:00', bat: 4100, rssi: -75 },
  { name: '12:10', bat: 4080, rssi: -80 },
  { name: '12:20', bat: 4050, rssi: -95 },
  { name: '12:30', bat: 4020, rssi: -115 },
  { name: '12:40', bat: 3950, rssi: -112 },
  { name: '12:50', bat: 3900, rssi: -85 },
];

const AnalyticsCharts = () => {
    return (
        <Box sx={{ display: 'flex', flexDirection: 'column', gap: 3 }}>
            <Paper sx={{ p: 2, height: 220 }}>
                <Typography variant="subtitle2" gutterBottom color="primary">Battery Voltage Trend (mV)</Typography>
                <ResponsiveContainer width="100%" height="85%">
                    <LineChart data={mockData}>
                        <CartesianGrid strokeDasharray="3 3" stroke="#333" />
                        <XAxis dataKey="name" hide />
                        <YAxis domain={[3500, 4300]} fontSize={10} />
                        <Tooltip contentStyle={{ backgroundColor: '#001e3c', border: 'none' }} />
                        <Line type="monotone" dataKey="bat" stroke="#00e676" strokeWidth={2} dot={false} />
                    </LineChart>
                </ResponsiveContainer>
            </Paper>

            <Paper sx={{ p: 2, height: 220 }}>
                <Typography variant="subtitle2" gutterBottom color="orange">Signal Quality (RSSI dBm)</Typography>
                <ResponsiveContainer width="100%" height="85%">
                    <LineChart data={mockData}>
                        <CartesianGrid strokeDasharray="3 3" stroke="#333" />
                        <XAxis dataKey="name" hide />
                        <YAxis domain={[-130, -50]} fontSize={10} />
                        <Tooltip contentStyle={{ backgroundColor: '#001e3c', border: 'none' }} />
                        <Line type="monotone" dataKey="rssi" stroke="#ff9800" strokeWidth={2} dot={false} />
                    </LineChart>
                </ResponsiveContainer>
            </Paper>
        </Box>
    );
};

export default AnalyticsCharts;

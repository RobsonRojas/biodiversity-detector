import React from 'react';
import { Box, List, ListItem, Typography, Divider, Chip, Stack } from '@mui/material';
import { Sensors as SensorsIcon, BatteryFull as BatteryIcon, Wifi as WifiIcon } from '@mui/icons-material';

interface Node {
  id: string;
  type: string;
  status: string;
  battery: number;
}

interface SidebarProps {
  nodes: Node[];
}

const Sidebar: React.FC<SidebarProps> = ({ nodes }) => {
  return (
    <Box sx={{ width: 300, borderRight: '1px solid rgba(255, 255, 255, 0.12)', bgcolor: 'background.paper' }}>
      <Box sx={{ p: 2 }}>
        <Typography variant="h6">Nodes List</Typography>
      </Box>
      <Divider />
      <List>
        {nodes.map((node) => (
          <ListItem key={node.id} sx={{ flexDirection: 'column', alignItems: 'flex-start', borderBottom: '1px solid rgba(255, 255, 255, 0.05)' }}>
            <Stack direction="row" spacing={1} alignItems="center" sx={{ width: '100%', mb: 1 }}>
              <SensorsIcon fontSize="small" color="primary" />
              <Typography variant="subtitle1" fontWeight="bold">{node.id}</Typography>
              <Chip label={node.status} size="small" color={node.status === 'online' ? 'success' : 'error'} variant="outlined" sx={{ ml: 'auto' }} />
            </Stack>
            
            <Stack direction="row" spacing={2} sx={{ color: 'text.secondary' }}>
              <Stack direction="row" spacing={0.5} alignItems="center">
                <BatteryIcon fontSize="inherit" />
                <Typography variant="caption">{node.battery}%</Typography>
              </Stack>
              <Stack direction="row" spacing={0.5} alignItems="center">
                <WifiIcon fontSize="inherit" />
                <Typography variant="caption">Good</Typography>
              </Stack>
            </Stack>
            <Typography variant="caption" sx={{ mt: 1, display: 'block', color: 'text.secondary' }}>
              Role: {node.type}
            </Typography>
          </ListItem>
        ))}
      </List>
    </Box>
  );
};

export default Sidebar;

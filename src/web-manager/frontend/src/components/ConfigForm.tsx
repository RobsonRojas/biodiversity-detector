import React, { useState } from 'react';
import { Box, Paper, Typography, Slider, TextField, Button, Stack, Switch, FormControlLabel } from '@mui/material';
import { Save as SaveIcon } from '@mui/icons-material';

interface ConfigProps {
  nodeId: string;
}

const ConfigForm: React.FC<ConfigProps> = ({ nodeId }) => {
  const [threshold, setThreshold] = useState<number>(0.75);
  const [interval, setInterval] = useState<number>(60);
  const [highSensitivity, setHighSensitivity] = useState<boolean>(false);

  const handleSave = () => {
    console.log(`Saving config for ${nodeId}:`, { threshold, interval, highSensitivity });
    // This would call the broadcast-config Edge Function
  };

  return (
    <Paper sx={{ p: 3, borderRadius: 2, bgcolor: 'background.paper', mb: 2 }}>
      <Typography variant="h6" gutterBottom>Remote Configuration: {nodeId}</Typography>
      
      <Stack spacing={3} sx={{ mt: 2 }}>
        <Box>
          <Typography gutterBottom>Detection Threshold</Typography>
          <Slider
            value={threshold}
            min={0}
            max={1}
            step={0.05}
            onChange={(_, val) => setThreshold(val as number)}
            valueLabelDisplay="auto"
            color="primary"
          />
        </Box>

        <TextField
          label="Active/Sleep Interval (s)"
          type="number"
          value={interval}
          onChange={(e) => setInterval(Number(e.target.value))}
          fullWidth
          variant="outlined"
          size="small"
        />

        <FormControlLabel
          control={
            <Switch
              checked={highSensitivity}
              onChange={(e) => setHighSensitivity(e.target.checked)}
              color="secondary"
            />
          }
          label="High Sensitivity Mode"
        />

        <Button
          variant="contained"
          startIcon={<SaveIcon />}
          onClick={handleSave}
          fullWidth
        >
          Broadcast Configuration
        </Button>
      </Stack>
    </Paper>
  );
};

export default ConfigForm;

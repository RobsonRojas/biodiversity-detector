import { BrowserRouter as Router, Routes, Route } from 'react-router-dom';
import { ThemeProvider, createTheme, CssBaseline, Box } from '@mui/material';
import Dashboard from './components/Dashboard';

const darkTheme = createTheme({
  palette: {
    mode: 'dark',
    primary: {
      main: '#00e676', // Forest green
    },
    background: {
      default: '#0a1929',
      paper: '#001e3c',
    },
  },
  typography: {
    fontFamily: '"Outfit", "Inter", sans-serif',
  },
});

function App() {
  return (
    <ThemeProvider theme={darkTheme}>
      <CssBaseline />
      <Router>
        <Box sx={{ display: 'flex', flexDirection: 'column', minHeight: '100vh' }}>
          <Routes>
            <Route path="/" element={<Dashboard />} />
            {/* Future routes like /config will go here */}
          </Routes>
        </Box>
      </Router>
    </ThemeProvider>
  );
}

export default App;

import { MapContainer, TileLayer, Marker, Popup } from 'react-leaflet';
import 'leaflet/dist/leaflet.css';
import L from 'leaflet';
import { useEffect } from 'react';
import { supabase } from '../lib/supabase';

import { useState } from 'react';

// Custom icons using Material Icons style / SVG
const getIcon = (role: string, isEvent = false) => L.divIcon({
    html: `<div style="
        background-color: ${isEvent ? '#f44336' : role === 'Gateway' ? '#2196f3' : role === 'Router' ? '#ff9800' : '#00e676'};
        width: ${isEvent ? '40px' : '30px'};
        height: ${isEvent ? '40px' : '30px'};
        border-radius: 50%;
        display: flex;
        align-items: center;
        justify-content: center;
        color: white;
        border: 2px solid white;
        box-shadow: 0 0 15px rgba(0,0,0,0.5);
        animation: ${isEvent ? 'pulse 2s infinite' : 'none'};
    "><span style="font-size: 16px; font-weight: bold;">${isEvent ? '⚠️' : role[0]}</span></div>`,
    className: 'custom-node-icon',
    iconSize: [30, 30],
    iconAnchor: [15, 15]
});

const Map = () => {
  const [nodes, setNodes] = useState<any[]>([]);
  const [events, setEvents] = useState<any[]>([]);

  const fetchData = async () => {
    const { data: nodeData } = await supabase.from('nodes').select('*');
    if (nodeData) setNodes(nodeData.map(n => ({ ...n, lat: n.latitude, lng: n.longitude, role: n.id === '5' ? 'Gateway' : 'Leaf' })));
    
    const { data: eventData } = await supabase.from('acoustic_events').select('*');
    if (eventData) setEvents(eventData);
  };

  useEffect(() => {
    fetchData();

    const nodeChannel = supabase
      .channel('nodes-realtime')
      .on('postgres_changes', { event: '*', schema: 'public', table: 'nodes' }, fetchData)
      .subscribe();

    const eventChannel = supabase
      .channel('events-realtime')
      .on('postgres_changes', { event: '*', schema: 'public', table: 'acoustic_events' }, fetchData)
      .subscribe();

    return () => {
      supabase.removeChannel(nodeChannel);
      supabase.removeChannel(eventChannel);
    };
  }, []);


  return (
    <div style={{ height: '100%', width: '100%' }}>
      <style>{`
        @keyframes pulse {
          0% { transform: scale(0.95); box-shadow: 0 0 0 0 rgba(244, 67, 54, 0.7); }
          70% { transform: scale(1); box-shadow: 0 0 0 10px rgba(244, 67, 54, 0); }
          100% { transform: scale(0.95); box-shadow: 0 0 0 0 rgba(244, 67, 54, 0); }
        }
      `}</style>
      <MapContainer center={[-23.5515, -46.6345]} zoom={17} style={{ height: '100%', width: '100%' }}>
        <TileLayer
          url="https://{s}.tile.osm.org/{z}/{x}/{y}.png"
          attribution='&copy; OSM'
        />
        
        {nodes.map(node => (
          <Marker key={node.id} position={[node.lat, node.lng]} icon={getIcon(node.role)}>
            <Popup>
              <strong>Node {node.id}</strong><br />
              Role: {node.role}<br />
              Accuracy: {node.location_accuracy?.toFixed(1)}m<br />
              Last Seen: {new Date(node.last_seen).toLocaleTimeString()}
            </Popup>
          </Marker>
        ))}

        {events.map(event => (
          <Marker key={event.id} position={[event.latitude, event.longitude]} icon={getIcon('', true)}>
            <Popup>
              <strong>⚠️ {event.sound_class.toUpperCase()} DETECTED</strong><br />
              Confidence: {(event.avg_confidence * 100).toFixed(1)}%<br />
              Time: {new Date(event.last_detected_at).toLocaleTimeString()}<br />
              Observers: {event.observer_nodes?.join(', ')}
            </Popup>
          </Marker>
        ))}
      </MapContainer>
    </div>
  );
};

export default Map;

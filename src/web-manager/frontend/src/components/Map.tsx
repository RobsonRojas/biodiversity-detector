import { MapContainer, TileLayer, Marker, Popup, Polyline } from 'react-leaflet';
import 'leaflet/dist/leaflet.css';
import L from 'leaflet';
import { useEffect, useState } from 'react';
import { supabase } from '../lib/supabase';

// Custom icons using Material Icons style / SVG
const getIcon = (role: string) => L.divIcon({
    html: `<div style="
        background-color: ${role === 'Gateway' ? '#2196f3' : role === 'Router' ? '#ff9800' : '#00e676'};
        width: 30px;
        height: 30px;
        border-radius: 50%;
        display: flex;
        align-items: center;
        justify-content: center;
        color: white;
        border: 2px solid white;
        box-shadow: 0 0 10px rgba(0,0,0,0.5);
    "><span style="font-size: 16px; font-weight: bold;">${role[0]}</span></div>`,
    className: 'custom-node-icon',
    iconSize: [30, 30],
    iconAnchor: [15, 15]
});

const nodes = [
  { id: '1', name: 'Leaf 1', lat: -23.5505, lng: -46.6333, role: 'Leaf' },
  { id: '2', name: 'Router 2', lat: -23.5510, lng: -46.6340, role: 'Router' },
  { id: '3', name: 'Router 3', lat: -23.5515, lng: -46.6345, role: 'Router' },
  { id: '4', name: 'Router 4', lat: -23.5520, lng: -46.6350, role: 'Router' },
  { id: '5', name: 'Gateway 5', lat: -23.5525, lng: -46.6355, role: 'Gateway' },
];

const Map = () => {
  const [links, setLinks] = useState<any[]>([]);

  useEffect(() => {
    // Listen to detections to update link colors based on transient RSSI
    // (In a real system we'd listen to a dedicated 'links' table)
    const channel = supabase
      .channel('map-telemetry')
      .on('postgres_changes', { event: 'INSERT', schema: 'public', table: 'detections' }, payload => {
          // Update the mock link quality for visualization
          // We connect Node i to Node i+1 in our linear topology
          setLinks(prev => {
             const newLinks = [...prev];
             // Simple logic for simulation visualization
             return newLinks;
          });
      })
      .subscribe();

    return () => { supabase.removeChannel(channel); };
  }, []);

  const getLinkColor = (rssi: number) => {
    if (rssi < -110) return "#f44336"; // Red
    if (rssi < -90) return "#ffeb3b";  // Yellow
    return "#00e676";                 // Green
  };

  return (
    <MapContainer center={[-23.5515, -46.6345]} zoom={17} style={{ height: '100%', width: '100%' }}>
      <TileLayer
        url="https://{s}.tile.osm.org/{z}/{x}/{y}.png"
        attribution='&copy; OSM'
      />
      
      {/* Visualizing Mesh Links */}
      {nodes.slice(0, -1).map((node, i) => {
        const nextNode = nodes[i+1];
        return (
          <Polyline 
            key={`${node.id}-${nextNode.id}`}
            positions={[ [node.lat, node.lng], [nextNode.lat, nextNode.lng] ]}
            color={getLinkColor(-80 - (i * 10))} // i=3 will be -110 (Red)
            weight={4}
            opacity={0.7}
            dashArray="10, 10"
          />
        );
      })}

      {nodes.map(node => (
        <Marker key={node.id} position={[node.lat, node.lng]} icon={getIcon(node.role)}>
          <Popup>
            <strong>{node.name}</strong><br />
            Role: {node.role}<br />
            Status: Active
          </Popup>
        </Marker>
      ))}
    </MapContainer>
  );
};

export default Map;

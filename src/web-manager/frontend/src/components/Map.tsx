import { MapContainer, TileLayer, Marker, Popup, Polyline } from 'react-leaflet';
import 'leaflet/dist/leaflet.css';
import L from 'leaflet';
import icon from 'leaflet/dist/images/marker-icon.png';
import iconShadow from 'leaflet/dist/images/marker-shadow.png';

let DefaultIcon = L.icon({
    iconUrl: icon,
    shadowUrl: iconShadow,
    iconSize: [25, 41],
    iconAnchor: [12, 41]
});

L.Marker.prototype.options.icon = DefaultIcon;

const nodes = [
  { id: 1, name: 'Leaf 1', lat: -23.5505, lng: -46.6333, role: 'Leaf' },
  { id: 2, name: 'Router 2', lat: -23.5510, lng: -46.6340, role: 'Router' },
  { id: 3, name: 'Router 3', lat: -23.5515, lng: -46.6345, role: 'Router' },
  { id: 4, name: 'Router 4', lat: -23.5520, lng: -46.6350, role: 'Router' },
  { id: 5, name: 'Gateway 5', lat: -23.5525, lng: -46.6355, role: 'Gateway' },
];

const Map = () => {
  const linePositions = nodes.map(n => [n.lat, n.lng] as [number, number]);

  return (
    <MapContainer center={[-23.5515, -46.6345]} zoom={17} style={{ height: '100%', width: '100%' }}>
      <TileLayer
        url="https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png"
        attribution='&copy; <a href="https://www.openstreetmap.org/copyright">OpenStreetMap</a>'
      />
      {nodes.map(node => (
        <Marker key={node.id} position={[node.lat, node.lng]}>
          <Popup>
            <strong>{node.name}</strong><br />
            Role: {node.role}<br />
            ID: 0x{node.id.toString(16)}
          </Popup>
        </Marker>
      ))}
      <Polyline positions={linePositions} color="#00e676" weight={3} dashArray="5, 10" />
    </MapContainer>
  );
};

export default Map;

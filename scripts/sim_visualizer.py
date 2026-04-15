#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
MUSA Forest Mesh Simulation Visualizer
=======================================
Generates an animated map video from per-node simulation logs.

Usage:
    python3 scripts/sim_visualizer.py [--log-dir sim_logs] [--output sim_output/musa_simulation.mp4]

Copyright (C) 2026 Robson Rojas — GPLv3
"""

import os
import re
import sys
import argparse
import math
import numpy as np
import matplotlib
matplotlib.use("Agg")  # headless backend

import matplotlib.pyplot as plt
import matplotlib.patches as mpatches
from matplotlib.animation import FuncAnimation, FFMpegWriter
from matplotlib.colors import LinearSegmentedColormap
from collections import defaultdict

# ──────────────────────────────────────────────────────────────
# Constants
# ──────────────────────────────────────────────────────────────
FPS = 30
VIDEO_W, VIDEO_H = 1920, 1080
DPI = 100

# MUSA Forest approximate bounding box (Jardim Botânico de Manaus)
MAP_LAT_MIN, MAP_LAT_MAX = -3.0120, -2.9990
MAP_LON_MIN, MAP_LON_MAX = -59.9510, -59.9380

# LoRa SX1262 theoretical range in dense jungle (meters)
LORA_RANGE_M = 500

# Color palette — dark forest theme
C_BG          = "#0a0f0d"
C_FOREST_DARK = "#0d1a14"
C_FOREST_MID  = "#122a1e"
C_FOREST_LITE = "#1a3a28"
C_CONTOUR     = "#1e4d33"
C_GRID        = "#1a3328"
C_TEXT         = "#c8e6d0"
C_TEXT_DIM     = "#5a7d64"
C_ACCENT       = "#00ff88"
C_ACCENT_DIM   = "#007744"
C_GATEWAY      = "#ffd700"
C_ROUTER       = "#00bfff"
C_LEAF         = "#ff6b6b"
C_HEARTBEAT    = "#3a7d5c"
C_RELAY        = "#00ddff"
C_ALERT_SAW    = "#ff4444"
C_ALERT_FROG   = "#44ff88"
C_HUD_BG       = "#0d1a14cc"
C_HUD_BORDER   = "#2a5d3d"

ROLE_COLORS = {"Gateway": C_GATEWAY, "Router": C_ROUTER, "Leaf": C_LEAF}
ROLE_MARKERS = {"Gateway": "^", "Router": "s", "Leaf": "o"}
ROLE_SIZES = {"Gateway": 220, "Router": 160, "Leaf": 140}

# ──────────────────────────────────────────────────────────────
# Log Parser
# ──────────────────────────────────────────────────────────────

# Regex patterns for log line parsing
RE_CONFIG = re.compile(
    r"\[SYSTEM\] Config: NodeID=(0x[0-9A-Fa-f]+), Role=(\w+), "
    r"Pos=\(([-\d.]+),\s*([-\d.]+)\)"
)
RE_HEARTBEAT = re.compile(
    r"\[SYSTEM\] Sending periodic Mesh Heartbeat "
    r"\(Bat:\s*(\d+)mV,\s*RSSI:\s*([-\d]+)dBm\)"
)
RE_MESH_RECV = re.compile(
    r"\[Mesh\] RECV: sender=(0x[0-9A-Fa-f]+) target=(0x[0-9A-Fa-f]+) "
    r"bat=(\d+)mV rssi=([-\d]+)dBm pos=\(([-\d.]+),\s*([-\d.]+)\) hops_rem=(\d+)"
)
RE_MESH_RELAY = re.compile(
    r"\[Mesh\] RELAY: src=(0x[0-9A-Fa-f]+) target=(0x[0-9A-Fa-f]+) via=(0x[0-9A-Fa-f]+)"
)
RE_GATEWAY_REACHED = re.compile(r"\[Gateway\] Final destination reached")
RE_ALERT = re.compile(r"\[ALERT\] (.+?) detected! Confidence: ([\d.]+)")
RE_LORA_SENT = re.compile(
    r"\[SimulatedLoRa\] SENT (\d+) bytes to localhost:(\d+)"
)
RE_LORA_RECV = re.compile(
    r"\[SimulatedLoRa\] RECV (\d+) bytes from"
)
RE_TELEGRAM = re.compile(
    r"\[TelegramClient\] Mock Sending Message.*?: (.+)"
)


class NodeState:
    """Accumulated state for one mesh node."""
    def __init__(self, node_id, role, lat, lon):
        self.node_id = node_id
        self.role = role
        self.lat = lat
        self.lon = lon
        self.battery = 0
        self.rssi = 0
        self.detections = defaultdict(int)


class SimEvent:
    """A single simulation event on the global timeline."""
    def __init__(self, t, kind, node_id, **data):
        self.t = t          # virtual time step
        self.kind = kind    # heartbeat | relay | alert | gateway_rx | lora_send
        self.node_id = node_id
        self.data = data


def parse_logs(log_dir):
    """Parse all nodeN.log files and return (nodes, events)."""
    nodes = {}       # node_id_int -> NodeState
    events = []      # list[SimEvent]
    port_to_id = {}  # port_int -> node_id_int  (derived from launch config)

    # Known port -> id mapping from launch_host_mesh.sh
    port_map = {5001: 1, 5002: 2, 5003: 3, 5004: 4, 5005: 5}

    log_files = sorted(
        f for f in os.listdir(log_dir) if f.startswith("node") and f.endswith(".log")
    )

    if not log_files:
        print(f"[Visualizer] ERROR: No log files found in {log_dir}")
        sys.exit(1)

    global_t = 0  # monotonic event counter (virtual time)

    for lf in log_files:
        path = os.path.join(log_dir, lf)
        # Attempt to extract node number from filename
        m = re.search(r"node(\d+)", lf)
        if not m:
            continue
        file_node_num = int(m.group(1))
        current_node_id = None

        with open(path, "r", encoding="utf-8", errors="replace") as fh:
            for line in fh:
                line = line.strip()

                # --- Config ---
                mc = RE_CONFIG.search(line)
                if mc:
                    nid = int(mc.group(1), 16)
                    role = mc.group(2)
                    lat = float(mc.group(3))
                    lon = float(mc.group(4))
                    nodes[nid] = NodeState(nid, role, lat, lon)
                    current_node_id = nid
                    continue

                if current_node_id is None:
                    current_node_id = file_node_num

                # --- Heartbeat ---
                mh = RE_HEARTBEAT.search(line)
                if mh:
                    bat = int(mh.group(1))
                    rssi = int(mh.group(2))
                    if current_node_id in nodes:
                        nodes[current_node_id].battery = bat
                        nodes[current_node_id].rssi = rssi
                    events.append(SimEvent(
                        global_t, "heartbeat", current_node_id,
                        battery=bat, rssi=rssi
                    ))
                    global_t += 1
                    continue

                # --- Alert ---
                ma = RE_ALERT.search(line)
                if ma:
                    label = ma.group(1)
                    conf = float(ma.group(2))
                    alert_type = "chainsaw" if "chainsaw" in label.lower() else "frog"
                    if current_node_id in nodes:
                        nodes[current_node_id].detections[alert_type] += 1
                    events.append(SimEvent(
                        global_t, "alert", current_node_id,
                        label=label, confidence=conf, alert_type=alert_type
                    ))
                    global_t += 1
                    continue

                # --- Mesh RECV ---
                mr = RE_MESH_RECV.search(line)
                if mr:
                    sender = int(mr.group(1), 16)
                    target = int(mr.group(2), 16)
                    bat = int(mr.group(3))
                    rssi = int(mr.group(4))
                    events.append(SimEvent(
                        global_t, "mesh_recv", current_node_id,
                        sender=sender, target=target, battery=bat, rssi=rssi
                    ))
                    global_t += 1
                    continue

                # --- Relay ---
                mrl = RE_MESH_RELAY.search(line)
                if mrl:
                    src = int(mrl.group(1), 16)
                    target = int(mrl.group(2), 16)
                    via = int(mrl.group(3), 16)
                    events.append(SimEvent(
                        global_t, "relay", current_node_id,
                        src=src, target=target, via=via
                    ))
                    global_t += 1
                    continue

                # --- Gateway ---
                mg = RE_GATEWAY_REACHED.search(line)
                if mg:
                    events.append(SimEvent(
                        global_t, "gateway_rx", current_node_id
                    ))
                    global_t += 1
                    continue

                # --- LoRa Send ---
                ms = RE_LORA_SENT.search(line)
                if ms:
                    nbytes = int(ms.group(1))
                    dest_port = int(ms.group(2))
                    dest_id = port_map.get(dest_port, 0)
                    events.append(SimEvent(
                        global_t, "lora_send", current_node_id,
                        dest=dest_id, nbytes=nbytes
                    ))
                    global_t += 1
                    continue

    # Sort events by virtual time
    events.sort(key=lambda e: e.t)
    return nodes, events


# ──────────────────────────────────────────────────────────────
# Coordinate Helpers
# ──────────────────────────────────────────────────────────────

def meters_to_deg_lat(meters):
    return meters / 111_000

def meters_to_deg_lon(meters, lat_ref):
    return meters / (111_000 * math.cos(math.radians(lat_ref)))

# ──────────────────────────────────────────────────────────────
# Forest Background Generator
# ──────────────────────────────────────────────────────────────

def generate_forest_terrain(ax, lat_range, lon_range):
    """Draw a stylized dark jungle terrain with contour lines."""
    nlat, nlon = 120, 120
    lats = np.linspace(lat_range[0], lat_range[1], nlat)
    lons = np.linspace(lon_range[0], lon_range[1], nlon)
    LON, LAT = np.meshgrid(lons, lats)

    # Procedural terrain — multiple octaves of simplex-like noise
    np.random.seed(42)
    Z = np.zeros_like(LAT)
    for freq, amp in [(3, 40), (7, 20), (13, 10), (29, 5)]:
        phase_x = np.random.uniform(0, 100)
        phase_y = np.random.uniform(0, 100)
        Z += amp * np.sin(freq * (LAT - lat_range[0]) * 800 + phase_x) * \
             np.cos(freq * (LON - lon_range[0]) * 800 + phase_y)

    # Forest color map
    forest_cmap = LinearSegmentedColormap.from_list(
        "forest", [C_FOREST_DARK, C_FOREST_MID, C_FOREST_LITE], N=256
    )

    ax.contourf(LON, LAT, Z, levels=20, cmap=forest_cmap, zorder=0)
    ax.contour(LON, LAT, Z, levels=12, colors=C_CONTOUR, linewidths=0.3,
               alpha=0.4, zorder=1)


# ──────────────────────────────────────────────────────────────
# Visualizer
# ──────────────────────────────────────────────────────────────

class MUSAVisualizer:
    """Orchestrates the full animation pipeline."""

    def __init__(self, nodes, events, output_path, fps=None):
        self.fps = fps or FPS
        self.nodes = nodes
        self.events = events
        self.output_path = output_path
        self.total_events = len(events)

        # Animation state
        self.active_packets = []   # (src_id, dst_id, progress, color, size)
        self.active_pulses = []    # (node_id, radius, alpha, color, label)
        self.event_log = []        # [(text, alpha)]  rolling log
        self.node_battery = {}     # nid -> mV
        self.node_rssi = {}        # nid -> dBm
        self.node_detections = defaultdict(lambda: defaultdict(int))
        self.gateway_rx_count = 0

        # We expand events into frames: each event -> N frames of animation
        self.FRAMES_PER_EVENT = 8
        self.PACKET_TRAVEL_FRAMES = 20
        self.PULSE_LIFETIME_FRAMES = 40
        self.total_frames = max(1, self.total_events * self.FRAMES_PER_EVENT + 120)  # +4s outro

        # Pre-compute node neighbor links for drawing mesh topology
        self.mesh_links = []  # (nid_a, nid_b)
        sorted_ids = sorted(self.nodes.keys())
        for i in range(len(sorted_ids) - 1):
            self.mesh_links.append((sorted_ids[i], sorted_ids[i + 1]))

    def _node_xy(self, nid):
        """Return (lon, lat) for plotting."""
        n = self.nodes.get(nid)
        if n:
            return (n.lon, n.lat)
        return (0, 0)

    def _init_figure(self):
        """Create the matplotlib figure and axes."""
        self.fig = plt.figure(
            figsize=(VIDEO_W / DPI, VIDEO_H / DPI), dpi=DPI,
            facecolor=C_BG
        )

        # Main map axes (left 72% of canvas)
        self.ax_map = self.fig.add_axes([0.02, 0.06, 0.68, 0.88])
        self.ax_map.set_facecolor(C_BG)
        self.ax_map.set_xlim(MAP_LON_MIN, MAP_LON_MAX)
        self.ax_map.set_ylim(MAP_LAT_MIN, MAP_LAT_MAX)
        self.ax_map.set_aspect("equal")
        self.ax_map.tick_params(colors=C_TEXT_DIM, labelsize=7)
        for spine in self.ax_map.spines.values():
            spine.set_color(C_HUD_BORDER)
            spine.set_linewidth(0.5)

        # Grid
        self.ax_map.grid(True, color=C_GRID, linewidth=0.2, alpha=0.5)

        # Forest terrain
        generate_forest_terrain(
            self.ax_map,
            (MAP_LAT_MIN, MAP_LAT_MAX),
            (MAP_LON_MIN, MAP_LON_MAX)
        )

        # HUD panel axes (right 26%)
        self.ax_hud = self.fig.add_axes([0.72, 0.06, 0.26, 0.88])
        self.ax_hud.set_facecolor(C_HUD_BG)
        self.ax_hud.set_xlim(0, 1)
        self.ax_hud.set_ylim(0, 1)
        self.ax_hud.axis("off")
        for spine in self.ax_hud.spines.values():
            spine.set_color(C_HUD_BORDER)

        # Title
        self.fig.text(
            0.36, 0.97,
            "MUSA FOREST  ·  WSN MESH SIMULATION",
            fontsize=14, color=C_ACCENT, fontweight="bold",
            ha="center", va="top",
            fontfamily="monospace"
        )
        self.fig.text(
            0.36, 0.945,
            "Jardim Botânico de Manaus  |  5-Node LoRa Mesh  |  GuardianCore",
            fontsize=8, color=C_TEXT_DIM, ha="center", va="top",
            fontfamily="monospace"
        )

        # Static elements: mesh links, LoRa circles, nodes
        self._draw_static()

    def _draw_static(self):
        """Draw static map elements: links, range circles, node markers."""
        # Mesh topology links
        for a, b in self.mesh_links:
            xa, ya = self._node_xy(a)
            xb, yb = self._node_xy(b)
            self.ax_map.plot(
                [xa, xb], [ya, yb],
                color=C_HEARTBEAT, linewidth=0.8, alpha=0.3,
                linestyle="--", zorder=2
            )

        # LoRa range circles
        lat_ref = (MAP_LAT_MIN + MAP_LAT_MAX) / 2
        r_lat = meters_to_deg_lat(LORA_RANGE_M)
        r_lon = meters_to_deg_lon(LORA_RANGE_M, lat_ref)

        for nid, ns in self.nodes.items():
            circle = mpatches.Ellipse(
                (ns.lon, ns.lat), width=2*r_lon, height=2*r_lat,
                fill=False, edgecolor=ROLE_COLORS.get(ns.role, C_TEXT_DIM),
                linewidth=0.4, alpha=0.15, linestyle=":", zorder=3
            )
            self.ax_map.add_patch(circle)

        # Node markers
        for nid, ns in self.nodes.items():
            color = ROLE_COLORS.get(ns.role, C_TEXT)
            marker = ROLE_MARKERS.get(ns.role, "o")
            size = ROLE_SIZES.get(ns.role, 120)

            # Glow
            self.ax_map.scatter(
                ns.lon, ns.lat, s=size * 3, c=color, alpha=0.08,
                marker="o", zorder=4
            )
            # Main marker
            self.ax_map.scatter(
                ns.lon, ns.lat, s=size, c=color, edgecolors="white",
                linewidths=0.6, marker=marker, zorder=5
            )
            # Label
            label = f"Node {nid}\n{ns.role}"
            self.ax_map.annotate(
                label, (ns.lon, ns.lat),
                textcoords="offset points", xytext=(12, -6),
                fontsize=6.5, color=color, fontweight="bold",
                fontfamily="monospace", zorder=6
            )

        # Legend
        legend_elements = [
            mpatches.Patch(facecolor=C_GATEWAY, label="Gateway"),
            mpatches.Patch(facecolor=C_ROUTER, label="Router"),
            mpatches.Patch(facecolor=C_LEAF, label="Leaf"),
        ]
        self.ax_map.legend(
            handles=legend_elements, loc="lower left",
            fontsize=7, facecolor=C_HUD_BG, edgecolor=C_HUD_BORDER,
            labelcolor=C_TEXT, framealpha=0.8
        )

    def _draw_hud(self, frame):
        """Redraw the HUD dashboard for the current frame."""
        self.ax_hud.clear()
        self.ax_hud.set_facecolor(C_HUD_BG)
        self.ax_hud.set_xlim(0, 1)
        self.ax_hud.set_ylim(0, 1)
        self.ax_hud.axis("off")

        y = 0.96
        dy = 0.028

        # Header
        self.ax_hud.text(0.5, y, "─── TELEMETRY DASHBOARD ───",
                         fontsize=9, color=C_ACCENT, ha="center",
                         fontfamily="monospace", fontweight="bold")
        y -= dy * 1.5

        # Progress
        progress = min(1.0, frame / max(1, self.total_frames - 120))
        bar_w = 0.8
        self.ax_hud.add_patch(plt.Rectangle(
            (0.1, y - 0.005), bar_w, 0.012,
            facecolor=C_FOREST_DARK, edgecolor=C_HUD_BORDER, linewidth=0.5
        ))
        self.ax_hud.add_patch(plt.Rectangle(
            (0.1, y - 0.005), bar_w * progress, 0.012,
            facecolor=C_ACCENT, alpha=0.6
        ))
        self.ax_hud.text(0.5, y + 0.015, f"Simulation Progress: {progress*100:.0f}%",
                         fontsize=7, color=C_TEXT_DIM, ha="center",
                         fontfamily="monospace")
        y -= dy * 2

        # Per-node status
        for nid in sorted(self.nodes.keys()):
            ns = self.nodes[nid]
            color = ROLE_COLORS.get(ns.role, C_TEXT)
            bat = self.node_battery.get(nid, ns.battery)
            rssi = self.node_rssi.get(nid, ns.rssi)

            # Node header
            self.ax_hud.text(0.05, y, f"■ Node {nid} ({ns.role})",
                             fontsize=8, color=color, fontweight="bold",
                             fontfamily="monospace")
            y -= dy

            # Battery bar
            bat_pct = min(1.0, max(0, (bat - 3000) / 1200))
            bat_color = C_ACCENT if bat_pct > 0.5 else (C_GATEWAY if bat_pct > 0.2 else C_ALERT_SAW)
            self.ax_hud.add_patch(plt.Rectangle(
                (0.08, y), 0.5, 0.01,
                facecolor=C_FOREST_DARK, edgecolor=C_HUD_BORDER, linewidth=0.3
            ))
            self.ax_hud.add_patch(plt.Rectangle(
                (0.08, y), 0.5 * bat_pct, 0.01,
                facecolor=bat_color, alpha=0.7
            ))
            self.ax_hud.text(0.62, y, f"{bat}mV",
                             fontsize=6, color=C_TEXT_DIM, fontfamily="monospace")
            y -= dy * 0.8

            # RSSI
            rssi_str = f"RSSI: {rssi}dBm"
            rssi_color = C_ACCENT if rssi > -80 else (C_GATEWAY if rssi > -100 else C_ALERT_SAW)
            self.ax_hud.text(0.08, y, rssi_str,
                             fontsize=6, color=rssi_color, fontfamily="monospace")

            # Detections
            det = self.node_detections.get(nid, {})
            det_str = ""
            if det.get("chainsaw", 0):
                det_str += f"[SAW]{det['chainsaw']} "
            if det.get("frog", 0):
                det_str += f"[FROG]{det['frog']}"
            if det_str:
                self.ax_hud.text(0.55, y, det_str,
                                 fontsize=7, color=C_TEXT, fontfamily="monospace")
            y -= dy * 1.3

        # Separator
        y -= dy * 0.3
        self.ax_hud.plot([0.05, 0.95], [y, y], color=C_HUD_BORDER, linewidth=0.5)
        y -= dy

        # Gateway reception counter
        self.ax_hud.text(0.05, y, f"[GW] Gateway RX: {self.gateway_rx_count}",
                         fontsize=8, color=C_GATEWAY, fontfamily="monospace",
                         fontweight="bold")
        y -= dy * 1.5

        # Event log (last 8 entries)
        self.ax_hud.text(0.05, y, "─── EVENT LOG ───",
                         fontsize=7, color=C_TEXT_DIM, ha="left",
                         fontfamily="monospace")
        y -= dy
        for text, alpha in self.event_log[-8:]:
            self.ax_hud.text(0.05, y, text, fontsize=5.5,
                             color=C_TEXT, alpha=max(0.3, alpha),
                             fontfamily="monospace")
            y -= dy * 0.85

        # Coordinates label
        self.ax_hud.text(0.5, 0.02,
                         f"LAT {MAP_LAT_MIN:.4f}–{MAP_LAT_MAX:.4f}\n"
                         f"LON {MAP_LON_MIN:.4f}–{MAP_LON_MAX:.4f}",
                         fontsize=5.5, color=C_TEXT_DIM, ha="center",
                         fontfamily="monospace")

    def _process_event(self, event_idx):
        """Consume an event and update animation state."""
        if event_idx >= self.total_events:
            return

        ev = self.events[event_idx]

        if ev.kind == "heartbeat":
            self.node_battery[ev.node_id] = ev.data.get("battery", 0)
            self.node_rssi[ev.node_id] = ev.data.get("rssi", 0)
            self.event_log.append((
                f"[HB] Node {ev.node_id} HB bat={ev.data.get('battery')}mV", 1.0
            ))

        elif ev.kind == "alert":
            atype = ev.data.get("alert_type", "?")
            self.node_detections[ev.node_id][atype] += 1
            color = C_ALERT_SAW if atype == "chainsaw" else C_ALERT_FROG
            emoji = "[SAW]" if atype == "chainsaw" else "[FROG]"
            label = ev.data.get("label", atype)
            self.active_pulses.append([
                ev.node_id, 0.0, 1.0, color, emoji
            ])
            self.event_log.append((
                f"{emoji} ALERT {label} node:{ev.node_id} conf:{ev.data.get('confidence',0):.2f}",
                1.0
            ))

        elif ev.kind == "lora_send":
            dest = ev.data.get("dest", 0)
            if dest and dest in self.nodes:
                self.active_packets.append([
                    ev.node_id, dest, 0.0, C_RELAY, ev.data.get("nbytes", 0)
                ])
                self.event_log.append((
                    f"-> Node {ev.node_id} -> Node {dest} ({ev.data.get('nbytes',0)}B)",
                    1.0
                ))

        elif ev.kind == "relay":
            via = ev.data.get("via", 0)
            if via and via in self.nodes:
                self.active_packets.append([
                    ev.node_id, via, 0.0, C_ROUTER, 0
                ])
                self.event_log.append((
                    f"~> Relay via Node {ev.node_id} -> {via}",
                    1.0
                ))

        elif ev.kind == "gateway_rx":
            self.gateway_rx_count += 1
            self.active_pulses.append([
                ev.node_id, 0.0, 1.0, C_GATEWAY, "[GW]"
            ])
            self.event_log.append((
                f"[GW] Gateway RX #{self.gateway_rx_count}",
                1.0
            ))

    def _draw_dynamic(self, frame):
        """Draw animated elements (packets, pulses) on the map for this frame."""
        # Remove expired dynamic artists from previous frame
        # We redraw them each frame, so just draw fresh ones

        # --- Animated packets (moving dots between nodes) ---
        new_packets = []
        for pkt in self.active_packets:
            src_id, dst_id, progress, color, nbytes = pkt
            progress += 1.0 / self.PACKET_TRAVEL_FRAMES
            if progress <= 1.0:
                sx, sy = self._node_xy(src_id)
                dx, dy = self._node_xy(dst_id)
                cx = sx + (dx - sx) * progress
                cy = sy + (dy - sy) * progress
                alpha = 1.0 - progress * 0.3

                # Packet dot
                self.ax_map.scatter(
                    cx, cy, s=50, c=color, alpha=alpha,
                    marker="D", zorder=10, edgecolors="white", linewidths=0.3
                )
                # Trail
                trail_len = 5
                for ti in range(1, trail_len + 1):
                    tp = max(0, progress - ti * 0.04)
                    tx = sx + (dx - sx) * tp
                    ty = sy + (dy - sy) * tp
                    self.ax_map.scatter(
                        tx, ty, s=15, c=color, alpha=alpha * 0.3 / ti,
                        marker=".", zorder=9
                    )

                # Highlight link
                self.ax_map.plot(
                    [sx, dx], [sy, dy], color=color, linewidth=1.2,
                    alpha=0.4, zorder=8
                )

                pkt[2] = progress
                new_packets.append(pkt)
        self.active_packets = new_packets

        # --- Detection pulses (expanding circles) ---
        new_pulses = []
        for pulse in self.active_pulses:
            nid, radius, alpha, color, emoji = pulse
            radius += 0.00008
            alpha -= 1.0 / self.PULSE_LIFETIME_FRAMES
            if alpha > 0:
                nx, ny = self._node_xy(nid)
                circle = mpatches.Circle(
                    (nx, ny), radius,
                    fill=False, edgecolor=color, linewidth=2.0,
                    alpha=alpha, zorder=11
                )
                self.ax_map.add_patch(circle)

                # Inner glow
                if alpha > 0.5:
                    self.ax_map.scatter(
                        nx, ny, s=300, c=color, alpha=alpha * 0.3,
                        marker="o", zorder=10
                    )
                    # Emoji label
                    self.ax_map.annotate(
                        emoji, (nx, ny),
                        textcoords="offset points", xytext=(0, 16),
                        fontsize=14, ha="center", zorder=12,
                        alpha=alpha
                    )

                pulse[1] = radius
                pulse[2] = alpha
                new_pulses.append(pulse)
        self.active_pulses = new_pulses

        # --- Fade event log entries ---
        for i, (text, alpha) in enumerate(self.event_log):
            self.event_log[i] = (text, max(0.2, alpha - 0.002))

    def _update_frame(self, frame):
        """Animation callback — called once per frame."""
        # Clear only dynamic map artists (keep static background)
        # We achieve this by removing zorder >= 8 artists
        artists_to_remove = [
            a for a in self.ax_map.get_children()
            if hasattr(a, "get_zorder") and a.get_zorder() >= 8
        ]
        for a in artists_to_remove:
            a.remove()

        # Process events mapped to this frame
        event_idx = frame // self.FRAMES_PER_EVENT
        sub_frame = frame % self.FRAMES_PER_EVENT
        if sub_frame == 0:
            self._process_event(event_idx)

        # Draw animated elements
        self._draw_dynamic(frame)

        # Update HUD
        self._draw_hud(frame)

        # Frame counter watermark
        self.ax_map.text(
            MAP_LON_MAX - 0.0005, MAP_LAT_MIN + 0.0005,
            f"Frame {frame}/{self.total_frames}",
            fontsize=5, color=C_TEXT_DIM, ha="right",
            fontfamily="monospace", zorder=20
        )

        return []

    def render(self):
        """Generate the full animation and save to MP4."""
        print(f"[Visualizer] Initializing figure...")
        self._init_figure()

        print(f"[Visualizer] Rendering {self.total_frames} frames "
              f"({self.total_events} events) at {self.fps} FPS...")
        print(f"[Visualizer] Output: {self.output_path}")
        print(f"[Visualizer] Resolution: {VIDEO_W}x{VIDEO_H} @ {DPI} DPI")

        os.makedirs(os.path.dirname(self.output_path) or ".", exist_ok=True)

        writer = FFMpegWriter(fps=self.fps, codec="libx264",
                              bitrate=8000,
                              extra_args=["-pix_fmt", "yuv420p"])

        anim = FuncAnimation(
            self.fig, self._update_frame,
            frames=self.total_frames,
            blit=False, repeat=False
        )

        anim.save(self.output_path, writer=writer,
                  savefig_kwargs={"facecolor": C_BG})

        plt.close(self.fig)
        print(f"[Visualizer] ✓ Video saved: {self.output_path}")

        # Print stats
        duration = self.total_frames / self.fps
        fsize = os.path.getsize(self.output_path) / (1024 * 1024)
        print(f"[Visualizer] Duration: {duration:.1f}s | Size: {fsize:.1f} MB")


# ──────────────────────────────────────────────────────────────
# CLI
# ──────────────────────────────────────────────────────────────

def main():
    parser = argparse.ArgumentParser(
        description="MUSA Forest Mesh Simulation Visualizer"
    )
    parser.add_argument(
        "--log-dir", default="sim_logs",
        help="Directory containing nodeN.log files (default: sim_logs)"
    )
    parser.add_argument(
        "--output", default="sim_output/musa_simulation.mp4",
        help="Output video path (default: sim_output/musa_simulation.mp4)"
    )
    parser.add_argument(
        "--fps", type=int, default=FPS,
        help=f"Frames per second (default: {FPS})"
    )
    args = parser.parse_args()

    target_fps = args.fps

    print("╔══════════════════════════════════════════════════╗")
    print("║  MUSA Forest — Simulation Visualizer            ║")
    print("║  Animated Map + Video Output                    ║")
    print("╚══════════════════════════════════════════════════╝")
    print()

    # Parse logs
    print(f"[Visualizer] Parsing logs from: {args.log_dir}")
    nodes, events = parse_logs(args.log_dir)
    print(f"[Visualizer] Found {len(nodes)} nodes, {len(events)} events")

    for nid in sorted(nodes.keys()):
        ns = nodes[nid]
        print(f"  Node {nid}: {ns.role} @ ({ns.lat:.4f}, {ns.lon:.4f})")

    print()

    # Render
    viz = MUSAVisualizer(nodes, events, args.output, fps=target_fps)
    viz.render()



if __name__ == "__main__":
    main()

#!/usr/bin/env python3
"""Fake bone densitometer acquisition board over TCP.

Speaks the same protocol as the Pico 2 firmware (see
bonedensity_software/app_win/src/core/device/deviceprotocol.h):

  host -> device:  A5 5A | gain u16 | frame index u16
  device -> host:  per channel 1..4:
                   AA 55 | gain u16 | frame index u16 | channel u8
                   | sample count u16 | samples u16 x count | EE EE
  All integers little-endian; samples carry 12 bits.

For every command it answers with the next recorded frame from experiment-log
files (round-*.jsonl, raw_BC/raw_BD/raw_AC/raw_AD = base64 uint16 LE), echoing
the command's gain and frame index. Frames loop forever.

The Android emulator reaches this host at 10.0.2.2, e.g.
  python3 fake_device.py ../../app_win/testdata/onset-consistency/low-1984.jsonl
  -> app connects to 10.0.2.2:5555

Only the Python standard library is used.
"""

import argparse
import base64
import json
import socket
import struct
import sys
import time
from pathlib import Path

CHANNEL_KEYS = ("raw_BC", "raw_BD", "raw_AC", "raw_AD")  # device channels 1..4
CMD_MAGIC = b"\xA5\x5A"
CMD_SIZE = 6


def load_frames(paths):
    frames = []
    for path in paths:
        for line_no, line in enumerate(Path(path).read_text(encoding="utf-8").splitlines(), 1):
            if not line.strip():
                continue
            row = json.loads(line)
            if row.get("event") != "frame" or not all(k in row for k in CHANNEL_KEYS):
                continue
            channels = []
            for key in CHANNEL_KEYS:
                raw = base64.b64decode(row[key])
                if len(raw) % 2 or not raw:
                    raise ValueError(f"{path}:{line_no}: {key} is not a uint16 array")
                count = len(raw) // 2
                if count > 5000:
                    raise ValueError(f"{path}:{line_no}: {key} has {count} samples (max 5000)")
                channels.append(raw)
            frames.append((f"{Path(path).name}#{row.get('sequence', line_no)}", channels))
    return frames


def channel_frame(gain, index, channel, raw):
    header = b"\xAA\x55" + struct.pack("<HHBH", gain, index, channel, len(raw) // 2)
    return header + raw + b"\xEE\xEE"


def acquisition(gain, index, channels):
    return b"".join(channel_frame(gain, index, ch, raw) for ch, raw in enumerate(channels, 1))


def take_commands(buffer):
    """Yields (gain, index) for complete commands; returns leftover bytes."""
    commands = []
    while True:
        start = buffer.find(CMD_MAGIC)
        if start < 0:
            # keep a trailing A5 that may start the next command
            return commands, buffer[-1:] if buffer.endswith(b"\xA5") else b""
        buffer = buffer[start:]
        if len(buffer) < CMD_SIZE:
            return commands, buffer
        gain, index = struct.unpack_from("<HH", buffer, 2)
        commands.append((gain, index))
        buffer = buffer[CMD_SIZE:]


def serve(args, frames):
    with socket.create_server((args.host, args.port), reuse_port=False) as server:
        print(f"fake device: {len(frames)} frame(s), listening on {args.host}:{args.port}", flush=True)
        while True:
            conn, peer = server.accept()
            print(f"connected: {peer[0]}:{peer[1]}", flush=True)
            sent = 0
            pending = b""
            with conn:
                conn.setsockopt(socket.IPPROTO_TCP, socket.TCP_NODELAY, 1)
                try:
                    while True:
                        data = conn.recv(4096)
                        if not data:
                            break
                        commands, pending = take_commands(pending + data)
                        for gain, index in commands:
                            name, channels = frames[sent % len(frames)]
                            if args.latency_ms:
                                time.sleep(args.latency_ms / 1000.0)
                            conn.sendall(acquisition(gain, index, channels))
                            sent += 1
                            if args.verbose:
                                print(f"  cmd gain={gain} index={index} -> {name}", flush=True)
                except (ConnectionResetError, BrokenPipeError):
                    pass
            print(f"disconnected after {sent} frame(s)", flush=True)
            if args.once:
                return


def main():
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("logs", nargs="+", help="round-*.jsonl experiment logs with raw_* channels")
    parser.add_argument("--host", default="0.0.0.0")
    parser.add_argument("--port", type=int, default=5555)
    parser.add_argument("--latency-ms", type=float, default=0.0,
                        help="delay before answering each command (simulated acquisition time)")
    parser.add_argument("--once", action="store_true", help="exit after the first client disconnects")
    parser.add_argument("-v", "--verbose", action="store_true")
    args = parser.parse_args()

    frames = load_frames(args.logs)
    if not frames:
        sys.exit("no frames with raw_BC/raw_BD/raw_AC/raw_AD found")
    try:
        serve(args, frames)
    except KeyboardInterrupt:
        pass


if __name__ == "__main__":
    main()

#!/usr/bin/env python3
"""Minimal host for fake_device.py: sends acquire commands, saves the reply bytes.

  python3 fake_device_client.py --frames 3 --out captured.bin
  replay_check <log.jsonl> --stream captured.bin   # check with the C++ FrameAssembler
"""

import argparse
import socket
import struct
import time


def main():
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("--host", default="127.0.0.1")
    parser.add_argument("--port", type=int, default=5555)
    parser.add_argument("--gain", type=int, default=1241)
    parser.add_argument("--frames", type=int, default=3)
    parser.add_argument("--interval-ms", type=float, default=80.0, help="desktop app polls every 80 ms")
    parser.add_argument("--out", default="captured.bin")
    args = parser.parse_args()

    received = bytearray()
    with socket.create_connection((args.host, args.port), timeout=5) as conn:
        conn.settimeout(0.05)
        start = time.monotonic()
        for index in range(args.frames):
            conn.sendall(b"\xA5\x5A" + struct.pack("<HH", args.gain, index))
            deadline = time.monotonic() + args.interval_ms / 1000.0
            while time.monotonic() < deadline:
                try:
                    chunk = conn.recv(65536)
                except socket.timeout:
                    continue
                if not chunk:
                    break
                received += chunk
        elapsed = time.monotonic() - start
    with open(args.out, "wb") as f:
        f.write(received)
    rate = len(received) / elapsed / 1024 if elapsed else 0
    print(f"{args.frames} command(s), {len(received)} bytes in {elapsed:.2f}s ({rate:.0f} KB/s) -> {args.out}")


if __name__ == "__main__":
    main()

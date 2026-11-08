"""ICD I1 test receiver (not a production VPN authentication server). Importing does not start any listener."""
import argparse
import json
import re
import time
from http.server import BaseHTTPRequestHandler, ThreadingHTTPServer


def validate_payload(body):
    expected = {"mac", "status", "rtsp_ok", "wg_ok", "config_version"}
    optional = {"storage_used_bytes", "firmware_version", "latency_ms"}
    return (
        isinstance(body, dict)
        and expected <= set(body) <= expected | optional
        and isinstance(body["mac"], str)
        and re.fullmatch(r"[0-9A-F]{12}", body["mac"]) is not None
        and body["status"] == "online"
        and type(body["rtsp_ok"]) is bool
        and type(body["wg_ok"]) is bool
        and type(body["config_version"]) is int
        and body["config_version"] == 0
        and all(key not in body or type(body[key]) is int
                for key in ("storage_used_bytes", "latency_ms"))
        and ("firmware_version" not in body or isinstance(body["firmware_version"], str))
    )


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--bind", default="127.0.0.1")
    parser.add_argument("--port", type=int, default=13000)
    parser.add_argument("--mode", choices=["ok", "forbidden", "slow", "chunked"], default="ok")
    parser.add_argument("--config-version", type=int, default=0)
    args = parser.parse_args()
    if args.config_version < 0:
        parser.error("--config-version must be nonnegative")

    class Handler(BaseHTTPRequestHandler):
        protocol_version = "HTTP/1.1"

        def send_error(self, code, message=None, explain=None):
            # Error strings are test fixtures; ICD does not fix heartbeat codes/text.
            payload = {"error_code": {400: "BAD_REQUEST", 404: "NOT_FOUND"}.get(code, "ERROR"),
                       "message": message or self.responses.get(code, ("Error",))[0]}
            data = json.dumps(payload, ensure_ascii=False).encode("utf-8")
            self.send_response(code)
            self.send_header("Content-Type", "application/json; charset=utf-8")
            self.send_header("Content-Length", str(len(data)))
            self.send_header("Connection", "close")
            self.end_headers()
            self.wfile.write(data)
            self.close_connection = True

        def do_POST(self):
            if self.path != "/v1/devices/heartbeat":
                self.send_error(404)
                return
            try:
                length = int(self.headers.get("Content-Length", "0"))
                if not 0 < length <= 2048:
                    raise ValueError("size")
                self.connection.settimeout(3)
                payload = json.loads(self.rfile.read(length))
                if not validate_payload(payload):
                    raise ValueError("payload schema")
            except (ValueError, OSError, UnicodeError):
                self.send_error(400)
                return
            print(f"RECEIVED monotonic={time.monotonic():.3f} {json.dumps(payload)}", flush=True)
            if args.mode == "slow":
                time.sleep(6)
            status = 403 if args.mode == "forbidden" else 200
            data = json.dumps({"error_code": "FORBIDDEN", "message": "Test rejection: unregistered VPN IP or MAC mismatch"} if status == 403 else {"config_version": args.config_version}).encode()
            try:
                self.send_response(status)
                self.send_header("Content-Type", "application/json")
                self.send_header("Connection", "close")
                if args.mode == "chunked":
                    self.send_header("Transfer-Encoding", "chunked")
                else:
                    self.send_header("Content-Length", str(len(data)))
                self.end_headers()
                if args.mode == "chunked":
                    self.wfile.write(f"{len(data):x}\r\n".encode() + data + b"\r\n0\r\n\r\n")
                else:
                    self.wfile.write(data)
            except (BrokenPipeError, ConnectionResetError):
                pass
            self.close_connection = True

    print(f"LOCAL TEST ONLY: {args.bind}:{args.port} mode={args.mode}", flush=True)
    with ThreadingHTTPServer((args.bind, args.port), Handler) as server:
        try:
            server.serve_forever()
        except KeyboardInterrupt:
            pass


if __name__ == "__main__":
    main()

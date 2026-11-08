"""Loopback-only HTTP contract test; child servers are stopped on exit."""
import http.client
import json
from pathlib import Path
import socket
import subprocess
import sys
import time


def run(mode):
    with socket.socket() as sock:
        sock.bind(("127.0.0.1", 0))
        port = sock.getsockname()[1]
    process = subprocess.Popen(
        [sys.executable, str(Path(__file__).with_name("mock_server.py")),
         "--bind", "127.0.0.1", "--port", str(port), "--mode", mode],
        stdout=subprocess.DEVNULL, stderr=subprocess.PIPE)
    try:
        for _ in range(50):
            if process.poll() is not None:
                raise RuntimeError(process.stderr.read().decode(errors="replace"))
            try:
                with socket.create_connection(("127.0.0.1", port), timeout=.1):
                    break
            except PermissionError:
                raise
            except OSError:
                time.sleep(.1)
        else:
            raise RuntimeError("Test server did not start")

        def post(body):
            connection = http.client.HTTPConnection("127.0.0.1", port, timeout=3)
            try:
                connection.request("POST", "/v1/devices/heartbeat", json.dumps(body),
                                   {"Content-Type": "application/json"})
                response = connection.getresponse()
                assert response.getheader("Content-Type").startswith("application/json")
                return response.status, json.loads(response.read())
            finally:
                connection.close()

        valid = dict(mac="7424CA5004AF", status="online", rtsp_ok=False,
                     wg_ok=True, config_version=0)
        expected = 200 if mode == "ok" else 403
        status, body = post(valid)
        assert status == expected
        if mode == "ok":
            assert body == {"config_version": 0}
        else:
            assert set(body) == {"error_code", "message"}
        for change in (dict(mac="7424ca5004af"), dict(wg_ok="true"),
                       dict(config_version=False)):
            status, body = post(dict(valid, **change))
            assert status == 400 and set(body) == {"error_code", "message"}
        missing = dict(valid)
        del missing["mac"]
        assert post(missing)[0] == 400
        assert post(dict(valid, storage_used_bytes=1, firmware_version="v1",
                         latency_ms=2))[0] == expected
        print("PASS mock HTTP", mode, "schema and JSON responses")
    finally:
        process.terminate()
        process.wait(timeout=5)
        process.stderr.close()


if __name__ == "__main__":
    run("ok")
    run("forbidden")

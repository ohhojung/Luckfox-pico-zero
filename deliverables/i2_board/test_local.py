"""Local integration checks. Never connects to a board or external server."""
import http.server, json, pathlib, subprocess, sys, tempfile, threading, time
prefix = sys.argv[1:]
assert prefix, 'Pass the heartbeat executable (optionally preceded by qemu-arm -L SYSROOT)'
requests = []
class Handler(http.server.BaseHTTPRequestHandler):
    def do_POST(self):
        requests.append((self.path, json.loads(self.rfile.read(int(self.headers['Content-Length'])))))
        body = b'{"config_version":7}'
        self.send_response(403 if len(requests)==1 else 200)
        self.send_header('Content-Length', str(len(body)))
        self.end_headers()
        self.wfile.write(body)
    def log_message(self, *args): pass
server = http.server.ThreadingHTTPServer(('127.0.0.1', 0), Handler)
threading.Thread(target=server.serve_forever, daemon=True).start()
interfaces = [p.name for p in pathlib.Path('/sys/class/net').iterdir() if p.name != 'lo']
assert interfaces, 'Need a MAC-bearing interface'
with tempfile.TemporaryDirectory() as directory:
    common = ['--host','127.0.0.1','--port',str(server.server_port),'--mac-interface',interfaces[0], '--pidfile',directory+'/pid']
    def run(args):
        result = subprocess.run(prefix + common + args, text=True, capture_output=True, timeout=20)
        assert result.returncode == 0, result.stdout + result.stderr
        return result.stdout + result.stderr
    output = run(['--dry-run','--wg-interface','lo'])
    assert 'DRY_RUN' in output and not requests, output
    output = run(['--send','--wg-interface','lo','--count','2'])
    assert len(requests)==2 and 'HTTP_403' in output and 'HTTP_200' in output, output
    for path, payload in requests:
        assert path == '/v1/devices/heartbeat', path
        assert set(payload)=={'mac','status','rtsp_ok','wg_ok','config_version'}, payload
        assert payload['status']=='online' and payload['config_version']==0
    output = run(['--send','--wg-interface','missingwg0','--count','1'])
    assert 'SEND_FAILED' in output and len(requests)==2, output
server.shutdown()
print('PASS: dry-run has no requests; HTTP 403 retries to 200; payload contract; absent VPN does not fall back')
helper = pathlib.Path(__file__).with_name('check-health.sh')
with tempfile.TemporaryDirectory() as directory:
    status = pathlib.Path(directory)/'status'
    stamp = int(float(pathlib.Path('/proc/uptime').read_text().split()[0])*1000)-20
    fields = dict(state='running',pid='123',monotonic_ms=str(stamp),main_frames='100',sub_frames='100',main_last_ms=str(stamp),sub_last_ms=str(stamp))
    def check(values, expected):
        status.write_text(''.join(f'{k}={v}\n' for k,v in values.items()))
        rc = subprocess.run(['sh',str(helper),str(status),'123']).returncode
        assert (rc==0)==expected, (values, rc)
    check(fields,True)
    check(dict(fields,sub_last_ms=str(stamp-6000)),False)
    check(dict(fields,sub_frames='0'),False)
    check(dict(fields,pid='456'),False)
    check(dict(fields,state='recovering'),False)
    check(dict(fields,monotonic_ms='bad'),False)
print('PASS: healthy dual stream; stale/empty/wrong-PID/recovering/malformed status rejected')

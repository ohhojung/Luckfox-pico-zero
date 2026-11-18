"""Status parser boundary tests, isolated files; no board access."""
import ctypes, pathlib, subprocess, tempfile
src=pathlib.Path(__file__).resolve().parents[1]/'i1_heartbeat_offline/i1_heartbeat/src'
with tempfile.TemporaryDirectory() as d:
    lib=pathlib.Path(d)/'health.so'
    subprocess.run(['cc','-std=c99','-Wall','-Wextra','-Werror','-shared','-fPIC',str(src/'frame_health.c'),'-o',str(lib)],check=True)
    check=ctypes.CDLL(str(lib)).hb_frame_health
    check.argtypes=[ctypes.c_char_p,ctypes.c_uint64,ctypes.c_uint64]
    check.restype=ctypes.c_int
    p=pathlib.Path(d)/'status'
    base=dict(state='running',pid='123',monotonic_ms='10000',main_frames='10',sub_frames='11',main_last_ms='9990',sub_last_ms='9980',attempt='1')
    def test(fields,expected,extra=''):
        p.write_text(''.join(f'{k}={v}\n' for k,v in fields.items())+extra)
        assert check(str(p).encode(),123,10010)==expected,fields
    assert check(str(p).encode(),123,10010)==0
    test(base,1)
    for key,value in [('state','recovering'),('pid','124'),('sub_frames','0'),('main_last_ms','4000'),('monotonic_ms','10011'),('sub_last_ms','10001'),('pid','+123'),('pid','18446744073709551616')]:
        test(dict(base,**{key:value}),0)
    test({k:v for k,v in base.items() if k!='main_frames'},0)
    test(base,0,'pid=123\n')
    test(base,0,'state=running\n')
print('PASS frame health: fresh, missing, stale, wrong PID, recovering, zero frames, future timestamps, malformed/overflow/duplicate fields')
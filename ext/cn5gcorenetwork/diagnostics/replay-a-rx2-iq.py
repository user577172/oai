#!/usr/bin/env python3
import socket, struct, threading, time
from pathlib import Path
import numpy as np
src=Path(__file__).with_name('a-rx2-20ms-replay.iq')
a=np.fromfile(src,dtype=np.int16)
assert len(a)==614400
samples=a.astype(np.int16).tobytes()
listener=socket.socket();listener.setsockopt(socket.SOL_SOCKET,socket.SO_REUSEADDR,1);listener.bind(('127.0.0.1',4043));listener.listen(1)
print('Listening 127.0.0.1:4043',flush=True)
sock,_=listener.accept();print('UE connected',flush=True)
def drain():
 try:
  while sock.recv(65536): pass
 except OSError: pass
threading.Thread(target=drain,daemon=True).start()
try:
 ts=0;bs=1920;length=len(samples)//4
 sock.sendall(struct.pack('<IIQII',1,1,0,0,0)+struct.pack('<f',0.)+b'\0'*4)
 while True:
  data=samples[(ts%length)*4:((ts%length)+bs)*4]
  sock.sendall(struct.pack('<IIQII',bs,1,ts+1,0,0)+struct.pack('<f',0.)+data)
  ts+=bs
  time.sleep(.001)
except (BrokenPipeError,ConnectionResetError):print('UE disconnected at',ts,flush=True)
finally:sock.close();listener.close()

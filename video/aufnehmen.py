# Clips fuer den Trailer vom runden Board holen (USB-Befehl V), Bild fuer Bild als Rohdaten (284x240 RGB565).
# Aufruf: python aufnehmen.py clips.json zielordner [nur_clipname]
# clips.json: [{"name":..., "bilder":N, "fps":30, "m":0, "z":1, "t0":..., "dt":1/30, "seite":0, "uhr0":0,
#               "auf0":-1, "blatt0":0, "blatt1":0, "anim0":0, "anim1":0, "flug0":-1, "ruhe":99999, "leucht":0}]
# Felder mit 0/1 am Ende werden ueber den Clip linear verlaufen; auf0/flug0 >= 0 laufen in ms mit der Zeit mit.
import serial, time, sys, json, os
W, H = 284, 240
clips = json.load(open(sys.argv[1], encoding='utf-8'))
ziel = sys.argv[2]
nur = sys.argv[3] if len(sys.argv) > 3 else None
s = serial.Serial(); s.port = 'COM10'; s.baudrate = 115200; s.timeout = 0.5; s.dtr = False; s.rts = False
s.open(); time.sleep(0.5); s.reset_input_buffer()
s.write(b'E'); time.sleep(0.5)  # Englisch

def bild(cmd, versuch=0):
    s.reset_input_buffer()
    s.write(cmd.encode())
    buf = b''; t0 = time.time()
    while b'FOTO\n' not in buf and time.time() - t0 < 8: buf += s.read(4096)
    if b'FOTO\n' not in buf:
        if versuch < 4: time.sleep(0.5); return bild(cmd, versuch + 1)
        raise RuntimeError('keine Antwort: ' + cmd)
    buf = buf[buf.index(b'FOTO\n') + 5:]
    while len(buf) < W * H * 2 and time.time() - t0 < 12: buf += s.read(W * H * 2 - len(buf))
    if len(buf) < W * H * 2:
        if versuch < 4: time.sleep(0.5); return bild(cmd, versuch + 1)
        raise RuntimeError('zu kurz: ' + cmd)
    return buf[:W * H * 2]

for c in clips:
    if nur and c['name'] != nur: continue
    ordner = os.path.join(ziel, c['name'])
    os.makedirs(ordner, exist_ok=True)
    n = c['bilder']; fps = c.get('fps', 30); dt = c.get('dt', 1 / fps)
    start = time.time()
    for i in range(n):
        datei = os.path.join(ordner, f'{i:05d}.bin')
        if os.path.exists(datei) and os.path.getsize(datei) == W * H * 2: continue
        u = i / max(1, n - 1)
        sek = i / fps
        t = c.get('t0', 0) + i * dt
        uhr = c.get('uhr0', 0) + sek
        auf = c.get('auf0', -1); auf = int(auf + sek * 1000) if auf >= 0 else -1
        flug = c.get('flug0', -1); flug = int(flug + sek * 1000) if flug >= 0 else -1
        blatt = c.get('blatt0', 0) + (c.get('blatt1', c.get('blatt0', 0)) - c.get('blatt0', 0)) * u
        anim = c.get('anim0', 0) + (c.get('anim1', c.get('anim0', 0)) - c.get('anim0', 0)) * u
        ruhe = c.get('ruhe', 99999); ruhe = int(ruhe + sek * 1000) if c.get('ruhe_laeuft') else ruhe
        cmd = f"V {c.get('m',0)} {c.get('z',1)} {t:.4f} {c.get('seite',0)} {uhr:.4f} {auf} {blatt:.4f} {anim:.4f} {flug} {ruhe} {c.get('leucht',0)}\n"
        open(datei, 'wb').write(bild(cmd))
    print(c['name'], n, 'Bilder', round(time.time() - start, 1), 's', flush=True)

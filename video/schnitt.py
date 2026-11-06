"""Maerchenbox-Trailer + Tutorial (englisch), 1920x1080, 30 fps, Musik musik.wav (96 BPM, Takt 2,5 s).

Bildquellen: echte Bilder vom runden Board (aufnehmen.py, 15 fps, 284x240 RGB565) und das Waveshare-Produktfoto.
Aufruf:  python schnitt.py voll  ausgabe.mp4            -> ganzes Video
         python schnitt.py probe ordner t1 t2 ...       -> einzelne Bilder (PNG) zum Ansehen
"""
import sys, os, math, glob, random
import numpy as np
from PIL import Image, ImageDraw, ImageFont, ImageFilter, ImageChops

BW, BH, FPS = 1920, 1080, 30
BAR = 2.5
DAUER = 58 * BAR + 5.0
TMP = os.path.join(os.environ['TEMP'], 'maerchen-video')
CLIPS = os.path.join(TMP, 'clips')
FOTO = r'C:\Users\heidr\Documents\Bitrix24\Elektronik\Schneiderlein-Box\esp32-s3-touch-amoled-1.75c-1.jpg'
FONTS = r'C:\Windows\Fonts'
GOLD = (255, 213, 90); GOLD_D = (214, 160, 60); WEISS = (246, 240, 228); LEISE = (190, 182, 225)
random.seed(4)

def font(name, size): return ImageFont.truetype(os.path.join(FONTS, name), size)
F_TITEL = lambda s: font('Gabriola.ttf', s)
F_KOPF = lambda s: font('GARABD.TTF', s)
F_TEXT = lambda s: font('segoeui.ttf', s)
F_HALB = lambda s: font('seguisb.ttf', s)

def klemm(v, a=0.0, b=1.0): return max(a, min(b, v))
def glatt(v): v = klemm(v); return v * v * (3 - 2 * v)
def raus(v): v = klemm(v); return 1 - (1 - v) ** 3
def fenster(t, a, b, ein=0.6, aus=0.5): return klemm((t - a) / ein) * klemm((b - t) / aus)

# ---------------- Board-Bilder ----------------
W, H, S = 284, 240, 464
_oy = np.arange(S); _wy = _oy * W // S - 22; _wx = np.arange(S) * W // S
_maske_cache = {}
def kreis_maske(d, weich=1.5):
    if d not in _maske_cache:
        k = 4
        m = Image.new('L', (d * k, d * k), 0)
        ImageDraw.Draw(m).ellipse((0, 0, d * k - 1, d * k - 1), fill=255)
        _maske_cache[d] = m.resize((d, d), Image.LANCZOS)
    return _maske_cache[d]

def roh(clip, i):
    dat = sorted(glob.glob(os.path.join(CLIPS, clip, '*.bin')))
    i = max(0, min(len(dat) - 1, i))
    b = open(dat[i], 'rb').read()
    v = np.frombuffer(b, '>u2').reshape(H, W).astype(np.uint32)
    rgb = np.stack([((v >> 11) & 31) << 3, ((v >> 5) & 63) << 2, (v & 31) << 3], -1).astype(np.uint8)
    return rgb

def randzeile(rows):
    m = rows.min(axis=0)  # (W,3)
    out = m.copy()
    for d in range(1, 5):
        out = np.minimum(out, np.roll(m, d, axis=0)); out = np.minimum(out, np.roll(m, -d, axis=0))
    return out

_rund_cache = {}
def rund(clip, i):
    """Bild i des Clips als runde Display-Ansicht (464 x 464 RGB), wie lcd_rund.cpp es zeichnet."""
    key = (clip, i)
    if key in _rund_cache: return _rund_cache[key]
    w = roh(clip, i)
    oben, unten = randzeile(w[0:3]), randzeile(w[H - 3:H])
    bild = np.empty((S, S, 3), np.uint8)
    for oy in range(S):
        wy = _wy[oy]
        zeile = oben if wy < 0 else unten if wy >= H else w[wy]
        bild[oy] = zeile[_wx]
    im = Image.fromarray(bild)
    if len(_rund_cache) > 60: _rund_cache.clear()
    _rund_cache[key] = im
    return im

def clipbild(clip, t, schleife=False):
    n = len(glob.glob(os.path.join(CLIPS, clip, '*.bin')))
    i = int(t * 15)
    if schleife: i %= max(1, n)
    return rund(clip, max(0, min(n - 1, i)))

def kreisbild(im, d):
    r = im.resize((d, d), Image.LANCZOS).convert('RGBA')
    r.putalpha(kreis_maske(d))
    return r

# ---------------- Geraet (Produktfoto ohne Hintergrund) ----------------
def geraet_vorbereiten():
    f = Image.open(FOTO).convert('RGB')
    a = np.asarray(f).min(axis=2).astype(np.float32)
    nichtweiss = np.clip((246 - a) / 21, 0, 1)
    gr = Image.fromarray((nichtweiss * 255).astype(np.uint8)).filter(ImageFilter.GaussianBlur(0.6))
    k = Image.new('L', (3200, 3200), 0); ImageDraw.Draw(k).ellipse((4 * 169, 4 * 168, 4 * 631, 4 * 630), fill=255)
    alpha = ImageChops.lighter(gr, k.resize((800, 800), Image.LANCZOS))
    g = f.convert('RGBA'); g.putalpha(alpha)
    return g.crop((150, 149, 650, 649))  # 500 x 500, Display-Mitte bei (250, 250), Radius 193
GERAET = geraet_vorbereiten()
G_R = 193

def geraet(schirm, groesse, glanz=True):
    """Geraet in 'groesse' Pixel Breite, auf dem Display das Bild 'schirm' (PIL RGB, quadratisch)."""
    g = GERAET.copy()
    d = 2 * G_R
    g.alpha_composite(kreisbild(schirm, d), (250 - G_R, 250 - G_R))
    if glanz:
        gl = Image.new('L', (500, 500), 0)
        ImageDraw.Draw(gl).ellipse((250 - G_R * 0.95, 250 - G_R * 1.05, 250 + G_R * 0.55, 250 - G_R * 0.15), fill=26)
        gl = gl.filter(ImageFilter.GaussianBlur(G_R * 0.22))
        weiss = Image.new('RGBA', (500, 500), (255, 255, 255, 0)); weiss.putalpha(gl)
        g.alpha_composite(weiss)
    return g.resize((groesse, groesse), Image.LANCZOS)

def schatten(img, r=30, deckung=150, versatz=(0, 24)):
    a = img.getchannel('A').filter(ImageFilter.GaussianBlur(r)).point(lambda v: v * deckung // 255)
    s = Image.new('RGBA', img.size, (0, 0, 0, 0)); s.putalpha(a)
    return s, versatz

# ---------------- Hintergrund ----------------
def hintergrund_vorbereiten():
    y = np.linspace(0, 1, BH)[:, None]
    oben = np.array([10, 9, 32]); unten = np.array([38, 26, 78])
    bild = oben + (unten - oben) * y[..., None]
    bild = np.repeat(bild, BW, axis=1)
    xx, yy = np.meshgrid(np.arange(BW), np.arange(BH))
    for cx, cy, r, f, st in [(400, 200, 700, (70, 50, 140), 0.45), (1600, 900, 800, (120, 70, 60), 0.35), (960, 1150, 700, (200, 150, 60), 0.22)]:
        d = np.sqrt((xx - cx) ** 2 + (yy - cy) ** 2) / r
        k = np.clip(1 - d, 0, 1) ** 2 * st
        bild = bild + (np.array(f) - bild) * k[..., None]
    v = np.sqrt(((xx - BW / 2) / BW) ** 2 + ((yy - BH / 2) / BH) ** 2)
    bild *= np.clip(1.15 - v * 0.7, 0, 1)[..., None]
    return Image.fromarray(np.clip(bild, 0, 255).astype(np.uint8)).convert('RGBA')
HG = hintergrund_vorbereiten()

def stern_sprite(r, farbe=(255, 250, 230)):
    d = int(r * 8) + 3
    im = Image.new('RGBA', (d, d), (0, 0, 0, 0)); dr = ImageDraw.Draw(im); c = d / 2
    for k in range(6, 0, -1):
        rr = r * k / 2
        dr.ellipse((c - rr, c - rr, c + rr, c + rr), fill=farbe + (int(255 * (1 / k) ** 1.6),))
    dr.line((c - r * 3.5, c, c + r * 3.5, c), fill=farbe + (90,), width=1)
    dr.line((c, c - r * 3.5, c, c + r * 3.5), fill=farbe + (90,), width=1)
    return im.filter(ImageFilter.GaussianBlur(0.5))
STERNE = [(random.uniform(0, BW), random.uniform(0, BH * 0.95), random.choice([0.6, 0.8, 1.0, 1.4, 2.0]),
           random.uniform(0, 6.28), random.uniform(0.6, 2.2)) for _ in range(240)]
SPRITES = {r: stern_sprite(r) for r in (0.6, 0.8, 1.0, 1.4, 2.0)}
GOLDSPRITES = {r: stern_sprite(r, (255, 215, 120)) for r in (1.0, 1.6, 2.4)}

def mit_alpha(img, a):
    if a >= 0.999: return img
    im = img.copy(); im.putalpha(im.getchannel('A').point(lambda v: int(v * a)))
    return im

def himmel(t, hell=1.0):
    bild = HG.copy()
    for x, y, r, ph, sp in STERNE:
        a = (0.55 + 0.45 * math.sin(t * sp + ph)) * hell
        s = SPRITES[r]
        bild.alpha_composite(mit_alpha(s, a), (int(x - s.width / 2), int(y - s.height / 2)))
    return bild

def feenstaub(bild, t, t0, dauer, cx, cy, breite=500, menge=40, seed=1):
    if not (t0 <= t <= t0 + dauer + 1.5): return
    rnd = random.Random(seed)
    for i in range(menge):
        st = t0 + rnd.random() * dauer
        u = (t - st) / 1.4
        if not (0 <= u <= 1): continue
        x = cx + rnd.uniform(-breite, breite) * (0.3 + u * 0.7)
        y = cy + rnd.uniform(-breite * 0.5, breite * 0.4) - u * 120
        s = GOLDSPRITES[rnd.choice([1.0, 1.6, 2.4])]
        bild.alpha_composite(mit_alpha(s, math.sin(u * math.pi)), (int(x), int(y)))

# ---------------- Text ----------------
_text_cache = {}
RAND = set()  # Texte, die dem Bildrand zu nah kommen (Pruefung)
def textbild(text, f, farbe=WEISS, leucht=None, schatten_=True):
    key = (text, f.path, f.size, farbe, leucht, schatten_)
    if key in _text_cache: return _text_cache[key]
    links, oben, rechts, unten = f.getbbox(text)
    pad = 40
    im = Image.new('RGBA', (rechts - links + 2 * pad, unten - oben + 2 * pad), (0, 0, 0, 0))
    if schatten_:
        sh = Image.new('RGBA', im.size, (0, 0, 0, 0))
        ImageDraw.Draw(sh).text((pad - links, pad - oben + 4), text, font=f, fill=(0, 0, 0, 160))
        im.alpha_composite(sh.filter(ImageFilter.GaussianBlur(6)))
    if leucht:
        gl = Image.new('RGBA', im.size, (0, 0, 0, 0))
        ImageDraw.Draw(gl).text((pad - links, pad - oben), text, font=f, fill=leucht + (200,))
        im.alpha_composite(gl.filter(ImageFilter.GaussianBlur(14)))
    ImageDraw.Draw(im).text((pad - links, pad - oben), text, font=f, fill=farbe + (255,))
    _text_cache[key] = (im, pad)
    return im, pad

def schreibe(bild, text, f, x, y, farbe=WEISS, a=1.0, ausr='l', leucht=None, dy=0.0):
    """x, y = linke (oder mittige/rechte) obere Kante des Textes."""
    if a <= 0.01: return
    im, pad = textbild(text, f, farbe, leucht)
    w = im.width - 2 * pad
    if ausr == 'm': x -= w / 2
    elif ausr == 'r': x -= w
    if a > 0.3 and (x + w > BW - 60 or x < 60): RAND.add((text, int(x), int(x + w)))
    bild.alpha_composite(mit_alpha(im, a), (int(x - pad), int(y - pad + dy)))

def kapitel(bild, t, nr, titel, t0, t1):
    """Kapitelkopf oben links: STEP n + Titel + Goldlinie."""
    a = fenster(t, t0, t1, 0.5, 0.4)
    if a <= 0: return
    dx = (1 - raus((t - t0) / 0.6)) * -40
    schreibe(bild, nr if isinstance(nr, str) else f'STEP {nr}', F_HALB(30), 120 + dx, 92, GOLD, a)
    schreibe(bild, titel, F_KOPF(78), 116 + dx, 128, WEISS, a)
    d = ImageDraw.Draw(bild)
    lw = int(220 * raus((t - t0 - 0.2) / 0.8))
    if lw > 0: d.rounded_rectangle((120, 232, 120 + lw, 236), 2, fill=GOLD + (int(255 * a),))

def zeile(bild, t, text, t0, t1, y, x=1040, groesse=40, farbe=WEISS, f=None, ausr='l'):
    a = fenster(t, t0, t1, 0.5, 0.4)
    dy = (1 - raus((t - t0) / 0.6)) * 18
    schreibe(bild, text, f or F_TEXT(groesse), x, y, farbe, a, ausr=ausr, dy=dy)

def punkt(bild, x, y, nr, a, farbe=GOLD):
    d = ImageDraw.Draw(bild)
    d.ellipse((x - 26, y - 26, x + 26, y + 26), fill=(40, 30, 90, int(230 * a)), outline=farbe + (int(255 * a),), width=3)
    schreibe(bild, str(nr), F_HALB(30), x, y - 22, farbe, a, ausr='m')

def setze_geraet(bild, schirm, cx, cy, groesse, a=1.0):
    g = geraet(schirm, groesse)
    sh, (sx, sy) = schatten(g, r=int(groesse * 0.05), deckung=170, versatz=(0, int(groesse * 0.04)))
    bild.alpha_composite(mit_alpha(sh, a), (int(cx - groesse / 2 + sx), int(cy - groesse / 2 + sy)))
    bild.alpha_composite(mit_alpha(g, a), (int(cx - groesse / 2), int(cy - groesse / 2)))
    return (cx - groesse / 2, cy - groesse / 2, groesse / 500)  # Ursprung und Massstab (500er Geraet)

def tipp(bild, ox, oy, sk, sx, sy, u):
    """Fingertipp-Welle an Schirmpunkt (sx, sy in 0..1 des Displays) - u = 0..1."""
    if not (0 <= u <= 1): return
    d = ImageDraw.Draw(bild)
    px = ox + (250 - G_R + sx * 2 * G_R) * sk; py = oy + (250 - G_R + sy * 2 * G_R) * sk
    for k in range(2):
        uu = klemm(u * 1.3 - k * 0.25)
        r = 20 + uu * 70
        d.ellipse((px - r, py - r, px + r, py + r), outline=(255, 255, 255, int(220 * (1 - uu))), width=5)
    r = 22 * (1 - u * 0.5)
    d.ellipse((px - r, py - r, px + r, py + r), fill=(255, 255, 255, int(120 * (1 - u))))

# ---------------- Szenen ----------------
TEASER = [('t_schneider', 'The Brave Little Tailor', 'Brothers Grimm'),
          ('t_rotkaeppchen', 'Little Red Riding Hood', 'Brothers Grimm'),
          ('t_bremen', 'The Town Musicians of Bremen', 'Brothers Grimm'),
          ('t_rapunzel', 'Rapunzel', 'Brothers Grimm'),
          ('t_sterntaler', 'The Star Coins', 'Brothers Grimm'),
          ('t_maxmoritz', 'Max and Moritz', 'Wilhelm Busch'),
          ('t_schweinchen', 'The Three Little Pigs', 'English folk tale'),
          ('t_cinderella', 'Cinderella', 'Charles Perrault')]
B = lambda n: n * BAR

def browser(bild, t, x0, y0, w, h):
    """Nachgebautes Browserfenster: daveslab.de/basteln -> Erase & install -> Port -> Installieren."""
    d = ImageDraw.Draw(bild)
    d.rounded_rectangle((x0, y0, x0 + w, y0 + h), 22, fill=(22, 20, 44, 250), outline=(80, 72, 140, 255), width=2)
    d.rounded_rectangle((x0, y0, x0 + w, y0 + 64), 22, fill=(38, 34, 72, 255))
    d.rectangle((x0, y0 + 40, x0 + w, y0 + 64), fill=(38, 34, 72, 255))
    for i, c in enumerate([(255, 95, 86), (255, 189, 46), (39, 201, 63)]):
        d.ellipse((x0 + 26 + i * 30, y0 + 22, x0 + 44 + i * 30, y0 + 40), fill=c + (255,))
    d.rounded_rectangle((x0 + 140, y0 + 14, x0 + w - 40, y0 + 50), 18, fill=(18, 16, 36, 255))
    schreibe(bild, 'daveslab.de/basteln/#maerchen', F_TEXT(24), x0 + 166, y0 + 16, LEISE, 1, leucht=None)
    cx = x0 + w / 2
    schreibe(bild, 'Install the fairy tale box', F_KOPF(46), cx, y0 + 100, WEISS, 1, ausr='m')
    # zwei Knoepfe
    bx1, bx2, by, bw, bh = x0 + 70, x0 + w / 2 + 20, y0 + 200, w / 2 - 90, 74
    d.rounded_rectangle((bx1, by, bx1 + bw, by + bh), 18, fill=(30, 70, 120, 255))
    schreibe(bild, 'Install update', F_HALB(30), bx1 + bw / 2, by + 16, WEISS, 1, ausr='m')
    druck = 1 - 0.25 * fenster(t, 3.2, 3.6, 0.1, 0.2)
    d.rounded_rectangle((bx2, by, bx2 + bw, by + bh), 18, fill=(int(200 * druck), int(60 * druck), int(100 * druck), 255))
    schreibe(bild, 'Erase & install', F_HALB(30), bx2 + bw / 2, by + 16, WEISS, 1, ausr='m')
    schreibe(bild, 'For new boards: erases everything and installs fresh.', F_TEXT(22), bx2, by + bh + 14, LEISE, 1)
    # Mauszeiger
    zx, zy = bx2 + bw * 0.6, by + bh * 0.55
    if t < 3.4:
        u = glatt((t - 1.2) / 2.0)
        mx, my = x0 + w * 0.2 + (zx - x0 - w * 0.2) * u, y0 + h * 0.85 + (zy - y0 - h * 0.85) * u
    elif t < 6.2:
        u = glatt((t - 4.6) / 1.4)
        mx, my = zx + (cx + 140 - zx) * u, zy + (y0 + 455 - zy) * u
    else:
        mx, my = cx + 140, y0 + 455
    # Dialog: Port waehlen
    if 4.0 <= t < 7.0:
        a = fenster(t, 4.0, 7.0, 0.3, 0.3)
        dx0, dy0, dw, dh = cx - 330, y0 + 150, 660, 360
        d.rounded_rectangle((dx0, dy0, dx0 + dw, dy0 + dh), 16, fill=(250, 250, 252, int(255 * a)))
        schreibe(bild, 'daveslab.de wants to connect to a serial port', F_HALB(24), dx0 + 30, dy0 + 26, (30, 30, 40), a)
        sel = t > 5.4
        d.rounded_rectangle((dx0 + 26, dy0 + 90, dx0 + dw - 26, dy0 + 150), 10,
                            fill=(210, 228, 255, int(255 * a)) if sel else (240, 240, 244, int(255 * a)))
        schreibe(bild, 'USB JTAG/serial debug unit (COM10)', F_TEXT(26), dx0 + 46, dy0 + 100, (20, 20, 30), a)
        d.rounded_rectangle((dx0 + dw - 210, dy0 + dh - 76, dx0 + dw - 30, dy0 + dh - 26), 25,
                            fill=(26, 115, 232, int(255 * a)))
        schreibe(bild, 'Connect', F_HALB(26), dx0 + dw - 120, dy0 + dh - 70, WEISS, a, ausr='m')
    # Fortschritt
    if t >= 7.0:
        a = klemm((t - 7.0) / 0.3)
        dx0, dy0, dw, dh = cx - 330, y0 + 150, 660, 300
        d.rounded_rectangle((dx0, dy0, dx0 + dw, dy0 + dh), 16, fill=(250, 250, 252, int(255 * a)))
        p = klemm((t - 7.4) / 5.0)
        if p < 1:
            schreibe(bild, 'Fairy Tale Box (fresh install)', F_HALB(28), cx, dy0 + 34, (30, 30, 40), a, ausr='m')
            schreibe(bild, f'Installing ... {int(p * 100)} %', F_TEXT(26), cx, dy0 + 110, (60, 60, 80), a, ausr='m')
            d.rounded_rectangle((dx0 + 60, dy0 + 180, dx0 + dw - 60, dy0 + 200), 10, fill=(220, 222, 230, int(255 * a)))
            d.rounded_rectangle((dx0 + 60, dy0 + 180, dx0 + 60 + (dw - 120) * max(0.03, p), dy0 + 200), 10, fill=(26, 115, 232, int(255 * a)))
        else:
            d.ellipse((cx - 40, dy0 + 40, cx + 40, dy0 + 120), fill=(39, 174, 96, 255))
            d.line((cx - 18, dy0 + 82, cx - 4, dy0 + 96, cx + 22, dy0 + 64), fill=(255, 255, 255, 255), width=8)
            schreibe(bild, 'Installation complete!', F_HALB(32), cx, dy0 + 150, (30, 30, 40), 1, ausr='m')
            schreibe(bild, 'The box restarts and opens the fairy tale book.', F_TEXT(24), cx, dy0 + 205, (80, 80, 100), 1, ausr='m')
    # Zeiger zeichnen
    if t < 7.0:
        pts = [(mx, my), (mx, my + 34), (mx + 9, my + 26), (mx + 16, my + 40), (mx + 22, my + 37), (mx + 15, my + 23), (mx + 27, my + 23)]
        d.polygon(pts, fill=(255, 255, 255, 255), outline=(0, 0, 0, 255))
        for tk in (3.3, 6.0):
            u = (t - tk) / 0.5
            if 0 <= u <= 1:
                r = 10 + u * 30
                d.ellipse((mx - r, my - r, mx + r, my + r), outline=(255, 255, 255, int(200 * (1 - u))), width=3)

def handy(bild, t, x0, y0):
    """Nachgebautes Handy: WLAN-Liste mit Maerchenbox-Setup, dann die Einrichtungsseite."""
    d = ImageDraw.Draw(bild)
    w, h = 330, 660
    d.rounded_rectangle((x0, y0, x0 + w, y0 + h), 46, fill=(12, 12, 16, 255), outline=(90, 90, 100, 255), width=3)
    d.rounded_rectangle((x0 + 14, y0 + 14, x0 + w - 14, y0 + h - 14), 36, fill=(245, 245, 248, 255))
    d.rounded_rectangle((x0 + w / 2 - 50, y0 + 24, x0 + w / 2 + 50, y0 + 44), 10, fill=(12, 12, 16, 255))
    if t < 4.5:
        schreibe(bild, 'Wi-Fi', F_HALB(34), x0 + 40, y0 + 70, (20, 20, 30), 1, leucht=None)
        netze = ['Märchenbox-Setup', 'Home-WiFi', 'Neighbour 5G', 'FRITZ!Box 7590']
        for i, n in enumerate(netze):
            y = y0 + 150 + i * 70
            if i == 0 and t > 1.5:
                d.rounded_rectangle((x0 + 26, y - 8, x0 + w - 26, y + 52), 14, fill=(214, 230, 255, 255))
            schreibe(bild, n, F_TEXT(26) if i else F_HALB(26), x0 + 46, y, (20, 20, 30), 1)
            for k in range(3):
                d.arc((x0 + w - 74 + k * 0, y + 4 - k * 6, x0 + w - 44 + k * 0, y + 34 + k * 6), 220, 320, fill=(60, 60, 80, 255), width=2)
        if 1.5 < t < 2.4:
            tipp_x, tipp_y = x0 + 160, y0 + 170
            u = (t - 1.6) / 0.6; r = 15 + 40 * u
            d.ellipse((tipp_x - r, tipp_y - r, tipp_x + r, tipp_y + r), outline=(26, 115, 232, int(200 * (1 - klemm(u)))), width=4)
    else:
        d.rounded_rectangle((x0 + 14, y0 + 14, x0 + w - 14, y0 + 130), 36, fill=(43, 42, 107, 255))
        d.rectangle((x0 + 14, y0 + 90, x0 + w - 14, y0 + 130), fill=(43, 42, 107, 255))
        schreibe(bild, 'Märchenbox', F_HALB(32), x0 + 40, y0 + 64, GOLD, 1)
        schreibe(bild, 'Which Wi-Fi should', F_TEXT(24), x0 + 40, y0 + 160, (30, 30, 40), 1)
        schreibe(bild, 'the box connect to?', F_TEXT(24), x0 + 40, y0 + 192, (30, 30, 40), 1)
        for i, (lab, val) in enumerate([('Wi-Fi', 'Home-WiFi'), ('Password', '••••••••')]):
            y = y0 + 260 + i * 110
            schreibe(bild, lab, F_HALB(22), x0 + 40, y, (80, 80, 100), 1)
            d.rounded_rectangle((x0 + 36, y + 34, x0 + w - 36, y + 84), 12, fill=(230, 230, 236, 255))
            schreibe(bild, val, F_TEXT(24), x0 + 54, y + 42, (20, 20, 30), 1)
        d.rounded_rectangle((x0 + 36, y0 + 510, x0 + w - 36, y0 + 570), 14, fill=(255, 213, 74, 255))
        schreibe(bild, 'Save', F_HALB(28), x0 + w / 2, y0 + 518, (43, 42, 107), 1, ausr='m')

def zeichne(t):
    bild = himmel(t, hell=1.0 if t < DAUER - 2 else klemm((DAUER - t) / 2))

    # ---- 0-5 s: Intro ----
    if t < B(2) + 0.6:
        schreibe(bild, 'DavesLab presents', F_HALB(40), BW / 2, 480, LEISE, fenster(t, 0.8, B(2) - 0.2, 1.0, 0.8), ausr='m')
        feenstaub(bild, t, 0.3, 4.0, BW / 2 - 40, 520, 420, 60, seed=2)

    # ---- 5-10 s: Geraet erscheint, Buch klappt auf, Titel ----
    if B(2) <= t < B(4):
        u = t - B(2)
        a = klemm(u / 0.8) * klemm((B(4) - t) / 0.45)
        gr = int(560 + 40 * raus(u / 2.5))
        setze_geraet(bild, clipbild('buch_auf', u + 0.6), BW / 2, 430, gr, a)
        schreibe(bild, 'Fairy Tale Box', F_TITEL(170), BW / 2, 720, GOLD, fenster(t, B(2) + 1.0, B(4), 0.9, 0.45), ausr='m', leucht=(255, 170, 60))
        schreibe(bild, '30 fairy tales  ·  drawn live  ·  set to music', F_TEXT(40), BW / 2, 905, WEISS,
                 fenster(t, B(2) + 1.8, B(4), 0.8, 0.45), ausr='m')
        feenstaub(bild, t, B(2) + 0.8, 2.0, BW / 2, 760, 520, 70, seed=3)

    # ---- 10-30 s: Teaser ----
    if B(4) <= t < B(12) + 0.4:
        k = min(7, int((t - B(4)) / BAR))
        for j in (k - 1, k):
            if j < 0: continue
            st = B(4) + j * BAR
            ein = klemm((t - st) / 0.35) if j == 0 else klemm((t - st + 0.35) / 0.35)
            a = ein * klemm((st + BAR + 0.35 - t) / 0.35) if j == k else klemm((st + BAR + 0.35 - t) / 0.35)
            if t >= B(12): a *= klemm((B(12) + 0.4 - t) / 0.4)
            if a <= 0: continue
            clip, name, autor = TEASER[j]
            u = t - st
            d = int(860 * (1.0 + 0.04 * u / BAR))
            kb = kreisbild(clipbild(clip, u + 0.3), d)
            # goldener Ring
            ring = Image.new('RGBA', (d + 40, d + 40), (0, 0, 0, 0))
            ImageDraw.Draw(ring).ellipse((6, 6, d + 34, d + 34), outline=GOLD + (200,), width=5)
            ring = ring.filter(ImageFilter.GaussianBlur(2))
            cx, cy = 700, 540
            bild.alpha_composite(mit_alpha(ring, a), (int(cx - d / 2 - 20), int(cy - d / 2 - 20)))
            bild.alpha_composite(mit_alpha(kb, a), (int(cx - d / 2), int(cy - d / 2)))
            ta = fenster(t, st + 0.2, st + BAR - 0.05, 0.5, 0.3) * (klemm((B(12) + 0.4 - t) / 0.4) if t >= B(12) else 1)
            gr = 64  # so gross wie moeglich, aber mit Rand rechts (bis x = 1840)
            while gr > 36 and F_KOPF(gr).getlength(name) > 1840 - 1210: gr -= 2
            schreibe(bild, name, F_KOPF(gr), 1210, 470 + (64 - gr) * 0.6, WEISS, ta, dy=(1 - raus((t - st - 0.2) / 0.6)) * 20)
            schreibe(bild, autor, F_TEXT(34), 1214, 560, GOLD, ta)
        schreibe(bild, f'{min(8, k + 1)} / 30 fairy tales', F_HALB(26), 1214, 640, LEISE, fenster(t, B(4) + 0.3, B(12) + 0.3, 0.6, 0.4))

    # ---- 30-35 s: Merkmale ----
    if B(12) <= t < B(14) + 0.3:
        u = t - B(12)
        a = klemm(u / 0.5) * klemm((B(14) + 0.3 - t) / 0.4)
        setze_geraet(bild, clipbild('t_schneider', u, True), BW / 2, 540, 620, a)
        merk = [('30 fairy tales', 300, 300), ('Drawn & set to music live', 300, 520), ('Read-along speech bubbles', 300, 740),
                ('Touch controls', 1620, 300), ('Settings for parents', 1620, 520), ('English & German', 1620, 740)]
        for i, (txt, x, y) in enumerate(merk):
            ta = fenster(t, B(12) + 0.3 + i * 0.35, B(14) + 0.3, 0.4, 0.4)
            if ta <= 0: continue
            f = F_HALB(36)
            im, pad = textbild(txt, f, WEISS)
            wt = im.width - 2 * pad
            dr = ImageDraw.Draw(bild)
            x0 = x - wt / 2 - 34
            dr.rounded_rectangle((x0, y - 12, x0 + wt + 68, y + 62), 37, fill=(40, 32, 92, int(220 * ta)), outline=GOLD + (int(200 * ta),), width=2)
            schreibe(bild, txt, f, x, y, WEISS, ta, ausr='m')

    # ---- 35-37,5 s: Kapitelkarte ----
    if B(14) <= t < B(15) + 0.2:
        a = fenster(t, B(14) + 0.1, B(15) + 0.2, 0.5, 0.4)
        schreibe(bild, 'Build your own', F_TITEL(150), BW / 2, 380, GOLD, a, ausr='m', leucht=(255, 170, 60))
        schreibe(bild, 'Step-by-step tutorial  ·  about 5 minutes  ·  no coding', F_TEXT(40), BW / 2, 590, WEISS, a, ausr='m')
        feenstaub(bild, t, B(14), 2.2, BW / 2, 480, 600, 70, seed=5)

    # ---- Schritt 1: Was man braucht (37,5-47,5) ----
    if B(15) <= t < B(19):
        u = t - B(15)
        kapitel(bild, t, 1, 'What you need', B(15), B(19))
        a = fenster(t, B(15) + 0.2, B(19), 0.6, 0.4)
        setze_geraet(bild, clipbild('buch_auf', 5.0), 560, 600, 640, a)
        punkte = [('Waveshare ESP32-S3-Touch-AMOLED-1.75C', 'round 1.75" AMOLED, touch, speaker, battery'),
                  ('A USB-C cable with data lines', 'not a charge-only cable'),
                  ('A computer with Chrome or Edge', 'Windows, Mac or Linux - no software to install')]
        for i, (h1, h2) in enumerate(punkte):
            st = B(15) + 1.0 + i * 1.6
            ta = fenster(t, st, B(19), 0.5, 0.4)
            punkt(bild, 1040, 420 + i * 170, i + 1, ta)
            schreibe(bild, h1, F_HALB(38), 1100, 392 + i * 170, WEISS, ta, dy=(1 - raus((t - st) / 0.6)) * 16)
            schreibe(bild, h2, F_TEXT(28), 1100, 446 + i * 170, LEISE, ta)

    # ---- Schritt 2: Aufspielen im Browser (47,5-62,5) ----
    if B(19) <= t < B(25):
        u = t - B(19)
        kapitel(bild, t, 2, 'Flash it from your browser', B(19), B(25))
        a = fenster(t, B(19) + 0.2, B(25), 0.6, 0.4)
        lay = Image.new('RGBA', (BW, BH), (0, 0, 0, 0))
        browser(lay, u, 400, 300, 1120, 600)
        bild.alpha_composite(mit_alpha(lay, a))
        texte = [('Open daveslab.de/basteln and choose "Fairy Tale Box"', 0.3, 3.0),
                 ('Click "Erase & install" and pick the USB port', 3.0, 7.2),
                 ('Wait about a minute - done!', 7.2, 15)]
        for txt, a0, a1 in texte:
            zeile(bild, t, txt, B(19) + a0, B(19) + a1, 940, x=BW / 2, groesse=36, ausr='m')
    # ---- Schritt 3: Erster Start (62,5-75) ----
    if B(25) <= t < B(30):
        u = t - B(25)
        kapitel(bild, t, 3, 'First start', B(25), B(30))
        a = fenster(t, B(25) + 0.2, B(30), 0.6, 0.4)
        setze_geraet(bild, clipbild('buch_auf', klemm(u - 0.8, 0, 99)), 620, 600, 700, a)
        zeile(bild, t, 'The fairy tale book opens.', B(25) + 1.2, B(30), 470, x=1110, groesse=44)
        zeile(bild, t, 'Every page is one tale -', B(25) + 3.2, B(30), 560, x=1110, groesse=34, farbe=LEISE)
        zeile(bild, t, 'with its own little melody.', B(25) + 3.2, B(30), 610, x=1110, groesse=34, farbe=LEISE)
        feenstaub(bild, t, B(25) + 1.6, 2.0, 620, 600, 380, 50, seed=7)

    # ---- Schritt 4: Bedienung (75-92,5) ----
    if B(30) <= t < B(37):
        u = t - B(30)
        kapitel(bild, t, 4, 'Controls', B(30), B(37))
        a = fenster(t, B(30) + 0.2, B(37), 0.6, 0.4)
        # Ablauf auf dem Schirm
        if u < 7.0: schirm = clipbild('buch_hinweise', u, True)
        elif u < 7.6: schirm = clipbild('blatt', u - 7.0)
        elif u < 10.0: schirm = clipbild('seite2', u - 7.6)
        elif u < 10.8: schirm = clipbild('flug', u - 10.0)
        else: schirm = clipbild('film_start', u - 10.8)
        ox, oy, sk = setze_geraet(bild, schirm, 600, 600, 720, a)
        # Touch-Bereiche (Drittel des Bildes) nacheinander zeigen
        zonen = [(0.0, 0.379, 'Top third', 'louder  ·  page back', 0.8), (0.379, 0.619, 'Middle', 'start  ·  double tap: next tale  ·  hold: book', 2.6),
                 (0.619, 1.0, 'Bottom third', 'quieter  ·  next page', 4.4)]
        for i, (z0, z1, h1, h2, st) in enumerate(zonen):
            za = fenster(u, st, 6.6, 0.4, 0.4) * a
            if za <= 0: continue
            lay = Image.new('RGBA', (500, 500), (0, 0, 0, 0))
            dz = ImageDraw.Draw(lay)
            top = 250 - G_R + z0 * 2 * G_R; bot = 250 - G_R + z1 * 2 * G_R
            aktiv = 0.3 + 0.7 * fenster(u, st, st + 1.8, 0.3, 0.4)  # gerade erklaert: kraeftig
            dz.rectangle((0, top + 2, 500, bot - 2), fill=(255, 213, 90, int(120 * aktiv)))
            dz.line((0, top, 500, top), fill=GOLD + (220,), width=2)
            dz.line((0, bot, 500, bot), fill=GOLD + (220,), width=2)
            m = Image.new('L', (500, 500), 0); ImageDraw.Draw(m).ellipse((250 - G_R, 250 - G_R, 250 + G_R, 250 + G_R), fill=255)
            lay.putalpha(ImageChops.multiply(lay.getchannel('A'), m))
            lay = lay.resize((int(500 * sk), int(500 * sk)), Image.LANCZOS)
            bild.alpha_composite(mit_alpha(lay, za), (int(ox), int(oy)))
            y = 380 + i * 150
            schreibe(bild, h1, F_HALB(40), 1100, y, GOLD, za * aktiv)
            schreibe(bild, h2, F_TEXT(30), 1100, y + 54, WEISS, za * aktiv)
        tipp(bild, ox, oy, sk, 0.5, 0.84, (u - 6.7) / 0.7)
        tipp(bild, ox, oy, sk, 0.5, 0.5, (u - 9.4) / 0.7)
        zeile(bild, t, 'Tap the bottom: the next page turns.', B(30) + 7.0, B(30) + 9.3, 480, x=1100, groesse=38)
        zeile(bild, t, 'Tap the middle: the tale starts.', B(30) + 9.4, B(37), 480, x=1100, groesse=38)
        zeile(bild, t, 'Speech bubbles stay until they are read.', B(30) + 11.5, B(37), 560, x=1100, groesse=30, farbe=LEISE)

    # ---- Schritt 5: Fuer Eltern (92,5-107,5) ----
    if B(37) <= t < B(43):
        u = t - B(37)
        kapitel(bild, t, 5, 'For parents', B(37), B(43))
        a = fenster(t, B(37) + 0.2, B(43), 0.6, 0.4)
        schirm = clipbild('buch_auf', 5.0) if u < 2.2 else clipbild('einstellungen', (u - 2.2))
        ox, oy, sk = setze_geraet(bild, schirm, 600, 600, 720, a)
        # Halten-Anzeige
        if u < 2.4:
            ha = fenster(u, 0.3, 2.4, 0.3, 0.3) * a
            prog = klemm((u - 0.5) / 1.6)
            dr = ImageDraw.Draw(bild)
            cx, cy = 1180, 470
            dr.ellipse((cx - 56, cy - 56, cx + 56, cy + 56), outline=(90, 80, 150, int(255 * ha)), width=10)
            if prog > 0: dr.arc((cx - 56, cy - 56, cx + 56, cy + 56), -90, -90 + 360 * prog, fill=GOLD + (int(255 * ha),), width=10)
            schreibe(bild, '3s', F_HALB(36), cx, cy - 26, WEISS, ha, ausr='m')
            schreibe(bild, 'Hold the BOOT button', F_HALB(40), 1270, 430, WEISS, ha)
            schreibe(bild, 'for 3 seconds', F_TEXT(32), 1270, 482, LEISE, ha)
        punkte = ['Maximum volume', 'Brightness', 'Reading speed for speech bubbles', 'Usage times - good night outside them',
                  'Language: English or German', 'Wi-Fi for the clock and updates']
        for i, p in enumerate(punkte):
            st = 2.8 + i * 1.5
            ta = fenster(u, st, 15, 0.4, 0.4) * a
            if ta <= 0: continue
            y = 330 + i * 90
            ImageDraw.Draw(bild).ellipse((1100, y + 16, 1116, y + 32), fill=GOLD + (int(255 * ta),))
            schreibe(bild, p, F_TEXT(34), 1136, y, WEISS, ta, dy=(1 - raus((u - st) / 0.5)) * 14)

    # ---- Schritt 6: WLAN (107,5-117,5) ----
    if B(43) <= t < B(47):
        u = t - B(43)
        kapitel(bild, t, 6, 'Wi-Fi (optional)', B(43), B(47))
        a = fenster(t, B(43) + 0.2, B(47), 0.6, 0.4)
        setze_geraet(bild, clipbild('wlan', u, True), 560, 600, 660, a)
        lay = Image.new('RGBA', (BW, BH), (0, 0, 0, 0))
        handy(lay, u, 1060, 300)
        bild.alpha_composite(mit_alpha(lay, a))
        zeile(bild, t, 'Connect your phone to', B(43) + 0.6, B(47), 420, x=1450, groesse=34)
        zeile(bild, t, '"Märchenbox-Setup"', B(43) + 0.6, B(47), 470, x=1450, groesse=34, farbe=GOLD, f=F_HALB(34))
        zeile(bild, t, 'pick your Wi-Fi, save.', B(43) + 0.6, B(47), 520, x=1450, groesse=34)
        zeile(bild, t, 'For the clock and', B(43) + 4.6, B(47), 620, x=1450, groesse=28, farbe=LEISE)
        zeile(bild, t, 'updates over Wi-Fi.', B(43) + 4.6, B(47), 660, x=1450, groesse=28, farbe=LEISE)

    # ---- Updates aus dem Internet (117,5-127,5) ----
    if B(47) <= t < B(51):
        u = t - B(47)
        kapitel(bild, t, 'UPDATES', 'Always new fairy tales', B(47), B(51))
        a = fenster(t, B(47) + 0.2, B(51), 0.6, 0.4)
        schirm = clipbild('buch_auf', 5.0) if u < 1.6 else clipbild('update', u - 1.6)
        setze_geraet(bild, schirm, 560, 600, 680, a)
        zeile(bild, t, 'New fairy tales arrive by themselves.', B(47) + 0.8, B(51), 400, x=1020, f=F_KOPF(52))
        punkte = ['The box checks for new versions over the internet', 'and installs them while it rests -', 'never in the middle of a story.',
                  'Wi-Fi and settings stay as they are.']
        for i, p in enumerate(punkte):
            st = 1.8 + i * 0.9
            ta = fenster(u, st, 10, 0.4, 0.4) * a
            if ta <= 0: continue
            y = 500 + i * 62 + (14 if i >= 3 else 0)
            if i in (0, 1, 3): ImageDraw.Draw(bild).ellipse((1024, y + 15, 1038, y + 29), fill=GOLD + (int(255 * ta),))
            schreibe(bild, p, F_TEXT(32), 1056, y, WEISS, ta, dy=(1 - raus((u - st) / 0.5)) * 12)
        zeile(bild, t, 'More fairy tales are on the way!', B(47) + 5.6, B(51), 780, x=1020, f=F_HALB(40), farbe=GOLD)
        feenstaub(bild, t, B(47) + 5.4, 1.6, 1300, 800, 420, 50, seed=13)

    # ---- Gute Nacht (127,5-135) ----
    if B(51) <= t < B(54):
        u = t - B(51)
        a = fenster(t, B(51) + 0.2, B(54), 0.6, 0.5)
        setze_geraet(bild, clipbild('gutenacht', u, True), 620, 560, 700, a)
        zeile(bild, t, 'Bedtime built in.', B(51) + 0.6, B(54), 440, x=1110, groesse=50, f=F_KOPF(64))
        zeile(bild, t, 'Outside the usage times', B(51) + 1.6, B(54), 540, x=1110, groesse=34, farbe=LEISE)
        zeile(bild, t, 'the box simply says good night.', B(51) + 1.6, B(54), 590, x=1110, groesse=34, farbe=LEISE)

    # ---- Abspann (135-150) ----
    if t >= B(54):
        u = t - B(54)
        a = klemm(u / 0.8) * (klemm((DAUER - 0.3 - t) / 1.8))
        setze_geraet(bild, clipbild('schluss', u, True), BW / 2, 400, 520, a)
        schreibe(bild, 'Fairy Tale Box', F_TITEL(140), BW / 2, 650, GOLD, fenster(t, B(54) + 0.8, DAUER - 0.3, 0.8, 1.8), ausr='m', leucht=(255, 170, 60))
        schreibe(bild, 'Free & open source  ·  new tales over the internet', F_HALB(38), BW / 2, 820, WEISS, fenster(t, B(54) + 1.8, DAUER - 0.3, 0.7, 1.8), ausr='m')
        schreibe(bild, 'daveslab.de/basteln    ·    github.com/ProD-byte/Maerchenbox', F_TEXT(34), BW / 2, 880, LEISE,
                 fenster(t, B(54) + 2.6, DAUER - 0.3, 0.7, 1.8), ausr='m')
        feenstaub(bild, t, B(54) + 0.5, 2.5, BW / 2, 680, 600, 80, seed=9)
        feenstaub(bild, t, B(57), 2.0, BW / 2, 450, 500, 60, seed=10)

    return bild.convert('RGB')

if __name__ == '__main__':
    if sys.argv[1] == 'rand':
        t = 0.0
        while t < DAUER:
            zeichne(t); t += 0.25
        for r in sorted(RAND): print(r)
        print('Rand-Treffer:', len(RAND))
        sys.exit()
    if sys.argv[1] == 'probe':
        os.makedirs(sys.argv[2], exist_ok=True)
        for s in sys.argv[3:]:
            zeichne(float(s)).save(os.path.join(sys.argv[2], f'p_{float(s):06.1f}.jpg'), quality=88)
            print(s, flush=True)
    else:
        import imageio_ffmpeg, time
        aus = sys.argv[2]
        n = int(DAUER * FPS)
        w = imageio_ffmpeg.write_frames(aus, (BW, BH), fps=FPS, codec='libx264', pix_fmt_out='yuv420p', quality=None,
                                        bitrate=None, macro_block_size=8, audio_path=os.path.join(TMP, 'musik.wav'),
                                        audio_codec='aac', output_params=['-crf', '18', '-preset', 'slow', '-b:a', '192k', '-movflags', '+faststart', '-shortest'])
        w.send(None)
        t0 = time.time()
        for i in range(n):
            w.send(zeichne(i / FPS).tobytes())
            if i % 150 == 0: print(f'{i}/{n}  {time.time() - t0:.0f}s', flush=True)
        w.close()
        print('fertig', aus)

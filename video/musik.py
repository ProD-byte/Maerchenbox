"""Musik fuer den Maerchenbox-Trailer: warmes Spieluhr-Stueck in G-Dur, 96 BPM (Takt = 2,5 s), 54 Takte + Ausklang.

Komplett synthetisch (keine Samples) - frei im Video verwendbar.
Takte: 0-1 Intro (Spieluhr allein) | 2-3 Titel | 4-11 Teaser (voll, sanfter Rhythmus) | 12-13 Merkmale |
14 Uebergang | 15-49 Tutorial (ruhig, Harfe) | 50-52 Abspann (Thema voll) | 53 Schlussakkord (klingt aus).
Aufruf: python musik.py ausgabe.wav
"""
import sys, wave
import numpy as np

SR = 44100
BPM = 96
BEAT = 60 / BPM
BAR = 4 * BEAT          # 2,5 s
TAKTE = 54
N = int((TAKTE * BAR + 6) * SR)
rng = np.random.default_rng(11)
L = np.zeros(N); R = np.zeros(N); HALL = np.zeros(N)

def hz(m): return 440.0 * 2 ** ((m - 69) / 12)
def zeit(n): return np.arange(n) / SR

def setze(sig, start, pan=0.0, hall=0.25, vol=1.0):
    i = int(start * SR)
    if i >= N: return
    sig = sig[: N - i] * vol
    l = np.cos((pan + 1) * np.pi / 4); r = np.sin((pan + 1) * np.pi / 4)
    L[i:i + len(sig)] += sig * l * 1.414
    R[i:i + len(sig)] += sig * r * 1.414
    HALL[i:i + len(sig)] += sig * hall

def spieluhr(m, dauer=2.2):
    """Spieluhr / Celesta: harte Anschlagsspitze, glockige Teiltoene, schnelles Abklingen."""
    n = int(dauer * SR); t = zeit(n); f = hz(m)
    s = (np.sin(2 * np.pi * f * t) * np.exp(-t * 2.6)
         + 0.45 * np.sin(2 * np.pi * f * 2.0 * t) * np.exp(-t * 4.5)
         + 0.25 * np.sin(2 * np.pi * f * 3.01 * t) * np.exp(-t * 7)
         + 0.12 * np.sin(2 * np.pi * f * 5.43 * t) * np.exp(-t * 12))
    s *= np.minimum(1, t * 800)
    return s * 0.32

def harfe(m, dauer=2.5):
    n = int(dauer * SR); t = zeit(n); f = hz(m)
    s = sum(np.sin(2 * np.pi * f * k * t) / k ** 1.6 * np.exp(-t * (1.4 + k * 0.9)) for k in range(1, 6))
    s *= np.minimum(1, t * 400)
    return s * 0.22

def flaeche(ms, dauer):
    """Weiche Streicherflaeche: verstimmte Saegezaehne, tiefpassartig geglaettet, langsam ein-/ausgeblendet."""
    n = int(dauer * SR); t = zeit(n); s = np.zeros(n)
    for m in ms:
        for d in (-0.07, 0.0, 0.08):
            f = hz(m + d)
            ph = (f * t) % 1.0
            saege = 2 * ph - 1
            s += saege * 0.5 + np.sin(2 * np.pi * f * t) * 0.8
    k = int(SR / 900)  # einfacher Tiefpass (gleitender Mittelwert)
    s = np.convolve(s, np.ones(k) / k, mode='same')
    huelle = np.minimum(1, t / 0.6) * np.minimum(1, (dauer - t) / 0.7)
    return s * huelle * 0.018

def bass(m, dauer):
    n = int(dauer * SR); t = zeit(n); f = hz(m)
    s = np.sin(2 * np.pi * f * t) + 0.3 * np.sin(2 * np.pi * 2 * f * t)
    s *= np.minimum(1, t * 60) * np.exp(-t * 0.9) * np.minimum(1, (dauer - t) * 8)
    return s * 0.16

def kick():
    n = int(0.45 * SR); t = zeit(n)
    f = 50 + 70 * np.exp(-t * 30)
    return np.sin(2 * np.pi * np.cumsum(f) / SR) * np.exp(-t * 7) * 0.32

def shaker(laut=1.0):
    n = int(0.09 * SR); t = zeit(n)
    s = rng.standard_normal(n); s = s - np.convolve(s, np.ones(6) / 6, mode='same')
    return s * np.exp(-t * 45) * 0.03 * laut

def glocke(m):
    n = int(5 * SR); t = zeit(n); f = hz(m)
    s = (np.sin(2 * np.pi * f * t) * np.exp(-t * 0.8) + 0.5 * np.sin(2 * np.pi * f * 2.76 * t) * np.exp(-t * 1.4)
         + 0.3 * np.sin(2 * np.pi * f * 5.4 * t) * np.exp(-t * 2.5))
    return s * np.minimum(1, t * 600) * 0.16

def funkeln(start, dauer, dichte=10):
    """Feenstaub: viele kurze, hohe Spieluhr-Toene aus der Tonleiter."""
    ton = [79, 81, 83, 86, 88, 91, 93, 95]
    for _ in range(int(dauer * dichte)):
        setze(spieluhr(rng.choice(ton), 1.0) * 0.35, start + rng.random() * dauer, pan=rng.uniform(-0.8, 0.8), hall=0.6)

# Akkorde je Takt: G - Em - C - D (Schluss C - D - G)
AKK = {'G': [55, 59, 62], 'Em': [52, 55, 59], 'C': [48, 52, 55], 'D': [50, 54, 57], 'Am': [57, 60, 64]}
FOLGE = ['G', 'Em', 'C', 'D']
GRUND = {'G': 43, 'Em': 40, 'C': 36, 'D': 38, 'Am': 45}
def akkord(takt):
    if takt == 51: return 'C'
    if takt == 52: return 'D'
    if takt >= 53: return 'G'
    return FOLGE[takt % 4]

# Thema (8 Takte, Viertel = 1): Tonhoehe, Start in Schlaegen, Laenge
THEMA = [(74, 0, 1), (79, 1, 1), (81, 2, 1), (83, 3, 1), (81, 4, 1.5), (79, 5.5, 0.5), (76, 6, 2),
         (72, 8, 1), (76, 9, 1), (79, 10, 1), (81, 11, 1), (79, 12, 1.5), (76, 13.5, 0.5), (74, 14, 2),
         (74, 16, 1), (79, 17, 1), (81, 18, 1), (83, 19, 1), (86, 20, 1.5), (83, 21.5, 0.5), (81, 22, 2),
         (79, 24, 1), (81, 25, 0.5), (79, 25.5, 0.5), (78, 26, 1), (74, 27, 1), (79, 28, 4)]
def thema(start_takt, oktave=0, vol=1.0, pan=0.15):
    for m, b, l in THEMA:
        setze(spieluhr(m + 12 * oktave, 1.6 + l * BEAT), start_takt * BAR + b * BEAT, pan=pan, hall=0.4, vol=vol)

for takt in range(TAKTE):
    t0 = takt * BAR
    a = akkord(takt)
    teaser = 4 <= takt <= 13
    tutorial = 15 <= takt <= 49
    abspann = takt >= 50
    # Flaeche (ab Takt 1)
    if takt >= 1 and takt != 14:
        laenge = BAR + (3.5 if takt == 53 else 0.15)
        setze(flaeche([x + 12 for x in AKK[a]], laenge), t0, pan=-0.2, hall=0.5, vol=1.15 if teaser or abspann else 0.85)
    # Bass
    if takt >= 2 and takt != 14:
        if takt == 53: setze(bass(GRUND[a], 5.5), t0, vol=1.1)
        else:
            setze(bass(GRUND[a], BEAT * 2), t0); setze(bass(GRUND[a] + (7 if teaser else 0), BEAT * 2), t0 + 2 * BEAT, vol=0.85)
    # Harfen-Arpeggio (Achtel)
    if (takt >= 2 and takt <= 13) or tutorial or (50 <= takt <= 52):
        ton = AKK[a] + [AKK[a][0] + 12, AKK[a][1] + 12, AKK[a][2] + 12]
        muster = [0, 1, 2, 3, 4, 3, 2, 1]
        for k, i in enumerate(muster):
            setze(harfe(ton[i] + 12, 1.6), t0 + k * BEAT / 2, pan=0.35 if k % 2 else -0.1, hall=0.35,
                  vol=0.75 if tutorial else 1.0)
    # sanfter Rhythmus im Teaser und im Abspann
    if teaser or (50 <= takt <= 52):
        for b in (0, 2):
            setze(kick(), t0 + b * BEAT, hall=0.05, vol=0.9 if teaser else 0.7)
        for k in range(8):
            setze(shaker(1.0 if k % 2 else 0.5), t0 + k * BEAT / 2, pan=0.4, hall=0.1)
    if tutorial and takt % 2 == 1:  # ganz leiser Puls
        setze(kick() * 0.35, t0, hall=0.05)

# Melodien
thema(0, vol=0.9)            # Intro + Titel (8 Takte Thema laufen bis in den Teaser)
thema(8, oktave=0, vol=1.0)  # Teaser zweite Haelfte
for s in (16, 24, 32, 40):   # Tutorial: Thema leise, eine Oktave hoeher, mit Pausen
    thema(s, oktave=1, vol=0.45, pan=-0.25)
thema(46, vol=0.95)          # Abspann (endet auf G in Takt 53)

# Feenstaub und Glocken an den Uebergaengen
funkeln(0.2, 4.5, 6)
funkeln(4 * BAR - 1.0, 1.5, 18)
funkeln(14 * BAR, BAR, 14)
for t in (4 * BAR, 12 * BAR, 15 * BAR, 50 * BAR):
    setze(glocke(79), t, hall=0.7, vol=0.8); setze(glocke(86), t + 0.02, hall=0.7, vol=0.5)
setze(glocke(67), 53 * BAR, hall=0.8, vol=1.0); setze(glocke(74), 53 * BAR, hall=0.8, vol=0.7); setze(glocke(79), 53 * BAR + 0.03, hall=0.8, vol=0.6)
# Uebergang Takt 14: Rauschen schwillt an
n = int(BAR * SR); t = zeit(n); w = rng.standard_normal(n)
w = np.convolve(w, np.ones(30) / 30, mode='same') * (t / BAR) ** 2 * 0.25
setze(w, 14 * BAR, hall=0.6)

# Hall: Faltung mit abklingendem Rauschen (Stereo durch zwei Antworten)
def hall_antwort(dauer=2.8):
    n = int(dauer * SR); t = zeit(n)
    a = rng.standard_normal(n) * np.exp(-t * 2.3)
    return np.convolve(a, np.ones(8) / 8, mode='same') * 0.06
def falte(x, h):
    m = len(x) + len(h) - 1; F = 1 << int(np.ceil(np.log2(m)))
    return np.fft.irfft(np.fft.rfft(x, F) * np.fft.rfft(h, F), F)[:len(x)]
L += falte(HALL, hall_antwort()); R += falte(HALL, hall_antwort())

# Ausklang, Normalisieren, weiche Begrenzung
st = np.stack([L, R])
ende = int((TAKTE * BAR + 5.5) * SR)
st = st[:, :ende]
st[:, -int(2 * SR):] *= np.linspace(1, 0, int(2 * SR)) ** 2
st /= np.max(np.abs(st)) / 0.92
st = np.tanh(st * 1.15) / np.tanh(1.15) * 0.92
st[:, :int(0.01 * SR)] *= np.linspace(0, 1, int(0.01 * SR))
pcm = (st.T * 32767).astype(np.int16)
with wave.open(sys.argv[1], 'wb') as w:
    w.setnchannels(2); w.setsampwidth(2); w.setframerate(SR); w.writeframes(pcm.tobytes())
print('Musik', round(ende / SR, 1), 's')

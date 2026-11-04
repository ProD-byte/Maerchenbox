"""Baut die Maerchen-Datei fuer die Partition "spiffs": Ton (Aufnahme + Erzaehlerin) und Bilder.

Aufruf: python werkzeuge/mach_maerchen.py [aufnahme-ordner] [ausgabe.bin] [vorhoer.wav]
Standard: aufnahme -> maerchen.bin (+ maerchen_vorhoeren.wav)

Im Aufnahme-Ordner (aus maerchenbox2.html aufgenommen):
  musik_48k_mono.raw  Musik und Geraeusche, 48 kHz mono int16
  sprache.json        Einsatzzeiten der Erzaehlerin (Sekunden ab Aufnahmebeginn) und Texte
  sprache0..3.wav     Erzaehlerin, 24 kHz mono (werkzeuge/sprache_erzeugen.ps1)
  bilder.json         Zeit jedes Bildes (Sekunden), bilder/f0000.jpg ...
  setup.jpg, update.jpg  Extra-Bilder fuer WLAN-Einrichtung und Update

Aufbau der Datei (little endian):
  Kopf:  "MRC2", rate, samples, audioOff, nBilder, indexOff, setupBild, updateBild  (je uint32)
  Index: je Bild offset, groesse, zeit_ms (uint32); nach den nBilder Bildern folgen setup und update
  Ton:   int16 mono
  Bilder: JPEG, 284 x 240
"""
import json, struct, sys, wave
from pathlib import Path
import numpy as np

RATE = 24000
LAENGE = 100.0      # Sekunden (Stueck endet bei ca. 97 s, danach der Vorhang)
DUCK = 0.45         # Musik waehrend der Erzaehlerstimme
PARTITION = 0xBF0000

ordner = Path(sys.argv[1] if len(sys.argv) > 1 else "aufnahme")
ziel = Path(sys.argv[2] if len(sys.argv) > 2 else "maerchen.bin")
vorhoer = Path(sys.argv[3] if len(sys.argv) > 3 else "maerchen_vorhoeren.wav")

# ---- Ton ----
m48 = np.fromfile(ordner / "musik_48k_mono.raw", dtype=np.int16).astype(np.float64) / 32768
n = np.arange(-64, 65)
h = np.sinc(2 * 10500 / 48000 * n) * np.hamming(len(n))  # Tiefpass vor dem Halbieren der Rate
h /= h.sum()
musik = np.convolve(m48, h, mode="same")[::2][: int(LAENGE * RATE)]
musik = np.pad(musik, (0, int(LAENGE * RATE) - len(musik)))

stimme = np.zeros_like(musik)
huelle = np.zeros_like(musik)
for i, e in enumerate(json.loads((ordner / "sprache.json").read_text(encoding="utf-8"))):
    with wave.open(str(ordner / f"sprache{i}.wav")) as w:
        assert w.getframerate() == RATE and w.getsampwidth() == 2 and w.getnchannels() == 1
        s = np.frombuffer(w.readframes(w.getnframes()), dtype=np.int16).astype(np.float64) / 32768
    a = int(e["at"] * RATE)
    b = min(len(stimme), a + len(s))
    stimme[a:b] += s[: b - a]
    huelle[a:b] = 1
k = int(0.3 * RATE)
huelle = np.convolve(huelle, np.ones(k) / k, mode="same")
mix = musik * (1 - (1 - DUCK) * huelle) + stimme
f = int(1.0 * RATE)
mix[-f:] *= np.linspace(1, 0, f)
mix *= 10 ** (-1 / 20) / np.abs(mix).max()
pcm = np.round(mix * 32767).astype("<i2").tobytes()

# ---- Bilder ----
zeiten = json.loads((ordner / "bilder.json").read_text())
bilder = [(round(t * 1000), (ordner / "bilder" / f"f{i:04d}.jpg").read_bytes()) for i, t in enumerate(zeiten) if t < LAENGE]
bilder.append((0, (ordner / "setup.jpg").read_bytes()))
bilder.append((0, (ordner / "update.jpg").read_bytes()))
nStory = len(bilder) - 2

KOPF = 4 + 7 * 4
indexOff = KOPF
audioOff = indexOff + 12 * len(bilder)
audioOff = (audioOff + 3) & ~3
off = audioOff + len(pcm)
index, daten = b"", b""
for ms, jpg in bilder:
    index += struct.pack("<III", off, len(jpg), ms)
    daten += jpg
    off += len(jpg)

kopf = b"MRC2" + struct.pack("<7I", RATE, len(pcm) // 2, audioOff, nStory, indexOff, nStory, nStory + 1)
blob = kopf + index
blob += b"\0" * (audioOff - len(blob)) + pcm + daten
assert len(blob) <= PARTITION, f"zu gross: {len(blob)} > {PARTITION}"
ziel.write_bytes(blob)
print(f"{ziel}: {len(pcm) / 2 / RATE:.1f} s Ton, {nStory} Bilder, {len(blob) / 1e6:.2f} MB "
      f"(Partition {PARTITION / 1e6:.2f} MB)")
with wave.open(str(vorhoer), "wb") as w:
    w.setnchannels(1); w.setsampwidth(2); w.setframerate(RATE); w.writeframes(pcm)
print(f"Vorhoeren: {vorhoer}")

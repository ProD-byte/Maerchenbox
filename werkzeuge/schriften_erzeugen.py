"""Erzeugt die geglaetteten Schriften fuer das Display (src/schriften.h und src/schriften.cpp).

Schrift: Lato (Lukasz Dziedzic, SIL Open Font License 1.1 - darf eingebettet werden),
aus C:\\Windows\\Fonts.

Format je Schrift:  Glyph-Tabelle (Unicode, Breite, Hoehe, x-Versatz, y-Versatz ab Grundlinie nach oben,
Vorschub, Offset) und Bitmaps mit 4 Bit Deckkraft je Pixel (2 Pixel je Byte, hohes Halbbyte zuerst).

Aufruf (im Projektordner):  python werkzeuge/schriften_erzeugen.py
"""
from pathlib import Path
from PIL import Image, ImageFont

ORDNER = Path(r"C:\Windows\Fonts")
FAKTOR = 4
TEXT = [chr(c) for c in range(0x20, 0x7F)] + list("ÄÖÜäöüß–…„“·♪")

SCHRIFTEN = [
    ("SCHRIFT_K", 15, "Bold"),
    ("SCHRIFT_M", 20, "Black"),
    ("SCHRIFT_B", 17, "Black"),   # Titel im Maerchenbuch
    ("SCHRIFT_G", 30, "Black"),
]


def glyph(font, ch):
    maske, (dx, dy) = font.getmask2(ch, mode="L", anchor="ls")
    w, h = maske.size
    adv = font.getlength(ch)
    if w == 0 or h == 0:
        return 0, 0, 0, 0, round(adv / FAKTOR), b""
    img = Image.frombytes("L", (w, h), bytes(maske))
    # auf Ziel verkleinern, Versatz auf ganze Zielpixel ausrichten
    x0, y0 = dx // FAKTOR, dy // FAKTOR
    pad = Image.new("L", ((w + dx - x0 * FAKTOR + FAKTOR - 1) // FAKTOR * FAKTOR,
                          (h + dy - y0 * FAKTOR + FAKTOR - 1) // FAKTOR * FAKTOR), 0)
    pad.paste(img, (dx - x0 * FAKTOR, dy - y0 * FAKTOR))
    klein = pad.resize((pad.width // FAKTOR, pad.height // FAKTOR), Image.BOX)
    bw, bh = klein.size
    px = list(klein.getdata())
    daten = bytearray()
    for y in range(bh):
        zeile = [min(15, (p + 8) // 17) for p in px[y * bw:(y + 1) * bw]]
        if len(zeile) % 2:
            zeile.append(0)
        for i in range(0, len(zeile), 2):
            daten.append(zeile[i] << 4 | zeile[i + 1])
    return bw, bh, x0, -y0, round(adv / FAKTOR), bytes(daten)


kopf = ["// Erzeugt von werkzeuge/schriften_erzeugen.py - nicht von Hand aendern.",
        "// Schrift: Lato (SIL Open Font License 1.1)", "#pragma once", '#include "gfx.h"', ""]
aus = ["// Erzeugt von werkzeuge/schriften_erzeugen.py - nicht von Hand aendern.",
       "// Schrift: Lato (SIL Open Font License 1.1)", '#include "schriften.h"', ""]
for name, groesse, schnitt in SCHRIFTEN:
    font = ImageFont.truetype(str(ORDNER / f"Lato-{schnitt}.ttf"), groesse * FAKTOR)
    asc, desc = font.getmetrics()
    tab, bits = [], bytearray()
    for ch in TEXT:
        w, h, x, y, adv, d = glyph(font, ch)
        tab.append((ord(ch), w, h, x, y, adv, len(bits)))
        bits += d
    aus.append(f"static const uint8_t {name}_BITS[] = {{")
    for i in range(0, len(bits), 24):
        aus.append("  " + ",".join(str(b) for b in bits[i:i + 24]) + ",")
    aus.append("};")
    aus.append(f"static const Glyph {name}_GLYPHS[] = {{")
    for t in tab:
        aus.append("  {%d,%d,%d,%d,%d,%d,%d}," % t)
    aus.append("};")
    kopf.append(f"extern const Schrift {name};  // {groesse} px {schnitt}")
    aus.append(f"const Schrift {name} = {{{name}_GLYPHS, {name}_BITS, {len(tab)}, "
               f"{round(asc / FAKTOR)}, {round(desc / FAKTOR)}}};")
    aus.append("")
    print(f"{name}: {groesse} px {schnitt}, {len(tab)} Zeichen, {len(bits)} Bytes")

src = Path(__file__).resolve().parent.parent / "src"
(src / "schriften.cpp").write_text("\n".join(aus) + "\n", encoding="utf-8")
(src / "schriften.h").write_text("\n".join(kopf) + "\n", encoding="utf-8")

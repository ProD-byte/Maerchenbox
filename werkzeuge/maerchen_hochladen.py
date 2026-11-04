"""Spielt maerchen.bin (Ton + Bilder) ueber WLAN auf die Schneiderlein-Box.

Aufruf: python werkzeuge/maerchen_hochladen.py [adresse]   (Standard: schneiderlein.local)
Passwort: ota_passwort aus ../Himmelsstation/geheim.ini (wie beim Programm-Update).
"""
import configparser, subprocess, sys
from pathlib import Path

box = Path(__file__).resolve().parent.parent
adresse = sys.argv[1] if len(sys.argv) > 1 else "schneiderlein.local"

c = configparser.ConfigParser(inline_comment_prefixes=(";",))
c.read(box.parent / "Himmelsstation" / "geheim.ini", encoding="utf-8")
passwort = c.get("geheim", "ota_passwort", fallback="").strip()

espota = Path.home() / ".platformio/packages/framework-arduinoespressif32@3.20014.231204/tools/espota.py"
befehl = [sys.executable, str(espota), "-i", adresse, "-s", "-f", str(box / "maerchen.bin"), "-r", "-t", "30"]
if passwort:
    befehl[4:4] = ["-a", passwort]
sys.exit(subprocess.call(befehl))

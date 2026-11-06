# Trailer + Tutorial (englisch)

1. `python aufnehmen.py clips.json %TEMP%\maerchen-video\clips` - holt die Szenen vom runden Board (USB-Befehl `V`,
   15 Bilder/s, mit PlatformIO-Python wegen pyserial). Die Box schaltet dafuer auf Englisch (Befehl `E`, nicht gespeichert).
2. `python musik.py %TEMP%\maerchen-video\musik.wav` - Spieluhr-Musik, 96 BPM, Takt 2,5 s (synthetisch, frei nutzbar).
3. `python schnitt.py probe ordner 12 80 ...` - Probebilder; `python schnitt.py voll Maerchenbox-Trailer-Tutorial.mp4`
   rendert das ganze Video (1920x1080, 30 fps, H.264 + AAC, braucht `pip install imageio-ffmpeg`).
4. `python vorschaubild.py thumbnail.jpg` - YouTube-Vorschaubild. Titel, Beschreibung und Kapitel: `youtube.txt`.
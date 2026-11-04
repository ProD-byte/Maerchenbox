# Märchenbox

Spieluhr-Filme für das Xiaozhi-Board `ostb-2st-wifi-double-mic`. Die Box zeichnet die Filme
live mit bis zu 30 Bildern pro Sekunde und erzeugt die Spieluhr-Musik selbst. Es gibt keine
gespeicherten Bilder oder Töne: Jedes Märchen ist Programmcode (`src/schneiderlein.cpp`).

## Das tapfere Schneiderlein (5 Minuten)

| Zeit | Szene |
|---|---|
| 0:00 | Werkstatt: nähen, Mus kaufen, Brot streichen |
| 0:34 | Sieben auf einen Streich, Gürtel sticken |
| 1:04 | Aufbruch mit Käse und Vogel, Wanderung zum Berg |
| 1:40 | Der Riese: Stein gegen Käse, Steinwurf gegen Vogel |
| 2:22 | Der Riese trägt den Baum, das Schneiderlein sitzt in den Ästen |
| 2:50 | Nacht im Wald: Steine, Zank, die Riesen fliehen |
| 3:28 | Das Einhorn rennt sein Horn in den Baum |
| 3:54 | Das Wildschwein in der Kapelle |
| 4:20 | Hochzeit mit Glocken und Feuerwerk, Ende |

Mit `?t=` beim Bildschirmfoto lässt sich jede Stelle ansehen, z. B. `bild.bmp?t=195`.

## Von einem, der auszog, das Fürchten zu lernen (5 Minuten)

Kindgerecht erzählt: niedliche Gespenster, Katzen und ein freundliches Skelett.

| Zeit | Szene |
|---|---|
| 0:00 | Zu Hause: der Bruder zittert vor der Eule, Hans weiß nicht, was Gruseln ist |
| 0:36 | Glockenturm: das Laken-Gespenst purzelt die Treppe hinunter |
| 1:12 | Wanderung zum Spukschloss, der König zeigt den Aushang |
| 1:44 | 1. Nacht: schwarze Katzen spielen Karten und werden verscheucht |
| 2:30 | 2. Nacht: ein Skelett fällt den Kamin herunter, Kegeln mit Kürbis |
| 3:16 | 3. Nacht: das fahrende Bett, der alte Bart im Amboss, drei Schatzkisten |
| 4:00 | Hochzeit |
| 4:24 | Ein Eimer kaltes Wasser mit Fischlein: Brrr! Ende |

## Bedienung

| Knopf | im Film / Startbild | im Märchenbuch |
|---|---|---|
| Start 1× | Start / Stopp | Märchen starten bzw. Einstellung ausführen |
| Start 2× | nächstes Märchen | – |
| Start lang | Märchenbuch | Buch schließen |
| Laut / Leise | Lautstärke (wird gemerkt) | umblättern |

Belegung am Gerät (so liegt es am besten in der Hand):

Die drei Knöpfe sitzen rechts am Gerät. Oben ist immer Lauter, in der Mitte Start, unten Leiser,
auch wenn das Bild um 180° gedreht ist (dann tauschen die Knöpfe oben und unten ihre Aufgabe).

| GPIO | Lage (Bild normal) | normal | Bild gedreht |
|---|---|---|---|
| 0 | oben | Lauter | Leiser |
| 39 | Mitte | Start | Start |
| 40 | unten | Leiser | Lauter |

Das Menü ist ein Märchenbuch: Beim Einschalten klappt es auf, jedes Märchen ist eine Doppelseite
(links ein bewegtes Bild, rechts Titel und Startknopf), sonst nichts – Kinder können nichts verstellen.

**Einstellungen (für Eltern): Lauter und Leiser zusammen 3 Sekunden halten.** Dort: Bild drehen
(180°, oben bleibt Lauter, wird gemerkt), WLAN einrichten, zurück zum Buch. Lauter/Leiser wählen,
Start führt aus; nach einer Minute ohne Taste geht es von selbst zurück ins Buch.

In den Einstellungen (Lauter + Leiser 3 s) „WLAN einrichten“ wählen (zurück: Start lang drücken), danach öffnet die Box das WLAN **„Märchenbox-Setup“**.
Mit dem Handy verbinden, WLAN wählen, speichern.

## Updates über WLAN

```bash
pio run -t upload
```

Passwort: `ota_passwort` aus `../Himmelsstation/geheim.ini`. Per USB, falls die Box nicht mehr startet:

```bash
pio run -e ostb2st -t upload
```

## Hilfen für die Entwicklung (im Heimnetz)

- `http://maerchenbox.local/bild.bmp` – Bildschirmfoto, mit `?t=42` das Märchen bei 42 s, `&m=1` zweites Märchen,
  im Buch mit `?blatt=0.5` mitten im Umblättern, `?auf=1200` beim Aufklappen (ms)
- `http://maerchenbox.local/taste?k=kurz|doppel|lang|laut|leise|einstellungen` – Knopf drücken
- `http://maerchenbox.local/status` – Zustand, Bilder/s, Zeichenzeit, letzte Tastendrücke
- `http://maerchenbox.local/messtest` – was kostet welche Form beim Zeichnen

## Aufbau

- `src/gfx.*` – Zeichnen: Polygone mit weichen Kanten, Kreise, Linien, Kurven, Text, Kamera
- `src/lcd.*` – NV3023-Display, Streifen per DMA
- `src/synth.*` – Spieluhr-Synthesizer (Noten, Glocken, Rauschen, Hall)
- `src/maerchen.h` – Schnittstelle eines Märchens: Bild zur Zeit t, Töne, Startbild, Vorschau fürs Buch
- `src/figuren.*` – gemeinsame Figuren, Kulissen und Effekte (Schneiderlein-Figur mit `Aussehen` einkleidbar)
- `src/melodien.h` – gemeinsame Spieluhr-Melodien
- `src/schneiderlein.cpp` – Das tapfere Schneiderlein
- `src/fuerchten.cpp` – Von einem, der auszog, das Fürchten zu lernen
- `werkzeuge/schriften_erzeugen.py` – Schrift Lato als `src/schriften.h`

Ein neues Märchen: eine weitere `.cpp` mit einem `Maerchen` (Bausteine aus `figuren.h`), in `maerchen.h` bekannt machen und in `LISTE` in `src/main.cpp` eintragen.

## Pins (aus der Xiaozhi-Firmware ausgelesen)

| Funktion | GPIO |
|---|---|
| I2C SDA / SCL (ES8311 0x18, ES7210 0x40) | 12 / 11 |
| I2S MCLK / BCLK / WS / DOUT / DIN | 5 / 15 / 16 / 6 / 7 |
| Lautsprecher-Verstärker | 4 |
| Display NV3023 (284 × 240, x-Versatz 36): SCLK / MOSI / CS / DC / Reset | 9 / 10 / 14 / 8 / 18 |
| Hintergrundbeleuchtung | 13 |
| Knöpfe (siehe Belegung oben) | 39 / 0 / 40 |
| Ladestatus | 47 |
| Akkuspannung (ADC2 Kanal 6) | 17 |

## Ältere Version

`aufnahme/`, `werkzeuge/mach_maerchen.py` und `werkzeuge/maerchen_hochladen.py` gehören zur
früheren Version, die die aufgenommene Märchenbox abgespielt hat. Sie werden nicht mehr gebraucht.

## Zurück zu Xiaozhi

```bash
esptool --port COM16 write-flash 0 ../Xiaozhi-Backup/xiaozhi_ostb-2st_full16MB.bin
```

# Englische Texte der Märchenbox

Die Box zeigt alle Texte auf Deutsch oder auf Englisch (Eltern-Einstellungen → „Sprache: Deutsch / Language: English“).
Im Quelltext steht nur Deutsch. Ist Englisch eingeschaltet, schlägt `Gfx::text` / `Gfx::textBreite` jeden Text in
`src/uebersetzung_en.cpp` nach (binäre Suche); unbekannte Texte bleiben deutsch.

Diese Datei wird erzeugt, nicht von Hand bearbeitet:

- `texte.json` – alle Texte aus den Märchen-Dateien (Index → deutscher Text), gezogen mit `texte_holen.py`
- `en/t*.json` – englisch je Index; `en/inc.json` – Texte aus `schneiderlein_teil2.inc` (deutsch → englisch);
  `en/main.json` – Menü-, Buch- und Einstellungstexte aus `main.cpp` sowie Vorlagen wie `"%d Minuten"`
- `tabelle_bauen.py` – baut daraus die sortierte Tabelle:

```
python werkzeuge/englisch/tabelle_bauen.py werkzeuge/englisch src/uebersetzung_en.cpp
```

Regeln: Ein Vers oder eine Sprechblase ist je Zeile ein eigener Text – die englische Zeile muss für sich
stehen und etwa gleich lang sein (höchstens ~34 Zeichen). Die Schrift kennt keine englischen Anführungszeichen
(” ’), dafür `'` und `"` nehmen. Texte, die mit `snprintf` gebaut werden, über `U("...")` (main.cpp) bzw.
`Gfx::uebersetze("...")` übersetzen. Neuer deutscher Text in einem Märchen → in `en/` ergänzen und neu bauen;
`tabelle_bauen.py` meldet fehlende Übersetzungen.

Prüfen über USB (rundes Board): `S` schaltet die Sprache um (ohne Speichern), danach `P` (Texte am Rand) oder
`F …` (Bildschirmfoto).

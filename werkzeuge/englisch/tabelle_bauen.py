# Baut src/uebersetzung_en.cpp: sortierte Tabelle deutsch -> englisch (strcmp-Reihenfolge = Bytes von UTF-8)
import json, sys, glob, os
d = sys.argv[1]
ziel = sys.argv[2]
texte = json.load(open(os.path.join(d, 'texte.json'), encoding='utf-8'))
paare = {}
fehlt = []
uebers = {}
for f in sorted(glob.glob(os.path.join(d, 'en', 't*.json'))):
    uebers.update(json.load(open(f, encoding='utf-8')))
for i, e in enumerate(texte):
    en = uebers.get(str(i))
    if en is None: fehlt.append((i, e['de'])); continue
    paare[e['de']] = en
paare.update(json.load(open(os.path.join(d, 'en', 'main.json'), encoding='utf-8')))
paare.update(json.load(open(os.path.join(d, 'en', 'inc.json'), encoding='utf-8')))
for f in sorted(glob.glob(os.path.join(d, 'en', 'd_*.json'))):  # Woerterbuecher neuer Maerchen (deutsch -> englisch)
    paare.update(json.load(open(f, encoding='utf-8')))
# Zeichen pruefen, die die Schrift nicht kennt
schlecht = [(k, v) for k, v in paare.items() if any(c in v for c in '’”‘')]
def c(s):  # als C-String (die Quelltexte sind mit Backslash-Escapes geschrieben: so lassen)
    return '"' + s.replace('"', '\\"') + '"'
schl = sorted(paare, key=lambda s: s.encode('utf-8'))
with open(ziel, 'w', encoding='utf-8', newline='\n') as o:
    o.write('// Englische Texte der Maerchenbox - erzeugt aus den deutschen Texten (sortiert fuer die Suche).\n')
    o.write('// Schluessel: der deutsche Text genau so, wie er im Quelltext steht.\n')
    o.write('#include <string.h>\n\nstruct Paar { const char *de, *en; };\nstatic const Paar PAARE[] = {\n')
    for k in schl:
        o.write('  {"' + k + '", ' + c(paare[k]) + '},\n')
    o.write('};\n\n')
    o.write('// Binaere Suche; unbekannte Texte kommen unveraendert zurueck\n')
    o.write('const char *aufEnglisch(const char *de) {\n')
    o.write('  int a = 0, b = (int)(sizeof PAARE / sizeof PAARE[0]) - 1;\n')
    o.write('  while (a <= b) {\n    int m = (a + b) / 2, v = strcmp(de, PAARE[m].de);\n')
    o.write('    if (v == 0) return PAARE[m].en;\n    if (v < 0) b = m - 1; else a = m + 1;\n  }\n  return de;\n}\n')
print('Paare', len(paare), 'fehlen', len(fehlt), fehlt[:5], 'schlechte Zeichen', schlecht[:5])

# Alle angezeigten Texte aus den Maerchen-Dateien ziehen (ohne main.cpp): je Datei eine Liste, ohne Doppelte.
import re, sys, os, json
src = sys.argv[1]
aus = sys.argv[2]
ohne = {'main.cpp', 'gfx.cpp', 'lcd.cpp', 'lcd_rund.cpp', 'synth.cpp', 'schriften.cpp'}
lit = re.compile(r'"((?:[^"\\\n]|\\.)*)"')
gesehen, liste = set(), []
for name in sorted(os.listdir(src)):
    if not (name.endswith('.cpp') or name.endswith('.h') or name.endswith('.inc')) or name in ohne: continue
    for zeile in open(os.path.join(src, name), encoding='utf-8'):
        z = zeile.strip()
        if z.startswith('#include') or z.startswith('//'): continue
        for m in lit.finditer(zeile):
            s = m.group(1)
            if not re.search(r'[A-Za-zÄÖÜäöüß]{2,}', s): continue
            if s in gesehen: continue
            gesehen.add(s)
            liste.append({'datei': name, 'de': s})
json.dump(liste, open(aus, 'w', encoding='utf-8'), ensure_ascii=False, indent=0)
print(len(liste))
from collections import Counter
for d, n in Counter(e['datei'] for e in liste).most_common(): print(d, n)

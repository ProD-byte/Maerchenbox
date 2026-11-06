# Stammzertifikate fuer das Update aus dem Internet als C-Datei (aus dem certifi-Paket von pip)
import sys, re
from pip._vendor import certifi
text = open(certifi.where(), encoding='utf-8').read()
bloecke = re.findall(r'# Label: "([^"]+)"[\s\S]*?(-----BEGIN CERTIFICATE-----[\s\S]*?-----END CERTIFICATE-----)', text)
gesucht = ['ISRG Root X1', 'ISRG Root X2', 'DigiCert Global Root G2', 'USERTrust RSA Certification Authority']
pem = ''
for name, b in bloecke:
    if name in gesucht:
        pem += b + '\n'; print('mit:', name)
with open(sys.argv[1], 'w', encoding='utf-8', newline='\n') as o:
    o.write('// Stammzertifikate, mit denen die Box prueft, dass das Update wirklich von prod-byte.github.io kommt\n')
    o.write('// (GitHub Pages: Let\'s Encrypt -> ISRG Root X1; die anderen als Reserve). Erzeugt mit werkzeuge/wurzeln.py.\n')
    o.write('#pragma once\nstatic const char UPDATE_WURZELN[] =\n')
    for z in pem.strip().split('\n'):
        o.write('  "' + z + '\\n"\n')
    o.write(';\n')

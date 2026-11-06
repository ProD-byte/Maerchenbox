# YouTube-Vorschaubild 1280x720 aus denselben Bausteinen wie der Trailer
import sys, os
sys.path.insert(0, os.path.dirname(__file__))
import schnitt as s
from PIL import ImageDraw

bild = s.himmel(3.0)
s.feenstaub(bild, 1.0, 0.2, 1.5, 1400, 520, 420, 90, seed=12)
s.setze_geraet(bild, s.clipbild('t_rotkaeppchen', 1.2), 560, 560, 860)
s.schreibe(bild, 'Fairy Tale', s.F_TITEL(230), 1440, 210, s.GOLD, 1, ausr='m', leucht=(255, 170, 60))
s.schreibe(bild, 'Box', s.F_TITEL(230), 1440, 420, s.GOLD, 1, ausr='m', leucht=(255, 170, 60))
d = ImageDraw.Draw(bild)
d.rounded_rectangle((1110, 760, 1770, 850), 45, fill=(40, 32, 92, 235), outline=s.GOLD + (230,), width=3)
s.schreibe(bild, '30 tales · round ESP32', s.F_HALB(52), 1440, 772, s.WEISS, 1, ausr='m')
s.schreibe(bild, 'DIY  ·  no coding', s.F_HALB(46), 1440, 890, s.LEISE, 1, ausr='m')
bild.convert('RGB').resize((1280, 720), s.Image.LANCZOS).save(sys.argv[1], quality=92)
print('ok')

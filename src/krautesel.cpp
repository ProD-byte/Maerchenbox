// Der Krautesel - ein Spieluhr-Film fuer die Maerchenbox, nach den Bruedern Grimm (KHM 122, Ausgabe 1857).
// Die alte Frau im Wald, die neun Voegel und der Wunschmantel, das Vogelherz und das Gold unter dem Kissen,
// die Hexe und ihre schoene Tochter, der Granatenberg, die drei Riesen, die Wolke, der Krautgarten mit den
// zwei Sorten Salat, drei Eselinnen beim Mueller - und am Ende die Hochzeit. Harte Stellen nur angedeutet.
#include "erzaehlen.h"

// ---------- Ablauf (Sekunden) ----------
static const float P1 = 0, P2 = 32, P3 = 60, P4 = 84, P5 = 112, P6 = 140, P7 = 164, P8 = 198, P9 = 232, P10 = 250,
                   P11 = 284, P12 = 330, P13 = 366, P14 = 406, PDAUER = 432;

// ---------- Aussehen ----------
static Aussehen jaegerLook(bool braun) {
  Aussehen a; a.weste = FARBE(0x4E7A3A); a.hemd = FARBE(0xF2E8D0); a.hose = FARBE(0x6E5A3A); a.haar = FARBE(0x8E5A2A);
  a.hut = FARBE(0x3E5E2E); a.hutD = FARBE(0x2E4622); a.feder = FARBE(0xE8E4DC);
  if (braun) { a.haut = FARBE(0xA8744A); a.wangen = false; }
  return a;
}
static Aussehen muellerLook() {
  Aussehen a; a.weste = FARBE(0xE8E4DC); a.hemd = FARBE(0xF6F2EA); a.hose = FARBE(0xD8D2C6); a.haar = FARBE(0xE0DCD4);
  a.muetze = 1;
  return a;
}
static const Aussehen JAEGER = jaegerLook(false), JAEGER_BRAUN = jaegerLook(true), MUELLER = muellerLook();
static FrauLook alteLook() {
  FrauLook l; l.kleid = FARBE(0x7A6A5A); l.schuerze = FARBE(0xA8987A); l.haar = FARBE(0xC8C8C8); l.frisur = 3;
  l.tuch = FARBE(0x8E6A4A); l.wangen = false; l.flicken = true;
  return l;
}
static FrauLook hexeLook() {
  FrauLook l; l.kleid = FARBE(0x4E3A6E); l.schuerze = FARBE(0x6E5A8E); l.haar = FARBE(0xBDBDBD); l.frisur = 3;
  l.tuch = FARBE(0x3A2A4E); l.wangen = false; l.haut = FARBE(0xD8D8B0);
  return l;
}
static FrauLook tochterLook() {
  FrauLook l; l.kleid = FARBE(0xB83A6A); l.haar = FARBE(0x6A3A22); l.frisur = 0; l.blume = FARBE(0xFFD54A); l.glanz = true;
  return l;
}
static FrauLook brautLook() {
  FrauLook l = tochterLook(); l.kleid = FARBE(0xFFF6F0); l.blume = FARBE(0xFF8FB0); l.rockBreite = 1.3f;
  return l;
}
static FrauLook magdLook() {
  FrauLook l; l.kleid = FARBE(0x6E8EB8); l.schuerze = WEISS; l.haar = FARBE(0xC98A3A); l.frisur = 3; l.tuch = WEISS;
  return l;
}
static const FrauLook ALTE = alteLook(), HEXE = hexeLook(), TOCHTER = tochterLook(), BRAUT = brautLook(), MAGD = magdLook();

static const Farbe MANTEL = FARBE(0x3A4AA8), MANTEL_H = FARBE(0x5A6ED0);

// =====================================================================
// Dinge
// =====================================================================
static void goldstueck(Gfx &g, float x, float y, float s, float t, uint8_t a = 255) {
  g.kontur(1.2f * s, FARBE(0x9A6A1E));
  g.kreis(x, y, 6 * s, GOLD, a);
  g.konturAus();
  g.kreis(x - 1.5f * s, y - 1.5f * s, 2.6f * s, FARBE(0xFFF0A0), a);
  g.stern(x + 3 * s, y - 3 * s, 4 * s, 1.2f * s, 4, t * 2, WEISS, (uint8_t)(a * (0.5f + 0.5f * sinf(t * 3))));
}

// der Wunschmantel, zusammengelegt oder flatternd (w = Wehen)
static void mantelTuch(Gfx &g, float x, float y, float s, float w, uint8_t a = 255) {
  float m[] = {x - 18 * s, y - 6 * s + w * 3, x + 16 * s, y - 9 * s - w * 2, x + 20 * s, y + 6 * s + w, x - 16 * s, y + 8 * s - w * 2};
  g.kontur(1.3f * s, OUT);
  g.polygon(m, 4, MANTEL, a);
  g.konturAus();
  g.linie(x - 14 * s, y + 4 * s, x + 16 * s, y + 2 * s, 2 * s, MANTEL_H, a);
  for (int i = 0; i < 4; i++) g.stern(x - 10 * s + i * 8 * s, y - 1 * s + (i & 1) * 3 * s, 2.4f * s, 1 * s, 5, 0, GOLD, a);
}

// der Mantel um die Schultern einer Figur (hinter ihr gezeichnet)
static void mantelUm(Gfx &g, float x, float y, float s, float dir, float t, float k, float wehen = 1) {
  if (k <= 0) return;
  auto PX = [&](float d) { return x + d * s * dir; };
  auto PY = [&](float d) { return y + d * s; };
  float w = sinf(t * 6) * 4 * wehen;
  uint8_t a = (uint8_t)(255 * klemme(k));
  float m[] = {PX(-9), PY(-46), PX(9), PY(-46), PX(12), PY(-10), PX(-26 - w * 2), PY(-8 + w), PX(-30 - w * 3), PY(-20)};
  g.kontur(1.3f * s, OUT);
  g.polygon(m, 5, MANTEL, a);
  g.konturAus();
  for (int i = 0; i < 3; i++) g.stern(PX(-14 - i * 5), PY(-34 + i * 8), 2.4f * s, 1 * s, 5, 0, GOLD, a);
  g.ellipse(PX(0), PY(-45), 11 * s, 3 * s, MANTEL_H, a);
}

static void flinte(Gfx &g, float hx, float hy, float s, float dir, float w) {
  float c = cosf(w) * dir, sn = sinf(w);
  g.linie(hx - c * 6 * s, hy + sn * 6 * s, hx + c * 30 * s, hy - sn * 30 * s, 2.4f * s, FARBE(0x3A3A44));
  g.linie(hx - c * 16 * s, hy + sn * 16 * s, hx - c * 2 * s, hy + sn * 2 * s, 4.8f * s, HOLZ_D);
}

// der Jaeger. was: 0 nichts, 1 Flinte geschultert, 2 Flinte angelegt (schraeg nach oben), 3 Salat im Arm, 4 Goldstueck
static void jaeger(Gfx &g, float x, float y, float s, float dir, float t, float lauf, float mund, int was = 1,
                   float mantel = 0, bool braun = false, bool zu = false, float armV = -1) {
  mantelUm(g, x, y, s, dir, t, mantel, lauf >= 0 ? 1.6f : 0.6f);
  Pose p = posis(braun ? JAEGER_BRAUN : JAEGER, t);
  p.lauf = lauf;
  p.mund = mund;
  p.zu = zu || p.zu;
  p.guertel = true;
  p.armV = armV >= 0 ? armV : was == 2 ? 1.9f : was == 3 ? 1.3f : was == 4 ? 1.5f : 0.5f;
  float hx, hy;
  schneiderlein(g, x, y, s, dir, p, &hx, &hy);
  if (was == 1) flinte(g, hx, hy, s, dir, 1.25f);
  if (was == 2) flinte(g, hx, hy, s, dir, 0.75f);
  if (was == 4) goldstueck(g, hx + 3 * s * dir, hy - 3 * s, s, t);
}

// Salat: gut = macht wieder zum Menschen (hellgruen), sonst die boese Sorte (blaugruen)
static void salat(Gfx &g, float x, float y, float s, bool gut, float angebissen = 0) {
  Farbe a = gut ? FARBE(0x8EDB5A) : FARBE(0x4E8E7A), b = gut ? FARBE(0xC8F08A) : FARBE(0x7EB8A8);
  g.kontur(1.1f * s, OUT);
  g.ellipse(x - 9 * s, y - 4 * s, 7 * s, 4 * s, a, 255, -0.4f);
  g.ellipse(x + 9 * s, y - 4 * s, 7 * s, 4 * s, a, 255, 0.4f);
  g.kreis(x, y - 8 * s, 10 * s, a);
  g.konturAus();
  g.kreis(x - 3 * s, y - 10 * s, 5 * s, b);
  g.kreis(x + 4 * s, y - 8 * s, 4 * s, b);
  g.bogen(x, y - 8 * s, 6 * s, 3.6f, 5.6f, 1 * s, a);
  if (angebissen > 0) g.kreis(x + 8 * s, y - 14 * s, 6 * angebissen * s, FARBE(0x6E5A3A));
}

// =====================================================================
// Figuren
// =====================================================================
// alte Frau (gebueckt, mit Stock); hexe = mit langer Nase und Warze
static void alte(Gfx &g, float x, float y, float s, float dir, float t, float mund, float armV, bool hexe,
                 float boese = 0) {
  const FrauLook &L = hexe ? HEXE : ALTE;
  FrauPose p = fposis(L, t, 3);
  p.mund = mund;
  p.armV = armV;
  p.boese = boese;
  frau(g, x, y, s, dir, p);
  g.kontur(1, OUT);
  g.ellipse(x + 10 * s * dir, y - 70 * s, hexe ? 5 * s : 3.4f * s, hexe ? 3.4f * s : 2.6f * s, L.haut, 255, 0.5f * dir);
  g.konturAus();
  if (hexe) g.kreis(x + 12 * s * dir, y - 72 * s, 1.2f * s, FARBE(0x6E5A3A));
  g.linie(x - 14 * s * dir, y - 40 * s, x - 18 * s * dir, y, 2.4f * s, HOLZ_D);
  g.bogen(x - 12 * s * dir, y - 42 * s, 4 * s, 3.2f, 6.2f, 2.4f * s, HOLZ_D);
}

static void tochter(Gfx &g, float x, float y, float s, float dir, float t, float mund, float armV = 0.4f,
                    bool weint = false, float armH = 0.2f, const FrauLook *L = &TOCHTER) {
  FrauPose p = fposis(*L, t, 1);
  p.mund = mund;
  p.armV = armV;
  p.armH = armH;
  p.weint = weint;
  frau(g, x, y, s, dir, p);
}

static void magd(Gfx &g, float x, float y, float s, float dir, float t, float mund, float armV = 0.4f, float lauf = -1) {
  FrauPose p = fposis(MAGD, t, 2);
  p.mund = mund;
  p.armV = armV;
  p.lauf = lauf;
  frau(g, x, y, s, dir, p);
}

// Esel (wie bei den Stadtmusikanten); tuch: Erkennungsfarbe am Hals, senk = Kopf zum Grasen gesenkt
static void esel(Gfx &g, float x, float y, float s, float dir, float t, float lauf = -1, float mund = 0, float senk = 0,
                 Farbe tuch = 0, bool blume = false, float traurig = 0) {
  const Farbe fell = FARBE(0x9A9AA2), hell = FARBE(0xD6D6DC), dunkel = FARBE(0x6E6E78);
  auto PX = [&](float d) { return x + d * s * dir; };
  auto PY = [&](float d) { return y + d * s; };
  g.kontur(1.4f * s, OUT);
  for (int i = 0; i < 4; i++) {
    float bx = i < 2 ? -20 + i * 9 : 14 + (i - 2) * 9;
    float sw = lauf >= 0 ? sinf(lauf * 6.2832f + i * 1.6f) * 8 : 0;
    g.linie(PX(bx), PY(-30), PX(bx + sw), PY(-4), 6 * s, i & 1 ? fell : dunkel);
    g.ellipse(PX(bx + sw + 1), PY(-3), 4.5f * s, 3 * s, FARBE(0x3A3436));
  }
  float sch = sinf(t * 3) * 3;
  g.kurve(PX(-26), PY(-38), PX(-36), PY(-34 + sch), PX(-38), PY(-22), 3 * s, fell);
  g.kreis(PX(-38), PY(-20), 3.5f * s, dunkel);
  g.ellipse(PX(0), PY(-40), 30 * s, 14 * s, fell);
  float hx = PX(24 + senk * 8), hy = PY(-56 + senk * 34);
  g.linie(PX(18), PY(-44), hx, hy, 12 * s, fell);
  float oh = traurig * 14;  // haengende Ohren
  float o1[] = {hx - 4 * s * dir, hy - 8 * s, hx - (10 + oh) * s * dir, hy - (34 - oh * 1.6f) * s, hx + 2 * s * dir, hy - 10 * s};
  float o2[] = {hx + 2 * s * dir, hy - 9 * s, hx + (4 - oh) * s * dir, hy - (36 - oh * 1.6f) * s, hx + 9 * s * dir, hy - 8 * s};
  g.polygon(o1, 3, fell);
  g.polygon(o2, 3, fell);
  g.ellipse(hx + 8 * s * dir, hy + 2 * s, 15 * s, 9 * s, fell, 255, (0.35f + senk * 0.6f) * dir);
  g.konturAus();
  g.ellipse(hx + 17 * s * dir, hy + 6 * s + senk * 4 * s, 7 * s, 5.5f * s, hell, 255, 0.35f * dir);
  g.kreis(hx + 20 * s * dir, hy + 4 * s + senk * 4 * s, 1.3f * s, OUT);
  if (mund > 0.05f) g.ellipse(hx + 17 * s * dir, hy + 10 * s + senk * 4 * s, 5 * s, (1 + mund * 4) * s, FARBE(0x7A2A2A));
  if (traurig > 0.5f) g.bogen(hx + 6 * s * dir, hy - 1 * s, 2.4f * s, 3.5f, 5.9f, 1.4f * s, OUT);
  else {
    g.kreis(hx + 6 * s * dir, hy - 2 * s, 2.6f * s, SCHWARZ);
    g.kreis(hx + 7 * s * dir, hy - 3 * s, 0.9f * s, WEISS);
  }
  for (int i = 0; i < 5; i++) g.kreis(PX(12 + i * 2.5f + senk * i), PY(-50 - i * 3 + senk * i * 5), 3 * s, dunkel);
  if (tuch) {
    g.kontur(1 * s, OUT);
    g.ellipse(PX(19 + senk * 3), PY(-46 + senk * 10), 6 * s, 9 * s, tuch, 255, 0.4f * dir);
    g.konturAus();
  }
  if (blume) {
    for (int i = 0; i < 5; i++) g.kreis(hx - 2 * s * dir + cosf(i * 1.256f) * 3 * s, hy - 10 * s + sinf(i * 1.256f) * 3 * s, 2.2f * s, FARBE(0xFF8FB0));
    g.kreis(hx - 2 * s * dir, hy - 10 * s, 1.6f * s, GOLD);
  }
}
// die drei Eselinnen: 0 die Alte (violettes Tuch), 1 die Magd (weisses Tuch), 2 die Tochter (Bluemchen)
static void eselin(Gfx &g, int wer, float x, float y, float s, float dir, float t, float lauf = -1, float senk = 0,
                   float traurig = 0) {
  esel(g, x, y, s, dir, t, lauf, 0, senk, wer == 0 ? HEXE.kleid : wer == 1 ? WEISS : 0, wer == 2, traurig);
}

// =====================================================================
// Orte
// =====================================================================
static void wald(Gfx &g, float t, float scroll = 0) {
  himmel(g, 0, t, 200);
  sonne(g, 236, 34, 11, t);
  wolke(g, fmodf(60 - scroll * 0.1f + 600, 400) - 60, 52, 0.8f);
  for (int i = 0; i < 7; i++) {
    float x = fmodf(i * 52 - scroll * 0.6f + 1040, 364) - 40;
    if (i & 1) tanne(g, x, 204, 0.9f + (i % 3) * 0.12f);
    else laubbaum(g, x, 204, 0.7f + (i % 3) * 0.1f);
  }
  boden(g, 200);
  g.rechteck(0, 210, 284, 12, FARBE(0xC9B48A));
  blumen(g, scroll, 230, 6, 7);
}

// Schlafstube daheim (Morgensonne im Fenster)
static void stube(Gfx &g, float t) {
  g.verlauf(0, 200, FARBE(0xE8D0A8), FARBE(0xC9A878));
  g.rechteck(0, 200, 284, 40, FARBE(0x8E6A46));
  for (int i = 0; i < 7; i++) g.linie(i * 46, 200, i * 46 - 20, 240, 1.4f, FARBE(0x7A5A3A));
  g.kontur(1.8f, OUT);
  g.rundRechteck(170, 50, 70, 64, 4, HOLZ);
  g.konturAus();
  g.rundRechteck(175, 55, 60, 54, 2, FARBE(0xFFE8A0));
  sonne(g, 205, 82, 10, t);
  g.rechteck(203, 55, 4, 54, HOLZ);
  g.kreis(205, 82, 70, FARBE(0xFFE8A0), 40);
}
static void bett(Gfx &g, float kissenHoch) {
  g.kontur(1.6f, OUT);
  g.rundRechteck(40, 160, 130, 44, 4, HOLZ);
  g.rundRechteck(30, 130, 14, 76, 4, HOLZ_D);
  g.konturAus();
  g.rundRechteck(44, 156, 122, 14, 6, FARBE(0xE8EEF8));
  g.rundRechteck(80, 150, 86, 18, 6, FARBE(0x7EA0D8));
  g.kontur(1.2f, OUT);
  g.rundRechteck(46, 140 - kissenHoch * 26, 36, 18, 8, WEISS);
  g.konturAus();
}

// Schloss von vorn mit grossem Fenster (fuer die Hexe und die Tochter) und Tor
static void schlossFront(Gfx &g, float t, float tor = 0, float nacht = 0) {
  himmel(g, nacht, t, 200);
  if (nacht < 0.5f) sonne(g, 40, 36, 10, t);
  boden(g, 200);
  g.rechteck(0, 210, 284, 12, FARBE(0xC9B48A));
  Farbe mauer = mischeFarbe(FARBE(0xE8D8C0), FARBE(0x8E86B8), nacht * 0.6f);
  g.kontur(1.8f, OUT);
  g.rechteck(130, 60, 160, 150, mauer);
  g.rechteck(120, 30, 40, 180, mauer);
  float d[] = {112, 32, 140, -6, 168, 32};
  g.polygon(d, 3, FARBE(0x8E3A5A));
  g.konturAus();
  for (int i = 0; i < 6; i++) g.rechteck(132 + i * 26, 50, 14, 12, mauer);
  // das grosse Fenster
  g.kontur(1.6f, OUT);
  g.rundRechteck(176, 76, 84, 60, 10, FARBE(0x3A2E4E));
  g.konturAus();
  g.kreis(218, 106, 50, FARBE(0xFFD27A), 26);
  // Tor
  g.kontur(1.6f, OUT);
  g.rundRechteck(138, 152, 34, 58, 14, FARBE(0x2E2620));
  g.konturAus();
  if (tor < 1) {
    float w = 34 * (1 - tor);
    g.rundRechteck(138, 152, w, 58, 12, HOLZ_D);
    for (int i = 1; i < 3; i++) g.linie(138 + w * i / 3, 156, 138 + w * i / 3, 208, 1.2f, OUT);
  }
}
static void fensterbrett(Gfx &g) {
  g.kontur(1.4f, OUT);
  g.rechteck(170, 134, 96, 9, FARBE(0xC8B8A0));
  g.konturAus();
}

// Saal im Schloss: Fenster mit Blick auf den Granatenberg, Herd mit Kessel
static void saal(Gfx &g, float t, float kessel = 0) {
  g.verlauf(0, 200, FARBE(0xCFC2D8), FARBE(0xA898B8));
  for (int r = 0; r < 6; r++)
    for (int c = 0; c < 8; c++) g.rechteck(c * 40 + (r & 1) * 20 - 20, r * 34, 38, 32, FARBE(0xB8AAC8), 90);
  g.rechteck(0, 200, 284, 40, FARBE(0x8E7A6A));
  for (int c = 0; c < 9; c++) if (c & 1) g.rechteck(c * 34, 200, 34, 40, FARBE(0x7A6858));
  // Fenster
  g.kontur(1.8f, OUT);
  g.rundRechteck(26, 46, 96, 82, 30, FARBE(0xA8D8F8));
  g.konturAus();
  float b[] = {40, 128, 66, 76, 84, 98, 100, 80, 112, 128};
  g.polygon(b, 5, FARBE(0x8E5A6A));
  for (int i = 0; i < 5; i++) g.stern(62 + i * 9, 100 + (i & 1) * 8, 2.6f, 1, 4, t + i, FARBE(0xE8283F));
  g.rechteck(72, 46, 4, 82, HOLZ_D);
  g.rechteck(26, 88, 96, 4, HOLZ_D);
  // Vorhang
  g.ellipse(26, 90, 10, 46, FARBE(0x8E2A4A));
  g.ellipse(122, 90, 10, 46, FARBE(0x8E2A4A));
  // Herd mit Kessel
  g.kontur(1.8f, OUT);
  g.rundRechteck(196, 120, 76, 82, 6, FARBE(0x9A8A86));
  g.konturAus();
  g.rundRechteck(206, 150, 56, 50, 18, FARBE(0x2E2620));
  for (int i = 0; i < 3; i++) {
    float h = 14 + 5 * sinf(t * 9 + i * 1.7f);
    float f[] = {218 + i * 16 - 7, 198, 218 + i * 16 + 7, 198, 218 + i * 16 + sinf(t * 7 + i) * 2, 198 - h};
    g.polygon(f, 3, FARBE(0xFF8A2A));
  }
  if (kessel > 0) {
    g.kontur(1.6f, OUT);
    g.ellipse(234, 150, 26, 18, FARBE(0x3A3A44));
    g.konturAus();
    g.ellipse(234, 136, 22, 5, FARBE(0x6EE07A));
    for (int i = 0; i < 4; i++) {
      float k = fmodf(t * 0.7f + i * 0.25f, 1);
      g.kreis(222 + i * 8 + sinf(t * 3 + i) * 4, 132 - k * 40, 3 + k * 4, FARBE(0x9AF0A0), (uint8_t)(200 * (1 - k)));
    }
  }
}

static void granatenberg(Gfx &g, float t, float nacht = 0) {
  himmel(g, nacht, t, 200);
  wolke(g, 40, 60, 1.1f);
  wolke(g, 220, 40, 0.8f);
  float b[] = {-10, 210, 60, 90, 120, 60, 170, 40, 230, 80, 294, 210};
  g.kontur(1.8f, OUT);
  g.polygon(b, 6, FARBE(0x8E5A6A));
  g.konturAus();
  float h[] = {120, 60, 170, 40, 150, 90, 110, 110};
  g.polygon(h, 4, FARBE(0xA87A88));
  for (int i = 0; i < 22; i++) {
    float x = 30 + zufall(i + 40) * 230, y = 110 + zufall(i + 80) * 96;
    if (y < 210 - fabsf(x - 150) * 0.2f) {
      g.stern(x, y, 4, 1.8f, 5, i, FARBE(0xE8283F));
      g.stern(x, y, 2, 1, 4, t * 2 + i, WEISS, (uint8_t)(160 + 90 * sinf(t * 3 + i)));
    }
  }
  g.rechteck(0, 210, 284, 30, FARBE(0x7A4E5A));
}

static void krautgarten(Gfx &g, float t, float gegessen1 = 0, float gegessen2 = 0) {
  himmel(g, 0, t, 200);
  sonne(g, 230, 32, 11, t);
  // Mauer
  g.kontur(1.6f, OUT);
  g.rechteck(-4, 120, 292, 84, FARBE(0xC8A88A));
  g.konturAus();
  for (int r = 0; r < 4; r++)
    for (int c = 0; c < 10; c++) g.linie(c * 32 + (r & 1) * 16, 124 + r * 20, c * 32 + (r & 1) * 16 + 28, 124 + r * 20, 1.2f, FARBE(0xA8886A));
  g.rechteck(0, 196, 284, 44, FARBE(0x7A5A3A));
  // zwei Beete: links die boese Sorte, rechts die gute
  for (int i = 0; i < 4; i++) {
    if (!(i == 2 && gegessen1 > 0)) salat(g, 42 + i * 22, 214 + (i & 1) * 4, 0.8f, false, i == 2 ? gegessen1 : 0);
    if (!(i == 1 && gegessen2 > 0)) salat(g, 172 + i * 22, 214 + (i & 1) * 4, 0.8f, true, i == 1 ? gegessen2 : 0);
  }
}

static void muehle(Gfx &g, float t) {
  himmel(g, 0, t, 200);
  sonne(g, 40, 34, 10, t);
  boden(g, 200);
  g.kontur(1.8f, OUT);
  g.rechteck(150, 90, 110, 112, FARBE(0xF0DCC0));
  float d[] = {140, 92, 205, 46, 270, 92};
  g.polygon(d, 3, FARBE(0xA8452E));
  g.konturAus();
  g.rundRechteck(170, 150, 24, 52, 8, HOLZ_D);
  g.rundRechteck(214, 110, 26, 24, 3, FARBE(0x9FD8FF));
  // Wasserrad
  g.rechteck(250, 196, 40, 14, FARBE(0x5AA8E8));
  g.kontur(1.6f, OUT);
  g.kreis(262, 160, 34, HOLZ, 0);
  g.konturAus();
  g.bogen(262, 160, 32, 0, 6.29f, 4, HOLZ);
  for (int i = 0; i < 8; i++) {
    float a = t * 1.2f + i * 0.785f;
    g.linie(262, 160, 262 + cosf(a) * 34, 160 + sinf(a) * 34, 3, HOLZ_D);
  }
  g.kreis(262, 160, 5, HOLZ_D);
  for (int i = 0; i < 5; i++) tropfen(g, 230 + i * 7, 196 + fmodf(t * 30 + i * 9, 10), 2, FARBE(0xBFE6FF));
  g.rechteck(0, 210, 284, 12, FARBE(0xC9B48A));
}

// =====================================================================
// Szenen
// =====================================================================
// 1) Die alte Frau im Wald bittet um ein Almosen und gibt guten Rat
static void szeneAlte(Gfx &g, float u) {
  float k = seg(u, 0, 5);
  wald(g, u, k * 60);
  float jx = lerp(30, 104, k);
  bool gegeben = u > 12.4f;
  alte(g, 190, 222, 0.85f, -1, u, (u > 6.4f && u < 11) || u > 13 ? 0.4f + 0.2f * sinf(u * 9) : 0,
       u > 6.4f && u < 12.8f ? 1.5f : u > 13 ? 1.1f + 0.3f * sinf(u * 2) : 0.4f, false);
  float gold = seg(u, 11.2f, 12.4f);
  jaeger(g, jx, 222, 0.95f, 1, u, k < 1 ? fmodf(u * 1.5f, 1) : -1, u < 5 ? 0.3f : 0, u > 10.8f && u < 12.6f ? 4 : 1);
  if (gold > 0 && gold < 1) goldstueck(g, lerp(122, 168, gold), 172 - huepf(gold) * 16, 0.9f, u);
  if (u < 5) for (int i = 0; i < 3; i++) {  // pfeift
    float ph = fmodf(u * 0.6f + i * 0.33f, 1);
    notenzeichen(g, jx + 18 + ph * 20, 140 - ph * 30, OUT, (uint8_t)(220 * huepf(ph)));
  }
  if (gegeben && u < 15) herzen(g, 190, 150, u, 3, 14);
  g.kameraAus();
  vers(g, {"Ein junger Jäger ging lustig", "pfeifend durch den Wald."}, aufzu(u, 0.4f, 6));
  blase(g, 200, 50, {"Guter Jäger, ich leide Hunger", "und Durst. Gib mir doch", "ein Almosen!"}, aufzu(u, 6.4f, 11), 196, 140);
  blase(g, 190, 50, {"Weil du so gut bist, hör zu:", "Auf einem Baum zanken sich", "neun Vögel um einen Mantel."}, aufzu(u, 13, 19.4f), 196, 140);
  blase(g, 190, 50, {"Schieß mitten hinein! Es ist", "ein Wunschmantel - er trägt", "dich, wohin du willst."}, aufzu(u, 19.8f, 25.4f), 196, 140);
  blase(g, 182, 50, {"Und schluck das Herz des Vogels:", "Dann liegt jeden Morgen ein", "Goldstück unter deinem Kissen."}, aufzu(u, 25.8f, 31.6f), 196, 140);
}

// 2) Die neun Voegel, der Schuss, der Mantel faellt
static void szeneVoegel(Gfx &g, float u) {
  wald(g, u, 60);
  laubbaum(g, 196, 206, 1.25f, FARBE(0x3E9A4E));
  bool schuss = u > 10.5f;
  float f = seg(u, 10.5f, 12.5f);
  // der Mantel zwischen den Voegeln
  float mx = 196 + (schuss ? 0 : sinf(u * 5) * 8), my = schuss ? lerp(136, 214, hinein(f)) : 136 + sinf(u * 7) * 3;
  if (!(schuss && u > 14)) mantelTuch(g, mx, my, 1, sinf(u * 9), 255);
  for (int i = 0; i < 9; i++) {
    float a = i * 0.698f + u * (schuss ? 0 : 3);
    float r = schuss ? 30 + f * 180 : 24 + 6 * sinf(u * 4 + i);
    float vx = mx + cosf(a) * r * 1.3f, vy = (schuss ? 136 : my) + sinf(a) * r * 0.6f - (schuss ? f * 60 : 0);
    if (i == 4 && schuss) continue;  // der eine Vogel faellt (nur angedeutet)
    vogel(g, vx, vy, 1.1f, u + i, cosf(a + 1.57f) > 0 ? 1 : -1);
  }
  if (schuss && u < 13) for (int i = 0; i < 6; i++)
    g.ellipse(196 + cosf(i * 1.05f) * f * 40, 136 + sinf(i * 1.05f) * f * 30 + f * 40, 3, 1.4f, FARBE(0xFFE070), (uint8_t)(255 * (1 - f)), i);
  bool hebt = u > 14;
  jaeger(g, hebt ? lerp(96, 170, seg(u, 14, 16)) : 96, 222, 0.95f, 1, u, hebt && u < 16 ? fmodf(u * 1.5f, 1) : -1,
         u > 0.4f && u < 5 ? 0.4f : 0, u > 7 && u < 12 ? 2 : 1);
  if (u > 10.5f && u < 11) g.kreis(150, 120, 8, WEISS, 220);  // Muendungsblitz
  if (u > 10.5f) wolkePuff(g, 150, 120, seg(u, 10.5f, 12), WEISS, 0.6f);
  // das Herz verschlucken: ein leuchtendes Herz schwebt zu ihm (angedeutet)
  float hz = seg(u, 17, 20);
  if (hz > 0 && hz < 1) {
    herz(g, lerp(196, 176, hz), lerp(214, 160, hz), 6 * (1 - hz * 0.6f), FARBE(0xE8283F), (uint8_t)(255 * (1 - hz * 0.7f)));
    glitzern(g, lerp(196, 176, hz), lerp(214, 160, hz), 14, u, 4);
  }
  if (u > 16) mantelTuch(g, 166, 184, 0.7f, 0);  // traegt den Mantel ueber dem Arm
  g.kameraAus();
  blase(g, 110, 50, {"Schöne Dinge verspricht sie -", "wenn's nur auch wahr wird!"}, aufzu(u, 0.4f, 5), 100, 130);
  vers(g, {"Bald sah er den Baum: Neun Vögel", "zerrten kreischend an einem Mantel."}, aufzu(u, 5.4f, 10.4f));
  vers(g, {"Der Mantel fiel herab, ein Vogel", "dazu. Wie die Alte gesagt hatte,", "schluckte er das Vogelherz."}, aufzu(u, 13, 22.4f));
  blase(g, 150, 60, {"Jetzt bin ich gespannt", "auf morgen früh!"}, aufzu(u, 22.8f, 27.4f), 170, 130);
  blende(g, u, 28, 170, 170);
}

// 3) Jeden Morgen ein Goldstueck unter dem Kissen
static void szeneGold(Gfx &g, float u) {
  if (u < 18) {
    stube(g, u);
    float hoch = u > 4 ? sanft(seg(u, 4, 5.4f)) : 0;
    bett(g, hoch);
    if (hoch > 0.5f) goldstueck(g, 64, 152, 1.2f, u);
    // der Haufen waechst Morgen fuer Morgen
    int n = (int)(seg(u, 9, 15) * 24);
    for (int i = 0; i < n; i++) goldstueck(g, 214 + (i % 6) * 9 - (i / 6) * 4 + (i / 6) * 4, 214 - (i / 6) * 8, 0.9f, u + i);
    if (u > 9 && u < 15) {
      char b[24];
      snprintf(b, sizeof b, "%d", 1 + (int)(seg(u, 9, 15) * 99));
      g.text(SCHRIFT_K, b, 238, 176, FARBE(0x7A4E1A), 1);
    }
    jaeger(g, 120, 222, 0.95f, 1, u, -1, hoch > 0.5f ? 0.8f : 0, 0, 0, false, u < 3, hoch > 0 && u < 9 ? 1.6f : -1);
    g.kameraAus();
    vers(g, {"Am Morgen hob er das Kissen -", "da lag wirklich ein Goldstück!", "Und so jeden Morgen."}, aufzu(u, 0.4f, 9));
    blase(g, 140, 60, {"Was hilft mir all mein Gold,", "wenn ich daheim bleibe?"}, aufzu(u, 11.6f, 17.6f), 130, 130);
    return;
  }
  float v = u - 18;
  wald(g, u, 200 + v * 30);
  jaeger(g, 130, 222, 0.95f, 1, u, fmodf(u * 1.5f, 1), 0.3f, 1);
  mantelTuch(g, 118, 180, 0.6f, 0);
  g.kameraAus();
  vers(g, {"Er nahm Abschied von den", "Eltern und zog in die Welt."}, aufzu(v, 0.2f, 6));
  blende(g, u, 24, 130, 170);
}

// 4) Das Schloss: die Hexe und ihr Herzenstoechterchen am Fenster
static void szeneSchloss(Gfx &g, float u) {
  schlossFront(g, u, seg(u, 20, 22));
  float k = seg(u, 0, 7);
  bool drin = u > 20;
  // die beiden im Fenster
  if (!drin || u < 23) {
    alte(g, 198, 160, 0.62f, -1, u, u > 8.4f && u < 20 ? 0.4f + 0.3f * sinf(u * 9) : 0, u > 8.4f && u < 14 ? 1.6f : 0.4f, true,
         u > 14 ? 0.8f : 0.3f);
    tochter(g, 238, 160, 0.62f, -1, u, 0, 0.4f, u > 14.4f && u < 20);
    fensterbrett(g);
  }
  float jx = drin ? lerp(110, 156, seg(u, 22, 25)) : lerp(-20, 110, k);
  if (u < 25) jaeger(g, jx, 222, 0.9f, 1, u, (k < 1 || (u > 22 && u < 25)) ? fmodf(u * 1.5f, 1) : -1, 0, 1, 0);
  if (u > 22 && u < 27) {
    tochter(g, 186, 222, 0.85f, -1, u, 0.4f, 1.4f + sinf(u * 4) * 0.3f);
    herzen(g, 170, 160, u, 4, 18);
  }
  g.kameraAus();
  vers(g, {"Er kam an ein prächtiges Schloss.", "Aus dem Fenster schauten eine", "Alte und ein schönes Mädchen."}, aufzu(u, 0.4f, 8));
  blase(g, 150, 50, {"Da kommt einer, der hat ein", "Vogelherz im Leib, mein", "Herzenstöchterchen!"}, aufzu(u, 8.4f, 14), 196, 120);
  blase(g, 150, 50, {"Das müssen wir ihm ablocken.", "Gehorch mir, sonst wehe dir!"}, aufzu(u, 14.4f, 20), 196, 120);
  vers(g, {"Der Jäger wurde freundlich", "empfangen und verliebte sich", "in das schöne Mädchen."}, aufzu(u, 21, 27.6f));
}

// 5) Der Trank: das Vogelherz geht verloren (nur angedeutet)
static void szeneTrank(Gfx &g, float u) {
  saal(g, u, 1);
  alte(g, 196, 222, 0.85f, -1, u, u > 21.4f ? 0.5f + 0.3f * sinf(u * 10) : 0,
       u < 7 ? 1.5f + 0.3f * sinf(u * 5) : u > 22 ? 1.8f : 0.4f, true, 0.6f);
  if (u > 22) goldstueck(g, 180, 150, 1, u);
  float trinkt = seg(u, 11.5f, 13);
  // das Maedchen reicht den Becher
  tochter(g, 132, 222, 0.85f, -1, u, u > 7.4f && u < 11.5f ? 0.4f : 0, u > 5 && u < 12 ? 1.3f : 0.4f, false, 0.2f);
  float bx = lerp(116, 86, trinkt), by = lerp(168, 150, trinkt);
  if (u > 5 && u < 14) {
    g.kontur(1, OUT);
    g.rundRechteck(bx - 5, by - 8, 10, 12, 3, GOLD);
    g.konturAus();
    g.ellipse(bx, by - 8, 4, 1.5f, FARBE(0x6EE07A));
  }
  bool verliebt = u > 13;
  jaeger(g, 70, 222, 0.95f, 1, u, -1, verliebt ? 0.2f : 0.3f, 0, 0, false, u > 12 && u < 15, trinkt > 0 && u < 13.4f ? 2.4f : -1);
  // das Herz kommt heraus und schwebt zum Maedchen
  float hz = seg(u, 13.4f, 17);
  if (hz > 0 && hz < 1) {
    float x = lerp(84, 140, hz), y = lerp(150, 150, hz) - huepf(hz) * 30;
    herz(g, x, y, 6, FARBE(0xE8283F), 255);
    glitzern(g, x, y, 14, u, 4);
  }
  if (verliebt) herzen(g, 70, 140, u, 3, 10);
  g.kameraAus();
  vers(g, {"Die Alte braute einen Trank,", "und das Mädchen reichte ihn ihm."}, aufzu(u, 0.4f, 7));
  blase(g, 150, 50, {"Nun, mein Liebster,", "trink mir zu!"}, aufzu(u, 7.4f, 11.5f), 136, 140);
  vers(g, {"Da kam das Vogelherz heraus.", "Das Mädchen musste es heimlich", "selbst verschlucken."}, aufzu(u, 13.4f, 21));
  vers(g, {"Nun lag das Gold unter ihrem", "Kissen - und die Alte holte es", "sich jeden Morgen."}, aufzu(u, 21.4f, 27.8f));
}

// 6) Der Wunschmantel soll auch noch her
static void szeneMantel(Gfx &g, float u) {
  saal(g, u, 0);
  bool jaegerDa = u > 14;
  alte(g, jaegerDa ? lerp(196, 330, seg(u, 14, 17)) : 196, 222, 0.85f, -1, u,
       (u > 0.4f && u < 5) || (u > 10.8f && u < 14.5f) ? 0.5f + 0.3f * sinf(u * 9) : 0, u > 10.8f && u < 14.5f ? 2.6f : 0.8f,
       true, u > 10.8f ? 1 : 0.5f);
  // das Maedchen am Fenster, traurig
  float tx = jaegerDa ? 96 : 132;
  tochter(g, tx, 222, 0.85f, jaegerDa ? 1 : -1, u, u > 5.4f && u < 10.5f ? 0.4f : 0, 0.4f, jaegerDa || (u > 10.8f && u < 14));
  if (jaegerDa) {
    float k = seg(u, 14, 17);
    jaeger(g, lerp(300, 180, k), 222, 0.95f, -1, u, k < 1 ? fmodf(u * 1.5f, 1) : -1, u > 15.4f && u < 19 ? 0.4f : 0, 0, 0);
  }
  g.kameraAus();
  blase(g, 200, 50, {"Das Vogelherz haben wir.", "Jetzt noch den Wunschmantel!"}, aufzu(u, 0.4f, 5), 196, 140);
  blase(g, 120, 50, {"Lass ihm doch den Mantel,", "er hat schon sein Gold verloren."}, aufzu(u, 5.4f, 10.5f), 132, 140);
  blase(g, 200, 50, {"So ein Mantel ist selten!", "Ich muss ihn haben!"}, aufzu(u, 10.8f, 14.5f), 196, 140);
  blase(g, 180, 50, {"Warum bist du so traurig?"}, aufzu(u, 15.4f, 19), 180, 140);
  blase(g, 110, 50, {"Drüben liegt der Granatenberg,", "wo die Edelsteine wachsen."}, aufzu(u, 19.4f, 23.8f), 96, 140);
}

// 7) Mit dem Mantel auf den Granatenberg - und dort betrogen
static void szeneBerg(Gfx &g, float u) {
  if (u < 10) {
    saal(g, u, 0);
    tochter(g, 96, 222, 0.85f, 1, u, u > 0.4f && u < 4.5f ? 0.4f : 0, 0.4f, u < 4.6f);
    jaeger(g, 180, 222, 0.95f, -1, u, -1, u > 4.8f && u < 9 ? 0.4f : 0, 0, sanft(seg(u, 8, 9.6f)));
    g.kameraAus();
    blase(g, 110, 50, {"Aber nur die Vögel kommen", "hin, kein Mensch."}, aufzu(u, 0.4f, 4.5f), 96, 140);
    blase(g, 180, 50, {"Den Kummer nehm ich dir", "bald vom Herzen!"}, aufzu(u, 4.8f, 9.4f), 180, 140);
    blende(g, u, 10, 140, 160);
    return;
  }
  if (u < 16) {  // der Flug
    float v = u - 10;
    himmel(g, 0, u, 240);
    for (int i = 0; i < 5; i++) wolke(g, fmodf(i * 80 - v * 90 + 800, 400) - 60, 60 + i * 34, 1 + (i & 1) * 0.6f);
    float fx = lerp(60, 200, v / 6), fy = 150 + sinf(v * 3) * 8;
    tochter(g, fx + 14, fy, 0.8f, 1, u, 0.6f, 1.2f);
    jaeger(g, fx, fy, 0.9f, 1, u, -1, 0.5f, 0, 1, false, false, 1.4f);
    g.kameraAus();
    vers(g, {"Er nahm sie unter den Mantel", "und wünschte sich hinüber."}, aufzu(v, 0.2f, 5.8f));
    if (v < 1) puff(g, 70, 140, v);
    return;
  }
  float v = u - 16;
  granatenberg(g, u);
  bool schlaeft = v > 9;
  bool weg = v > 14;
  if (!schlaeft) {
    jaeger(g, 110, 222, 0.95f, 1, u, -1, v > 5.4f && v < 9 ? 0.4f : 0.3f, 0, 1, false, false, v < 5 ? 1 + sinf(u * 4) * 0.6f : 0.5f);
    tochter(g, 170, 222, 0.85f, -1, u, 0.3f, v < 5 ? 1 + sinf(u * 4 + 1) * 0.6f : 0.4f);
  } else {
    // er schlaeft, den Kopf in ihrem Schoss
    tochter(g, 160, 222, 0.85f, -1, u, 0, 0.8f);
    if (!weg) {
      Pose p = posis(JAEGER, u);
      p.zu = true;
      p.armV = 1.5f;
      schneiderlein(g, 108, 230, 0.8f, 1, p);
      mantelUm(g, 108, 230, 0.8f, 1, u, 1 - seg(v, 11, 12.5f), 0.2f);
    } else {
      Pose p = posis(JAEGER, u);
      p.zu = true;
      schneiderlein(g, 108, 230, 0.8f, 1, p);
    }
    zzz(g, 120, 150, u);
    if (v > 11 && v < 14) mantelUm(g, 160, 222, 0.85f, -1, u, seg(v, 11.5f, 13));
  }
  if (weg) puff(g, 160, 180, seg(v, 14, 15.5f), FARBE(0xE8283F));
  g.kameraAus();
  vers(g, {"Dort funkelten die Granaten.", "Sie sammelten die schönsten."}, aufzu(v, 0.4f, 5));
  blase(g, 120, 60, {"Ich bin so müde. Lass uns", "ein wenig niedersitzen."}, aufzu(v, 5.4f, 9), 110, 130);
  vers(g, {"Als er schlief, nahm sie den", "Mantel und wünschte sich heim."}, aufzu(v, 11, 17.6f));
}

// 8) Allein auf dem Berg - die drei Riesen
static void szeneRiesen(Gfx &g, float u) {
  granatenberg(g, u);
  bool liegt = u > 7.5f && u < 30;
  // die Riesen kommen von rechts, gehen am Ende wieder
  float k = seg(u, 7, 12), z = seg(u, 27, 32);
  for (int i = 0; i < 3; i++) {
    RiesePose r;
    r.farbe = i == 1 ? 1 : 0;
    r.lauf = (k > 0 && k < 1) || (z > 0 && z < 1) ? fmodf(u * 0.9f + i * 0.3f, 1) : -1;
    bool spricht = (i == 0 && u > 12.4f && u < 16.6f) || (i == 1 && u > 16.8f && u < 20.4f) || (i == 2 && u > 20.6f && u < 27);
    r.mund = spricht ? 0.5f + 0.4f * sinf(u * 9) : 0;
    r.boese = i == 1 ? 0.8f : 0;
    r.armV = i == 0 && u > 13 && u < 16 ? 1 : 0.3f;
    float x = lerp(330 + i * 60, 158 + i * 48, k) + z * 260;
    if (x < 330) riese(g, x, 224, 0.62f, z > 0 ? 1 : -1, r);
  }
  // der Jaeger
  Pose p = posis(JAEGER, u);
  if (liegt) {
    p.zu = true;
    schneiderlein(g, 90, 230, 0.8f, 1, p);
  } else {
    p.mund = u < 6 ? 0.5f : 0.3f;
    p.armV = u < 6 ? 2.2f : 0.5f;
    schneiderlein(g, 90, 222, 0.9f, 1, p);
  }
  if (u > 12.4f && u < 15) for (int i = 0; i < 3; i++) g.stern(90 + i * 8, 170, 2, 1, 4, u * 4, GOLD);
  g.kameraAus();
  blase(g, 110, 60, {"Ach, wie groß ist die", "Untreue in der Welt!"}, aufzu(u, 0.4f, 6), 90, 130);
  vers(g, {"Da kamen drei Riesen. Schnell", "stellte er sich schlafend."}, aufzu(u, 6.4f, 12));
  blase(g, 190, 40, {"Was liegt denn da", "für ein Erdwurm?"}, aufzu(u, 12.4f, 16.6f), 170, 90);
  blase(g, 200, 40, {"Tritt ihn tot!"}, aufzu(u, 16.8f, 20.4f), 214, 90);
  blase(g, 170, 40, {"Das ist nicht der Mühe wert!", "Steigt er auf den Gipfel,", "tragen ihn die Wolken fort."}, aufzu(u, 20.6f, 27), 254, 90);
  vers(g, {"Der Jäger merkte sich das gut."}, aufzu(u, 28, 33.6f));
}

// 9) Die Wolke traegt ihn fort und setzt ihn in einen Krautgarten
static void szeneWolke(Gfx &g, float u) {
  // Wolke (wx = Mitte, wy = Oberkante, auf der er steht)
  auto wolkeMit = [&](float wx, float wy, float armV, uint8_t a = 255) {
    wolke(g, wx - 45, wy + 34, 3.4f, a);
    Pose p = posis(JAEGER, u);
    p.armV = armV;
    p.armH = armV;
    p.mund = 0.5f;
    schneiderlein(g, wx, wy + 4, 0.8f, 1, p);
  };
  if (u < 9) {
    granatenberg(g, u);
    float k = seg(u, 0, 3.4f);
    if (u < 4) {
      Pose p = posis(JAEGER, u);
      p.lauf = k < 1 ? fmodf(u * 1.4f, 1) : -1;
      p.armV = k < 1 ? 0.4f : 1.6f;
      schneiderlein(g, lerp(90, 150, k), lerp(222, 180, k), 0.8f, 1, p);
      wolke(g, lerp(330, 105, sanft(seg(u, 1, 4))), 214, 3.4f);
    } else {
      float f = hinein(seg(u, 5, 9));
      wolkeMit(150 + f * 160, 180 - f * 120, 2.4f);
    }
    g.kameraAus();
    vers(g, {"Er stieg auf den Gipfel. Da kam", "eine Wolke und trug ihn fort."}, aufzu(u, 0.4f, 8.6f));
    return;
  }
  float v = u - 9;
  krautgarten(g, u);
  float k = sanft(seg(v, 0, 5));
  float fade = seg(v, 5.5f, 8);
  if (fade <= 0) wolkeMit(lerp(-40, 142, k), lerp(40, 190, k), 2.4f - k * 1.4f);
  else {
    wolke(g, 97, 224 - fade * 200, 3.4f, (uint8_t)(255 * (1 - fade)));
    Pose p = posis(JAEGER, u);
    p.mund = 0.4f;
    schneiderlein(g, 142, 222, 0.85f, 1, p);
  }
  g.kameraAus();
  vers(g, {"Sie setzte ihn sanft in einen", "großen Krautgarten."}, aufzu(v, 0.4f, 8.6f));
}

// 10) Der Krautesel: die eine Sorte macht zum Esel, die andere wieder zum Menschen
static void szeneKraut(Gfx &g, float u) {
  bool esel1 = u > 8, mensch = u > 20;
  krautgarten(g, u, u > 6 ? 1 : 0, u > 18 ? 1 : 0);
  if (!esel1 || mensch) {
    float x = mensch ? 196 : 110;
    jaeger(g, x, 222, 0.9f, mensch ? -1 : 1, u, -1, (u < 5.5f) || (u > 27 && u < 33.6f) ? 0.4f : 0.6f, u > 30 ? 3 : 0, 0,
           false, false, (u > 6 && u < 8) ? 1.8f : -1);
    if (u > 6 && u < 8) salat(g, 128, 180, 0.6f, false, seg(u, 6, 8));
    if (u > 30) { salat(g, 182, 178, 0.6f, false); salat(g, 182, 192, 0.6f, true); }
  } else {
    float k = seg(u, 15, 19);
    float x = lerp(110, 196, k);
    float frisst = (u > 10 && u < 15) || u > 19 ? 0.8f + 0.2f * sinf(u * 6) : 0;
    esel(g, x, 222, 0.85f, 1, u, k > 0 && k < 1 ? fmodf(u * 1.4f, 1) : -1, frisst > 0 ? 0.5f + 0.5f * sinf(u * 10) : 0, frisst);
  }
  if (u > 8 && u < 9.6f) puff(g, 110, 170, seg(u, 8, 9.6f), FARBE(0x8EDB5A));
  if (u > 20 && u < 21.6f) puff(g, 196, 170, seg(u, 20, 21.6f), FARBE(0x8EDB5A));
  g.kameraAus();
  blase(g, 120, 60, {"Ich hab solchen Hunger!", "Hier wächst nur Salat..."}, aufzu(u, 0.4f, 5.6f), 110, 130);
  vers(g, {"Kaum hatte er gegessen, wuchsen", "ihm lange Ohren und vier Beine -", "er war ein Esel!"}, aufzu(u, 9, 15.8f));
  vers(g, {"Da fraß er von der anderen Sorte -", "und wurde wieder ein Mensch."}, aufzu(u, 20.6f, 26.8f));
  blase(g, 170, 60, {"Das hilft mir zu meinem Recht", "und straft die Untreue!"}, aufzu(u, 27.2f, 33.6f), 196, 130);
}

// 11) Verkleidet ins Schloss: die Alte, die Magd und die Tochter werden Eselinnen
static void szeneEselinnen(Gfx &g, float u) {
  if (u < 18) {
    schlossFront(g, u, seg(u, 10, 11.5f));
    float k = seg(u, 0, 6);
    jaeger(g, lerp(-10, 104, k), 222, 0.95f, 1, u, k < 1 ? fmodf(u * 1.5f, 1) : -1, u > 6.4f && u < 12 ? 0.5f : 0, 3, 0, true);
    salat(g, lerp(-10, 104, k) + 16, 180, 0.6f, false);
    if (u > 10) alte(g, 160, 222, 0.85f, -1, u, u > 12.4f && u < 17 ? 0.5f + 0.3f * sinf(u * 9) : 0, u > 12.4f ? 1.6f : 0.4f, true);
    g.kameraAus();
    vers(g, {"Er färbte sein Gesicht braun,", "damit ihn niemand erkannte."}, aufzu(u, 0.4f, 6));
    blase(g, 120, 50, {"Ich bin ein Bote des Königs", "und bringe den köstlichsten", "Salat unter der Sonne!"}, aufzu(u, 6.4f, 12), 104, 130);
    blase(g, 180, 50, {"Lieber Landsmann, lasst", "mich doch einmal kosten!"}, aufzu(u, 12.4f, 17.6f), 160, 140);
    return;
  }
  float v = u - 18;
  saal(g, u, 0);
  // die Alte kostet -> Eselin (bei v = 3)
  if (v < 3) {
    alte(g, 210, 222, 0.85f, -1, u, 0.5f + 0.4f * sinf(u * 12), 1.6f, true);
    salat(g, 188, 166, 0.5f, false, seg(v, 0, 3) * 0.8f);
  } else eselin(g, 0, v < 8 ? 210 : lerp(210, 340, seg(v, 8, 11)), 222, 0.8f, v < 8 ? -1 : 1, u, v > 8 ? fmodf(u * 1.4f, 1) : -1);
  puff(g, 210, 170, seg(v, 3, 4.4f), FARBE(0x8EDB5A));
  // die Magd traegt die Schuessel und nascht (bei v = 12)
  if (v > 8 && v < 12) {
    float k = seg(v, 8, 11);
    magd(g, lerp(250, 170, k), 222, 0.85f, -1, u, v > 11 ? 0.6f : 0, 1.4f, k < 1 ? fmodf(u * 1.4f, 1) : -1);
    salat(g, lerp(250, 170, k) - 14, 176, 0.5f, false);
  }
  if (v >= 12) eselin(g, 1, v < 18 ? 170 : lerp(170, 340, seg(v, 18, 21)), 222, 0.8f, v < 18 ? -1 : 1, u, v > 18 ? fmodf(u * 1.4f, 1) : -1);
  if (v > 12.4f && v < 14) for (int i = 0; i < 4; i++) g.ellipse(150 + i * 8, 218, 5, 2, FARBE(0x4E8E7A));  // die Blaetter am Boden
  puff(g, 170, 170, seg(v, 12, 13.4f), FARBE(0x8EDB5A));
  // der Jaeger bringt der Tochter die Schuessel selbst (bei v = 23)
  if (v > 16) {
    jaeger(g, 86, 222, 0.95f, 1, u, -1, v > 17.4f && v < 21.6f ? 0.5f : 0, v < 21 ? 3 : 0, 0, true);
    if (v < 21) salat(g, 104, 180, 0.6f, false);
    if (v < 23) {
      tochter(g, 136, 222, 0.85f, -1, u, v > 21 ? 0.5f + 0.4f * sinf(u * 12) : 0, v > 21 ? 1.6f : 0.4f);
      if (v > 21) salat(g, 122, 166, 0.5f, false, seg(v, 21, 23) * 0.8f);
    } else eselin(g, 2, 136, 222, 0.8f, -1, u);
    puff(g, 136, 170, seg(v, 23, 24.4f), FARBE(0x8EDB5A));
  }
  g.kameraAus();
  vers(g, {"Kaum hatte die Alte gekostet,", "lief sie als Eselin hinaus."}, aufzu(v, 3.4f, 8.2f));
  vers(g, {"Die Magd naschte auf dem Weg -", "und wurde auch eine Eselin."}, aufzu(v, 12.4f, 17));
  blase(g, 110, 50, {"Ich bring euch selbst", "das köstliche Essen."}, aufzu(v, 17.4f, 21.6f), 86, 130);
  vers(g, {"Auch das Mädchen aß davon -", "und war eine Eselin."}, aufzu(v, 23.4f, 27.8f));
}

// 12) Zum Mueller: wie jede gehalten werden soll
static void szeneMueller(Gfx &g, float u) {
  if (u < 10) {
    saal(g, u, 0);
    eselin(g, 0, 236, 222, 0.7f, -1, u);
    eselin(g, 1, 190, 222, 0.7f, -1, u);
    eselin(g, 2, 144, 222, 0.7f, -1, u, -1, 0, 0.6f);
    bool sauber = u > 3;
    jaeger(g, 70, 222, 0.95f, 1, u, -1, u > 5.4f ? 0.5f : 0, 0, 0, !sauber, false, u > 1 && u < 3 ? 2.6f : -1);
    if (u > 1 && u < 3) for (int i = 0; i < 4; i++) tropfen(g, 66 + i * 4, 130 + fmodf(u * 40 + i * 7, 20), 2, FARBE(0x7EC0EE));
    g.kameraAus();
    vers(g, {"Er wusch sich das Gesicht -", "da erkannten sie ihn."}, aufzu(u, 0.4f, 5));
    blase(g, 120, 60, {"Jetzt sollt ihr den Lohn", "für eure Untreue empfangen!"}, aufzu(u, 5.4f, 10), 76, 130);
    return;
  }
  float v = u - 10;
  muehle(g, u);
  float k = seg(v, 0, 5);
  float lauf = k < 1 ? fmodf(u * 1.4f, 1) : -1;
  float jx = lerp(-150, 60, k);
  g.linie(jx + 8, 190, jx + 150, 194, 1.4f, HOLZ_D);  // das Seil
  eselin(g, 0, jx + 50, 222, 0.55f, 1, u, lauf);
  eselin(g, 1, jx + 94, 222, 0.55f, 1, u, lauf);
  eselin(g, 2, jx + 138, 222, 0.55f, 1, u, lauf, 0, 0.6f);
  jaeger(g, jx, 222, 0.95f, 1, u, lauf, v > 6.4f && v < 11 ? 0.5f : 0, 0);
  Pose m = posis(MUELLER, u);
  m.mund = v > 11 && v < 25 ? 0.2f : 0;
  m.armV = v > 21 ? 0.4f + fabsf(sinf(u * 2)) * 0.4f : 0.4f;
  schneiderlein(g, 236, 222, 0.85f, -1, m);
  g.kameraAus();
  blase(g, 120, 50, {"Nehmt diese drei Tiere in Kost", "und haltet sie, wie ich sage:"}, aufzu(v, 6.4f, 11), 60, 130);
  vers(g, {"Die Alte: dreimal Schläge und", "einmal Futter. Die Magd: einmal", "Schläge und dreimal Futter."}, aufzu(v, 11.4f, 18.6f));
  vers(g, {"Die Jüngste aber: keine Schläge", "und dreimal Futter - schlagen", "lassen konnte er sie nicht."}, aufzu(v, 19, 26));
}

// 13) Die alte Eselin ist tot; die beiden anderen werden erloest - Vergebung
static void szeneVergebung(Gfx &g, float u) {
  muehle(g, u);
  Pose m = posis(MUELLER, u);
  m.mund = u > 0.4f && u < 7 ? 0.5f + 0.3f * sinf(u * 9) : 0;
  m.armV = u < 7 ? 1 : 0.4f;
  schneiderlein(g, 220, 222, 0.85f, -1, m);
  bool mensch = u > 13;
  if (!mensch) {
    eselin(g, 1, 172, 222, 0.62f, -1, u, -1, 0, 1);
    eselin(g, 2, 124, 222, 0.62f, -1, u, -1, 0, 1);
    if (u > 10) { salat(g, 150, 214, 0.6f, true); salat(g, 100, 214, 0.6f, true); }
  } else {
    magd(g, 176, 222, 0.82f, -1, u, 0.3f, 0.4f);
    float knie = u > 15 && u < 31 ? 10 : 0;
    tochter(g, 128, 222 + knie, 0.85f, -1, u, (u > 15.4f && u < 31) ? 0.4f + 0.2f * sinf(u * 8) : 0.3f,
            u > 15 && u < 31 ? 1.8f : 0.4f, u > 15 && u < 27, 1.6f);
  }
  puff(g, 172, 170, seg(u, 13, 14.4f), FARBE(0x8EDB5A));
  puff(g, 124, 170, seg(u, 13.5f, 14.9f), FARBE(0x8EDB5A));
  jaeger(g, 66, 222, 0.95f, 1, u, -1, u > 31.4f ? 0.5f : 0, u > 8 && u < 12 ? 3 : 0, 0, false, false, u > 31.4f ? 1.4f : -1);
  if (u > 34) herzen(g, 98, 150, u, 6, 30);
  g.kameraAus();
  blase(g, 190, 50, {"Die alte Eselin ist tot.", "Die zwei anderen sind so traurig,", "die leben auch nicht mehr lang."}, aufzu(u, 0.4f, 7), 220, 130);
  vers(g, {"Da erbarmte sich der Jäger", "und gab ihnen vom guten Salat."}, aufzu(u, 7.4f, 13));
  blase(g, 140, 50, {"Ach, mein Liebster, verzeih mir!", "Die Mutter hat mich gezwungen."}, aufzu(u, 15.4f, 21), 128, 150);
  blase(g, 140, 50, {"Ich hab dich von Herzen lieb.", "Dein Mantel hängt im Schrank,"}, aufzu(u, 21.4f, 26.6f), 128, 150);
  blase(g, 140, 50, {"und das Vogelherz will ich", "dir zurückgeben."}, aufzu(u, 27, 31), 128, 150);
  blase(g, 110, 50, {"Behalt es nur, es ist einerlei.", "Du sollst meine Frau werden!"}, aufzu(u, 31.4f, 38), 70, 130);
  blende(g, u, 40, 100, 160);
}

// 14) Die Hochzeit
static void szeneHochzeit(Gfx &g, float u) {
  himmel(g, 0, u, 200);
  sonne(g, 240, 30, 11, u);
  schloss(g, 142, 200, u, 0);
  boden(g, 200);
  g.rechteck(0, 210, 284, 12, FARBE(0xC9B48A));
  magd(g, 222, 222, 0.75f, -1, u, 0.4f, 2.2f + sinf(u * 5) * 0.4f);
  Pose m = posis(MUELLER, u);
  m.armV = 2.4f + sinf(u * 4) * 0.4f;
  m.mund = 0.5f;
  schneiderlein(g, 256, 222, 0.75f, -1, m);
  tochter(g, 150, 222, 0.9f, -1, u, 0.4f, 1.2f, false, 0.2f, &BRAUT);
  jaeger(g, 110, 222, 0.95f, 1, u, -1, 0.5f, 0, 1, false, false, 1.2f);
  herzen(g, 130, 150, u, 6, 30);
  for (int i = 0; i < 3; i++) feuerwerk(g, 50 + i * 90, 50 + (i & 1) * 20, fmodf(u * 0.4f + i * 0.33f, 1), i == 0 ? GOLD : i == 1 ? FARBE(0xFF8FB0) : FARBE(0x9FD8FF));
  g.kameraAus();
  vers(g, {"Da wurde Hochzeit gehalten,", "und sie lebten vergnügt", "bis an ihren Tod."}, aufzu(u, 0.4f, 12));
  endeBild(g, u, 14, 130, 170);
}

static void bild(Gfx &g, float t) {
  if (t < P2) szeneAlte(g, t - P1);
  else if (t < P3) szeneVoegel(g, t - P2);
  else if (t < P4) szeneGold(g, t - P3);
  else if (t < P5) szeneSchloss(g, t - P4);
  else if (t < P6) szeneTrank(g, t - P5);
  else if (t < P7) szeneMantel(g, t - P6);
  else if (t < P8) szeneBerg(g, t - P7);
  else if (t < P9) szeneRiesen(g, t - P8);
  else if (t < P10) szeneWolke(g, t - P9);
  else if (t < P11) szeneKraut(g, t - P10);
  else if (t < P12) szeneEselinnen(g, t - P11);
  else if (t < P13) szeneMueller(g, t - P12);
  else if (t < P14) szeneVergebung(g, t - P13);
  else szeneHochzeit(g, t - P14);
}

// =====================================================================
// Musik
// =====================================================================
// Jaegerlied (F-Dur, wie ein Hornruf im Sechsachtel)
static const Ton3 MEL_KRAUT[] = {
  {65, .5, 41}, {69, .25, 0}, {72, .75, 0}, {72, .5, 0}, {77, .25, 0}, {76, .75, 48}, {72, .5, 0},
  {74, .25, 0}, {72, .5, 0}, {70, .25, 0}, {69, .75, 41}, {67, .75, 48}, {65, .5, 41}, {69, .25, 0},
  {72, .75, 0}, {77, .5, 0}, {76, .25, 0}, {74, .75, 46}, {72, .5, 48}, {70, .25, 0}, {69, .5, 0},
  {67, .25, 48}, {65, 1.5, 41}};
// Hexenweise (d-Moll, schleichend)
static const Ton3 MEL_HEXE[] = {
  {62, .5, 38}, {65, .5, 0}, {69, .5, 0}, {68, .5, 0}, {69, 1, 0}, {0, .5, 0}, {65, .5, 0},
  {64, .5, 37}, {67, .5, 0}, {70, .5, 0}, {69, .5, 0}, {65, 1, 0}, {62, 1, 38}};
static void iaa(Synth &s, float x) {  // I-aah
  s.ton(x, 520, 0.16f, 0.02f, 0.35f, SAEGE, GERAEUSCH, 380);
  s.ton(x + 0.4f, 300, 0.16f, 0.02f, 0.5f, SAEGE, GERAEUSCH, 200);
}
static void zauber(Synth &s, float x) { for (int k = 0; k < 6; k++) s.note(x + k * 0.06f, 84 + k * 2, 0.45f); }
static void ton(Synth &s, float t0, float t1) {
  float a = P1;
  spieleMelodie(s, M(MEL_KRAUT), a + 0.5f, a + 31, 0.32f, t0, t1, 0.65f);
  if (bei(a + 12, t0, t1)) s.note(a + 12, 88, 0.6f);
  a = P2;
  spieleMelodie(s, M(MEL_JAGD), a + 0.4f, a + 10.4f, 0.26f, t0, t1, 0.45f);
  for (int i = 0; i < 14; i++) {  // Gezeter der Voegel
    float x = a + 5.4f + i * 0.36f;
    if (bei(x, t0, t1)) s.ton(x, 2400 + (i % 3) * 500, 0.06f, 0.005f, 0.08f, DREIECK, GERAEUSCH, 3200);
  }
  if (bei(a + 10.5f, t0, t1)) { s.rauschen(a + 10.5f, 0.5f, TIEFPASS, 1600, 0.45f, 1, 0.001f, 200); s.ton(a + 10.5f, 120, 0.35f, 0.001f, 0.3f, SINUS, GERAEUSCH, 40); }
  if (bei(a + 17, t0, t1)) zauber(s, a + 17);
  spieleMelodie(s, M(MEL_KRAUT), a + 13, a + 27.5f, 0.34f, t0, t1, 0.5f);
  a = P3;
  if (bei(a + 5, t0, t1)) s.glocke(a + 5, 84, 0.6f);
  for (int i = 0; i < 12; i++) if (bei(a + 9 + i * 0.5f, t0, t1)) s.note(a + 9 + i * 0.5f, 79 + (i % 4) * 3, 0.35f);  // klimpern
  spieleMelodie(s, M(MEL_WANDER), a + 18, a + 23.6f, 0.3f, t0, t1, 0.6f);
  a = P4;
  spieleMelodie(s, M(MEL_KRAUT), a + 0.4f, a + 8, 0.32f, t0, t1, 0.55f);
  spieleMelodie(s, M(MEL_HEXE), a + 8.4f, a + 20, 0.36f, t0, t1, 0.5f);
  spieleMelodie(s, M(MEL_WIEGE), a + 21, a + 27.6f, 0.4f, t0, t1, 0.55f);
  a = P5;
  spieleMelodie(s, M(MEL_HEXE), a + 0.4f, a + 11, 0.36f, t0, t1, 0.5f);
  for (int i = 0; i < 10; i++) if (bei(a + 0.4f + i * 0.7f, t0, t1)) s.ton(a + 0.4f + i * 0.7f, 300 + zufall(i) * 200, 0.08f, 0.01f, 0.2f, SINUS, GERAEUSCH, 600);  // blubb
  if (bei(a + 13.4f, t0, t1)) zauber(s, a + 13.4f);
  spieleMelodie(s, M(MEL_SPUK), a + 14, a + 27.6f, 0.42f, t0, t1, 0.35f);
  a = P6;
  spieleMelodie(s, M(MEL_HEXE), a + 0.4f, a + 14.5f, 0.36f, t0, t1, 0.5f);
  spieleMelodie(s, M(MEL_WIEGE), a + 15, a + 23.8f, 0.42f, t0, t1, 0.5f);
  a = P7;
  if (bei(a + 10, t0, t1)) { zauber(s, a + 10); s.rauschen(a + 10, 5.5f, BANDPASS, 900, 0.08f, 0.8f, 0.5f, 1800); }
  spieleMelodie(s, M(MEL_F), a + 10.4f, a + 24.8f, 0.28f, t0, t1, 0.5f);
  if (bei(a + 30, t0, t1)) zauber(s, a + 30);
  spieleMelodie(s, M(MEL_N), a + 27, a + 33.8f, 0.4f, t0, t1, 0.35f);
  a = P8;
  spieleMelodie(s, M(MEL_N), a + 0.4f, a + 7, 0.4f, t0, t1, 0.4f);
  for (int i = 0; i < 8; i++) {  // Riesenschritte
    float x = a + 7 + i * 0.6f;
    if (bei(x, t0, t1)) s.ton(x, 70, 0.4f, 0.005f, 0.3f, SINUS, GERAEUSCH, 40);
    x = a + 27 + i * 0.6f;
    if (bei(x, t0, t1)) s.ton(x, 70, 0.3f, 0.005f, 0.3f, SINUS, GERAEUSCH, 40);
  }
  spieleMelodie(s, M(MEL_RIESE), a + 12.4f, a + 27, 0.42f, t0, t1, 0.5f);
  a = P9;
  if (bei(a + 3, t0, t1)) s.rauschen(a + 3, 9, BANDPASS, 500, 0.08f, 0.7f, 1, 1200);  // Wind
  spieleMelodie(s, M(MEL_WIEGE), a + 3, a + 17.6f, 0.38f, t0, t1, 0.5f);
  a = P10;
  for (int i = 0; i < 5; i++) if (bei(a + 6 + i * 0.3f, t0, t1)) s.rauschen(a + 6 + i * 0.3f, 0.08f, BANDPASS, 2600, 0.12f);  // mampf
  if (bei(a + 8, t0, t1)) zauber(s, a + 8);
  if (bei(a + 10, t0, t1)) iaa(s, a + 10);
  spieleMelodie(s, M(MEL_KEGEL), a + 10.4f, a + 19, 0.3f, t0, t1, 0.5f);
  if (bei(a + 20, t0, t1)) zauber(s, a + 20);
  spieleMelodie(s, M(MEL_KRAUT), a + 21, a + 33.6f, 0.32f, t0, t1, 0.55f);
  a = P11;
  spieleMelodie(s, M(MEL_KRAUT), a + 0.4f, a + 17.6f, 0.34f, t0, t1, 0.5f);
  float z[3] = {a + 21, a + 30, a + 41};
  for (int i = 0; i < 3; i++) {
    if (bei(z[i], t0, t1)) zauber(s, z[i]);
    if (bei(z[i] + 1, t0, t1)) iaa(s, z[i] + 1);
  }
  if (bei(a + 30.4f, t0, t1)) s.rauschen(a + 30.4f, 0.3f, BANDPASS, 2400, 0.2f);  // Schuessel klirrt
  spieleMelodie(s, M(MEL_KEGEL), a + 18.4f, a + 45.6f, 0.32f, t0, t1, 0.4f);
  a = P12;
  spieleMelodie(s, M(MEL_HEXE), a + 5.4f, a + 10, 0.36f, t0, t1, 0.4f);
  for (int i = 0; i < 18; i++) if (bei(a + 10 + i * 0.28f, t0, t1)) s.rauschen(a + 10 + i * 0.28f, 0.05f, BANDPASS, 1500 + (i & 1) * 300, 0.1f);
  spieleMelodie(s, M(MEL_WANDER), a + 15.4f, a + 35.6f, 0.34f, t0, t1, 0.45f);
  a = P13;
  spieleMelodie(s, M(MEL_N), a + 0.4f, a + 12.6f, 0.42f, t0, t1, 0.35f);
  if (bei(a + 13, t0, t1)) zauber(s, a + 13);
  if (bei(a + 13.5f, t0, t1)) zauber(s, a + 13.5f);
  spieleMelodie(s, M(MEL_WIEGE), a + 15.4f, a + 31, 0.42f, t0, t1, 0.55f);
  spieleMelodie(s, M(MEL_KRAUT), a + 31.4f, a + 39.6f, 0.3f, t0, t1, 0.6f);
  a = P14;
  for (int i = 0; i < 6; i++) if (bei(a + 0.4f + i * 1.2f, t0, t1)) s.glocke(a + 0.4f + i * 1.2f, i & 1 ? 67 : 72, 0.6f);
  spieleMelodie(s, M(MEL_F), a + 0.8f, a + 13.6f, 0.28f, t0, t1, 0.7f);
  endeKlang(s, a + 14.4f, t0, t1);
}

// =====================================================================
// Startbild, Vorschau, Erkennungszeichen
// =====================================================================
static void titelSzene(Gfx &g, float t) {
  krautgarten(g, t);
  esel(g, 196, 226, 0.8f, -1, t, -1, 0, 0.6f + 0.2f * sinf(t * 2));
  jaeger(g, 104, 226, 0.95f, 1, t, -1, 0.4f, 0, 1, false, false, 1.3f);
  salat(g, 122, 180, 0.6f, true);
}
static void titelbild(Gfx &g, float t) { titelSzene(g, t); titelRahmen(g, t, "Der Krautesel", 180); }
static void vorschau(Gfx &g, float t) { titelSzene(g, t); }
static void symbol(Gfx &g, float x, float y, float t) {
  salat(g, x - 22, y - 10, 1.6f, false);
  salat(g, x + 22, y - 10, 1.6f, true);
  glitzern(g, x, y - 30, 40, t, 5);
}

const Maerchen KRAUTESEL = {"Der Krautesel", PDAUER, FARBE(0x8EDB5A), bild, ton, titelbild, vorschau, symbol,
                            LIED(MEL_KRAUT, 0.32f), nullptr};

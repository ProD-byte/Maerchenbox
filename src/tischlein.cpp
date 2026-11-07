// Tischlein deck dich, Goldesel und Knueppel aus dem Sack - ein Spieluhr-Film fuer die Maerchenbox, nach den Bruedern Grimm.
// Drei Brueder lernen ein Handwerk und bekommen drei Wunderdinge. Der Wirt vertauscht Tisch und Esel -
// bis der Knueppel aus dem Sack springt. Am Ende feiert die ganze Familie.
#include "erzaehlen.h"

// ---------- Ablauf (Sekunden) ----------
static const float P1 = 0, P2 = 22, P3 = 48, P4 = 76, P5 = 106, P6 = 144, PDAUER = 176;

static Aussehen sohnLook(int i) {
  Aussehen a;
  a.weste = i == 0 ? FARBE(0x3E7FDC) : i == 1 ? FARBE(0xE0A23C) : FARBE(0x3DAE5B);
  a.hut = i == 0 ? FARBE(0x2E5FAE) : i == 1 ? FARBE(0xC0632E) : FARBE(0x2C8A45);
  a.hutD = OUT; a.feder = i == 2 ? FARBE(0xE8473D) : GOLD;
  a.haar = i == 1 ? FARBE(0xC98A3A) : FARBE(0x6E3E20);
  return a;
}
static Aussehen vaterLook() { Aussehen a; a.weste = FARBE(0x8E6E4E); a.hose = FARBE(0x4A3B2A); a.haar = FARBE(0xD8D8D8); a.muetze = 1; a.halstuch = FARBE(0xE0A23C); return a; }
static Aussehen wirtLook() { Aussehen a; a.weste = FARBE(0xF6F2EA); a.hemd = FARBE(0xC0392B); a.hose = FARBE(0x3A3436); a.haar = FARBE(0x2E2220); a.muetze = 1; a.wangen = true; return a; }
static Aussehen meisterLook() { Aussehen a; a.weste = FARBE(0x9A6A3E); a.hemd = FARBE(0xF2E8D0); a.hose = FARBE(0x5A4A3A); a.haar = FARBE(0x9A9A9A); a.muetze = 1; return a; }
static const Aussehen S0 = sohnLook(0), S1 = sohnLook(1), S2 = sohnLook(2), VATER = vaterLook(), WIRT = wirtLook(), MEISTER = meisterLook();
static const Aussehen *SOHN[3] = {&S0, &S1, &S2};

// ---------- Dinge ----------
static void tischlein(Gfx &g, float x, float y, float s, float gedeckt, float t) {
  g.kontur(1.4f, OUT);
  g.rechteck(x - 30 * s, y - 34 * s, 5 * s, 34 * s, HOLZ_D);
  g.rechteck(x + 25 * s, y - 34 * s, 5 * s, 34 * s, HOLZ_D);
  g.rundRechteck(x - 38 * s, y - 40 * s, 76 * s, 8 * s, 2 * s, HOLZ);
  g.konturAus();
  if (gedeckt <= 0.02f) return;
  uint8_t a = (uint8_t)(255 * klemme(gedeckt));
  g.rechteck(x - 36 * s, y - 42 * s, 72 * s, 3 * s, WEISS, a);              // Tischtuch
  g.ellipse(x - 20 * s, y - 44 * s, 12 * s, 3 * s, WEISS, a);               // Teller
  g.ellipse(x - 20 * s, y - 49 * s, 9 * s, 6 * s, FARBE(0xC9742E), a);      // Braten
  g.ellipse(x - 24 * s, y - 51 * s, 3 * s, 2 * s, FARBE(0xE8A05A), a);
  g.ellipse(x + 4 * s, y - 46 * s, 8 * s, 5 * s, FARBE(0xE0B060), a);       // Brot
  g.rechteck(x + 18 * s, y - 60 * s, 6 * s, 16 * s, FARBE(0x8E2A4A), a);    // Wein
  g.rechteck(x + 19.5f * s, y - 66 * s, 3 * s, 6 * s, FARBE(0x8E2A4A), a);
  g.kreis(x + 30 * s, y - 47 * s, 4 * s, FARBE(0xE8473D), a);               // Apfel
  if (gedeckt < 1) puff(g, x, y - 50 * s, gedeckt);
  else for (int i = 0; i < 2; i++) {  // Dampf vom Braten
    float k = fmodf(t * 0.6f + i * 0.5f, 1);
    g.kurve(x - 22 * s + i * 5, y - 54 * s - k * 14, x - 25 * s + i * 5, y - 58 * s - k * 14, x - 20 * s + i * 5, y - 62 * s - k * 14, 1.2f, WEISS, (uint8_t)(160 * (1 - k)));
  }
}

static void esel(Gfx &g, float x, float y, float s, float dir, float t, float lauf = -1, float mund = 0) {
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
  g.kurve(PX(-26), PY(-38), PX(-36), PY(-34), PX(-38), PY(-22), 3 * s, fell);
  g.kreis(PX(-38), PY(-20), 3.5f * s, dunkel);
  g.ellipse(PX(0), PY(-40), 30 * s, 14 * s, fell);
  float hx = PX(24), hy = PY(-56);
  g.linie(PX(18), PY(-44), hx, hy, 12 * s, fell);
  float o1[] = {hx - 4 * s * dir, hy - 8 * s, hx - 10 * s * dir, hy - 34 * s, hx + 2 * s * dir, hy - 10 * s};
  float o2[] = {hx + 2 * s * dir, hy - 9 * s, hx + 4 * s * dir, hy - 36 * s, hx + 9 * s * dir, hy - 8 * s};
  g.polygon(o1, 3, fell);
  g.polygon(o2, 3, fell);
  g.ellipse(hx + 8 * s * dir, hy + 2 * s, 15 * s, 9 * s, fell, 255, 0.35f * dir);
  g.konturAus();
  g.ellipse(hx + 17 * s * dir, hy + 6 * s, 7 * s, 5.5f * s, hell, 255, 0.35f * dir);
  g.kreis(hx + 20 * s * dir, hy + 4 * s, 1.3f * s, OUT);
  if (mund > 0.05f) g.ellipse(hx + 17 * s * dir, hy + 10 * s, 5 * s, (1 + mund * 4) * s, FARBE(0x7A2A2A));
  g.kreis(hx + 6 * s * dir, hy - 2 * s, 2.6f * s, SCHWARZ);
  g.kreis(hx + 7 * s * dir, hy - 3 * s, 0.9f * s, WEISS);
  for (int i = 0; i < 5; i++) g.kreis(PX(12 + i * 2.5f), PY(-50 - i * 3), 3 * s, dunkel);
  // rote Decke mit Goldborte
  g.rundRechteck(PX(-12) < PX(12) ? PX(-12) : PX(12), PY(-54), 24 * s, 18 * s, 3 * s, FARBE(0xC0392B));
  g.rechteck(PX(-12) < PX(12) ? PX(-12) : PX(12), PY(-38), 24 * s, 2.5f * s, GOLD);
}

static void muenze(Gfx &g, float x, float y, float r, float t, uint8_t a = 255) {
  float b = fabsf(cosf(t * 5)) * 0.7f + 0.3f;
  g.ellipse(x, y, r * b, r, FARBE(0xE8B020), a);
  g.ellipse(x, y, r * b * 0.7f, r * 0.7f, GOLD, a);
}

// Goldregen aus dem Maul des Esels
static void goldregen(Gfx &g, float x, float y, float u, float bis) {
  for (int i = 0; i < 12; i++) {
    float k = fmodf(u * 0.9f + i / 12.0f, 1);
    if (u - k / 0.9f < 0 || u > bis) continue;
    float px = x + (zufall(i) - 0.5f) * 30 + k * 10, py = y + k * k * 60;
    muenze(g, px, py, 3.5f, u + i, (uint8_t)(255 * (1 - k * 0.4f)));
  }
}

static void sack(Gfx &g, float x, float y, float s, float auf) {
  g.kontur(1.3f, OUT);
  g.ellipse(x, y - 14 * s, 13 * s, 15 * s, FARBE(0xC9A46A));
  g.konturAus();
  g.ellipse(x, y - 28 * s - auf * 2, 6 * s, 3 * s, FARBE(0xA8823E));
  g.linie(x - 5 * s, y - 27 * s, x + 5 * s, y - 27 * s, 1.6f, FARBE(0x6E4A2A));
}

static void knueppel(Gfx &g, float x, float y, float w, float s = 1) {
  float c = cosf(w), sn = sinf(w);
  g.kontur(1.2f, OUT);
  g.linie(x - c * 16 * s, y - sn * 16 * s, x + c * 16 * s, y + sn * 16 * s, 5 * s, HOLZ);
  g.kreis(x + c * 17 * s, y + sn * 17 * s, 5 * s, HOLZ);
  g.konturAus();
  g.kreis(x + c * 15 * s, y + sn * 15 * s, 1.5f * s, HOLZ_D);
}

static void stube(Gfx &g, float t, float nacht = 0) {
  g.verlauf(0, 200, mischeFarbe(FARBE(0xE8D2B0), FARBE(0x3A3456), nacht), mischeFarbe(FARBE(0xD6B88E), FARBE(0x2A2440), nacht));
  g.rechteck(0, 200, 284, 40, mischeFarbe(FARBE(0x9E7656), FARBE(0x3A2E3A), nacht));
  g.kontur(1.6f, OUT);
  g.rundRechteck(220, 40, 50, 60, 4, HOLZ);
  g.konturAus();
  g.rundRechteck(225, 45, 40, 50, 2, nacht > 0.5f ? FARBE(0x1E2A5A) : FARBE(0x9FD8FF));
  g.rechteck(243, 45, 3, 50, HOLZ);
  if (nacht > 0.5f) mond(g, 236, 60, 7);
}

static void werkstatt(Gfx &g, float t, int art) {
  stube(g, t);
  // Werkzeuge an der Wand: Saege, Hobel / Mehlsaecke / Drechselbank
  if (art == 0) {
    float sa[] = {30, 60, 90, 60, 90, 74, 30, 70};
    g.polygon(sa, 4, STEIN);
    g.rechteck(14, 58, 18, 14, HOLZ);
    g.rundRechteck(110, 66, 30, 10, 3, HOLZ);
  } else if (art == 1) {
    for (int i = 0; i < 3; i++) { sack(g, 30 + i * 30, 200, 1.1f, 0); g.text(SCHRIFT_K, "M", 30 + i * 30, 190, FARBE(0x8E6E3E), 1); }
    // Muehlrad im Fenster
    g.kontur(1.2f, OUT);
    g.kreis(245, 70, 16, HOLZ);
    g.konturAus();
    for (int i = 0; i < 6; i++) { float a = t * 1.5f + i * 1.047f; g.linie(245, 70, 245 + cosf(a) * 16, 70 + sinf(a) * 16, 2, HOLZ_D); }
  } else {
    g.kontur(1.2f, OUT);
    g.rechteck(20, 150, 90, 10, HOLZ);
    g.konturAus();
    g.rechteck(24, 160, 6, 54, HOLZ_D);
    g.rechteck(100, 160, 6, 54, HOLZ_D);
    g.ellipse(65, 144, 22, 5, FARBE(0xE0B060));
    g.kreis(65 + sinf(t * 8) * 18, 144, 2, WEISS, 180);  // Spaene
  }
}

static void landschaft(Gfx &g, float t, float scroll) {
  himmel(g, 0, t, 190);
  sonne(g, 248, 30, 11, t);
  wolke(g, fmodf(60 - scroll * 0.2f + 600, 400) - 60, 46, 0.8f);
  huegel(g, scroll * 0.4f, 0.03f, 190, 24, 160, GRAS_H);
  for (int i = 0; i < 4; i++) laubbaum(g, fmodf(i * 110 - scroll + 880, 440) - 60, 200, 0.6f + (i & 1) * 0.15f);
  boden(g, 200);
  blumen(g, scroll, 222, 6, 3);
}

// Wirtshaus von aussen
static void wirtshaus(Gfx &g, float t, float nacht) {
  himmel(g, nacht, t, 190);
  if (nacht < 0.5f) sonne(g, 40, 30, 10, t); else mond(g, 40, 34, 9);
  boden(g, 200, mischeFarbe(GRAS, FARBE(0x2E4A3A), nacht));
  schornstein(g, 140, 82, t, 1, nacht);
  hauswand(g, 120, 100, 130, 100, mischeFarbe(FARBE(0xF0DCC0), FARBE(0x6E6480), nacht), true);
  ziegeldach(g, 120, 250, 102, 50, mischeFarbe(FARBE(0xA8452E), FARBE(0x5A2E3A), nacht));
  holztuer(g, 172, 200, 26, 48, HOLZ_D, true);
  g.kontur(1, OUT);
  g.rundRechteck(166, 144, 38, 6, 2, HOLZ);  // Vordach ueber der Tuer
  g.konturAus();
  fenster(g, 134, 126, 20, 18, nacht > 0.5f ? 1 : 0, true, true);
  fenster(g, 216, 126, 20, 18, nacht > 0.5f ? 1 : 0, true, true);
  // Bank vor dem Wirtshaus
  g.rechteck(208, 186, 36, 4, HOLZ);
  g.rechteck(212, 190, 3, 10, HOLZ_D);
  g.rechteck(237, 190, 3, 10, HOLZ_D);
  // Wirtshausschild
  g.linie(250, 112, 272, 112, 2, OUT);
  g.kontur(1.2f, OUT);
  g.rundRechteck(256, 116, 22, 18, 3, HOLZ);
  g.konturAus();
  g.ellipse(267, 125, 5, 6, GOLD);  // Bierkrug
}

// =====================================================================
// Szenen
// =====================================================================
// 1) Der Vater schickt die drei Soehne in die Welt
static void szeneAnfang(Gfx &g, float u) {
  landschaft(g, u, 0);
  haeuschen(g, 45, 200, 70, 70, FARBE(0xF0DCC0), FARBE(0xC0503A), u);
  Pose v = posis(VATER, u);
  v.armV = 2.4f + sinf(u * 3) * 0.3f * (u > 12);
  v.mund = u > 4 && u < 10 ? 0.5f : 0;
  schneiderlein(g, 66, 214, 0.9f, 1, v);
  for (int i = 0; i < 3; i++) {
    float k = seg(u, 12 + i * 1.2f, 20 + i * 1.2f);
    Pose p = posis(*SOHN[i], u, i);
    p.lauf = k > 0 && k < 1 ? fmodf(u * 1.4f + i * 0.3f, 1) : -1;
    p.ranzen = true;
    schneiderlein(g, lerp(120 + i * 44, 330 + i * 44, k), 214, 0.85f, 1, p);
  }
  g.kameraAus();
  vers(g, {"Ein Schneider hatte drei Söhne.", "Eine Ziege hatte sie verleumdet,", "da mussten alle drei fort."}, aufzu(u, 0.4f, 6));
  vers(g, {"Jeder sollte in der Welt", "ein Handwerk lernen."}, aufzu(u, 6.4f, 12));
  blende(g, u, 22, 140, 150);
}

// 2) Der Aelteste beim Schreiner: Tischlein, deck dich!
static void szeneTisch(Gfx &g, float u) {
  werkstatt(g, u, 0);
  float k = seg(u, 13, 14.2f);
  tischlein(g, 160, 214, 1.2f, u > 13 ? k : 0, u);
  Pose m = posis(MEISTER, u);
  m.armV = u > 4 && u < 10 ? 1.6f : 0.5f;
  m.mund = u > 4 && u < 10 ? 0.4f : 0;
  schneiderlein(g, 60, 214, 0.95f, 1, m);
  Pose p = posis(S0, u);
  p.armV = u > 10.5f && u < 13 ? 2.2f : 0.5f;
  p.mund = u > 10.5f && u < 13 ? 0.7f : u > 14 ? 0.8f : 0;
  p.zu = u > 15 && fmodf(u, 2) < 0.6f;
  schneiderlein(g, 240, 214, 0.95f, -1, p);
  if (u > 14.2f) glitzern(g, 160, 150, 40, u, 6, (uint8_t)(255 * aufzu(u, 14.2f, 22)));
  g.kameraAus();
  vers(g, {"Der Älteste lernte beim Schreiner."}, aufzu(u, 0.4f, 4));
  blase(g, 70, 60, {"Zum Abschied schenke ich dir", "ein Tischlein. Sag nur:"}, aufzu(u, 4.2f, 10.2f), 66, 130);
  blase(g, 220, 60, {"Tischlein, deck dich!"}, aufzu(u, 10.5f, 13.4f), 236, 130);
  vers(g, {"Und schon stand das schönste", "Essen auf dem Tisch."}, aufzu(u, 15, 24));
  blende(g, u, 26, 160, 170);
}

// 3) Im Wirtshaus vertauscht der Wirt das Tischlein - daheim bleibt es leer
static void szeneWirt1(Gfx &g, float u) {
  if (u < 16) {
    stube(g, u, 1);
    // der Sohn schlaeft im Bett
    g.kontur(1.4f, OUT);
    g.rundRechteck(14, 186, 100, 16, 3, HOLZ);
    g.konturAus();
    g.rundRechteck(30, 176, 80, 14, 5, FARBE(0x5B8FD7));
    g.kreis(26, 178, 8, HAUT);
    g.ellipse(22, 172, 8, 5, S0.haar);
    zzz(g, 34, 160, u);
    float k = seg(u, 3, 13);
    // der Wirt schleicht: altes Tischlein hin, das Zaubertischlein weg
    tischlein(g, lerp(150, 150, k), 214, 1, 0, u);
    Pose w = posis(WIRT, u);
    w.lauf = fmodf(u * 0.6f, 1);
    w.armV = 1.5f; w.armH = 1.5f;
    w.boese = 0.6f;
    w.blick = 1;
    float wx = lerp(200, 300, k);
    tischlein(g, wx + 30, 170, 0.7f, 0, u);
    schneiderlein(g, wx, 214, 0.95f, 1, w);
    g.kameraAus();
    vers(g, {"Im Wirtshaus zeigte er stolz", "sein Tischlein. Doch nachts..."}, aufzu(u, 0.4f, 6));
    vers(g, {"...stellte der Wirt ein ganz", "gewöhnliches Tischlein hin."}, aufzu(u, 6.4f, 15.6f));
    return;
  }
  float v = u - 16;
  stube(g, u);
  tischlein(g, 142, 214, 1.2f, 0, u);
  Pose s = posis(S0, u);
  s.armV = v < 4 ? 2.2f : 0.5f;
  s.mund = v < 4 ? 0.7f : 0.2f;
  schneiderlein(g, 220, 214, 0.95f, -1, s);
  Pose va = posis(VATER, u);
  va.mund = v > 5 ? 0.6f : 0;
  schneiderlein(g, 60, 214, 0.9f, 1, va);
  if (v > 4) fragezeichen(g, 142, 120, 1, (uint8_t)(255 * aufzu(v, 4, 12)));
  g.kameraAus();
  blase(g, 200, 70, {"Tischlein, deck dich!"}, aufzu(v, 0.4f, 4), 214, 140);
  vers(g, {"Aber das Tischlein blieb leer.", "Alle lachten ihn aus."}, aufzu(v, 4.4f, 11.6f));
  blende(g, u, 28, 142, 170);
}

// 4) Der Zweite beim Mueller: der Goldesel - Bricklebrit!
static void szeneEsel(Gfx &g, float u) {
  werkstatt(g, u, 1);
  esel(g, 150, 214, 1.1f, 1, u, -1, u > 10 && u < 18 ? 0.5f : 0);
  if (u > 10.5f) goldregen(g, 192, 160, u - 10.5f, 18);
  for (int i = 0; i < 8; i++) if (u > 12 + i * 0.6f) muenze(g, 168 + (i % 4) * 9, 212 - (i / 4) * 5, 3.5f, i * 2.0f);
  Pose p = posis(S1, u);
  p.armV = u > 7 && u < 10.5f ? 2.4f : 0.5f;
  p.mund = u > 7 && u < 10.5f ? 0.7f : u > 12 ? 0.8f : 0;
  schneiderlein(g, 250, 214, 0.95f, -1, p);
  g.kameraAus();
  vers(g, {"Der Zweite lernte beim Müller", "und bekam einen Esel."}, aufzu(u, 0.4f, 6.6f));
  blase(g, 220, 60, {"Bricklebrit!"}, aufzu(u, 7, 10.5f), 244, 130);
  vers(g, {"Da regnete es Goldstücke!", "Aber im selben Wirtshaus..."}, aufzu(u, 12, 20));
  vers(g, {"...tauschte der Wirt auch", "den Esel aus. Daheim: kein Gold."}, aufzu(u, 20.4f, 28.6f));
  blende(g, u, 30, 150, 170);
}

// 5) Der Juengste beim Drechsler: Knueppel aus dem Sack!
static void szeneKnueppel(Gfx &g, float u) {
  if (u < 10) {
    werkstatt(g, u, 2);
    sack(g, 150, 214, 1.3f, 0);
    Pose p = posis(S2, u);
    p.armV = 1.6f;
    p.mund = 0.3f;
    schneiderlein(g, 230, 214, 0.95f, -1, p);
    g.kameraAus();
    vers(g, {"Der Jüngste lernte beim Drechsler.", "Er bekam einen Sack - und", "darin steckte ein Knüppel."}, aufzu(u, 0.4f, 9.6f));
    return;
  }
  float v = u - 10;
  stube(g, u, 1);
  // er stellt sich schlafend, der Sack unter dem Kopf
  g.kontur(1.4f, OUT);
  g.rundRechteck(14, 186, 100, 16, 3, HOLZ);
  g.konturAus();
  g.rundRechteck(30, 176, 80, 14, 5, FARBE(0x3DAE5B));
  bool raus = v > 8;
  sack(g, 24, 192, 0.7f, raus);
  g.kreis(34, 176, 8, HAUT);
  g.ellipse(30, 170, 8, 5, S2.haar);
  if (v < 6) zzz(g, 44, 158, u);
  // der Wirt schleicht heran - dann tanzt der Knueppel
  Pose w = posis(WIRT, u);
  float hops = raus ? huepf(fmodf(v * 2.2f, 1)) * 14 : 0;
  float wx = raus ? 150 + sinf(v * 1.4f) * 50 : lerp(260, 110, seg(v, 0, 6));
  w.lauf = !raus ? fmodf(v * 0.6f, 1) : fmodf(v * 2, 1);
  w.armV = raus ? 2.8f : 1.4f;
  w.armH = raus ? 2.8f : 1.2f;
  w.mund = raus ? 0.9f : 0;
  w.boese = raus ? 0 : 0.6f;
  w.zu = raus && fmodf(v, 0.8f) < 0.3f;
  schneiderlein(g, wx, 214 - hops, 0.95f, raus ? (cosf(v * 1.4f) > 0 ? 1 : -1) : -1, w);
  if (raus && v < 30) {
    float kn = v * 9;
    knueppel(g, wx + cosf(v * 3) * 20, 140 - hops + sinf(v * 6) * 10, kn);
    if (fmodf(v * 2.2f, 1) < 0.3f) for (int i = 0; i < 4; i++) g.stern(wx + cosf(i * 1.57f + v) * 18, 130 - hops + sinf(i * 1.57f + v) * 8, 3, 1.2f, 4, v * 6, GOLD);
  }
  if (v >= 30) {
    float k = seg(v, 30, 31);
    knueppel(g, lerp(wx, 26, k), lerp(140, 178, k), lerp(v * 9, 1.57f, k), 1 - k * 0.6f);
  }
  g.kameraAus();
  vers(g, {"Im Wirtshaus wollte der Wirt", "auch den Sack stehlen..."}, aufzu(v, 0.4f, 6));
  blase(g, 90, 70, {"Knüppel, aus dem Sack!"}, aufzu(v, 6.4f, 9.6f), 44, 160);
  blase(g, 200, 50, {"Au! Au! Hör auf!", "Ich gebe alles zurück!"}, aufzu(v, 14, 23), wx, 120);
  blase(g, 90, 70, {"Knüppel, in den Sack!"}, aufzu(v, 25, 29.6f), 44, 160);
  vers(g, {"Der Wirt gab Tischlein und", "Esel wieder heraus."}, aufzu(v, 30.4f, 37.6f));
  blende(g, u, 38, 140, 170);
}

// 6) Daheim: Tischlein, Esel und Knueppel - ein grosses Fest
static void szeneFest(Gfx &g, float u) {
  stube(g, u);
  // Girlande
  for (int i = 0; i < 9; i++) g.kreis(14 + i * 30, 20 + huepf(i / 8.0f) * 10, 5, i & 1 ? FARBE(0xE8473D) : GOLD);
  tischlein(g, 90, 214, 1.15f, u > 3 ? seg(u, 3, 4.2f) : 0, u);
  esel(g, 256, 214, 0.62f, -1, u, -1, u > 8 && u < 14 ? 0.5f : 0);
  if (u > 8.4f) goldregen(g, 228, 182, u - 8.4f, 14);
  const float sx[3] = {22, 152, 178};
  for (int i = 0; i < 3; i++) {
    Pose p = posis(*SOHN[i], u, i);
    p.armV = 2.4f + sinf(u * 4 + i) * 0.4f;
    p.mund = 0.6f;
    float hop = fabsf(sinf(u * 3 + i)) * 4;
    schneiderlein(g, sx[i], 214 - hop, 0.7f, i == 0 ? 1 : -1, p);
  }
  Pose v = posis(VATER, u);
  v.armV = 2.6f; v.mund = 0.8f;
  schneiderlein(g, 206, 214, 0.8f, -1, v);
  knueppel(g, 270, 110 + sinf(u * 2) * 3, 1.57f + sinf(u * 2) * 0.2f, 0.8f);
  if (u > 4) herzen(g, 142, 150, u, 5, 90);
  g.kameraAus();
  blase(g, 110, 60, {"Tischlein, deck dich!"}, aufzu(u, 0.4f, 3.4f), 60, 150);
  blase(g, 210, 60, {"Bricklebrit!"}, aufzu(u, 5, 8.2f), 196, 140);
  vers(g, {"Nun hatten sie genug zu essen", "und Gold, so viel sie wollten,", "und lebten vergnügt zusammen."}, aufzu(u, 15, 25));
  endeBild(g, u, 27);
}

static void bild(Gfx &g, float t) {
  if (t < P2) szeneAnfang(g, t - P1);
  else if (t < P3) szeneTisch(g, t - P2);
  else if (t < P4) szeneWirt1(g, t - P3);
  else if (t < P5) szeneEsel(g, t - P4);
  else if (t < P6) szeneKnueppel(g, t - P5);
  else szeneFest(g, t - P6);
}

// =====================================================================
// Musik
// =====================================================================
// Tischlein-Lied (G-Dur, froh)
static const Ton3 MEL_TISCH[] = {
  {67, .5, 43}, {71, .5, 0}, {74, .5, 0}, {71, .5, 0}, {72, .5, 48}, {76, .5, 0}, {74, 1, 0},
  {71, .5, 43}, {72, .5, 0}, {74, .5, 0}, {76, .5, 0}, {74, 1, 50}, {67, 1, 0},
  {69, .5, 50}, {71, .5, 0}, {72, .5, 0}, {74, .5, 0}, {76, .5, 48}, {74, .5, 0}, {72, .5, 0}, {69, .5, 0}, {67, 2, 43}};
static void zauberKlang(Synth &s, float x) { for (int k = 0; k < 6; k++) s.note(x + k * 0.07f, 79 + k * 2, 0.6f); }
static void ton(Synth &s, float t0, float t1) {
  float a = P1;
  spieleMelodie(s, M(MEL_TISCH), a + 0.5f, a + 21.5f, 0.3f, t0, t1, 0.75f);
  a = P2;
  spieleMelodie(s, M(MEL_WANDER), a + 0.4f, a + 10, 0.36f, t0, t1, 0.5f);
  if (bei(a + 13, t0, t1)) zauberKlang(s, a + 13);
  spieleMelodie(s, M(MEL_TISCH), a + 14.6f, a + 25.5f, 0.3f, t0, t1, 0.75f);
  a = P3;
  spieleMelodie(s, M(MEL_SPUK), a + 0.4f, a + 15.5f, 0.42f, t0, t1, 0.45f);
  if (bei(a + 20.2f, t0, t1)) { s.note(a + 20.2f, 64, 0.5f); s.note(a + 20.6f, 60, 0.5f); s.note(a + 21.0f, 55, 0.5f); }  // nichts!
  a = P4;
  spieleMelodie(s, M(MEL_WANDER), a + 0.4f, a + 7, 0.36f, t0, t1, 0.5f);
  if (bei(a + 10.5f, t0, t1)) { s.ton(a + 10.5f, 520, 0.2f, 0.01f, 0.25f, SAEGE, GERAEUSCH, 380); s.ton(a + 10.8f, 380, 0.2f, 0.01f, 0.35f, SAEGE, GERAEUSCH, 300); }  // I-aah
  for (int i = 0; i < 14; i++) if (bei(a + 10.8f + i * 0.5f, t0, t1)) s.note(a + 10.8f + i * 0.5f, 88 + (i % 3) * 3, 0.35f);  // klingeling
  spieleMelodie(s, M(MEL_SPUK), a + 20.4f, a + 29.5f, 0.42f, t0, t1, 0.4f);
  a = P5;
  spieleMelodie(s, M(MEL_WANDER), a + 0.4f, a + 9.6f, 0.36f, t0, t1, 0.5f);
  spieleMelodie(s, M(MEL_SPUK), a + 10.4f, a + 16, 0.42f, t0, t1, 0.4f);
  for (int i = 0; i < 26; i++) {  // der Knueppel tanzt: plumps, plumps
    float x = a + 18.2f + i * 0.45f;
    if (bei(x, t0, t1)) { s.ton(x, 160, 0.35f, 0.002f, 0.12f, SINUS, GERAEUSCH, 90); s.rauschen(x, 0.06f, BANDPASS, 900, 0.12f); }
  }
  spieleMelodie(s, M(MEL_JAGD), a + 18.2f, a + 34, 0.16f, t0, t1, 0.6f);
  a = P6;
  if (bei(a + 3, t0, t1)) zauberKlang(s, a + 3);
  for (int i = 0; i < 10; i++) if (bei(a + 8.8f + i * 0.5f, t0, t1)) s.note(a + 8.8f + i * 0.5f, 88 + (i % 3) * 3, 0.3f);
  spieleMelodie(s, M(MEL_TISCH), a + 4.4f, a + 26.5f, 0.26f, t0, t1, 0.8f);
  endeKlang(s, a + 27.2f, t0, t1);
}

// =====================================================================
// Startbild, Vorschau, Erkennungszeichen
// =====================================================================
static void titelSzene(Gfx &g, float t) {
  stube(g, t);
  tischlein(g, 90, 222, 1.2f, 1, t);
  esel(g, 210, 222, 0.85f, -1, t);
  knueppel(g, 150, 110 + sinf(t * 2) * 4, 1.57f + sinf(t * 2) * 0.3f);
  sack(g, 150, 222, 0.9f, 0);
  glitzern(g, 90, 150, 40, t, 5);
}
static void titelbild(Gfx &g, float t) { titelSzene(g, t); titelRahmen(g, t, "Tischlein deck dich", 200, SCHRIFT_M); }
static void vorschau(Gfx &g, float t) { titelSzene(g, t); }
static void symbol(Gfx &g, float x, float y, float t) {
  tischlein(g, x, y, 1.3f, 1, t);
  glitzern(g, x, y - 60, 34, t, 5);
}

const Maerchen TISCHLEIN = {"Tischlein deck dich", PDAUER, FARBE(0xC9742E), bild, ton, titelbild, vorschau, symbol,
                            LIED(MEL_TISCH, 0.3f), nullptr};

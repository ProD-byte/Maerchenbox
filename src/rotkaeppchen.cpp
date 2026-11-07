// Rotkaeppchen - ein Spieluhr-Film fuer die Maerchenbox, nach dem Maerchen der Brueder Grimm.
// Kuchen und Wein, der Wolf im Wald, die Blumen, die Grossmutter, "Ei, Grossmutter, was hast du fuer grosse Ohren!",
// der Jaeger, die Wackersteine. Das Fressen ist nur angedeutet.
#include "erzaehlen.h"

// ---------- Ablauf (Sekunden) ----------
static const float P1 = 0, P2 = 32, P3 = 80, P4 = 104, P5 = 134, P6 = 196, P7 = 252, PDAUER = 286;

// ---------- Aussehen ----------
static FrauLook rotLook() {
  FrauLook l; l.kleid = FARBE(0x5B7FC7); l.schuerze = WEISS; l.haar = FARBE(0xF2C14E); l.frisur = 3;
  l.tuch = FARBE(0xD8283A);
  return l;
}
static FrauLook mutterLook() {
  FrauLook l; l.kleid = FARBE(0x7A9A5A); l.schuerze = FARBE(0xF4EEDC); l.haar = FARBE(0xC98A3A); l.frisur = 2;
  return l;
}
static FrauLook omaLook() {
  FrauLook l; l.kleid = FARBE(0x9A7AB8); l.schuerze = FARBE(0xF2EEE4); l.haar = FARBE(0xE8E8E8); l.frisur = 3;
  l.tuch = WEISS;
  return l;
}
static const FrauLook ROT = rotLook(), MUTTER = mutterLook(), OMA = omaLook();
static Aussehen jaegerLook() {
  Aussehen a; a.weste = FARBE(0x4E7A3A); a.hemd = FARBE(0xE8DCC0); a.hose = FARBE(0x5A4636); a.haar = FARBE(0x6E4A2A);
  a.hut = FARBE(0x3A5A2A); a.hutD = FARBE(0x2A4220); a.feder = FARBE(0xC9A43A);
  return a;
}
static const Aussehen JAEGER = jaegerLook();

// ---------- Der Wolf (wie bei den sieben Geisslein) ----------
struct WolfPose {
  float lauf = -1;
  bool liegt = false, zu = false, naht = false;
  float mund = 0, dick = 0;
};
static void wolf(Gfx &g, float x, float y, float s, float dir, const WolfPose &p, float t) {
  const Farbe fell = FARBE(0x7E8696), hell = FARBE(0xC9CDD6), dunkel = FARBE(0x5E6474), pf = FARBE(0x2A2630);
  auto PX = [&](float dx) { return x + dx * s * dir; };
  auto PY = [&](float dy) { return y + dy * s; };
  float kx, ky, kw = 0;
  g.kontur(1.5f * s, OUT);
  if (p.liegt) {
    float atm = sinf(t * 1.6f) * 2 * s;
    for (int i = 0; i < 4; i++) {
      float bx = -24 + i * 14;
      g.linie(PX(bx), PY(-20), PX(bx + 3), PY(-40 - (i & 1) * 4), 6 * s, fell);
      g.kreis(PX(bx + 3), PY(-42 - (i & 1) * 4), 4 * s, pf);
    }
    g.kurve(PX(-36), PY(-14), PX(-56), PY(-14), PX(-62), PY(-26), 10 * s, fell);
    g.ellipse(PX(0), PY(-16), 38 * s, 14 * s, fell);
    g.ellipse(PX(2), PY(-26) - atm, (18 + p.dick * 14) * s, (8 + p.dick * 12) * s + atm, hell);
    if (p.naht)
      for (int i = 0; i < 6; i++) {
        float nx = PX(-12 + i * 5), ny = PY(-28 - p.dick * 12) - atm;
        g.linie(nx - 2 * s, ny - 3 * s, nx + 2 * s, ny + 3 * s, 1.2f * s, OUT);
      }
    kx = PX(40); ky = PY(-18); kw = 0.4f;
  } else {
    for (int i = 0; i < 4; i++) {
      float bx = i < 2 ? -24 + i * 10 : 18 + (i - 2) * 10;
      float sw = p.lauf >= 0 ? sinf(p.lauf * 6.2831853f + i * 1.6f) * 9 : 0;
      g.linie(PX(bx), PY(-28), PX(bx + sw), PY(-5), 6 * s, i & 1 ? fell : dunkel);
      g.ellipse(PX(bx + sw + 1), PY(-3), 5 * s, 3 * s, pf);
    }
    g.kurve(PX(-30), PY(-36), PX(-50), PY(-34), PX(-54), PY(-20), 10 * s, fell);
    g.ellipse(PX(0), PY(-36), 34 * s, 15 * s, fell);
    if (p.dick > 0) g.ellipse(PX(2), PY(-28), (16 + p.dick * 10) * s, (8 + p.dick * 10) * s, hell);
    kx = PX(36); ky = PY(-50);
  }
  float c = cosf(kw), sn = sinf(kw);
  auto K = [&](float dx, float dy, float *o) { o[0] = kx + (dx * c - dy * sn) * s * dir; o[1] = ky + (dx * sn + dy * c) * s; };
  float a[2], b2[2];
  K(-8, -12, a); K(4, -12, b2);
  float o1[] = {a[0] - 4 * s, a[1] + 4 * s, a[0], a[1] - 10 * s, a[0] + 5 * s, a[1] + 4 * s};
  float o2[] = {b2[0] - 4 * s, b2[1] + 4 * s, b2[0], b2[1] - 10 * s, b2[0] + 5 * s, b2[1] + 4 * s};
  g.polygon(o1, 3, fell);
  g.polygon(o2, 3, fell);
  g.kreis(kx, ky, 14 * s, fell);
  float m = p.mund;
  float sch[8];
  K(6, -4, sch); K(28, 0, sch + 2); K(28, 4 + m * 6, sch + 4); K(6, 8, sch + 6);
  g.polygon(sch, 4, fell);
  g.konturAus();
  if (m > 0.05f) {
    float u2[8];
    K(8, 6, u2); K(26, 6 + m * 8, u2 + 2); K(26, 10 + m * 10, u2 + 4); K(8, 10, u2 + 6);
    g.polygon(u2, 4, FARBE(0x7A2A2A));
    for (int i = 0; i < 3; i++) { float z[2]; K(12 + i * 5, 6 + m * 2, z); g.stern(z[0], z[1] + 1 * s, 2 * s, 0.8f * s, 2, 1.57f, WEISS); }
  }
  K(27, 1, a);
  g.kreis(a[0], a[1], 3 * s, SCHWARZ);
  K(4, -2, a);
  if (p.zu) g.bogen(a[0], a[1], 3 * s, 3.5f, 5.9f, 1.6f * s, OUT);
  else {
    g.kreis(a[0], a[1], 3 * s, FARBE(0xF2D24A));
    g.kreis(a[0] + 0.8f * s * dir, a[1], 1.4f * s, SCHWARZ);
    g.linie(a[0] - 4 * s, a[1] - 5 * s, a[0] + 4 * s, a[1] - 3 * s, 1.6f * s, OUT);
  }
}

// der Wolf im Bett der Grossmutter: Kopf mit Haube auf dem Kissen; gross = 0..4 (Ohren, Augen, Haende, Maul)
static void wolfImBett(Gfx &g, float x, float y, float t, int gross, float mund) {
  const Farbe fell = FARBE(0x7E8696), pf = FARBE(0x2A2630);
  float s = 1;
  // Ohren schauen aus der Haube
  float oh = gross >= 1 ? 1.5f : 1;
  float o1[] = {x - 12, y - 10, x - 16, y - 22 * oh, x - 4, y - 14};
  float o2[] = {x + 4, y - 14, x + 10, y - 24 * oh, x + 14, y - 10};
  g.kontur(1.3f, OUT);
  g.polygon(o1, 3, fell);
  g.polygon(o2, 3, fell);
  g.kreis(x, y, 15, fell);
  // Schnauze nach rechts
  float sch[] = {x + 6, y - 4, x + 30, y - 1, x + 30, y + 4 + mund * 8, x + 6, y + 9};
  g.polygon(sch, 4, fell);
  g.konturAus();
  if (mund > 0.05f) {
    float m[] = {x + 8, y + 6, x + 28, y + 6 + mund * 9, x + 28, y + 10 + mund * 11, x + 8, y + 10};
    g.polygon(m, 4, FARBE(0x7A2A2A));
    for (int i = 0; i < 3; i++) g.stern(x + 13 + i * 5, y + 8 + mund * 2, 2, 0.8f, 2, 1.57f, WEISS);
  }
  g.kreis(x + 29, y + 1, 3, SCHWARZ);
  // die Haube der Grossmutter
  g.kontur(1.2f, OUT);
  float h[] = {x - 17, y - 2, x - 13, y - 16, x, y - 20, x + 12, y - 16, x + 16, y - 4, x + 6, y - 11, x - 8, y - 11};
  g.polygon(h, 7, WEISS);
  g.konturAus();
  for (int i = 0; i < 5; i++) g.kreis(x - 15 + i * 7.5f, y - 2 + (i == 0 || i == 4 ? 0 : 2), 2.4f, WEISS);
  // Brille, grosse Augen
  float ag = gross >= 2 ? 1.6f : 1;
  g.kreis(x + 2, y - 3, 3.4f * ag, FARBE(0xF2D24A));
  g.kreis(x + 3, y - 3, 1.5f * ag, SCHWARZ);
  g.bogen(x + 2, y - 3, 4.5f * ag, 0, 6.29f, 1, OUT);
  // grosse Haende auf der Bettdecke
  if (gross >= 3)
    for (int i = 0; i < 2; i++) {
      g.kontur(1.1f, OUT);
      g.ellipse(x + 34 + i * 16, y + 24, 9, 6, fell);
      g.konturAus();
      for (int k = 0; k < 3; k++) g.linie(x + 39 + i * 16, y + 21 + k * 3, x + 44 + i * 16, y + 21 + k * 3, 1, pf);
    }
  (void)s;
  (void)t;
}

// ---------- Kulissen ----------
static void stube(Gfx &g, float t) {
  g.verlauf(0, 200, FARBE(0xE8D2B0), FARBE(0xD6B88E));
  g.rechteck(0, 200, 284, 40, FARBE(0x9E7656));
  g.kontur(1.8f, OUT);
  g.rundRechteck(150, 44, 90, 70, 6, HOLZ);
  g.konturAus();
  g.rundRechteck(156, 50, 78, 58, 3, FARBE(0x9FD8FF));
  laubbaum(g, 200, 108, 0.6f, FARBE(0x4CB85A));
  g.rechteck(193, 50, 4, 58, HOLZ);
  g.rechteck(146, 112, 98, 6, HOLZ_D);
  for (int i = 0; i < 3; i++) g.kreis(162 + i * 30, 108, 5, i == 1 ? FARBE(0xE8473D) : FARBE(0xF2A0C8));  // Blumentoepfe
  g.rechteck(20, 70, 90, 5, HOLZ_D);
  for (int i = 0; i < 4; i++) {
    g.kontur(1.2f, OUT);
    g.rundRechteck(26 + i * 22, 52, 16, 18, 4, i & 1 ? FARBE(0xC0632E) : FARBE(0x9AA6B8));
    g.konturAus();
  }
}

static void korb(Gfx &g, float x, float y, float s, bool blumen = false) {
  g.bogen(x, y - 8 * s, 10 * s, 3.4f, 6.0f, 2 * s, HOLZ_D);
  g.kontur(1.1f, OUT);
  float k[] = {x - 11 * s, y - 8 * s, x + 11 * s, y - 8 * s, x + 8 * s, y + 4 * s, x - 8 * s, y + 4 * s};
  g.polygon(k, 4, FARBE(0xC9944A));
  g.konturAus();
  for (int i = 0; i < 3; i++) g.linie(x - 8 * s, y - 5 * s + i * 3 * s, x + 8 * s, y - 5 * s + i * 3 * s, 0.8f * s, FARBE(0x9A6A2E));
  g.rechteck(x - 9 * s, y - 11 * s, 18 * s, 3 * s, WEISS);  // Tuch
  g.rechteck(x + 2 * s, y - 18 * s, 3.5f * s, 8 * s, FARBE(0x6E1E3A));  // Wein
  if (blumen)
    for (int i = 0; i < 4; i++) g.kreis(x - 6 * s + i * 4 * s, y - 13 * s - (i & 1) * 3 * s, 2.4f * s, i & 1 ? FARBE(0xF2A0C8) : GOLD);
}

static void wald(Gfx &g, float t, float tief = 0) {
  himmel(g, tief * 0.3f, t, 200);
  sonne(g, 248, 30, 11, t);
  Farbe tan = mischeFarbe(FARBE(0x2F8F4E), FARBE(0x1F5A45), tief);
  for (int i = 0; i < 6; i++) tanne(g, i * 52 + 4, 200, 0.8f + (i % 3) * 0.18f, tan);
  for (int i = 0; i < 2; i++) laubbaum(g, 70 + i * 140, 204, 1.1f, mischeFarbe(FARBE(0x4CB85A), FARBE(0x2E7A4A), tief));
  boden(g, 200, GRAS);
  blumen(g, 0, 214, 9, 3);
  for (int i = 0; i < 10; i++) {  // Blumen am Wegrand
    float x = 8 + i * 29, y = 222 + (i & 1) * 8;
    g.linie(x, y, x, y - 8, 1, GRAS_D);
    g.kreis(x, y - 9, 3, i % 3 == 0 ? FARBE(0xF2A0C8) : i % 3 == 1 ? GOLD : FARBE(0x8EA6F0));
  }
}

// Grossmutters Haus unter den drei Eichbaeumen, unten die Nusshecken
static void omaHaus(Gfx &g, float t, float tuerAuf) {
  himmel(g, 0, t, 200);
  sonne(g, 248, 30, 11, t);
  for (int i = 0; i < 3; i++) laubbaum(g, 30 + i * 108, 200, 1.4f, FARBE(0x3E9A4B));
  boden(g, 200);
  schornstein(g, 176, 98, t);
  hauswand(g, 96, 120, 100, 80, FARBE(0xF1DDB0), true);
  ziegeldach(g, 96, 196, 122, 42, FARBE(0x9A5A3A));
  fenster(g, 110, 138, 20, 18);
  fenster(g, 164, 138, 20, 18);
  // Tuer
  g.rundRechteck(134, 156, 26, 44, 10, FARBE(0x2E2620));
  if (tuerAuf < 0.05f) holztuer(g, 134, 200, 26, 44, HOLZ);
  else {
    g.rundRechteck(134, 156, 26 * (1 - tuerAuf * 0.8f), 44, 10, HOLZ);
    g.kreis(134 + 26 * (1 - tuerAuf * 0.8f) - 5, 180, 2, GOLD);
  }
  // Rosenstock an der Wand
  for (int i = 0; i < 9; i++) {
    float rx = 100 + (i % 3) * 5 + zufall(i + 3) * 4, ry = 160 + (i / 3) * 12;
    g.kreis(rx, ry, 4, GRAS_D);
    if (i & 1) g.kreis(rx + 1, ry - 1, 2.2f, FARBE(0xE8473D));
  }
  for (int i = 0; i < 7; i++) g.kreis(10 + i * 14, 206 - (i & 1) * 3, 9, FARBE(0x4E8A3A));  // Nusshecke
  for (int i = 0; i < 6; i++) g.kreis(214 + i * 14, 206 - (i & 1) * 3, 9, FARBE(0x4E8A3A));
}

static void omaStube(Gfx &g, float t, float vorhang) {
  g.verlauf(0, 200, FARBE(0xE6D6B6), FARBE(0xD6BE96));
  g.rechteck(0, 200, 284, 40, FARBE(0x9E7656));
  g.kontur(1.6f, OUT);
  g.rundRechteck(30, 40, 50, 60, 4, HOLZ);
  g.konturAus();
  g.rundRechteck(35, 45, 40, 50, 2, FARBE(0x9FD8FF));
  // Bett mit Vorhaengen
  g.kontur(1.6f, OUT);
  g.rechteck(120, 30, 150, 12, HOLZ_D);
  g.konturAus();
  g.rechteck(126, 190, 140, 20, HOLZ);
  g.rundRechteck(130, 140, 132, 54, 10, FARBE(0xE8E0F0));
  float vz = vorhang * 46;
  for (int i = 0; i < 2; i++) {
    float vx = i ? 270 - vz : 120;
    g.rechteck(vx, 42, vz + 10, 150, FARBE(0xC0504A));
    for (int k = 0; k < 4; k++) g.rechteck(vx + 4 + k * (vz + 6) / 4, 42, 1.6f, 150, FARBE(0x9A3A36));
  }
}

static void bettDecke(Gfx &g, float t, float atmen) {
  g.kontur(1.4f, OUT);
  g.rundRechteck(150, 156, 112, 40 + atmen, 10, FARBE(0x8FA6E8));
  g.konturAus();
  for (int i = 0; i < 5; i++) g.kreis(166 + i * 20, 172, 2.4f, FARBE(0xF2F2FF));
}

static void steine(Gfx &g, float x, float y, int n) {
  for (int i = 0; i < n; i++) steinbrocken(g, x + (i % 3) * 14, y - (i / 3) * 10, 7);
}

// =====================================================================
// Szenen
// =====================================================================
static void singen(Gfx &g, float x, float y, float t);

// 1) Die Mutter schickt Rotkaeppchen mit Kuchen und Wein zur Grossmutter
static void szeneMutter(Gfx &g, float u) {
  stube(g, u);
  FrauPose m = fposis(MUTTER, u);
  m.armV = u > 8 && u < 14 ? 1.7f : 0.6f;
  m.mund = (u > 8 && u < 14) || (u > 14.4f && u < 21) ? 0.5f : 0;
  float mhx, mhy;
  frau(g, 90, 214, 1, 1, m, &mhx, &mhy);
  if (u < 13) korb(g, mhx + 6, mhy + 4, 1);
  FrauPose r = fposis(ROT, u, 1);
  r.mund = u > 21.4f && u < 25 ? 0.6f : 0.2f;
  r.armV = u > 12 ? 1.5f : 0.8f;
  float rx = u < 26 ? 170 : lerp(170, 330, seg(u, 26, 31));
  r.lauf = u > 26 ? fmodf(u * 1.8f, 1) : -1;
  float rhx, rhy;
  frau(g, rx, 214, 0.82f, u > 26 ? 1 : -1, r, &rhx, &rhy);
  if (u > 13) korb(g, rhx + (u > 26 ? 4 : -4), rhy + 4, 0.9f);
  if (u < 7) glitzern(g, 170, 124, 16, u, 4);
  g.kameraAus();
  vers(g, {"Es war einmal eine kleine süße Dirne,", "die hatte jedermann lieb."}, aufzu(u, 0.4f, 4));
  vers(g, {"Die Großmutter schenkte ihr ein", "Käppchen von rotem Samt."}, aufzu(u, 4.2f, 8));
  blase(g, 110, 46, {"Komm, Rotkäppchen, da hast du", "Kuchen und eine Flasche Wein,", "bring das der Großmutter."}, aufzu(u, 8.2f, 14), 96, 130);
  blase(g, 110, 46, {"Geh hübsch sittsam", "und lauf nicht vom Weg ab!"}, aufzu(u, 14.4f, 21), 96, 130);
  blase(g, 190, 60, {"Ich will schon", "alles gut machen."}, aufzu(u, 21.4f, 25.4f), 176, 140);
  blende(g, u, 32, 140, 150);
}

// 2) Im Wald: der Wolf
static void szeneWolf(Gfx &g, float u) {
  wald(g, u);
  FrauPose r = fposis(ROT, u, 1);
  float rx = lerp(-30, 90, sanft(seg(u, 0, 5)));
  r.lauf = u < 5 ? fmodf(u * 1.6f, 1) : -1;
  r.mund = (u > 9 && u < 11.4f) || (u > 15.6f && u < 18) || (u > 22 && u < 25.6f) || (u > 30 && u < 37.6f) ? 0.5f : 0.15f;
  r.armV = u > 30 && u < 38 ? 1.4f : 0.9f;
  float rhx, rhy;
  frau(g, rx, 214, 0.82f, 1, r, &rhx, &rhy);
  korb(g, rhx + 4, rhy + 4, 0.9f);
  WolfPose w;
  float wx = lerp(330, 200, sanft(seg(u, 4, 8)));
  w.lauf = u > 4 && u < 8 ? fmodf(u * 1.4f, 1) : -1;
  w.mund = (u > 7.6f && u < 9) || (u > 11.6f && u < 15.4f) || (u > 18.2f && u < 21.8f) || (u > 25.8f && u < 29.8f) ||
                   (u > 38 && u < 45)
               ? 0.35f + 0.25f * fabsf(sinf(u * 8))
               : 0;
  wolf(g, wx, 214, 0.9f, -1, w, u);
  g.kameraAus();
  blase(g, 200, 70, {"Guten Tag, Rotkäppchen."}, aufzu(u, 7.6f, 9.4f), 186, 150);
  blase(g, 90, 80, {"Schönen Dank, Wolf."}, aufzu(u, 9.6f, 11.4f), 96, 140);
  blase(g, 200, 70, {"Wo hinaus so früh,", "Rotkäppchen?"}, aufzu(u, 11.6f, 15.4f), 186, 150);
  blase(g, 90, 80, {"Zur Großmutter."}, aufzu(u, 15.6f, 18), 96, 140);
  blase(g, 200, 70, {"Was trägst du", "unter der Schürze?"}, aufzu(u, 18.2f, 21.8f), 186, 150);
  blase(g, 90, 70, {"Kuchen und Wein, die kranke", "Großmutter soll sich stärken."}, aufzu(u, 22, 25.6f), 96, 140);
  blase(g, 200, 70, {"Rotkäppchen, wo wohnt", "deine Großmutter?"}, aufzu(u, 25.8f, 29.8f), 186, 150);
  blase(g, 110, 60, {"Unter den drei großen", "Eichbäumen, da steht ihr Haus,", "unten sind die Nusshecken."}, aufzu(u, 30, 37.6f), 96, 140);
  blase(g, 180, 60, {"Sieh einmal die schönen", "Blumen, die ringsumher stehen!"}, aufzu(u, 38, 45), 186, 150);
  blende(g, u, 48, 140, 150);
}

// 3) Rotkaeppchen pflueckt Blumen und laeuft immer tiefer in den Wald - der Wolf geht geradewegs zur Grossmutter
static void szeneBlumen(Gfx &g, float u) {
  wald(g, u, seg(u, 0, 20) * 0.6f);
  for (int i = 0; i < 5; i++) {
    float ph = fmodf(u * 0.2f + i * 0.2f, 1);
    float bx = fmodf(i * 64 + u * 10, 300) - 10, by = 150 + sinf(u * 2 + i) * 20;
    g.ellipse(bx - 3, by, 4, 2.5f, i & 1 ? GOLD : WEISS, 220, sinf(u * 18 + i));  // Schmetterlinge
    g.ellipse(bx + 3, by, 4, 2.5f, i & 1 ? GOLD : WEISS, 220, -sinf(u * 18 + i));
    (void)ph;
  }
  FrauPose r = fposis(ROT, u, 1);
  float rx = 60 + u * 7;
  r.armV = fmodf(u, 3) < 1.4f ? 0.3f : 1.4f;  // bueckt sich und pflueckt
  r.lauf = fmodf(u, 3) > 1.4f ? fmodf(u * 1.4f, 1) : -1;
  r.mund = 0.4f;
  float rhx, rhy;
  frau(g, rx, 214, 0.82f, 1, r, &rhx, &rhy);
  korb(g, rhx + 4, rhy + 4, 0.9f, true);
  if (fmodf(u, 3) < 1.4f) singen(g, rx + 10, 130, u);
  // der Wolf schleicht davon
  if (u < 10) {
    WolfPose w;
    w.lauf = fmodf(u * 2, 1);
    wolf(g, lerp(200, 340, seg(u, 0, 8)), 214, 0.9f, 1, w, u);
  }
  g.kameraAus();
  vers(g, {"Rotkäppchen lief vom Weg ab", "in den Wald und suchte Blumen."}, aufzu(u, 0.6f, 8));
  vers(g, {"Der Wolf aber lief geradeswegs", "nach dem Haus der Großmutter."}, aufzu(u, 9, 16));
  blende(g, u, 24, 140, 150);
}

// 4) Der Wolf an Grossmutters Tuer - nur angedeutet
static void szeneTuer(Gfx &g, float u) {
  float auf = seg(u, 13, 14.5f) * (1 - seg(u, 19, 20));
  omaHaus(g, u, auf);
  WolfPose w;
  float wx = lerp(-60, 118, sanft(seg(u, 0, 4)));
  w.lauf = u < 4 ? fmodf(u * 1.6f, 1) : -1;
  w.mund = (u > 5 && u < 9) ? 0.5f : 0;
  if (u < 15) wolf(g, wx, 214, 0.85f, 1, w, u);
  else if (u < 18) { w.lauf = fmodf(u * 1.6f, 1); g.ausschnitt(0, 0, 160, 240); wolf(g, lerp(118, 170, seg(u, 15, 18)), 214, 0.85f, 1, w, u); g.ausschnittAus(); }
  if (u > 3 && u < 5) for (int i = 0; i < 3; i++) g.stern(130, 172 - i * 6, 3, 1.2f, 4, u * 6 + i, OUT);  // klopf
  if (u > 19 && u < 24) {  // Stille: nur das Haus, ein Vogel fliegt erschrocken auf
    float k = seg(u, 19, 23);
    vogel(g, 150 + k * 90, 70 - k * 30, 0.8f, fmodf(u * 3, 1), 1);
  }
  g.kameraAus();
  blase(g, 146, 100, {"Wer ist draußen?"}, aufzu(u, 4.4f, 5.4f), 160, 150);
  blase(g, 80, 90, {"Rotkäppchen, das bringt", "Kuchen und Wein, mach auf!"}, aufzu(u, 5.6f, 9), 120, 150);
  blase(g, 170, 90, {"Drück nur auf die Klinke,", "ich bin zu schwach", "und kann nicht aufstehen."}, aufzu(u, 9.2f, 13), 160, 150);
  vers(g, {"Er ging ohne ein Wort zu sprechen", "gerade zum Bett der Großmutter ..."}, aufzu(u, 18, 25));
  blende(g, u, 30, 146, 160);
}

// 5) "Ei, Grossmutter, was hast du fuer grosse Ohren!"
static void szeneOhren(Gfx &g, float u) {
  if (u < 12) {
    omaHaus(g, u, 1);
    FrauPose r = fposis(ROT, u, 1);
    float rx = lerp(-30, 130, sanft(seg(u, 0, 6)));
    r.lauf = u < 6 ? fmodf(u * 1.5f, 1) : -1;
    r.armV = 1.2f;
    float rhx, rhy;
    frau(g, rx, 214, 0.82f, 1, r, &rhx, &rhy);
    korb(g, rhx + 4, rhy + 4, 0.9f, true);
    if (u > 6) fragezeichen(g, 150, 110, 1, (uint8_t)(255 * aufzu(u, 6, 11)));
    g.kameraAus();
    vers(g, {"Sie wunderte sich, dass die Tür", "aufstand. Wie ist mir's heute bange!"}, aufzu(u, 5.4f, 11.6f));
    blende(g, u, 62, 146, 160);
    return;
  }
  float v = u - 12;
  // die vier Fragen: Ohren, Augen, Haende und Maul werden jedes Mal groesser
  int frage = v < 10 ? 0 : v < 20 ? 1 : v < 30 ? 2 : v < 40 ? 3 : 4;
  omaStube(g, u, 1 - seg(v, 0, 2) * 0.7f);
  float mund = frage >= 4 ? 0.3f + 0.7f * seg(v, 40, 43) : (fmodf(v, 10) > 5.4f ? 0.3f + 0.2f * fabsf(sinf(v * 8)) : 0);
  wolfImBett(g, 168, 146, u, frage + 1, mund);  // was gerade gefragt wird, ist schon gross
  bettDecke(g, u, sinf(u * 2) * 1.5f);
  FrauPose r = fposis(ROT, u, 1);
  r.armV = 1.0f;
  r.mund = fmodf(v, 10) > 0.6f && fmodf(v, 10) < 5 ? 0.6f : 0.1f;
  if (frage >= 4) { r.mund = 0.9f; r.armV = 2.6f; r.armH = 2.6f; }
  float rhx, rhy;
  frau(g, 100, 214, 0.9f, 1, r, &rhx, &rhy);
  korb(g, rhx + 4, rhy + 4, 1, true);
  g.kameraAus();
  float a0 = 0.6f, b0 = 10.6f, c0 = 20.6f, d0 = 30.6f;
  blase(g, 90, 46, {"Ei, Großmutter, was hast du", "für große Ohren!"}, aufzu(v, a0, a0 + 4.4f), 96, 120);
  blase(g, 200, 60, {"Dass ich dich", "besser hören kann."}, aufzu(v, a0 + 4.8f, a0 + 9.2f), 190, 120);
  blase(g, 90, 46, {"Ei, Großmutter, was hast du", "für große Augen!"}, aufzu(v, b0, b0 + 4.4f), 96, 120);
  blase(g, 200, 60, {"Dass ich dich", "besser sehen kann."}, aufzu(v, b0 + 4.8f, b0 + 9.2f), 190, 120);
  blase(g, 90, 46, {"Ei, Großmutter, was hast du", "für große Hände!"}, aufzu(v, c0, c0 + 4.4f), 96, 120);
  blase(g, 200, 60, {"Dass ich dich", "besser packen kann."}, aufzu(v, c0 + 4.8f, c0 + 9.2f), 190, 120);
  blase(g, 100, 46, {"Aber, Großmutter, was hast du", "für ein entsetzlich großes Maul!"}, aufzu(v, d0, d0 + 4.6f), 96, 120);
  blase(g, 200, 60, {"Dass ich dich", "besser fressen kann!"}, aufzu(v, d0 + 5, d0 + 9.6f), 190, 120);
  // der Sprung - nur angedeutet: es wird schlagartig dunkel
  if (v > 41) g.rechteck(0, 0, 284, 240, 0, (uint8_t)(255 * seg(v, 41, 41.6f)));
  if (v > 44) {
    g.text(SCHRIFT_M, "Happ!", 142, 128, WEISS, 1, (uint8_t)(255 * aufzu(v, 42, 47)));
  }
}

// 6) Der Wolf schnarcht; der Jaeger; Rotkaeppchen und die Grossmutter kommen heraus; Wackersteine
static void szeneJaeger(Gfx &g, float u) {
  if (u < 14) {
    omaHaus(g, u, 1);
    Pose j = posis(JAEGER, u);
    float jx = lerp(330, 186, sanft(seg(u, 0, 5)));
    j.lauf = u < 5 ? fmodf(u * 1.5f, 1) : -1;
    j.mund = u > 6 && u < 12 ? 0.5f : 0;
    j.armV = 1.3f;
    float hx, hy;
    schneiderlein(g, jx, 214, 0.95f, -1, j, &hx, &hy);
    g.linie(hx, hy, hx + 8, hy - 30, 2.5f, HOLZ_D);  // Flinte
    zzz(g, 160, 120, u);
    g.kameraAus();
    blase(g, 160, 60, {"Wie die alte Frau schnarcht!", "Du musst doch sehen,", "ob ihr etwas fehlt."}, aufzu(u, 6, 13), jx - 6, 130);
    if (u > 13) g.rechteck(0, 0, 284, 240, 0, (uint8_t)(255 * seg(u, 13, 14)));
    return;
  }
  float v = u - 14;
  omaStube(g, u, 0.3f);
  WolfPose w;
  w.liegt = v < 34;
  w.zu = v < 30;
  w.dick = v < 12 ? 1 : 1 - seg(v, 12, 16) * 0.5f;
  w.naht = v > 20;
  if (v > 18 && v < 22) w.dick = 0.9f;  // Steine hinein
  float wx = 150;
  if (v < 34) wolf(g, wx, 214, 1, 1, w, u);
  // Jaeger mit Schere
  Pose j = posis(JAEGER, u);
  j.armV = v > 4 && v < 10 ? 1.6f + fabsf(sinf(u * 10)) * 0.3f : 0.8f;
  j.mund = (v > 0.4f && v < 4) ? 0.6f : 0.2f;
  float hx, hy;
  schneiderlein(g, 250, 214, 0.95f, -1, j, &hx, &hy);
  if (v > 4 && v < 10) {
    g.linie(hx, hy, hx - 9, hy - 6, 2, FARBE(0xC0C8D4));
    g.linie(hx, hy, hx - 9, hy + 2, 2, FARBE(0xC0C8D4));
  }
  // heraus springen Rotkaeppchen und die Grossmutter
  if (v > 7) {
    FrauPose r = fposis(ROT, u, 1);
    float k = seg(v, 7, 9);
    r.mund = v > 9 && v < 14 ? 0.6f : 0.3f;
    r.armV = 2.2f;
    frau(g, lerp(150, 70, k), 214 - huepf(k) * 40, 0.82f, 1, r);
  }
  if (v > 10) {
    FrauPose o = fposis(OMA, u);
    float k = seg(v, 10, 12);
    o.mund = 0.3f;
    o.armV = 1.6f;
    frau(g, lerp(150, 30, k), 214 - huepf(k) * 30, 0.85f, 1, o);
  }
  if (v > 15 && v < 22) steine(g, 120, 200, (int)((v - 15) * 1.2f) + 1);
  // der Wolf wacht auf, will fort, faellt um - nur ein Plumps und Staub
  if (v > 30) {
    WolfPose w2;
    w2.dick = 0.9f;
    w2.mund = 0.4f;
    float k = seg(v, 30, 33);
    if (v < 34) {}
    else {
      w2.lauf = v < 36 ? fmodf(u * 1.5f, 1) : -1;
      float x2 = lerp(150, 210, seg(v, 34, 36));
      if (v < 36.4f) wolf(g, x2, 214, 1, 1, w2, u);
      if (v > 36) { wolkePuff(g, 210, 196, seg(v, 36, 38), FARBE(0xC9B8A0), 1.4f); }
      if (v > 36.4f) { WolfPose w3; w3.liegt = true; w3.zu = true; w3.dick = 0.8f; wolf(g, 210, 214, 1, 1, w3, 0); }
    }
    (void)k;
  }
  if (v > 36 && v < 36.5f) g.wackeln(sinf(u * 90) * 4, 3);
  g.kameraAus();
  if (v < 1) g.rechteck(0, 0, 284, 240, 0, (uint8_t)(255 * (1 - v)));
  blase(g, 180, 46, {"Finde ich dich hier,", "du alter Sünder!"}, aufzu(v, 0.4f, 4), 240, 130);
  blase(g, 90, 46, {"Ach, wie war ich erschrocken,", "wie war's so dunkel", "in dem Wolf seinem Leib!"}, aufzu(v, 9.4f, 15), 70, 130);
  vers(g, {"Rotkäppchen holte geschwind", "große Steine herbei."}, aufzu(v, 15.4f, 22));
  vers(g, {"Als der Wolf aufwachte, wollte er", "fortspringen, aber die Steine ..."}, aufzu(v, 30.4f, 36));
  vers(g, {"... waren so schwer,", "dass er gleich niedersank."}, aufzu(v, 36.4f, 41.6f));
  blende(g, u, 56, 150, 150);
}

// 7) Kuchen und Wein; "Du willst dein Lebtag nicht wieder allein vom Wege ab in den Wald laufen"
static void szeneSchluss(Gfx &g, float u) {
  omaStube(g, u, 0.2f);
  g.kontur(1.4f, OUT);
  g.rundRechteck(70, 170, 120, 8, 3, HOLZ);
  g.konturAus();
  g.rechteck(80, 178, 6, 36, HOLZ_D);
  g.rechteck(174, 178, 6, 36, HOLZ_D);
  // Kuchen und Wein auf dem Tisch
  g.ellipse(110, 166, 18, 5, FARBE(0xC9884A));
  g.rechteck(94, 160, 32, 6, FARBE(0xE0A050));
  g.rechteck(140, 148, 7, 20, FARBE(0x6E1E3A));
  g.rechteck(150, 156, 6, 12, FARBE(0xE8E0F0));
  FrauPose o = fposis(OMA, u);
  o.mund = 0.4f + 0.2f * fabsf(sinf(u * 3));
  o.armV = 1.6f;
  frau(g, 50, 214, 0.95f, 1, o);
  FrauPose r = fposis(ROT, u, 1);
  r.mund = u > 12 && u < 22 ? 0.5f : 0.3f;
  r.armV = 1.4f;
  frau(g, 210, 214, 0.85f, -1, r);
  Pose j = posis(JAEGER, u);
  j.mund = 0.4f;
  j.armV = 2.2f;
  schneiderlein(g, 254, 214, 0.95f, -1, j);
  herzen(g, 130, 140, u, 5, 40);
  g.kameraAus();
  vers(g, {"Da waren alle drei vergnügt.", "Die Großmutter aß den Kuchen."}, aufzu(u, 0.6f, 9));
  blase(g, 150, 60, {"Du willst dein Lebtag nicht", "wieder allein vom Wege ab", "in den Wald laufen!"}, aufzu(u, 10, 22), 210, 130);
  endeBild(g, u, 28);
  if (u < 1.2f) g.iris(142, 150, lerp(0, 330, sanft(u / 1.2f)));
}

static void singen(Gfx &g, float x, float y, float t) {
  for (int i = 0; i < 3; i++) {
    float ph = fmodf(t * 0.45f + i / 3.0f, 1);
    notenzeichen(g, x + ph * 26 + sinf(ph * 8) * 4, y - ph * 46, i & 1 ? FARBE(0xB28DFF) : FARBE(0xE8473D), (uint8_t)(230 * huepf(ph)));
  }
}

static void bild(Gfx &g, float t) {
  if (t < P2) szeneMutter(g, t - P1);
  else if (t < P3) szeneWolf(g, t - P2);
  else if (t < P4) szeneBlumen(g, t - P3);
  else if (t < P5) szeneTuer(g, t - P4);
  else if (t < P6) szeneOhren(g, t - P5);
  else if (t < P7) szeneJaeger(g, t - P6);
  else szeneSchluss(g, t - P7);
}

// =====================================================================
// Musik
// =====================================================================
// Rotkaeppchens Lied (G-Dur, huepfend)
static const Ton3 MEL_ROT[] = {
  {67, .5, 43}, {71, .5, 0}, {74, .5, 0}, {71, .5, 0}, {72, .5, 48}, {76, .5, 0}, {74, 1, 0},
  {72, .5, 45}, {71, .5, 0}, {69, .5, 0}, {71, .5, 0}, {74, .5, 43}, {72, .5, 0}, {71, 1, 0},
  {67, .5, 43}, {71, .5, 0}, {74, .5, 0}, {79, .5, 0}, {78, .5, 50}, {76, .5, 0}, {74, .5, 0}, {72, .5, 0},
  {71, .5, 43}, {69, .5, 0}, {67, 1, 0}};
// der Wolf (e-Moll, schleichend)
static const Ton3 MEL_WOLF[] = {
  {52, 1, 40}, {0, .5, 0}, {55, .5, 0}, {54, 1, 0}, {52, 1, 0}, {55, 1, 40}, {0, .5, 0}, {59, .5, 0}, {58, 1, 0}, {55, 1, 0},
  {52, 1, 40}, {54, .5, 0}, {55, .5, 0}, {57, 1, 0}, {54, 1, 0}, {52, 2, 40}};

static void klopf(Synth &s, float x) { for (int i = 0; i < 3; i++) s.ton(x + i * 0.3f, 160, 0.35f, 0.002f, 0.12f, SINUS, GERAEUSCH, 90); }

static void ton(Synth &s, float t0, float t1) {
  float a = P1;
  spieleMelodie(s, M(MEL_ROT), a + 0.5f, a + 31.5f, 0.3f, t0, t1, 0.8f);
  a = P2;
  spieleMelodie(s, M(MEL_ROT), a + 0.4f, a + 7, 0.3f, t0, t1, 0.75f);
  spieleMelodie(s, M(MEL_WOLF), a + 7.4f, a + 47.5f, 0.36f, t0, t1, 0.75f);
  a = P3;
  spieleMelodie(s, M(MEL_ROT), a + 0.4f, a + 23.5f, 0.26f, t0, t1, 0.8f);
  for (int i = 0; i < 6; i++) if (bei(a + 2 + i * 3.4f, t0, t1)) s.glocke(a + 2 + i * 3.4f, 84 + (i % 3) * 3, 0.3f);  // Blume gepflueckt
  a = P4;
  spieleMelodie(s, M(MEL_WOLF), a + 0.4f, a + 4, 0.3f, t0, t1, 0.8f);
  if (bei(a + 3.2f, t0, t1)) klopf(s, a + 3.2f);
  spieleMelodie(s, M(MEL_WOLF), a + 9, a + 18, 0.36f, t0, t1, 0.75f);
  if (bei(a + 13, t0, t1)) s.ton(a + 13, 300, 0.12f, 0.01f, 0.5f, SAEGE, GERAEUSCH, 200);  // die Tuer knarrt
  spieleMelodie(s, M(MEL_SPUK), a + 19, a + 29.5f, 0.36f, t0, t1, 0.7f);
  a = P5;
  spieleMelodie(s, M(MEL_ROT), a + 0.4f, a + 5.4f, 0.3f, t0, t1, 0.7f);
  spieleMelodie(s, M(MEL_SPUK), a + 6, a + 12, 0.36f, t0, t1, 0.7f);
  for (int f = 0; f < 4; f++) {
    float b = a + 12 + f * 10;
    spieleMelodie(s, M(MEL_WOLF), b + 0.2f, b + 9.8f, 0.3f + f * 0.02f, t0, t1, 0.6f + f * 0.08f);
    if (bei(b + 5.2f, t0, t1)) for (int k = 0; k < 3; k++) s.note(b + 5.2f + k * 0.12f, 52 + f * 3 - k * 2, 0.7f);
  }
  if (bei(a + 53, t0, t1)) { s.ton(a + 53, 90, 0.4f, 0.002f, 0.5f, SINUS, GERAEUSCH, 40); s.rauschen(a + 53, 0.5f, TIEFPASS, 1500, 0.3f); }  // Happ
  a = P6;
  for (float x = ceilf(t0 / 1.8f) * 1.8f; x < t1; x += 1.8f)
    if (x > a && x < a + 44) { s.rauschen(x, 0.8f, TIEFPASS, 400, 0.08f, 0.7f, 0.3f); }  // Schnarchen
  spieleMelodie(s, M(MEL_JAGD), a + 0.4f, a + 13, 0.2f, t0, t1, 0.7f);
  for (int i = 0; i < 4; i++) if (bei(a + 18 + i * 0.6f, t0, t1)) s.rauschen(a + 18 + i * 0.6f, 0.08f, HOCHPASS, 3000, 0.1f);  // Schere
  if (bei(a + 21, t0, t1)) for (int k = 0; k < 8; k++) s.glocke(a + 21 + k * 0.12f, 72 + k * 2, 0.5f);  // heraus!
  if (bei(a + 24, t0, t1)) for (int k = 0; k < 8; k++) s.glocke(a + 24 + k * 0.12f, 67 + k * 2, 0.5f);
  for (int i = 0; i < 5; i++) if (bei(a + 29.6f + i * 1.2f, t0, t1)) s.ton(a + 29.6f + i * 1.2f, 120, 0.25f, 0.002f, 0.15f, SINUS, GERAEUSCH, 80);  // Steine
  if (bei(a + 50, t0, t1)) { s.ton(a + 50, 100, 0.4f, 0.002f, 0.4f, SINUS, GERAEUSCH, 45); s.rauschen(a + 50, 0.5f, TIEFPASS, 900, 0.25f); }  // plumps
  spieleMelodie(s, M(MEL_ROT), a + 51, a + 55.5f, 0.3f, t0, t1, 0.7f);
  a = P7;
  spieleMelodie(s, M(MEL_F), a + 0.4f, a + 30, 0.26f, t0, t1, 0.85f);
  endeKlang(s, a + 30.2f, t0, t1);
}

// =====================================================================
// Startbild, Vorschau, Erkennungszeichen
// =====================================================================
static void titelSzene(Gfx &g, float t) {
  wald(g, t);
  FrauPose r = fposis(ROT, t, 1);
  r.armV = 1.2f;
  r.mund = 0.3f;
  float hx, hy;
  frau(g, 150, 222, 1, 1, r, &hx, &hy);
  korb(g, hx + 4, hy + 4, 1.1f, true);
  WolfPose w;
  w.mund = 0.2f;
  wolf(g, 250, 222, 0.8f, -1, w, t);
}
static void titelbild(Gfx &g, float t) { titelSzene(g, t); titelRahmen(g, t, "Rotkäppchen", 190); }
static void vorschau(Gfx &g, float t) { titelSzene(g, t); }
static void symbol(Gfx &g, float x, float y, float t) {
  // die rote Kappe und der Korb mit Kuchen und Wein
  FrauPose r = fposis(ROT, t, 1);
  r.armV = 1.3f;
  r.mund = 0.3f;
  float hx, hy;
  frau(g, x - 14, y, 0.78f, 1, r, &hx, &hy);
  korb(g, hx + 6, hy + 6, 1.2f, true);
  for (int i = 0; i < 3; i++) {
    float bx = x + 34 + i * 10;
    g.linie(bx, y, bx, y - 12, 1, GRAS_D);
    g.kreis(bx, y - 13, 3.5f, i == 1 ? GOLD : FARBE(0xF2A0C8));
  }
}

const Maerchen ROTKAEPPCHEN = {"Rotkäppchen", PDAUER, FARBE(0xD8283A), bild, ton, titelbild, vorschau, symbol,
                               LIED(MEL_ROT, 0.32f), nullptr};

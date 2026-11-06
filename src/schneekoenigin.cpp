// Die Schneekoenigin - ein Spieluhr-Film fuer die Maerchenbox, nach Hans Christian Andersen (1844).
// Der Spiegel des Trolls, Kay und Gerda zwischen den Rosen in der Dachrinne, die Splitter in Auge und Herz,
// der grosse weisse Schlitten, die roten Schuhe im Fluss, der Blumengarten, Kraehe, Prinz und Prinzessin,
// das Raeubermaedchen und das Rentier, Lappin und Finnin, das Eisschloss und das Wort "Ewigkeit" -
// und Gerdas heisse Traenen, die Kays Herz auftauen.
#include "erzaehlen.h"

// ---------- Ablauf (Sekunden) ----------
static const float P1 = 0, P2 = 26, P3 = 52, P4 = 78, P5 = 104, P6 = 126, P7 = 156, P8 = 182, P9 = 212, P10 = 242,
                   P11 = 270, P12 = 296, P13 = 324, P14 = 346, P15 = 370, P16 = 402, P17 = 426, PDAUER = 450;

// ---------- Aussehen ----------
static Aussehen kayLook(bool kalt) {
  Aussehen a; a.weste = FARBE(0x3E6EB8); a.hemd = FARBE(0xF6F2EA); a.hose = FARBE(0x5A4A3A); a.haar = FARBE(0xE0A23C);
  a.muetze = 1;
  if (kalt) { a.haut = FARBE(0xC8D8F0); a.wangen = false; a.weste = FARBE(0x6E8EB8); }
  return a;
}
static Aussehen prinzLook() {
  Aussehen a; a.weste = FARBE(0x7A2A8E); a.hemd = WEISS; a.hose = FARBE(0x2E2A5A); a.haar = FARBE(0x3A2A20); a.muetze = 3;
  return a;
}
static Aussehen raeuberLook(int i) {
  Aussehen a; a.weste = i & 1 ? FARBE(0x5A3A3A) : FARBE(0x3A4A3A); a.hemd = FARBE(0xC9B8A0); a.hose = FARBE(0x2E2A30);
  a.haar = FARBE(0x2E2220); a.hut = FARBE(0x1E1A22); a.hutD = FARBE(0x141016); a.feder = FARBE(0xC0392B); a.wangen = false;
  return a;
}
static const Aussehen KAY = kayLook(false), KAY_KALT = kayLook(true), PRINZ = prinzLook(), R0 = raeuberLook(0), R1 = raeuberLook(1);
static FrauLook gerdaLook() {
  FrauLook l; l.kleid = FARBE(0x5A8ED8); l.schuerze = WEISS; l.haar = FARBE(0xC98A3A); l.frisur = 1; l.schuh = FARBE(0xE8283F);
  return l;
}
static FrauLook gerdaBarfuss() { FrauLook l = gerdaLook(); l.schuh = HAUT; return l; }
static FrauLook gerdaReise() {
  FrauLook l = gerdaLook(); l.kleid = FARBE(0xC0392B); l.schuerze = 0; l.tuch = FARBE(0xF2E8D0); l.schuh = FARBE(0x6E4A2E);
  return l;
}
static FrauLook omaLook() { FrauLook l; l.kleid = FARBE(0x4A3A5A); l.schuerze = FARBE(0xD8D0C0); l.haar = FARBE(0xE0E0E0); l.frisur = 2; return l; }
static FrauLook koeniginLook() {
  FrauLook l; l.kleid = FARBE(0xA8D0F4); l.haar = FARBE(0xF6FAFF); l.haut = FARBE(0xEEF4FF); l.frisur = 0; l.krone = true;
  l.glanz = true; l.rockBreite = 1.35f; l.wangen = false;
  return l;
}
static FrauLook blumenfrauLook() {
  FrauLook l; l.kleid = FARBE(0x8E6EA8); l.schuerze = FARBE(0xF2E8D0); l.haar = FARBE(0xD0D0D0); l.frisur = 2; l.blume = FARBE(0xE8473D);
  return l;
}
static FrauLook prinzessinLook() {
  FrauLook l; l.kleid = FARBE(0xFF8FC0); l.haar = FARBE(0xF2C14E); l.krone = true; l.glanz = true; l.rockBreite = 1.2f;
  return l;
}
static FrauLook raeubermaedchenLook() {
  FrauLook l; l.kleid = FARBE(0x7A5A3A); l.schuerze = FARBE(0xC0392B); l.haar = FARBE(0x2A1E16); l.frisur = 4; l.flicken = true;
  return l;
}
static FrauLook lappinLook() {
  FrauLook l; l.kleid = FARBE(0x3E5EA8); l.schuerze = FARBE(0xE8473D); l.haar = FARBE(0xA8A8A8); l.frisur = 3; l.tuch = FARBE(0xE8473D);
  return l;
}
static FrauLook finninLook() {
  FrauLook l; l.kleid = FARBE(0x6E5A3A); l.haar = FARBE(0xC8C8C8); l.frisur = 2; l.wangen = true;
  return l;
}
static const FrauLook GERDA = gerdaLook(), GERDA_BARFUSS = gerdaBarfuss(), GERDA_REISE = gerdaReise(), OMA = omaLook(),
                      KOENIGIN = koeniginLook(), BLUMENFRAU = blumenfrauLook(), PRINZESSIN = prinzessinLook(),
                      RMAEDCHEN = raeubermaedchenLook(), LAPPIN = lappinLook(), FINNIN = finninLook();

static const Farbe EIS = FARBE(0x9FD0F8), EIS_H = FARBE(0xDCF0FF), EIS_D = FARBE(0x5E9AD8), ROSE = FARBE(0xE8405A);

// =====================================================================
// Figuren
// =====================================================================
static void kay(Gfx &g, float x, float y, float s, float dir, float t, float mund, float armV = 0.4f, float lauf = -1,
                bool kalt = false, float boese = 0, float armH = 0.2f, bool zu = false) {
  Pose p = posis(kalt ? KAY_KALT : KAY, t, 1);
  p.mund = mund;
  p.armV = armV;
  p.armH = armH;
  p.lauf = lauf;
  p.boese = boese;
  p.zu = zu || p.zu;
  schneiderlein(g, x, y, s, dir, p);
}
static void gerda(Gfx &g, float x, float y, float s, float dir, float t, float mund, float armV = 0.4f, float lauf = -1,
                  const FrauLook *L = &GERDA, bool weint = false, float armH = 0.2f, bool zu = false) {
  FrauPose p = fposis(*L, t);
  p.mund = mund;
  p.armV = armV;
  p.armH = armH;
  p.lauf = lauf;
  p.weint = weint;
  p.zu = zu || p.zu;
  frau(g, x, y, s, dir, p);
}
static void frauX(Gfx &g, const FrauLook &L, float x, float y, float s, float dir, float t, float mund, float armV = 0.4f,
                  float armH = 0.2f, float lauf = -1) {
  FrauPose p = fposis(L, t, 2);
  p.mund = mund;
  p.armV = armV;
  p.armH = armH;
  p.lauf = lauf;
  frau(g, x, y, s, dir, p);
}
static void koenigin(Gfx &g, float x, float y, float s, float dir, float t, float mund = 0, float armV = 0.6f, uint8_t a = 255) {
  g.kreis(x, y - 50 * s, 60 * s, EIS_H, (uint8_t)(a * 0.25f));
  frauX(g, KOENIGIN, x, y, s, dir, t, mund, armV, 0.4f);
  for (int i = 0; i < 5; i++) g.stern(x + (i - 2) * 6 * s, y - 92 * s - (i == 2 ? 6 : 0) * s, 4 * s, 1.4f * s, 6, t + i, WEISS);
  for (int i = 0; i < 4; i++) {
    float k = fmodf(t * 0.5f + i * 0.25f, 1);
    g.stern(x + cosf(i * 1.7f + t) * 30 * s, y - 20 * s - k * 70 * s, 3 * s, 1.2f * s, 6, t, WEISS, (uint8_t)(200 * huepf(k)));
  }
}
static void troll(Gfx &g, float x, float y, float s, float dir, float t, float lacht) {
  auto PX = [&](float d) { return x + d * s * dir; };
  auto PY = [&](float d) { return y + d * s; };
  const Farbe rot = FARBE(0xC0392B), dunkel = FARBE(0x8E2A2A);
  g.kurve(PX(-10), PY(-10), PX(-26), PY(-4), PX(-24), PY(-20), 2.4f * s, dunkel);
  g.kontur(1.2f * s, OUT);
  g.ellipse(PX(0), PY(-14), 12 * s, 14 * s, rot);
  g.kreis(PX(4), PY(-34), 11 * s, rot);
  g.konturAus();
  float h1[] = {PX(-4), PY(-42), PX(-8), PY(-56), PX(2), PY(-44)};
  float h2[] = {PX(8), PY(-42), PX(14), PY(-56), PX(14), PY(-40)};
  g.polygon(h1, 3, FARBE(0xF2E8D0));
  g.polygon(h2, 3, FARBE(0xF2E8D0));
  g.kreis(PX(2), PY(-36), 2, GOLD);
  g.kreis(PX(9), PY(-36), 2, GOLD);
  g.ellipse(PX(6), PY(-27), 5 * s, (1 + lacht * 3) * s, FARBE(0x3A1A1A));
  g.stern(PX(-26), PY(-22), 3 * s, 1 * s, 3, 0, dunkel);
}
static void kraehe(Gfx &g, float x, float y, float s, float dir, float t, float mund, float flug = -1) {
  auto PX = [&](float d) { return x + d * s * dir; };
  auto PY = [&](float d) { return y + d * s; };
  const Farbe sw = FARBE(0x2A2A3A);
  float f = flug >= 0 ? sinf(flug * 14) : 0.3f;
  g.linie(PX(-2), PY(-2), PX(-2), PY(4), 1.6f * s, FARBE(0x5A4A3A));
  g.linie(PX(3), PY(-2), PX(3), PY(4), 1.6f * s, FARBE(0x5A4A3A));
  float sch[] = {PX(-10), PY(-8), PX(-22), PY(-4), PX(-10), PY(-2)};
  g.polygon(sch, 3, sw);
  g.ellipse(PX(0), PY(-8), 11 * s, 7 * s, sw);
  float fl[] = {PX(-4), PY(-10), PX(-14), PY(-18 - f * 10), PX(6), PY(-10)};
  g.polygon(fl, 3, FARBE(0x3A3A4E));
  g.kreis(PX(9), PY(-15), 6 * s, sw);
  float sn[] = {PX(13), PY(-17), PX(21), PY(-15 + mund * 2), PX(13), PY(-12)};
  g.polygon(sn, 3, FARBE(0x5A5A5A));
  if (mund > 0.3f) { float sn2[] = {PX(13), PY(-14), PX(20), PY(-11), PX(13), PY(-12)}; g.polygon(sn2, 3, FARBE(0x5A5A5A)); }
  g.kreis(PX(10), PY(-17), 1.4f * s, WEISS);
}
static void rentier(Gfx &g, float x, float y, float s, float dir, float t, float lauf = -1) {
  auto PX = [&](float d) { return x + d * s * dir; };
  auto PY = [&](float d) { return y + d * s; };
  const Farbe fell = FARBE(0x9A6E4A), hell = FARBE(0xE8D8C0), dunkel = FARBE(0x6E4A2E);
  g.kontur(1.4f * s, OUT);
  for (int i = 0; i < 4; i++) {
    float bx = i < 2 ? -22 + i * 9 : 16 + (i - 2) * 9;
    float sw = lauf >= 0 ? sinf(lauf * 6.2832f + i * 1.6f) * 9 : 0;
    g.linie(PX(bx), PY(-34), PX(bx + sw), PY(-4), 5 * s, i & 1 ? fell : dunkel);
  }
  g.ellipse(PX(0), PY(-44), 32 * s, 15 * s, fell);
  g.ellipse(PX(-2), PY(-36), 22 * s, 6 * s, hell);
  float hx = PX(30), hy = PY(-66);
  g.linie(PX(22), PY(-50), hx, hy, 12 * s, fell);
  g.ellipse(hx + 8 * s * dir, hy + 2 * s, 12 * s, 7 * s, fell, 255, 0.3f * dir);
  g.konturAus();
  g.ellipse(hx + 12 * s * dir, hy + 4 * s, 5 * s, 4 * s, dunkel);
  g.kreis(hx + 4 * s * dir, hy - 2 * s, 2 * s, SCHWARZ);
  // Geweih
  for (int k = -1; k <= 1; k += 2) {
    float bx = hx + k * 3 * s;
    g.linie(bx, hy - 6 * s, bx - 10 * s * dir + k * 4 * s, hy - 30 * s, 2.2f * s, FARBE(0xD8C49A));
    g.linie(bx - 4 * s * dir, hy - 16 * s, bx + 6 * s * dir, hy - 24 * s, 1.8f * s, FARBE(0xD8C49A));
    g.linie(bx - 8 * s * dir, hy - 24 * s, bx - 18 * s * dir, hy - 30 * s, 1.8f * s, FARBE(0xD8C49A));
  }
  g.ellipse(PX(-30), PY(-48), 5 * s, 4 * s, hell);
}
static void engel(Gfx &g, float x, float y, float s, float t, uint8_t a = 255) {
  float f = sinf(t * 10) * 0.4f;
  g.ellipse(x - 8 * s, y - 14 * s, 8 * s, 4 * s, EIS_H, a, -0.6f - f);
  g.ellipse(x + 8 * s, y - 14 * s, 8 * s, 4 * s, EIS_H, a, 0.6f + f);
  float k[] = {x - 7 * s, y, x + 7 * s, y, x + 3 * s, y - 16 * s, x - 3 * s, y - 16 * s};
  g.polygon(k, 4, WEISS, a);
  g.kreis(x, y - 21 * s, 5 * s, HAUT, a);
  g.bogen(x, y - 30 * s, 5 * s, 0, 6.29f, 1.4f * s, GOLD, a);
}

// =====================================================================
// Dinge und Orte
// =====================================================================
static void rosenkasten(Gfx &g, float x, float y, float t, float bluehen = 1) {
  g.kontur(1.4f, OUT);
  g.rechteck(x - 26, y - 14, 52, 14, HOLZ);
  g.konturAus();
  for (int i = 0; i < 4; i++) g.kurve(x - 18 + i * 12, y - 14, x - 22 + i * 12, y - 30, x - 16 + i * 12, y - 40, 2, FARBE(0x3E8E4E));
  if (bluehen > 0) for (int i = 0; i < 4; i++) {
    float rx = x - 16 + i * 12, ry = y - 40 + (i & 1) * 6;
    g.kreis(rx, ry, 5 * bluehen, ROSE);
    g.kreis(rx, ry, 2.4f * bluehen, FARBE(0xFF8FA0));
  }
}
static void schneeFall(Gfx &g, float t, int n, float wind = 0, uint8_t a = 230) {
  for (int i = 0; i < n; i++) {
    float x = fmodf(zufall(i + 400) * 300 + t * (8 + wind * 40) + sinf(t + i) * 6, 300) - 8;
    float y = fmodf(zufall(i + 440) * 240 + t * (18 + zufall(i) * 14), 250) - 6;
    g.kreis(x, y, 1.3f + zufall(i + 7) * 1.2f, WEISS, a);
  }
}
static void nordlicht(Gfx &g, float t) {
  const Farbe c[] = {FARBE(0x6EF0A8), FARBE(0x7EC8F8), FARBE(0xB88EF0)};
  for (int k = 0; k < 3; k++)
    for (int i = 0; i < 20; i++) {
      float x = i * 15, y = 40 + k * 16 + sinf(t * 0.7f + i * 0.4f + k) * 14;
      g.ellipse(x, y, 10, 18, c[k], 60);
    }
}

// die beiden Dachstuben mit den Rosen in der Dachrinne dazwischen
static void dachstuben(Gfx &g, float t, float winter = 0, float rosen = 1) {
  himmel(g, winter * 0.5f, t, 240);
  if (winter < 0.5f) sonne(g, 142, 30, 10, t);
  Farbe w = FARBE(0xE8C8A0), d = FARBE(0xA8452E);
  g.kontur(1.8f, OUT);
  g.rechteck(-10, 60, 100, 180, w);
  g.rechteck(194, 60, 100, 180, w);
  float d1[] = {-10, 62, 90, 62, 40, 20};
  float d2[] = {194, 62, 294, 62, 244, 20};
  g.polygon(d1, 3, d);
  g.polygon(d2, 3, d);
  g.konturAus();
  if (winter > 0) { float s1[] = {-10, 62, 40, 20, 90, 62, 40, 30}; g.polygon(s1, 4, WEISS); float s2[] = {194, 62, 244, 20, 294, 62, 244, 30}; g.polygon(s2, 4, WEISS); }
  // Fenster
  g.rundRechteck(40, 100, 46, 50, 4, winter > 0 ? FARBE(0xD8ECFF) : FARBE(0x9FD8FF));
  g.rundRechteck(198, 100, 46, 50, 4, winter > 0 ? FARBE(0xD8ECFF) : FARBE(0x9FD8FF));
  // Dachrinne mit den Rosenkaesten
  g.rechteck(84, 168, 116, 8, STEIN_D);
  if (winter < 0.5f) {
    rosenkasten(g, 116, 168, t, rosen);
    rosenkasten(g, 168, 168, t, rosen);
  } else {
    g.rechteck(90, 154, 104, 14, HOLZ);
    g.rechteck(90, 150, 104, 6, WEISS);
  }
}

static void winterstube(Gfx &g, float t) {
  g.verlauf(0, 200, FARBE(0xC8A47A), FARBE(0xA8845A));
  g.rechteck(0, 200, 284, 40, FARBE(0x7A5A3A));
  g.kontur(1.6f, OUT);
  g.rundRechteck(150, 50, 100, 90, 4, HOLZ);
  g.konturAus();
  g.rundRechteck(155, 55, 90, 80, 2, FARBE(0x3A4A7A));
  for (int i = 0; i < 6; i++) g.stern(162 + (i % 3) * 30, 64 + (i / 3) * 56, 10, 3, 6, i, EIS_H, 230);
  g.rechteck(198, 55, 4, 80, HOLZ);
  // Ofen
  g.kontur(1.6f, OUT);
  g.rundRechteck(14, 110, 50, 92, 6, FARBE(0x5A5A6E));
  g.konturAus();
  g.ellipse(39, 176, 12, 6 + sinf(t * 7), FARBE(0xFF8A2A));
}

static void marktplatz(Gfx &g, float t) {
  himmel(g, 0.3f, t, 200);
  for (int i = 0; i < 5; i++) {
    float x = i * 64 - 10, h = 80 + (i % 3) * 20;
    Farbe c = i % 3 == 0 ? FARBE(0xE8C8A0) : i % 3 == 1 ? FARBE(0xC8D8E8) : FARBE(0xE8B0A0);
    g.kontur(1.4f, OUT);
    g.rechteck(x, 200 - h, 56, h, c);
    float d[] = {x - 4, 200 - h, x + 28, 200 - h - 30, x + 60, 200 - h};
    g.polygon(d, 3, FARBE(0x8E3A2E));
    g.konturAus();
    float s[] = {x - 4, 200 - h, x + 28, 200 - h - 30, x + 60, 200 - h, x + 28, 200 - h - 20};
    g.polygon(s, 4, WEISS);
    g.rundRechteck(x + 18, 200 - h + 24, 20, 18, 3, FARBE(0xFFE8A0));
  }
  g.rechteck(0, 200, 284, 40, FARBE(0xF2F6FF));
  schneeFall(g, t, 30);
}
static void grosserSchlitten(Gfx &g, float x, float y, float s, float t, float lauf) {
  // weisse Pferde
  einhorn(g, x + 80 * s, y, 0.6f * s, lauf, 1, 0, 0, true);
  einhorn(g, x + 110 * s, y - 4, 0.56f * s, lauf >= 0 ? fmodf(lauf + 0.3f, 1) : -1, 1, 0, 0, true);
  g.kontur(1.6f * s, OUT);
  g.rundRechteck(x - 40 * s, y - 44 * s, 80 * s, 32 * s, 10 * s, WEISS);
  g.konturAus();
  g.rundRechteck(x - 36 * s, y - 40 * s, 72 * s, 8 * s, 4 * s, EIS_H);
  g.linie(x - 44 * s, y - 4 * s, x + 44 * s, y - 4 * s, 3 * s, EIS_D);
  g.bogen(x + 44 * s, y - 14 * s, 10 * s, -1.57f, 1.57f, 3 * s, EIS_D);
  g.linie(x + 40 * s, y - 34 * s, x + 64 * s, y - 40 * s, 1.6f * s, EIS_D);
}

static void fluss(Gfx &g, float t, float scroll = 0) {
  himmel(g, 0, t, 150);
  sonne(g, 236, 30, 11, t);
  for (int i = 0; i < 5; i++) laubbaum(g, fmodf(i * 70 - scroll * 0.4f + 700, 350) - 40, 152, 0.6f + (i & 1) * 0.15f);
  g.rechteck(0, 148, 284, 20, GRAS);
  g.verlauf(166, 240, FARBE(0x5AA8E8), FARBE(0x3A7AC8));
  for (int i = 0; i < 10; i++) g.linie(fmodf(i * 37 + t * 20, 300) - 10, 180 + (i % 4) * 14, fmodf(i * 37 + t * 20, 300) + 10, 180 + (i % 4) * 14, 1.6f, WEISS, 120);
}
static void boot(Gfx &g, float x, float y, float t) {
  float w = sinf(t * 2) * 2;
  g.kontur(1.6f, OUT);
  float b[] = {x - 36, y - 14 + w, x + 36, y - 14 - w, x + 26, y + w, x - 26, y - w};
  g.polygon(b, 4, HOLZ);
  g.konturAus();
  g.linie(x - 34, y - 12 + w, x + 34, y - 12 - w, 2, HOLZ_D);
}

static void blumengarten(Gfx &g, float t, float herbst = 0) {
  himmel(g, 0, t, 200);
  sonne(g, 240, 32, 11, t);
  boden(g, 196, mischeFarbe(GRAS, FARBE(0xB89A4A), herbst));
  // Haeuschen mit roten und blauen Fenstern
  g.kontur(1.6f, OUT);
  g.rechteck(170, 110, 100, 90, FARBE(0xF0DCC0));
  float d[] = {160, 112, 220, 66, 280, 112};
  g.polygon(d, 3, FARBE(0xC8A040));
  g.konturAus();
  g.rundRechteck(184, 130, 22, 22, 3, FARBE(0xE8405A));
  g.rundRechteck(236, 130, 22, 22, 3, FARBE(0x3E7FDC));
  laubbaum(g, 40, 200, 0.9f, mischeFarbe(FARBE(0x4CB85A), FARBE(0xD88A3A), herbst));
  for (int i = 0; i < 8; i++) g.kreis(20 + zufall(i) * 44, 120 + zufall(i + 5) * 30, 3.4f, FARBE(0xC0203A));  // Kirschen
  for (int i = 0; i < 18; i++) {
    float x = 10 + i * 15.5f, y = 210 + (i & 1) * 10;
    Farbe c = i % 4 == 0 ? FARBE(0xFFD54A) : i % 4 == 1 ? FARBE(0xB28DFF) : i % 4 == 2 ? FARBE(0xFF9AC0) : WEISS;
    g.linie(x, y + 10, x, y, 1.4f, GRAS_D);
    g.kreis(x, y, 4 * (1 - herbst * 0.6f), c);
  }
}

static void wald(Gfx &g, float t, float nacht) {
  himmel(g, nacht, t, 200);
  if (nacht > 0.5f) mond(g, 236, 36, 10);
  for (int i = 0; i < 7; i++) tanne(g, i * 48 - 20, 204, 0.9f + (i % 3) * 0.15f, mischeFarbe(FARBE(0x2F8F4E), FARBE(0x1F3B45), nacht));
  boden(g, 200, mischeFarbe(GRAS, FARBE(0x2F4A40), nacht));
  g.rechteck(0, 208, 284, 14, mischeFarbe(FARBE(0xC9B48A), FARBE(0x4A4A50), nacht));
}
static void kutsche(Gfx &g, float x, float y, float s, float t, float lauf) {
  einhorn(g, x + 70 * s, y, 0.6f * s, lauf, 1, 0, 0, true);
  g.kontur(1.6f * s, OUT);
  g.rundRechteck(x - 34 * s, y - 64 * s, 68 * s, 46 * s, 12 * s, GOLD);
  g.konturAus();
  g.rundRechteck(x - 20 * s, y - 56 * s, 22 * s, 20 * s, 4 * s, FARBE(0x9FD8FF));
  g.stern(x + 18 * s, y - 42 * s, 6 * s, 2.4f * s, 5, 0, FARBE(0xE8283F));
  for (int i = -1; i <= 1; i += 2) {
    float rx = x + i * 22 * s;
    g.bogen(rx, y - 12 * s, 11 * s, 0, 6.29f, 2.4f * s, HOLZ_D);
    for (int k = 0; k < 4; k++) { float a = lauf * 6.28f + k * 0.785f; g.linie(rx, y - 12 * s, rx + cosf(a) * 10 * s, y - 12 * s + sinf(a) * 10 * s, 1.4f * s, HOLZ_D); }
  }
}

static void raeuberburg(Gfx &g, float t) {
  g.verlauf(0, 206, FARBE(0x5A4A44), FARBE(0x3A302A));
  for (int r = 0; r < 6; r++)
    for (int c = 0; c < 8; c++) g.rundRechteck(c * 40 + (r & 1) * 20 - 20, r * 34, 37, 31, 4, FARBE(0x6E5E54), 120);
  g.rechteck(0, 206, 284, 34, FARBE(0x3A2E26));
  // Feuer in der Mitte
  float x = 142, y = 222;
  for (int i = 0; i < 3; i++) g.linie(x - 12 + i * 8, y, x - 6 + i * 6, y - 6, 3, HOLZ_D);
  for (int i = 0; i < 3; i++) {
    float h = 16 + 6 * sinf(t * 9 + i * 1.7f);
    float f[] = {x - 8 + i * 8 - 5, y - 2, x - 8 + i * 8 + 5, y - 2, x - 8 + i * 8 + sinf(t * 7 + i) * 2, y - 2 - h};
    g.polygon(f, 3, i == 1 ? FARBE(0xFFD54A) : FARBE(0xFF8A2A));
  }
  g.kreis(x, y - 14, 60, FARBE(0xFFB050), 36);
  // Stange mit den Waldtauben
  g.rechteck(16, 96, 110, 5, HOLZ_D);
}

static void schneefeld(Gfx &g, float t, float scroll = 0) {
  himmel(g, 1, t, 200);
  for (int i = 0; i < 16; i++) g.stern(zufall(i + 700) * 284, zufall(i + 720) * 110, 2, 0.8f, 4, 0, WEISS, 180);
  nordlicht(g, t);
  float b[] = {0, 210, 60, 150, 120, 180, 190, 140, 284, 190, 284, 240, 0, 240};
  g.polygon(b, 7, FARBE(0xC8D8EC));
  g.rechteck(0, 200, 284, 40, FARBE(0xEEF4FF));
  for (int i = 0; i < 5; i++) g.ellipse(fmodf(i * 80 - scroll + 800, 400) - 60, 214 + (i & 1) * 10, 40, 5, FARBE(0xD8E4F4));
}
static void huette(Gfx &g, float t, bool finnin) {
  g.verlauf(0, 206, finnin ? FARBE(0x8E5A3A) : FARBE(0x6E5A4A), finnin ? FARBE(0x6E4A2E) : FARBE(0x5A4A3A));
  g.rechteck(0, 206, 284, 34, FARBE(0x4A3A2E));
  g.kontur(1.6f, OUT);
  g.rundRechteck(196, 120, 70, 86, 8, FARBE(0x8E8A86));
  g.konturAus();
  g.rundRechteck(208, 160, 46, 40, 14, FARBE(0x2E2620));
  for (int i = 0; i < 3; i++) {
    float h = 12 + 5 * sinf(t * 9 + i * 1.7f);
    float f[] = {218 + i * 13 - 6, 198, 218 + i * 13 + 6, 198, 218 + i * 13, 198 - h};
    g.polygon(f, 3, FARBE(0xFF8A2A));
  }
  if (finnin) g.kreis(142, 120, 160, FARBE(0xFF9A5A), 30);
  // Lampe aus Tran
  g.ellipse(80, 120, 10, 4, FARBE(0x8E7A5A));
  g.ellipse(80, 114, 3, 5 + sinf(t * 8), FARBE(0xFFB83D));
}
static void stockfisch(Gfx &g, float x, float y, float s, bool schrift) {
  g.kontur(1 * s, OUT);
  g.ellipse(x, y, 16 * s, 6 * s, FARBE(0xD8C8A8));
  g.konturAus();
  float sw[] = {x - 14 * s, y, x - 24 * s, y - 6 * s, x - 24 * s, y + 6 * s};
  g.polygon(sw, 3, FARBE(0xD8C8A8));
  g.kreis(x + 10 * s, y - 1 * s, 1.4f * s, OUT);
  if (schrift) for (int i = 0; i < 3; i++) g.linie(x - 8 * s, y - 2 * s + i * 2 * s, x + 4 * s, y - 2 * s + i * 2 * s, 0.8f * s, FARBE(0x5A3A2A));
}

static void eisschloss(Gfx &g, float t, float tauen = 0) {
  g.verlauf(0, 240, mischeFarbe(FARBE(0x2A3A6E), FARBE(0x5A8AC8), tauen), mischeFarbe(FARBE(0x5A7AB8), FARBE(0xA8D0F0), tauen));
  nordlicht(g, t);
  for (int i = 0; i < 6; i++) {  // Eissaeulen
    float x = 10 + i * 54;
    g.rechteck(x, 40, 18, 170, EIS, 160);
    g.rechteck(x + 4, 40, 4, 170, EIS_H, 160);
    float sp[] = {x - 4, 40, x + 9, 6, x + 22, 40};
    g.polygon(sp, 3, EIS_H, 180);
  }
  // der zugefrorene See ("Spiegel des Verstandes"), in tausend Stuecke gesprungen
  g.ellipse(142, 222, 150, 26, mischeFarbe(FARBE(0xBFE0FA), FARBE(0x8EC8F0), tauen));
  for (int i = 0; i < 14; i++) {
    float x = 20 + zufall(i + 3) * 244, y = 206 + zufall(i + 30) * 30;
    g.linie(x, y, x + 12, y + 4, 1, EIS_D, 160);
  }
}

// =====================================================================
// Szenen
// =====================================================================
// 1) Der Spiegel des Trolls zerspringt
static void szeneSpiegel(Gfx &g, float u) {
  himmel(g, 1, u, 240);
  for (int i = 0; i < 24; i++) g.stern(zufall(i + 700) * 284, zufall(i + 720) * 240, 2, 0.8f, 4, 0, WEISS, 180);
  float hoch = seg(u, 8, 14);
  float bruch = seg(u, 14, 16);
  float x = 142, y = lerp(210, 150, hoch);
  if (bruch <= 0) {
    // der Spiegel: alles Schoene darin wird haesslich (verzerrte Blume)
    g.kontur(1.6f, OUT);
    g.rundRechteck(x - 26, y - 70, 52, 66, 20, FARBE(0x8E7A5A));
    g.konturAus();
    g.rundRechteck(x - 21, y - 65, 42, 56, 16, FARBE(0xA8B8C8));
    g.ellipse(x, y - 36, 6, 14, FARBE(0x6E8A5A), 255, sinf(u * 3));
    g.ellipse(x + 4, y - 50, 10, 5, FARBE(0x7A5A6E), 255, 0.6f);
    troll(g, x + 40, y - 10, 1, -1, u, u < 8 ? 0.5f + 0.4f * sinf(u * 10) : 0.4f);
    troll(g, x - 40, y - 10, 0.8f, 1, u, 0.6f);
  } else {
    for (int i = 0; i < 40; i++) {
      float a = zufall(i) * 6.28f, r = bruch * (30 + zufall(i + 9) * 100) + (u - 16) * 5;
      float sx = x + cosf(a) * r, sy = 150 + sinf(a) * r * 0.45f + (u - 16) * 4;
      float sp[] = {sx, sy - 3, sx + 3, sy + 2, sx - 2, sy + 3};
      g.polygon(sp, 3, FARBE(0xD8E8F8));
      g.stern(sx, sy, 2.4f, 0.8f, 4, u * 3 + i, WEISS, 160);
    }
    if (bruch < 1) g.kreis(x, y - 36, 50 * (1 - bruch), WEISS, (uint8_t)(200 * (1 - bruch)));
  }
  g.kameraAus();
  vers(g, {"Ein böser Troll machte einen", "Spiegel: Alles Gute und Schöne", "sah darin hässlich aus."}, aufzu(u, 0.4f, 7.6f));
  vers(g, {"Damit flogen die Trolle hinauf", "zum Himmel - da zersprang er."}, aufzu(u, 8, 14));
  vers(g, {"Die Splitter flogen in die Welt.", "Wem einer ins Auge kommt, der", "sieht nur noch das Schlechte."}, aufzu(u, 16, 25.6f));
}

// 2) Kay und Gerda zwischen den Rosen
static void szeneRosen(Gfx &g, float u) {
  dachstuben(g, u);
  kay(g, 112, 168, 0.62f, 1, u, u > 14 && u < 24 ? 0.4f + 0.2f * sinf(u * 6) : 0.3f, 1.2f);
  gerda(g, 172, 168, 0.6f, -1, u, u > 14 && u < 24 ? 0.4f + 0.2f * sinf(u * 6 + 1) : 0.3f, 1.2f);
  if (u > 14) for (int i = 0; i < 3; i++) {
    float ph = fmodf(u * 0.5f + i * 0.33f, 1);
    notenzeichen(g, 142 + sinf(ph * 6) * 20, 100 - ph * 50, FARBE(0xE8405A), (uint8_t)(230 * huepf(ph)));
  }
  g.kameraAus();
  vers(g, {"Kay und Gerda wohnten in zwei", "Dachstuben einander gegenüber."}, aufzu(u, 0.4f, 6.6f));
  vers(g, {"In der Dachrinne dazwischen", "wuchsen Rosen. Dort spielten", "die beiden jeden Sommer."}, aufzu(u, 7, 13.6f));
  vers(g, {"Die Rosen blühn und verwehen,", "das Christkind werden wir sehen!"}, aufzu(u, 14, 25.6f));
}

// 3) Die Grossmutter erzaehlt - die Schneekoenigin am Fenster
static void szeneErzaehlt(Gfx &g, float u) {
  winterstube(g, u);
  frauX(g, OMA, 90, 222, 0.85f, 1, u, u > 0.4f && u < 12 ? 0.4f + 0.2f * sinf(u * 8) : 0, 1);
  kay(g, 140, 222, 0.75f, 1, u, u > 12.4f && u < 16 ? 0.5f : 0.2f, u > 18 ? 1.8f : 0.4f);
  float k = seg(u, 18, 21);
  if (k > 0) {
    g.ausschnitt(155, 55, 90, 80);
    koenigin(g, 210, 150, 0.8f, -1, u, 0, 1.4f, (uint8_t)(255 * k));
    g.ausschnittAus();
  }
  g.kameraAus();
  blase(g, 110, 50, {"Das sind die weißen Bienen,", "die schwärmen. Ihre Königin", "ist die Schneekönigin."}, aufzu(u, 0.4f, 12), 90, 140);
  blase(g, 150, 50, {"Soll sie nur kommen, ich setz", "sie auf den heißen Ofen!"}, aufzu(u, 12.4f, 17.6f), 140, 150);
  vers(g, {"Am Abend sah Kay draußen eine", "Frau aus lauter glitzerndem Eis.", "Sie nickte ihm zu..."}, aufzu(u, 18, 25.6f));
}

// 4) Die Splitter in Auge und Herz
static void szeneSplitter(Gfx &g, float u) {
  dachstuben(g, u);
  bool getroffen = u > 6;
  kay(g, 112, 168, 0.62f, 1, u, u > 6 && u < 12 ? 0.9f : 0.2f, u > 6 && u < 12 ? 2.4f : 0.4f, -1, false, u > 12 ? 1 : 0, 0.2f, u > 6 && u < 9);
  gerda(g, 172, 168, 0.6f, -1, u, u > 12 ? 0.6f : 0.3f, 0.4f, -1, &GERDA, u > 18);
  for (int i = 0; i < 2; i++) {
    float k = seg(u, 4 + i * 2, 5.4f + i * 2);
    if (k > 0 && k < 1) {
      float sx = lerp(10, 118, k), sy = lerp(20, 128 + i * 12, k);
      g.stern(sx, sy, 3, 1, 4, u * 6, WEISS);
    }
  }
  (void)getroffen;
  g.kameraAus();
  vers(g, {"Im Sommer, als sie gerade", "Bilder ansahen..."}, aufzu(u, 0.4f, 4));
  blase(g, 90, 50, {"Au! Es stach mich ins Herz!", "Und jetzt flog mir etwas", "ins Auge!"}, aufzu(u, 6.4f, 12), 112, 110);
  vers(g, {"Es waren zwei Splitter vom", "Zauberspiegel. Kays Herz wurde", "wie ein Eisklumpen."}, aufzu(u, 12.4f, 18));
  blase(g, 190, 50, {"Kay, warum bist du", "auf einmal so böse?"}, aufzu(u, 18.4f, 25.6f), 172, 110);
}

// 5) Kay wird boese
static void szeneBoese(Gfx &g, float u) {
  dachstuben(g, u, 0, 1 - seg(u, 4, 7) * 0.7f);
  kay(g, 112, 168, 0.62f, 1, u, u > 0.4f && u < 7 ? 0.6f + 0.2f * sinf(u * 8) : 0.3f, u > 3 && u < 7 ? 2 + sinf(u * 10) * 0.5f : 0.4f, -1, false, 1);
  gerda(g, 172, 168, 0.6f, -1, u, 0.2f, 0.4f, -1, &GERDA, true);
  if (u > 4 && u < 9) for (int i = 0; i < 4; i++) {
    float k = fmodf(u * 0.6f + i * 0.25f, 1);
    g.kreis(120 + i * 6 + k * 30, 128 + k * 60, 3, ROSE, (uint8_t)(255 * (1 - k)));
  }
  g.kameraAus();
  blase(g, 100, 50, {"Pfui, die Rose ist ja", "wurmstichig! Hässlich!"}, aufzu(u, 0.4f, 7), 112, 110);
  vers(g, {"Er riss die Rosen ab, äffte die", "Großmutter nach und lachte", "über alles, was Gerda liebte."}, aufzu(u, 7.4f, 15.6f));
  vers(g, {"Das machten die Splitter", "in seinem Auge und Herzen."}, aufzu(u, 16, 21.6f));
}

// 6) Der grosse weisse Schlitten
static void szeneSchlitten(Gfx &g, float u) {
  if (u > 20) {
    himmel(g, 0.6f, u, 200);
    g.rechteck(0, 200, 284, 40, FARBE(0xF2F6FF));
    schneeFall(g, u, 50, 1.5f);
    grosserSchlitten(g, 70, 222, 1, u, -1);
    float kuss = seg(u, 22, 24);
    koenigin(g, 170, 222, 0.9f, -1, u, 0, 1.2f + kuss * 0.6f);
    kay(g, 120, 222, 0.75f, 1, u, kuss > 0.5f ? 0 : 0.5f, 0.4f, -1, kuss > 0.5f, 0, 0.2f, kuss > 0 && kuss < 1);
    if (kuss > 0 && kuss < 1) puff(g, 130, 150, kuss, WEISS);
    g.kameraAus();
    vers(g, {"Darin saß die Schneekönigin.", "Sie küsste Kay auf die Stirn -", "da vergaß er Gerda und alle daheim."}, aufzu(u, 20, 29.6f));
    return;
  }
  marktplatz(g, u);
  float k = seg(u, 2, 6), weg = seg(u, 14, 20);
  float sx = weg > 0 ? lerp(120, 360, hinein(weg)) : lerp(-120, 120, k);
  float lauf = (k < 1 || weg > 0) ? fmodf(u * 1.6f, 1) : -1;
  grosserSchlitten(g, sx, 222, 1, u, lauf);
  koenigin(g, sx - 6, 188, 0.6f, 1, u, u > 20 && u < 26 ? 0.3f : 0, 0.6f);
  // Kays kleiner Schlitten, an den grossen gebunden
  float kx = weg > 0 ? sx - 76 : (u < 8 ? 40 : lerp(40, sx - 76, seg(u, 8, 11)));
  g.linie(kx + 18, 210, sx - 40, 214, 1.2f, HOLZ_D);
  g.kontur(1.2f, OUT);
  g.rundRechteck(kx - 18, 206, 36, 10, 3, FARBE(0xC0392B));
  g.konturAus();
  g.linie(kx - 20, 218, kx + 20, 218, 2, HOLZ_D);
  kay(g, kx, 208, 0.6f, 1, u, u > 14 ? 0.8f : 0.4f, u > 14 ? 2.6f : 1.2f, -1, u > 22);
  g.kameraAus();
  vers(g, {"Im Winter band Kay seinen Schlitten", "an einen großen weißen Schlitten."}, aufzu(u, 0.4f, 7.6f));
  vers(g, {"Der fuhr schneller und schneller,", "zum Tor hinaus, durch das", "Schneegestöber."}, aufzu(u, 8, 14.6f));
  vers(g, {"Darin saß die Schneekönigin.", "Sie küsste Kay auf die Stirn -", "da vergaß er Gerda und alle daheim."}, aufzu(u, 20, 29.6f));
}

// 7) Fruehling: Gerda schenkt dem Fluss ihre roten Schuhe
static void szeneFluss(Gfx &g, float u) {
  fluss(g, u, u > 14 ? (u - 14) * 30 : 0);
  bool imBoot = u > 12;
  if (!imBoot) {
    gerda(g, 110, 160, 0.75f, 1, u, u > 0.4f && u < 7 ? 0.4f + 0.2f * sinf(u * 8) : 0.2f, u > 7 && u < 9 ? 2.2f : 0.4f, -1,
          u > 8 ? &GERDA_BARFUSS : &GERDA);
    float wk = seg(u, 8, 10);
    if (wk > 0) for (int i = 0; i < 2; i++) {
      float x = lerp(124, 170 + i * 10, wk), y = lerp(150, 196, wk) - huepf(wk) * 30;
      g.ellipse(x, y + fmodf(u * 4, 1), 6, 3, FARBE(0xE8283F));
    }
    boot(g, 190, 196, u);
  } else {
    float k = seg(u, 12, 14);
    boot(g, lerp(190, 150, k), 196, u);
    gerda(g, lerp(190, 150, k), 186, 0.6f, 1, u, 0.3f, 0.4f, -1, &GERDA_BARFUSS, u > 16 && u < 20);
  }
  g.kameraAus();
  blase(g, 110, 40, {"Fluss, hast du meinen Kay?", "Ich schenk dir meine roten", "Schuhe, gib ihn mir wieder!"}, aufzu(u, 0.4f, 7), 110, 90);
  vers(g, {"Gerda stieg in ein Boot -", "da trieb es fort, den Fluss hinab."}, aufzu(u, 12.4f, 18));
  vers(g, {"Vielleicht bringt mich der Fluss", "zu Kay, dachte sie."}, aufzu(u, 18.4f, 25.6f));
}

// 8) Der Blumengarten der alten Frau
static void szeneGarten(Gfx &g, float u) {
  float herbst = seg(u, 24, 28);
  blumengarten(g, u, herbst);
  bool flieht = u > 24;
  frauX(g, BLUMENFRAU, 210, 222, 0.85f, -1, u, (u > 0.4f && u < 6) ? 0.5f + 0.2f * sinf(u * 8) : 0, u > 6 && u < 12 ? 1.6f + sinf(u * 6) * 0.3f : 0.4f);
  // ihr grosser Hut mit Blumen
  g.ellipse(210, 152, 22, 5, FARBE(0xE8C860));
  for (int i = 0; i < 4; i++) g.kreis(198 + i * 8, 148, 3.4f, i == 1 ? ROSE : i & 1 ? FARBE(0xB28DFF) : FARBE(0xFFD54A));
  float gx = flieht ? lerp(150, -30, seg(u, 25, 30)) : 150;
  gerda(g, gx, 222, 0.75f, flieht ? -1 : 1, u, (u > 16.4f && u < 24) ? 0.4f : 0.2f, 0.4f, flieht ? fmodf(u * 1.6f, 1) : -1,
        &GERDA_BARFUSS, u > 13 && u < 17);
  float tr = seg(u, 14, 16);
  if (tr > 0) rosenkasten(g, 106, 222, u, tr);
  g.kameraAus();
  blase(g, 190, 50, {"Komm, du armes Kind! Ich", "kämme dir dein Haar."}, aufzu(u, 0.4f, 6), 210, 140);
  vers(g, {"Beim Kämmen vergaß Gerda Kay.", "Die Alte ließ alle Rosen in", "die Erde versinken."}, aufzu(u, 6.4f, 13));
  vers(g, {"Doch an ihrem Hut sah Gerda eine", "Rose. Sie weinte - da wuchs", "der Rosenbusch wieder empor."}, aufzu(u, 13.4f, 19));
  blase(g, 110, 60, {"Kay ist nicht tot - wir", "waren unter der Erde!"}, aufzu(u, 19.4f, 24), 106, 170);
  vers(g, {"Gerda lief fort. Es war", "schon Herbst geworden."}, aufzu(u, 25, 29.6f));
}

// 9) Die Kraehe, das Schloss, Prinz und Prinzessin
static void szeneSchloss(Gfx &g, float u) {
  if (u < 14) {
    wald(g, u, 0.2f);
    kraehe(g, 190, 222, 1.6f, -1, u, (u > 0.4f && u < 13) ? 0.5f + 0.5f * sinf(u * 9) : 0);
    gerda(g, 112, 222, 0.75f, 1, u, 0.3f, 0.4f, -1, &GERDA_BARFUSS);
    g.kameraAus();
    blase(g, 190, 60, {"Krah! Krah! Im Schloss wohnt", "eine kluge Prinzessin. Sie hat", "einen Burschen geheiratet..."}, aufzu(u, 0.4f, 7), 196, 170);
    blase(g, 190, 60, {"...mit Stiefeln, die knarrten,", "und langem Haar - wie dein Kay!"}, aufzu(u, 7.4f, 13), 196, 170);
    return;
  }
  float v = u - 14;
  himmel(g, 1, u, 240);
  g.verlauf(0, 240, FARBE(0x3A2A5A), FARBE(0x5A3A6E));
  // zwei Betten wie Lilien
  for (int i = 0; i < 2; i++) {
    float x = 90 + i * 110;
    g.linie(x, 220, x, 160, 3, GRAS_D);
    float b[] = {x - 30, 160, x, 130, x + 30, 160, x, 176};
    g.polygon(b, 4, i ? FARBE(0xFF8FC0) : WEISS);
  }
  if (v < 8) {
    g.kreis(98, 150, 8, HAUT);  // der Nacken im Bett...
    g.ellipse(94, 144, 9, 6, FARBE(0x3A2A20));
  }
  float auf = seg(v, 6, 8);
  if (auf > 0) {
    Pose p = posis(PRINZ, u);
    p.mund = 0.4f;
    schneiderlein(g, 90, 222, 0.8f, 1, p);
    frauX(g, PRINZESSIN, 200, 222, 0.8f, -1, u, v > 8.4f && v < 14 ? 0.5f : 0.3f, 1.4f);
  }
  gerda(g, 146, 222, 0.75f, 1, u, 0, 0.4f, -1, &GERDA_BARFUSS, v > 4 && v < 10);
  g.kameraAus();
  vers(g, {"Nachts schlich Gerda ins Schloss.", "Aber es war nicht Kay."}, aufzu(v, 0.4f, 6));
  blase(g, 200, 50, {"Armes Kind! Du bekommst Stiefel,", "einen Muff und eine goldene", "Kutsche für deinen Weg."}, aufzu(v, 8.4f, 15.6f), 200, 140);
}

// 10) Die Raeuber - das kleine Raeubermaedchen
static void szeneRaeuber(Gfx &g, float u) {
  if (u < 12) {
    wald(g, u, 0.6f);
    float k = seg(u, 0, 4);
    kutsche(g, lerp(-80, 110, k), 222, 1, u, k < 1 ? fmodf(u * 1.6f, 1) : -1);
    if (u > 5) {
      float rk = seg(u, 5, 7);
      Pose a = posis(R0, u); a.armV = 2.4f; a.mund = 0.7f; a.boese = 1;
      Pose b = posis(R1, u, 1); b.armV = 2.2f; b.mund = 0.6f; b.boese = 1;
      schneiderlein(g, lerp(330, 210, rk), 222, 0.85f, -1, a);
      schneiderlein(g, lerp(370, 250, rk), 222, 0.85f, -1, b);
    }
    g.kameraAus();
    vers(g, {"Im dunklen Wald blitzte die goldene", "Kutsche - da kamen die Räuber!"}, aufzu(u, 0.4f, 11.6f));
    return;
  }
  float v = u - 12;
  raeuberburg(g, u);
  for (int i = 0; i < 4; i++) taube(g, 34 + i * 26, 96, 0.6f, 1, -1, 0);
  rentier(g, 236, 222, 0.8f, -1, u);
  frauX(g, RMAEDCHEN, 160, 222, 0.8f, -1, u, (v > 0.4f && v < 9) ? 0.5f + 0.3f * sinf(u * 8) : 0.2f, 1.4f);
  gerda(g, 100, 222, 0.75f, 1, u, v > 9.4f ? 0.4f : 0.2f, 0.4f, -1, &GERDA_REISE);
  g.kameraAus();
  blase(g, 180, 50, {"Die gehört mir! Sie soll", "mit mir spielen und", "bei mir schlafen!"}, aufzu(v, 0.4f, 9), 160, 140);
  vers(g, {"Gerda erzählte dem Räubermädchen", "von Kay - und wie lieb sie ihn hat."}, aufzu(v, 9.4f, 15.6f));
}

// 11) Die Tauben wissen es - das Rentier kennt den Weg
static void szeneRentier(Gfx &g, float u) {
  raeuberburg(g, u);
  for (int i = 0; i < 4; i++) taube(g, 34 + i * 26, 96, 0.6f, 1, -1, i == 1 && u < 8 ? fabsf(sinf(u * 6)) : 0);
  bool los = u > 18;
  rentier(g, 236, 222, 0.8f, -1, u, -1);
  frauX(g, RMAEDCHEN, 160, 222, 0.8f, -1, u, (u > 12.4f && u < 18) ? 0.5f + 0.3f * sinf(u * 8) : 0.2f, u > 12 ? 1.8f : 0.4f);
  gerda(g, 100, 222, 0.75f, 1, u, 0.3f, 0.4f, -1, &GERDA_REISE, u > 18.4f);
  g.kameraAus();
  blase(g, 90, 140, {"Ruckedigu! Wir sahen Kay im", "Schlitten der Schneekönigin -", "nach Lappland flog sie!"}, aufzu(u, 0.4f, 7), 60, 96);
  blase(g, 200, 60, {"Dort ist es herrlich! Dort springt", "man frei in den glitzernden Tälern."}, aufzu(u, 7.4f, 12), 250, 150);
  blase(g, 150, 50, {"Trag sie nach Lappland,", "Rentier - zum Schloss der", "Schneekönigin!"}, aufzu(u, 12.4f, 18), 160, 140);
  vers(g, {"Das Räubermädchen band das", "Rentier los. Gerda weinte vor Freude."}, aufzu(u, 18.4f, 25.6f));
  (void)los;
}

// 12) Die Lappin und die Finnin
static void szeneLappland(Gfx &g, float u) {
  if (u < 6) {
    schneefeld(g, u, u * 80);
    rentier(g, 150, 222, 0.85f, 1, u, fmodf(u * 1.8f, 1));
    gerda(g, 140, 188, 0.6f, 1, u, 0.3f, 1.4f, -1, &GERDA_REISE);
    g.kameraAus();
    vers(g, {"Fort ging es durch Schnee und", "Nacht, unter dem Nordlicht."}, aufzu(u, 0.4f, 5.8f));
    return;
  }
  float v = u - 6;
  bool finnin = v > 8;
  huette(g, u, finnin);
  rentier(g, 236, 222, 0.75f, -1, u);
  gerda(g, 110, 222, 0.75f, 1, u, 0.2f, 0.4f, -1, &GERDA_REISE);
  frauX(g, finnin ? FINNIN : LAPPIN, 166, 222, 0.8f, -1, u,
        (v > 0.4f && v < 7.6f) || (v > 10.4f && v < 21.6f) ? 0.5f + 0.2f * sinf(u * 8) : 0, finnin ? 1 : 1.4f);
  if (!finnin) stockfisch(g, 150, 180, 1, true);
  g.kameraAus();
  vers(g, {"Die Lappin schrieb ein paar Worte", "auf einen Stockfisch - für die Finnin."}, aufzu(v, 0.4f, 7.6f));
  blase(g, 180, 50, {"Ich kann ihr keine größere Macht", "geben, als sie schon hat."}, aufzu(v, 10.4f, 15.8f), 166, 140);
  blase(g, 180, 50, {"Ihre Macht sitzt in ihrem Herzen,", "weil sie ein liebes Kind ist."}, aufzu(v, 16.2f, 21.6f), 166, 140);
}

// 13) Die Schneeflocken-Wache und die Engel
static void szeneEngel(Gfx &g, float u) {
  schneefeld(g, u);
  // die Schneeflocken als Ungeheuer (Igel, Schlangen, kleine Baeren - nur angedeutet als grosse Flocken)
  float weg = seg(u, 10, 16);
  for (int i = 0; i < 8; i++) {
    float a = i * 0.785f + u * 0.5f;
    float r = 70 + 20 * sinf(u + i) + weg * 120;
    g.stern(142 + cosf(a) * r, 150 + sinf(a) * r * 0.5f, 12, 4, 6, u + i, FARBE(0xE8F2FF), (uint8_t)(230 * (1 - weg)));
    g.kreis(142 + cosf(a) * r - 3, 150 + sinf(a) * r * 0.5f - 2, 1.6f, FARBE(0x5A6EA8), (uint8_t)(230 * (1 - weg)));
  }
  gerda(g, 142, 222, 0.8f, 1, u, 0.2f, u > 4 ? 2.6f : 0.4f, -1, &GERDA_BARFUSS, false, u > 4 ? 2.6f : 0.2f, u > 4 && u < 10);
  float ek = seg(u, 6, 10);
  for (int i = 0; i < 5; i++) {
    float a = i * 1.256f + u * 0.4f;
    if (ek > 0) engel(g, 142 + cosf(a) * (30 + ek * 50), 160 + sinf(a) * 30 - ek * 20, 0.8f, u + i, (uint8_t)(255 * ek));
  }
  g.kameraAus();
  vers(g, {"Vor dem Schloss stürmten lebendige", "Schneeflocken auf Gerda zu."}, aufzu(u, 0.4f, 6));
  vers(g, {"Sie betete - und ihr Atem wurde", "zu kleinen Engeln, die die", "Flocken zerschlugen."}, aufzu(u, 6.4f, 15.6f));
  vers(g, {"Nun fror Gerda nicht mehr."}, aufzu(u, 16, 21.6f));
}

// 14) Im Eisschloss: Kay legt das Wort "Ewigkeit"
static void szeneEwigkeit(Gfx &g, float u) {
  eisschloss(g, u);
  // Eisstuecke, die er hin und her legt
  for (int i = 0; i < 8; i++) {
    float x = 90 + i * 14 + sinf(u * 0.7f + i) * 6, y = 214 + (i & 1) * 4;
    float e[] = {x - 5, y - 3, x + 5, y - 4, x + 4, y + 3, x - 4, y + 3};
    g.polygon(e, 4, EIS_H);
  }
  kay(g, 150, 214, 0.75f, -1, u, 0, 1 + sinf(u * 1.5f) * 0.4f, -1, true, 0.2f);
  g.text(SCHRIFT_K, "EWIG...", 100, 196, EIS_D, 1, 200);
  g.kameraAus();
  vers(g, {"Kay saß ganz allein im Eisschloss,", "blau vor Kälte - aber er merkte", "es nicht. Sein Herz war aus Eis."}, aufzu(u, 0.4f, 8.6f));
  vers(g, {"Er legte Eisstücke zu dem Wort", "EWIGKEIT - dann wäre er sein", "eigener Herr, hatte sie gesagt."}, aufzu(u, 9, 17.6f));
  vers(g, {"Aber das Wort gelang ihm nie."}, aufzu(u, 18, 23.6f));
}

// 15) Gerda findet Kay - die heissen Traenen
static void szeneFinden(Gfx &g, float u) {
  float tauen = seg(u, 14, 24);
  eisschloss(g, u, tauen);
  float k = seg(u, 0, 5);
  bool umarmt = u > 6;
  float gx = umarmt ? 136 : lerp(-20, 120, k);
  gerda(g, gx, 222, 0.78f, 1, u, (u > 6.4f && u < 12) ? 0.5f : u > 14 ? 0.5f + 0.2f * sinf(u * 6) : 0.3f, umarmt ? 1.6f : 0.4f,
        k < 1 ? fmodf(u * 1.6f, 1) : -1, &GERDA_BARFUSS, u > 8 && u < 16);
  bool warm = u > 18;
  kay(g, 168, 222, 0.75f, -1, u, (u > 18.4f && u < 25) ? 0.5f + 0.3f * sinf(u * 8) : 0.1f, warm ? 1.6f : 0.6f, -1, !warm, 0, 0.2f,
      u > 16 && u < 18.4f);
  for (int i = 0; i < 3; i++) {
    float tk = fmodf(u * 0.8f + i * 0.33f, 1);
    if (u > 8 && u < 16) tropfen(g, 150 + i * 4, 160 + tk * 30, 2.4f, FARBE(0x7EC0EE), (uint8_t)(255 * (1 - tk)));
  }
  if (u > 16 && u < 18.4f) for (int i = 0; i < 2; i++) tropfen(g, 172 + i * 8, 160 + fmodf(u * 2, 1) * 14, 2, FARBE(0x7EC0EE));
  if (warm) herzen(g, 152, 150, u, 5, 22);
  if (u > 25) {  // die Eisstuecke tanzen und legen selbst das Wort
    float wk = seg(u, 25, 28);
    g.text(SCHRIFT_M, "EWIGKEIT", 142, lerp(160, 136, wk), EIS_D, 1, (uint8_t)(255 * wk));
    glitzern(g, 142, 130, 60, u, 6);
  }
  g.kameraAus();
  blase(g, 100, 50, {"Kay! Lieber kleiner Kay!", "Endlich hab ich dich gefunden!"}, aufzu(u, 6.4f, 12), 136, 140);
  vers(g, {"Gerda weinte heiße Tränen. Die", "drangen in sein Herz und tauten", "das Eis und den Splitter fort."}, aufzu(u, 12.4f, 18));
  blase(g, 200, 50, {"Gerda! Liebe kleine Gerda!", "Wo bist du so lange gewesen?"}, aufzu(u, 18.4f, 25), 168, 140);
  vers(g, {"Da tanzten die Eisstücke vor", "Freude und legten das Wort selbst."}, aufzu(u, 25.4f, 31.6f));
}

// 16) Die Heimreise - es wird Fruehling
static void szeneHeimreise(Gfx &g, float u) {
  float fr = seg(u, 8, 16);
  if (fr < 0.5f) schneefeld(g, u, u * 60);
  else {
    himmel(g, 0, u, 200);
    sonne(g, 240, 32, 11, u);
    huegel(g, u * 30, 0.025f, 196, 26, 170, GRAS_H);
    boden(g, 200);
    blumen(g, u * 60, 228, 6, 4);
  }
  if (fr < 0.5f) {
    rentier(g, 160, 222, 0.85f, 1, u, fmodf(u * 1.6f, 1));
    gerda(g, 154, 188, 0.55f, 1, u, 0.5f, 1.2f, -1, &GERDA_REISE);
    kay(g, 172, 190, 0.55f, 1, u, 0.5f, 1.2f);
  } else {
    gerda(g, 120, 222, 0.8f, 1, u, 0.5f, 1.2f, fmodf(u * 1.4f, 1), &GERDA_REISE);
    kay(g, 160, 222, 0.8f, 1, u, 0.5f, 1.2f, fmodf(u * 1.4f + 0.2f, 1));
    for (int i = 0; i < 3; i++) vogel(g, 60 + i * 40 + sinf(u + i) * 10, 70 + i * 8, 1, u + i, 1);
  }
  g.kameraAus();
  vers(g, {"Das Rentier trug sie zur Finnin", "und zur Lappin, und überall", "wurde es grün und warm."}, aufzu(u, 0.4f, 9));
  vers(g, {"Auch das Räubermädchen trafen", "sie wieder - und sie freute sich."}, aufzu(u, 9.4f, 16));
  vers(g, {"Hand in Hand gingen Kay und", "Gerda heim. Es war Frühling."}, aufzu(u, 16.4f, 23.6f));
}

// 17) Daheim bei der Grossmutter - die Rosen bluehen
static void szeneDaheim(Gfx &g, float u) {
  dachstuben(g, u);
  kay(g, 112, 168, 0.62f, 1, u, 0.4f + 0.2f * sinf(u * 5), 1.2f);
  gerda(g, 172, 168, 0.6f, -1, u, 0.4f + 0.2f * sinf(u * 5 + 1), 1.2f);
  frauX(g, OMA, 63, 150, 0.45f, 1, u, 0.3f, 1);
  herzen(g, 142, 120, u, 5, 30);
  g.kameraAus();
  vers(g, {"Daheim saß die Großmutter im", "Sonnenschein. Die Rosen blühten."}, aufzu(u, 0.4f, 8));
  vers(g, {"Kay und Gerda waren groß geworden -", "und doch Kinder, Kinder im Herzen.", "Und es war Sommer."}, aufzu(u, 8.4f, 18));
  endeBild(g, u, 20, 142, 150);
}

static void bild(Gfx &g, float t) {
  if (t < P2) szeneSpiegel(g, t - P1);
  else if (t < P3) szeneRosen(g, t - P2);
  else if (t < P4) szeneErzaehlt(g, t - P3);
  else if (t < P5) szeneSplitter(g, t - P4);
  else if (t < P6) szeneBoese(g, t - P5);
  else if (t < P7) szeneSchlitten(g, t - P6);
  else if (t < P8) szeneFluss(g, t - P7);
  else if (t < P9) szeneGarten(g, t - P8);
  else if (t < P10) szeneSchloss(g, t - P9);
  else if (t < P11) szeneRaeuber(g, t - P10);
  else if (t < P12) szeneRentier(g, t - P11);
  else if (t < P13) szeneLappland(g, t - P12);
  else if (t < P14) szeneEngel(g, t - P13);
  else if (t < P15) szeneEwigkeit(g, t - P14);
  else if (t < P16) szeneFinden(g, t - P15);
  else if (t < P17) szeneHeimreise(g, t - P16);
  else szeneDaheim(g, t - P17);
}

// =====================================================================
// Musik
// =====================================================================
// Rosenlied (F-Dur, innig)
static const Ton3 MEL_ROSEN[] = {
  {72, 1, 41}, {69, .5, 0}, {70, .5, 0}, {72, 1, 0}, {77, 1, 0}, {76, 1, 48}, {74, .5, 0}, {72, .5, 0}, {70, 2, 0},
  {69, 1, 41}, {70, .5, 0}, {72, .5, 0}, {74, 1, 46}, {72, 1, 0}, {70, .5, 48}, {69, .5, 0}, {67, 1, 0}, {65, 2, 41}};
// Eismelodie (h-Moll, glaesern, hoch)
static const Ton3 MEL_EIS[] = {
  {83, .5, 47}, {78, .5, 0}, {74, .5, 0}, {78, .5, 0}, {83, 1, 0}, {81, 1, 45}, {79, .5, 43}, {78, .5, 0}, {76, .5, 0},
  {74, .5, 0}, {73, 2, 42}, {74, .5, 43}, {76, .5, 0}, {78, .5, 0}, {79, .5, 0}, {81, 1, 45}, {78, 1, 0}, {83, 2, 47}};
static void zauber(Synth &s, float x) { for (int k = 0; k < 6; k++) s.note(x + k * 0.06f, 84 + k * 2, 0.45f); }
static void klirr(Synth &s, float x) { for (int k = 0; k < 8; k++) s.ton(x + k * 0.04f, 2600 + zufall(k) * 2400, 0.08f, 0.002f, 0.2f, SINUS, GERAEUSCH, 1800); }
static void schellen(Synth &s, float a, float b, float t0, float t1) {
  for (float x = a; x < b; x += 0.25f) if (bei(x, t0, t1)) s.ton(x, 3200 + ((int)(x * 4) & 1) * 400, 0.04f, 0.002f, 0.12f, SINUS, GERAEUSCH, 3000);
}
static void ton(Synth &s, float t0, float t1) {
  float a = P1;
  spieleMelodie(s, M(MEL_SPUK), a + 0.5f, a + 14, 0.42f, t0, t1, 0.4f);
  if (bei(a + 14, t0, t1)) { klirr(s, a + 14); s.rauschen(a + 14, 0.8f, HOCHPASS, 3000, 0.25f); }
  spieleMelodie(s, M(MEL_EIS), a + 16, a + 25.6f, 0.36f, t0, t1, 0.45f);
  a = P2;
  spieleMelodie(s, M(MEL_ROSEN), a + 0.4f, a + 25.6f, 0.4f, t0, t1, 0.65f);
  a = P3;
  if (bei(a, t0, t1)) s.rauschen(a, 26, BANDPASS, 600, 0.04f, 0.8f, 1, 1200);
  spieleMelodie(s, M(MEL_WIEGE), a + 0.4f, a + 17.6f, 0.44f, t0, t1, 0.45f);
  spieleMelodie(s, M(MEL_EIS), a + 18, a + 25.6f, 0.36f, t0, t1, 0.45f);
  a = P4;
  spieleMelodie(s, M(MEL_ROSEN), a + 0.4f, a + 4, 0.4f, t0, t1, 0.55f);
  if (bei(a + 5.4f, t0, t1)) klirr(s, a + 5.4f);
  if (bei(a + 7.4f, t0, t1)) klirr(s, a + 7.4f);
  spieleMelodie(s, M(MEL_SPUK), a + 8, a + 25.6f, 0.42f, t0, t1, 0.35f);
  a = P5;
  spieleMelodie(s, M(MEL_SPUK), a + 0.4f, a + 21.6f, 0.4f, t0, t1, 0.35f);
  a = P6;
  schellen(s, a + 2, a + 20, t0, t1);
  spieleMelodie(s, M(MEL_JAGD), a + 8, a + 19, 0.22f, t0, t1, 0.4f);
  if (bei(a + 14, t0, t1)) s.rauschen(a + 14, 15, BANDPASS, 700, 0.07f, 0.8f, 1, 1600);
  if (bei(a + 22, t0, t1)) zauber(s, a + 22);
  spieleMelodie(s, M(MEL_EIS), a + 20, a + 29.6f, 0.38f, t0, t1, 0.45f);
  a = P7;
  if (bei(a + 9.5f, t0, t1)) s.ton(a + 9.5f, 400, 0.2f, 0.002f, 0.3f, SINUS, GERAEUSCH, 160);  // plitsch
  if (bei(a, t0, t1)) s.rauschen(a, 26, BANDPASS, 900, 0.03f, 0.6f, 1);
  spieleMelodie(s, M(MEL_WANDER), a + 12.4f, a + 25.6f, 0.36f, t0, t1, 0.45f);
  a = P8;
  spieleMelodie(s, M(MEL_WIEGE), a + 0.4f, a + 13, 0.42f, t0, t1, 0.45f);
  if (bei(a + 14, t0, t1)) zauber(s, a + 14);
  spieleMelodie(s, M(MEL_ROSEN), a + 14.4f, a + 24, 0.38f, t0, t1, 0.6f);
  spieleMelodie(s, M(MEL_WANDER), a + 25, a + 29.6f, 0.3f, t0, t1, 0.5f);
  a = P9;
  for (int i = 0; i < 10; i++) if (bei(a + 0.6f + i * 1.3f, t0, t1)) s.ton(a + 0.6f + i * 1.3f, 700, 0.12f, 0.005f, 0.15f, SAEGE, GERAEUSCH, 500);  // krah
  spieleMelodie(s, M(MEL_N), a + 14, a + 29.6f, 0.42f, t0, t1, 0.4f);
  a = P10;
  spieleMelodie(s, M(MEL_JAGD), a + 3, a + 11.6f, 0.2f, t0, t1, 0.55f);
  if (bei(a + 12, t0, t1)) s.rauschen(a + 12, 16, BANDPASS, 1600, 0.03f, 1, 1);
  spieleMelodie(s, M(MEL_KEGEL), a + 12.4f, a + 27.6f, 0.32f, t0, t1, 0.4f);
  a = P11;
  for (int i = 0; i < 6; i++) if (bei(a + 0.6f + i * 0.9f, t0, t1)) s.ton(a + 0.6f + i * 0.9f, 420, 0.1f, 0.02f, 0.2f, SINUS, GERAEUSCH, 380);  // gurr
  spieleMelodie(s, M(MEL_WANDER), a + 7.4f, a + 25.6f, 0.34f, t0, t1, 0.45f);
  a = P12;
  schellen(s, a, a + 6, t0, t1);
  spieleMelodie(s, M(MEL_EIS), a + 0.4f, a + 6, 0.36f, t0, t1, 0.45f);
  spieleMelodie(s, M(MEL_WIEGE), a + 6.4f, a + 27.6f, 0.44f, t0, t1, 0.45f);
  a = P13;
  if (bei(a, t0, t1)) s.rauschen(a, 12, BANDPASS, 700, 0.08f, 0.8f, 1, 1500);
  for (int i = 0; i < 5; i++) if (bei(a + 6.4f + i * 0.8f, t0, t1)) s.glocke(a + 6.4f + i * 0.8f, 79 + i * 2, 0.4f);
  spieleMelodie(s, M(MEL_ROSEN), a + 10, a + 21.6f, 0.4f, t0, t1, 0.5f);
  a = P14;
  spieleMelodie(s, M(MEL_EIS), a + 0.4f, a + 23.6f, 0.44f, t0, t1, 0.4f);
  for (int i = 0; i < 6; i++) if (bei(a + 2 + i * 3.6f, t0, t1)) s.note(a + 2 + i * 3.6f, 91, 0.25f);
  a = P15;
  spieleMelodie(s, M(MEL_EIS), a + 0.4f, a + 12, 0.42f, t0, t1, 0.35f);
  spieleMelodie(s, M(MEL_ROSEN), a + 12.4f, a + 25, 0.42f, t0, t1, 0.65f);
  if (bei(a + 25, t0, t1)) { zauber(s, a + 25); klirr(s, a + 25.6f); }
  spieleMelodie(s, M(MEL_F), a + 25.4f, a + 31.6f, 0.28f, t0, t1, 0.5f);
  a = P16;
  spieleMelodie(s, M(MEL_WANDER), a + 0.4f, a + 23.6f, 0.32f, t0, t1, 0.55f);
  a = P17;
  spieleMelodie(s, M(MEL_ROSEN), a + 0.4f, a + 19.6f, 0.38f, t0, t1, 0.7f);
  endeKlang(s, a + 20.4f, t0, t1);
}

// =====================================================================
// Startbild, Vorschau, Erkennungszeichen
// =====================================================================
static void titelSzene(Gfx &g, float t) {
  eisschloss(g, t);
  koenigin(g, 200, 226, 0.9f, -1, t, 0, 1.2f);
  gerda(g, 100, 226, 0.8f, 1, t, 0.3f, 0.6f, -1, &GERDA_REISE);
  schneeFall(g, t, 20);
}
static void titelbild(Gfx &g, float t) { titelSzene(g, t); titelRahmen(g, t, "Die Schneekönigin", 204, SCHRIFT_M); }
static void vorschau(Gfx &g, float t) { titelSzene(g, t); }
static void symbol(Gfx &g, float x, float y, float t) {
  for (int i = 0; i < 5; i++) g.stern(x + (i - 2) * 14, y - 40 - (i == 2 ? 12 : i & 1 ? 4 : 0), 10, 3, 6, t + i, i == 2 ? WEISS : EIS);
  g.rundRechteck(x - 38, y - 34, 76, 12, 5, EIS_H);
  glitzern(g, x, y - 40, 44, t, 6);
}

const Maerchen SCHNEEKOENIGIN = {"Die Schneekönigin", PDAUER, FARBE(0xDCF0FF), bild, ton, titelbild, vorschau, symbol,
                                 LIED(MEL_ROSEN, 0.4f), "H. C. Andersen"};

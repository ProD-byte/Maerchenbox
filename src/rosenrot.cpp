// Schneeweisschen und Rosenrot - ein Spieluhr-Film fuer die Maerchenbox, nach dem Maerchen der Brueder Grimm.
// Die zwei Rosenbaeumchen, der Baer im Winter, dreimal der undankbare Zwerg (Baumstamm, Angel, Adler),
// die Edelsteine, der Prankenschlag und der Koenigssohn unter dem Baerenfell.
#include "maerchen.h"
#include "schriften.h"
#include "figuren.h"
#include "melodien.h"
#include <stdio.h>
#include <initializer_list>

// ---------- Ablauf (Sekunden) ----------
static const float R1 = 0, R2 = 34, R3 = 74, R4 = 96, R5 = 132, R6 = 168, R7 = 200, R8 = 248, RDAUER = 300;

// ---------- Aussehen ----------
static FrauLook schneeLook() {
  FrauLook l; l.kleid = FARBE(0xE8F0FF); l.haar = FARBE(0xF2D27A); l.frisur = 1; l.schuerze = FARBE(0xFFFFFF);
  l.blume = WEISS;
  return l;
}
static FrauLook roseLook() {
  FrauLook l; l.kleid = FARBE(0xE0473A); l.haar = FARBE(0x5A2E1A); l.frisur = 0; l.schuerze = FARBE(0xFFE6D6);
  l.blume = FARBE(0xFF3050);
  return l;
}
static FrauLook mutterLook() {
  FrauLook l; l.kleid = FARBE(0x8E7CA8); l.haar = FARBE(0xB0B0B0); l.frisur = 3; l.tuch = FARBE(0xD9C9E8);
  l.schuerze = FARBE(0xF4EEDC); l.wangen = false;
  return l;
}
static FrauLook brautWeiss() { FrauLook l = schneeLook(); l.kleid = WEISS; l.glanz = true; l.rockBreite = 1.4f; l.krone = true; l.frisur = 2; return l; }
static FrauLook brautRot() { FrauLook l = roseLook(); l.glanz = true; l.rockBreite = 1.4f; l.krone = true; l.frisur = 2; return l; }
static FrauLook engelLook() {
  FrauLook l; l.kleid = WEISS; l.haar = FARBE(0xFFE28A); l.frisur = 0; l.glanz = true; l.schuh = WEISS;
  return l;
}
static const FrauLook SCHNEE = schneeLook(), ROSE = roseLook(), MUTTER = mutterLook(), BRAUT_W = brautWeiss(),
                      BRAUT_R = brautRot(), ENGEL = engelLook();

static Aussehen zwergLook() {
  Aussehen a;
  a.weste = FARBE(0x7A5A3A); a.hemd = FARBE(0x9A7A5A); a.hose = FARBE(0x4A3B2A); a.haar = FARBE(0xC9C9C9);
  a.hut = FARBE(0xC0392B); a.hutD = FARBE(0x8E2A20); a.feder = FARBE(0xC0392B); a.haut = FARBE(0xF2C9A8);
  return a;
}
static Aussehen prinzLook() {
  Aussehen a;
  a.weste = FARBE(0xE0B43C); a.hemd = FARBE(0xFFF6E0); a.hose = FARBE(0x8E6E2A); a.haar = FARBE(0x5A3A26);
  a.muetze = 3; a.halstuch = FARBE(0xE8473D);
  return a;
}
static Aussehen bruderLook() {
  Aussehen a;
  a.weste = FARBE(0x3E7FDC); a.hemd = FARBE(0xFFFFFF); a.hose = FARBE(0x2E3A6E); a.haar = FARBE(0x3A2A20);
  a.muetze = 3; a.halstuch = GOLD;
  return a;
}
static const Aussehen ZWERG = zwergLook(), PRINZ = prinzLook(), BRUDER = bruderLook();

static FrauPose fp(const FrauLook &l, float t) {
  FrauPose p;
  p.look = &l;
  p.zeit = t;
  p.zu = fmodf(t + l.kleid * 0.0007f, 3.7f) < 0.13f;
  return p;
}

// =====================================================================
// Hilfen
// =====================================================================
static float aufzu(float u, float a, float b) { return federnd(seg(u, a, a + 0.4f)) * (1 - seg(u, b - 0.3f, b)); }

static void blase(Gfx &g, float x, float y, std::initializer_list<const char *> zeilen, float k, float sx, float sy,
                  Farbe f = WEISS) {
  if (k <= 0.05f) return;
  int w = 0, n = 0;
  for (const char *z : zeilen) { int b = g.textBreite(SCHRIFT_K, z); if (b > w) w = b; n++; }
  float bw = (w + 18) * k, bh = (8 + n * 18) * k;
  if (x - bw / 2 < 3) x = 3 + bw / 2;
  if (x + bw / 2 > 281) x = 281 - bw / 2;
  float sp[] = {x - 6, y + bh / 2 - 2, x + 6, y + bh / 2 - 2, sx, sy};
  g.kontur(1.4f, OUT);
  g.rundRechteck(x - bw / 2, y - bh / 2, bw, bh, 9 * k, f);
  g.konturAus();
  g.polygon(sp, 3, f);
  if (k > 0.85f) {
    int i = 0;
    for (const char *z : zeilen) g.text(SCHRIFT_K, z, x, y - bh / 2 + 18 + i++ * 18, OUT, 1);
  }
}

static void glitzern(Gfx &g, float x, float y, float r, float t, int n, uint8_t a = 255) {
  for (int i = 0; i < n; i++) {
    float an = t * 2 + i * 6.2831853f / n;
    float rr = r * (0.6f + 0.4f * sinf(t * 3 + i));
    g.stern(x + cosf(an) * rr, y + sinf(an) * rr * 0.8f, 4, 1.6f, 4, an, i & 1 ? GOLD : WEISS, a);
  }
}

static void puff(Gfx &g, float x, float y, float k, Farbe c = GOLD) {
  if (k <= 0 || k >= 1) return;
  for (int i = 0; i < 10; i++) {
    float a = i * 0.628f;
    g.stern(x + cosf(a) * k * 50, y + sinf(a) * k * 44, 6 * (1 - k) + 2, 2, 5, a + k * 4, i & 1 ? c : WEISS,
            (uint8_t)(255 * (1 - k)));
  }
  g.kreis(x, y, 34 * (1 - k), WEISS, (uint8_t)(180 * (1 - k)));
}

// =====================================================================
// Figuren
// =====================================================================
struct BaerPose {
  float lauf = -1;
  bool aufrecht = false;  // sitzt / steht auf den Hinterbeinen
  float pranke = 0;       // 0..1 Prankenschlag
  bool zu = false;
  float mund = 0;
  float schnee = 0;       // Schnee im Fell
  float gold = 0;         // Gold schimmert durch das zerrissene Fell
};

static void baer(Gfx &g, float x, float y, float s, float dir, const BaerPose &p, float t) {
  const Farbe fell = FARBE(0x6E4426), hell = FARBE(0x9A6A3E), dunkel = FARBE(0x4E2E18);
  auto PX = [&](float dx) { return x + dx * s * dir; };
  auto PY = [&](float dy) { return y + dy * s; };
  float kx, ky;
  g.kontur(1.6f * s, OUT);
  if (!p.aufrecht) {
    for (int i = 0; i < 4; i++) {
      float bx = i < 2 ? -30 + i * 14 : 16 + (i - 2) * 14;
      float sw = p.lauf >= 0 ? sinf(p.lauf * 6.2831853f + i * 1.6f) * 8 : 0;
      g.linie(PX(bx), PY(-30), PX(bx + sw), PY(-6), 15 * s, i & 1 ? fell : dunkel);
      g.ellipse(PX(bx + sw + 2), PY(-4), 9 * s, 5 * s, dunkel);
    }
    g.ellipse(PX(0), PY(-40), 44 * s, 26 * s, fell);
    kx = PX(46); ky = PY(-50 + (p.lauf >= 0 ? sinf(p.lauf * 12.566f) * 2 : 0));
  } else {
    g.ellipse(PX(-10), PY(-8), 14 * s, 9 * s, dunkel);
    g.ellipse(PX(14), PY(-8), 14 * s, 9 * s, dunkel);
    g.ellipse(PX(0), PY(-42), 30 * s, 36 * s, fell);
    g.ellipse(PX(4), PY(-38), 16 * s, 22 * s, hell);
    // Arme (vorderer mit Prankenschlag)
    float w = 0.6f + p.pranke * 2.2f;
    g.linie(PX(-18), PY(-62), PX(-26), PY(-34), 13 * s, dunkel);
    float ax = PX(20) + sinf(w) * 28 * s * dir, ay = PY(-62) + cosf(w) * 28 * s;
    g.linie(PX(20), PY(-62), ax, ay, 14 * s, fell);
    g.kreis(ax, ay, 9 * s, dunkel);
    kx = PX(4); ky = PY(-92);
  }
  // Kopf
  g.kreis(kx - 12 * s * dir, ky - 14 * s, 7 * s, fell);
  g.kreis(kx + 10 * s * dir, ky - 15 * s, 7 * s, fell);
  g.kreis(kx, ky, 19 * s, fell);
  g.konturAus();
  g.kreis(kx - 12 * s * dir, ky - 14 * s, 3.5f * s, hell);
  g.kreis(kx + 10 * s * dir, ky - 15 * s, 3.5f * s, hell);
  g.ellipse(kx + 8 * s * dir, ky + 6 * s, 10 * s, 7.5f * s, hell);
  g.ellipse(kx + 13 * s * dir, ky + 2 * s, 4 * s, 3 * s, SCHWARZ);
  for (int i = 0; i < 2; i++) {
    float ex = kx + (i ? 9 : -4) * s * dir, ey = ky - 4 * s;
    if (p.zu) g.bogen(ex, ey, 2.8f * s, 3.5f, 5.9f, 1.6f * s, OUT);
    else { g.kreis(ex, ey, 2.8f * s, SCHWARZ); g.kreis(ex + 0.8f * s * dir, ey - 1 * s, 1 * s, WEISS); }
  }
  if (p.mund > 0.05f) g.ellipse(kx + 9 * s * dir, ky + 11 * s, 4 * s, (1 + p.mund * 3) * s, FARBE(0x7A2A2A));
  else g.bogen(kx + 9 * s * dir, ky + 7 * s, 4 * s, 0.5f, 2.6f, 1.4f * s, OUT);
  // Schnee im Fell
  if (p.schnee > 0)
    for (int i = 0; i < 12; i++) {
      float sx = x + (zufall(i + 400) - 0.5f) * 70 * s, sy = y - (20 + zufall(i + 420) * 50) * s - (p.aufrecht ? 10 * s : 0);
      g.kreis(sx, sy, 2.6f * s, WEISS, (uint8_t)(255 * p.schnee));
    }
  // Gold schimmert durch das Loch im Fell
  if (p.gold > 0) {
    float gx = PX(-20), gy = PY(p.aufrecht ? -50 : -54);
    g.ellipse(gx, gy, 6 * s, 4 * s, GOLD, (uint8_t)(255 * p.gold));
    g.stern(gx, gy, 6 * s, 2 * s, 4, t * 3, WEISS, (uint8_t)(255 * p.gold));
  }
}

// Zwerg mit langem Bart; bartX/bartY: wo das Bartende festhaengt (0 = haengt frei)
static void zwerg(Gfx &g, float x, float y, float s, float dir, const Pose &pIn, float t, float bartX = 0,
                  float bartY = 0, float bartLaenge = 1) {
  Pose p = pIn;
  p.aussehen = &ZWERG;
  schneiderlein(g, x, y, s, dir, p);
  float cx = x + 5 * s * dir, cy = y - 50 * s;
  float ex = bartX ? bartX : cx + sinf(t * 2) * 2 * s, ey = bartX ? bartY : cy + 38 * s * bartLaenge;
  float mx = (cx + ex) / 2 + (bartX ? 0 : 4 * s * dir), my = (cy + ey) / 2 + (bartX ? 10 * s : 0);
  // Bart als sich verjuengende Flaeche
  float pts[2 * 20];
  int n = 0;
  for (int i = 0; i <= 8; i++) {
    float u = i / 8.0f, v = 1 - u;
    float bx = v * v * cx + 2 * v * u * mx + u * u * ex, by = v * v * cy + 2 * v * u * my + u * u * ey;
    float w = lerp(11, 2.5f, u) * s;
    pts[2 * n] = bx - w; pts[2 * n + 1] = by; n++;
  }
  for (int i = 8; i >= 0; i--) {
    float u = i / 8.0f, v = 1 - u;
    float bx = v * v * cx + 2 * v * u * mx + u * u * ex, by = v * v * cy + 2 * v * u * my + u * u * ey;
    float w = lerp(11, 2.5f, u) * s;
    pts[2 * n] = bx + w; pts[2 * n + 1] = by; n++;
  }
  g.polygon(pts, n, FARBE(0xEDEDED));
  g.ellipse(cx, cy - 2 * s, 9 * s, 4 * s, FARBE(0xEDEDED));
}

static void hase(Gfx &g, float x, float y, float s, float dir, float t, float huepf = 0) {
  const Farbe f = FARBE(0xB89A78);
  float h = huepf * fabsf(sinf(t * 7)) * 8 * s;
  g.kontur(1.1f * s, OUT);
  g.ellipse(x - 2 * s * dir, y - 10 * s - h, 12 * s, 9 * s, f);
  g.ellipse(x + 6 * s * dir, y - 30 * s - h, 3 * s, 10 * s, f, 255, 0.2f * dir);
  g.ellipse(x + 11 * s * dir, y - 29 * s - h, 3 * s, 10 * s, f, 255, 0.4f * dir);
  g.kreis(x + 9 * s * dir, y - 18 * s - h, 7 * s, f);
  g.konturAus();
  g.kreis(x - 13 * s * dir, y - 10 * s - h, 4 * s, WEISS);
  g.kreis(x + 11 * s * dir, y - 19 * s - h, 1.6f * s, SCHWARZ);
  g.kreis(x + 15 * s * dir, y - 16 * s - h, 1.2f * s, FARBE(0xFF8FB0));
}

static void reh(Gfx &g, float x, float y, float s, float dir, float t, float lauf = -1) {
  const Farbe f = FARBE(0xB0703A), d = FARBE(0x8E5426);
  auto PX = [&](float dx) { return x + dx * s * dir; };
  auto PY = [&](float dy) { return y + dy * s; };
  g.kontur(1.2f * s, OUT);
  for (int i = 0; i < 4; i++) {
    float bx = i < 2 ? -14 + i * 6 : 10 + (i - 2) * 6;
    float sw = lauf >= 0 ? sinf(lauf * 6.2831853f + i * 1.6f) * 6 : 0;
    g.linie(PX(bx), PY(-22), PX(bx + sw), PY(-2), 3.5f * s, d);
  }
  g.ellipse(PX(0), PY(-28), 20 * s, 10 * s, f);
  float graest = lauf < 0 ? 0.5f + 0.5f * sinf(t * 0.8f) : 0;  // senkt den Kopf zum Grasen
  float kx = PX(lerp(22, 26, graest)), ky = PY(lerp(-50, -24, graest));
  g.linie(PX(14), PY(-32), kx, ky + 4 * s, 7 * s, f);
  g.ellipse(kx + 3 * s * dir, ky, 8 * s, 6 * s, f);
  g.ellipse(kx - 3 * s * dir, ky - 8 * s, 3 * s, 7 * s, f, 255, -0.4f * dir);
  g.konturAus();
  for (int i = 0; i < 5; i++) g.kreis(PX(-10 + i * 5), PY(-30 + (i & 1) * 4), 1.6f * s, WEISS);
  g.kreis(kx + 3 * s * dir, ky - 1 * s, 1.5f * s, SCHWARZ);
  g.kreis(kx + 10 * s * dir, ky + 1 * s, 1.4f * s, SCHWARZ);
}

static void adler(Gfx &g, float x, float y, float s, float t, float dir) {
  const Farbe f = FARBE(0x6E4426), h = FARBE(0xF4EEDC);
  float w = sinf(t * 6) * 16 * s;
  float l[] = {x, y - 4 * s, x - 50 * s, y - 10 * s - w, x - 38 * s, y + 2 * s - w * 0.4f, x - 20 * s, y + 6 * s};
  float r[] = {x, y - 4 * s, x + 50 * s, y - 10 * s - w, x + 38 * s, y + 2 * s - w * 0.4f, x + 20 * s, y + 6 * s};
  g.kontur(1.3f, OUT);
  g.polygon(l, 4, f);
  g.polygon(r, 4, f);
  g.ellipse(x, y + 4 * s, 12 * s, 14 * s, f);
  g.kreis(x + 6 * s * dir, y - 10 * s, 8 * s, h);
  g.konturAus();
  float sn[] = {x + 12 * s * dir, y - 12 * s, x + 20 * s * dir, y - 7 * s, x + 12 * s * dir, y - 5 * s};
  g.polygon(sn, 3, FARBE(0xF2A93B));
  g.kreis(x + 8 * s * dir, y - 12 * s, 1.6f * s, SCHWARZ);
  g.linie(x - 4 * s, y + 16 * s, x - 6 * s, y + 24 * s, 2 * s, FARBE(0xF2A93B));
  g.linie(x + 4 * s, y + 16 * s, x + 6 * s, y + 24 * s, 2 * s, FARBE(0xF2A93B));
}

static void engel(Gfx &g, float x, float y, float s, float t, uint8_t a) {
  g.kreis(x, y - 50 * s, 60 * s, FARBE(0xFFF6C8), (uint8_t)(a * 0.25f));
  g.ellipse(x - 16 * s, y - 52 * s, 14 * s, 22 * s, WEISS, a, -0.5f + sinf(t * 2) * 0.1f);
  g.ellipse(x + 16 * s, y - 52 * s, 14 * s, 22 * s, WEISS, a, 0.5f - sinf(t * 2) * 0.1f);
  FrauPose p = fp(ENGEL, t);
  p.armV = 1.2f; p.armH = 1.2f;
  frau(g, x, y, s, 1, p);
  g.bogen(x + 1 * s, y - 96 * s, 9 * s, 3.4f, 6.0f, 2 * s, GOLD, a);
}

static void schere(Gfx &g, float x, float y, float s, float auf) {
  g.kreis(x - 4 * s, y + 4 * s, 3 * s, FARBE(0xE0473A));
  g.kreis(x + 4 * s, y + 4 * s, 3 * s, FARBE(0xE0473A));
  g.linie(x - 2 * s, y + 2 * s, x + (4 + auf * 4) * s, y - 12 * s, 2 * s, FARBE(0xC7D0DC));
  g.linie(x + 2 * s, y + 2 * s, x - (4 + auf * 4) * s + 8 * s, y - 12 * s, 2 * s, FARBE(0xC7D0DC));
}

static void sack(Gfx &g, float x, float y, float s, Farbe inhalt) {
  g.kontur(1.3f * s, OUT);
  g.ellipse(x, y - 10 * s, 12 * s, 11 * s, FARBE(0xC9A46A));
  g.konturAus();
  g.ellipse(x, y - 20 * s, 7 * s, 3 * s, FARBE(0xA8823E));
  for (int i = 0; i < 3; i++) g.kreis(x - 4 * s + i * 4 * s, y - 22 * s, 2.4f * s, inhalt);
}

static void rosenbusch(Gfx &g, float x, float y, float s, Farbe rose, float t, float bluete = 1) {
  g.kontur(1.4f, OUT);
  g.kreis(x - 12 * s, y - 16 * s, 14 * s, FARBE(0x3E9A4B));
  g.kreis(x + 12 * s, y - 18 * s, 14 * s, FARBE(0x3E9A4B));
  g.kreis(x, y - 30 * s, 15 * s, FARBE(0x3E9A4B));
  g.konturAus();
  int n = (int)(7 * bluete);
  for (int i = 0; i < n; i++) {
    float rx = x + (zufall(i + (int)x) - 0.5f) * 36 * s, ry = y - 10 * s - zufall(i + 40 + (int)x) * 34 * s;
    g.kreis(rx, ry, 4.5f * s, rose);
    g.kreis(rx - 1 * s, ry - 1 * s, 2 * s, mischeFarbe(rose, WEISS, 0.5f));
  }
}

static void besen(Gfx &g, float x, float y, float w) {
  float ex = x + cosf(w) * 30, ey = y + sinf(w) * 30;
  g.linie(x, y, ex, ey, 2.5f, HOLZ);
  float q[] = {ex - 5, ey, ex + 5, ey, ex + 8, ey + 12, ex - 8, ey + 12};
  g.polygon(q, 4, FARBE(0xD9B46A));
}

// =====================================================================
// Kulissen
// =====================================================================
static void huetteAussen(Gfx &g, float t, float nacht, float schnee, float bluete) {
  himmel(g, nacht, t, 196);
  if (nacht < 0.5f) sonne(g, 244, 34, 12, t);
  huegel(g, 0, 0, 186, 30, 140, mischeFarbe(FARBE(0xA7DE85), WEISS, schnee));
  tanne(g, 20, 196, 1, mischeFarbe(FARBE(0x2F8F4E), FARBE(0xDDE8E0), schnee * 0.5f));
  tanne(g, 262, 196, 1.1f, mischeFarbe(FARBE(0x2F8F4E), FARBE(0xDDE8E0), schnee * 0.5f));
  boden(g, 196, mischeFarbe(GRAS, WEISS, schnee));
  g.kontur(1.8f, OUT);
  g.rechteck(84, 104, 116, 92, FARBE(0xE8D3A2));
  float d[] = {72, 106, 142, 56, 212, 106};
  g.polygon(d, 3, mischeFarbe(FARBE(0x9A7A4A), WEISS, schnee * 0.8f));
  g.konturAus();
  g.rundRechteck(128, 146, 28, 50, 12, HOLZ);
  g.kreis(152, 168, 1.8f, GOLD);
  g.rundRechteck(96, 120, 22, 20, 2, mischeFarbe(FARBE(0x8FD3FF), FARBE(0xFFD24A), nacht));
  g.rundRechteck(168, 120, 22, 20, 2, mischeFarbe(FARBE(0x8FD3FF), FARBE(0xFFD24A), nacht));
  rosenbusch(g, 104, 198, 0.75f, WEISS, t, bluete);
  rosenbusch(g, 182, 198, 0.75f, FARBE(0xE8283F), t, bluete);
}

static void stube(Gfx &g, float t) {
  g.verlauf(0, 190, FARBE(0x8E6A4E), FARBE(0x6E4E36));
  g.rechteck(0, 190, 284, 50, FARBE(0x7E5A3E));
  for (int i = 0; i < 4; i++) g.rechteck(0, 200 + i * 12, 284, 1.5f, FARBE(0x5E3E26));
  // Herd mit Feuer
  g.kontur(2, OUT);
  g.rundRechteck(14, 104, 86, 92, 6, FARBE(0xA89888));
  g.konturAus();
  g.rundRechteck(24, 132, 66, 62, 24, FARBE(0x2E2620));
  for (int i = 0; i < 4; i++) {
    float fx = 36 + i * 14, h = 18 + 6 * sinf(t * 9 + i * 1.7f);
    float f[] = {fx - 7, 192, fx + 7, 192, fx + sinf(t * 7 + i) * 2, 192 - h};
    g.polygon(f, 3, FARBE(0xFF8A2A));
  }
  g.kreis(57, 170, 70, FARBE(0xFFB050), 26);
  // Fenster mit Schnee
  g.kontur(1.8f, OUT);
  g.rundRechteck(116, 50, 54, 50, 4, HOLZ);
  g.konturAus();
  g.rundRechteck(120, 54, 46, 42, 2, FARBE(0x34306E));
  for (int i = 0; i < 6; i++) {
    float sx = 122 + fmodf(zufall(i + 200) * 46 + sinf(t + i) * 4, 44), sy = 54 + fmodf(zufall(i + 210) * 40 + t * 12, 40);
    g.kreis(sx, sy, 1.4f, WEISS);
  }
  // Tuer rechts mit Haken
  g.kontur(1.8f, OUT);
  g.rundRechteck(222, 96, 50, 100, 6, HOLZ_D);
  g.konturAus();
  g.linie(228, 120, 228, 126, 2, FARBE(0x5E6270));
}

static void wald(Gfx &g, float t, float abend) {
  himmel(g, abend * 0.4f, t, 200);
  sonne(g, 240, lerp(36, 90, abend), 13, t);
  huegel(g, 0, 0, 186, 34, 150, mischeFarbe(FARBE(0xA7DE85), FARBE(0xC89A6A), abend * 0.5f));
  for (int i = 0; i < 4; i++) tanne(g, 20 + i * 80, 196, 0.9f + (i & 1) * 0.2f);
  boden(g, 200);
  blumen(g, 0, 200, 12, 17);
}

// =====================================================================
// Szenen
// =====================================================================
// 1) Die Huette mit den zwei Rosenbaeumchen; die Schwestern; die Tiere; der Engel am Abgrund
static void szeneAnfang(Gfx &g, float u) {
  if (u < 12) {
    huetteAussen(g, u, 0, 0, 1);
    FrauPose m = fp(MUTTER, u);
    frau(g, 142, 196, 0.85f, 1, m);
    FrauPose a = fp(SCHNEE, u), b = fp(ROSE, u + 1);
    a.armV = 1.6f; b.armV = 1.6f;  // halten sich an den Haenden
    frau(g, 116, 220, 0.9f, 1, a);
    frau(g, 166, 220, 0.9f, -1, b);
    blase(g, 90, 46, {"Wir wollen uns nicht verlassen."}, aufzu(u, 3, 6.6f), 112, 122);
    blase(g, 190, 46, {"Solange wir leben, nicht."}, aufzu(u, 6.8f, 10.6f), 170, 122);
  } else if (u < 24) {
    wald(g, u, 0);
    FrauPose a = fp(SCHNEE, u), b = fp(ROSE, u + 1);
    a.armV = 1.3f;
    b.lauf = fmodf(u * 1.8f, 1); b.armV = 2.5f + sinf(u * 6) * 0.3f; b.mund = 0.5f;
    float bx = 180 + sinf(u * 0.9f) * 50;
    frau(g, 90, 214, 0.9f, 1, a);
    frau(g, bx, 214, 0.9f, cosf(u * 0.9f) > 0 ? 1 : -1, b);
    hase(g, 118, 214, 0.9f, -1, u);      // frisst aus der Hand
    reh(g, 236, 214, 1, -1, u);           // grast daneben
    for (int i = 0; i < 2; i++) vogel(g, 40 + i * 40, 120 + sinf(u * 2 + i) * 4, 0.8f, -1, 1);
    for (int i = 0; i < 3; i++) {
      float ph = fmodf(u * 0.5f + i / 3.0f, 1);
      notenzeichen(g, 50 + i * 30 + ph * 10, 110 - ph * 40, i & 1 ? FARBE(0x3E7FDC) : FARBE(0xE8473D), (uint8_t)(220 * huepf(ph)));
    }
    // Schmetterling, den Rosenrot fangen will
    float sx = bx + 26 + sinf(u * 3) * 10, sy = 150 + sinf(u * 4.3f) * 10;
    float fl = fabsf(sinf(u * 14));
    g.ellipse(sx - 4, sy, 5 * fl + 1, 6, FARBE(0xFFD54A), 230);
    g.ellipse(sx + 4, sy, 5 * fl + 1, 6, FARBE(0xFFD54A), 230);
  } else {
    // Nacht im Wald: sie schlafen auf dem Moos, ein Kind in weissem Kleid wacht ueber sie
    float morgen = seg(u, 31, 33);
    himmel(g, 1 - morgen * 0.7f, u, 200);
    if (morgen < 0.5f) mond(g, 240, 36, 12);
    for (int i = 0; i < 3; i++) tanne(g, 30 + i * 60, 196, 1, FARBE(0x1F4B45));
    boden(g, 200, mischeFarbe(FARBE(0x2F5E4A), GRAS, morgen));
    // der Abgrund rechts
    g.rechteck(214, 200, 70, 40, FARBE(0x1A1620));
    g.rechteck(214, 196, 4, 44, FARBE(0x3E3028));
    FrauPose a = fp(SCHNEE, u), b = fp(ROSE, u + 1);
    a.zu = b.zu = morgen < 0.5f;
    frau(g, 120, 220, 0.85f, 1, a);
    frau(g, 156, 220, 0.85f, -1, b);
    if (morgen < 0.5f) zzz(g, 140, 130, u);
    uint8_t ea = (uint8_t)(255 * seg(u, 25, 27) * (1 - seg(u, 31, 32.5f)));
    if (ea > 5) engel(g, 194, 210, 0.75f, u, ea);
  }
  blende(g, u, 34, 140, 150);
}

// 2) Winterabend: der Baer klopft an, liegt am Feuer, die Kinder kehren den Schnee ab und spielen mit ihm
static void szeneWinter(Gfx &g, float u) {
  stube(g, u);
  // Mutter mit dem Buch
  FrauPose m = fp(MUTTER, u);
  m.armV = 1.4f;
  m.mund = u > 14 && u < 17 && fmodf(u, 0.4f) < 0.2f ? 0.5f : 0;
  float mx, my;
  frau(g, 120, 214, 0.9f, 1, m, &mx, &my);
  if (u < 14) { g.rundRechteck(mx - 2, my - 8, 14, 10, 2, FARBE(0x8E2F3A)); g.rechteck(mx + 4, my - 8, 1, 10, WEISS); }
  // Baer
  BaerPose bp;
  bp.schnee = 1 - seg(u, 20, 25);
  bp.zu = u > 37;
  bp.mund = (u > 10 && u < 14) || (u > 31 && u < 37) ? 0.5f : 0;
  float tuer = seg(u, 5, 6.5f);
  if (u > 6.5f && u < 17) {
    // nur der Kopf schaut zur Tuer herein
    g.ausschnitt(222, 96, 62, 100);
    baer(g, 300 - seg(u, 6.5f, 7.5f) * 30, 214, 0.95f, -1, bp, u);
    g.ausschnittAus();
  } else if (u >= 17) {
    float bx = lerp(250, 150, sanft(seg(u, 17, 20)));
    bp.lauf = u < 20 ? fmodf(u * 1.2f, 1) : -1;
    if (u > 25 && u < 33) bp.lauf = -1;
    baer(g, bx + (u > 25 && u < 31 ? sinf(u * 4) * 6 : 0), 214, 0.95f, -1, bp, u);
  }
  // Tuer
  if (tuer > 0 && u < 17.5f) {
    g.kontur(1.4f, OUT);
    g.rundRechteck(222, 96, 50 * (1 - tuer * 0.85f), 100, 6, HOLZ);
    g.konturAus();
  }
  // die Kinder
  FrauPose a = fp(SCHNEE, u), b = fp(ROSE, u + 1);
  float ax = 170, bx2 = 206;
  if (u > 4 && u < 7) { bx2 = 214; b.armV = 2.2f; }  // Rosenrot macht auf
  if (u > 7.5f && u < 14) {  // verstecken sich
    ax = lerp(170, 92, sanft(seg(u, 7.5f, 8.5f))); bx2 = lerp(206, 80, sanft(seg(u, 7.5f, 8.7f)));
    a.mund = b.mund = 0.8f; a.armV = b.armV = 2.7f;
  }
  if (u > 14 && u < 20) { ax = 92; bx2 = 80; }
  if (u > 20 && u < 25) { ax = 116; bx2 = 196; a.armV = 1.6f + sinf(u * 10) * 0.4f; b.armV = 1.6f + cosf(u * 10) * 0.4f; }  // kehren
  if (u > 25 && u < 37) {  // spielen: auf dem Ruecken, mit der Haselrute
    ax = 140 + sinf(u * 2) * 8; bx2 = 186;
    a.mund = b.mund = 0.6f; b.armV = 2.4f + sinf(u * 7) * 0.5f;
  }
  if (u > 37) { ax = 110; bx2 = 200; a.zu = b.zu = true; }
  float ahx, ahy, bhx, bhy;
  if (u > 25 && u < 37) {
    frau(g, ax, 186 - fabsf(sinf(u * 3)) * 4, 0.75f, 1, a, &ahx, &ahy);  // reitet auf dem Baeren
    frau(g, bx2, 214, 0.85f, -1, b, &bhx, &bhy);
    g.linie(bhx, bhy, bhx - 18, bhy - 14 + sinf(u * 7) * 10, 2, FARBE(0x8E5B3A));  // Haselrute
  } else {
    frau(g, ax, 214, 0.85f, 1, a, &ahx, &ahy);
    frau(g, bx2, 214, 0.85f, -1, b, &bhx, &bhy);
  }
  if (u > 20 && u < 25) {
    besen(g, ahx, ahy, 0.4f + sinf(u * 10) * 0.4f);
    besen(g, bhx, bhy, 2.7f + cosf(u * 10) * 0.4f);
    for (int i = 0; i < 8; i++) {  // Schnee stiebt
      float k = fmodf(u * 2 + i * 0.125f, 1);
      g.kreis(150 + (i - 4) * 10 + k * 20 * (i & 1 ? 1 : -1), 170 - k * 30, 2, WEISS, (uint8_t)(255 * (1 - k)));
    }
  }
  if (u > 37) zzz(g, 150, 120, u);
  blase(g, 230, 50, {"Fürchtet euch nicht,", "ich tu euch nichts,", "ich bin halb erfroren."}, aufzu(u, 9.6f, 14), 250, 110);
  blase(g, 110, 54, {"Du armer Bär,", "leg dich ans Feuer!"}, aufzu(u, 14, 17.4f), 120, 130);
  blase(g, 150, 46, {"Lasst mich am Leben, ihr Kinder:", "Schneeweißchen, Rosenrot,", "schlägst dir den Freier tot."}, aufzu(u, 31, 37), 160, 112);
  blende(g, u, 40, 150, 150);
}

// 3) Fruehling: der Baer muss fort, sein Fell bleibt am Tuerhaken haengen, Gold schimmert durch
static void szeneAbschied(Gfx &g, float u) {
  huetteAussen(g, u, 0, 0, 0.4f + seg(u, 0, 20) * 0.6f);
  BaerPose bp;
  bp.gold = seg(u, 9, 9.6f) * (1 - seg(u, 13, 14));
  float bx = 142, by = 214;
  if (u < 8) bx = 142;
  else { bx = lerp(142, -80, seg(u, 12, 18)); bp.lauf = u > 12 ? fmodf(u * 1.6f, 1) : -1; }
  bp.mund = u > 2 && u < 7 ? 0.4f : 0;
  if (u < 9) { g.ausschnitt(0, 0, 284, 240); }
  baer(g, bx, by, 0.95f, -1, bp, u);
  g.ausschnittAus();
  if (u > 8.6f && u < 11) {  // ein Fetzen Fell am Haken
    g.ellipse(132, 170, 5, 3, FARBE(0x6E4426));
  }
  FrauPose a = fp(SCHNEE, u), b = fp(ROSE, u + 1);
  a.armV = u > 6 && u < 9 ? 2.2f : 0.5f;  // macht den Riegel auf
  if (u > 9.6f && u < 14) { a.mund = 0.6f; a.blick = -1; }
  if (u > 12) { a.armV = 2.6f + sinf(u * 9) * 0.3f; b.armV = 2.6f + cosf(u * 9) * 0.3f; }  // winken
  frau(g, 200, 214, 0.85f, -1, a);
  frau(g, 232, 214, 0.85f, -1, b);
  blase(g, 110, 50, {"Nun muss ich fort", "und meine Schätze vor den", "bösen Zwergen hüten."}, aufzu(u, 2, 7.6f), 150, 150);
  if (u > 10 && u < 13) fragezeichen(g, 214, 110, 1, (uint8_t)(255 * seg(u, 10, 10.4f)));
  blende(g, u, 22, 150, 150);
}

// 4) Der erste Zwerg: mit dem Bart im Spalt des Baumstamms
static void szeneZwerg1(Gfx &g, float u) {
  wald(g, u, 0);
  // Baumstamm mit Spalt und Keil
  g.kontur(1.6f, OUT);
  g.rundRechteck(150, 176, 120, 28, 12, FARBE(0x8E5B3A));
  g.konturAus();
  g.ellipse(150, 190, 10, 14, FARBE(0xC99A6A));
  g.linie(150, 190, 214, 186, 2, FARBE(0x5A3A1E));
  float keil[] = {200, 174, 212, 174, 206, 188};
  if (u < 13) g.polygon(keil, 3, FARBE(0xC7D0DC));
  bool fest = u < 13.2f;
  Pose p;
  p.boese = 1;
  p.mund = fmodf(u, 0.3f) < 0.15f ? 0.6f : 0.3f;
  float zx = 120, zy = 214 - (fest ? fabsf(sinf(u * 6)) * 6 : 0);  // springt wie ein Huendchen an der Schnur hin und her
  if (fest) { zx = 118 + sinf(u * 3) * 8; p.armV = 2.4f; p.armH = 2.4f; }
  if (u > 16) { zx = 112; p.armV = 1.6f; }  // greift den Sack
  if (u > 21) { zx = lerp(112, -40, seg(u, 21, 25)); p.lauf = fmodf(u * 2, 1); }
  if (fest) zwerg(g, zx, zy, 0.62f, 1, p, u, 192, 186);
  else zwerg(g, zx, zy, 0.62f, u > 21 ? -1 : 1, p, u, 0, 0, 0.75f);
  if (u > 16 && u < 25) sack(g, zx + 12 * (u > 21 ? -1 : 1), 214, 0.9f, GOLD);
  else if (u <= 16) sack(g, 104, 214, 0.9f, GOLD);  // liegt zwischen den Wurzeln
  if (u > 13 && u < 14) { g.stern(186, 186, 8, 3, 6, u * 5, WEISS); }  // schnipp
  FrauPose a = fp(SCHNEE, u), b = fp(ROSE, u + 1);
  a.armV = u > 10 && u < 14 ? 1.8f : 0.5f;
  b.armV = u > 8 && u < 10 ? 2.4f : 0.5f;
  if (u > 26) { a.mund = b.mund = 0.4f; }
  float ahx, ahy;
  frau(g, 40, 214, 0.85f, 1, b);
  frau(g, 70, 214, 0.85f, 1, a, &ahx, &ahy);
  if (u > 10 && u < 14.5f) schere(g, u < 12.5f ? ahx + 4 : 186, u < 12.5f ? ahy - 2 : 184, 1, fabsf(sinf(u * 8)));
  blase(g, 150, 50, {"Was steht ihr da?", "Könnt ihr mir nicht helfen?"}, aufzu(u, 2.4f, 7.2f), 128, 150);
  blase(g, 70, 56, {"Ich will Leute holen."}, aufzu(u, 7.4f, 9.8f), 46, 130);
  blase(g, 90, 56, {"Ich weiß Rat!"}, aufzu(u, 10, 12.4f), 72, 130);
  blase(g, 150, 50, {"Ungehobeltes Volk,", "schneidet mir ein Stück", "von meinem stolzen Bart ab!"}, aufzu(u, 15.6f, 21), 120, 150);
  if (u > 26 && u < 34) fragezeichen(g, 56, 112, 1, (uint8_t)(255 * seg(u, 26, 26.4f)));
  blende(g, u, 36, 140, 150);
}

// 5) Der zweite Zwerg: der Bart in der Angelschnur, ein grosser Fisch zieht ihn zum Wasser
static void szeneZwerg2(Gfx &g, float u) {
  himmel(g, 0, u, 160);
  sonne(g, 240, 34, 12, u);
  boden(g, 150);
  g.rechteck(0, 186, 284, 54, FARBE(0x6EB8E8));  // Bach
  for (int i = 0; i < 8; i++) {
    float x = fmodf(i * 40 + u * 18, 320) - 20;
    g.ellipse(x, 198 + (i & 1) * 18, 12, 2, WEISS, 140);
  }
  g.rechteck(0, 150, 284, 36, GRAS);
  for (int i = 0; i < 5; i++) {  // Schilf
    float x = 30 + i * 56;
    g.linie(x, 188, x + sinf(u + i) * 3, 158, 2, GRAS_D);
    g.ellipse(x + sinf(u + i) * 3, 158, 2.5f, 7, FARBE(0x8E5B3A));
  }
  bool fest = u < 14.2f;
  float zug = fest ? sinf(u * 2.3f) * 6 + seg(u, 2, 8) * 20 : 0;  // der Fisch zieht
  Pose p;
  p.boese = 1;
  p.mund = fmodf(u, 0.3f) < 0.15f ? 0.6f : 0.3f;
  float zx = 150 + zug, zy = 186 - (fest ? fabsf(sinf(u * 7)) * 5 : 0);
  p.armV = fest ? 1.4f : 1.6f;
  if (u > 22) { zx = lerp(150, 330, seg(u, 22, 28)); p.lauf = fmodf(u * 2, 1); }
  // Angel und Fisch
  float fx = 210 + zug + sinf(u * 4) * 6, fy = 212 + sinf(u * 5) * 4;
  if (fest) {
    g.linie(zx + 8, zy - 40, zx + 40, zy - 66, 2, HOLZ);
    g.kurve(zx + 40, zy - 66, fx + 10, zy - 70, fx, fy, 1, OUT);
    zwerg(g, zx, zy, 0.62f, 1, p, u, fx - 8, fy - 6);
  } else {
    zwerg(g, zx, zy, 0.62f, u > 22 ? 1 : -1, p, u, 0, 0, 0.55f);
  }
  if (u < 16) fisch(g, fx, fy, 2.2f, sinf(u * 6) * 0.4f);
  else if (u < 18) fisch(g, lerp(fx, 300, seg(u, 16, 18)), 220, 2.2f, 0.3f);  // schwimmt davon
  if (u > 17 && u < 28) sack(g, zx + (u > 22 ? -12 : 12), 186, 0.9f, WEISS);  // Perlen aus dem Schilf
  FrauPose a = fp(SCHNEE, u), b = fp(ROSE, u + 1);
  float ax = lerp(-30, 96, sanft(seg(u, 4, 7))), bx = ax - 30;
  a.lauf = b.lauf = u < 7 ? fmodf(u * 1.6f, 1) : -1;
  if (u > 9 && u < 14) { a.armV = 1.8f; b.armV = 1.8f; }  // halten ihn fest
  float ahx, ahy;
  frau(g, bx, 186, 0.85f, 1, b);
  frau(g, ax, 186, 0.85f, 1, a, &ahx, &ahy);
  if (u > 11 && u < 15) schere(g, u < 13.5f ? ahx + 4 : fx - 30, u < 13.5f ? ahy - 2 : fy - 14, 1, fabsf(sinf(u * 8)));
  if (u > 14 && u < 15) g.stern(fx - 26, fy - 12, 8, 3, 6, u * 5, WEISS);
  blase(g, 150, 46, {"Ihr Lorche, ist das Manier,", "einem das Gesicht zu schänden?"}, aufzu(u, 16, 22), 150, 130);
  blende(g, u, 36, 140, 150);
}

// 6) Der dritte Zwerg: ein Adler packt ihn, die Maedchen halten ihn fest, das Roeckchen zerreisst
static void szeneZwerg3(Gfx &g, float u) {
  himmel(g, 0, u, 200);
  sonne(g, 240, 34, 12, u);
  huegel(g, 0, 0, 190, 30, 140, FARBE(0xC9C08A));
  boden(g, 202, FARBE(0xB5AE6A));
  for (int i = 0; i < 4; i++) steinbrocken(g, 30 + i * 70, 206, 12 + (i & 1) * 6);
  // Adler kreist, stoesst herab, packt den Zwerg (9-15), laesst los
  float ax, ay;
  float zx = 150, zy = 214;
  if (u < 8.6f) { ax = 142 + cosf(u * 0.9f) * 90; ay = 50 + sinf(u * 1.8f) * 14; }
  else if (u < 9.4f) { float k = seg(u, 8.6f, 9.4f); ax = lerp(142 + cosf(8.6f * 0.9f) * 90, 150, k); ay = lerp(50, 150, hinein(k)); }
  else if (u < 15) { ax = 150 + sinf(u * 3) * 6; ay = 150 - seg(u, 9.4f, 11) * 30 + seg(u, 11, 14) * 14; zy = ay + 64; zx = ax; }
  else { float k = seg(u, 15, 19); ax = lerp(150, 330, k); ay = lerp(130, 10, k); }
  Pose p;
  p.boese = 1;
  p.mund = 0.7f;
  p.armV = u > 9.4f && u < 15 ? 2.6f : 0.5f;
  if (u > 22) { zx = lerp(150, 70, seg(u, 22, 26)); p.lauf = fmodf(u * 2, 1); }
  if (u < 26.5f) zwerg(g, zx, zy, 0.62f, u > 22 ? -1 : 1, p, u, 0, 0, 0.55f);
  if (u > 15 && u < 20) g.ellipse(zx + 6, zy - 22, 4, 6, FARBE(0x9A7A5A));  // Loch im Roeckchen
  if (u > 20 && u < 26.5f) sack(g, zx + (u > 22 ? -12 : 12), 214, 0.9f, FARBE(0x6EC6FF));  // Edelsteine
  adler(g, ax, ay, 1, u, ax < 150 ? 1 : -1);
  if (u > 9.4f && u < 15) {  // Krallen halten den Zwerg
    g.linie(ax - 4, ay + 22, zx - 2, zy - 60, 2, FARBE(0xF2A93B));
    g.linie(ax + 4, ay + 22, zx + 4, zy - 60, 2, FARBE(0xF2A93B));
  }
  FrauPose a = fp(SCHNEE, u), b = fp(ROSE, u + 1);
  float axx = lerp(-30, 124, sanft(seg(u, 3, 7))), bxx = lerp(-60, 176, sanft(seg(u, 3.5f, 8)));
  a.lauf = b.lauf = u < 8 ? fmodf(u * 1.8f, 1) : -1;
  if (u > 10 && u < 15) { a.armV = b.armV = 2.6f; axx = 136; bxx = 164; }  // halten ihn fest
  frau(g, axx, 214, 0.85f, 1, a);
  frau(g, bxx, 214, 0.85f, -1, b);
  blase(g, 150, 46, {"Hättet ihr nicht säuberlicher", "mit mir umgehen können?", "Mein Röckchen ist zerrissen!"}, aufzu(u, 15.6f, 21.4f), 150, 150);
  if (u > 26 && u < 27.5f) puff(g, 70, 200, seg(u, 26, 27.5f), STEIN);  // schluepft unter den Stein
  blende(g, u, 32, 150, 150);
}

// 7) Die Edelsteine auf dem reinen Platz; der Baer; der Prankenschlag; der Koenigssohn
static void szeneBaer(Gfx &g, float u) {
  float abend = 0.5f + 0.5f * seg(u, 0, 10);
  wald(g, u, abend);
  // Edelsteine glitzern in der Abendsonne
  const Farbe stein[] = {FARBE(0xE8283F), FARBE(0x3E7FDC), FARBE(0x3DAE5B), GOLD, FARBE(0xB28DFF), WEISS};
  for (int i = 0; i < 18; i++) {
    float x = 110 + zufall(i + 800) * 80, y = 206 + zufall(i + 820) * 14;
    float f = 0.5f + 0.5f * sinf(u * 4 + i * 1.3f);
    g.stern(x, y, 3 + f * 2, 1.4f, 4, i, stein[i % 6], (uint8_t)(180 + f * 75));
  }
  bool zwergDa = u < 21.6f;
  Pose p;
  p.boese = u < 14 ? 1 : 0;
  p.mund = u > 8 && u < 21 ? 0.7f : 0.2f;
  if (u > 14) { p.armV = 2.7f; p.armH = 2.7f; }  // fleht
  if (zwergDa) zwerg(g, 150, 214, 0.62f, -1, p, u, 0, 0, 0.5f);
  puff(g, 150, 190, seg(u, 21.6f, 23));
  // der Baer kommt aus dem Wald
  BaerPose bp;
  float bx = lerp(340, 214, sanft(seg(u, 12, 16)));
  bp.lauf = u > 12 && u < 16 ? fmodf(u * 1.6f, 1) : -1;
  bp.aufrecht = u > 16;
  bp.pranke = u > 20.8f && u < 22 ? sanft(seg(u, 20.8f, 21.4f)) : 0;
  bp.mund = (u > 25 && u < 29) ? 0.5f : 0;
  bool prinz = u > 30.2f;
  if (!prinz) baer(g, bx, 214, 0.9f, -1, bp, u);
  if (u > 29 && u < 31.5f) {  // das Baerenfell faellt ab
    float k = seg(u, 29.4f, 31.5f);
    g.ellipse(bx, 214 - 4 * k, 34 + 6 * k, lerp(30, 6, k), FARBE(0x6E4426), (uint8_t)(255 * (1 - seg(u, 31, 31.5f)) + 80));
  }
  if (u > 31) g.ellipse(bx + 14, 212, 26, 5, FARBE(0x6E4426));  // das Fell liegt da
  puff(g, bx, 170, seg(u, 29.6f, 31.2f));
  if (prinz) {
    Pose pr;
    pr.aussehen = &PRINZ;
    pr.mund = u > 33 && u < 40 ? 0.4f : 0.2f;
    pr.armV = u > 40 ? 2.4f : 0.8f;
    schneiderlein(g, bx, 214, 1, -1, pr);
    glitzern(g, bx, 150, 40, u, 6, (uint8_t)(255 * (1 - seg(u, 34, 38))));
  }
  // die Maedchen
  FrauPose a = fp(SCHNEE, u), b = fp(ROSE, u + 1);
  float ax = lerp(-30, 74, sanft(seg(u, 2, 5))), bx2 = ax - 30;
  a.lauf = b.lauf = u < 5 ? fmodf(u * 1.6f, 1) : -1;
  if (u > 4 && u < 8) { a.mund = b.mund = 0.6f; }
  if (u > 22 && u < 26) { ax = lerp(74, 30, seg(u, 22, 25)); bx2 = ax - 30; a.lauf = b.lauf = fmodf(u * 2, 1); a.mund = b.mund = 0.8f; }  // laufen fort
  if (u >= 26) { ax = 30; bx2 = 0; }
  if (u > 31) { ax = lerp(30, 90, sanft(seg(u, 31, 34))); bx2 = ax - 30; a.mund = b.mund = 0.5f; }
  frau(g, bx2, 214, 0.85f, 1, b);
  frau(g, ax, 214, 0.85f, 1, a);
  if (u > 40)
    for (int i = 0; i < 5; i++) {
      float k = fmodf((u - 40) * 0.45f + i / 5.0f, 1);
      herz(g, 150 + sinf(k * 7 + i) * 24, 150 - k * 100, 5 + (i % 3), FARBE(0xFF5A7A), (uint8_t)(255 * huepf(k)));
    }
  blase(g, 150, 50, {"Was steht ihr da und haltet", "Maulaffen feil?"}, aufzu(u, 8.2f, 12), 150, 160);
  blase(g, 150, 46, {"Lieber Herr Bär, verschont mich!", "Packt die zwei Mädchen,", "das sind zarte Bissen!"}, aufzu(u, 16.2f, 20.8f), 150, 160);
  blase(g, 190, 46, {"Schneeweißchen und Rosenrot,", "fürchtet euch nicht,", "wartet, ich will mit euch gehen!"}, aufzu(u, 24.6f, 29.2f), 214, 110);
  blase(g, 170, 46, {"Ich bin eines Königs Sohn.", "Der gottlose Zwerg hatte mich", "in einen wilden Bären verwünscht."}, aufzu(u, 33, 39.6f), 196, 110);
  blende(g, u, 48, 150, 150);
}

// 8) Hochzeit: Schneeweisschen mit dem Koenigssohn, Rosenrot mit seinem Bruder; die zwei Rosenbaeumchen
static void szeneHochzeit(Gfx &g, float u) {
  if (u < 30) {
    himmel(g, 0, u, 196);
    sonne(g, 244, 34, 12, u);
    schloss(g, 142, 196, u, 0);
    boden(g, 196);
    float tep[] = {120, 196, 164, 196, 200, 240, 84, 240};
    g.polygon(tep, 4, FARBE(0xD8283F));
    for (int i = 0; i < 3; i++) sack(g, 20 + i * 22, 232, 0.9f, i == 0 ? GOLD : i == 1 ? WEISS : FARBE(0x6EC6FF));
    FrauPose m = fp(MUTTER, u);
    m.mund = 0.4f;
    frau(g, 252, 214, 0.85f, -1, m);
    FrauPose a = fp(BRAUT_W, u), b = fp(BRAUT_R, u + 1);
    a.mund = b.mund = 0.4f;
    Pose p, q;
    p.aussehen = &PRINZ; q.aussehen = &BRUDER;
    p.mund = q.mund = 0.4f;
    p.armV = q.armV = u > 8 ? 2.5f + sinf(u * 8) * 0.3f : 0.8f;
    schneiderlein(g, 74, 214, 0.92f, 1, p);
    frau(g, 108, 214, 0.88f, -1, a);
    frau(g, 170, 214, 0.88f, 1, b);
    schneiderlein(g, 204, 214, 0.92f, -1, q);
    for (int i = 0; i < 6; i++) {
      float k = fmodf(u * 0.45f + i / 6.0f, 1);
      herz(g, 140 + sinf(k * 7 + i) * 40, 150 - k * 110, 5 + (i % 3), FARBE(0xFF5A7A), (uint8_t)(255 * huepf(k)));
    }
    if (u > 4)
      for (int i = 0; i < 18; i++) {
        float x = zufall(i + 2990) * 284 + sinf(u * 2 + i) * 12;
        float y = fmodf(zufall(i + 2960) * 240 + (u - 4) * 40, 250) - 10;
        g.ellipse(x, y, 3, 2, i & 1 ? FARBE(0xFF3050) : WEISS, 230, u + i);  // weisse und rote Rosenblaetter
      }
    blende(g, u, 30, 140, 150);
    return;
  }
  // die zwei Rosenbaeumchen vor dem Fenster, sie bluehen jedes Jahr weiss und rot
  float v = u - 30;
  g.verlauf(0, 240, FARBE(0xF6E2B4), FARBE(0xE8C483));
  g.kontur(2, OUT);
  g.rundRechteck(70, 40, 144, 120, 10, HOLZ);
  g.konturAus();
  g.rundRechteck(78, 48, 128, 104, 6, FARBE(0xBFE6FF));
  g.rechteck(140, 48, 4, 104, HOLZ);
  g.rechteck(78, 98, 128, 4, HOLZ);
  sonne(g, 180, 72, 10, u);
  g.rechteck(60, 158, 164, 10, HOLZ_D);
  rosenbusch(g, 96, 240, 1.4f, WEISS, u, seg(v, 0.5f, 4));
  rosenbusch(g, 188, 240, 1.4f, FARBE(0xE8283F), u, seg(v, 0.5f, 4));
  g.kameraAus();
  if (v < 1.2f) g.iris(142, 160, lerp(0, 330, sanft(v / 1.2f)));
  if (v > 16) g.iris(142, 170, lerp(330, 0, sanft(seg(v, 16, 18.5f))));
  if (v > 18.5f) {
    uint8_t a = (uint8_t)(255 * sanft(seg(v, 18.5f, 19.5f)));
    g.text(SCHRIFT_G, "Ende", 142, 122, GOLD, 1, a);
    for (int i = 0; i < 6; i++) {
      float an = v * 1.2f + i * 1.047f;
      g.stern(142 + cosf(an) * 60, 112 + sinf(an) * 30, 5, 2, 5, an, i & 1 ? FARBE(0xFF3050) : WEISS, a);
    }
  }
}

static void bild(Gfx &g, float t) {
  if (t < R2) szeneAnfang(g, t - R1);
  else if (t < R3) szeneWinter(g, t - R2);
  else if (t < R4) szeneAbschied(g, t - R3);
  else if (t < R5) szeneZwerg1(g, t - R4);
  else if (t < R6) szeneZwerg2(g, t - R5);
  else if (t < R7) szeneZwerg3(g, t - R6);
  else if (t < R8) szeneBaer(g, t - R7);
  else szeneHochzeit(g, t - R8);
}

// =====================================================================
// Musik
// =====================================================================
// Rosenlied (F-Dur, wiegend)
static const Ton3 MEL_ROSEN[] = {
  {72, 1, 53}, {74, .5, 0}, {76, .5, 0}, {77, 1, 0}, {72, 1, 0}, {74, 1, 46}, {70, 1, 0}, {72, 2, 0},
  {69, 1, 48}, {72, .5, 0}, {77, .5, 0}, {76, 1, 0}, {74, 1, 0}, {72, 1, 53}, {69, 1, 0}, {65, 2, 0}};
// Baerentanz (g-Moll, tapsig)
static const Ton3 MEL_BAER[] = {
  {55, 1, 43}, {58, .5, 0}, {62, .5, 0}, {58, 1, 0}, {55, 1, 0}, {57, 1, 45}, {60, .5, 0}, {63, .5, 0}, {62, 2, 0},
  {55, 1, 43}, {58, .5, 0}, {62, .5, 0}, {67, 1, 0}, {66, 1, 0}, {67, 1, 43}, {62, 1, 0}, {55, 2, 0}};
// Zwergenzank (schnell, keifend)
static const Ton3 MEL_ZWERG[] = {
  {81, .25, 57}, {80, .25, 0}, {81, .25, 0}, {76, .25, 0}, {77, .5, 0}, {74, .5, 0}, {81, .25, 52}, {80, .25, 0},
  {81, .25, 0}, {84, .25, 0}, {83, .5, 0}, {79, .5, 0}, {76, .5, 57}, {77, .5, 0}, {74, 1, 0}};

static void ton(Synth &s, float t0, float t1) {
  float a = R1;
  spieleMelodie(s, M(MEL_ROSEN), a + 0.5f, a + 23.5f, 0.32f, t0, t1, 0.9f);
  for (float x = ceilf(t0 / 2.6f) * 2.6f; x < t1; x += 2.6f)
    if (x > a + 12 && x < a + 24) for (int k = 0; k < 4; k++) s.note(x + k * 0.08f, 93 + (k & 1) * 3, 0.3f);  // Voegel
  spieleMelodie(s, M(MEL_WIEGE), a + 24.5f, a + 33.5f, 0.42f, t0, t1, 0.7f);
  if (bei(a + 25.5f, t0, t1)) for (int k = 0; k < 8; k++) s.glocke(a + 25.5f + k * 0.2f, 84 + (k % 4) * 3, 0.35f);  // Engel
  a = R2;
  spieleMelodie(s, M(MEL_WIEGE), a + 0.5f, a + 3.5f, 0.42f, t0, t1, 0.6f);
  for (int i = 0; i < 3; i++) if (bei(a + 4 + i * 0.3f, t0, t1)) s.ton(a + 4 + i * 0.3f, 160, 0.35f, 0.002f, 0.12f, SINUS, GERAEUSCH, 90);  // es klopft
  if (bei(a + 7.4f, t0, t1)) s.ton(a + 7.4f, 110, 0.14f, 0.1f, 0.7f, DREIECK, GERAEUSCH, 80);  // Brummen
  if (bei(a + 7.8f, t0, t1)) for (int k = 0; k < 6; k++) s.note(a + 7.8f + k * 0.08f, 88 + (k & 1) * 4, 0.5f);  // Schreck
  spieleMelodie(s, M(MEL_BAER), a + 10, a + 37, 0.34f, t0, t1, 0.85f);
  for (float x = ceilf(t0 / 0.35f) * 0.35f; x < t1; x += 0.35f)
    if (x > a + 20 && x < a + 25) s.rauschen(x, 0.15f, BANDPASS, 1600, 0.05f, 0.8f, 0.03f, 2600);  // kehren
  for (float x = ceilf(t0 / 2.0f) * 2.0f; x < t1; x += 2.0f)
    if (x > a + 25 && x < a + 31) s.ton(x, 100, 0.12f, 0.08f, 0.5f, DREIECK, GERAEUSCH, 85);  // lacht brummend
  spieleMelodie(s, M(MEL_WIEGE), a + 37, a + 39.5f, 0.42f, t0, t1, 0.6f);
  a = R3;
  spieleMelodie(s, M(MEL_ROSEN), a + 0.5f, a + 8.5f, 0.32f, t0, t1, 0.8f);
  if (bei(a + 8.6f, t0, t1)) { s.rauschen(a + 8.6f, 0.15f, HOCHPASS, 2500, 0.1f); s.glocke(a + 9.2f, 88, 0.6f); }  // reisst, Gold
  spieleMelodie(s, M(MEL_BAER), a + 12, a + 21, 0.28f, t0, t1, 0.7f);
  for (float base : {R4, R5, R6}) {
    spieleMelodie(s, M(MEL_ZWERG), base + 1.5f, base + 9.5f, 0.24f, t0, t1, 0.8f);
    spieleMelodie(s, M(MEL_ZWERG), base + 15.5f, base + 21.5f, 0.22f, t0, t1, 0.8f);
    spieleMelodie(s, M(MEL_ROSEN), base + 26, base + 31, 0.32f, t0, t1, 0.6f);
  }
  for (float x : {R4 + 13.0f, R5 + 14.0f}) if (bei(x, t0, t1)) { s.ton(x, 3200, 0.1f, 0.002f, 0.05f); s.ton(x + 0.08f, 3600, 0.08f, 0.002f, 0.05f); }  // schnipp
  for (float x = ceilf(t0 / 0.4f) * 0.4f; x < t1; x += 0.4f)
    if (x > R5 + 1 && x < R5 + 15) s.rauschen(x, 0.1f, TIEFPASS, 1200, 0.05f);  // plaetschern
  for (float x = ceilf(t0 / 0.9f) * 0.9f; x < t1; x += 0.9f)
    if (x > R6 && x < R6 + 15) s.rauschen(x, 0.25f, BANDPASS, 700, 0.08f, 0.8f, 0.05f, 1400);  // Fluegelschlag
  if (bei(R6 + 8.8f, t0, t1)) s.ton(R6 + 8.8f, 1400, 0.12f, 0.02f, 0.6f, DREIECK, GERAEUSCH, 900);  // Adlerschrei
  if (bei(R6 + 15, t0, t1)) s.rauschen(R6 + 15, 0.2f, HOCHPASS, 2500, 0.15f);  // ratsch
  a = R7;
  spieleMelodie(s, M(MEL_ROSEN), a + 0.5f, a + 8, 0.32f, t0, t1, 0.8f);
  for (float x = ceilf(t0 / 0.5f) * 0.5f; x < t1; x += 0.5f)
    if (x > a + 2 && x < a + 12) s.ton(x, 2800 + fmodf(x * 31, 5) * 300, 0.03f, 0.002f, 0.15f);  // Edelsteine funkeln
  spieleMelodie(s, M(MEL_ZWERG), a + 8.2f, a + 12, 0.22f, t0, t1, 0.8f);
  if (bei(a + 12, t0, t1)) s.ton(a + 12, 90, 0.18f, 0.1f, 1.2f, DREIECK, GERAEUSCH, 70);  // Brummen
  spieleMelodie(s, M(MEL_BAER), a + 13, a + 20.6f, 0.26f, t0, t1, 0.9f);
  if (bei(a + 21.2f, t0, t1)) { s.ton(a + 21.2f, 80, 0.4f, 0.002f, 0.35f, SINUS, GERAEUSCH, 40); s.note(a + 21.2f, 43, 1); }  // Prankenschlag
  if (bei(a + 21.8f, t0, t1)) for (int k = 0; k < 6; k++) s.note(a + 21.8f + k * 0.07f, 96 - k * 2, 0.4f);
  spieleMelodie(s, M(MEL_JAGD), a + 22, a + 25, 0.16f, t0, t1, 0.7f);
  if (bei(a + 29.6f, t0, t1)) for (int k = 0; k < 12; k++) s.glocke(a + 29.6f + k * 0.14f, 72 + (k % 6) * 2, 0.5f);  // Verwandlung
  spieleMelodie(s, M(MEL_F), a + 33, a + 47.5f, 0.26f, t0, t1, 0.85f);
  a = R8;
  spieleMelodie(s, M(MEL_F), a + 0.5f, a + 8, 0.24f, t0, t1, 1);
  for (int i = 0; i < 14; i++) if (bei(a + 8 + i * 0.6f, t0, t1)) s.glocke(a + 8 + i * 0.6f, i & 1 ? 79 : 72 + (i % 4 == 2) * 4, 0.85f);
  spieleMelodie(s, M(MEL_ROSEN), a + 30.5f, a + 47.5f, 0.32f, t0, t1, 0.9f);
  if (bei(a + 48.5f, t0, t1)) { s.glocke(a + 48.5f, 72); s.glocke(a + 48.8f, 76); s.glocke(a + 49.1f, 79); s.glocke(a + 49.5f, 84); }
}

// =====================================================================
// Startbild und Vorschau
// =====================================================================
static void titelSzene(Gfx &g, float t) {
  huetteAussen(g, t, 0, 0, 1);
  FrauPose a = fp(SCHNEE, t), b = fp(ROSE, t + 1);
  a.armV = 1.6f; b.armV = 1.6f;
  a.mund = b.mund = 0.3f;
  frau(g, 52, 222, 1, 1, a);
  frau(g, 88, 222, 1, -1, b);
  BaerPose bp;
  bp.zu = fmodf(t, 3.4f) < 0.15f;
  baer(g, 236, 222, 0.75f, -1, bp, t);
}

static void titelbild(Gfx &g, float t) {
  titelSzene(g, t);
  g.rundRechteck(104, 18, 176, 70, 14, FARBE(0x1E1A3A), 160);
  g.text(SCHRIFT_M, "Schneeweißchen", 192, 46, WEISS, 1);
  g.text(SCHRIFT_K, "und", 192, 62, GOLD, 1);
  g.text(SCHRIFT_M, "Rosenrot", 192, 82, FARBE(0xFF6A7A), 1);
  float b = 1 + sinf(t * 4) * 0.08f;
  g.kontur(2, OUT);
  g.kreis(142, 116, 14 * b, FARBE(0xE8473D));
  g.konturAus();
  float tri[] = {137, 108, 137, 124, 150, 116};
  g.polygon(tri, 3, WEISS);
}

static void vorschau(Gfx &g, float t) { titelSzene(g, t); }

static void symbol(Gfx &g, float x, float y, float t) {
  rosenbusch(g, x - 42, y, 0.7f, WEISS, t);
  rosenbusch(g, x + 42, y, 0.7f, FARBE(0xE8283F), t);
  BaerPose b;
  b.mund = 0.3f;
  baer(g, x, y, 0.55f, 1, b, t);
}

const Maerchen ROSENROT = {"Schneeweißchen und Rosenrot", RDAUER, FARBE(0xE8283F), bild, ton, titelbild, vorschau, symbol, LIED(MEL_ROSEN, 0.4f)};

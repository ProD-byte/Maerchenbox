// Cinderella - ein Spieluhr-Film fuer die Maerchenbox, nach dem Maerchen von Charles Perrault
// ("Cendrillon ou la petite pantoufle de verre"): die Patin, der Kuerbis, die Maeuse, die Ratte und die Eidechsen,
// das Kleid aus Gold und Silber, die glaesernen Pantoeffelchen, Mitternacht und die Schuhprobe.
#include "maerchen.h"
#include "schriften.h"
#include "figuren.h"
#include "melodien.h"
#include <stdio.h>
#include <initializer_list>

// ---------- Ablauf (Sekunden) ----------
static const float P1 = 0, P2 = 34, P3 = 96, P4 = 114, P5 = 146, P6 = 192, P7 = 256, PDAUER = 292;

// ---------- Aussehen ----------
static FrauLook lumpenLook() {
  FrauLook l; l.kleid = FARBE(0x9A8E80); l.schuerze = FARBE(0x7E7268); l.haar = FARBE(0x8A5A2E); l.frisur = 3;
  l.tuch = FARBE(0x8E8070); l.flicken = true; l.schuh = FARBE(0x6E5A4A);
  return l;
}
static FrauLook ballLook() {
  FrauLook l; l.kleid = FARBE(0xF2CF6A); l.haar = FARBE(0x8A5A2E); l.frisur = 2; l.glanz = true; l.rockBreite = 1.45f;
  l.blume = FARBE(0xE8F4FF); l.schuh = FARBE(0xBFE6FF);
  return l;
}
static FrauLook brautLook() { FrauLook l = ballLook(); l.kleid = WEISS; l.krone = true; return l; }
static FrauLook stiefmutterLook() {
  FrauLook l; l.kleid = FARBE(0x6E3A6E); l.haar = FARBE(0x5A4A4A); l.frisur = 2; l.wangen = false; l.rockBreite = 1.2f;
  return l;
}
static FrauLook schwester1() { FrauLook l; l.kleid = FARBE(0xE07A2A); l.haar = FARBE(0x3A2A20); l.frisur = 2; l.rockBreite = 1.3f; l.blume = FARBE(0xC0392B); return l; }
static FrauLook schwester2() { FrauLook l; l.kleid = FARBE(0x6EA84E); l.haar = FARBE(0xC0632E); l.frisur = 1; l.rockBreite = 1.3f; l.blume = FARBE(0xFFD54A); return l; }
static FrauLook patinLook() {
  FrauLook l; l.kleid = FARBE(0xE8B8D8); l.haar = FARBE(0xF2F2F2); l.frisur = 2; l.glanz = true; l.schuerze = FARBE(0xFFF0F8);
  return l;
}
static const FrauLook LUMPEN = lumpenLook(), BALL = ballLook(), BRAUT = brautLook(), STIEF = stiefmutterLook(),
                      SCHW1 = schwester1(), SCHW2 = schwester2(), PATIN = patinLook();

static Aussehen prinzLook() {
  Aussehen a; a.weste = FARBE(0x2E5AA8); a.hemd = FARBE(0xFFF6E0); a.hose = FARBE(0xF2F2F2); a.haar = FARBE(0x3A2A20);
  a.muetze = 3; a.halstuch = GOLD;
  return a;
}
static Aussehen kutscherLook() {
  Aussehen a; a.weste = FARBE(0x8E8E9A); a.hemd = FARBE(0xC9C9D2); a.hose = FARBE(0x5E5E6A); a.haar = FARBE(0x9A9AA6);
  a.hut = FARBE(0x3A3A4A); a.hutD = FARBE(0x2A2A36); a.feder = GOLD; a.haut = FARBE(0xE8D2C0);
  return a;
}
static Aussehen lakaiLook() {
  Aussehen a; a.weste = FARBE(0x3DAE5B); a.hemd = FARBE(0xFFF6E0); a.hose = FARBE(0x2C8A45); a.haar = FARBE(0xE8E8E8);
  a.muetze = 1; a.halstuch = GOLD;
  return a;
}
static Aussehen heroldLook() {
  Aussehen a; a.weste = FARBE(0xC0392B); a.hemd = GOLD; a.hose = FARBE(0x2E3A6E); a.haar = FARBE(0x5A3A26);
  a.hut = FARBE(0xC0392B); a.hutD = FARBE(0x8E2A20); a.feder = WEISS;
  return a;
}
static const Aussehen PRINZ = prinzLook(), KUTSCHER = kutscherLook(), LAKAI = lakaiLook(), HEROLD = heroldLook();

static FrauPose fp(const FrauLook &l, float t) {
  FrauPose p;
  p.look = &l;
  p.zeit = t;
  p.zu = fmodf(t + l.kleid * 0.0007f, 3.7f) < 0.13f;
  return p;
}
static Pose mp(const Aussehen &a, float t) {
  Pose p;
  p.aussehen = &a;
  p.zu = fmodf(t + 0.4f, 4.1f) < 0.13f;
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
  if (y - bh / 2 < 3) y = 3 + bh / 2;
  inKreis(x, y, bw, bh);  // rundes Board: ganz in den sichtbaren Kreis
  float sp[] = {x - 6, y + bh / 2 - 2, x + 6, y + bh / 2 - 2, sx, sy};
  g.kontur(1.4f, OUT);
  g.rundRechteck(x - bw / 2, y - bh / 2, bw, bh, 9 * k, f);
  g.konturAus();
  g.polygon(sp, 3, f);
  if (k > 0.85f) {
    blaseLesbar = 1;
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

// ---------- Dinge und Tiere ----------
static void glasschuh(Gfx &g, float x, float y, float s) {
  g.kontur(1.4f, FARBE(0x3E78B8));
  float q[] = {x - 9 * s, y - 1 * s, x + 9 * s, y, x + 11 * s, y - 3 * s, x + 2 * s, y - 6 * s, x - 7 * s, y - 9 * s, x - 10 * s, y - 5 * s};
  g.polygon(q, 6, FARBE(0xBFE4FF), 235);
  g.konturAus();
  g.linie(x - 8 * s, y - 2 * s, x - 8 * s, y + 3 * s, 1.6f * s, FARBE(0x7AB0E0));  // Absatz
  g.linie(x - 5 * s, y - 6 * s, x + 4 * s, y - 3 * s, 0.9f * s, WEISS, 220);
  g.stern(x + 3 * s, y - 4 * s, 2.5f * s, 1 * s, 4, 0, WEISS, 240);
}

static void zauberstab(Gfx &g, float x, float y, float t, float leucht = 0) {
  g.linie(x, y, x + 6, y - 18, 1.8f, FARBE(0xF2F2F2));
  g.stern(x + 6, y - 20, 5 + leucht * 2, 2.2f, 5, t * 2, GOLD);
  if (leucht > 0) g.kreis(x + 6, y - 20, 12, GOLD, (uint8_t)(90 * leucht));
}

static void kuerbis(Gfx &g, float x, float y, float s) {
  g.kontur(1.3f * s, OUT);
  g.ellipse(x, y - 16 * s, 26 * s, 17 * s, FARBE(0xF08A2A));
  g.konturAus();
  for (int i = -1; i <= 1; i++) g.ellipse(x + i * 10 * s, y - 16 * s, 6 * s, 16 * s, FARBE(0xD9701E));
  g.linie(x, y - 32 * s, x + 3 * s, y - 40 * s, 3 * s, FARBE(0x5A7A2E));
}

// goldene Kutsche; lauf >= 0 dreht die Raeder
static void kutsche(Gfx &g, float x, float y, float s, float lauf) {
  float dreh = lauf >= 0 ? lauf * 6.28f : 0;
  g.kontur(1.6f * s, OUT);
  g.ellipse(x, y - 44 * s, 38 * s, 30 * s, GOLD);
  g.konturAus();
  for (int i = -1; i <= 1; i++) g.ellipse(x + i * 14 * s, y - 44 * s, 6 * s, 28 * s, FARBE(0xF2C040));
  g.rundRechteck(x - 12 * s, y - 60 * s, 24 * s, 22 * s, 8 * s, FARBE(0x8FD3FF));
  g.stern(x, y - 78 * s, 6 * s, 2.5f * s, 5, 0, GOLD);
  for (int i = 0; i < 2; i++) {
    float wx = x + (i ? 24 : -24) * s;
    g.kontur(1.3f * s, OUT);
    g.kreis(wx, y - 12 * s, 12 * s, FARBE(0xE0A23C));
    g.konturAus();
    for (int k = 0; k < 4; k++) {
      float a = dreh + k * 0.785f;
      g.linie(wx - cosf(a) * 10 * s, y - 12 * s - sinf(a) * 10 * s, wx + cosf(a) * 10 * s, y - 12 * s + sinf(a) * 10 * s, 1.4f * s, FARBE(0x8E5A2A));
    }
  }
}

static void maus(Gfx &g, float x, float y, float s, float dir, float lauf) {
  float hop = lauf >= 0 ? fabsf(sinf(lauf * 6.28f)) * 2 : 0;
  g.kurve(x - 6 * s * dir, y - 3 * s, x - 14 * s * dir, y - 8 * s, x - 18 * s * dir, y - 2 * s, 1 * s, FARBE(0xC9A0A0));
  g.kontur(1 * s, OUT);
  g.ellipse(x, y - 4 * s - hop, 8 * s, 5 * s, FARBE(0xA8A8B0));
  g.konturAus();
  g.kreis(x + 6 * s * dir, y - 9 * s - hop, 3 * s, FARBE(0xE8C0C0));
  g.kreis(x + 8 * s * dir, y - 4 * s - hop, 1 * s, SCHWARZ);
  g.kreis(x + 11 * s * dir, y - 3 * s - hop, 1 * s, FARBE(0xFF8CA0));
}

static void eidechse(Gfx &g, float x, float y, float s, float t) {
  g.kurve(x - 10 * s, y - 2 * s, x - 18 * s, y - 2 * s + sinf(t * 8) * 3 * s, x - 24 * s, y - 4 * s, 2 * s, FARBE(0x4CA85A));
  g.ellipse(x, y - 3 * s, 10 * s, 3.5f * s, FARBE(0x5DBE5A));
  g.kreis(x + 10 * s, y - 4 * s, 3 * s, FARBE(0x5DBE5A));
  g.kreis(x + 11 * s, y - 5 * s, 0.8f * s, SCHWARZ);
}

static void ratte(Gfx &g, float x, float y, float s, float dir) {
  g.kurve(x - 8 * s * dir, y - 3 * s, x - 20 * s * dir, y - 10 * s, x - 26 * s * dir, y - 2 * s, 1.4f * s, FARBE(0xC9A0A0));
  g.kontur(1 * s, OUT);
  g.ellipse(x, y - 6 * s, 12 * s, 7 * s, FARBE(0x7E7068));
  g.konturAus();
  g.kreis(x + 9 * s * dir, y - 12 * s, 3.5f * s, FARBE(0xC9A0A0));
  g.kreis(x + 11 * s * dir, y - 7 * s, 1.2f * s, SCHWARZ);
  for (int i = -1; i <= 1; i++) g.linie(x + 14 * s * dir, y - 5 * s, x + 20 * s * dir, y - 5 * s + i * 2 * s, 0.7f, OUT);
}

// Schlossuhr; zeit in Minuten nach 23 Uhr (60 = Mitternacht)
static void uhr(Gfx &g, float x, float y, float r, float minuten) {
  g.kontur(2, OUT);
  g.kreis(x, y, r, FARBE(0xFFF6E0));
  g.konturAus();
  g.bogen(x, y, r - 1, 0, 6.29f, 3, GOLD);
  for (int i = 0; i < 12; i++) {
    float a = i * 0.5236f;
    g.kreis(x + sinf(a) * (r - 7), y - cosf(a) * (r - 7), 1.6f, OUT);
  }
  float am = minuten / 60 * 6.2832f, as = (11 + minuten / 60) / 12 * 6.2832f;
  g.linie(x, y, x + sinf(as) * r * 0.5f, y - cosf(as) * r * 0.5f, 3, OUT);
  g.linie(x, y, x + sinf(am) * r * 0.8f, y - cosf(am) * r * 0.8f, 2, OUT);
  g.kreis(x, y, 2.5f, GOLD);
}

// Kutscher mit dickem Schnurrbart (die Ratte)
static void kutscher(Gfx &g, float x, float y, float s, float dir, float t) {
  Pose p = mp(KUTSCHER, t);
  p.armV = 1.6f;
  float hx, hy;
  schneiderlein(g, x, y, s, dir, p, &hx, &hy);
  float cx = x + 6 * s * dir, cy = y - 52 * s;
  g.ellipse(cx - 4 * s, cy, 6 * s, 2.5f * s, FARBE(0x5E5E6A), 255, -0.3f);
  g.ellipse(cx + 4 * s, cy, 6 * s, 2.5f * s, FARBE(0x5E5E6A), 255, 0.3f);
  g.linie(hx, hy, hx + 8 * dir, hy - 24, 1.4f, HOLZ_D);  // Peitsche
}

// ---------- Kulissen ----------
static void kueche(Gfx &g, float t, float nacht) {
  g.verlauf(0, 200, mischeFarbe(FARBE(0xD9B48A), FARBE(0x5E4A3A), nacht), mischeFarbe(FARBE(0xB88E64), FARBE(0x3E3028), nacht));
  g.rechteck(0, 200, 284, 40, FARBE(0x7E6A56));
  g.kontur(2, OUT);
  g.rundRechteck(150, 110, 110, 92, 6, FARBE(0xA89888));
  g.konturAus();
  g.rechteck(166, 30, 78, 82, FARBE(0x988878));
  g.rundRechteck(168, 140, 74, 60, 24, FARBE(0x2E2620));
  for (int i = 0; i < 4; i++) {
    float fx = 182 + i * 14, h = 14 + 5 * sinf(t * 9 + i * 1.7f);
    float f[] = {fx - 6, 196, fx + 6, 196, fx + sinf(t * 7 + i) * 2, 196 - h};
    g.polygon(f, 3, FARBE(0xFF8A2A));
  }
  g.ellipse(130, 214, 36, 7, FARBE(0x9A9490));  // Asche
  g.ellipse(130, 211, 22, 4, FARBE(0xB5B0AC));
  g.rechteck(20, 70, 90, 5, HOLZ_D);
  for (int i = 0; i < 4; i++) {
    g.kontur(1.2f, OUT);
    g.rundRechteck(26 + i * 22, 52, 16, 18, 4, i & 1 ? FARBE(0xC0632E) : FARBE(0x9AA6B8));
    g.konturAus();
  }
  if (nacht > 0) g.kreis(205, 170, 70, FARBE(0xFFB050), (uint8_t)(30 * nacht));
  g.kontur(1.8f, OUT);
  g.rundRechteck(30, 100, 50, 46, 4, HOLZ);
  g.konturAus();
  g.rundRechteck(34, 104, 42, 38, 2, mischeFarbe(FARBE(0xBFE6FF), FARBE(0x34306E), nacht));
  if (nacht > 0.5f) g.kreis(64, 116, 5, FARBE(0xFFF6D8));
}

static void garten(Gfx &g, float t) {
  himmel(g, 1, t, 200);
  for (int i = 0; i < 22; i++) g.stern(zufall(i + 700) * 284, zufall(i + 720) * 100, 2, 0.8f, 4, 0, WEISS, 200);
  mond(g, 240, 36, 11);
  boden(g, 200, FARBE(0x2F4A40));
  g.kontur(1.8f, OUT);
  g.rechteck(0, 90, 90, 110, FARBE(0x8E7E6E));
  float d[] = {-10, 92, 45, 56, 100, 92};
  g.polygon(d, 3, FARBE(0x5A3A3A));
  g.konturAus();
  g.rundRechteck(24, 120, 22, 20, 2, FARBE(0xFFD27A));
  for (int i = 0; i < 6; i++) g.ellipse(120 + i * 28, 206, 10, 4, FARBE(0x3E6A3A));  // Gemuesebeet
}

static void ballsaal(Gfx &g, float t) {
  g.verlauf(0, 180, FARBE(0xF6E2B4), FARBE(0xE8C483));
  for (int i = 0; i < 4; i++) {
    float x = 20 + i * 82;
    g.rechteck(x - 10, 30, 20, 160, FARBE(0xFFF3D6));
    g.rechteck(x - 14, 26, 28, 8, FARBE(0xE8D3A2));
    g.rechteck(x - 14, 184, 28, 8, FARBE(0xE8D3A2));
  }
  g.rechteck(0, 192, 284, 48, FARBE(0xC9A46A));
  for (int r = 0; r < 3; r++)
    for (int c = 0; c < 12; c++)
      if ((r + c) & 1) g.rechteck(c * 26 - r * 8, 192 + r * 16, 26, 16, FARBE(0xA8823E));
  g.linie(142, 0, 142, 26, 2, GOLD);
  g.ellipse(142, 34, 34, 7, GOLD);
  for (int i = 0; i < 5; i++) {
    float x = 114 + i * 14;
    g.rechteck(x - 2, 20, 4, 12, WEISS);
    g.ellipse(x + sinf(t * 9 + i) * 0.6f, 16, 2, 4, FARBE(0xFFB83D));
  }
  g.kreis(142, 30, 60, FARBE(0xFFF3C0), 40);
}

static void treppe(Gfx &g) {
  himmel(g, 1, 0, 200);
  for (int i = 0; i < 26; i++) g.stern(zufall(i + 700) * 284, zufall(i + 720) * 90, 2, 0.8f, 4, 0, WEISS, 200);
  schloss(g, 142, 120, 0, 1);
  for (int i = 0; i < 7; i++) {
    float y = 120 + i * 17, w = 120 + i * 24;
    g.rechteck(142 - w / 2, y, w, 17, FARBE(0xE8DCC8));
    g.rechteck(142 - w / 2, y, w, 3, FARBE(0xFFF6E8));
  }
}

static void landstrasse(Gfx &g, float t, float scroll) {
  himmel(g, 0.85f, t, 190);
  for (int i = 0; i < 20; i++) g.stern(zufall(i + 700) * 284, zufall(i + 720) * 90, 2, 0.8f, 4, 0, WEISS, 200);
  mond(g, 50, 36, 11);
  huegel(g, scroll * 0.3f, 0.5f, 190, 30, 90, FARBE(0x2E4A5A));
  schloss(g, 250 - scroll * 0.1f, 190, t, 1);
  boden(g, 190, FARBE(0x3E5A4A));
  g.rechteck(0, 214, 284, 14, FARBE(0x8E7E6A));
  for (int i = 0; i < 4; i++) tanne(g, fmodf(i * 90 - scroll + 900, 360) - 40, 200, 0.6f, FARBE(0x1F3B45));
}

static void tanz(Gfx &g, float cx, float u, const FrauLook &kleid) {
  float w = u * 1.6f;
  float pxp = cx + cosf(w) * 18, axp = cx - cosf(w) * 18;
  bool prinzVorn = sinf(w) > 0;
  FrauPose a = fp(kleid, u);
  a.drehen = sinf(u * 3) * 4;
  a.armV = 1.7f; a.mund = 0.4f;
  Pose p = mp(PRINZ, u);
  p.armV = 1.7f; p.mund = 0.3f;
  float hop = fabsf(sinf(u * 3.2f)) * 3;
  if (prinzVorn) { frau(g, axp, 214 - hop, 0.92f, axp < pxp ? 1 : -1, a); schneiderlein(g, pxp, 214 - hop, 0.95f, pxp < axp ? 1 : -1, p); }
  else { schneiderlein(g, pxp, 214 - hop, 0.95f, pxp < axp ? 1 : -1, p); frau(g, axp, 214 - hop, 0.92f, axp < pxp ? 1 : -1, a); }
}

// =====================================================================
// Szenen
// =====================================================================
// 1) In der Asche; die Einladung zum Ball; "Ihr spottet meiner"
static void szeneKueche(Gfx &g, float u) {
  kueche(g, u, 0);
  FrauPose c = fp(LUMPEN, u);
  c.armV = u < 10 ? 0.6f + fabsf(sinf(u * 4)) * 0.8f : 1.2f;  // fegt
  c.mund = u > 26.4f && u < 32 ? 0.5f : 0;
  float chx, chy;
  frau(g, 122, 214, 0.9f, 1, c, &chx, &chy);
  if (u < 10) g.linie(chx, chy, chx + 10, 214, 2.4f, HOLZ);  // Besen
  FrauPose st = fp(STIEF, u), s1 = fp(SCHW1, u), s2 = fp(SCHW2, u);
  st.boese = s1.boese = s2.boese = 0.6f;
  float k = sanft(seg(u, 9, 12));
  float sx = lerp(-60, 40, k);
  s1.mund = u > 12.6f && u < 17 ? 0.6f : u > 22 && u < 26 ? 0.5f : 0;
  s2.mund = u > 17.2f && u < 21.6f ? 0.6f : 0;
  s1.armV = u > 12.6f ? 1.8f : 0.6f;
  float hx, hy;
  frau(g, sx - 30, 214, 1, 1, st);
  frau(g, sx, 214, 0.95f, 1, s2);
  frau(g, sx + 32, 214, 0.95f, 1, s1, &hx, &hy);
  if (u > 12) {  // die Einladung des Koenigssohns
    g.rundRechteck(hx - 4, hy - 12, 16, 12, 2, WEISS);
    g.kreis(hx + 4, hy - 4, 2.4f, FARBE(0xC0392B));
  }
  blase(g, 100, 40, {"Der Königssohn gibt einen Ball!"}, aufzu(u, 12.6f, 17), sx + 32, 120);
  blase(g, 100, 40, {"Ich ziehe mein rotes Samtkleid an!"}, aufzu(u, 17.2f, 21.8f), sx, 120);
  blase(g, 110, 40, {"Cinderella, möchtest du", "nicht auch auf den Ball?"}, aufzu(u, 22, 26.2f), sx + 32, 120);
  blase(g, 170, 40, {"Ach, ihr spottet meiner,", "das ist nichts für mich."}, aufzu(u, 26.4f, 32), 130, 120);
  g.kameraAus();
  g.text(SCHRIFT_K, "Sie saß immer in der Asche am Herd.", 142, 232, WEISS, 1, (uint8_t)(255 * aufzu(u, 1, 8.6f)));
  blende(g, u, 34, 130, 150);
}

// 2) Die Patin: der Kuerbis, die Maeuse, die Ratte, die Eidechsen, das Kleid und die Glaspantoeffelchen
static void szenePatin(Gfx &g, float u) {
  if (u < 18) {
    kueche(g, u, 0.8f);
    FrauPose c = fp(LUMPEN, u);
    c.weint = u < 9;
    c.armV = u < 9 ? 2.2f : 1.4f;
    c.mund = u > 10 && u < 12.4f ? 0.6f : 0;
    frau(g, 116, 214, 0.9f, 1, c);
    if (u > 5) {
      FrauPose p = fp(PATIN, u);
      p.mund = (u > 7 && u < 10) || (u > 12.6f && u < 17) ? 0.5f : 0;
      p.armV = 1.7f;
      float hx, hy;
      frau(g, 200, 214, 0.95f, -1, p, &hx, &hy);
      zauberstab(g, hx, hy, u);
      puff(g, 200, 160, seg(u, 5, 6.2f), FARBE(0xFFB8E0));
    }
    blase(g, 180, 40, {"Du möchtest wohl gern", "auf den Ball, nicht wahr?"}, aufzu(u, 7, 10), 200, 130);
    blase(g, 110, 50, {"Ach ja!"}, aufzu(u, 10, 12.4f), 118, 130);
    blase(g, 180, 40, {"Geh in den Garten", "und hol mir einen Kürbis."}, aufzu(u, 12.6f, 17), 200, 130);
    g.kameraAus();
    g.text(SCHRIFT_K, "Ihre Patin war eine Fee.", 142, 232, WEISS, 1, (uint8_t)(255 * aufzu(u, 5.4f, 9)));
    if (u > 17) g.rechteck(0, 0, 284, 240, 0, (uint8_t)(255 * seg(u, 17, 18)));
  } else if (u < 46) {
    // im Garten: Kuerbis -> Kutsche, Maeuse -> Pferde, Ratte -> Kutscher, Eidechsen -> Diener
    float v = u - 18;
    garten(g, u);
    FrauPose p = fp(PATIN, u);
    p.armV = (v > 2 && v < 3.4f) || (v > 9 && v < 10.4f) || (v > 15 && v < 16.4f) || (v > 21 && v < 22.4f) ? 2.6f : 1.7f;
    float hx, hy;
    frau(g, 60, 214, 0.9f, 1, p, &hx, &hy);
    zauberstab(g, hx, hy, u, p.armV > 2 ? 1 : 0);
    FrauPose c = fp(LUMPEN, u);
    c.mund = v > 24 ? 0.6f : 0.3f;
    c.armV = v > 24 ? 2.2f : 0.8f;
    frau(g, 100, 214, 0.85f, 1, c);
    // Kuerbis -> Kutsche
    if (v < 3) kuerbis(g, 190, 214, 1);
    else kutsche(g, 190, 214, 0.9f, -1);
    puff(g, 190, 180, seg(v, 2.6f, 4));
    // sechs Maeuse -> Pferde (zwei sind zu sehen)
    if (v > 5) {
      if (v < 10) for (int i = 0; i < 3; i++) maus(g, 238 + i * 14, 214, 0.9f, -1, fmodf(u * 2 + i * 0.3f, 1));
      else { einhorn(g, 262, 214, 0.75f, -1, -1, 0, 0, true); }
      puff(g, 252, 190, seg(v, 9.6f, 11));
    }
    // die Ratte -> Kutscher
    if (v > 12) {
      if (v < 16) ratte(g, 150, 214, 0.9f, 1);
      else kutscher(g, 190, 150, 0.6f, 1, u);
      puff(g, 150, 196, seg(v, 15.6f, 17));
    }
    // sechs Eidechsen -> Diener
    if (v > 18) {
      if (v < 22) for (int i = 0; i < 2; i++) eidechse(g, 140 + i * 30, 228, 0.9f, u + i);
      else for (int i = 0; i < 2; i++) { Pose l = mp(LAKAI, u + i); schneiderlein(g, 150 + i * 22, 228, 0.55f, 1, l); }
      puff(g, 154, 210, seg(v, 21.6f, 23));
    }
    g.kameraAus();
    const char *was = v < 5 ? "Ein Kürbis wird eine goldene Kutsche ..." : v < 12 ? "sechs Mäuse werden sechs Pferde ..."
                    : v < 18 ? "die Ratte wird ein Kutscher ..." : "sechs Eidechsen werden sechs Diener.";
    float a0 = v < 5 ? 0.4f : v < 12 ? 5 : v < 18 ? 12 : 18;
    float a1 = v < 5 ? 5 : v < 12 ? 12 : v < 18 ? 18 : 27.6f;
    g.text(SCHRIFT_K, was, 142, 20, WEISS, 1, (uint8_t)(255 * aufzu(v, a0, a1)));
  } else {
    // das Kleid aus Gold und Silber und die glaesernen Pantoeffelchen; "Vor Mitternacht!"
    float v = u - 46;
    garten(g, u);
    kutsche(g, 190, 214, 0.9f, -1);
    einhorn(g, 262, 214, 0.75f, -1, -1, 0, 0, true);
    kutscher(g, 190, 150, 0.6f, 1, u);
    bool schoen = v > 2;
    FrauPose c = fp(schoen ? BALL : LUMPEN, u);
    c.drehen = schoen && v < 6 ? sinf(v * 6) * 5 : 0;
    c.mund = v > 2.4f && v < 6 ? 0.6f : 0;
    c.armV = v > 2.4f && v < 6 ? 2.2f : 1.0f;
    frau(g, 140, 214, 0.95f, 1, c);
    if (schoen && v < 7) { glasschuh(g, 134, 214, 0.6f); glitzern(g, 140, 170, 40, u, 8, (uint8_t)(255 * (1 - seg(v, 5, 7)))); }
    FrauPose p = fp(PATIN, u);
    p.armV = v < 2.2f ? 2.6f : 1.6f;
    p.mund = v > 7 && v < 14 ? 0.6f : 0;
    float hx, hy;
    frau(g, 60, 214, 0.9f, 1, p, &hx, &hy);
    zauberstab(g, hx, hy, u, v < 2.2f ? 1 : 0);
    puff(g, 140, 160, seg(v, 1.6f, 3));
    blase(g, 180, 40, {"Ach, wie schön!"}, aufzu(v, 2.6f, 6), 148, 120);
    blase(g, 110, 40, {"Bleib nicht länger als bis", "Mitternacht! Sonst wird alles", "wieder, wie es war."}, aufzu(v, 7, 14.6f), 66, 120);
  }
  blende(g, u, 62, 142, 150);
}

// 3) Fahrt zum Schloss
static void szeneFahrt(Gfx &g, float u) {
  float scroll = u * 60;
  landstrasse(g, u, scroll);
  float x = lerp(-60, 150, sanft(seg(u, 0, 5)));
  if (u > 13) x = lerp(150, 360, seg(u, 13, 17.6f));
  float lauf = fmodf(u * 1.6f, 1), hop = fabsf(sinf(u * 10)) * 2;
  kutsche(g, x - 40, 222 - hop, 0.85f, lauf);
  g.ausschnitt(x - 50, 222 - hop - 51, 20, 19);
  FrauPose c = fp(BALL, u);
  c.mund = 0.4f;
  frau(g, x - 40, 222 - hop - 2, 0.55f, 1, c);
  g.ausschnittAus();
  kutscher(g, x - 20, 176 - hop, 0.5f, 1, u);
  einhorn(g, x + 34, 222, 0.75f, lauf, 1, 0, 0, true);
  einhorn(g, x + 74, 222, 0.75f, fmodf(lauf + 0.5f, 1), 1, 0, 0, true);
  blende(g, u, 18, 142, 160);
}

// 4) Auf dem Ball: alle verstummen; der Koenigssohn tanzt nur mit ihr
static void szeneBall(Gfx &g, float u) {
  ballsaal(g, u);
  uhr(g, 142, 70, 20, lerp(0, 50, seg(u, 0, 32)));
  if (u < 12) {
    FrauPose c = fp(BALL, u);
    float cx = lerp(-30, 110, sanft(seg(u, 0, 4)));
    c.lauf = u < 4 ? fmodf(u * 1.4f, 1) : -1;
    frau(g, cx, 214, 0.92f, 1, c);
    Pose p = mp(PRINZ, u);
    p.armV = u > 6 ? 1.6f : 0.6f;
    p.mund = u > 4.4f && u < 8 ? 0.6f : 0;
    schneiderlein(g, 180, 214, 0.95f, -1, p);
    FrauPose s1 = fp(SCHW1, u), s2 = fp(SCHW2, u);
    s1.mund = s2.mund = u > 4 ? 0.7f : 0;
    frau(g, 236, 214, 0.85f, -1, s1);
    frau(g, 264, 214, 0.85f, -1, s2);
    blase(g, 190, 40, {"Ah, wie schön sie ist!"}, aufzu(u, 4.4f, 8.4f), 190, 130);
    g.kameraAus();
    g.text(SCHRIFT_K, "Die Musik hörte auf, alle sahen sie an.", 142, 232, OUT, 1, (uint8_t)(255 * aufzu(u, 1, 6)));
  } else {
    tanz(g, 142, u, BALL);
    FrauPose s1 = fp(SCHW1, u), s2 = fp(SCHW2, u);
    s1.boese = s2.boese = 0.5f;
    frau(g, 30, 214, 0.85f, 1, s1);
    frau(g, 256, 214, 0.85f, -1, s2);
    if (u > 14) for (int i = 0; i < 4; i++) {
        float k = fmodf((u - 14) * 0.5f + i / 4.0f, 1);
        herz(g, 142 + sinf(k * 7 + i) * 24, 140 - k * 80, 5 + (i % 3), FARBE(0xFF5A7A), (uint8_t)(255 * huepf(k)));
      }
    g.kameraAus();
    g.text(SCHRIFT_K, "Der Königssohn tanzte nur mit ihr.", 142, 232, OUT, 1, (uint8_t)(255 * aufzu(u, 13, 22)));
  }
  blende(g, u, 32, 142, 150);
}

// 5) Mitternacht! Die Flucht, der verlorene Glasschuh, und alles wird wieder, wie es war
static void szeneMitternacht(Gfx &g, float u) {
  if (u < 8) {
    ballsaal(g, u);
    uhr(g, 142, 70, 26, lerp(50, 60, seg(u, 0, 3)));
    tanz(g, 142, u + 32, BALL);
    g.kameraAus();
    if (u > 3) {
      int schlag = (int)((u - 3) / 0.4f) + 1;
      if (schlag > 12) schlag = 12;
      char b[16];
      snprintf(b, sizeof b, "%d!", schlag);
      g.text(SCHRIFT_G, b, 142, 140, FARBE(0xC0392B), 1);
    }
    if (u > 3 && u < 3.3f) g.wackeln(2, 0);
  } else if (u < 24) {
    float v = u - 8;
    treppe(g);
    FrauPose c = fp(BALL, u);
    c.lauf = fmodf(u * 2.2f, 1);
    c.armV = 1.2f;
    float k = seg(v, 0, 6);
    float cx = lerp(150, 40, k), cy = lerp(120, 238, k);
    frau(g, cx, cy, 0.7f, -1, c);
    if (v > 2.4f) {  // ein Pantoeffelchen bleibt auf der Treppe
      float sk = seg(v, 2.4f, 3);
      glasschuh(g, lerp(118, 112, sk), lerp(160, 171, sk), 0.8f);
      if (v > 9) glitzern(g, 114, 166, 14, u, 5);
    }
    Pose p = mp(PRINZ, u);
    float pk = seg(v, 2, 9);
    p.lauf = pk < 1 ? fmodf(u * 1.8f, 1) : -1;
    if (pk >= 1) { p.armV = 2.4f; p.mund = 0.5f; }
    float px = lerp(150, 126, pk), py = lerp(120, 172, pk);
    schneiderlein(g, px, py, 0.72f, -1, p);
    g.kameraAus();
    g.text(SCHRIFT_K, "Sie verlor ein Glaspantöffelchen.", 142, 20, WEISS, 1, (uint8_t)(255 * aufzu(v, 3, 10)));
    g.text(SCHRIFT_K, "Der Königssohn hob es sorgsam auf.", 142, 20, WEISS, 1, (uint8_t)(255 * aufzu(v, 10.4f, 15.6f)));
  } else {
    // auf der Landstrasse: alles wird wieder, wie es war
    float v = u - 24;
    landstrasse(g, u, 0);
    bool vorbei = v > 2;
    if (!vorbei) {
      kutsche(g, 150, 222, 0.85f, -1);
      einhorn(g, 224, 222, 0.75f, -1, 1, 0, 0, true);
    } else {
      float rollen = seg(v, 2, 7);
      kuerbis(g, 150 + rollen * 30, 228, 0.9f);
      for (int i = 0; i < 4; i++) maus(g, lerp(220, 340, seg(v, 2.5f + i * 0.3f, 7)) + i * 10, 228, 0.9f, 1, fmodf(u * 2 + i * 0.25f, 1));
      ratte(g, lerp(190, 340, seg(v, 3, 8)), 228, 0.9f, 1);
      eidechse(g, lerp(130, -40, seg(v, 2.6f, 8)), 234, 0.8f, u);
    }
    puff(g, 170, 190, seg(v, 1.6f, 3), WEISS);
    FrauPose c = fp(vorbei ? LUMPEN : BALL, u);
    c.mund = v > 8 && v < 12 ? 0.4f : 0;
    c.armV = v > 8 ? 1.6f : 0.6f;
    float hx, hy;
    frau(g, 90, 228, 0.9f, 1, c, &hx, &hy);
    if (v > 8) glasschuh(g, hx + 4, hy, 0.7f);
    g.kameraAus();
    g.text(SCHRIFT_K, "Nur das andere Pantöffelchen blieb ihr.", 142, 20, WEISS, 1, (uint8_t)(255 * aufzu(v, 8, 15)));
    if (v < 1) g.rechteck(0, 0, 284, 240, 0, (uint8_t)(255 * (1 - v)));
  }
  blende(g, u, 46, 142, 150);
}

// 6) Die Schuhprobe: zu klein fuer die Schwestern; "Lasst mich doch sehen, ob er mir passt!"
static void szeneProbe(Gfx &g, float u) {
  kueche(g, u, 0);
  Pose h = mp(HEROLD, u);
  h.armV = 1.6f;
  h.mund = u < 4.6f ? 0.6f : u > 30 && u < 33 ? 0.6f : 0;
  float hhx, hhy;
  float hx = lerp(330, 230, sanft(seg(u, 0, 2.4f)));
  schneiderlein(g, hx, 214, 0.95f, -1, h, &hhx, &hhy);
  // Kissen mit dem Glasschuh
  g.rundRechteck(hhx - 18, hhy - 4, 22, 6, 3, FARBE(0xC0392B));
  bool amFuss = (u > 6 && u < 12) || (u > 13 && u < 19) || u > 34;
  if (!amFuss) glasschuh(g, hhx - 7, hhy - 4, 0.6f);
  FrauPose s1 = fp(SCHW1, u), s2 = fp(SCHW2, u);
  // die Schwestern zwaengen und druecken - es geht nicht
  s1.boese = u > 6 && u < 12 ? 1 : 0.3f;
  s2.boese = u > 13 && u < 19 ? 1 : 0.3f;
  s1.mund = u > 9 && u < 12 ? 0.7f : 0;
  s2.mund = u > 16 && u < 19 ? 0.7f : u > 21 && u < 25 ? 0.6f : 0;
  float s1x = u > 6 && u < 12 ? 176 : 30, s2x = u > 13 && u < 19 ? 176 : 60;
  if (u > 34) { s1.mund = s2.mund = 0.9f; s1.armV = s2.armV = 2.6f; }
  frau(g, s1x, 214, 0.95f, 1, s1);
  frau(g, s2x, 214, 0.95f, 1, s2);
  if ((u > 6 && u < 12) || (u > 13 && u < 19)) {
    float sx = u < 12 ? s1x : s2x;
    glasschuh(g, sx + 10, 214, 0.65f + fabsf(sinf(u * 9)) * 0.03f);
    for (int i = 0; i < 3; i++) g.stern(sx + 12 + i * 6, 196 - i * 4, 3, 1.2f, 4, u * 4, WEISS, (uint8_t)(200 * fabsf(sinf(u * 6 + i))));
  }
  FrauPose c = fp(u > 42 ? BALL : LUMPEN, u);
  c.mund = u > 20.8f && u < 24.6f ? 0.6f : u > 36 && u < 40 ? 0.6f : 0;
  c.armV = u > 36 && u < 40 ? 1.8f : 0.8f;
  float cx = u > 26 ? lerp(124, 176, seg(u, 26, 28)) : 124;
  float chx, chy;
  frau(g, cx, 214, 0.9f, 1, c, &chx, &chy);
  if (u > 34) glasschuh(g, cx + 10, 214, 0.65f);
  if (u > 36 && u < 42) glasschuh(g, chx + 4, chy, 0.65f);  // zieht das andere aus der Tasche
  if (u > 34 && u < 37) glitzern(g, cx + 10, 206, 14, u, 6);
  if (u > 40) {
    FrauPose p = fp(PATIN, u);
    p.armV = 2.6f;
    float px, py;
    frau(g, 100, 214, 0.9f, 1, p, &px, &py);
    zauberstab(g, px, py, u, 1);
    puff(g, 100, 160, seg(u, 40, 41.2f), FARBE(0xFFB8E0));
    puff(g, cx, 160, seg(u, 41.6f, 43));
  }
  blase(g, 200, 40, {"Wem das Pantöffelchen passt,", "die wird die Braut des Prinzen!"}, aufzu(u, 0.6f, 4.8f), hx, 130);
  blase(g, 120, 50, {"Es ist zu klein!"}, aufzu(u, 9, 12), s1x, 130);
  blase(g, 120, 50, {"Es ist zu klein!"}, aufzu(u, 16, 19), s2x, 130);
  blase(g, 100, 40, {"Lasst mich doch sehen,", "ob es mir nicht passt!"}, aufzu(u, 20.8f, 24.6f), 124, 130);
  blase(g, 60, 40, {"Ha, ha, ha!"}, aufzu(u, 21.6f, 25), s2x, 130, FARBE(0xFFE8C0));
  blase(g, 200, 40, {"Jedes Mädchen soll es probieren."}, aufzu(u, 30, 33.4f), hx, 130);
  blase(g, 110, 40, {"Es passt wie angegossen!"}, aufzu(u, 34.4f, 36), hx, 130);
  blase(g, 120, 40, {"Hier ist das andere!"}, aufzu(u, 36.4f, 40), cx, 130);
  g.kameraAus();
  if (u > 50) g.text(SCHRIFT_K, "Die Schwestern baten sie um Verzeihung.", 142, 20, OUT, 1, (uint8_t)(255 * aufzu(u, 50, 64)));
  blende(g, u, 64, 142, 150);
}

// 7) Sie verzeiht den Schwestern von Herzen; Hochzeit im Schloss
static void szeneHochzeit(Gfx &g, float u) {
  himmel(g, 0, u, 196);
  sonne(g, 244, 34, 12, u);
  schloss(g, 142, 196, u, 0);
  boden(g, 196);
  FrauPose b = fp(BRAUT, u);
  b.mund = 0.4f;
  frau(g, 124, 214, 0.92f, 1, b);
  Pose p = mp(PRINZ, u);
  p.mund = 0.4f;
  p.armV = 2.5f + sinf(u * 8) * 0.3f;
  schneiderlein(g, 160, 214, 0.95f, -1, p);
  FrauPose s1 = fp(SCHW1, u), s2 = fp(SCHW2, u), pt = fp(PATIN, u);
  s1.mund = s2.mund = 0.4f;
  frau(g, 40, 214, 0.85f, 1, s1);
  frau(g, 72, 214, 0.85f, 1, s2);
  float phx, phy;
  pt.armV = 2.2f;
  frau(g, 232, 214, 0.85f, -1, pt, &phx, &phy);
  zauberstab(g, phx, phy, u, 0.5f);
  for (int i = 0; i < 6; i++) {
    float k = fmodf(u * 0.45f + i / 6.0f, 1);
    herz(g, 142 + sinf(k * 7 + i) * 40, 150 - k * 110, 5 + (i % 3), FARBE(0xFF5A7A), (uint8_t)(255 * huepf(k)));
  }
  g.kameraAus();
  if (u < 1) g.rechteck(0, 0, 284, 240, 0, (uint8_t)(255 * (1 - u)));
  g.text(SCHRIFT_K, "Cinderella verzieh ihnen von Herzen.", 142, 20, OUT, 1, (uint8_t)(255 * aufzu(u, 1, 9)));
  g.text(SCHRIFT_K, "Sie war so gut, wie sie schön war.", 142, 20, OUT, 1, (uint8_t)(255 * aufzu(u, 9.4f, 20)));
  if (u > 30.5f) g.iris(142, 170, lerp(330, 0, sanft(seg(u, 30.5f, 32.5f))));
  if (u > 32.5f) {
    uint8_t a = (uint8_t)(255 * sanft(seg(u, 32.5f, 33.5f)));
    g.text(SCHRIFT_G, "Ende", 142, 122, GOLD, 1, a);
    for (int i = 0; i < 6; i++) {
      float an = u * 1.2f + i * 1.047f;
      g.stern(142 + cosf(an) * 60, 112 + sinf(an) * 30, 5, 2, 5, an, GOLD, a);
    }
  }
}

static void bild(Gfx &g, float t) {
  if (t < P2) szeneKueche(g, t - P1);
  else if (t < P3) szenePatin(g, t - P2);
  else if (t < P4) szeneFahrt(g, t - P3);
  else if (t < P5) szeneBall(g, t - P4);
  else if (t < P6) szeneMitternacht(g, t - P5);
  else if (t < P7) szeneProbe(g, t - P6);
  else szeneHochzeit(g, t - P7);
}

// =====================================================================
// Musik
// =====================================================================
// Walzer (G-Dur, 3/4)
static const Ton3 MEL_WALZER[] = {
  {74, 1, 43}, {79, 1, 0}, {78, 1, 0}, {76, 2, 48}, {74, 1, 0}, {72, 1, 45}, {71, 1, 0}, {69, 1, 0}, {71, 3, 43},
  {74, 1, 43}, {81, 1, 0}, {79, 1, 0}, {78, 2, 45}, {76, 1, 0}, {74, 1, 50}, {78, 1, 0}, {76, 1, 0}, {79, 3, 43}};
// Cinderellas Lied (e-Moll, still)
static const Ton3 MEL_ASCHE[] = {
  {76, 1, 52}, {79, 1, 0}, {78, .5, 0}, {76, .5, 0}, {75, 1, 47}, {76, 1, 0}, {71, 2, 0},
  {72, 1, 48}, {76, 1, 0}, {74, .5, 0}, {72, .5, 0}, {71, 1, 47}, {69, 1, 0}, {71, 2, 52}};

static void zauber(Synth &s, float x) { for (int k = 0; k < 10; k++) s.glocke(x + k * 0.07f, 72 + (k % 5) * 3 + k, 0.45f); }

static void ton(Synth &s, float t0, float t1) {
  float a = P1;
  spieleMelodie(s, M(MEL_ASCHE), a + 0.5f, a + 12, 0.4f, t0, t1, 0.75f);
  for (float x = ceilf(t0 / 0.8f) * 0.8f; x < t1; x += 0.8f)
    if (x > a + 1 && x < a + 10) s.rauschen(x, 0.12f, BANDPASS, 1800, 0.05f);  // fegen
  if (bei(a + 12.4f, t0, t1)) { s.note(a + 12.4f, 72, 0.6f); s.note(a + 12.6f, 76, 0.6f); s.note(a + 12.8f, 79, 0.6f); s.note(a + 13.2f, 84, 0.7f); }  // Fanfare
  spieleMelodie(s, M(MEL_WALZER), a + 13.5f, a + 26, 0.22f, t0, t1, 0.8f);
  spieleMelodie(s, M(MEL_ASCHE), a + 26.4f, a + 33.5f, 0.4f, t0, t1, 0.75f);
  a = P2;
  spieleMelodie(s, M(MEL_ASCHE), a + 0.4f, a + 5, 0.45f, t0, t1, 0.7f);
  if (bei(a + 5, t0, t1)) zauber(s, a + 5);
  spieleMelodie(s, M(MEL_WIEGE), a + 6.4f, a + 17.5f, 0.36f, t0, t1, 0.75f);
  float zt[] = {a + 20.6f, a + 27.6f, a + 33.6f, a + 39.6f, a + 47.6f};
  for (float x : zt) if (bei(x, t0, t1)) zauber(s, x);
  for (float x = ceilf(t0 / 0.15f) * 0.15f; x < t1; x += 0.15f)
    if (x > a + 23 && x < a + 28) s.note(x, 96 + ((int)(x * 7) % 3) * 2, 0.12f);  // Maeuse piepen
  spieleMelodie(s, M(MEL_WIEGE), a + 22, a + 46, 0.3f, t0, t1, 0.6f);
  spieleMelodie(s, M(MEL_WALZER), a + 49, a + 61.5f, 0.24f, t0, t1, 0.8f);
  a = P3;
  spieleMelodie(s, M(MEL_WALZER), a + 0.2f, a + 17.5f, 0.2f, t0, t1, 0.8f);
  for (float x = ceilf(t0 / 0.227f) * 0.227f; x < t1; x += 0.227f)
    if (x > a && x < a + 17.6f) s.note(x, 79, 0.15f);  // Hufe
  a = P4;
  if (bei(a + 4.4f, t0, t1)) for (int k = 0; k < 8; k++) s.glocke(a + 4.4f + k * 0.15f, 76 + (k % 4) * 3, 0.4f);
  spieleMelodie(s, M(MEL_WALZER), a + 9, a + 31.5f, 0.22f, t0, t1, 0.85f);
  a = P5;
  spieleMelodie(s, M(MEL_WALZER), a, a + 3, 0.22f, t0, t1, 0.8f);
  for (int i = 0; i < 12; i++) if (bei(a + 3 + i * 0.4f, t0, t1)) s.ton(a + 3 + i * 0.4f, 220, 0.6f, 0.002f, 0.5f, SINUS, GERAEUSCH, 110);  // zwoelf Schlaege
  spieleMelodie(s, M(MEL_JAGD), a + 8, a + 16, 0.18f, t0, t1, 0.8f);
  if (bei(a + 10.6f, t0, t1)) for (int k = 0; k < 4; k++) s.glocke(a + 10.6f + k * 0.08f, 90 + k * 2, 0.4f);  // der Schuh
  spieleMelodie(s, M(MEL_ASCHE), a + 17, a + 23.5f, 0.4f, t0, t1, 0.7f);
  if (bei(a + 25.6f, t0, t1)) for (int k = 0; k < 10; k++) s.glocke(a + 25.6f + k * 0.07f, 90 - k * 3, 0.45f);  // alles wie es war
  for (float x = ceilf(t0 / 0.15f) * 0.15f; x < t1; x += 0.15f)
    if (x > a + 26 && x < a + 31) s.note(x, 96 + ((int)(x * 7) % 3) * 2, 0.12f);
  spieleMelodie(s, M(MEL_ASCHE), a + 32, a + 45.5f, 0.42f, t0, t1, 0.75f);
  a = P6;
  if (bei(a + 0.4f, t0, t1)) { s.note(a + 0.4f, 72, 0.6f); s.note(a + 0.6f, 76, 0.6f); s.note(a + 0.8f, 79, 0.6f); s.note(a + 1.2f, 84, 0.7f); }
  spieleMelodie(s, M(MEL_KEGEL), a + 6, a + 19, 0.3f, t0, t1, 0.7f);
  for (float x : {a + 9.2f, a + 16.2f}) if (bei(x, t0, t1)) for (int k = 0; k < 4; k++) s.note(x + k * 0.12f, 60 - k * 2, 0.6f);
  for (float x : {a + 21.8f, a + 22.2f, a + 22.6f}) if (bei(x, t0, t1)) s.note(x, 88, 0.3f);  // ha, ha, ha
  spieleMelodie(s, M(MEL_ASCHE), a + 25, a + 33.5f, 0.36f, t0, t1, 0.75f);
  if (bei(a + 34.2f, t0, t1)) for (int k = 0; k < 12; k++) s.glocke(a + 34.2f + k * 0.15f, 72 + (k % 6) * 2, 0.5f);  // passt
  if (bei(a + 40, t0, t1)) zauber(s, a + 40);
  if (bei(a + 41.6f, t0, t1)) zauber(s, a + 41.6f);
  spieleMelodie(s, M(MEL_WALZER), a + 43, a + 63.5f, 0.24f, t0, t1, 0.8f);
  a = P7;
  spieleMelodie(s, M(MEL_F), a + 0.4f, a + 32.5f, 0.26f, t0, t1, 0.9f);
  for (int i = 0; i < 10; i++) if (bei(a + 14 + i * 0.6f, t0, t1)) s.glocke(a + 14 + i * 0.6f, i & 1 ? 79 : 72 + (i % 4 == 2) * 4, 0.8f);
  if (bei(a + 32.6f, t0, t1)) { s.glocke(a + 32.6f, 72); s.glocke(a + 32.9f, 76); s.glocke(a + 33.2f, 79); s.glocke(a + 33.6f, 84); }
}

// =====================================================================
// Startbild und Vorschau
// =====================================================================
static void titelSzene(Gfx &g, float t) {
  treppe(g);
  FrauPose c = fp(BALL, t);
  c.drehen = sinf(t * 2) * 3;
  c.armV = 1.4f;
  frau(g, 168, 232, 1, -1, c);
  glasschuh(g, 120, 214, 1.1f);
  glitzern(g, 120, 206, 18, t, 5);
  uhr(g, 240, 70, 18, 58 + sinf(t) * 1.5f);
}

static void titelbild(Gfx &g, float t) {
  titelSzene(g, t);
  g.rundRechteck(8, 20, 170, 50, 14, FARBE(0x1E1A3A), 160);
  g.text(SCHRIFT_G, "Cinderella", 93, 55, GOLD, 1);
  float b = 1 + sinf(t * 4) * 0.08f;
  g.kontur(2, OUT);
  g.kreis(93, 96, 14 * b, FARBE(0xE8473D));
  g.konturAus();
  float tri[] = {88, 88, 88, 104, 101, 96};
  g.polygon(tri, 3, WEISS);
}

static void vorschau(Gfx &g, float t) { titelSzene(g, t); }

static void symbol(Gfx &g, float x, float y, float t) {
  kuerbis(g, x + 34, y, 0.8f);
  glasschuh(g, x - 16, y - 8 - fabsf(sinf(t * 2)) * 4, 2.6f);
  glitzern(g, x - 16, y - 20, 26, t, 5, 220);
}

const Maerchen CINDERELLA = {"Cinderella", PDAUER, FARBE(0xBFE6FF), bild, ton, titelbild, vorschau, symbol, LIED(MEL_WALZER, 0.3f), "Perrault"};

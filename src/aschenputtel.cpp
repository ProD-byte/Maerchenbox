// Aschenputtel - ein Spieluhr-Film fuer die Maerchenbox, nach dem Maerchen der Brueder Grimm.
// Haselbaum auf dem Grab, Linsen in der Asche, drei Ballnaechte, die Schuhprobe mit den Tauben
// ("Rucke di guh, Blut ist im Schuh") und die Strafe der Schwestern. Gezeichnet im Spieluhr-Stil.
#include "maerchen.h"
#include "schriften.h"
#include "figuren.h"
#include "melodien.h"
#include <stdio.h>
#include <initializer_list>

// ---------- Ablauf (Sekunden) ----------
static const float A1 = 0, A2 = 28, A3 = 54, A4 = 84, A5 = 116, A6 = 140, A7 = 182, A8 = 206, A9 = 262, ADAUER = 300;

// ---------- Aussehen ----------
static FrauLook look(Farbe kleid, Farbe haar, int frisur) {
  FrauLook l;
  l.kleid = kleid; l.haar = haar; l.frisur = frisur;
  return l;
}
static FrauLook tochterLook() { FrauLook l = look(FARBE(0x7EB6E8), FARBE(0x7A4524), 0); l.blume = FARBE(0xFF7FA8); return l; }
static FrauLook ascheLook() {
  FrauLook l = look(FARBE(0x8E8676), FARBE(0x7A4524), 3);
  l.tuch = FARBE(0x6E6252); l.flicken = true; l.schuh = FARBE(0xA8652F); l.schuerze = FARBE(0xB5AD9C);
  return l;
}
static FrauLook silberLook() {
  FrauLook l = look(FARBE(0xD9DEEA), FARBE(0x7A4524), 2);
  l.glanz = true; l.rockBreite = 1.45f; l.schuh = FARBE(0xC7D0DC); l.blume = FARBE(0xB28DFF);
  return l;
}
static FrauLook goldLook() {
  FrauLook l = look(FARBE(0xF2C94E), FARBE(0x7A4524), 2);
  l.glanz = true; l.rockBreite = 1.5f; l.schuh = GOLD; l.blume = FARBE(0xFF5A7A);
  return l;
}
static FrauLook rosaLook() {
  FrauLook l = look(FARBE(0xF6B8D2), FARBE(0x7A4524), 2);
  l.glanz = true; l.rockBreite = 1.45f; l.schuh = WEISS; l.blume = WEISS;
  return l;
}
static FrauLook brautLook() {
  FrauLook l = look(FARBE(0xFFFFFF), FARBE(0x7A4524), 2);
  l.glanz = true; l.rockBreite = 1.5f; l.krone = true; l.schuh = GOLD;
  return l;
}
static FrauLook mutterLook() { FrauLook l = look(FARBE(0xE8D6F0), FARBE(0x7A4524), 0); l.haut = FARBE(0xF2DCCB); l.wangen = false; return l; }
static FrauLook stiefLook() { FrauLook l = look(FARBE(0x6B3A7A), FARBE(0x9A9A9A), 2); l.wangen = false; return l; }
static FrauLook schw1Look() { return look(FARBE(0xF28AB8), FARBE(0x2A2018), 0); }
static FrauLook schw2Look() { return look(FARBE(0xF2B84A), FARBE(0xC0632E), 1); }
static const FrauLook ROSA = rosaLook();
static const FrauLook TOCHTER = tochterLook(), ASCHE = ascheLook(), SILBER = silberLook(), GOLDKLEID = goldLook(),
                      BRAUT = brautLook(), MUTTER = mutterLook(), STIEF = stiefLook(), SCHW1 = schw1Look(),
                      SCHW2 = schw2Look();

static Aussehen vaterLook() {
  Aussehen a;
  a.weste = FARBE(0x6E5A3A); a.hose = FARBE(0x4A3B2A); a.hut = FARBE(0x5A4A3A); a.hutD = FARBE(0x3A2E22);
  a.feder = FARBE(0x3E7FDC); a.haar = FARBE(0x5A3A26);
  return a;
}
static Aussehen prinzLook() {
  Aussehen a;
  a.weste = FARBE(0x3E5FC8); a.hemd = FARBE(0xFFFFFF); a.hose = FARBE(0x2E3A6E); a.haar = FARBE(0x5A3A26);
  a.muetze = 3; a.halstuch = GOLD;
  return a;
}
static const Aussehen VATER = vaterLook(), PRINZ = prinzLook();

static FrauPose fp(const FrauLook &l, float t) {
  FrauPose p;
  p.look = &l;
  p.zeit = t;
  p.zu = fmodf(t + l.kleid * 0.001f, 3.9f) < 0.13f;
  return p;
}
static Pose mp(const Aussehen &a, float t) {
  Pose p;
  p.aussehen = &a;
  p.zu = fmodf(t + 0.7f, 4.1f) < 0.13f;
  return p;
}

// =====================================================================
// Hilfen
// =====================================================================
static float aufzu(float u, float a, float b) { return federnd(seg(u, a, a + 0.4f)) * (1 - seg(u, b - 0.3f, b)); }

// Sprechblase mit einer oder mehreren Zeilen (Bildschirmkoordinaten)
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

static void puff(Gfx &g, float x, float y, float k) {  // Verwandlung
  if (k <= 0 || k >= 1) return;
  for (int i = 0; i < 10; i++) {
    float a = i * 0.628f;
    g.stern(x + cosf(a) * k * 46, y + sinf(a) * k * 40, 6 * (1 - k) + 2, 2, 5, a + k * 4, i & 1 ? GOLD : WEISS,
            (uint8_t)(255 * (1 - k)));
  }
  g.kreis(x, y, 30 * (1 - k), WEISS, (uint8_t)(180 * (1 - k)));
}

static void schuh(Gfx &g, float x, float y, float s, Farbe c, bool blut = false) {
  g.kontur(1.2f, OUT);
  float q[] = {x - 9 * s, y, x + 9 * s, y, x + 11 * s, y - 3 * s, x + 2 * s, y - 6 * s, x - 7 * s, y - 8 * s, x - 10 * s, y - 4 * s};
  g.polygon(q, 6, c);
  g.konturAus();
  g.rechteck(x - 9 * s, y - 2 * s, 3 * s, 4 * s, c);  // Absatz
  g.stern(x + 3 * s, y - 4 * s, 2.5f * s, 1 * s, 4, 0, WEISS, 220);
  if (blut) {
    g.ellipse(x - 4 * s, y - 6 * s, 3.5f * s, 2 * s, FARBE(0xC01830));
    g.kreis(x - 4 * s, y + 2 * s, 1.5f * s, FARBE(0xC01830));
  }
}

static void schuessel(Gfx &g, float x, float y, float voll) {
  g.kontur(1.3f, OUT);
  float q[] = {x - 18, y - 12, x + 18, y - 12, x + 12, y, x - 12, y};
  g.polygon(q, 4, FARBE(0xE8D0A0));
  g.konturAus();
  if (voll > 0) g.ellipse(x, y - 12, 16 * voll, 3 * voll, FARBE(0x8E6E3A));
  for (int i = 0; i < (int)(voll * 9); i++) g.kreis(x - 12 + i * 3, y - 13 - (i & 1), 1.2f, FARBE(0xB08850));
}

// Reiter auf dem Pferd: zuerst die Reiter, dann das Pferd (verdeckt die Beine)
static void ritt(Gfx &g, float x, float y, float t, float dir, const FrauLook *braut, bool laeuft) {
  float lauf = laeuft ? fmodf(t * 1.6f, 1) : -1;
  float hop = laeuft ? fabsf(sinf(t * 10)) * 2 : 0;
  if (braut) {
    FrauPose b = fp(*braut, t);
    b.armV = 1.3f;
    frau(g, x - 14 * dir, y - 44 - hop, 0.7f, dir, b);
  }
  Pose p = mp(PRINZ, t);
  p.armV = 1.4f;
  schneiderlein(g, x + 6 * dir, y - 44 - hop, 0.72f, dir, p);
  einhorn(g, x, y, 0.95f, lauf, dir, 0, 0, true);
}

// =====================================================================
// Kulissen
// =====================================================================
static void sterbebett(Gfx &g, float x, float y) {
  g.kontur(1.6f, OUT);
  g.rundRechteck(x - 70, y - 64, 12, 64, 3, FARBE(0x8E5426));
  g.rundRechteck(x - 64, y - 30, 128, 14, 3, FARBE(0xA8652F));
  g.konturAus();
  g.ellipse(x - 40, y - 36, 18, 8, WEISS);
  g.kontur(1.4f, OUT);
  g.rundRechteck(x - 50, y - 32, 112, 20, 6, FARBE(0xC9B8E8));
  g.konturAus();
  for (int i = 0; i < 5; i++) g.kreis(x - 36 + i * 20, y - 22, 2.4f, WEISS);
  g.rundRechteck(x + 58, y - 44, 10, 44, 3, FARBE(0x8E5426));
}
static void zimmer(Gfx &g, float t, float abend) {
  g.verlauf(0, 190, mischeFarbe(FARBE(0xE8DCF2), FARBE(0x7E6E9A), abend), mischeFarbe(FARBE(0xCDBDE0), FARBE(0x5E4E7E), abend));
  g.rechteck(0, 190, 284, 50, FARBE(0x9E7656));
  for (int i = 0; i < 4; i++) g.rechteck(0, 200 + i * 12, 284, 1.5f, FARBE(0x7E5636));
  g.kontur(2, OUT);
  g.rundRechteck(200, 40, 64, 72, 30, HOLZ);
  g.konturAus();
  g.rundRechteck(206, 46, 52, 62, 24, mischeFarbe(FARBE(0xBFE6FF), FARBE(0x34306E), abend));
  if (abend > 0.5f) mond(g, 232, 70, 8);
  g.rechteck(230, 46, 3, 62, HOLZ);
}

// Hof mit dem Grab der Mutter und dem Haselbaum (groesse 0..1), Schnee 0..1
static void grabHof(Gfx &g, float t, float baum, float schnee, float nacht, float rascheln = 0) {
  himmel(g, nacht, t, 196);
  if (nacht < 0.4f) sonne(g, 244, 36, 12, t);
  else mond(g, 244, 36, 11);
  huegel(g, 0, 0, 186, 30, 150, mischeFarbe(FARBE(0xA7DE85), FARBE(0x4E5A70), nacht));
  boden(g, 196, mischeFarbe(mischeFarbe(GRAS, WEISS, schnee), FARBE(0x3E5A4A), nacht * 0.7f));
  // Haus im Hintergrund
  g.kontur(1.6f, OUT);
  g.rechteck(-10, 100, 90, 96, FARBE(0xF1DDB0));
  float d[] = {-20, 102, 35, 60, 90, 102};
  g.polygon(d, 3, FARBE(0x9A5A3A));
  g.konturAus();
  g.rundRechteck(30, 150, 22, 46, 10, HOLZ);
  g.rundRechteck(56, 122, 16, 16, 2, mischeFarbe(FARBE(0x8FD3FF), FARBE(0xFFD24A), nacht));
  // Grab
  g.kontur(1.4f, OUT);
  g.ellipse(176, 206, 34, 8, mischeFarbe(FARBE(0x8E6E4E), WEISS, schnee));
  g.rundRechteck(164, 168, 24, 36, 10, mischeFarbe(STEIN, WEISS, schnee * 0.6f));
  g.konturAus();
  g.linie(176, 176, 176, 194, 2.5f, FARBE(0x7D8392));
  g.linie(169, 182, 183, 182, 2.5f, FARBE(0x7D8392));
  if (schnee < 0.5f) for (int i = 0; i < 4; i++) g.kreis(150 + i * 16, 206 + (i & 1) * 2, 2.5f, i & 1 ? FARBE(0xFF7FA8) : GOLD);
  // Haselbaum
  if (baum > 0.02f) {
    float wack = rascheln * sinf(t * 30) * 3;
    float bx = 216 + wack, by = 210;
    laubbaum(g, bx, by, 0.25f + 0.9f * baum, FARBE(0x5CB85A));
    if (baum > 0.6f) for (int i = 0; i < 6; i++) g.kreis(bx - 22 + zufall(i + 70) * 44, by - 70 * baum - zufall(i + 80) * 30 * baum, 2.2f, FARBE(0xA8652F));
  }
}

static void kueche(Gfx &g, float t, float nacht, float linsen = 0) {
  g.verlauf(0, 200, mischeFarbe(FARBE(0xD9B48A), FARBE(0x5E4A3A), nacht), mischeFarbe(FARBE(0xB88E64), FARBE(0x3E3028), nacht));
  g.rechteck(0, 200, 284, 40, FARBE(0x7E6A56));
  // Herd mit Feuer und Asche
  g.kontur(2, OUT);
  g.rundRechteck(150, 110, 110, 92, 6, FARBE(0xA89888));
  g.konturAus();
  g.rechteck(166, 30, 78, 82, FARBE(0x988878));
  g.rundRechteck(168, 140, 74, 60, 24, FARBE(0x2E2620));
  for (int i = 0; i < 4; i++) {
    float fx = 182 + i * 14, h = 18 + 6 * sinf(t * 9 + i * 1.7f);
    float f[] = {fx - 7, 196, fx + 7, 196, fx + sinf(t * 7 + i) * 2, 196 - h};
    g.polygon(f, 3, FARBE(0xFF8A2A));
  }
  g.ellipse(120, 214, 40, 8, FARBE(0x9A9490));  // Aschehaufen vor dem Herd
  g.ellipse(120, 211, 26, 5, FARBE(0xB5B0AC));
  for (int i = 0; i < (int)(linsen * 40); i++) g.kreis(88 + zufall(i + 300) * 64, 208 + zufall(i + 340) * 10, 1.3f, FARBE(0xB08850));
  // Regal mit Toepfen
  g.rechteck(20, 70, 90, 5, HOLZ_D);
  for (int i = 0; i < 4; i++) {
    g.kontur(1.2f, OUT);
    g.rundRechteck(26 + i * 22, 52, 16, 18, 4, i & 1 ? FARBE(0xC0632E) : FARBE(0x9AA6B8));
    g.konturAus();
  }
  if (nacht > 0) g.kreis(205, 170, 70, FARBE(0xFFB050), (uint8_t)(30 * nacht));
  // Fenster (fuer die Tauben)
  g.kontur(1.8f, OUT);
  g.rundRechteck(30, 100, 50, 46, 4, HOLZ);
  g.konturAus();
  g.rundRechteck(34, 104, 42, 38, 2, mischeFarbe(FARBE(0xBFE6FF), FARBE(0x34306E), nacht));
}

static void ballsaal(Gfx &g, float t) {
  g.verlauf(0, 180, FARBE(0xF6E2B4), FARBE(0xE8C483));
  for (int i = 0; i < 4; i++) {  // Saeulen
    float x = 20 + i * 82;
    g.rechteck(x - 10, 30, 20, 160, FARBE(0xFFF3D6));
    g.rechteck(x - 14, 26, 28, 8, FARBE(0xE8D3A2));
    g.rechteck(x - 14, 184, 28, 8, FARBE(0xE8D3A2));
    g.rechteck(x - 3, 34, 3, 150, FARBE(0xF0E2C0));
  }
  // Fussboden mit Schachbrett
  g.rechteck(0, 192, 284, 48, FARBE(0xC9A46A));
  for (int r = 0; r < 3; r++)
    for (int c = 0; c < 12; c++)
      if ((r + c) & 1) g.rechteck(c * 26 - r * 8, 192 + r * 16, 26, 16, FARBE(0xA8823E));
  // Kronleuchter
  g.linie(142, 0, 142, 26, 2, GOLD);
  g.ellipse(142, 34, 34, 7, GOLD);
  for (int i = 0; i < 5; i++) {
    float x = 114 + i * 14;
    g.rechteck(x - 2, 20, 4, 12, WEISS);
    g.ellipse(x + sinf(t * 9 + i) * 0.6f, 16, 2, 4, FARBE(0xFFB83D));
  }
  g.kreis(142, 30, 60, FARBE(0xFFF3C0), 40);
}

static void taubenhaus(Gfx &g, float x, float y, float kipp) {
  float c = cosf(kipp), s = sinf(kipp);
  auto R = [&](float dx, float dy, float *o) { o[0] = x + dx * c - dy * s; o[1] = y + dx * s + dy * c; };
  float q[8], a[2], b[2];
  R(-3, 0, a); R(-3, -60, b);
  g.linie(a[0], a[1], b[0], b[1], 6, HOLZ_D);
  R(-20, -60, q); R(20, -60, q + 2); R(20, -90, q + 4); R(-20, -90, q + 6);
  g.kontur(1.5f, OUT);
  g.polygon(q, 4, FARBE(0xF1DDB0));
  float d[6];
  R(-26, -88, d); R(0, -108, d + 2); R(26, -88, d + 4);
  g.polygon(d, 3, FARBE(0x9A5A3A));
  g.konturAus();
  R(-8, -72, a); g.kreis(a[0], a[1], 5, FARBE(0x3A2A20));
  R(8, -72, a); g.kreis(a[0], a[1], 5, FARBE(0x3A2A20));
}

static void birnbaum(Gfx &g, float x, float y, float kipp, float t) {
  float c = cosf(kipp), s = sinf(kipp);
  auto R = [&](float dx, float dy, float *o) { o[0] = x + dx * c - dy * s; o[1] = y + dx * s + dy * c; };
  float a[2], b[2];
  R(0, 0, a); R(0, -60, b);
  g.kontur(1.6f, OUT);
  g.linie(a[0], a[1], b[0], b[1], 12, FARBE(0x8E5B3A));
  for (int i = 0; i < 3; i++) { R(-22 + i * 22, -78 - (i == 1) * 10, a); g.kreis(a[0], a[1], 24, FARBE(0x4CB85A)); }
  g.konturAus();
  for (int i = 0; i < 6; i++) {
    R(-30 + zufall(i + 600) * 60, -60 - zufall(i + 610) * 36, a);
    g.ellipse(a[0], a[1], 3.5f, 4.5f, FARBE(0xD8E04A));
  }
}

static void treppe(Gfx &g, float pech) {
  himmel(g, 1, 0, 200);
  for (int i = 0; i < 26; i++) g.stern(zufall(i + 700) * 284, zufall(i + 720) * 90, 2, 0.8f, 4, 0, WEISS, 200);
  schloss(g, 142, 120, 0, 1);
  for (int i = 0; i < 7; i++) {
    float y = 120 + i * 17, w = 120 + i * 24;
    g.rechteck(142 - w / 2, y, w, 17, FARBE(0xE8DCC8));
    g.rechteck(142 - w / 2, y, w, 3, FARBE(0xFFF6E8));
    if (pech > 0 && (i == 3 || i == 5)) g.ellipse(142 + (i == 3 ? -20 : 18), y + 9, 18 * pech, 5 * pech, FARBE(0x1E1A22));
  }
}

static void haus(Gfx &g, float t) {
  himmel(g, 0, t, 196);
  sonne(g, 244, 34, 12, t);
  boden(g, 196);
  g.kontur(1.8f, OUT);
  g.rechteck(20, 90, 130, 106, FARBE(0xF1DDB0));
  float d[] = {8, 92, 85, 48, 162, 92};
  g.polygon(d, 3, FARBE(0x9A5A3A));
  g.konturAus();
  g.rundRechteck(70, 146, 28, 50, 12, HOLZ);
  g.rundRechteck(32, 112, 22, 20, 2, FARBE(0x8FD3FF));
  g.rundRechteck(114, 112, 22, 20, 2, FARBE(0x8FD3FF));
  for (int i = 0; i < 5; i++) g.kreis(24 + i * 30, 200, 4, i & 1 ? FARBE(0xFF7FA8) : GOLD);
}

// =====================================================================
// Szenen
// =====================================================================
// 1) Die Mutter stirbt: "Bleib fromm und gut"; das Grab, Schnee wie ein weisses Tuch, dann Fruehling
static void szeneMutter(Gfx &g, float u) {
  if (u < 14) {
    zimmer(g, u, seg(u, 8, 12) * 0.6f);
    sterbebett(g, 120, 214);
    FrauPose m = fp(MUTTER, u);
    m.zu = u > 8.5f || fmodf(u, 3.5f) < 0.15f;
    m.mund = u > 2.5f && u < 7.5f && fmodf(u, 0.5f) < 0.25f ? 0.5f : 0;
    g.ausschnitt(40, 0, 160, 186);
    frau(g, 88, 230, 0.8f, 1, m);
    g.ausschnittAus();
    FrauPose t = fp(TOCHTER, u);
    t.armV = 1.5f;
    t.weint = u > 9;
    t.mund = u > 9 ? 0.4f : 0;
    frau(g, 214, 214, 0.9f, -1, t);
    blase(g, 96, 70, {"Liebes Kind, bleib fromm und gut,", "dann wird dir Gott beistehen."}, aufzu(u, 2.5f, 7.8f), 92, 150);
  } else {
    float schnee = seg(u, 17, 21) * (1 - seg(u, 23, 26));
    grabHof(g, u, 0, schnee, 0);
    if (u > 16 && u < 24)  // Schnee faellt
      for (int i = 0; i < 30; i++) {
        float x = fmodf(zufall(i + 30) * 300 + sinf(u + i) * 10, 290) - 3, y = fmodf(zufall(i + 60) * 240 + u * 30, 240);
        g.kreis(x, y, 1.8f, WEISS, 220);
      }
    FrauPose t = fp(TOCHTER, u);
    t.weint = true;
    t.armV = 2.8f;
    t.mund = 0.3f;
    frau(g, 140, 214, 0.9f, 1, t);
  }
  blende(g, u, 28, 140, 140);
}

// 2) Die Stiefmutter mit den zwei Schwestern; graue Kittel und Holzschuhe; in der Asche
static void szeneStiefmutter(Gfx &g, float u) {
  if (u < 6) {
    haus(g, u);
    Pose v = mp(VATER, u);
    v.lauf = fmodf(u * 1.6f, 1);
    float x = lerp(320, 190, sanft(seg(u, 0, 5)));
    schneiderlein(g, x, 214, 0.95f, -1, v);
    const FrauLook *l[3] = {&STIEF, &SCHW1, &SCHW2};
    for (int i = 0; i < 3; i++) {
      FrauPose p = fp(*l[i], u + i);
      p.lauf = u < 5 ? fmodf(u * 1.6f + i * 0.3f, 1) : -1;
      p.boese = i == 0 ? 0.5f : 0;
      frau(g, x + 34 + i * 26, 214, i == 0 ? 0.95f : 0.85f, -1, p);
    }
    blende(g, u, 26, 160, 150);
    return;
  }
  float nacht = seg(u, 19, 21);
  kueche(g, u, nacht);
  // Aschenputtel: erst im schoenen Kleid, dann im grauen Kittel
  bool kittel = u > 10.5f;
  FrauPose a = fp(kittel ? ASCHE : TOCHTER, u);
  a.weint = u > 11 && u < 14;
  float ax = 110, ay = 214;
  if (u > 14 && u < 19) { ax = lerp(60, 140, sanft(seg(u, 14, 17))); a.armV = 2.6f; a.lauf = u < 17 ? fmodf(u * 1.6f, 1) : -1; }  // traegt Wasser
  if (u > 21) { a.zu = true; ay = 218; }  // schlaeft bei der Asche
  frau(g, ax, ay, 0.9f, 1, a);
  if (u > 14 && u < 19) eimer(g, ax + 14, 174, 0.9f, 0);
  if (u > 21) zzz(g, ax + 8, 130, u);
  puff(g, ax, 170, seg(u, 10, 11.2f));
  // die Schwestern lachen
  if (u < 19) {
    FrauPose s1 = fp(SCHW1, u), s2 = fp(SCHW2, u + 1);
    s1.boese = s2.boese = 0.7f;
    s1.mund = s2.mund = u > 12 && fmodf(u, 0.4f) < 0.2f ? 0.6f : 0.2f;
    s1.armV = u > 7 && u < 10 ? 2.4f : 0.5f;  // nimmt das Kleid weg
    frau(g, 236, 214, 0.85f, -1, s2);
    frau(g, 200, 214, 0.85f, -1, s1);
  }
  blase(g, 200, 60, {"Hinaus mit der Küchenmagd!"}, aufzu(u, 6.4f, 9.8f), 200, 116);
  blase(g, 190, 60, {"Aschenputtel! Aschenputtel!"}, aufzu(u, 12, 15.6f), 214, 116);
  blende(g, u, 26, 140, 150);
}

// 3) Der Vater zieht zur Messe: Kleider, Perlen - und das erste Reis, das an seinen Hut stoesst
static void szeneHasel(Gfx &g, float u) {
  if (u < 10) {
    haus(g, u);
    Pose v = mp(VATER, u);
    v.armV = 0.6f;
    schneiderlein(g, 196, 214, 0.95f, -1, v);
    FrauPose s1 = fp(SCHW1, u), s2 = fp(SCHW2, u + 1), a = fp(ASCHE, u);
    s1.mund = u < 3.5f ? 0.5f : 0; s2.mund = u > 3 && u < 6 ? 0.5f : 0; a.mund = u > 6 ? 0.3f : 0;
    s1.armV = 2.2f; s2.armV = 2.2f;
    frau(g, 236, 214, 0.85f, -1, s1);
    frau(g, 264, 214, 0.85f, -1, s2);
    frau(g, 150, 214, 0.85f, 1, a);
    blase(g, 222, 54, {"Schöne Kleider!"}, aufzu(u, 0.6f, 3.4f), 236, 120);
    blase(g, 222, 54, {"Perlen und Edelsteine!"}, aufzu(u, 3.6f, 6), 262, 120);
    blase(g, 110, 54, {"Das erste Reis,", "das an Euren Hut stößt!"}, aufzu(u, 6.2f, 9.8f), 146, 120);
  } else if (u < 22) {
    // durch den Wald; ein Haselreis streift den Hut
    float scroll = (u - 10) * 36;
    himmel(g, 0, u, 200);
    huegel(g, scroll, 0.3f, 186, 34, 140, FARBE(0xA7DE85));
    for (int i = 0; i < 4; i++) tanne(g, fmodf(i * 90 - scroll * 0.6f + 3600, 360) - 40, 200, 0.9f);
    boden(g, 200);
    float bx = lerp(330, 110, seg(u, 10, 14));
    laubbaum(g, bx, 206, 0.9f, FARBE(0x5CB85A));
    g.linie(bx - 10, 130, bx - 40, 140, 3, FARBE(0x8E5B3A));  // tiefer Ast
    Pose v = mp(VATER, u);
    v.lauf = u < 17 ? fmodf(u * 1.6f, 1) : -1;
    if (u > 14.2f && u < 16) { v.armV = 2.8f; v.mund = 0.5f; }  // greift nach dem Reis
    if (u > 17) v.armV = 1.4f;
    float hx, hy;
    schneiderlein(g, 90, 216, 0.95f, 1, v, &hx, &hy);
    if (u > 15) g.linie(hx, hy, hx + 14, hy - 18, 2.5f, FARBE(0x8E5B3A));  // das abgebrochene Reis
    if (u > 17) {  // wieder zu Hause: Geschenke
      g.rechteck(0, 0, 284, 240, 0, (uint8_t)(255 * (1 - seg(u, 17, 17.4f)) * seg(u, 16.6f, 17)));
    }
  } else {
    // das Reis auf dem Grab, mit Traenen begossen, waechst zum schoenen Baum; ein weisses Voeglein
    float baum = sanft(seg(u, 24, 29));
    grabHof(g, u, baum, 0, 0);
    FrauPose a = fp(ASCHE, u);
    a.weint = u > 23;
    a.armV = u < 24 ? 1.6f : 2.6f;
    frau(g, 150, 214, 0.9f, 1, a);
    if (u > 28) taube(g, 216, lerp(20, 128, seg(u, 28, 29.5f)), 0.8f, -1, u < 29.5f ? u : -1);
  }
  blende(g, u, 30, 150, 150);
}

// 4) Die Linsen in der Asche: "Ihr zahmen Taeubchen ... die guten ins Toepfchen, die schlechten ins Kroepfchen"
static void szeneLinsen(Gfx &g, float u) {
  float linsen = seg(u, 8, 9.5f) * (1 - seg(u, 16, 24));
  kueche(g, u, 0, linsen);
  FrauPose st = fp(STIEF, u);
  st.boese = 1;
  float sx = 220;
  if (u > 7 && u < 9.5f) st.armV = lerp(1.2f, 2.4f, seg(u, 7, 8));  // schuettet die Linsen aus
  if (u > 29) { sx = lerp(220, 330, seg(u, 29, 32)); st.lauf = fmodf(u * 1.6f, 1); }
  float shx, shy;
  frau(g, sx, 214, 0.95f, -1, st, &shx, &shy);
  if (u > 6 && u < 10) schuessel(g, shx - 6, shy + (u > 8 ? -6 : 4), u < 8 ? 1 : 0);
  FrauPose s1 = fp(SCHW1, u), s2 = fp(SCHW2, u + 1);
  float qx = u > 29 ? lerp(250, 360, seg(u, 29.4f, 32)) : 250;
  s1.mund = u < 4 ? 0.6f : 0;
  frau(g, qx, 214, 0.85f, -1, s1);
  frau(g, qx + 22, 214, 0.85f, -1, s2);
  FrauPose a = fp(ASCHE, u);
  float ax = 96;
  a.mund = u > 4 && u < 7 ? 0.4f : 0;
  if (u > 10 && u < 15) { ax = 70; a.armV = 2.7f; a.mund = 0.4f; }  // ruft die Tauben
  if (u > 24 && u < 28) { ax = lerp(96, 176, seg(u, 24, 26)); a.armV = 1.5f; }
  a.weint = u > 29;
  float ahx, ahy;
  frau(g, ax, 214, 0.9f, 1, a, &ahx, &ahy);
  if (u > 15 && u < 24) schuessel(g, 140, 216, seg(u, 16, 24));
  if (u > 24 && u < 28) schuessel(g, ahx + 6, ahy + 4, 1);
  // Tauben kommen zum Fenster herein und picken
  if (u > 13 && u < 25)
    for (int i = 0; i < 6; i++) {
      float an = 13 + i * 0.35f;
      float k = sanft(seg(u, an, an + 1.6f));
      float zx = 92 + (i % 3) * 22, zy = 210 - (i / 3) * 6;
      float x = lerp(55, zx, k), y = lerp(120, zy, k) - sinf(k * 3.14159f) * 20;
      if (u > 23.5f) { float w = seg(u, 23.5f + i * 0.1f, 24.6f); x = lerp(zx, 55, w); y = lerp(zy, 120, w); }
      bool fliegt = k < 1 || u > 23.5f;
      float pick = !fliegt ? fabsf(sinf(u * 8 + i)) : 0;
      taube(g, x, y, 0.7f, i & 1 ? -1 : 1, fliegt ? u : -1, pick);
    }
  blase(g, 210, 50, {"Ein Fest beim König!"}, aufzu(u, 0.4f, 3.6f), 250, 120);
  blase(g, 100, 60, {"Darf ich mit?"}, aufzu(u, 4, 6.8f), 100, 128);
  blase(g, 196, 54, {"Lies die Linsen aus der Asche!"}, aufzu(u, 8.6f, 11.4f), 214, 118);
  blase(g, 110, 46, {"Ihr zahmen Täubchen,", "ihr Turteltäubchen,", "kommt und helft mir lesen!"}, aufzu(u, 11.4f, 15.4f), 74, 128);
  blase(g, 142, 50, {"Die guten ins Töpfchen,", "die schlechten ins Kröpfchen."}, aufzu(u, 16.6f, 22), 120, 196);
  blase(g, 200, 54, {"Du hast keine Kleider", "und kannst nicht tanzen!"}, aufzu(u, 26.4f, 30), 214, 118);
  blende(g, u, 32, 140, 150);
}

// 5) Am Haselbaum: "Baeumchen, ruettel dich und schuettel dich, wirf Gold und Silber ueber mich!"
static void szeneBaeumchen(Gfx &g, float u) {
  float rasch = seg(u, 7.6f, 8) * (1 - seg(u, 9.4f, 10));
  grabHof(g, u, 1, 0, 0.6f, rasch);
  taube(g, 222, 120, 0.8f, -1, u > 8 && u < 10 ? u : -1);
  bool kleid = u > 11.2f;
  FrauPose a = fp(kleid ? SILBER : ASCHE, u);
  a.armV = u < 8 ? 2.5f : 1.2f;
  a.mund = u > 2 && u < 8 && fmodf(u, 0.5f) < 0.25f ? 0.4f : kleid ? 0.5f : 0;
  float ax = 150;
  if (u > 14) { ax = lerp(150, -40, seg(u, 15, 22)); a.lauf = fmodf(u * 1.6f, 1); }
  frau(g, ax, 214, 0.95f, u > 14 ? -1 : 1, a);
  if (u > 8.5f && u < 12)  // Gold und Silber fallen herab
    for (int i = 0; i < 14; i++) {
      float x = 150 + (zufall(i + 900) - 0.5f) * 70, y = fmodf(80 + zufall(i + 950) * 40 + (u - 8.5f) * 60, 140) + 60;
      g.stern(x, y, 4, 1.6f, 4, u + i, i & 1 ? GOLD : WEISS);
    }
  puff(g, 150, 170, seg(u, 10.6f, 11.8f));
  if (kleid && u < 15) glitzern(g, ax, 160, 34, u, 6);
  blase(g, 120, 46, {"Bäumchen, rüttel dich und schüttel dich,", "wirf Gold und Silber über mich!"}, aufzu(u, 1.6f, 8), 146, 126);
  blende(g, u, 24, 150, 150);
}

// Tanz: zwei Figuren drehen sich um eine Mitte
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

// 6) Erster und zweiter Ball: Tanz mit dem Koenigssohn; Flucht ins Taubenhaus und auf den Birnbaum
static void szeneBall(Gfx &g, float u) {
  if (u < 22) {
    ballsaal(g, u);
    // Stiefmutter und Schwestern am Rand erkennen sie nicht
    FrauPose st = fp(STIEF, u), s1 = fp(SCHW1, u + 1), s2 = fp(SCHW2, u + 2);
    st.boese = 0.4f; s1.mund = u > 8 && u < 12 ? 0.5f : 0;
    frau(g, 250, 214, 0.85f, -1, st);
    frau(g, 230, 214, 0.8f, -1, s1);
    frau(g, 270, 214, 0.8f, -1, s2);
    if (u < 4) {
      FrauPose a = fp(SILBER, u);
      Pose p = mp(PRINZ, u);
      p.armV = 1.6f; p.lauf = u < 3 ? fmodf(u * 1.6f, 1) : -1;
      frau(g, 90, 214, 0.92f, 1, a);
      schneiderlein(g, lerp(-30, 130, sanft(seg(u, 0, 3))), 214, 0.95f, -1, p);
    } else tanz(g, 110, u, SILBER);
    for (int i = 0; i < 4; i++) {  // Noten
      float ph = fmodf(u * 0.4f + i / 4.0f, 1);
      notenzeichen(g, 40 + i * 60, 150 - ph * 100, i & 1 ? FARBE(0x3E7FDC) : FARBE(0xE8473D), (uint8_t)(220 * huepf(ph)));
    }
    blase(g, 220, 60, {"Wer ist die schöne", "fremde Königstochter?"}, aufzu(u, 8, 12.4f), 236, 130);
    blase(g, 110, 60, {"Das ist meine Tänzerin!"}, aufzu(u, 13.4f, 17), 120, 128);
    blende(g, u, 22, 120, 150);
    return;
  }
  // Flucht im Garten: am ersten Abend ins Taubenhaus, am zweiten auf den Birnbaum
  float v = u - 22;
  bool zwei = v > 10;
  float w = zwei ? v - 10 : v;
  himmel(g, 1, u, 196);
  mond(g, 244, 36, 11);
  boden(g, 196, FARBE(0x3E5A4A));
  float kipp = sanft(seg(w, 6, 7.5f)) * 1.4f;
  if (!zwei) taubenhaus(g, 170, 206, kipp);
  else birnbaum(g, 170, 206, kipp, u);
  // Aschenputtel springt hinein / klettert hinauf
  if (w < 3) {
    FrauPose a = fp(zwei ? ROSA : SILBER, u);
    a.lauf = fmodf(u * 2.4f, 1);
    float k = seg(w, 0, 3);
    frau(g, lerp(-30, 170, k), 214 - sinf(seg(w, 2, 3) * 3.14159f) * 60, 0.85f, 1, a);
  }
  Pose p = mp(PRINZ, u);
  float px = lerp(-40, 110, seg(w, 1.5f, 4));
  p.lauf = w < 4 ? fmodf(u * 2, 1) : -1;
  p.armV = w > 4 ? 2.4f : 0.5f;
  schneiderlein(g, px, 214, 0.95f, 1, p);
  // der Vater haut mit der Axt
  if (w > 3.5f) {
    Pose va = mp(VATER, u);
    float k = seg(w, 3.5f, 5);
    float vx = lerp(330, 222, k);
    va.lauf = k < 1 ? fmodf(u * 1.6f, 1) : -1;
    float hx, hy;
    va.armV = w > 5 && w < 7 ? 1.5f + fabsf(sinf(w * 7)) * 1.4f : 0.8f;
    schneiderlein(g, vx, 214, 0.95f, -1, va, &hx, &hy);
    if (w > 5 && w < 7.5f) {
      g.linie(hx, hy, hx - 10, hy - 20, 3, HOLZ);
      float ax2[] = {hx - 6, hy - 24, hx - 18, hy - 20, hx - 14, hy - 12};
      g.polygon(ax2, 3, FARBE(0xC7D0DC));
    }
  }
  if (w > 7.5f) fragezeichen(g, 140, 120, 1, (uint8_t)(255 * seg(w, 7.5f, 8)));
  g.kameraAus();
  g.text(SCHRIFT_K, zwei ? "2. Abend" : "1. Abend", 10, 18, WEISS);
  blende(g, u, 42, 150, 150);
}

// 7) Dritter Abend: goldenes Kleid, die Treppe mit Pech, der goldene Pantoffel bleibt haengen
static void szeneSchuh(Gfx &g, float u) {
  if (u < 9) {
    ballsaal(g, u);
    tanz(g, 142, u + 3, GOLDKLEID);
    glitzern(g, 142, 150, 60, u, 8, 200);
    blende(g, u, 24, 142, 150);
    return;
  }
  treppe(g, 1);
  // sie springt die Treppe hinunter, ein Schuh bleibt kleben
  float k = seg(u, 9, 13);
  if (u < 13.2f) {
    FrauPose a = fp(GOLDKLEID, u);
    a.lauf = fmodf(u * 2.4f, 1);
    frau(g, lerp(142, 90, k), lerp(124, 234, k), 0.8f, -1, a);
  }
  if (u > 11.2f) schuh(g, 124, 182, 1, GOLD);
  if (u > 13) {
    Pose p = mp(PRINZ, u);
    float hx, hy;
    p.armV = u > 15 ? 2.8f : 1.2f;
    float px = lerp(150, 132, seg(u, 13, 14.5f));
    schneiderlein(g, px, 186, 0.9f, -1, p, &hx, &hy);
    if (u > 15) { schuh(g, hx, hy - 2, 1.1f, GOLD); glitzern(g, hx, hy - 6, 14, u, 4); }
  }
  blase(g, 142, 50, {"Keine soll meine Gemahlin werden", "als die, an deren Fuß", "dieser goldene Schuh passt!"}, aufzu(u, 15.6f, 21.6f), 130, 150);
  blende(g, u, 24, 142, 150);
}

// 8) Die Schuhprobe: Zehe ab, Ferse ab - "Rucke di guh, Blut ist im Schuh" - dann die rechte Braut
static void szeneProbe(Gfx &g, float u) {
  // Abschnitte: 0-14 erste Schwester, 14-22 Ritt 1, 22-29 zweite Schwester, 29-34 Ritt 2,
  //             34-48 Aschenputtel, 48-56 Ritt mit der rechten Braut
  const FrauLook *braut = nullptr;
  bool ritt1 = u > 14 && u < 22, ritt2 = u > 29 && u < 34, ritt3 = u > 48;
  if (ritt1 || ritt2 || ritt3) {
    float w = ritt1 ? u - 14 : ritt2 ? u - 29 : u - 48;
    float dauer = ritt1 ? 8 : ritt2 ? 5 : 8;
    grabHof(g, u, 1, 0, 0);
    // zwei Taeubchen auf dem Haselbaum
    bool fliegen = ritt3 && w > 6;
    for (int i = 0; i < 2; i++) {
      float tx = 206 + i * 22, ty = 128 - i * 10;
      if (fliegen) { float k = seg(w, 6, 7.6f); tx = lerp(tx, 128 + i * 26, k); ty = lerp(ty, 146, k); }
      taube(g, tx, ty, 0.7f, -1, fliegen && w < 7.6f ? u : -1);
    }
    braut = ritt1 ? &SCHW1 : ritt2 ? &SCHW2 : &ASCHE;
    float x = ritt3 ? lerp(320, 140, sanft(seg(w, 0, 3))) : lerp(-60, 120, sanft(seg(w, 0, 2.5f)));
    float dir = ritt3 ? -1 : 1;
    if (!ritt3 && w > dauer - 2) { x = lerp(120, -60, seg(w, dauer - 2, dauer)); dir = -1; }  // zurueck!
    ritt(g, x, 214, u, dir, braut, true);
    if (!ritt3 && w > 2 && w < dauer - 1.6f) {
      g.ellipse(x + 4, 206, 4, 2, FARBE(0xC01830));  // Blut tropft aus dem Schuh
      blase(g, 160, 44, {"Rucke di guh, rucke di guh,", "Blut ist im Schuh:", "der Schuh ist zu klein,",
                         "die rechte Braut sitzt noch daheim."}, aufzu(w, 2.2f, dauer - 1.6f), 214, 120);
    }
    if (ritt3)
      blase(g, 160, 44, {"Rucke di guh, rucke di guh,", "kein Blut im Schuh:", "der Schuh ist nicht zu klein,",
                         "die rechte Braut, die führt er heim."}, aufzu(w, 1.6f, 6.6f), 214, 120);
    blende(g, u, 56, 142, 150);
    return;
  }
  // in der Stube: Anprobe
  zimmer(g, u, 0);
  bool erste = u < 14, zweite = u > 22 && u < 29;
  Pose p = mp(PRINZ, u);
  p.armV = u > 41 && u < 46 ? 1.8f : 0.6f;
  float phx, phy;
  schneiderlein(g, 236, 214, 0.95f, -1, p, &phx, &phy);
  if (erste || zweite) {
    float w = erste ? u : u - 22;
    float start = erste ? 3 : 0;
    FrauPose st = fp(STIEF, u);
    st.boese = 0.7f;
    float mx, my;
    st.armV = w > start + 2 && w < start + 5 ? 2.0f : 0.6f;
    frau(g, 80, 214, 0.95f, 1, st, &mx, &my);
    if (w > start + 2 && w < start + 5) {  // reicht das Messer
      g.linie(mx, my, mx + 10, my - 6, 2.5f, FARBE(0xC7D0DC));
      g.linie(mx - 4, my + 2, mx, my, 3, HOLZ);
    }
    FrauPose s = fp(erste ? SCHW1 : SCHW2, u);
    s.mund = w > start + 5 && w < start + 6.5f ? 0.9f : 0;  // Au!
    s.zu = w > start + 5 && w < start + 6.5f;
    frau(g, 140, 214, 0.9f, 1, s);
    schuh(g, 154, 214, 1.1f, GOLD, w > start + 5.5f);
    if (w > start + 5 && w < start + 5.6f) g.stern(150, 206, 10, 4, 6, w * 4, WEISS);
    if (erste) {
      blase(g, 96, 54, {"Hau die Zehe ab!", "Wenn du Königin bist,", "brauchst du nicht mehr", "zu Fuß zu gehen."}, aufzu(w, start + 1.6f, start + 5), 86, 120);
    } else {
      blase(g, 96, 54, {"Hau ein Stück", "von der Ferse ab!"}, aufzu(w, start + 1.6f, start + 5), 86, 120);
    }
    blende(g, u, 56, 142, 150);
    return;
  }
  // Aschenputtel
  float w = u - 34;
  Pose v = mp(VATER, u);
  schneiderlein(g, 60, 214, 0.95f, 1, v);
  blase(g, 150, 50, {"Habt ihr keine andere Tochter?"}, aufzu(w, 0.4f, 3), 220, 120);
  blase(g, 100, 50, {"Nur ein kleines Aschenputtel."}, aufzu(w, 3.2f, 6.4f), 68, 120);
  if (w > 6) {
    FrauPose a = fp(ASCHE, u);
    float ax = lerp(-30, 140, sanft(seg(w, 6, 8.5f)));
    a.lauf = w < 8.5f ? fmodf(u * 1.6f, 1) : -1;
    a.mund = w > 10 ? 0.4f : 0;
    frau(g, ax, 214, 0.9f, 1, a);
    if (w > 8.5f && w < 10) glitzern(g, ax, 150, 22, u, 5, 180);  // waescht Haende und Gesicht
    if (w > 9) {
      schuh(g, 154, 214, 1.1f, GOLD);
      if (w > 10) glitzern(g, 154, 206, 16, u, 6);  // passt wie angegossen
    }
    if (w > 10.5f) glitzern(g, 196, 150, 60, u, 8, 160);
  }
  blase(g, 180, 46, {"Das ist die rechte Braut!"}, aufzu(w, 11, 14), 230, 120);
  blende(g, u, 56, 142, 150);
}

// 9) Hochzeit: die Tauben picken den falschen Schwestern die Augen aus (nur angedeutet)
static void szeneHochzeit(Gfx &g, float u) {
  himmel(g, 0, u, 196);
  sonne(g, 244, 34, 12, u);
  boden(g, 196);
  kapelle(g, 142, 206, u < 12 ? 0 : 1, 0);
  // Hochzeitszug: Schwestern rechts und links, beim Kirchgang und beim Herausgehen
  bool hinein = u < 12;
  float zug = hinein ? lerp(-60, 120, sanft(seg(u, 0, 8))) : lerp(142, 220, sanft(seg(u, 13, 18)));
  FrauPose b = fp(BRAUT, u);
  b.mund = 0.4f;
  b.lauf = (hinein && u < 8) || (!hinein && u > 13 && u < 18) ? fmodf(u * 1.4f, 1) : -1;
  Pose p = mp(PRINZ, u);
  p.lauf = b.lauf;
  p.mund = 0.4f;
  float d = hinein ? 1 : 1;
  FrauPose s1 = fp(SCHW1, u), s2 = fp(SCHW2, u + 1);
  s1.lauf = s2.lauf = b.lauf;
  // nach dem Strafgericht: Augen zu, tasten
  bool blind1 = u > 9.5f, blind2 = u > 9.5f;
  if (blind1) { s1.zu = true; s1.armV = 1.8f; s1.mund = 0.5f; }
  if (blind2) { s2.zu = true; s2.armV = 1.8f; s2.mund = 0.5f; }
  if (u < 12 || u > 13) {
    frau(g, zug - 52, 214, 0.85f, d, s1);
    frau(g, zug - 14, 214, 0.92f, d, b);
    schneiderlein(g, zug + 18, 214, 0.95f, d, p);
    frau(g, zug + 56, 214, 0.85f, d, s2);
  }
  // Tauben auf den Schultern der Braut, dann fliegen sie zu den Schwestern
  for (int i = 0; i < 2; i++) {
    float tx = zug - 20 + i * 12, ty = 140;
    float an = hinein ? 7 : 15.5f;
    float k = seg(u, an + i * 0.3f, an + 0.9f + i * 0.3f) * (1 - seg(u, an + 2.2f, an + 3));
    float zx = i ? zug + 56 : zug - 52, zy = 146;
    float x = lerp(tx, zx, k), y = lerp(ty, zy, k);
    taube(g, x, y, 0.6f, i ? 1 : -1, k > 0 && k < 1 ? u : -1, k >= 1 ? fabsf(sinf(u * 10)) : 0);
    if (k > 0.95f) g.stern(zx, zy + 4, 6, 2.5f, 5, u * 6, WEISS, 200);
  }
  if (u > 19) {
    for (int i = 0; i < 6; i++) {
      float k = fmodf((u - 19) * 0.45f + i / 6.0f, 1);
      herz(g, zug + 10 + sinf(k * 7 + i) * 24, 150 - k * 110, 5 + (i % 3), FARBE(0xFF5A7A), (uint8_t)(255 * huepf(k)));
    }
  }
  if (u > 20 && u < 30)
    for (int i = 0; i < 16; i++) {
      float x = zufall(i + 1990) * 284 + sinf(u * 2 + i) * 12;
      float y = fmodf(zufall(i + 1960) * 240 + (u - 20) * 40, 250) - 10;
      g.ellipse(x, y, 3, 2, i & 1 ? FARBE(0xFF9FC8) : WEISS, 230, u + i);
    }
  g.kameraAus();
  if (u < 1.2f) g.iris(142, 150, lerp(0, 330, sanft(u / 1.2f)));
  if (u > 31) g.iris(zug + 10, 160, lerp(330, 0, sanft(seg(u, 31, 33.5f))));
  if (u > 33.5f) {
    uint8_t a = (uint8_t)(255 * sanft(seg(u, 33.5f, 34.5f)));
    g.text(SCHRIFT_G, "Ende", 142, 122, GOLD, 1, a);
    for (int i = 0; i < 6; i++) {
      float an = u * 1.2f + i * 1.047f;
      g.stern(142 + cosf(an) * 60, 112 + sinf(an) * 30, 5, 2, 5, an, GOLD, a);
    }
  }
}

static void bild(Gfx &g, float t) {
  if (t < A2) szeneMutter(g, t - A1);
  else if (t < A3) szeneStiefmutter(g, t - A2);
  else if (t < A4) szeneHasel(g, t - A3);
  else if (t < A5) szeneLinsen(g, t - A4);
  else if (t < A6) szeneBaeumchen(g, t - A5);
  else if (t < A7) szeneBall(g, t - A6);
  else if (t < A8) szeneSchuh(g, t - A7);
  else if (t < A9) szeneProbe(g, t - A8);
  else szeneHochzeit(g, t - A9);
}

// =====================================================================
// Musik
// =====================================================================
// Aschenputtels Lied (d-Moll, traurig und zart)
static const Ton3 MEL_ASCHE[] = {
  {74, 1, 50}, {72, .5, 0}, {70, .5, 0}, {69, 1, 0}, {65, 1, 0}, {67, 1, 46}, {69, .5, 0}, {70, .5, 0}, {69, 2, 0},
  {70, 1, 43}, {69, .5, 0}, {67, .5, 0}, {65, 1, 0}, {62, 1, 0}, {64, 1, 45}, {61, 1, 0}, {62, 2, 50}};
// Walzer im Ballsaal (G-Dur, 3/4)
static const Ton3 MEL_WALZER[] = {
  {67, 1, 43}, {71, 1, 0}, {74, 1, 0}, {79, 2, 0}, {78, 1, 0}, {76, 1, 48}, {74, 1, 0}, {72, 1, 0}, {71, 3, 0},
  {69, 1, 50}, {72, 1, 0}, {76, 1, 0}, {78, 2, 0}, {76, 1, 0}, {74, 1, 43}, {71, 1, 0}, {67, 1, 0}, {74, 3, 0}};
// Taubenruf: "Rucke di guh"
static void ruckediguh(Synth &s, float x) {
  for (int k = 0; k < 2; k++) {
    s.ton(x + k * 0.7f, 330, 0.08f, 0.04f, 0.18f, SINUS, MUSIK, 300);
    s.ton(x + k * 0.7f + 0.2f, 294, 0.08f, 0.04f, 0.18f, SINUS, MUSIK, 280);
    s.ton(x + k * 0.7f + 0.4f, 262, 0.09f, 0.04f, 0.3f, SINUS, MUSIK, 250);
  }
}

static void ton(Synth &s, float t0, float t1) {
  float a = A1;
  spieleMelodie(s, M(MEL_ASCHE), a + 0.5f, a + 27.5f, 0.42f, t0, t1, 0.8f);
  if (bei(a + 9, t0, t1)) s.glocke(a + 9, 62, 0.5f);
  a = A2;
  spieleMelodie(s, M(MEL_SPUK), a + 6, a + 19, 0.3f, t0, t1, 0.7f);
  for (float x : {a + 12.2f, a + 13.4f, a + 14.6f}) if (bei(x, t0, t1)) for (int k = 0; k < 3; k++) s.note(x + k * 0.1f, 84 - k * 2, 0.4f);  // Gelaechter
  if (bei(a + 10, t0, t1)) for (int k = 0; k < 6; k++) s.note(a + 10 + k * 0.08f, 88 - k * 3, 0.5f);  // Puff
  spieleMelodie(s, M(MEL_ASCHE), a + 19.5f, a + 25.5f, 0.42f, t0, t1, 0.6f);
  a = A3;
  spieleMelodie(s, M(MEL_WANDER), a + 0.5f, a + 21.5f, 0.26f, t0, t1, 0.8f);
  if (bei(a + 14.4f, t0, t1)) for (int k = 0; k < 5; k++) s.note(a + 14.4f + k * 0.07f, 90 - k * 3, 0.5f);  // Hut faellt
  spieleMelodie(s, M(MEL_ASCHE), a + 22.5f, a + 27, 0.4f, t0, t1, 0.7f);
  if (bei(a + 24, t0, t1)) for (int k = 0; k < 10; k++) s.note(a + 24 + k * 0.45f, 67 + k * 2, 0.4f);  // der Baum waechst
  if (bei(a + 28.4f, t0, t1)) for (int k = 0; k < 4; k++) s.note(a + 28.4f + k * 0.09f, 91 + (k & 1) * 3, 0.45f);  // Voeglein
  a = A4;
  spieleMelodie(s, M(MEL_F), a + 0.3f, a + 4, 0.22f, t0, t1, 0.7f);
  if (bei(a + 8.2f, t0, t1)) for (int k = 0; k < 16; k++) s.rauschen(a + 8.2f + k * 0.05f, 0.03f, HOCHPASS, 3500, 0.06f);  // Linsen rieseln
  spieleMelodie(s, M(MEL_KEGEL), a + 15.5f, a + 23.5f, 0.24f, t0, t1, 0.8f);
  for (float x = ceilf(t0 / 0.18f) * 0.18f; x < t1; x += 0.18f)
    if (x > a + 16 && x < a + 23.4f) s.ton(x, 2400 + fmodf(x * 53, 4) * 200, 0.03f, 0.002f, 0.04f);  // Picken
  for (int i = 0; i < 6; i++) if (bei(a + 13 + i * 0.35f, t0, t1)) s.rauschen(a + 13 + i * 0.35f, 0.3f, BANDPASS, 1200, 0.06f, 0.8f, 0.05f, 2400);  // Fluegel
  spieleMelodie(s, M(MEL_ASCHE), a + 26, a + 31.5f, 0.42f, t0, t1, 0.7f);
  a = A5;
  for (int k = 0; k < 8; k++) if (bei(a + 1.8f + k * 0.75f, t0, t1)) s.note(a + 1.8f + k * 0.75f, 72 + (k % 4) * 2, 0.5f);
  if (bei(a + 7.6f, t0, t1)) for (int k = 0; k < 10; k++) s.rauschen(a + 7.6f + k * 0.15f, 0.12f, HOCHPASS, 2200, 0.06f);  // rascheln
  if (bei(a + 8.6f, t0, t1)) for (int k = 0; k < 14; k++) s.glocke(a + 8.6f + k * 0.18f, 84 + (k % 5) * 2, 0.4f);
  if (bei(a + 10.6f, t0, t1)) for (int k = 0; k < 6; k++) s.note(a + 10.6f + k * 0.08f, 79 + k * 2, 0.6f);
  spieleMelodie(s, M(MEL_F), a + 12, a + 23, 0.26f, t0, t1, 0.8f);
  a = A6;
  spieleMelodie(s, M(MEL_WALZER), a + 0.4f, a + 21.5f, 0.24f, t0, t1, 0.9f);
  spieleMelodie(s, M(MEL_JAGD), a + 22, a + 26, 0.16f, t0, t1, 0.8f);
  spieleMelodie(s, M(MEL_JAGD), a + 32, a + 36, 0.16f, t0, t1, 0.8f);
  for (float w : {a + 27.0f, a + 37.0f}) for (int k = 0; k < 5; k++) if (bei(w + k * 0.4f, t0, t1)) s.ton(w + k * 0.4f, 180, 0.25f, 0.002f, 0.12f, SINUS, GERAEUSCH, 90);  // Axt
  for (float w : {a + 29.4f, a + 39.4f}) if (bei(w, t0, t1)) { s.ton(w, 90, 0.35f, 0.002f, 0.35f, SINUS, GERAEUSCH, 45); s.rauschen(w, 0.3f, TIEFPASS, 800, 0.3f); }
  a = A7;
  spieleMelodie(s, M(MEL_WALZER), a + 0.3f, a + 9, 0.22f, t0, t1, 0.9f);
  if (bei(a + 9, t0, t1)) for (int k = 0; k < 8; k++) s.note(a + 9 + k * 0.45f, 84 - k * 2, 0.5f);  // die Treppe hinab
  if (bei(a + 11.2f, t0, t1)) s.glocke(a + 11.2f, 88, 0.8f);  // der Schuh bleibt haengen
  spieleMelodie(s, M(MEL_ASCHE), a + 14, a + 23.5f, 0.4f, t0, t1, 0.7f);
  a = A8;
  spieleMelodie(s, M(MEL_SPUK), a + 0.5f, a + 13, 0.3f, t0, t1, 0.7f);
  for (float x : {a + 8.0f, a + 27.0f}) if (bei(x, t0, t1)) { s.rauschen(x, 0.08f, HOCHPASS, 3000, 0.15f); for (int k = 0; k < 3; k++) s.note(x + 0.1f + k * 0.1f, 64 - k * 3, 0.7f); }  // Au!
  for (float x : {a + 16.0f, a + 31.0f}) if (bei(x, t0, t1)) { ruckediguh(s, x); }
  spieleMelodie(s, M(MEL_JAGD), a + 14, a + 15.8f, 0.18f, t0, t1, 0.6f);
  spieleMelodie(s, M(MEL_ASCHE), a + 34, a + 44, 0.38f, t0, t1, 0.7f);
  if (bei(a + 44, t0, t1)) for (int k = 0; k < 8; k++) s.glocke(a + 44 + k * 0.15f, 79 + (k % 4) * 2, 0.5f);  // passt!
  if (bei(a + 49.6f, t0, t1)) ruckediguh(s, a + 49.6f);
  spieleMelodie(s, M(MEL_F), a + 51.5f, a + 55.5f, 0.24f, t0, t1, 0.9f);
  a = A9;
  spieleMelodie(s, M(MEL_F), a + 0.4f, a + 7, 0.24f, t0, t1, 0.9f);
  for (float x : {a + 7.4f, a + 15.9f}) if (bei(x, t0, t1)) for (int k = 0; k < 4; k++) s.note(x + k * 0.12f, 60 - k * 2, 0.6f);
  for (int i = 0; i < 14; i++) if (bei(a + 18 + i * 0.6f, t0, t1)) s.glocke(a + 18 + i * 0.6f, i & 1 ? 79 : 72 + (i % 4 == 2) * 4, 0.85f);
  spieleMelodie(s, M(MEL_WALZER), a + 26, a + 32, 0.24f, t0, t1, 0.8f);
  if (bei(a + 33.5f, t0, t1)) { s.glocke(a + 33.5f, 72); s.glocke(a + 33.8f, 76); s.glocke(a + 34.1f, 79); s.glocke(a + 34.5f, 84); }
}

// =====================================================================
// Startbild und Vorschau
// =====================================================================
static void titelSzene(Gfx &g, float t) {
  grabHof(g, t, 1, 0, 0.5f);
  for (int i = 0; i < 2; i++) taube(g, 206 + i * 24, 126 - i * 8, 0.7f, -1);
  FrauPose a = fp(GOLDKLEID, t);
  a.drehen = sinf(t * 2) * 3;
  a.armV = 1.2f + sinf(t * 2) * 0.3f;
  a.mund = 0.4f;
  frau(g, 100, 222, 1.1f, 1, a);
  glitzern(g, 100, 150, 46, t, 6, 200);
  schuh(g, 150, 228, 1.2f, GOLD);
}

static void titelbild(Gfx &g, float t) {
  titelSzene(g, t);
  g.rundRechteck(140, 64, 136, 50, 14, FARBE(0x1E1A3A), 160);
  g.text(SCHRIFT_G, "Aschenputtel", 208, 99, GOLD, 1);
  float b = 1 + sinf(t * 4) * 0.08f;
  g.kontur(2, OUT);
  g.kreis(208, 150, 14 * b, FARBE(0xE8473D));
  g.konturAus();
  float tri[] = {203, 142, 203, 158, 216, 150};
  g.polygon(tri, 3, WEISS);
}

static void vorschau(Gfx &g, float t) { titelSzene(g, t); }

static void symbol(Gfx &g, float x, float y, float t) {
  schuh(g, x, y - 8 - fabsf(sinf(t * 2)) * 4, 2.2f, GOLD);
  glitzern(g, x, y - 18, 24, t, 4, 200);
  for (int i = 0; i < 2; i++) taube(g, x - 36 + i * 72, y - 46 + sinf(t * 2.4f + i * 2) * 5, 1.4f, i ? -1 : 1, fmodf(t * 1.6f + i * 0.5f, 1));
}

const Maerchen ASCHENPUTTEL = {"Aschenputtel", ADAUER, FARBE(0xF2C94E), bild, ton, titelbild, vorschau, symbol, LIED(MEL_ASCHE, 0.42f)};

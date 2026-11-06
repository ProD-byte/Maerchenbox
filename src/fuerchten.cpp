// Von einem, der auszog, das Fürchten zu lernen - ein Spieluhr-Film fuer die Maerchenbox.
// Nach dem Maerchen der Brueder Grimm: Kuester im Glockenturm, die sieben Gehenkten, drei Naechte im
// verwunschenen Schloss (Katzen und Hunde, halber Mann und Kegeln, toter Vetter und der Alte),
// am Ende der Eimer mit Gruendlingen. Gezeichnet im Spieluhr-Stil, ohne Blut.
#include "maerchen.h"
#include "schriften.h"
#include "figuren.h"
#include "melodien.h"
#include <stdio.h>
#include <initializer_list>

// ---------- Ablauf (Sekunden) ----------
static const float F1 = 0, F2 = 30, F3 = 60, F4 = 96, F5 = 114, F6 = 156, F7 = 194, F8 = 244, F9 = 262, FDAUER = 300;

// ---------- Aussehen ----------
static Aussehen hansLook() {
  Aussehen a;
  a.weste = FARBE(0xD9553F); a.hemd = FARBE(0xFFF6E8); a.hose = FARBE(0x4A5C8C);
  a.haar = FARBE(0xC0632E); a.muetze = 1; a.halstuch = FARBE(0x3E7FDC);
  return a;
}
static Aussehen bruderLook() {
  Aussehen a;
  a.weste = FARBE(0x5B8F3A); a.hose = FARBE(0x7A5236); a.haar = FARBE(0x3A2A20); a.muetze = 1;
  return a;
}
static Aussehen vaterLook() {
  Aussehen a;
  a.weste = FARBE(0x8E6E4E); a.hose = FARBE(0x5A4636); a.haar = FARBE(0xC9C9C9); a.muetze = 1;
  return a;
}
static Aussehen kuesterLook() {
  Aussehen a;
  a.weste = FARBE(0x3A3A4A); a.hose = FARBE(0x2E2E3A); a.haar = FARBE(0x9A9A9A); a.muetze = 1;
  return a;
}
static Aussehen hansNachtLook() {
  Aussehen a = hansLook();
  a.muetze = 2; a.hut = FARBE(0x3E7FDC); a.halstuch = 0;
  return a;
}
static const Aussehen HANS = hansLook(), BRUDER = bruderLook(), VATER = vaterLook(), KUESTER = kuesterLook(),
                      HANS_NACHT = hansNachtLook();

static Pose pose(const Aussehen &a) {
  Pose p;
  p.aussehen = &a;
  return p;
}

// =====================================================================
// Neue Figuren und Dinge
// =====================================================================
static void bart(Gfx &g, float x, float y, float s, float dir) {
  g.ellipse(x + 5 * s * dir, y - 47 * s, 11 * s, 8 * s, FARBE(0xE6E6E6));
}

static void eule(Gfx &g, float x, float y, float s, float t, bool ruft) {
  g.kontur(1.3f * s, OUT);
  g.ellipse(x, y, 12 * s, 15 * s, FARBE(0x8A5A34));
  g.konturAus();
  float o1[] = {x - 11 * s, y - 10 * s, x - 6 * s, y - 13 * s, x - 9 * s, y - 20 * s};
  float o2[] = {x + 11 * s, y - 10 * s, x + 6 * s, y - 13 * s, x + 9 * s, y - 20 * s};
  g.polygon(o1, 3, FARBE(0x6E4426));
  g.polygon(o2, 3, FARBE(0x6E4426));
  g.ellipse(x, y + 4 * s, 7 * s, 9 * s, FARBE(0xD9B98A));
  bool zu = fmodf(t, 3.1f) < 0.15f;
  for (int i = -1; i <= 1; i += 2) {
    g.kreis(x + i * 5 * s, y - 6 * s, 4.5f * s, WEISS);
    if (zu) g.linie(x + i * 5 * s - 3 * s, y - 6 * s, x + i * 5 * s + 3 * s, y - 6 * s, 1.5f * s, OUT);
    else g.kreis(x + i * 5 * s, y - 6 * s, 2.3f * s, SCHWARZ);
  }
  float sn[] = {x - 2 * s, y - 2 * s, x + 2 * s, y - 2 * s, x, y + (ruft ? 4 : 2) * s};
  g.polygon(sn, 3, FARBE(0xF2A93B));
}

static void kerze(Gfx &g, float x, float y, float t) {
  g.kontur(1.2f, OUT);
  g.rundRechteck(x - 4, y - 18, 8, 18, 2, FARBE(0xFFF3D6));
  g.konturAus();
  g.rundRechteck(x - 8, y - 2, 16, 4, 2, FARBE(0xC9A46A));
  float f = sinf(t * 13) * 0.8f + sinf(t * 7.3f) * 0.6f;
  g.kreis(x, y - 25, 10, FARBE(0xFFE9A0), 50);
  g.ellipse(x + f * 0.5f, y - 24, 3, 5.5f, FARBE(0xFFB83D));
  g.ellipse(x + f * 0.3f, y - 23, 1.6f, 3, FARBE(0xFFF3C0));
}

// Bettlaken-Gespenst (der Kuester); fall = Drehwinkel beim Hinunterpurzeln
static void gespenst(Gfx &g, float x, float y, float s, float t, float arme, float fall = 0) {
  float c = cosf(fall), sn = sinf(fall);
  auto R = [&](float dx, float dy, float *o) { o[0] = x + (dx * c - dy * sn) * s; o[1] = y + (dx * sn + dy * c) * s; };
  float pts[2 * 20];
  int n = 0;
  for (int i = 0; i <= 8; i++) {  // Kopf (Halbkreis)
    float a = 3.14159f + i * 3.14159f / 8;
    R(cosf(a) * 20, -40 + sinf(a) * 20, pts + 2 * n); n++;
  }
  R(22, -10, pts + 2 * n); n++;
  for (int i = 0; i < 5; i++) {  // gewellter Saum
    float wx = 20 - i * 10, wy = 4 + ((i + (int)(t * 6)) & 1) * 5;
    R(wx, wy, pts + 2 * n); n++;
  }
  R(-22, -10, pts + 2 * n); n++;
  g.polygon(pts, n, FARBE(0xFFFFFF), 235);
  float a[2], b[2];
  R(-7, -42, a); R(7, -42, b);
  g.ellipse(a[0], a[1], 3.5f * s, 5 * s, SCHWARZ);
  g.ellipse(b[0], b[1], 3.5f * s, 5 * s, SCHWARZ);
  R(0, -30, a);
  g.ellipse(a[0], a[1], 4 * s, 3 * s, SCHWARZ);
  // Arme
  for (int i = -1; i <= 1; i += 2) {
    float w = arme * (0.5f + 0.5f * sinf(t * 8 + i));
    R(i * 18, -26, a);
    R(i * (30 + w * 4), -34 - w * 12, b);
    g.linie(a[0], a[1], b[0], b[1], 7 * s, FARBE(0xFFFFFF), 235);
  }
}

static void glocke(Gfx &g, float x, float y, float s, float schwung) {
  float c = cosf(schwung), sn = sinf(schwung);
  auto R = [&](float dx, float dy, float *o) { o[0] = x + (dx * c - dy * sn) * s; o[1] = y + (dx * sn + dy * c) * s; };
  float p[2 * 8];
  const float form[8][2] = {{-8, 0}, {8, 0}, {14, 20}, {24, 40}, {-24, 40}, {-14, 20}, {-8, 0}, {-8, 0}};
  for (int i = 0; i < 6; i++) R(form[i][0], form[i][1], p + 2 * i);
  g.kontur(1.5f, OUT);
  g.polygon(p, 6, FARBE(0xE0B43C));
  g.konturAus();
  float k[2];
  R(0, 46, k);
  g.kreis(k[0], k[1], 5 * s, FARBE(0x9A7426));
  R(-16, 30, k);
  g.ellipse(k[0], k[1], 3 * s, 7 * s, FARBE(0xFFE28A), 160, schwung);
}

static void fledermaus(Gfx &g, float x, float y, float s, float t) {
  float f = sinf(t * 16) * 5 * s;
  float l[] = {x, y, x - 14 * s, y - 4 * s - f, x - 9 * s, y + 2 * s, x - 5 * s, y + 3 * s};
  float r[] = {x, y, x + 14 * s, y - 4 * s - f, x + 9 * s, y + 2 * s, x + 5 * s, y + 3 * s};
  g.polygon(l, 4, FARBE(0x2A2340));
  g.polygon(r, 4, FARBE(0x2A2340));
  g.kreis(x, y, 4 * s, FARBE(0x2A2340));
  g.kreis(x - 1.5f * s, y - 1, 1 * s, FARBE(0xFFE066));
  g.kreis(x + 1.5f * s, y - 1, 1 * s, FARBE(0xFFE066));
}

static void katze(Gfx &g, float x, float y, float s, float dir, float t, float lauf = -1) {
  const Farbe fell = FARBE(0x2E2A3E);
  auto PX = [&](float dx) { return x + dx * s * dir; };
  auto PY = [&](float dy) { return y + dy * s; };
  // Schwanz
  g.kurve(PX(-10), PY(-8), PX(-22), PY(-14 + sinf(t * 3) * 4), PX(-18), PY(-30), 4 * s, fell);
  if (lauf >= 0) {
    for (int i = 0; i < 4; i++) {
      float sw = sinf(lauf * 6.2831853f + i * 1.6f) * 5;
      g.linie(PX(-8 + (i & 1) * 4 + (i > 1) * 12), PY(-8), PX(-8 + (i & 1) * 4 + (i > 1) * 12 + sw), PY(-1), 3.5f * s, fell);
    }
    g.ellipse(PX(2), PY(-11), 14 * s, 7 * s, fell);
    g.kreis(PX(16), PY(-16), 7 * s, fell);
  } else {
    g.ellipse(PX(0), PY(-10), 10 * s, 11 * s, fell);
    g.kreis(PX(4), PY(-24), 8 * s, fell);
  }
  float kx = lauf >= 0 ? PX(16) : PX(4), ky = lauf >= 0 ? PY(-16) : PY(-24);
  float o1[] = {kx - 7 * s, ky - 3 * s, kx - 6 * s, ky - 12 * s, kx - 1 * s, ky - 6 * s};
  float o2[] = {kx + 7 * s, ky - 3 * s, kx + 6 * s, ky - 12 * s, kx + 1 * s, ky - 6 * s};
  g.polygon(o1, 3, fell);
  g.polygon(o2, 3, fell);
  bool zu = fmodf(t + x * 0.01f, 3.7f) < 0.15f;
  for (int i = -1; i <= 1; i += 2) {
    if (zu) g.linie(kx + i * 3.2f * s - 2 * s, ky, kx + i * 3.2f * s + 2 * s, ky, 1.2f * s, FARBE(0xFFE066));
    else {
      g.ellipse(kx + i * 3.2f * s, ky, 2.4f * s, 2.8f * s, FARBE(0xFFE066));
      g.ellipse(kx + i * 3.2f * s, ky, 0.8f * s, 2.2f * s, SCHWARZ);
    }
  }
  g.kreis(kx, ky + 3 * s, 1 * s, FARBE(0xFF8FB0));
}

static void karte(Gfx &g, float x, float y, float w, Farbe farbe, uint8_t a = 255) {
  g.kontur(0.8f, OUT);
  g.rundRechteck(x - 5, y - 7, 10, 14, 2, WEISS, a);
  g.konturAus();
  g.stern(x, y, 3.2f, 1.6f, 4, w, farbe, a);
}

static void uhr(Gfx &g, float x, float y, float r, float zeit) {
  g.kontur(1.6f, OUT);
  g.rundRechteck(x - r - 6, y - r - 8, 2 * r + 12, 2 * r + 46, 6, FARBE(0x7E4620));
  g.kreis(x, y, r, FARBE(0xFFF3D6));
  g.konturAus();
  for (int i = 0; i < 12; i++) {
    float a = i * 0.5236f;
    g.kreis(x + cosf(a) * r * 0.82f, y + sinf(a) * r * 0.82f, 1.2f, OUT);
  }
  float h = zeit / 12 * 6.2831853f - 1.5708f, m = fmodf(zeit, 1) * 6.2831853f - 1.5708f;
  g.linie(x, y, x + cosf(h) * r * 0.5f, y + sinf(h) * r * 0.5f, 2.5f, OUT);
  g.linie(x, y, x + cosf(m) * r * 0.75f, y + sinf(m) * r * 0.75f, 1.6f, OUT);
  // Pendel
  float p = sinf(zeit * 40) * 0.35f;
  g.linie(x, y + r + 4, x + sinf(p) * 22, y + r + 4 + cosf(p) * 22, 1.5f, GOLD);
  g.kreis(x + sinf(p) * 22, y + r + 4 + cosf(p) * 22, 5, GOLD);
}

static void bett(Gfx &g, float x, float y, float s, float kipp, const Pose &p, bool mitHans = true) {
  float c = cosf(kipp), sn = sinf(kipp);
  auto R = [&](float dx, float dy, float *o) { o[0] = x + (dx * c - dy * sn) * s; o[1] = y + (dx * sn + dy * c) * s; };
  float q[8], a[2];
  // Raeder
  for (int i = -1; i <= 1; i += 2) { R(i * 34, -2, a); g.kreis(a[0], a[1], 5 * s, FARBE(0x5A3A1E)); }
  // Kopfteil
  R(-46, -48, q); R(-38, -48, q + 2); R(-38, -6, q + 4); R(-46, -6, q + 6);
  g.polygon(q, 4, FARBE(0x8E5426));
  R(-44, -8, q); R(44, -8, q + 2); R(44, -20, q + 4); R(-44, -20, q + 6);
  g.polygon(q, 4, FARBE(0xA8652F));
  if (mitHans) {
    // Kopf auf dem Kissen (mit Schlafmuetze)
    R(-28, -26, a);
    Pose pk = p;
    pk.aussehen = &HANS_NACHT;
    g.kontur(1.2f, OUT);
    g.ellipse(a[0] + 2 * s, a[1] + 2 * s, 12 * s, 6 * s, WEISS);  // Kissen
    g.konturAus();
    // nur der Kopf schaut heraus: Figur so setzen, dass der Kopf auf dem Kissen liegt, unten abschneiden
    if (fabsf(kipp) < 0.3f) g.ausschnitt(x - 60 * s, y - 90 * s, 120 * s, 68 * s);
    schneiderlein(g, a[0] + 4 * s, a[1] + 58 * s * 0.6f, 0.6f * s, 1, pk, nullptr, nullptr, 1);
    g.ausschnittAus();
  }
  // Bettdecke
  R(-36, -22, q); R(44, -22, q + 2); R(44, -6, q + 4); R(-36, -6, q + 6);
  g.kontur(1.3f, OUT);
  g.polygon(q, 4, FARBE(0x6EA8E8));
  g.konturAus();
  for (int i = 0; i < 4; i++) { R(-24 + i * 18, -14, a); g.kreis(a[0], a[1], 2.2f * s, WEISS); }
  // Fussteil
  R(38, -36, q); R(46, -36, q + 2); R(46, -6, q + 4); R(38, -6, q + 6);
  g.polygon(q, 4, FARBE(0x8E5426));
}

static void amboss(Gfx &g, float x, float y, float s, float spalt) {
  g.kontur(1.5f, OUT);
  float l[] = {x - 26 * s - spalt, y - 30 * s, x - spalt, y - 30 * s, x - spalt, y - 6 * s, x - 10 * s - spalt, y,
               x - 14 * s - spalt, y - 18 * s, x - 20 * s - spalt, y - 22 * s};
  float r[] = {x + spalt, y - 30 * s, x + 30 * s + spalt, y - 30 * s, x + 18 * s + spalt, y - 22 * s,
               x + 14 * s + spalt, y - 18 * s, x + 10 * s + spalt, y, x + spalt, y - 6 * s};
  g.polygon(l, 6, FARBE(0x5E6270));
  g.polygon(r, 6, FARBE(0x5E6270));
  g.konturAus();
  g.rechteck(x - 20 * s - spalt, y - 30 * s, 18 * s, 3 * s, FARBE(0x8E94A6));
}

static void truhe(Gfx &g, float x, float y, float s, float t, float auf) {
  g.kontur(1.5f, OUT);
  g.rundRechteck(x - 18 * s, y - 20 * s, 36 * s, 20 * s, 3 * s, FARBE(0x8E5426));
  g.konturAus();
  if (auf > 0) {
    for (int i = 0; i < 7; i++) g.kreis(x - 12 * s + i * 4 * s, y - 20 * s - (i & 1) * 3 * s, 4 * s, GOLD);
    for (int i = 0; i < 3; i++) {
      float a = t * 3 + i * 2.1f;
      g.stern(x + cosf(a) * 14 * s, y - 28 * s + sinf(a) * 5 * s, 3 * s, 1.2f * s, 4, a, WEISS, (uint8_t)(255 * auf));
    }
  }
  float dw = auf * 1.2f;
  float d[] = {x - 18 * s, y - 20 * s, x + 18 * s, y - 20 * s, x + 18 * s - sinf(dw) * 2, y - 20 * s - cosf(dw) * 12 * s,
               x - 18 * s + sinf(dw) * 2, y - 20 * s - cosf(dw) * 12 * s};
  g.polygon(d, 4, FARBE(0xA8652F));
  g.rechteck(x - 18 * s, y - 13 * s, 36 * s, 3 * s, GOLD);
  g.kreis(x, y - 12 * s, 2.5f * s, GOLD);
}



static void brrr(Gfx &g, float x, float y, float t, float staerke) {
  // Zitterlinien links und rechts
  for (int i = 0; i < 3; i++) {
    float o = sinf(t * 40 + i) * 2;
    g.linie(x - 26 - i * 5 + o, y - 10 + i * 8, x - 22 - i * 5 + o, y - 2 + i * 8, 1.6f, OUT, (uint8_t)(255 * staerke));
    g.linie(x + 26 + i * 5 - o, y - 10 + i * 8, x + 22 + i * 5 - o, y - 2 + i * 8, 1.6f, OUT, (uint8_t)(255 * staerke));
  }
}

// ---------- weitere Figuren und Dinge nach dem Original ----------
static Aussehen totLook() {  // Gehenkte, halber Mann, toter Vetter: blass, ohne Baeckchen
  Aussehen a;
  a.weste = FARBE(0x7E8696); a.hemd = FARBE(0xCDD3DC); a.hose = FARBE(0x5E6474); a.haar = FARBE(0x4E5260);
  a.haut = FARBE(0xD6DCCB); a.wangen = false; a.muetze = 1;
  return a;
}
static Aussehen fuhrmannLook() {
  Aussehen a;
  a.weste = FARBE(0x6E5A3A); a.hose = FARBE(0x4A3B2A); a.hut = FARBE(0x5A4A3A); a.hutD = FARBE(0x3A2E22);
  a.feder = FARBE(0x5A4A3A); a.haar = FARBE(0x5A3A26);
  return a;
}
static Aussehen wirtLook() {
  Aussehen a;
  a.weste = FARBE(0xFFFFFF); a.hemd = FARBE(0xE8D8B8); a.hose = FARBE(0x5A4636); a.haar = FARBE(0x8A6040); a.muetze = 1;
  return a;
}
static const Aussehen TOT = totLook(), FUHRMANN = fuhrmannLook(), WIRT = wirtLook();

// Sprechblase mit kurzem Text (Bildschirmkoordinaten, Spitze zeigt nach (sx, sy))
static void blase(Gfx &g, float x, float y, const char *txt, float k, float sx, float sy, Farbe f = WEISS) {
  if (k <= 0.05f) return;
  int w = g.textBreite(SCHRIFT_K, txt) + 18;
  float bw = w * k, bh = 26 * k;
  inKreis(x, y, bw, bh);  // rundes Board: ganz in den sichtbaren Kreis
  float sp[] = {x - 6, y + bh / 2 - 2, x + 6, y + bh / 2 - 2, sx, sy};
  g.kontur(1.4f, OUT);
  g.rundRechteck(x - bw / 2, y - bh / 2, bw, bh, 10 * k, f);
  g.konturAus();
  g.polygon(sp, 3, f);
  if (k > 0.8f) { g.text(SCHRIFT_K, txt, x, y + 5, OUT, 1); blaseLesbar = 1; }
}
static float aufzu(float u, float a, float b) { return federnd(seg(u, a, a + 0.4f)) * (1 - seg(u, b - 0.3f, b)); }

static void totenkopf(Gfx &g, float x, float y, float r, float dreh = 0) {
  const Farbe k = FARBE(0xF4F1E6);
  g.kontur(1.2f, OUT);
  g.kreis(x, y, r, k);
  g.konturAus();
  float c = cosf(dreh) * 0.35f;
  g.ellipse(x - r * (0.38f - c * 0.3f), y - r * 0.05f, r * 0.26f, r * 0.32f, FARBE(0x2E2A3E));
  g.ellipse(x + r * (0.38f + c * 0.3f), y - r * 0.05f, r * 0.26f, r * 0.32f, FARBE(0x2E2A3E));
  g.ellipse(x, y + r * 0.32f, r * 0.1f, r * 0.12f, FARBE(0x2E2A3E));
  for (int i = -2; i <= 2; i++) g.linie(x + i * r * 0.16f, y + r * 0.58f, x + i * r * 0.16f, y + r * 0.78f, r * 0.07f, FARBE(0x2E2A3E));
}

static void totenbein(Gfx &g, float x, float y, float s, float umfall) {
  float c = cosf(umfall), sn = sinf(umfall);
  float ox = -sn * 24 * s, oy = -c * 24 * s;  // nach oben
  const Farbe k = FARBE(0xF4F1E6);
  g.kontur(1, OUT);
  g.linie(x, y, x + ox, y + oy, 4 * s, k);
  g.kreis(x - 2.5f * s * c, y, 3 * s, k);
  g.kreis(x + 2.5f * s * c, y, 3 * s, k);
  g.kreis(x + ox - 2.5f * s * c, y + oy, 3 * s, k);
  g.kreis(x + ox + 2.5f * s * c, y + oy, 3 * s, k);
  g.konturAus();
}

static void hund(Gfx &g, float x, float y, float s, float dir, float t, float lauf) {
  const Farbe fell = FARBE(0x26222E);
  auto PX = [&](float dx) { return x + dx * s * dir; };
  auto PY = [&](float dy) { return y + dy * s; };
  for (int i = 0; i < 4; i++) {
    float sw = lauf >= 0 ? sinf(lauf * 6.2831853f + i * 1.6f) * 6 : 0;
    float bx = i < 2 ? -12 + i * 6 : 8 + (i - 2) * 6;
    g.linie(PX(bx), PY(-12), PX(bx + sw), PY(-1), 4 * s, fell);
  }
  g.ellipse(PX(0), PY(-16), 17 * s, 8 * s, fell);
  g.kurve(PX(-16), PY(-18), PX(-24), PY(-26), PX(-22), PY(-32), 3 * s, fell);
  g.kreis(PX(18), PY(-24), 8 * s, fell);
  g.ellipse(PX(26), PY(-21), 6 * s, 4 * s, fell);
  g.ellipse(PX(14), PY(-28), 3 * s, 7 * s, fell, 255, 0.5f * dir);
  g.kreis(PX(20), PY(-26), 2 * s, FARBE(0xFF5030));  // gluehende Augen
  g.kreis(PX(20), PY(-26), 4 * s, FARBE(0xFF5030), 70);
  // gluehende Kette
  for (int i = 0; i < 6; i++) {
    float u = i / 5.0f;
    float cx = lerp(PX(10), PX(-26), u), cy = PY(-18) + sinf(u * 3.14159f) * 10 + sinf(t * 6 + i) * 2;
    g.kreis(cx, cy, 2.6f * s, FARBE(0xFF9A3C));
    g.kreis(cx, cy, 4.5f * s, FARBE(0xFF7A1C), 60);
  }
}

static void schnitzbank(Gfx &g, float x, float y, float zu) {
  g.kontur(1.4f, OUT);
  g.rundRechteck(x - 30, y - 26, 60, 8, 2, HOLZ);
  g.konturAus();
  g.rechteck(x - 26, y - 18, 5, 18, HOLZ_D);
  g.rechteck(x + 21, y - 18, 5, 18, HOLZ_D);
  // Klemme (Schraubstock)
  float o = lerp(10, 2, zu);
  g.rundRechteck(x + 6, y - 40, 6, 16, 2, FARBE(0x8E94A6));
  g.rundRechteck(x + 6 + o + 6, y - 40, 6, 16, 2, FARBE(0x8E94A6));
  g.linie(x + 12 + o + 6, y - 34, x + 30, y - 34, 2, FARBE(0x5E6270));
  g.kreis(x + 30, y - 34, 3, FARBE(0x5E6270));
}

static void drehbank(Gfx &g, float x, float y, float dreh, bool mitKopf) {
  g.kontur(1.4f, OUT);
  g.rundRechteck(x - 34, y - 20, 68, 7, 2, HOLZ);
  g.konturAus();
  g.rechteck(x - 30, y - 13, 5, 13, HOLZ_D);
  g.rechteck(x + 25, y - 13, 5, 13, HOLZ_D);
  g.rundRechteck(x - 30, y - 40, 8, 20, 2, FARBE(0x8E94A6));
  g.rundRechteck(x + 22, y - 40, 8, 20, 2, FARBE(0x8E94A6));
  g.kreis(x - 26, y - 46, 8, FARBE(0x6E4426));  // Rad
  g.linie(x - 26 + cosf(dreh * 6) * 7, y - 46 + sinf(dreh * 6) * 7, x - 26 - cosf(dreh * 6) * 7, y - 46 - sinf(dreh * 6) * 7, 1.5f, OUT);
  if (mitKopf) totenkopf(g, x, y - 32, 10, dreh * 9);
}

static void galgenbaum(Gfx &g, float x, float y, float s) {
  g.kontur(1.8f, OUT);
  g.rundRechteck(x - 8 * s, y - 120 * s, 16 * s, 120 * s, 5 * s, FARBE(0x5E4632));
  g.linie(x, y - 104 * s, x - 108 * s, y - 112 * s, 9 * s, FARBE(0x5E4632));
  g.linie(x, y - 104 * s, x + 108 * s, y - 110 * s, 9 * s, FARBE(0x5E4632));
  g.konturAus();
  for (int i = 0; i < 4; i++) g.kreis(x - 60 * s + i * 40 * s, y - 126 * s - (i & 1) * 8 * s, 18 * s, FARBE(0x3A4A38));
}

static void geldbeutel(Gfx &g, float x, float y, float s) {
  g.kontur(1.3f, OUT);
  g.ellipse(x, y, 9 * s, 8 * s, FARBE(0xB0703A));
  g.konturAus();
  g.linie(x - 4 * s, y - 7 * s, x + 4 * s, y - 7 * s, 2 * s, FARBE(0x6E4426));
  g.kreis(x, y + 1 * s, 3 * s, GOLD);
}

static void sarg(Gfx &g, float x, float y, float deckel) {
  float s[] = {x - 50, y, x + 50, y, x + 56, y - 12, x + 44, y - 24, x - 44, y - 24, x - 56, y - 12};
  g.kontur(1.6f, OUT);
  g.polygon(s, 6, FARBE(0x5A3A26));
  g.konturAus();
  g.rechteck(x - 50, y - 20, 100, 3, FARBE(0x7A5236));
  if (deckel > 0) {  // Deckel liegt drauf (1) oder steht offen daneben (0)
    float d[] = {x - 52, y - 24 - (1 - deckel) * 30, x + 52, y - 24 - (1 - deckel) * 30, x + 46, y - 30 - (1 - deckel) * 30,
                 x - 46, y - 30 - (1 - deckel) * 30};
    g.polygon(d, 4, FARBE(0x6E4426));
  }
}

static void bach(Gfx &g, float t) {
  himmel(g, 0, t, 160);
  sonne(g, 236, 36, 13, t);
  boden(g, 150);
  g.rechteck(0, 176, 284, 40, FARBE(0x6EB8E8));
  for (int i = 0; i < 8; i++) {
    float x = fmodf(i * 40 + t * 18, 320) - 20;
    g.ellipse(x, 186 + (i & 1) * 14, 12, 2, WEISS, 140);
  }
  g.rechteck(0, 216, 284, 24, GRAS);
  for (int i = 0; i < 6; i++) {  // Schilf
    float x = 20 + i * 48;
    g.linie(x, 178, x + sinf(t + i) * 3, 150, 2, GRAS_D);
    g.ellipse(x + sinf(t + i) * 3, 150, 2.5f, 7, FARBE(0x8E5B3A));
  }
}

// =====================================================================
// Kulissen
// =====================================================================
// Mauersteine deckend, Farbe je Reihe passend zum Wandverlauf vorgemischt (schneller als halbdurchsichtig)
static void mauer(Gfx &g, int reihen, Farbe oben, Farbe unten, float hoehe, Farbe stein, uint8_t deck) {
  for (int r = 0; r < reihen; r++) {
    Farbe w = mischeFarbe(oben, unten, (r * 30 + 15) / hoehe);
    Farbe c = mischeFarbe(w, stein, deck / 255.0f);
    for (int k = 0; k < 6; k++) g.rundRechteck(k * 52 - (r & 1) * 26 + 2, r * 30 + 2, 48, 26, 3, c);
  }
}

static void stube(Gfx &g, float t, bool eulenRuf) {
  g.verlauf(0, 180, FARBE(0x6E4E3A), FARBE(0x4E3426));
  g.rechteck(0, 180, 284, 60, FARBE(0x5A3A26));
  for (int i = 0; i < 4; i++) g.rechteck(0, 190 + i * 12, 284, 1.5f, FARBE(0x4A2E1E));
  // Fenster mit Nacht, Mond und Eule
  g.kontur(2, OUT);
  g.rundRechteck(30, 40, 80, 70, 6, HOLZ);
  g.konturAus();
  g.ausschnitt(36, 46, 68, 58);
  g.verlauf(46, 104, FARBE(0x1A2250), FARBE(0x34306E));
  mond(g, 88, 62, 9);
  for (int i = 0; i < 5; i++) g.stern(42 + i * 12, 54 + (i & 1) * 16, 2, 0.8f, 4, 0, WEISS, 200);
  eule(g, 54, 92, 0.8f, t, eulenRuf);
  g.ausschnittAus();
  g.rechteck(68, 46, 4, 58, HOLZ);
  g.rundRechteck(26, 106, 88, 7, 3, HOLZ_D);
  // Tisch mit Kerze
  g.kontur(1.6f, OUT);
  g.rundRechteck(150, 160, 110, 10, 3, HOLZ);
  g.konturAus();
  g.rechteck(160, 170, 8, 40, HOLZ_D);
  g.rechteck(242, 170, 8, 40, HOLZ_D);
  kerze(g, 205, 160, t);
  // warmer Lichtschein
  g.kreis(205, 135, 60, FARBE(0xFFD98A), 22);
}

static void turm(Gfx &g, float t) {
  g.verlauf(0, 240, FARBE(0x3E3A5A), FARBE(0x24223A));
  mauer(g, 8, FARBE(0x3E3A5A), FARBE(0x24223A), 240, FARBE(0x4A4668), 120);
  // Fenster mit Mond
  g.rundRechteck(214, 30, 40, 54, 18, FARBE(0x1A2250));
  mond(g, 234, 52, 8);
  // Treppe
  for (int i = 0; i < 6; i++) g.rechteck(150 + i * 22, 214 - i * 18, 140, 18, FARBE(0x6B6488));
  for (int i = 0; i < 6; i++) g.rechteck(150 + i * 22, 214 - i * 18, 140, 3, FARBE(0x8E88AE));
  g.rechteck(0, 214, 284, 26, FARBE(0x5A5478));
}

static void halle(Gfx &g, float t, float feuer = 1) {
  g.verlauf(0, 200, FARBE(0x3A3550), FARBE(0x2A2638));
  mauer(g, 7, FARBE(0x3A3550), FARBE(0x2A2638), 200, FARBE(0x47415E), 110);
  g.rechteck(0, 200, 284, 40, FARBE(0x4A3E36));
  for (int i = 0; i < 7; i++) g.rechteck(i * 44, 200, 2, 40, FARBE(0x3A302A));
  // Fenster mit Mond
  g.rundRechteck(222, 34, 40, 58, 18, FARBE(0x1A2250));
  mond(g, 242, 56, 8);
  g.linie(242, 34, 242, 92, 2, FARBE(0x3A3550));
  // Kamin
  g.kontur(2, OUT);
  g.rundRechteck(46, 96, 104, 108, 4, FARBE(0x8A8296));
  g.konturAus();
  g.rechteck(64, 20, 68, 78, FARBE(0x7A728A));     // Schornstein
  g.rechteck(40, 92, 116, 10, FARBE(0x9E96AA));    // Sims
  g.rundRechteck(64, 126, 68, 78, 30, FARBE(0x1E1A28));
  if (feuer > 0) {  // Feuer
    for (int i = 0; i < 5; i++) {
      float fx = 74 + i * 12, h = 22 + 8 * sinf(t * 9 + i * 1.7f) + (i == 2) * 10;
      float fl[] = {fx - 8, 200, fx + 8, 200, fx + sinf(t * 7 + i) * 3, 200 - h * feuer};
      g.polygon(fl, 3, FARBE(0xFF8A2A));
      float f2[] = {fx - 4, 200, fx + 4, 200, fx + sinf(t * 8 + i) * 2, 200 - h * 0.6f * feuer};
      g.polygon(f2, 3, FARBE(0xFFD24A));
    }
    g.kreis(98, 180, 70, FARBE(0xFFB050), (uint8_t)(26 * feuer));
  }
  for (int i = 0; i < 3; i++) g.rundRechteck(70 + i * 20, 196, 18, 6, 3, FARBE(0x6E4426));
}

static void wanderland(Gfx &g, float scroll, float t, float abend) {
  himmel(g, abend * 0.5f, t, 200);
  sonne(g, 236, lerp(36, 70, abend), 13, t);
  huegel(g, scroll, 0.3f, 186, 34, 150, mischeFarbe(FARBE(0xA7DE85), FARBE(0x7E8FA8), abend * 0.6f));
  huegel(g, scroll, 0.6f, 200, 30, 110, mischeFarbe(FARBE(0x8FD46A), FARBE(0x6E7E98), abend * 0.6f));
  boden(g, 202, mischeFarbe(GRAS, FARBE(0x5E6E80), abend * 0.6f));
  g.rechteck(0, 212, 284, 10, FARBE(0xE8D3A2));
  for (int i = 0; i < 3; i++) {
    float tx = fmodf(300 + i * 150 - scroll + 4500, 450) - 60;
    laubbaum(g, tx, 206, 0.75f + (i & 1) * 0.15f);
  }
  blumen(g, scroll, 202, 10, 123);
}

// Spukschloss auf dem Huegel
static void spukschloss(Gfx &g, float x, float y, float t, float s = 1) {
  g.ellipse(x, y + 10 * s, 120 * s, 30 * s, FARBE(0x4E4A6A));
  g.kontur(1.8f, OUT);
  g.rechteck(x - 50 * s, y - 50 * s, 100 * s, 50 * s, FARBE(0x6B6488));
  for (int i = -1; i <= 1; i++) {
    float tx = x + i * 44 * s, th = (i == 0 ? 90 : 70) * s;
    g.rechteck(tx - 12 * s, y - th, 24 * s, th, FARBE(0x6B6488));
    float d[] = {tx - 16 * s, y - th, tx, y - th - 30 * s, tx + 16 * s, y - th};
    g.polygon(d, 3, FARBE(0x3E3A5A));
  }
  g.konturAus();
  for (int i = -1; i <= 1; i++) {
    float tx = x + i * 44 * s, th = (i == 0 ? 90 : 70) * s;
    g.rundRechteck(tx - 4 * s, y - th + 18 * s, 8 * s, 12 * s, 4 * s, FARBE(0xFFD24A), (uint8_t)(150 + 100 * sinf(t * 3 + i)));
  }
  g.rundRechteck(x - 12 * s, y - 30 * s, 24 * s, 30 * s, 12 * s, FARBE(0x2A2638));
  for (int i = 0; i < 3; i++) {
    float a = t * 0.9f + i * 2.1f;
    fledermaus(g, x + cosf(a) * 70 * s, y - 100 * s + sinf(a * 1.3f) * 20 * s, 0.8f * s, t + i);
  }
}

// =====================================================================
// Szenen (nach dem Maerchen der Brueder Grimm)
// =====================================================================
static float zitter(float t, float k) { return sinf(t * 47) * 2.2f * k; }

// 1) Zu Hause: der Vater erzaehlt, der Bruder sagt "Es gruselt mir", Hans versteht das nicht
static void szeneStube(Gfx &g, float u) {
  stube(g, u, (u > 7 && u < 8.2f) || (u > 9 && u < 10.2f));
  Pose v = pose(VATER);
  v.armV = 1.4f + (u < 7 ? sinf(u * 3) * 0.3f : 0); v.mund = u < 7 && fmodf(u, 0.5f) < 0.25f ? 0.4f : 0;
  v.zu = fmodf(u, 4.4f) < 0.15f;
  if (u > 15 && u < 20) { v.boese = 0.8f; v.armV = 2.5f + sinf(u * 6) * 0.2f; v.mund = 0.6f; }  // schimpft
  schneiderlein(g, 232, 210, 1.05f, -1, v);
  bart(g, 232, 210, 1.05f, -1);
  float angst = seg(u, 7.2f, 7.6f) * (1 - seg(u, 12.5f, 13.5f));
  Pose b = pose(BRUDER);
  b.mund = angst > 0.3f ? 0.9f : 0; b.armV = angst > 0.3f ? 2.7f : 0.4f; b.armH = angst > 0.3f ? 2.7f : 0.3f;
  b.blick = -1;
  schneiderlein(g, 150 + zitter(u, angst), 214, 0.95f, -1, b);
  if (angst > 0.3f) brrr(g, 150, 160, u, angst);
  Pose h = pose(HANS);
  h.blick = -0.5f; h.zu = fmodf(u + 1, 3.6f) < 0.12f;
  if (u > 12 && u < 15) { h.mund = 0.4f; h.armV = 1.2f; h.armH = 1.2f; }
  if (u > 20 && u < 24) { h.mund = 0.6f; h.armV = 2.5f; }
  schneiderlein(g, 80, 214, 0.95f, 1, h);
  // Der Vater erzaehlt eine Gruselgeschichte
  if (u > 1 && u < 6.5f) {
    float k = aufzu(u, 1, 6.5f);
    g.kontur(1.4f, OUT);
    g.ellipse(196, 62, 30 * k, 22 * k, FARBE(0xCFE0FF));
    g.konturAus();
    if (k > 0.6f) gespenst(g, 196, 80, 0.42f, u, 1);
  }
  blase(g, 120, 104, "Es gruselt mir!", aufzu(u, 8, 12.5f), 146, 124);
  if (u > 12.5f && u < 15.5f) {  // Hans: was ist das?
    float k = aufzu(u, 12.5f, 15.5f);
    g.kontur(1.4f, OUT);
    g.ellipse(70, 70, 26 * k, 18 * k, WEISS);
    g.konturAus();
    if (k > 0.6f) { brrr(g, 70, 72, u, 1); g.text(SCHRIFT_M, "?", 70, 77, OUT, 1); }
  }
  blase(g, 200, 50, "Lern endlich was!", aufzu(u, 16, 20), 222, 120);
  blase(g, 86, 104, "Gruseln lernen!", aufzu(u, 20.5f, 24.5f), 84, 130);
  // der Kuester kommt und will es ihm beibringen
  if (u > 24) {
    Pose k = pose(KUESTER);
    k.armV = 2.2f;
    schneiderlein(g, lerp(-30, 26, sanft(seg(u, 24, 26))), 214, 0.9f, 1, k);
    if (u > 26) {
      float kk = aufzu(u, 26, 29.4f);
      g.kontur(1.4f, OUT);
      g.ellipse(46, 104, 22 * kk, 18 * kk, WEISS);
      g.konturAus();
      if (kk > 0.6f) glocke(g, 46, 92, 0.4f, sinf(u * 6) * 0.4f);
    }
  }
  blende(g, u, 30, 140, 140);
}

// 2) Glockenturm: der Kuester als weisses Gespenst; "Wer da?"; Hans stoesst ihn die Treppe hinab
static void szeneTurm(Gfx &g, float u) {
  if (u >= 19) {  // wieder zu Hause: der Vater gibt ihm 50 Taler und schickt ihn fort
    stube(g, u, false);
    Pose v = pose(VATER);
    v.boese = 0.6f;
    v.armV = u < 23 ? 1.5f : 2.4f;  // reicht den Beutel, zeigt dann zur Tuer
    float vx, vy;
    schneiderlein(g, 200, 214, 1.05f, -1, v, &vx, &vy);
    bart(g, 200, 214, 1.05f, -1);
    Pose h = pose(HANS);
    h.mund = 0.3f;
    float hx = u < 24 ? 140 : lerp(140, -40, seg(u, 24, 28));
    h.lauf = u > 24 ? fmodf(u * 1.7f, 1) : -1;
    h.armV = 1.3f;
    float bx, by;
    schneiderlein(g, hx, 214, 0.95f, u > 24 ? -1 : 1, h, &bx, &by);
    if (u < 21.5f) geldbeutel(g, vx, vy + 4, 1);
    else geldbeutel(g, bx, by + 4, 1);
    blase(g, 196, 70, "50 Taler. Geh!", aufzu(u, 19.5f, 23.5f), 196, 104);
    blende(g, u, 30, 140, 140);
    return;
  }
  turm(g, u);
  float schwung = u < 6 ? sinf(u * 4.5f) * 0.4f * (1 - seg(u, 4, 6)) : 0;
  g.linie(90, 0, 90, 36, 4, FARBE(0x6E4426));
  glocke(g, 90, 36, 1.2f, schwung);
  float seilY = 90 + (u < 5 ? fabsf(sinf(u * 4.5f)) * 14 : 0);
  g.linie(90, 92, 90, seilY + 60, 2.5f, FARBE(0xC9A46A));
  Pose h = pose(HANS);
  h.zu = fmodf(u + 0.4f, 3.3f) < 0.12f;
  float hx = 76;
  if (u < 5) { h.armV = 2.8f; h.armH = 2.6f; }
  if (u > 12.5f) { hx = lerp(76, 168, sanft(seg(u, 12.5f, 14))); h.lauf = u < 14 ? fmodf(u * 2, 1) : -1; }
  if (u > 14 && u < 15) h.armV = lerp(1.0f, 1.9f, seg(u, 14, 14.4f));  // stoesst
  schneiderlein(g, hx, 214, 0.95f, 1, h);
  // das Gespenst steht regungslos oben auf der Treppe
  float gx = 196, gy = 142, fall = 0;
  if (u < 3) gx = 400;
  else if (u > 14.4f) {
    float k = seg(u, 14.4f, 18);
    gx = lerp(196, 250, k);
    gy = lerp(142, 210, k) - fabsf(sinf(k * 12)) * 10;
    fall = k * 14;
  }
  if (u < 18.2f) gespenst(g, gx, gy, 0.85f, u, 0, fall);
  else {  // da liegt der Kuester und hat ein Bein gebrochen
    Pose k = pose(KUESTER);
    k.zu = true; k.mund = 0.6f;
    schneiderlein(g, 250, 214, 0.8f, -1, k);
    g.rundRechteck(244, 194, 10, 14, 3, WEISS);  // Verband
    g.ellipse(268, 214, 22, 5, WEISS, 230);
    for (int i = 0; i < 4; i++) {
      float a = u * 4 + i * 1.57f;
      g.stern(250 + cosf(a) * 16, 128 + sinf(a) * 5, 3, 1.3f, 5, a, GOLD);
    }
  }
  if (u > 17.8f && u < 18.2f) g.wackeln(sinf(u * 90) * 4, cosf(u * 80) * 3);
  blase(g, 78, 108, "Wer da?", aufzu(u, 5, 7), 80, 128);
  blase(g, 78, 108, "Wer da?!", aufzu(u, 7.6f, 9.6f), 80, 128);
  blase(g, 78, 108, "Antworte!", aufzu(u, 10.2f, 12.2f), 80, 128);
  blende(g, u, 30, 110, 150);
}

// 3) Unter dem Galgen: die sieben Gehenkten ans Feuer gesetzt, die Kleider brennen
static void gehenkter(Gfx &g, float x, float y, float s, bool brennt, float t) {
  Pose p = pose(TOT);
  p.zu = true; p.armV = 0.1f; p.armH = 0.1f;
  schneiderlein(g, x, y, s, 1, p);
  if (brennt)
    for (int i = 0; i < 2; i++) {
      float fx = x + (i ? 6 : -6) * s, fy = y - 14 * s, h = (12 + 5 * sinf(t * 11 + i + x)) * s;
      float f[] = {fx - 5 * s, fy, fx + 5 * s, fy, fx + sinf(t * 8 + i) * 2, fy - h};
      g.polygon(f, 3, FARBE(0xFF8A2A));
      float f2[] = {fx - 2.5f * s, fy, fx + 2.5f * s, fy, fx, fy - h * 0.6f};
      g.polygon(f2, 3, FARBE(0xFFD24A));
    }
}

static void szeneGalgen(Gfx &g, float u) {
  float nacht = seg(u, 8, 10.5f) * (1 - seg(u, 26.5f, 29));
  himmel(g, nacht, u, 200);
  if (nacht > 0.4f) mond(g, 238, 40, 13);
  else sonne(g, 238, lerp(36, 90, seg(u, 0, 8)), 13, u);
  huegel(g, 0, 0, 190, 34, 140, mischeFarbe(FARBE(0xA7DE85), FARBE(0x3E4A60), nacht));
  boden(g, 202, mischeFarbe(GRAS, FARBE(0x3E5A4A), nacht));
  g.rechteck(0, 212, 284, 10, mischeFarbe(FARBE(0xE8D3A2), FARBE(0x6E6A70), nacht));
  galgenbaum(g, 150, 206, 1);
  // Feuer
  float feuer = seg(u, 9.5f, 10.5f) * (1 - seg(u, 27, 28.5f));
  if (feuer > 0) {
    for (int i = 0; i < 3; i++) g.rundRechteck(134 + i * 10, 208, 14, 5, 2, FARBE(0x6E4426));
    for (int i = 0; i < 4; i++) {
      float fx = 138 + i * 8, h = (16 + 6 * sinf(u * 9 + i * 1.7f)) * feuer;
      float f[] = {fx - 6, 210, fx + 6, 210, fx + sinf(u * 7 + i) * 2, 210 - h};
      g.polygon(f, 3, FARBE(0xFF8A2A));
    }
    g.kreis(150, 196, 50, FARBE(0xFFB050), (uint8_t)(30 * feuer));
  }
  // die sieben: haengen, werden ans Feuer gesetzt (11-15), brennen (15-20), wieder aufgehaengt (20-24)
  float unten = sanft(seg(u, 11, 15)) * (1 - sanft(seg(u, 20, 24)));
  for (int i = 0; i < 7; i++) {
    float hx = 150 + (i - 3) * 28 + sinf(u * 1.1f + i) * 2 * (1 - unten), hy = 160;
    float sx = 150 + (i - 3) * 30 + (i == 3 ? 0 : 0), sy = 214;
    if (i == 3) sx = 150, sy = 222;  // sitzt hinter dem Feuer
    float x = lerp(hx, sx, unten), y = lerp(hy, sy, unten);
    if (unten < 0.05f) g.linie(hx, 100, hx, 110, 1.4f, FARBE(0x9A8A6A));
    gehenkter(g, x, y, lerp(0.5f, 0.55f, unten), u > 15.5f && u < 21 && (i & 1), u);
  }
  // Hans
  Pose h = pose(HANS);
  h.ranzen = true;
  h.zu = fmodf(u + 0.4f, 3.4f) < 0.12f;
  float hx = 80;
  if (u < 3) { hx = lerp(-30, 80, seg(u, 0, 3)); h.lauf = fmodf(u * 1.7f, 1); }
  if (u > 17 && u < 20) { h.boese = 0.8f; h.mund = 0.6f; h.armV = 2.5f + sinf(u * 8) * 0.3f; }
  if (u > 24 && u < 28) h.zu = true;  // schlaeft
  if (u > 33) { hx = lerp(80, 320, seg(u, 33, 36)); h.lauf = fmodf(u * 1.7f, 1); }
  schneiderlein(g, hx, 216, 0.85f, 1, h);
  if (u > 24 && u < 28) zzz(g, hx + 6, 130, u);
  // der Fuhrmann
  Pose f = pose(FUHRMANN);
  float fx = -60;
  if (u < 8) { fx = lerp(-60, 30, seg(u, 0, 3)); f.armV = u > 3 ? 2.2f : 0.4f; f.lauf = u < 3 ? fmodf(u * 1.7f, 1) : -1; }
  if (u > 6 && u < 9) { fx = lerp(30, -60, seg(u, 6.5f, 9)); f.lauf = fmodf(u * 1.7f, 1); }
  if (u > 28.5f && u < 34) { fx = lerp(-60, 30, seg(u, 28.5f, 30.5f)); f.mund = 0.5f; }
  if (fx > -50) schneiderlein(g, fx, 216, 0.9f, 1, f);
  blase(g, 64, 104, "Setz dich drunter!", aufzu(u, 3.2f, 6.6f), 40, 128);
  blase(g, 130, 96, "Passt doch auf!", aufzu(u, 17, 20.2f), 92, 128);
  blase(g, 70, 104, "Na, gruselt's?", aufzu(u, 30.5f, 32.5f), 40, 128);
  if (u > 31.5f && u < 33.5f) fragezeichen(g, 98, 120, 1, (uint8_t)(255 * seg(u, 31.5f, 31.9f)));
  blende(g, u, 36, 150, 150);
}

// 4) Wirtshaus und Koenig: das verwunschene Schloss, drei Naechte, Feuer, Drehbank und Schnitzbank
static void szeneWirtshaus(Gfx &g, float u) {
  if (u < 9) {
    wanderland(g, 0, u, 0.6f);
    spukschloss(g, 232, 150, u, 0.6f);
    g.kontur(1.8f, OUT);  // Wirtshaus
    g.rechteck(10, 120, 100, 86, FARBE(0xF1DDB0));
    float d[] = {0, 122, 60, 84, 120, 122};
    g.polygon(d, 3, FARBE(0x9A5A3A));
    g.konturAus();
    g.rundRechteck(48, 160, 24, 46, 10, HOLZ);
    g.rundRechteck(20, 136, 18, 16, 2, FARBE(0xFFD24A));
    g.rundRechteck(82, 136, 18, 16, 2, FARBE(0xFFD24A));
    g.linie(110, 126, 126, 126, 2, OUT);
    g.rundRechteck(118, 128, 18, 14, 3, HOLZ);  // Schild mit Krug
    Pose w = pose(WIRT);
    w.armV = u > 1.5f ? 2.2f : 0.4f; w.mund = fmodf(u, 0.5f) < 0.25f ? 0.4f : 0;
    schneiderlein(g, 60, 214, 0.95f, 1, w);
    Pose h = pose(HANS);
    h.ranzen = true;
    h.mund = u > 6 ? 0.7f : 0;
    schneiderlein(g, 140, 216, 0.9f, -1, h);
    float k = aufzu(u, 1.5f, 8.5f);
    if (k > 0.05f) {
      g.kontur(1.4f, OUT);
      g.rundRechteck(80 - 50 * k, 40 - 28 * k, 100 * k, 56 * k, 10 * k, FARBE(0xFBF1DC));
      g.konturAus();
      if (k > 0.8f) {
        spukschloss(g, 58, 62, u, 0.25f);
        for (int i = 0; i < 3; i++) g.text(SCHRIFT_M, "|", 84 + i * 6, 52, OUT);
        truhe(g, 108, 62, 0.45f, u, 1);
      }
    }
  } else {
    himmel(g, 0, u, 200);
    sonne(g, 240, 36, 14, u);
    schloss(g, 150, 196, u, 0);
    boden(g, 196);
    koenig(g, 196, 214, 0.9f, -1, u > 15 ? u : 0);
    Pose h = pose(HANS);
    h.ranzen = true;
    h.armV = 1.6f;
    schneiderlein(g, 110, 216, 0.9f, 1, h);
    float k = aufzu(u, 10, 17.6f);
    if (k > 0.05f) {  // er bittet um Feuer, eine Drehbank und eine Schnitzbank mit Messer
      g.kontur(1.4f, OUT);
      g.rundRechteck(70 - 64 * k, 30, 128 * k, 50 * k, 10 * k, WEISS);
      g.konturAus();
      if (k > 0.8f) {
        for (int i = 0; i < 3; i++) {
          float f[] = {22 + i * 7.0f, 68, 30 + i * 7.0f, 68, 26 + i * 7.0f, 52};
          g.polygon(f, 3, FARBE(0xFF8A2A));
        }
        drehbank(g, 70, 76, u, false);
        schnitzbank(g, 114, 76, 0);
      }
    }
  }
  blende(g, u, 18, 140, 140);
}

// 5) Erste Nacht: Katzen ("Miau, uns friert"), Pfoten in der Schnitzbank, Katzen und Hunde, das Bett faehrt
static void szeneNacht1(Gfx &g, float u) {
  halle(g, u);
  schnitzbank(g, 206, 214, seg(u, 17.5f, 18.2f) * (1 - seg(u, 20, 20.3f)));
  bool imBett = u > 31;
  if (!imBett) {
    Pose h = pose(HANS);
    h.zu = fmodf(u + 0.3f, 3.7f) < 0.12f;
    h.armV = 1.2f;
    float hx = 160;
    if (u > 9 && u < 15) { h.armV = 1.4f + sinf(u * 3) * 0.3f; }
    if (u > 17 && u < 23) { hx = 186; h.armV = u > 20 ? 2.7f : 1.6f; }
    if (u > 24 && u < 31) { h.armV = 2.4f + sinf(u * 14) * 0.6f; h.boese = 0.6f; h.mund = 0.7f; }  // haut um sich
    float mx, my;
    schneiderlein(g, hx, 214, 0.95f, -1, h, &mx, &my);
    if (u > 24 && u < 31) g.linie(mx, my, mx - 14 * cosf(u * 14), my - 14, 2.5f, FARBE(0xC7D0DC));  // Messer
  }
  // zwei Katzen
  for (int i = 0; i < 2; i++) {
    if (u > 23.5f) break;
    float kx = 100 - i * 26, ky = 214, lauf = -1;
    if (u < 6) { kx = lerp(-30 - i * 20, kx, sanft(seg(u, 2 + i, 6))); lauf = fmodf(u * 2, 1); }
    if (u > 17.5f && u < 20.3f) { kx = 194 + i * 16; ky = 190; }  // Pfoten in der Schnitzbank
    if (u > 20.3f) {  // zum Fenster hinaus in den Graben
      float k = seg(u, 20.3f, 22.3f);
      kx = lerp(194 + i * 16, 242, k);
      ky = lerp(190, 64, k) - sinf(k * 3.14159f) * 40;
      lauf = 0.3f;
    }
    katze(g, kx, ky, 1, 1, u + i, lauf);
  }
  if (u > 22.2f && u < 24)  // Platsch im Graben vor dem Fenster
    for (int i = 0; i < 2; i++) {
      float r = 4 + seg(u, 22.2f, 24) * 14 + i * 5;
      g.bogen(242, 80, r, 0, 3.14159f, 1.5f, FARBE(0x8FD8FF), (uint8_t)(255 * (1 - seg(u, 22.2f, 24))));
    }
  // Karten
  if (u > 9 && u < 17)
    for (int i = 0; i < 5; i++) karte(g, 112 + i * 9, 216, u + i, i & 1 ? FARBE(0xE0473A) : OUT);
  // viele schwarze Katzen und Hunde mit gluehenden Ketten
  if (u > 24 && u < 31)
    for (int i = 0; i < 6; i++) {
      float an = 24 + i * 0.3f;
      float zx = 40 + i * 40;
      float x = i & 1 ? lerp(330, zx, sanft(seg(u, an, an + 1.5f))) : lerp(-50, zx, sanft(seg(u, an, an + 1.5f)));
      float dir = i & 1 ? -1 : 1;
      if (u > 28.5f) { x = lerp(zx, i & 1 ? 340 : -60, seg(u, 28.5f + i * 0.1f, 30.5f)); dir = -dir; }  // fliehen
      if (i % 3 == 0) hund(g, x, 214, 1, dir, u, fmodf(u * 2, 1));
      else katze(g, x, 214, 1.1f, dir, u + i, fmodf(u * 2, 1));
    }
  // das Bett faehrt durchs ganze Schloss und kippt um
  if (imBett) {
    float bx = 150, by = 214, kipp = 0;
    if (u > 32 && u < 38) { float k = (u - 32) / 6; bx = 142 + sinf(k * 6.2831853f) * 90; by = 214 - fabsf(sinf(u * 6)) * 4; kipp = sinf(u * 5) * 0.06f; }
    if (u >= 38) { float k = sanft(seg(u, 38, 39.4f)); bx = lerp(142, 70, k); kipp = 3 * k; by = 214 - sinf(k * 3.14159f) * 36 - 30 * k; }
    Pose p;
    p.zu = u < 38 && fmodf(u, 2) < 1;
    p.mund = 0.7f;
    bett(g, bx, by, 1, kipp, p, u < 38.5f);
    if (u > 39.4f) {
      Pose h = pose(HANS_NACHT);
      h.mund = 0.6f; h.zu = true;
      schneiderlein(g, lerp(80, 150, seg(u, 39.4f, 40.6f)), 214, 0.9f, 1, h);
    }
  }
  blase(g, 92, 150, "Miau, uns friert!", aufzu(u, 6.2f, 9), 92, 186);
  blase(g, 150, 110, "Zeigt mal eure Pfoten!", aufzu(u, 14.6f, 17.4f), 160, 130);
  blende(g, u, 42, 150, 150);
}

// halber Mann: obere oder untere Haelfte (Schnitt am Guertel)
static void halberMann(Gfx &g, float x, float y, float s, bool oben, float t, float dir = 1) {
  Pose p = pose(TOT);
  p.zu = fmodf(t + x, 3) < 0.15f;
  p.armV = 0.8f + sinf(t * 3) * 0.2f;
  float schnitt = y - 26 * s;
  if (oben) g.ausschnitt(x - 40, schnitt - 120, 80, 120);
  else g.ausschnitt(x - 40, schnitt, 80, 60);
  schneiderlein(g, x, y, s, dir, p);
  g.ausschnittAus();
}

// 6) Zweite Nacht: ein halber Mann faellt den Schornstein herab, dann die andere Haelfte; Kegeln mit Totenbeinen
static void szeneNacht2(Gfx &g, float u) {
  halle(g, u);
  uhr(g, 180, 58, 13, 11 + seg(u, 0, 30) * 0.98f + (u > 30 ? 0.02f : 0));
  drehbank(g, 196, 214, u, u > 13 && u < 19);
  bool da = u < 31;
  // die Haelften
  if (da && u > 2) {
    float oy = u < 3.4f ? lerp(-60, 200, hinein(seg(u, 2, 3.4f))) : 200;
    float uy = u < 5.6f ? lerp(-60, 200, hinein(seg(u, 4.4f, 5.6f))) : 200;
    float mx = lerp(98, 236, sanft(seg(u, 6, 8.5f)));
    float ox = u > 6 ? mx : 92, ux = u > 6 ? mx : 108;
    if (u > 4.4f) halberMann(g, ux, uy + 14, 0.9f, false, u);
    halberMann(g, ox, oy + 14 - (u > 8.5f ? 0 : 0), 0.9f, true, u);
    if (u > 8.5f && u < 9.5f)
      for (int i = 0; i < 4; i++) g.stern(mx + (i - 1.5f) * 10, 186, 4, 1.6f, 5, u * 4, GOLD);
  }
  // weitere kommen mit neun Totenbeinen und zwei Totenkoepfen
  if (da && u > 9) {
    for (int i = 0; i < 2; i++) {
      Pose p = pose(TOT);
      p.armV = 1.6f;
      float x = lerp(330 + i * 30, 262 - i * 2, sanft(seg(u, 9, 11.5f)));
      schneiderlein(g, x, 214, 0.85f, -1, p);
    }
    for (int i = 0; i < 9; i++) {  // aufgestellte Beine
      int r = i < 3 ? 0 : i < 6 ? 1 : 2;
      float bx = 236 + (i % 3) * 8 + r * 4, by = 214 - r * 3;
      float um = 0;
      float w1 = 21.6f, w2 = 27.6f;
      if (u > w1 && u < 24) um = sanft(seg(u, w1 + (i % 4) * 0.06f, w1 + 0.5f)) * (i & 1 ? 1.4f : -1.4f);
      if (u > w2 && u < 30) um = sanft(seg(u, w2 + (i % 3) * 0.06f, w2 + 0.5f)) * (i & 1 ? -1.4f : 1.4f) * (i < 6);
      if (u > 12 || i < 4) totenbein(g, bx + um * 6, by, 0.7f, um);
    }
  }
  // Totenkoepfe: Hans dreht sie an der Drehbank rund, dann wird gekegelt
  if (da && u > 11) {
    float kx = 214, ky = 206;
    if (u > 13 && u < 19) kx = -100;  // gerade in der Drehbank
    if (u > 19 && u < 21.6f) { float k = seg(u, 20, 21.6f); kx = lerp(170, 240, k); }
    if (u > 21.6f && u < 26) kx = 160;
    if (u > 26 && u < 27.6f) { float k = seg(u, 26, 27.6f); kx = lerp(250, 238, k); kx = lerp(268, 236, k); }
    if (kx > 0) totenkopf(g, kx, ky, 7, u * 8 * (u > 20 && u < 27.6f));
  }
  Pose h = pose(HANS);
  h.zu = fmodf(u + 0.6f, 3.4f) < 0.12f;
  float hx = 150;
  if (u > 13 && u < 19) { hx = 168; h.armV = 1.5f + sinf(u * 9) * 0.3f; }  // dreht an der Drehbank
  if (u > 19 && u < 20.2f) h.armV = lerp(0.4f, 1.6f, seg(u, 19, 20));
  if (u > 21.6f && u < 24) { h.armV = 2.7f; h.armH = 2.7f; h.mund = 0.8f; h.zu = true; }
  schneiderlein(g, hx, 214, 0.95f, 1, h);
  if (u > 13 && u < 19)
    for (int i = 0; i < 4; i++) {  // Spaene
      float k = fmodf(u * 2 + i * 0.25f, 1);
      g.kreis(196 + k * 20, 184 - sinf(k * 3.14f) * 14, 1.5f, FARBE(0xF4F1E6), (uint8_t)(255 * (1 - k)));
    }
  if (u > 30.5f && u < 32)  // um Mitternacht ist alles verschwunden
    for (int i = 0; i < 8; i++) {
      float a = i * 0.785f, k = seg(u, 30.5f, 32);
      g.stern(240 + cosf(a) * k * 50, 170 + sinf(a) * k * 40, 6, 2.5f, 5, a, GOLD, (uint8_t)(255 * (1 - k)));
    }
  blase(g, 150, 110, "Ich spiel mit!", aufzu(u, 11.6f, 13.6f), 152, 130);
  blende(g, u, 38, 150, 150);
}

// 7) Dritte Nacht: der tote Vetter im Sarg, dann der Alte mit dem Bart, Amboss, Eisenstange, drei Kisten Gold
static void szeneNacht3(Gfx &g, float u) {
  halle(g, u, 0.8f);
  // sechs Maenner tragen einen Sarg herein (drei sieht man)
  float sx = 150, sy = 214;
  bool traegt = u < 6 || (u > 19 && u < 22.5f);
  if (u < 6) sx = lerp(360, 150, sanft(seg(u, 0, 5)));
  if (u > 19) sx = lerp(150, 400, seg(u, 19, 22.5f));
  if (traegt) {
    for (int i = 0; i < 3; i++) {
      Pose p = pose(TOT);
      p.armV = 2.9f; p.armH = 2.9f;
      p.lauf = fmodf(u * 1.5f + i * 0.3f, 1);
      schneiderlein(g, sx - 40 + i * 40, 214, 0.85f, -1, p);
    }
    sarg(g, sx, 150, 1);
  } else if (u < 22.5f) {
    float deckel = u < 8 ? 1 : u < 9 ? 1 - seg(u, 8, 9) : u < 17.5f ? 0 : seg(u, 17.5f, 18.2f);
    // der Vetter liegt darin; spaeter richtet er sich auf
    float auf = sanft(seg(u, 14, 15.2f)) * (1 - sanft(seg(u, 17.2f, 17.8f)));
    if (deckel < 1) {
      Pose v = pose(TOT);
      v.zu = auf < 0.5f;
      v.armV = 1.6f * auf; v.armH = 1.6f * auf; v.boese = auf; v.mund = auf * 0.6f;
      g.ausschnitt(60, 0, 140, sy - 24);
      schneiderlein(g, 116, sy + 24 - auf * 40, 0.85f, 1, v);
      g.ausschnittAus();
    }
    sarg(g, sx, sy, deckel);
  }
  // Hans
  Pose h = pose(HANS);
  h.zu = fmodf(u + 0.3f, 3.6f) < 0.12f;
  float hx = 60, hdir = 1;
  if (u > 9 && u < 14) { hx = 80; h.armV = 1.4f + sinf(u * 9) * 0.2f; h.armH = 1.4f - sinf(u * 9) * 0.2f; }  // waermt ihn
  if (u > 14.2f && u < 16.8f) { h.mund = 0.7f; h.armV = 2.4f; }
  if (u > 16.8f && u < 18) { hx = 92; h.armV = 1.8f; h.boese = 0.7f; }  // legt ihn zurueck
  if (u > 22.5f) hx = 120;
  if (u > 26 && u < 28.6f) { h.armV = lerp(3.0f, 1.6f, seg(u, 28, 28.6f)); }  // Axt
  if (u > 29.5f && u < 34) { h.armV = 1.2f + fabsf(sinf(u * 9)) * 1.6f; h.boese = 0.6f; }  // Eisenstange
  if (u > 37) { h.armV = 2.7f; h.armH = 2.7f; h.mund = 0.8f; }
  float ax, ay;
  schneiderlein(g, hx, 214, 0.95f, hdir, h, &ax, &ay);
  if (u > 9 && u < 14) for (int i = 0; i < 3; i++) g.bogen(ax + 6, ay - 8 - i * 6, 5 + i * 3, -2.4f, -0.8f, 1.5f, FARBE(0xFF9A3C), 160);
  if (u > 26 && u < 28.8f) {
    g.linie(ax, ay, ax + 10, ay - 22, 3, HOLZ);
    float a2[] = {ax + 6, ay - 26, ax + 18, ay - 22, ax + 14, ay - 14};
    g.polygon(a2, 3, FARBE(0xC7D0DC));
  }
  if (u > 29.5f && u < 34) g.linie(ax, ay, ax + 26 * cosf(u * 9), ay - 20, 3.5f, FARBE(0x5E6270));  // Eisenstange
  // der Alte
  if (u > 22.5f && u < 46) {
    RiesePose r;
    r.farbe = 2;
    r.armV = 0.6f;
    float x = lerp(330, 236, sanft(seg(u, 22.5f, 24.5f)));
    if (u > 24.5f && u < 26.5f) { r.boese = 1; r.mund = 0.7f; r.armV = 2.5f; }
    if (u > 28.6f && u < 36) { r.staunt = 1; r.mund = 0.9f; r.kopf = 0.45f; }
    if (u > 36) { r.mund = 0.3f; r.armV = 1.4f; }
    riese(g, x, 214, 0.8f, -1, r);
    if (u > 25.5f) {
      float spalt = u > 28.4f ? lerp(0, 4, seg(u, 28.4f, 28.6f)) : 0;
      if (u > 36) spalt = lerp(4, 0, seg(u, 36, 36.4f));
      amboss(g, 182, 214, 1, spalt);
      if (u > 28.6f && u < 36.2f) g.kurve(212, 112, 202, 160, 184, 186, 7, FARBE(0xF4F6FA));
    }
  }
  if (u > 28.4f && u < 28.8f) { g.wackeln(sinf(u * 90) * 5, cosf(u * 80) * 4); g.stern(182, 186, 16, 6, 7, u * 3, GOLD); }
  if (u > 36.5f) for (int i = 0; i < 3; i++) truhe(g, 22 + i * 34, 214, 0.75f, u + i, seg(u, 37 + i * 0.4f, 38 + i * 0.4f));
  if (u > 45 && u < 46.5f)
    for (int i = 0; i < 8; i++) {
      float a = i * 0.785f, k = seg(u, 45, 46.5f);
      g.stern(236 + cosf(a) * k * 60, 140 + sinf(a) * k * 50, 6, 2.5f, 5, a, GOLD, (uint8_t)(255 * (1 - k)));
    }
  if (u > 46) g.kreis(242, 60, 30, FARBE(0xFFE9A0), (uint8_t)(120 * seg(u, 46, 48)));
  blase(g, 96, 110, "Ich wärm dich!", aufzu(u, 9.4f, 13.4f), 90, 130);
  blase(g, 150, 100, "Jetzt erwürg ich dich!", aufzu(u, 15, 17.2f), 120, 140);
  blase(g, 200, 50, "Du sollst sterben!", aufzu(u, 24.6f, 26.8f), 226, 90);
  blase(g, 200, 50, "Hör auf, du kriegst Gold!", aufzu(u, 32.4f, 35.6f), 226, 90);
  blende(g, u, 50, 150, 150);
}

// 8) Hochzeit - aber gruseln kann er sich noch immer nicht
static void szeneHochzeit2(Gfx &g, float u) {
  himmel(g, 0, u, 200);
  sonne(g, 240, 36, 14, u);
  schloss(g, 142, 196, u, 0);
  boden(g, 196);
  float tep[] = {120, 196, 164, 196, 200, 240, 84, 240};
  g.polygon(tep, 4, FARBE(0xD8283F));
  koenig(g, 70, 214, 0.9f, 1, u > 2 && u < 6 ? u : 0);
  for (int i = 0; i < 3; i++) truhe(g, 34 + i * 26, 236, 0.55f, u + i, 1);
  prinzessin(g, lerp(214, 166, seg(u, 4, 6)), 214, 0.9f, -1, u > 10 ? u : 0);
  Pose h = pose(HANS);
  h.mund = 0.4f;
  h.zu = u > 7 && u < 11;
  h.armV = u > 10 && u < 13 ? 2.6f + sinf(u * 9) * 0.3f : 0.5f;
  if (u > 13) { h.mund = 0; h.blick = -1; }
  schneiderlein(g, 128, 214, 0.9f, 1, h);
  if (u > 6 && u < 13)
    for (int i = 0; i < 6; i++) {
      float k = fmodf((u - 6) * 0.45f + i / 6.0f, 1);
      herz(g, 146 + sinf(k * 7 + i) * 24, 150 - k * 110, 5 + (i % 3), FARBE(0xFF5A7A), (uint8_t)(255 * huepf(k)));
    }
  blase(g, 110, 104, "Wenn mir nur gruselte!", aufzu(u, 13.2f, 17), 124, 130);
  blende(g, u, 18, 142, 140);
}

// 9) Das Kammermaedchen holt Gruendlinge aus dem Bach; die junge Koenigin giesst sie ueber ihn
static void szeneGruseln(Gfx &g, float u) {
  if (u < 8) {
    bach(g, u);
    bauersfrau(g, 140, 128, 0);  // kniet am Ufer
    float k = seg(u, 2, 4);
    eimer(g, 166, lerp(188, 166, k), 1.1f, lerp(-1.2f, 0, k));
    for (int i = 0; i < 3; i++) {
      float f = seg(u, 3 + i * 0.6f, 4 + i * 0.6f);
      if (f > 0 && f < 1) fisch(g, lerp(120 + i * 30, 166, f), 194 - sinf(f * 3.14159f) * 36, 0.9f, f * 3);
    }
    g.kameraAus();
    if (u < 1.2f) g.iris(150, 150, lerp(0, 330, sanft(u / 1.2f)));
    if (u > 7) g.rechteck(0, 0, 284, 240, 0, (uint8_t)(255 * seg(u, 7, 8)));
    return;
  }
  float v = u - 8;
  g.verlauf(0, 200, FARBE(0x3A3466), FARBE(0x2A2650));
  g.rundRechteck(30, 30, 50, 66, 20, FARBE(0x1A2250));
  mond(g, 55, 56, 9);
  g.rechteck(0, 200, 284, 40, FARBE(0x5A3A5E));
  Pose p;
  p.zu = v < 9.4f;  // schlaeft
  p.mund = v > 10 ? 1 : 0.2f;
  float zit = seg(v, 10, 10.4f) * (1 - seg(v, 19, 21));
  bett(g, 150 + zitter(v, zit * 2), 214, 1.2f, 0, p);
  if (v < 4) zzz(g, 118, 150, v);
  float px = lerp(320, 222, sanft(seg(v, 4, 7)));
  float kipp = sanft(seg(v, 8.5f, 9.6f)) * -2.2f;
  prinzessin(g, px, 214, 0.9f, -1, 0);
  if (v > 4 && v < 23) eimer(g, px - 16, 170, 1.1f, kipp);
  if (v > 9.2f && v < 12) {
    float k = seg(v, 9.2f, 12);
    for (int i = 0; i < 14; i++) {
      float x = lerp(px - 26, 130 + (i % 7) * 7, k) + sinf(i * 1.7f) * 6, y = lerp(160, 186, k) + (i / 7) * 4;
      tropfen(g, x, y, 2.4f, FARBE(0x8FD8FF), (uint8_t)(255 * (1 - k * 0.5f)));
    }
  }
  if (v > 9.8f)
    for (int i = 0; i < 5; i++) {
      float k = seg(v, 9.8f + i * 0.1f, 10.8f + i * 0.1f);
      float x = lerp(px - 24, 120 + i * 14, k), y = lerp(160, 184, k) - sinf(k * 3.14159f) * 20;
      if (v > 11) y -= fabsf(sinf(v * 9 + i)) * 6;
      fisch(g, x, y, 0.9f, sinf(v * 10 + i) * 0.8f);
    }
  if (zit > 0) brrr(g, 150, 170, v, zit);
  blase(g, 142, 96, "Ach, was gruselt mir!", aufzu(v, 10.6f, 15.6f), 130, 160);
  if (v > 15.6f && v < 19) g.text(SCHRIFT_G, "Brrr!", 142, 110, WEISS, 1, (uint8_t)(255 * aufzu(v, 15.6f, 19)));
  blase(g, 142, 96, "Jetzt weiß ich, was Gruseln ist!", aufzu(v, 19.2f, 23.4f), 130, 160);
  if (v > 23 && v < 27)
    for (int i = 0; i < 6; i++) {
      float k = fmodf((v - 23) * 0.5f + i / 6.0f, 1);
      herz(g, 170 + sinf(k * 7 + i) * 30, 160 - k * 100, 5 + (i % 3), FARBE(0xFF5A7A), (uint8_t)(255 * huepf(k)));
    }
  g.kameraAus();
  if (v > 25.5f) g.iris(150, 170, lerp(330, 0, sanft(seg(v, 25.5f, 27.5f))));
  if (v > 27.5f) {
    uint8_t a = (uint8_t)(255 * sanft(seg(v, 27.5f, 28.5f)));
    g.text(SCHRIFT_G, "Ende", 142, 122, GOLD, 1, a);
    for (int i = 0; i < 6; i++) {
      float an = v * 1.2f + i * 1.047f;
      g.stern(142 + cosf(an) * 60, 112 + sinf(an) * 30, 5, 2, 5, an, GOLD, a);
    }
  }
}

static void bild(Gfx &g, float t) {
  if (t < F2) szeneStube(g, t - F1);
  else if (t < F3) szeneTurm(g, t - F2);
  else if (t < F4) szeneGalgen(g, t - F3);
  else if (t < F5) szeneWirtshaus(g, t - F4);
  else if (t < F6) szeneNacht1(g, t - F5);
  else if (t < F7) szeneNacht2(g, t - F6);
  else if (t < F8) szeneNacht3(g, t - F7);
  else if (t < F9) szeneHochzeit2(g, t - F8);
  else szeneGruseln(g, t - F9);
}


static void schlag(Synth &s, float x, float tief = 1) {  // dumpfer Schlag
  s.ton(x, 110 * tief, 0.35f, 0.002f, 0.3f, SINUS, GERAEUSCH, 55 * tief);
  s.rauschen(x, 0.2f, TIEFPASS, 900, 0.25f);
}
static void miau(Synth &s, float x) {
  s.ton(x, 700, 0.09f, 0.05f, 0.35f, DREIECK, GERAEUSCH, 950);
  s.ton(x + 0.25f, 900, 0.07f, 0.03f, 0.3f, DREIECK, GERAEUSCH, 600);
}
static void fragend(Synth &s, float x) { for (int k = 0; k < 3; k++) s.note(x + k * 0.3f, 71 + k * 2, 0.6f); }

static void ton(Synth &s, float t0, float t1) {
  // 1 Stube
  float a = F1;
  spieleMelodie(s, M(MEL_SPUK), a + 0.6f, a + 7, 0.34f, t0, t1, 0.6f);
  for (float x : {a + 7.0f, a + 9.0f}) if (bei(x, t0, t1)) { s.ton(x, 392, 0.12f, 0.08f, 0.35f, SINUS, MUSIK, 370); s.ton(x + 0.45f, 330, 0.12f, 0.08f, 0.5f, SINUS, MUSIK, 311); }
  if (bei(a + 7.6f, t0, t1)) for (int k = 0; k < 12; k++) s.note(a + 7.6f + k * 0.06f, 60 + (k & 1), 0.3f);
  if (bei(a + 12.6f, t0, t1)) fragend(s, a + 12.6f);
  if (bei(a + 16, t0, t1)) for (int k = 0; k < 4; k++) s.note(a + 16 + k * 0.18f, 55 - k * 2, 0.7f);
  spieleMelodie(s, M(MEL_WANDER), a + 20.5f, a + 29, 0.26f, t0, t1, 0.8f);
  // 2 Turm
  a = F2;
  for (int i = 0; i < 4; i++) if (bei(a + 0.4f + i * 1.4f, t0, t1)) s.glocke(a + 0.4f + i * 1.4f, i & 1 ? 55 : 60, 1);
  spieleMelodie(s, M(MEL_SPUK), a + 3, a + 14, 0.32f, t0, t1, 0.9f);
  for (float x : {a + 5.0f, a + 7.6f, a + 10.2f}) if (bei(x, t0, t1)) { s.note(x, 67, 0.7f); s.note(x + 0.2f, 72, 0.7f); }
  if (bei(a + 14.4f, t0, t1)) for (int k = 0; k < 14; k++) s.note(a + 14.4f + k * 0.25f, 84 - k * 2, 0.5f);
  if (bei(a + 17.9f, t0, t1)) { schlag(s, a + 17.9f); s.note(a + 17.9f, 48, 0.9f); }
  if (bei(a + 19.6f, t0, t1)) for (int k = 0; k < 5; k++) s.ton(a + 19.6f + k * 0.07f, 2600 + k * 200, 0.05f, 0.002f, 0.08f);  // Taler klimpern
  spieleMelodie(s, M(MEL_WANDER), a + 23.5f, a + 29, 0.26f, t0, t1, 0.8f);
  // 3 Galgen
  a = F3;
  spieleMelodie(s, M(MEL_WANDER), a + 0.4f, a + 8, 0.26f, t0, t1, 0.9f);
  spieleMelodie(s, M(MEL_N), a + 9, a + 27, 0.42f, t0, t1, 0.75f);
  for (float x = ceilf(t0 / 0.5f) * 0.5f; x < t1; x += 0.5f)
    if (x > a + 10 && x < a + 27) s.rauschen(x, 0.05f, HOCHPASS, 2500, 0.03f);  // Feuer knistert
  for (int i = 0; i < 7; i++) if (bei(a + 11 + i * 0.5f, t0, t1)) s.note(a + 11 + i * 0.5f, 64 - i, 0.4f);  // herunterholen
  if (bei(a + 17, t0, t1)) for (int k = 0; k < 4; k++) s.note(a + 17 + k * 0.15f, 60 + k * 3, 0.8f);
  for (int i = 0; i < 7; i++) if (bei(a + 20 + i * 0.55f, t0, t1)) s.note(a + 20 + i * 0.55f, 57 + i, 0.4f);  // wieder hinauf
  if (bei(a + 31.6f, t0, t1)) fragend(s, a + 31.6f);
  spieleMelodie(s, M(MEL_WANDER), a + 32.5f, a + 35.5f, 0.26f, t0, t1, 0.8f);
  // 4 Wirtshaus und Koenig
  a = F4;
  spieleMelodie(s, M(MEL_SPUK), a + 0.5f, a + 8.5f, 0.3f, t0, t1, 0.8f);
  spieleMelodie(s, M(MEL_F), a + 9, a + 17.5f, 0.24f, t0, t1, 0.9f);
  // 5 Erste Nacht
  a = F5;
  spieleMelodie(s, M(MEL_SPUK), a + 0.6f, a + 9, 0.32f, t0, t1, 0.8f);
  for (float x : {a + 6.4f, a + 7.4f}) if (bei(x, t0, t1)) miau(s, x);
  spieleMelodie(s, M(MEL_KEGEL), a + 9, a + 15, 0.26f, t0, t1, 0.7f);
  for (float x = ceilf(t0 / 0.6f) * 0.6f; x < t1; x += 0.6f)
    if (x > a + 9 && x < a + 15) s.rauschen(x, 0.03f, HOCHPASS, 3000, 0.04f);  // Karten
  if (bei(a + 17.8f, t0, t1)) { s.ton(a + 17.8f, 300, 0.2f, 0.002f, 0.1f, SINUS, GERAEUSCH, 150); for (int k = 0; k < 2; k++) miau(s, a + 18 + k * 0.4f); }
  if (bei(a + 20.4f, t0, t1)) for (int k = 0; k < 8; k++) s.note(a + 20.4f + k * 0.12f, 72 + k * 2, 0.5f);
  if (bei(a + 22.2f, t0, t1)) s.rauschen(a + 22.2f, 0.5f, TIEFPASS, 2500, 0.25f, 0.7f, 0.005f, 400);  // Platsch
  spieleMelodie(s, M(MEL_JAGD), a + 24, a + 31, 0.15f, t0, t1, 0.9f);
  for (float x = ceilf(t0 / 0.7f) * 0.7f; x < t1; x += 0.7f)
    if (x > a + 24.5f && x < a + 29) { s.ton(x, 140, 0.1f, 0.02f, 0.15f, DREIECK, GERAEUSCH, 110); s.rauschen(x + 0.2f, 0.05f, HOCHPASS, 3500, 0.05f); }
  spieleMelodie(s, M(MEL_JAGD), a + 32, a + 38, 0.2f, t0, t1, 0.7f);
  for (float x = ceilf(t0 / 0.3f) * 0.3f; x < t1; x += 0.3f)
    if (x > a + 32 && x < a + 38) s.rauschen(x, 0.08f, TIEFPASS, 400, 0.1f);  // das Bett rumpelt
  if (bei(a + 39.3f, t0, t1)) { schlag(s, a + 39.3f, 0.9f); s.note(a + 39.3f, 45, 1); }
  // 6 Zweite Nacht
  a = F6;
  spieleMelodie(s, M(MEL_SPUK), a + 0.6f, a + 11, 0.32f, t0, t1, 0.8f);
  for (float x : {a + 2.0f, a + 4.4f}) if (bei(x, t0, t1)) { s.ton(x, 900, 0.1f, 0.02f, 0.9f, SINUS, MUSIK, 300); }  // Schrei im Schornstein
  for (float x : {a + 3.4f, a + 5.6f}) if (bei(x, t0, t1)) schlag(s, x, 0.9f);
  if (bei(a + 8.5f, t0, t1)) for (int k = 0; k < 4; k++) s.note(a + 8.5f + k * 0.08f, 76 + k * 3, 0.6f);
  for (float x = ceilf(t0 / 0.2f) * 0.2f; x < t1; x += 0.2f)
    if (x > a + 13 && x < a + 19) s.ton(x, 1400 + fmodf(x * 37, 3) * 200, 0.04f, 0.002f, 0.08f);  // Drehbank
  spieleMelodie(s, M(MEL_KEGEL), a + 19, a + 30, 0.24f, t0, t1, 0.9f);
  for (float w : {a + 20.0f, a + 26.0f}) if (bei(w, t0, t1)) for (int k = 0; k < 6; k++) s.rauschen(w + k * 0.25f, 0.06f, TIEFPASS, 500, 0.08f);
  for (float w : {a + 21.6f, a + 27.6f}) if (bei(w, t0, t1)) for (int k = 0; k < 9; k++) s.note(w + k * 0.05f, 72 + k * 2, 0.6f);
  for (int i = 0; i < 12; i++) if (bei(a + 30 + i * 0.2f, t0, t1)) s.glocke(a + 30 + i * 0.2f, 60, 0.7f);
  spieleMelodie(s, M(MEL_WIEGE), a + 33, a + 37.5f, 0.4f, t0, t1, 0.6f);
  // 7 Dritte Nacht
  a = F7;
  spieleMelodie(s, M(MEL_N), a + 0.6f, a + 22, 0.42f, t0, t1, 0.75f);
  for (float x = ceilf(t0 / 0.6f) * 0.6f; x < t1; x += 0.6f)
    if ((x > a && x < a + 5) || (x > a + 19 && x < a + 22.5f)) s.rauschen(x, 0.05f, TIEFPASS, 500, 0.08f);  // Schritte
  if (bei(a + 5, t0, t1)) schlag(s, a + 5, 0.8f);
  if (bei(a + 14, t0, t1)) for (int k = 0; k < 6; k++) s.note(a + 14 + k * 0.15f, 52 + k, 0.6f);  // er erhebt sich
  if (bei(a + 18, t0, t1)) schlag(s, a + 18, 0.9f);
  spieleMelodie(s, M(MEL_RIESE), a + 22.5f, a + 28, 0.4f, t0, t1, 0.9f);
  if (bei(a + 28.4f, t0, t1)) { s.ton(a + 28.4f, 200, 0.3f, 0.002f, 0.2f, SINUS, GERAEUSCH, 90); s.glocke(a + 28.4f, 84, 0.9f); s.glocke(a + 28.45f, 88, 0.6f); }
  for (float x = ceilf(t0 / 0.35f) * 0.35f; x < t1; x += 0.35f)
    if (x > a + 29.5f && x < a + 34) s.ton(x, 900, 0.08f, 0.002f, 0.12f, SINUS, GERAEUSCH, 700);  // Eisenstange
  if (bei(a + 30, t0, t1)) for (int k = 0; k < 8; k++) s.note(a + 30 + k * 0.4f, 67 - (k & 1) * 5, 0.45f);
  for (int i = 0; i < 3; i++) if (bei(a + 37 + i * 0.4f, t0, t1)) for (int k = 0; k < 4; k++) s.note(a + 37 + i * 0.4f + k * 0.06f, 88 + k * 2, 0.5f);
  for (int i = 0; i < 12; i++) if (bei(a + 42.6f + i * 0.2f, t0, t1)) s.glocke(a + 42.6f + i * 0.2f, 60, 0.6f);
  spieleMelodie(s, M(MEL_F), a + 46, a + 49.5f, 0.24f, t0, t1, 0.9f);
  // 8 Hochzeit
  a = F8;
  spieleMelodie(s, M(MEL_F), a + 0.5f, a + 6, 0.24f, t0, t1, 1);
  for (int i = 0; i < 10; i++) if (bei(a + 6 + i * 0.6f, t0, t1)) s.glocke(a + 6 + i * 0.6f, i & 1 ? 79 : 72 + (i % 4 == 2) * 4, 0.85f);
  if (bei(a + 13.3f, t0, t1)) fragend(s, a + 13.3f);
  spieleMelodie(s, M(MEL_WIEGE), a + 14, a + 17.5f, 0.4f, t0, t1, 0.6f);
  // 9 Bach und Gruseln
  a = F9;
  spieleMelodie(s, M(MEL_WANDER), a + 0.5f, a + 7, 0.26f, t0, t1, 0.8f);
  for (int i = 0; i < 3; i++) if (bei(a + 3 + i * 0.6f, t0, t1)) s.ton(a + 3 + i * 0.6f, 800, 0.08f, 0.002f, 0.08f, SINUS, GERAEUSCH, 1400);
  float v = a + 8;
  spieleMelodie(s, M(MEL_WIEGE), v + 0.5f, v + 8.5f, 0.42f, t0, t1, 0.7f);
  if (bei(v + 8.6f, t0, t1)) for (int k = 0; k < 6; k++) s.note(v + 8.6f + k * 0.08f, 84 - k * 3, 0.6f);
  if (bei(v + 9.3f, t0, t1)) { s.rauschen(v + 9.3f, 0.7f, TIEFPASS, 3500, 0.35f, 0.7f, 0.005f, 400); s.rauschen(v + 9.3f, 0.3f, HOCHPASS, 2500, 0.15f); }
  for (int i = 0; i < 8; i++) if (bei(v + 10.8f + i * 0.35f, t0, t1)) s.ton(v + 10.8f + i * 0.35f, 700 + (i % 3) * 120, 0.08f, 0.002f, 0.08f, SINUS, GERAEUSCH, 1300);
  if (bei(v + 10.2f, t0, t1)) for (int k = 0; k < 30; k++) s.note(v + 10.2f + k * 0.05f, 55 + (k & 1) * 2, 0.45f);
  spieleMelodie(s, M(MEL_F), v + 19, v + 26, 0.24f, t0, t1, 0.9f);
  if (bei(v + 27.5f, t0, t1)) { s.glocke(v + 27.5f, 72); s.glocke(v + 27.8f, 76); s.glocke(v + 28.1f, 79); s.glocke(v + 28.5f, 84); }
}

// =====================================================================
// Startbild und Vorschau
// =====================================================================
static void titelSzene(Gfx &g, float t) {
  g.verlauf(0, 200, FARBE(0x2B2A6B), FARBE(0x4E3D8F));
  for (int i = 0; i < 16; i++) {
    float fu = 0.5f + 0.5f * sinf(t * 2 + i * 1.7f);
    g.stern(zufall(i + 500) * 284, zufall(i + 520) * 110, 1.6f + fu * 1.4f, 0.7f, 4, 0, WEISS, (uint8_t)(120 + fu * 130));
  }
  mond(g, 236, 40, 15);
  spukschloss(g, 200, 196, t, 0.9f);
  g.rechteck(0, 196, 284, 44, FARBE(0x3E5A4A));
  gespenst(g, 116 + sinf(t * 0.8f) * 14, 84 + sinf(t * 1.6f) * 8, 0.6f, t, 1);
  Pose h = pose(HANS);
  h.mund = 0.5f;
  h.armV = 1.1f; h.armH = 1.1f;  // Arme verschraenkt: keine Angst!
  h.zu = fmodf(t, 3.5f) < 0.12f;
  schneiderlein(g, 62, 222, 1.1f, 1, h);
}

static void titelbild(Gfx &g, float t) {
  titelSzene(g, t);
  g.rundRechteck(104, 102, 168, 76, 14, FARBE(0x1E1A3A), 170);
  g.text(SCHRIFT_K, "Von einem, der auszog,", 188, 122, WEISS, 1);
  g.text(SCHRIFT_M, "das Fürchten", 188, 146, GOLD, 1);
  g.text(SCHRIFT_M, "zu lernen", 188, 168, GOLD, 1);
  float b = 1 + sinf(t * 4) * 0.08f;
  g.kontur(2, OUT);
  g.kreis(188, 204, 14 * b, FARBE(0xE8473D));
  g.konturAus();
  float tri[] = {183, 196, 183, 212, 196, 204};
  g.polygon(tri, 3, WEISS);
}

static void vorschau(Gfx &g, float t) { titelSzene(g, t); }

static void symbol(Gfx &g, float x, float y, float t) {
  kerze(g, x - 38, y - 2, t);
  gespenst(g, x + 12, y - 12 + sinf(t * 2) * 5, 0.85f, t, 1);
}

const Maerchen FUERCHTEN = {"Von einem, der auszog, das Fürchten zu lernen", FDAUER, FARBE(0x5B3D8F), bild, ton,
                            titelbild, vorschau, symbol, LIED(MEL_WANDER, 0.36f)};

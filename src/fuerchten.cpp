// Von einem, der auszog, das Fürchten zu lernen - ein Spieluhr-Film fuer die Maerchenbox.
// Kindgerecht erzaehlt: niedliche Gespenster, Katzen und ein freundliches Skelett;
// am Ende lernt Hans das Gruseln mit einem Eimer kaltem Wasser voller Fischlein.
#include "maerchen.h"
#include "schriften.h"
#include "figuren.h"
#include "melodien.h"
#include <stdio.h>
#include <initializer_list>

// ---------- Ablauf (Sekunden) ----------
static const float F1 = 0, F2 = 36, F3 = 72, F4 = 104, F5 = 150, F6 = 196, F7 = 240, F8 = 264, FDAUER = 300;

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

// freundliches Skelett; oben/unten getrennt verschiebbar (faellt in zwei Haelften den Kamin herunter)
static void skelett(Gfx &g, float x, float y, float s, float t, float dyOben, float dxOben, float arme) {
  const Farbe k = FARBE(0xF4F1E6);
  // Beine und Becken
  float sw = sinf(t * 6) * 2 * (arme > 0.5f);
  g.kontur(1.2f, OUT);
  g.linie(x - 6 * s, y - 34 * s, x - 7 * s + sw, y - 3 * s, 3.5f * s, k);
  g.linie(x + 6 * s, y - 34 * s, x + 7 * s - sw, y - 3 * s, 3.5f * s, k);
  g.ellipse(x - 8 * s + sw, y - 2 * s, 5 * s, 2.5f * s, k);
  g.ellipse(x + 8 * s - sw, y - 2 * s, 5 * s, 2.5f * s, k);
  g.ellipse(x, y - 36 * s, 10 * s, 5 * s, k);
  g.konturAus();
  // Oberkoerper
  float ox = x + dxOben, oy = y + dyOben;
  g.kontur(1.2f, OUT);
  g.linie(ox, oy - 40 * s, ox, oy - 64 * s, 3 * s, k);
  for (int i = 0; i < 3; i++) g.ellipse(ox, oy - 48 * s - i * 6 * s, 9 * s - i, 2 * s, k);
  for (int i = -1; i <= 1; i += 2) {
    float w = 0.4f + arme * (1.6f + 0.5f * sinf(t * 7 + i));
    float ax = ox + i * 9 * s, ay = oy - 62 * s;
    float hx = ax + i * sinf(w) * 20 * s, hy = ay + cosf(w) * 20 * s;
    g.linie(ax, ay, hx, hy, 3 * s, k);
    g.kreis(hx, hy, 3 * s, k);
  }
  g.kreis(ox, oy - 76 * s, 12 * s, k);
  g.konturAus();
  g.ellipse(ox - 4.5f * s, oy - 77 * s, 3.2f * s, 3.8f * s, FARBE(0x2E2A3E));
  g.ellipse(ox + 4.5f * s, oy - 77 * s, 3.2f * s, 3.8f * s, FARBE(0x2E2A3E));
  g.kreis(ox - 4 * s, oy - 78 * s, 1 * s, WEISS);
  g.kreis(ox + 5 * s, oy - 78 * s, 1 * s, WEISS);
  g.bogen(ox, oy - 72 * s, 6 * s, 0.4f, 2.7f, 1.6f * s, FARBE(0x2E2A3E));
  for (int i = -2; i <= 2; i++) g.linie(ox + i * 2.2f * s, oy - 67.5f * s, ox + i * 2.2f * s, oy - 65.5f * s, 0.8f * s, FARBE(0x2E2A3E));
}

static void kegel(Gfx &g, float x, float y, float s, float umfall) {
  float c = cosf(umfall), sn = sinf(umfall);
  auto R = [&](float dx, float dy, float *o) { o[0] = x + (dx * c - dy * sn) * s; o[1] = y + (dx * sn + dy * c) * s; };
  float p[2 * 8];
  const float f[8][2] = {{-4, 0}, {4, 0}, {6, -10}, {3, -16}, {4, -22}, {-4, -22}, {-3, -16}, {-6, -10}};
  for (int i = 0; i < 8; i++) R(f[i][0], f[i][1], p + 2 * i);
  g.kontur(1, OUT);
  g.polygon(p, 8, WEISS);
  g.konturAus();
  float a[2], b[2];
  R(-5, -13, a); R(5, -13, b);
  g.linie(a[0], a[1], b[0], b[1], 2 * s, FARBE(0xE0473A));
}

static void kuerbis(Gfx &g, float x, float y, float r, float dreh) {
  g.kontur(1.3f, OUT);
  g.ellipse(x, y, r, r * 0.85f, FARBE(0xF28A1E));
  g.konturAus();
  g.ellipse(x - r * 0.4f, y, r * 0.35f, r * 0.8f, FARBE(0xE07610), 255, 0);
  g.ellipse(x + r * 0.4f, y, r * 0.35f, r * 0.8f, FARBE(0xE07610), 255, 0);
  g.linie(x + cosf(dreh - 1.57f) * r * 0.8f, y + sinf(dreh - 1.57f) * r * 0.8f, x + cosf(dreh - 1.57f) * r * 1.15f,
          y + sinf(dreh - 1.57f) * r * 1.15f, 3, FARBE(0x5A8F2E));
  // freundliches Gesicht
  g.kreis(x - r * 0.35f, y - r * 0.15f, r * 0.13f, FARBE(0x5A2E12));
  g.kreis(x + r * 0.35f, y - r * 0.15f, r * 0.13f, FARBE(0x5A2E12));
  g.bogen(x, y + r * 0.05f, r * 0.4f, 0.4f, 2.7f, r * 0.12f, FARBE(0x5A2E12));
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

static void fisch(Gfx &g, float x, float y, float s, float w, uint8_t a = 255) {
  g.ellipse(x, y, 6 * s, 3.2f * s, FARBE(0xFF9A3C), a, w);
  float c = cosf(w), sn = sinf(w);
  float sch[] = {x - 5 * s * c, y - 5 * s * sn, x - 10 * s * c - 4 * s * sn, y - 10 * s * sn + 4 * s * c,
                 x - 10 * s * c + 4 * s * sn, y - 10 * s * sn - 4 * s * c};
  g.polygon(sch, 3, FARBE(0xFF7A1C), a);
  g.kreis(x + 3 * s * c, y + 3 * s * sn - 1 * s, 1 * s, SCHWARZ, a);
}

static void eimer(Gfx &g, float x, float y, float s, float kipp) {
  float c = cosf(kipp), sn = sinf(kipp);
  auto R = [&](float dx, float dy, float *o) { o[0] = x + (dx * c - dy * sn) * s; o[1] = y + (dx * sn + dy * c) * s; };
  float q[8];
  R(-10, -16, q); R(10, -16, q + 2); R(8, 0, q + 4); R(-8, 0, q + 6);
  g.kontur(1.3f, OUT);
  g.polygon(q, 4, FARBE(0x9AA6B8));
  g.konturAus();
  R(-10, -16, q); R(10, -16, q + 2); R(10, -13, q + 4); R(-10, -13, q + 6);
  g.polygon(q, 4, FARBE(0x6EC6FF));
  float a[2], b[2];
  R(-10, -16, a); R(10, -16, b);
  g.kurve(a[0], a[1], (a[0] + b[0]) / 2 - sn * 12 * s, (a[1] + b[1]) / 2 - c * 12 * s, b[0], b[1], 1.5f, OUT);
}

static void brrr(Gfx &g, float x, float y, float t, float staerke) {
  // Zitterlinien links und rechts
  for (int i = 0; i < 3; i++) {
    float o = sinf(t * 40 + i) * 2;
    g.linie(x - 26 - i * 5 + o, y - 10 + i * 8, x - 22 - i * 5 + o, y - 2 + i * 8, 1.6f, OUT, (uint8_t)(255 * staerke));
    g.linie(x + 26 + i * 5 - o, y - 10 + i * 8, x + 22 + i * 5 - o, y - 2 + i * 8, 1.6f, OUT, (uint8_t)(255 * staerke));
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
// Szenen
// =====================================================================
static float zitter(float t, float k) { return sinf(t * 47) * 2.2f * k; }

static void szeneStube(Gfx &g, float u) {
  bool ruf = (u > 8 && u < 9.2f) || (u > 10 && u < 11.2f);
  stube(g, u, ruf);
  // Vater sitzt am Tisch (liest), Bruder zittert, Hans laechelt
  Pose v = pose(VATER);
  v.armV = 1.4f; v.zu = fmodf(u, 4.4f) < 0.15f;
  if (u > 19 && u < 26) { v.boese = 0.6f; v.armV = 2.4f + sinf(u * 6) * 0.2f; }  // schuettelt den Kopf
  schneiderlein(g, 232, 210, 1.05f, -1, v);
  bart(g, 232, 210, 1.05f, -1);
  float angst = seg(u, 8.2f, 8.6f) * (1 - seg(u, 14, 15));
  Pose b = pose(BRUDER);
  b.mund = angst > 0.3f ? 0.9f : 0; b.armV = angst > 0.3f ? 2.7f : 0.4f; b.armH = angst > 0.3f ? 2.7f : 0.3f;
  b.blick = -1;
  schneiderlein(g, 150 + zitter(u, angst), 214, 0.95f, -1, b);
  if (angst > 0.3f) brrr(g, 150, 160, u, angst);
  Pose h = pose(HANS);
  h.blick = -0.5f; h.zu = fmodf(u + 1, 3.6f) < 0.12f;
  float hx = 80;
  if (u > 26) { hx = lerp(80, -40, seg(u, 30, 34)); h.lauf = u > 30 ? fmodf(u * 1.7f, 1) : -1; h.ranzen = true; }
  if (u > 26 && u < 30) { h.armV = 2.6f + sinf(u * 10) * 0.4f; h.mund = 0.5f; }  // winkt zum Abschied
  if (u > 12 && u < 18) { h.mund = 0.4f; h.armV = 1.2f; h.armH = 1.2f; }        // wundert sich
  schneiderlein(g, hx, 214, 0.95f, u > 30 ? -1 : 1, h);
  // Gedankenblase: was ist Gruseln?
  if (u > 13 && u < 18.5f) {
    float k = federnd(seg(u, 13, 13.5f)) * (1 - seg(u, 18, 18.5f));
    g.kontur(1.4f, OUT);
    g.kreis(98, 104, 3 * k, WEISS);
    g.kreis(106, 92, 4.5f * k, WEISS);
    g.ellipse(124, 66, 26 * k, 18 * k, WEISS);
    g.konturAus();
    if (k > 0.6f) { brrr(g, 124, 70, u, 1); g.text(SCHRIFT_M, "?", 124, 74, OUT, 1); }
  }
  // Hans will in die Welt: Blase mit Gespenst
  if (u > 19 && u < 25.5f) {
    float k = federnd(seg(u, 19, 19.5f)) * (1 - seg(u, 25, 25.5f));
    g.kontur(1.4f, OUT);
    g.kreis(92, 112, 3 * k, FARBE(0xCFE0FF));
    g.ellipse(110, 74, 28 * k, 22 * k, FARBE(0xCFE0FF));
    g.konturAus();
    if (k > 0.6f) gespenst(g, 110, 90, 0.4f, u, 1);
  }
  blende(g, u, 36, 140, 140);
}

static void szeneTurm(Gfx &g, float u) {
  turm(g, u);
  // Glocke mit Seil
  float schwung = u > 3 && u < 10 ? sinf((u - 3) * 4.5f) * 0.4f * (1 - seg(u, 8, 10)) : 0;
  g.linie(90, 0, 90, 36, 4, FARBE(0x6E4426));
  glocke(g, 90, 36, 1.2f, schwung);
  float seilY = 90 + (u > 3 && u < 9 ? fabsf(sinf((u - 3) * 4.5f)) * 14 : 0);
  g.linie(90, 92, 90, seilY + 60, 2.5f, FARBE(0xC9A46A));
  // Hans zieht am Seil, dann dreht er sich zum Gespenst
  Pose h = pose(HANS);
  h.zu = fmodf(u + 0.4f, 3.3f) < 0.12f;
  float hx = 76;
  if (u < 10) { h.armV = 2.8f; h.armH = 2.6f; }
  if (u > 12 && u < 20) { h.armV = 1.1f; h.armH = 1.1f; h.mund = 0.2f; }  // Arme verschraenkt, grinst
  if (u > 20 && u < 22) { h.armV = lerp(1.0f, 1.9f, seg(u, 20, 20.4f)); }  // schubst
  if (u > 26) { h.armV = 0.6f; h.mund = 0.4f; }
  schneiderlein(g, hx, 214, 0.95f, 1, h);
  // Gespenst auf der Treppe
  float gx = 208, gy = 160, fall = 0;
  if (u < 11) { gx = 300; }
  else if (u < 20) { gx = lerp(300, 196, sanft(seg(u, 11, 13.5f))); gy = 142 + sinf(u * 2.5f) * 4; }
  else {  // purzelt die Treppe hinunter
    float k = seg(u, 20.4f, 24);
    gx = lerp(196, 250, k);
    gy = lerp(142, 210, k) - fabsf(sinf(k * 12)) * 10;
    fall = k * 14;
  }
  if (u < 24.5f) gespenst(g, gx, gy, 0.85f, u, u > 13 && u < 20 ? 1 : 0.2f, fall);
  else {  // Laken weg: da liegt der Kuester mit Sternchen
    Pose k = pose(KUESTER);
    k.zu = true; k.mund = 0.5f;
    schneiderlein(g, 250, 214, 0.8f, -1, k);
    g.ellipse(270, 214, 24, 5, WEISS, 230);
    for (int i = 0; i < 4; i++) {
      float a = u * 4 + i * 1.57f;
      g.stern(250 + cosf(a) * 16, 128 + sinf(a) * 5, 3, 1.3f, 5, a, GOLD);
    }
  }
  if (u > 24 && u < 24.4f) g.wackeln(sinf(u * 90) * 4, cosf(u * 80) * 3);
  if (u > 28 && u < 33) fragezeichen(g, 96, 86, 1, (uint8_t)(255 * seg(u, 28, 28.4f)));
  blende(g, u, 36, 110, 150);
}

static void szeneWandern(Gfx &g, float u) {
  float abend = seg(u, 18, 30);
  float scroll = u < 18 ? u * 40 : 720 + (u - 18) * 10;
  wanderland(g, scroll, u, abend);
  // Spukschloss taucht auf
  if (u > 14) spukschloss(g, lerp(380, 200, sanft(seg(u, 14, 22))), 150, u, 0.75f);
  Pose h = pose(HANS);
  h.ranzen = true;
  h.lauf = u < 24 ? fmodf(u * 1.7f, 1) : -1;
  h.armV = u < 24 ? 0.4f + sinf(u * 10.7f) * 0.5f : 2.6f;
  h.armH = u < 24 ? 0.4f - sinf(u * 10.7f) * 0.5f : 0.4f;
  h.mund = u > 24 ? 0.5f : 0;
  h.zu = fmodf(u + 0.2f, 3.4f) < 0.12f;
  float hop = h.lauf >= 0 ? fabsf(sinf(h.lauf * 6.2831853f)) * 3 : 0;
  schneiderlein(g, 90, 216 - hop, 0.95f, 1, h);
  // Pfeif-Noten
  if (u < 18)
    for (int i = 0; i < 3; i++) {
      float ph = fmodf(u * 0.5f + i / 3.0f, 1);
      notenzeichen(g, 104 + ph * 30, 120 - ph * 50, i & 1 ? FARBE(0x3E7FDC) : FARBE(0xE8473D), (uint8_t)(230 * huepf(ph)));
    }
  // Koenig mit Aushang
  if (u > 22) {
    float kx = lerp(320, 176, sanft(seg(u, 22, 25)));
    koenig(g, kx, 216, 0.85f, -1, 0);
    if (u > 25) {
      float k = federnd(seg(u, 25, 25.5f));
      g.kontur(1.4f, OUT);
      g.rundRechteck(140, 40 - (1 - k) * 60, 96, 70, 6, FARBE(0xFBF1DC));
      g.konturAus();
      if (k > 0.5f) {
        spukschloss(g, 168, 88, u, 0.28f);
        for (int i = 0; i < 3; i++) g.text(SCHRIFT_M, "|", 196 + i * 7, 74, OUT);
        herz(g, 222, 86, 6, FARBE(0xFF5A7A), 255);
        mond(g, 210, 58, 6);
      }
    }
  }
  blende(g, u, 32, 120, 150);
}

static void szeneKatzen(Gfx &g, float u) {
  halle(g, u);
  // Hans am Feuer
  Pose h = pose(HANS);
  h.zu = fmodf(u + 0.3f, 3.7f) < 0.12f;
  h.armV = 1.2f;
  bool karten = u > 10 && u < 26;
  if (karten) { h.armV = 1.4f + sinf(u * 3) * 0.3f; h.mund = u > 22 ? 0.6f : 0.2f; }
  if (u > 30 && u < 40) { h.armV = 2.4f + sinf(u * 14) * 0.6f; h.boese = 0.5f; h.mund = 0.7f; }  // husch, raus!
  schneiderlein(g, 170, 214, 0.95f, -1, h);
  // zwei Katzen, spaeter viele
  int n = u < 28 ? 2 : 7;
  for (int i = 0; i < n; i++) {
    float kx, ky = 214, dir = 1, lauf = -1;
    if (i < 2) {
      kx = u < 6 ? lerp(-30 - i * 20, 112 - i * 26, sanft(seg(u, 2 + i, 6))) : 112 - i * 26;
      if (u < 6) lauf = fmodf(u * 2, 1);
    } else {
      float an = 27 + i * 0.4f;
      kx = lerp(-40, 30 + (i - 2) * 22, sanft(seg(u, an, an + 2)));
      ky = 214 - (i & 1) * 6;
      if (u < an + 2) lauf = fmodf(u * 2, 1);
    }
    if (u > 31) {  // fliehen zum Fenster hinaus
      float k = seg(u, 31 + i * 0.25f, 33 + i * 0.25f);
      if (k >= 1) continue;
      kx = lerp(kx, 242, k);
      ky = lerp(ky, 60, k) - sinf(k * 3.14159f) * 40;
      lauf = fmodf(u * 3, 1);
    }
    katze(g, kx, ky, 1, dir, u + i, lauf);
  }
  // Karten auf dem Boden
  if (karten)
    for (int i = 0; i < 5; i++) {
      float x = 118 + i * 9, y = 216;
      float fl = u > 22 && u < 24 ? sinf((u - 22) * 3.14159f / 2) * 30 : 0;  // Hans gewinnt: Karten fliegen
      karte(g, x + (i - 2) * fl * 0.4f, y - fl * (0.6f + (i & 1) * 0.4f), u + i, i & 1 ? FARBE(0xE0473A) : OUT);
    }
  // Miau-Blasen
  if (u > 6.5f && u < 9.5f) {
    g.kontur(1.2f, OUT);
    g.ellipse(100, 150, 22, 13, WEISS);
    g.konturAus();
    g.text(SCHRIFT_K, "Miau", 100, 155, OUT, 1);
  }
  blende(g, u, 46, 150, 150);
}

static void szeneSkelett(Gfx &g, float u) {
  halle(g, u, 1 - seg(u, 1, 3) + seg(u, 40, 44));
  uhr(g, 236, 130, 16, 11 + seg(u, 0, 36) * 0.98f + (u > 36 ? 0.02f : 0));
  // Skelett faellt in zwei Haelften den Kamin herunter und setzt sich zusammen
  float sx = 98, sy = 204;
  float dyO = 0, dxO = 0, arme = 0.3f;
  bool da = u > 3 && u < 38.5f;
  if (u < 6) { dyO = lerp(-180, -12, hinein(seg(u, 3, 4.3f))); }
  if (u >= 4.3f && u < 7) { dxO = 18 * (1 - seg(u, 5.5f, 7)); dyO = -12 * (1 - seg(u, 5.5f, 7)); }
  float unten = u < 5 ? lerp(-200, 0, hinein(seg(u, 3.6f, 5))) : 0;
  if (u > 7) { sx = lerp(98, 210, sanft(seg(u, 7, 10))); arme = u > 10 ? 1 : 0.3f; }
  if (u > 30) { sx = 210; arme = 1; }
  if (da) skelett(g, sx, sy + unten, 0.95f, u, dyO, dxO, arme);
  if (u > 38.5f && u < 40)  // Puff - um Mitternacht verschwindet es
    for (int i = 0; i < 8; i++) {
      float a = i * 0.785f, k = seg(u, 38.5f, 40);
      g.stern(210 + cosf(a) * k * 50, 140 + sinf(a) * k * 40, 6, 2.5f, 5, a, GOLD, (uint8_t)(255 * (1 - k)));
    }
  // Kegel und Kuerbis
  if (u > 10) {
    for (int i = 0; i < 6; i++) {
      float kx = 238 + (i % 3) * 10 - (i / 3) * 5, ky = 214 - (i / 3) * 6;
      float um = u > 19.2f + i * 0.05f ? sanft(seg(u, 19.2f + i * 0.05f, 19.8f + i * 0.05f)) * (i & 1 ? 1.4f : -1.4f) : 0;
      if (u > 27) um = 0;  // wieder aufgestellt
      kegel(g, kx + um * 6, ky, 0.9f, um);
    }
    // Hans rollt den Kuerbis
    float kk = seg(u, 17, 19.3f);
    float bx = lerp(140, 244, kk), by = 206;
    if (u < 17) bx = 140;
    if (u > 27 && u < 29.5f) { float k2 = seg(u, 27, 29.5f); bx = lerp(184, 250, k2); }  // Skelett kegelt
    if (u > 29.5f) bx = 160;
    kuerbis(g, bx, by, 8, u * 3 * (kk > 0 && kk < 1));
  }
  Pose h = pose(HANS);
  h.zu = fmodf(u + 0.6f, 3.4f) < 0.12f;
  h.armV = u > 15 && u < 17.2f ? lerp(0.4f, 1.6f, seg(u, 15, 17)) : 0.5f;
  if (u > 19.8f && u < 24) { h.armV = 2.7f; h.armH = 2.7f; h.mund = 0.8f; h.zu = true; }  // alle neune!
  if (u > 30 && u < 37) { h.mund = 0.4f; }
  schneiderlein(g, 140, 214, 0.95f, 1, h);
  if (u > 20 && u < 23)
    for (int i = 0; i < 5; i++) {
      float a = u * 3 + i * 1.256f;
      g.stern(244 + cosf(a) * 26, 190 + sinf(a) * 12, 4, 1.7f, 5, a, GOLD);
    }
  blende(g, u, 46, 150, 140);
}

static void szeneBett(Gfx &g, float u) {
  halle(g, u, 0.6f);
  // Bett faehrt mit Hans durchs Schloss
  float bx, by = 214, kipp = 0;
  if (u < 3) bx = 150;
  else if (u < 15) {
    float k = (u - 3) / 12;
    bx = 142 + sinf(k * 6.2831853f * 1.5f) * 100;
    by = 214 - fabsf(sinf(u * 6)) * 4;
    kipp = sinf(u * 5) * 0.06f;
  } else if (u < 16.5f) { bx = lerp(142, 62, seg(u, 15, 16.5f)); kipp = lerp(0, 3.0f, sanft(seg(u, 15, 16.5f))); by = 214 - sinf(seg(u, 15, 16.5f) * 3.14159f) * 40; }
  else { bx = 62; kipp = 3.0f; }
  Pose p;
  p.zu = u < 15 && fmodf(u, 2) < 1;  // geniesst die Fahrt
  p.mund = 0.7f;
  if (u < 16.5f) bett(g, bx, by, 1, kipp, p);
  else {
    bett(g, bx, by - 30, 1, kipp, p, false);  // liegt umgedreht da
    Pose h = pose(HANS_NACHT);
    h.mund = 0.7f; h.armV = 2.6f; h.zu = true;
    float k = seg(u, 16.5f, 18);
    if (u < 18) schneiderlein(g, lerp(70, 128, k), 214, 0.9f, 1, h);
  }
  // der alte Mann mit dem langen Bart
  RiesePose r;
  r.farbe = 2;
  r.armV = 0.6f; r.armH = 0.4f;
  float ax = 230;
  bool alter = u > 19;
  if (alter) {
    if (u < 22) ax = lerp(330, 230, sanft(seg(u, 19, 22)));
    if (u > 22 && u < 26) { r.boese = 0.6f; r.armV = 2.4f + sinf(u * 5) * 0.2f; r.mund = 0.6f; }  // "Ich bin staerker!"
    if (u > 28 && u < 36) { r.staunt = 1; r.mund = 0.9f; r.kopf = 0.45f; }  // Bart steckt fest!
    if (u > 36) { r.mund = 0.3f; r.armV = 1.6f; }
    riese(g, ax, 214, 0.8f, -1, r);
  }
  // Amboss: Hans spaltet ihn und klemmt den Bart ein
  if (u > 24) {
    float spalt = u > 27 ? lerp(0, 4, seg(u, 27, 27.2f)) : 0;
    if (u > 33) spalt = lerp(4, 0, seg(u, 33, 33.4f)) + 0;
    amboss(g, 176, 214, 1, spalt);
    if (u > 28 && u < 36) g.kurve(206, 112, 196, 160, 178, 186, 7, FARBE(0xF4F6FA));  // eingeklemmter Bart
  }
  if (u >= 18) {
    Pose h = pose(HANS);
    h.zu = fmodf(u, 3.2f) < 0.12f;
    float hx = 128;
    if (u > 25 && u < 27.2f) { h.armV = lerp(3.0f, 1.6f, seg(u, 26.6f, 27.2f)); }  // Axt hoch und runter
    if (u > 28 && u < 36) { h.mund = 0.5f; h.armV = 1.2f; h.armH = 1.2f; }
    if (u > 38) { h.armV = 2.7f; h.armH = 2.7f; h.mund = 0.8f; }
    float hx2, hy2;
    schneiderlein(g, hx, 214, 0.95f, 1, h, &hx2, &hy2);
    if (u > 24 && u < 28) {  // Axt
      g.linie(hx2, hy2, hx2 + 10, hy2 - 22, 3, HOLZ);
      float ax2[] = {hx2 + 6, hy2 - 26, hx2 + 18, hy2 - 22, hx2 + 14, hy2 - 14};
      g.polygon(ax2, 3, FARBE(0xC7D0DC));
    }
  }
  if (u > 27 && u < 27.4f) { g.wackeln(sinf(u * 90) * 5, cosf(u * 80) * 4); g.stern(176, 186, 16, 6, 7, u * 3, GOLD); }
  // Schatzkisten
  if (u > 36)
    for (int i = 0; i < 3; i++) truhe(g, 40 + i * 44, 214, 0.8f, u + i, seg(u, 36.5f + i * 0.4f, 37.5f + i * 0.4f));
  // Morgen: Sonne durchs Fenster
  if (u > 41) g.kreis(242, 60, 30, FARBE(0xFFE9A0), (uint8_t)(120 * seg(u, 41, 44)));
  blende(g, u, 44, 150, 140);
}

static void szeneHochzeit2(Gfx &g, float u) {
  himmel(g, 0, u, 200);
  sonne(g, 240, 36, 14, u);
  schloss(g, 142, 196, u, 0);
  boden(g, 196);
  float tep[] = {120, 196, 164, 196, 200, 240, 84, 240};
  g.polygon(tep, 4, FARBE(0xD8283F));
  koenig(g, 70, 214, 0.9f, 1, u > 3 && u < 7 ? u : 0);
  for (int i = 0; i < 3; i++) truhe(g, 34 + i * 26, 236, 0.55f, u + i, 1);
  prinzessin(g, lerp(214, 166, seg(u, 6, 8)), 214, 0.9f, -1, u > 14 ? u : 0);
  Pose h = pose(HANS);
  h.mund = 0.4f;
  h.zu = u > 9 && u < 15;
  h.armV = u > 14 ? 2.6f + sinf(u * 9) * 0.3f : 0.5f;
  schneiderlein(g, 128, 214, 0.9f, 1, h);
  if (u > 8 && u < 18)
    for (int i = 0; i < 6; i++) {
      float k = fmodf((u - 8) * 0.45f + i / 6.0f, 1);
      herz(g, 146 + sinf(k * 7 + i) * 24, 150 - k * 110, 5 + (i % 3), FARBE(0xFF5A7A), (uint8_t)(255 * huepf(k)));
    }
  if (u > 9 && u < 22)
    for (int i = 0; i < 18; i++) {
      float x = zufall(i + 1900) * 284 + sinf(u * 2 + i) * 12;
      float y = fmodf(zufall(i + 1950) * 240 + (u - 9) * 40, 250) - 10;
      g.ellipse(x, y, 3, 2, i & 1 ? FARBE(0xFF9FC8) : WEISS, 230, u + i);
    }
  blende(g, u, 24, 142, 140);
}

static void szeneGruseln(Gfx &g, float u) {
  // Schlafzimmer im Schloss, Nacht
  g.verlauf(0, 200, FARBE(0x3A3466), FARBE(0x2A2650));
  g.rundRechteck(30, 30, 50, 66, 20, FARBE(0x1A2250));
  mond(g, 55, 56, 9);
  g.rechteck(0, 200, 284, 40, FARBE(0x5A3A5E));
  // Hans im Bett: weiss immer noch nicht, was Gruseln ist
  Pose p;
  p.zu = u > 12 && u < 15;  // schlaeft kurz ein
  p.mund = u > 17 ? 1 : 0.2f;
  float zit = seg(u, 17, 17.4f) * (1 - seg(u, 26, 28));
  bett(g, 150 + zitter(u, zit * 2), 214, 1.2f, 0, p);
  if (u > 3 && u < 11) {  // Gedankenblase: Gruseln?
    float k = federnd(seg(u, 3, 3.5f)) * (1 - seg(u, 10.5f, 11));
    g.kontur(1.4f, OUT);
    g.kreis(132, 150, 3 * k, WEISS);
    g.ellipse(116, 120, 30 * k, 18 * k, WEISS);
    g.konturAus();
    if (k > 0.6f) { brrr(g, 116, 124, u, 1); g.text(SCHRIFT_M, "?", 116, 128, OUT, 1); }
  }
  // Prinzessin kommt mit dem Eimer
  float px = lerp(320, 222, sanft(seg(u, 11, 14)));
  float kipp = sanft(seg(u, 15.5f, 16.6f)) * -2.2f;
  prinzessin(g, px, 214, 0.9f, -1, 0);
  if (u > 11 && u < 30) eimer(g, px - 16, 170, 1.1f, kipp);
  // Wasser und Fischlein
  if (u > 16.2f && u < 19) {
    float k = seg(u, 16.2f, 19);
    for (int i = 0; i < 14; i++) {
      float x = lerp(px - 26, 130 + (i % 7) * 7, k) + sinf(i * 1.7f) * 6, y = lerp(160, 186, k) + (i / 7) * 4;
      tropfen(g, x, y, 2.4f, FARBE(0x8FD8FF), (uint8_t)(255 * (1 - k * 0.5f)));
    }
  }
  if (u > 16.8f)
    for (int i = 0; i < 5; i++) {
      float k = seg(u, 16.8f + i * 0.1f, 17.8f + i * 0.1f);
      float x = lerp(px - 24, 120 + i * 14, k), y = lerp(160, 184, k) - sinf(k * 3.14159f) * 20;
      if (u > 18) y -= fabsf(sinf(u * 9 + i)) * 6;  // zappeln
      fisch(g, x, y, 0.9f, sinf(u * 10 + i) * 0.8f);
    }
  // Brrr! Jetzt weiss er, was Gruseln ist
  if (zit > 0) {
    brrr(g, 150, 170, u, zit);
    g.text(SCHRIFT_G, "Brrr!", 150, 120, WEISS, 1, (uint8_t)(255 * zit));
  }
  if (u > 26 && u < 32)
    for (int i = 0; i < 6; i++) {
      float k = fmodf((u - 26) * 0.5f + i / 6.0f, 1);
      herz(g, 170 + sinf(k * 7 + i) * 30, 160 - k * 100, 5 + (i % 3), FARBE(0xFF5A7A), (uint8_t)(255 * huepf(k)));
    }
  // Ende
  g.kameraAus();
  if (u < 1.2f) g.iris(150, 150, lerp(0, 330, sanft(u / 1.2f)));
  if (u > 30) g.iris(150, 170, lerp(330, 0, sanft(seg(u, 30, 32.5f))));
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
  if (t < F2) szeneStube(g, t - F1);
  else if (t < F3) szeneTurm(g, t - F2);
  else if (t < F4) szeneWandern(g, t - F3);
  else if (t < F5) szeneKatzen(g, t - F4);
  else if (t < F6) szeneSkelett(g, t - F5);
  else if (t < F7) szeneBett(g, t - F6);
  else if (t < F8) szeneHochzeit2(g, t - F7);
  else szeneGruseln(g, t - F8);
}

// =====================================================================
// Musik und Geraeusche
// =====================================================================
// Spuklied (a-Moll, auf Zehenspitzen)
static const Ton3 MEL_SPUK[] = {
  {69, .5, 45}, {0, .5, 0}, {72, .5, 0}, {0, .5, 0}, {76, .5, 0}, {75, .5, 0}, {76, 1, 0},
  {74, .5, 41}, {0, .5, 0}, {71, .5, 0}, {0, .5, 0}, {68, .5, 0}, {71, .5, 0}, {69, 1, 0},
  {69, .5, 45}, {72, .5, 0}, {76, .5, 0}, {81, .5, 0}, {80, .5, 40}, {76, .5, 0}, {77, .5, 0}, {74, .5, 0},
  {71, .5, 40}, {68, .5, 0}, {71, .5, 0}, {74, .5, 0}, {69, 2, 45}};
// Kegellied (C-Dur, huepfend)
static const Ton3 MEL_KEGEL[] = {
  {72, .5, 48}, {76, .25, 0}, {79, .25, 0}, {84, .5, 0}, {79, .5, 0}, {77, .5, 53}, {81, .25, 0}, {84, .25, 0},
  {81, .5, 0}, {77, .5, 0}, {76, .5, 55}, {79, .5, 0}, {74, .5, 0}, {77, .5, 0}, {72, 1, 48}, {0, 1, 0}};
// Wiegenlied (F-Dur, zum Einschlafen)
static const Ton3 MEL_WIEGE[] = {
  {72, 1.5, 53}, {69, .5, 0}, {72, 2, 0}, {74, 1, 46}, {72, 1, 0}, {69, 2, 0}, {70, 1, 48}, {67, 1, 0},
  {69, 1, 53}, {65, 1, 0}, {67, 4, 48}};

static void ton(Synth &s, float t0, float t1) {
  // 1 Stube
  spieleMelodie(s, M(MEL_WIEGE), F1 + 0.5f, F1 + 7.5f, 0.42f, t0, t1, 0.7f);
  for (float x : {8.0f, 10.0f}) if (bei(x, t0, t1)) {  // Eule: Huhu
      s.ton(x, 392, 0.12f, 0.08f, 0.35f, SINUS, MUSIK, 370);
      s.ton(x + 0.45f, 330, 0.12f, 0.08f, 0.5f, SINUS, MUSIK, 311);
    }
  if (bei(8.6f, t0, t1)) for (int k = 0; k < 12; k++) s.note(8.6f + k * 0.06f, 60 + (k & 1), 0.3f);  // Bruder zittert
  if (bei(13.2f, t0, t1)) for (int k = 0; k < 3; k++) s.note(13.2f + k * 0.3f, 71 + k * 2, 0.6f);  // hm?
  spieleMelodie(s, M(MEL_WANDER), F1 + 19, F2 - 1.5f, 0.26f, t0, t1, 0.9f);
  // 2 Glockenturm
  for (int i = 0; i < 6; i++) {
    float x = F2 + 3.3f + i * 1.4f;
    if (bei(x, t0, t1)) s.glocke(x, i & 1 ? 55 : 60, 1);
  }
  spieleMelodie(s, M(MEL_SPUK), F2 + 11, F2 + 20, 0.3f, t0, t1, 0.9f);
  if (bei(F2 + 13, t0, t1)) s.ton(F2 + 13, 330, 0.1f, 0.4f, 1.2f, SINUS, MUSIK, 220);  // Huuu
  if (bei(F2 + 20.4f, t0, t1)) for (int k = 0; k < 14; k++) s.note(F2 + 20.4f + k * 0.25f, 84 - k * 2, 0.5f);  // purzelt
  if (bei(F2 + 24, t0, t1)) { s.ton(F2 + 24, 120, 0.3f, 0.002f, 0.25f, SINUS, GERAEUSCH, 60); s.note(F2 + 24, 48, 0.9f); }
  if (bei(F2 + 28, t0, t1)) for (int k = 0; k < 3; k++) s.note(F2 + 28 + k * 0.3f, 71 + k * 2, 0.6f);
  // 3 Wanderung
  spieleMelodie(s, M(MEL_WANDER), F3 + 0.5f, F3 + 22, 0.26f, t0, t1, 1);
  spieleMelodie(s, M(MEL_F), F3 + 22, F4 - 1, 0.24f, t0, t1, 0.9f);
  // 4 Katzen
  spieleMelodie(s, M(MEL_SPUK), F4 + 1, F4 + 10, 0.32f, t0, t1, 0.8f);
  for (float x : {F4 + 6.6f, F4 + 7.6f}) if (bei(x, t0, t1)) {  // Miau
      s.ton(x, 700, 0.09f, 0.05f, 0.35f, DREIECK, GERAEUSCH, 950);
      s.ton(x + 0.25f, 900, 0.07f, 0.03f, 0.3f, DREIECK, GERAEUSCH, 600);
    }
  spieleMelodie(s, M(MEL_KEGEL), F4 + 10, F4 + 26, 0.26f, t0, t1, 0.8f);
  for (float x = ceilf(t0 / 0.6f) * 0.6f; x < t1; x += 0.6f)
    if (x > F4 + 10 && x < F4 + 22) s.rauschen(x, 0.03f, HOCHPASS, 3000, 0.04f);  // Karten
  if (bei(F4 + 22, t0, t1)) for (int k = 0; k < 6; k++) s.note(F4 + 22 + k * 0.1f, 76 + k * 2, 0.7f);  // gewonnen
  if (bei(F4 + 28, t0, t1)) for (int i = 0; i < 5; i++) s.ton(F4 + 28 + i * 0.35f, 650 + i * 40, 0.06f, 0.04f, 0.3f, DREIECK, GERAEUSCH, 900);
  spieleMelodie(s, M(MEL_JAGD), F4 + 30, F4 + 34, 0.15f, t0, t1, 0.9f);
  spieleMelodie(s, M(MEL_WIEGE), F4 + 36, F5 - 1, 0.4f, t0, t1, 0.6f);
  // 5 Skelett und Kegeln
  if (bei(F5 + 3, t0, t1)) for (int k = 0; k < 8; k++) s.note(F5 + 3 + k * 0.12f, 88 - k * 3, 0.5f);  // faellt herunter
  if (bei(F5 + 4.3f, t0, t1)) { s.note(F5 + 4.3f, 76, 0.8f); s.note(F5 + 5, 72, 0.8f); }  // klapper
  for (float x = ceilf(t0 / 0.25f) * 0.25f; x < t1; x += 0.25f)
    if (x > F5 + 5.5f && x < F5 + 7) s.ton(x, 1800 + fmodf(x * 37, 3) * 300, 0.05f, 0.002f, 0.05f);  // Knochen klappern
  spieleMelodie(s, M(MEL_KEGEL), F5 + 8, F5 + 36, 0.24f, t0, t1, 0.9f);
  if (bei(F5 + 17, t0, t1)) for (int k = 0; k < 9; k++) s.rauschen(F5 + 17 + k * 0.25f, 0.06f, TIEFPASS, 500, 0.08f);  // rollt
  if (bei(F5 + 19.2f, t0, t1)) for (int k = 0; k < 9; k++) s.note(F5 + 19.2f + k * 0.05f, 72 + k * 2, 0.6f);  // alle neune
  if (bei(F5 + 19.8f, t0, t1)) for (int k = 0; k < 4; k++) s.glocke(F5 + 19.8f + k * 0.15f, 79 + k * 2, 0.6f);
  for (int i = 0; i < 12; i++) {  // Mitternacht
    float x = F5 + 36 + i * 0.2f;
    if (bei(x, t0, t1)) s.glocke(x, 60, 0.7f);
  }
  if (bei(F5 + 38.5f, t0, t1)) for (int k = 0; k < 6; k++) s.note(F5 + 38.5f + k * 0.07f, 96 - k * 2, 0.4f);  // Puff
  spieleMelodie(s, M(MEL_WIEGE), F5 + 41, F6 - 0.5f, 0.4f, t0, t1, 0.6f);
  // 6 Bett faehrt, alter Mann, Amboss, Schatz
  spieleMelodie(s, M(MEL_JAGD), F6 + 3, F6 + 15, 0.2f, t0, t1, 0.8f);
  for (float x = ceilf(t0 / 0.3f) * 0.3f; x < t1; x += 0.3f)
    if (x > F6 + 3 && x < F6 + 15) s.rauschen(x, 0.08f, TIEFPASS, 400, 0.1f);  // Raeder rumpeln
  if (bei(F6 + 16.4f, t0, t1)) { s.ton(F6 + 16.4f, 100, 0.35f, 0.002f, 0.3f, SINUS, GERAEUSCH, 50); s.note(F6 + 16.4f, 45, 1); }
  if (bei(F6 + 17, t0, t1)) for (int k = 0; k < 5; k++) s.note(F6 + 17 + k * 0.12f, 76 + k * 3, 0.7f);  // lacht
  spieleMelodie(s, M(MEL_RIESE), F6 + 19, F6 + 27, 0.4f, t0, t1, 0.9f);
  if (bei(F6 + 27, t0, t1)) { s.ton(F6 + 27, 200, 0.3f, 0.002f, 0.2f, SINUS, GERAEUSCH, 90); s.glocke(F6 + 27, 84, 0.9f); s.glocke(F6 + 27.05f, 88, 0.6f); }
  if (bei(F6 + 28.5f, t0, t1)) for (int k = 0; k < 6; k++) s.note(F6 + 28.5f + k * 0.2f, 67 - (k & 1) * 5, 0.5f);  // au au
  for (int i = 0; i < 3; i++) if (bei(F6 + 36.5f + i * 0.4f, t0, t1)) for (int k = 0; k < 4; k++) s.note(F6 + 36.5f + i * 0.4f + k * 0.06f, 88 + k * 2, 0.5f);
  spieleMelodie(s, M(MEL_F), F6 + 38, F7 - 0.5f, 0.24f, t0, t1, 0.9f);
  // 7 Hochzeit
  spieleMelodie(s, M(MEL_F), F7 + 0.5f, F7 + 8, 0.24f, t0, t1, 1);
  for (int i = 0; i < 14; i++) {
    float x = F7 + 8 + i * 0.6f;
    if (bei(x, t0, t1)) s.glocke(x, i & 1 ? 79 : 72 + (i % 4 == 2) * 4, 0.85f);
  }
  spieleMelodie(s, M(MEL_A), F7 + 17, F8 - 0.5f, 0.26f, t0, t1, 0.8f);
  // 8 Gruseln
  spieleMelodie(s, M(MEL_WIEGE), F8 + 1, F8 + 11, 0.42f, t0, t1, 0.7f);
  if (bei(F8 + 4, t0, t1)) for (int k = 0; k < 3; k++) s.note(F8 + 4 + k * 0.3f, 71 + k * 2, 0.6f);
  if (bei(F8 + 15.6f, t0, t1)) for (int k = 0; k < 6; k++) s.note(F8 + 15.6f + k * 0.08f, 84 - k * 3, 0.6f);
  if (bei(F8 + 16.3f, t0, t1)) {  // Platsch!
    s.rauschen(F8 + 16.3f, 0.7f, TIEFPASS, 3500, 0.35f, 0.7f, 0.005f, 400);
    s.rauschen(F8 + 16.3f, 0.3f, HOCHPASS, 2500, 0.15f);
  }
  for (int i = 0; i < 8; i++) if (bei(F8 + 17.8f + i * 0.35f, t0, t1)) s.ton(F8 + 17.8f + i * 0.35f, 700 + (i % 3) * 120, 0.08f, 0.002f, 0.08f, SINUS, GERAEUSCH, 1300);  // Fische zappeln
  if (bei(F8 + 17.2f, t0, t1)) for (int k = 0; k < 30; k++) s.note(F8 + 17.2f + k * 0.05f, 55 + (k & 1) * 2, 0.45f);  // Brrrr!
  spieleMelodie(s, M(MEL_F), F8 + 22, F8 + 31, 0.24f, t0, t1, 0.9f);
  if (bei(F8 + 32.5f, t0, t1)) { s.glocke(F8 + 32.5f, 72); s.glocke(F8 + 32.8f, 76); s.glocke(F8 + 33.1f, 79); s.glocke(F8 + 33.5f, 84); }
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

const Maerchen FUERCHTEN = {"Von einem, der auszog, das Fürchten zu lernen", FDAUER, FARBE(0x5B3D8F), bild, ton,
                            titelbild, vorschau};

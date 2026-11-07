// Max und Moritz - eine Bubengeschichte in sieben Streichen, nach Wilhelm Busch (1865).
// Als Spieluhr-Bildergeschichte: oben die Verse wie in einem Bilderbuch, dazu Sprechblasen.
// Die harten Stellen (Huehner, Pulver, Muehle) sind nur angedeutet.
#include "maerchen.h"
#include "schriften.h"
#include "figuren.h"
#include "melodien.h"
#include <stdio.h>
#include <initializer_list>

// ---------- Ablauf (Sekunden) ----------
static const float P0 = 0, P1 = 24, P2 = 76, P3 = 122, P4 = 172, P5 = 218, P6 = 262, P7 = 312, P8 = 362, PDAUER = 386;

// ---------- Aussehen ----------
static Aussehen maxLook() {
  Aussehen a; a.weste = FARBE(0x3E8E4E); a.hemd = FARBE(0xF6F0E0); a.hose = FARBE(0x3E4E7A); a.haar = FARBE(0x2E2220);
  a.muetze = 1;
  return a;
}
static Aussehen moritzLook() {
  Aussehen a; a.weste = FARBE(0xC8403A); a.hemd = FARBE(0xF6F0E0); a.hose = FARBE(0x7A5A3A); a.haar = FARBE(0xF2C14E);
  a.muetze = 1;
  return a;
}
static Aussehen boeckLook() {
  Aussehen a; a.weste = FARBE(0x8E6E4E); a.hemd = FARBE(0xE8E0D0); a.hose = FARBE(0x4E4A44); a.haar = FARBE(0x9A9A9A);
  a.hut = FARBE(0x5E6E8E); a.hutD = FARBE(0x45506A); a.feder = FARBE(0x5E6E8E); a.wangen = false;
  return a;
}
static Aussehen laempelLook() {
  Aussehen a; a.weste = FARBE(0x2E2A30); a.hemd = WEISS; a.hose = FARBE(0x2E2A30); a.haar = FARBE(0xBDBDBD); a.muetze = 1;
  a.wangen = false; a.halstuch = WEISS;
  return a;
}
static Aussehen russLook() {  // Laempel nach dem Knall: schwarz und ohne Haar
  Aussehen a = laempelLook(); a.haut = FARBE(0x4A4440); a.haar = FARBE(0x3A3430); a.weste = FARBE(0x2A2420);
  a.hemd = FARBE(0x6E6660); a.halstuch = FARBE(0x6E6660);
  return a;
}
static Aussehen fritzLook() {
  Aussehen a; a.weste = FARBE(0xF2EEE4); a.hemd = FARBE(0xF2EEE4); a.hose = FARBE(0xF2EEE4); a.haar = FARBE(0x9A9A9A);
  a.muetze = 2; a.hut = FARBE(0xE8E0F0); a.hutD = FARBE(0xC9C0D6); a.feder = FARBE(0xE8473D);
  return a;
}
static Aussehen baeckerLook() {
  Aussehen a; a.weste = WEISS; a.hemd = WEISS; a.hose = FARBE(0xE8E0D0); a.haar = FARBE(0x8A5A2E); a.muetze = 1;
  return a;
}
static Aussehen meckeLook() {
  Aussehen a; a.weste = FARBE(0x6E8E4E); a.hemd = FARBE(0xD8C8A8); a.hose = FARBE(0x5A4636); a.haar = FARBE(0x6E4A2A);
  a.hut = FARBE(0xC9A46A); a.hutD = FARBE(0xA8823E); a.feder = FARBE(0xC9A46A);
  return a;
}
static Aussehen muellerLook() {
  Aussehen a; a.weste = FARBE(0xE8E4DC); a.hemd = FARBE(0xF6F2EA); a.hose = FARBE(0xD8D2C6); a.haar = FARBE(0xE0DCD4);
  a.muetze = 1; a.haut = FARBE(0xFBE6D2);
  return a;
}
static const Aussehen MAX = maxLook(), MORITZ = moritzLook(), BOECK = boeckLook(), LAEMPEL = laempelLook(),
                      RUSS = russLook(), FRITZ = fritzLook(), BAECKER = baeckerLook(), MECKE = meckeLook(),
                      MUELLER = muellerLook();

static FrauLook bolteLook() {
  FrauLook l; l.kleid = FARBE(0x4E5A7A); l.schuerze = FARBE(0xF2EEE4); l.haar = FARBE(0xBDBDBD); l.frisur = 3;
  l.tuch = WEISS; l.wangen = true;
  return l;
}
static FrauLook frauBoeckLook() {
  FrauLook l; l.kleid = FARBE(0x9A5A7A); l.schuerze = FARBE(0xE8D8C0); l.haar = FARBE(0x8A6A4A); l.frisur = 2;
  return l;
}
static const FrauLook BOLTE = bolteLook(), FRAU_BOECK = frauBoeckLook();

static Pose mp(const Aussehen &a, float t, float versatz = 0) {
  Pose p;
  p.aussehen = &a;
  p.zu = fmodf(t + 0.4f + versatz, 3.9f) < 0.13f;
  return p;
}
static FrauPose fp(const FrauLook &l, float t) {
  FrauPose p;
  p.look = &l;
  p.zeit = t;
  p.zu = fmodf(t + 1.1f, 4.3f) < 0.13f;
  return p;
}

// ---------- Hilfen ----------
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

// Verse wie in einem Bilderbuch: Pergamentstreifen oben im Bild
static void vers(Gfx &g, std::initializer_list<const char *> zeilen, float k) {
  if (k <= 0.02f) return;
  int n = (int)zeilen.size();
  float h = 9 + n * 17;
  uint8_t a = (uint8_t)(255 * klemme(k));
  g.kameraAus();
#ifdef BOARD_RUND
  // rund: Streifen nur so breit wie der Text, so weit unten, dass er ganz im Kreis liegt
  int tw = 0;
  for (const char *z : zeilen) { int b = g.textBreite(SCHRIFT_K, z); if (b > tw) tw = b; }
  float bw = tw + 30, vx = 142, vy = 4 + h / 2;
  inKreis(vx, vy, bw, h);
  float x0 = vx - bw / 2, y0 = vy - h / 2;
#else
  float bw = 272, x0 = 6, y0 = 4;
#endif
  g.kontur(1.2f, FARBE(0xB8862B));
  g.rundRechteck(x0, y0, bw, h, 7, FARBE(0xFBF1DC), a);
  g.konturAus();
  g.kreis(x0 + 7, y0 + h / 2, 2.2f, FARBE(0xC8485E), a);
  g.kreis(x0 + bw - 7, y0 + h / 2, 2.2f, FARBE(0xC8485E), a);
  if (k > 0.8f) {
    blaseLesbar = 1;
    int i = 0;
    for (const char *z : zeilen) g.text(SCHRIFT_K, z, x0 + bw / 2, y0 + 14 + i++ * 17, FARBE(0x5A3418), 1);
  }
}

static void puff(Gfx &g, float x, float y, float k, Farbe c = WEISS, float gr = 1) {
  if (k <= 0 || k >= 1) return;
  for (int i = 0; i < 9; i++) {
    float a = i * 0.698f;
    g.kreis(x + cosf(a) * k * 34 * gr, y + sinf(a) * k * 28 * gr, (9 - 6 * k) * gr, c, (uint8_t)(220 * (1 - k)));
  }
}

static void feder(Gfx &g, float x, float y, float w, Farbe c, uint8_t a = 255) {
  g.ellipse(x, y, 4, 1.6f, c, a, w);
  g.linie(x - cosf(w) * 4, y - sinf(w) * 4, x + cosf(w) * 4, y + sinf(w) * 4, 0.6f, FARBE(0xA8A090), a);
}

// ---------- Max und Moritz ----------
// Moritz hat die blonde Haartolle, Max ist der Dunkle
static void bub(Gfx &g, bool moritz, float x, float y, float s, float dir, const Pose &p, float *hx = nullptr,
                float *hy = nullptr) {
  schneiderlein(g, x, y, s, dir, p, hx, hy);
  auto PX = [&](float d) { return x + d * s * dir; };
  auto PY = [&](float d) { return y + d * s; };
  if (moritz) {  // die Tolle
    g.kontur(1.1f * s, OUT);
    g.kurve(PX(2), PY(-84), PX(4), PY(-95), PX(11), PY(-94), 4.2f * s, MORITZ.haar);
    g.konturAus();
    g.kreis(PX(10.5f), PY(-93.5f), 2.4f * s, MORITZ.haar);
  } else {  // Max: glatter Haarschopf, Stirnfransen
    g.ellipse(PX(2), PY(-76), 16 * s, 7 * s, MAX.haar);
    g.ellipse(PX(11), PY(-71), 6 * s, 4 * s, MAX.haar, 255, 0.4f * dir);
  }
}
static Pose bp(bool moritz, float t) {
  Pose p = mp(moritz ? MORITZ : MAX, t, moritz ? 1.7f : 0);
  p.mund = 0.15f;
  return p;
}
// die beiden kichern (Schultern huepfen)
static float kicher(float t, int i) { return fabsf(sinf(t * 9 + i * 1.3f)) * 2.5f; }

// ---------- Tiere ----------
static void huhn(Gfx &g, float x, float y, float s, float dir, float t, float pick = 0, bool hahn = false,
                 float kipp = 0, bool zu = false) {
  auto PX = [&](float d) { return x + d * s * dir; };
  auto PY = [&](float d) { return y + d * s; };
  Farbe fed = hahn ? FARBE(0xC9702E) : FARBE(0xF6F0E4);
  if (kipp == 0) {
    g.linie(PX(-2), PY(-8), PX(-3), PY(-1), 1.4f * s, FARBE(0xE0A23C));
    g.linie(PX(3), PY(-8), PX(4), PY(-1), 1.4f * s, FARBE(0xE0A23C));
  }
  // Schwanz
  if (hahn) {
    for (int i = 0; i < 4; i++)
      g.kurve(PX(-9), PY(-16), PX(-18 - i * 2), PY(-28 + i * 4), PX(-16 - i * 3), PY(-12 + i * 3), 3 * s,
              i & 1 ? FARBE(0x2E6E4E) : FARBE(0x1E3A5A));
  } else {
    float sw[] = {PX(-8), PY(-15), PX(-15), PY(-24), PX(-13), PY(-12)};
    g.polygon(sw, 3, fed);
  }
  g.kontur(1.1f * s, OUT);
  g.ellipse(PX(0), PY(-14), 11 * s, 8 * s, fed, 255, kipp);
  g.konturAus();
  g.ellipse(PX(-2), PY(-14), 6 * s, 4 * s, mischeFarbe(fed, OUT, 0.15f), 255, kipp);  // Fluegel
  float kx = PX(8 + pick * 4), ky = PY(-22 + pick * 14);
  g.kontur(1.0f * s, OUT);
  g.kreis(kx, ky, 5 * s, fed);
  g.konturAus();
  g.kreis(kx + 0.5f * s * dir, ky - 5 * s, (hahn ? 3.2f : 2.2f) * s, FARBE(0xE0302A));  // Kamm
  if (hahn) g.kreis(kx - 2 * s * dir, ky - 5.5f * s, 2.6f * s, FARBE(0xE0302A));
  float sn[] = {kx + 4 * s * dir, ky - 1 * s, kx + 9 * s * dir, ky + 0.5f * s, kx + 4 * s * dir, ky + 2 * s};
  g.polygon(sn, 3, FARBE(0xF2B230));
  g.kreis(kx + 3 * s * dir, ky + 3.5f * s, 1.6f * s, FARBE(0xE0302A));  // Kehllappen
  if (zu) g.bogen(kx + 1.5f * s * dir, ky - 0.5f * s, 1.5f * s, 3.5f, 5.9f, 1 * s, OUT);
  else g.kreis(kx + 1.5f * s * dir, ky - 1 * s, 1.1f * s, SCHWARZ);
}

// Huhn haengt am Faden am Ast (Kopf nach unten, Augen zu)
static void huhnHaengt(Gfx &g, float x, float y, float s, float t, float schw) {
  g.linie(x, y, x + schw * 3, y + 12 * s, 0.8f, FARBE(0xE8E0D0));
  g.kontur(1.1f * s, OUT);
  g.ellipse(x + schw * 3, y + 22 * s, 7 * s, 10 * s, FARBE(0xF6F0E4), 255, schw * 0.2f);
  g.konturAus();
  g.kreis(x + schw * 3, y + 34 * s, 4 * s, FARBE(0xF6F0E4));
  g.kreis(x + schw * 3, y + 38 * s, 2 * s, FARBE(0xE0302A));
  g.bogen(x + schw * 3 + 1.5f * s, y + 33 * s, 1.4f * s, 0.3f, 2.8f, 0.9f * s, OUT);
  g.linie(x + schw * 3 - 2 * s, y + 13 * s, x + schw * 3 - 3 * s, y + 9 * s, 1.2f * s, FARBE(0xE0A23C));
  g.linie(x + schw * 3 + 2 * s, y + 13 * s, x + schw * 3 + 3 * s, y + 9 * s, 1.2f * s, FARBE(0xE0A23C));
}

static void bratHuhn(Gfx &g, float x, float y, float s) {
  g.ellipse(x, y, 9 * s, 5.5f * s, FARBE(0xC9702E));
  g.ellipse(x - 1 * s, y - 1.5f * s, 5 * s, 2.5f * s, FARBE(0xE0A050));
  g.linie(x + 7 * s, y - 2 * s, x + 11 * s, y - 5 * s, 2 * s, FARBE(0xC9702E));
  g.kreis(x + 11.5f * s, y - 5.5f * s, 1.6f * s, FARBE(0xF6E8D0));
}

static void spitz(Gfx &g, float x, float y, float s, float dir, float t, float lauf = -1, bool schlaeft = false) {
  auto PX = [&](float d) { return x + d * s * dir; };
  auto PY = [&](float d) { return y + d * s; };
  Farbe f = FARBE(0xFBF8F2);
  for (int i = 0; i < 4; i++) {
    float bx = i < 2 ? -10 + i * 5 : 6 + (i - 2) * 5;
    float sw = lauf >= 0 ? sinf(lauf * 6.2832f + i * 1.7f) * 5 : 0;
    if (!schlaeft) g.linie(PX(bx), PY(-8), PX(bx + sw), PY(-1), 3 * s, i & 1 ? f : FARBE(0xE8E2D8));
  }
  g.kontur(1.1f * s, OUT);
  g.ellipse(PX(0), PY(schlaeft ? -6 : -11), 14 * s, (schlaeft ? 6 : 7) * s, f);
  float kx = PX(schlaeft ? 13 : 13), ky = PY(schlaeft ? -8 : -19);
  g.kreis(kx, ky, 6.5f * s, f);
  g.konturAus();
  // Ringelschwanz
  g.kontur(1, OUT);
  g.kreis(PX(-13), PY(schlaeft ? -10 : -18), 4.5f * s, f);
  g.konturAus();
  float o1[] = {kx - 4 * s * dir, ky - 4 * s, kx - 1 * s * dir, ky - 12 * s, kx + 1 * s * dir, ky - 4 * s};
  float o2[] = {kx + 1 * s * dir, ky - 4 * s, kx + 4 * s * dir, ky - 11 * s, kx + 5 * s * dir, ky - 3 * s};
  g.polygon(o1, 3, f);
  g.polygon(o2, 3, f);
  g.kreis(kx + 6 * s * dir, ky + 1 * s, 1.6f * s, SCHWARZ);
  if (schlaeft) g.bogen(kx + 2 * s * dir, ky - 1 * s, 1.6f * s, 0.3f, 2.8f, 1 * s, OUT);
  else g.kreis(kx + 2 * s * dir, ky - 1.5f * s, 1.2f * s, SCHWARZ);
}

static void gans(Gfx &g, float x, float y, float s, float dir, float t, float schwimm = 1, float hals = 0) {
  auto PX = [&](float d) { return x + d * s * dir; };
  auto PY = [&](float d) { return y + d * s; };
  float w = sinf(t * 2) * 1.2f * schwimm;
  g.kontur(1.1f * s, OUT);
  g.ellipse(PX(0), PY(-8 + w), 14 * s, 7 * s, WEISS);
  g.konturAus();
  float sw[] = {PX(-12), PY(-10 + w), PX(-18), PY(-16 + w), PX(-14), PY(-6 + w)};
  g.polygon(sw, 3, WEISS);
  g.linie(PX(9), PY(-11 + w), PX(12 + hals * 6), PY(-24 + w + hals * 10), 4 * s, WEISS);
  g.kontur(1, OUT);
  g.kreis(PX(12 + hals * 6), PY(-26 + w + hals * 10), 4 * s, WEISS);
  g.konturAus();
  float sn[] = {PX(15 + hals * 6), PY(-27 + w + hals * 10), PX(21 + hals * 6), PY(-25 + w + hals * 10), PX(15 + hals * 6), PY(-23.5f + w + hals * 10)};
  g.polygon(sn, 3, FARBE(0xF28A2A));
  g.kreis(PX(13 + hals * 6), PY(-27.5f + w + hals * 10), 0.9f * s, SCHWARZ);
}

static void ente(Gfx &g, float x, float y, float s, float dir, float t, float pick) {
  auto PX = [&](float d) { return x + d * s * dir; };
  auto PY = [&](float d) { return y + d * s; };
  g.linie(PX(-1), PY(-4), PX(-1), PY(0), 1.4f * s, FARBE(0xF28A2A));
  g.linie(PX(3), PY(-4), PX(3), PY(0), 1.4f * s, FARBE(0xF28A2A));
  g.kontur(1.0f * s, OUT);
  g.ellipse(PX(0), PY(-9), 10 * s, 6 * s, FARBE(0xB89A6A));
  float kx = PX(8 + pick * 3), ky = PY(-16 + pick * 10);
  g.kreis(kx, ky, 4.2f * s, FARBE(0x2E8A4E));
  g.konturAus();
  g.rechteck(PX(6) < PX(10) ? PX(6) : PX(10), PY(-12 + pick * 8), 4 * s, 1.5f * s, WEISS);
  float sn[] = {kx + 3 * s * dir, ky - 0.5f * s, kx + 8 * s * dir, ky + 1 * s, kx + 3 * s * dir, ky + 2 * s};
  g.polygon(sn, 3, FARBE(0xF2B230));
  g.kreis(kx + 1.5f * s * dir, ky - 1 * s, 0.9f * s, SCHWARZ);
}

static void maikaefer(Gfx &g, float x, float y, float s, float t, float flug = 0) {
  if (flug > 0) {
    float f = sinf(t * 40) * 0.5f + 0.5f;
    g.ellipse(x - 3 * s, y - 4 * s, 5 * s, 2 * s, WEISS, (uint8_t)(120 + 60 * f), -0.6f);
    g.ellipse(x + 3 * s, y - 4 * s, 5 * s, 2 * s, WEISS, (uint8_t)(120 + 60 * f), 0.6f);
  }
  for (int i = -1; i <= 1; i++) g.linie(x - 4 * s, y + i * 2 * s, x - 7 * s, y + i * 3 * s + 1, 0.8f * s, OUT);
  for (int i = -1; i <= 1; i++) g.linie(x + 4 * s, y + i * 2 * s, x + 7 * s, y + i * 3 * s + 1, 0.8f * s, OUT);
  g.ellipse(x, y, 4.2f * s, 5.5f * s, FARBE(0x9A4E1E));
  g.linie(x, y - 4 * s, x, y + 5 * s, 0.7f * s, FARBE(0x5A2A0E));
  g.kreis(x, y - 5.5f * s, 2.6f * s, SCHWARZ);
  g.linie(x - 1 * s, y - 7.5f * s, x - 3 * s, y - 10 * s, 0.7f * s, OUT);
  g.linie(x + 1 * s, y - 7.5f * s, x + 3 * s, y - 10 * s, 0.7f * s, OUT);
}

// ---------- Kulissen ----------
static void haus(Gfx &g, float x, float y, float w, float h, Farbe wand, Farbe dach, float t, bool schorn = true,
                 float rauch = 0) {
  if (schorn) schornstein(g, x + w * 0.75f, y - h - w * 0.12f, t, rauch);
  hauswand(g, x, y - h, w, h, wand, true);
  ziegeldach(g, x, x + w, y - h + 2, w * 0.5f, dach);
  fenster(g, x + w * 0.12f, y - h * 0.4f, w * 0.2f, h * 0.22f, 0, true, false);
  fenster(g, x + w * 0.68f, y - h * 0.4f, w * 0.2f, h * 0.22f, 0, true, false);
  holztuer(g, x + w * 0.4f, y, w * 0.2f, h * 0.5f, HOLZ, true);
}

static void zaun(Gfx &g, float x0, float x1, float y) {
  g.rechteck(x0, y - 22, x1 - x0, 3, FARBE(0xB08050));
  g.rechteck(x0, y - 12, x1 - x0, 3, FARBE(0xB08050));
  for (float x = x0; x < x1; x += 13) {
    float l[] = {x, y, x, y - 26, x + 3, y - 30, x + 6, y - 26, x + 6, y};
    g.polygon(l, 5, FARBE(0xD8B080));
  }
}

static void dorf(Gfx &g, float t, float abend = 0) {
  himmel(g, abend, t, 196);
  if (abend < 0.5f) sonne(g, 248, 32, 12, t);
  wolke(g, fmodf(60 + t * 4, 340) - 30, 40, 0.8f);
  wolke(g, fmodf(210 + t * 3, 340) - 30, 26, 0.6f);
  huegel(g, 0, 0.3f, 196, 30, 120, FARBE(0x8ACB6A));
  haus(g, 10, 196, 64, 46, FARBE(0xF4E4C4), FARBE(0xC0503A), t);
  haus(g, 200, 196, 70, 50, FARBE(0xF0DCC0), FARBE(0xA8463A), t, true, 1);
  g.kontur(1.4f, OUT);  // Kirchturm hinten
  g.rechteck(128, 120, 22, 76, FARBE(0xE8E0D0));
  float sp[] = {124, 122, 139, 80, 154, 122};
  g.polygon(sp, 3, FARBE(0x5B7FC7));
  g.konturAus();
  g.linie(139, 80, 139, 70, 1.5f, GOLD);
  g.linie(135, 74, 143, 74, 1.5f, GOLD);
  boden(g, 196);
}

// Witwe Boltes Hof mit dem duerren Apfelbaum
static void bolteHof(Gfx &g, float t) {
  himmel(g, 0, t, 196);
  sonne(g, 40, 32, 12, t);
  wolke(g, fmodf(150 + t * 4, 340) - 30, 34, 0.7f);
  haus(g, 182, 196, 92, 64, FARBE(0xF4E4C4), FARBE(0xC0503A), t, true, 0.6f);
  boden(g, 196);
  // der Apfelbaum mit dem waagrechten duerren Ast
  g.kontur(1.4f, OUT);
  g.linie(70, 200, 74, 110, 9, HOLZ_D);
  g.konturAus();
  g.linie(72, 118, 150, 104, 4, HOLZ_D);
  g.linie(120, 109, 132, 94, 2.5f, HOLZ_D);
  g.kreis(52, 92, 26, FARBE(0x5DAE48));
  g.kreis(80, 80, 28, FARBE(0x4CA85A));
  g.kreis(66, 108, 18, FARBE(0x5DAE48));
  for (int i = 0; i < 5; i++) g.kreis(46 + i * 10, 84 + (i & 1) * 14, 3, FARBE(0xE8473D));
  zaun(g, -4, 60, 214);
}

static void kueche(Gfx &g, float t, float feuer = 1) {
  g.verlauf(0, 200, FARBE(0xE8D2B0), FARBE(0xD6B88E));
  g.rechteck(0, 200, 284, 40, FARBE(0x9E7656));
  for (int i = 0; i < 7; i++) g.rechteck(i * 42, 200, 1.5f, 40, FARBE(0x7E5A3E));
  // Herd mit Schornstein bis oben
  g.kontur(1.8f, OUT);
  g.rechteck(150, 0, 56, 132, FARBE(0xB8A898));
  g.rundRechteck(130, 132, 96, 70, 4, FARBE(0xA89888));
  g.konturAus();
  g.rundRechteck(150, 160, 56, 40, 14, FARBE(0x2E2620));
  for (int i = 0; i < 3; i++) {
    float fx = 164 + i * 14, h = (12 + 5 * sinf(t * 9 + i * 1.7f)) * feuer;
    float f[] = {fx - 6, 196, fx + 6, 196, fx + sinf(t * 7 + i) * 2, 196 - h};
    g.polygon(f, 3, FARBE(0xFF8A2A));
  }
  g.rechteck(20, 70, 90, 5, HOLZ_D);
  for (int i = 0; i < 4; i++) {
    g.kontur(1.2f, OUT);
    g.rundRechteck(26 + i * 22, 52, 16, 18, 4, i & 1 ? FARBE(0xC0632E) : FARBE(0x9AA6B8));
    g.konturAus();
  }
  g.kontur(1.4f, OUT);  // Kellertuer
  g.rundRechteck(30, 130, 44, 70, 5, HOLZ);
  g.konturAus();
  g.kreis(66, 166, 2.4f, GOLD);
}

static void pfanne(Gfx &g, float x, float y, int huehner, float t) {
  g.linie(x + 30, y - 4, x + 62, y - 8, 4, FARBE(0x3A3436));
  g.kontur(1.4f, OUT);
  g.ellipse(x, y - 4, 32, 7, FARBE(0x3A3436));
  g.konturAus();
  g.ellipse(x, y - 6, 28, 4.5f, FARBE(0x5A4A3A));
  for (int i = 0; i < huehner; i++) bratHuhn(g, x - 18 + i * 12, y - 8, 0.9f);
  for (int i = 0; i < 3; i++) {
    float k = fmodf(t * 0.6f + i * 0.33f, 1);
    g.kurve(x - 10 + i * 10, y - 12 - k * 20, x - 14 + i * 10, y - 18 - k * 20, x - 8 + i * 10, y - 24 - k * 20, 1.4f,
            WEISS, (uint8_t)(160 * (1 - k)));
  }
}

static void bach(Gfx &g, float t, float steg, float einsatz = 0) {
  himmel(g, 0, t, 170);
  sonne(g, 244, 30, 12, t);
  haus(g, 196, 170, 76, 52, FARBE(0xF0DCC0), FARBE(0x6E7EA8), t);  // Boecks Haus
  g.rechteck(0, 170, 284, 70, GRAS);
  // der Bach
  g.verlauf(186, 240, FARBE(0x6EB8E8), FARBE(0x3E88C8));
  for (int i = 0; i < 9; i++) {
    float x = fmodf(i * 37 + t * 18, 300) - 10;
    g.linie(x, 196 + (i % 3) * 12, x + 14, 196 + (i % 3) * 12, 1.4f, FARBE(0xBFE6FF), 160);
  }
  // Ufer
  g.ellipse(18, 192, 40, 10, GRAS_D);
  g.ellipse(270, 192, 40, 10, GRAS_D);
  // der Steg (bricht in der Mitte, wenn eingesaegt)
  if (steg > 0) {
    float knick = einsatz;
    float l[] = {20, 186, 142, 186 + knick * 26, 142, 192 + knick * 26, 20, 192};
    float r[] = {142, 186 + knick * 26, 264, 186, 264, 192, 142, 192 + knick * 26};
    g.kontur(1.2f, OUT);
    g.polygon(l, 4, HOLZ);
    g.polygon(r, 4, HOLZ);
    g.konturAus();
    if (steg > 1 && knick == 0) g.linie(140, 186, 144, 192, 1.4f, OUT);  // der Saegeschnitt
  }
}

static void stube(Gfx &g, float t, float nacht = 0) {
  g.verlauf(0, 200, mischeFarbe(FARBE(0xE6D6B6), FARBE(0x4E4466), nacht), mischeFarbe(FARBE(0xD6BE96), FARBE(0x3A3050), nacht));
  for (int i = 0; i < 12; i++) g.rechteck(i * 24 + 10, 0, 1.2f, 200, FARBE(0xC9B48E), (uint8_t)(120 * (1 - nacht)));  // Tapete
  g.rechteck(0, 200, 284, 40, FARBE(0x9E7656));
  g.kontur(1.8f, OUT);
  g.rundRechteck(196, 40, 60, 70, 4, HOLZ);
  g.konturAus();
  g.rundRechteck(201, 45, 50, 60, 2, mischeFarbe(FARBE(0xBFE6FF), FARBE(0x22285A), nacht));
  g.rechteck(224, 45, 3, 60, HOLZ);
  if (nacht > 0.5f) g.kreis(240, 60, 7, FARBE(0xFFF1B0));
}

static void lehnstuhl(Gfx &g, float x, float y) {
  g.kontur(1.4f, OUT);
  g.rundRechteck(x - 26, y - 78, 52, 60, 12, FARBE(0x8E2F3A));
  g.rundRechteck(x - 32, y - 34, 64, 22, 8, FARBE(0xA8404A));
  g.konturAus();
  g.rechteck(x - 26, y - 12, 5, 12, HOLZ_D);
  g.rechteck(x + 21, y - 12, 5, 12, HOLZ_D);
}

static void pfeife(Gfx &g, float x, float y, float s, float dir, bool qualm, float t) {
  g.linie(x, y, x + 18 * s * dir, y + 4 * s, 1.8f * s, FARBE(0x6E4A2A));
  g.kontur(1, OUT);
  g.rundRechteck(x + 16 * s * dir - 4 * s, y - 4 * s, 8 * s, 12 * s, 2 * s, FARBE(0xF2EEE4));
  g.konturAus();
  if (qualm)
    for (int i = 0; i < 3; i++) {
      float k = fmodf(t * 0.5f + i * 0.33f, 1);
      g.kreis(x + 16 * s * dir + sinf(k * 6) * 4, y - 6 * s - k * 30, 2 + k * 5, FARBE(0xE8E4EE), (uint8_t)(150 * (1 - k)));
    }
}

static void bett(Gfx &g, float x, float y, float decke) {
  g.kontur(1.6f, OUT);
  g.rundRechteck(x - 60, y - 60, 12, 60, 3, HOLZ);
  g.rundRechteck(x + 48, y - 44, 12, 44, 3, HOLZ);
  g.rundRechteck(x - 56, y - 30, 112, 18, 3, HOLZ);
  g.konturAus();
  g.rundRechteck(x - 52, y - 40, 26, 14, 6, WEISS);  // Kissen
  g.rundRechteck(x - 30 - decke * 10, y - 38, 80, 14 + decke * 2, 5, FARBE(0x8FA6E8));
  for (int i = 0; i < 4; i++) g.kreis(x - 16 + i * 18, y - 31, 2, FARBE(0xF2F2FF));
}

static void backstube(Gfx &g, float t) {
  g.verlauf(0, 200, FARBE(0xF2E2C8), FARBE(0xE0C8A0));
  g.rechteck(0, 200, 284, 40, FARBE(0xA89070));
  // Backofen
  g.kontur(1.8f, OUT);
  g.rundRechteck(170, 92, 104, 110, 8, FARBE(0xC0705A));
  g.konturAus();
  for (int r = 0; r < 6; r++) for (int c = 0; c < 5; c++)
      g.rechteck(172 + c * 20 + (r & 1) * 10, 96 + r * 17, 18, 1.4f, FARBE(0x9A4E3E));
  g.rundRechteck(196, 140, 52, 44, 22, FARBE(0x2E2220));
  for (int i = 0; i < 3; i++) g.ellipse(210 + i * 12, 180, 5, 3 + 2 * sinf(t * 8 + i), FARBE(0xFF8A2A));
  // Schornstein
  g.rechteck(206, 0, 34, 92, FARBE(0xB0604A));
  // Regal mit Brezeln
  g.rechteck(14, 60, 110, 5, HOLZ_D);
  for (int i = 0; i < 4; i++) {
    float bx = 28 + i * 26;
    g.bogen(bx - 4, 52, 5, 0, 6.29f, 2.5f, FARBE(0xB8702E));
    g.bogen(bx + 4, 52, 5, 0, 6.29f, 2.5f, FARBE(0xB8702E));
  }
}

static void kiste(Gfx &g, float x, float y, float w, float h, Farbe inhalt, const char *was) {
  g.kontur(1.4f, OUT);
  g.rechteck(x, y - h, w, h, HOLZ);
  g.konturAus();
  g.rechteck(x + 3, y - h + 3, w - 6, 8, inhalt);
  g.text(SCHRIFT_K, was, x + w / 2, y - 6, FARBE(0xF6E8D0), 1);
}

static void muehle(Gfx &g, float x, float y, float t, float dreh) {
  g.kontur(1.8f, OUT);
  g.rechteck(x - 40, y - 86, 80, 86, FARBE(0xE0D4BC));
  float d[] = {x - 50, y - 84, x, y - 124, x + 50, y - 84};
  g.polygon(d, 3, FARBE(0x8E5A3A));
  g.konturAus();
  // Trichter oben am Dach
  float tr[] = {x - 22, y - 126, x + 22, y - 126, x + 6, y - 104, x - 6, y - 104};
  g.kontur(1.2f, OUT);
  g.polygon(tr, 4, HOLZ);
  g.konturAus();
  // Wasserrad
  float rx = x - 54, ry = y - 30;
  g.kontur(1.4f, OUT);
  g.kreis(rx, ry, 30, FARBE(0x8E6E4E));
  g.konturAus();
  g.kreis(rx, ry, 24, FARBE(0xA8D8F0));
  for (int i = 0; i < 8; i++) {
    float a = dreh + i * 0.785f;
    g.linie(rx, ry, rx + cosf(a) * 30, ry + sinf(a) * 30, 3, FARBE(0x6E4A2A));
  }
  g.kreis(rx, ry, 5, FARBE(0x6E4A2A));
  // Auslauf unten
  g.rechteck(x + 30, y - 22, 20, 8, HOLZ);
  g.rundRechteck(x - 10, y - 50, 20, 50, 8, HOLZ);
}

static void sack(Gfx &g, float x, float y, float s, Farbe c = FARBE(0xD8C09A), bool loch = false, bool zappel = false,
                 float t = 0) {
  float z = zappel ? sinf(t * 14) * 2 : 0;
  g.kontur(1.2f, OUT);
  g.ellipse(x + z, y - 16 * s, 13 * s, 17 * s, c);
  g.konturAus();
  g.ellipse(x + z, y - 32 * s, 6 * s, 3 * s, mischeFarbe(c, OUT, 0.25f));
  g.linie(x - 4 * s + z, y - 31 * s, x + 4 * s + z, y - 33 * s, 1.6f * s, FARBE(0x6E4A2A));
  if (loch) g.ellipse(x + 6 * s, y - 6 * s, 2.5f * s, 2 * s, FARBE(0x4A3420));
}

// =====================================================================
// Szenen
// =====================================================================
// Vorwort: "Ach, was muss man oft von boesen Kindern hoeren oder lesen!"
static void szeneVorwort(Gfx &g, float u) {
  dorf(g, u);
  for (int b = 0; b < 2; b++) {
    Pose p = bp(b, u);
    float x = lerp(-40 - b * 32, 120 + b * 36, sanft(seg(u, 0.5f, 6)));
    p.lauf = u < 6 ? fmodf(u * 1.8f + b * 0.5f, 1) : -1;
    if (u > 6) { p.mund = 0.6f; p.zu = true; p.armV = 1.2f + b * 0.3f; }
    float k = u > 6 ? kicher(u, b) : 0;
    bub(g, b, x, 214 - k, 0.8f, b ? -1 : 1, p);
  }
  if (u > 12) {
    float k = seg(u, 12, 13.5f);
    g.text(SCHRIFT_M, "Max", 112, 110 - huepf(k) * 8, MAX.weste, 1, (uint8_t)(255 * k));
    g.text(SCHRIFT_M, "Moritz", 170, 104 - huepf(k) * 8, MORITZ.weste, 1, (uint8_t)(255 * k));
  }
  vers(g, {"Ach, was muss man oft von bösen", "Kindern hören oder lesen!"}, aufzu(u, 1, 7));
  vers(g, {"Wie zum Beispiel hier von diesen,", "Welche Max und Moritz hießen."}, aufzu(u, 7.2f, 13.6f));
  vers(g, {"Die, anstatt durch weise Lehren", "Sich zum Guten zu bekehren,"}, aufzu(u, 13.8f, 18.4f));
  vers(g, {"Oftmals noch darüber lachten", "Und sich heimlich lustig machten."}, aufzu(u, 18.6f, 23.6f));
  blende(g, u, 24, 142, 150);
}

// Erster Streich: die Huehner der Witwe Bolte
static void szeneStreich1(Gfx &g, float u) {
  bolteHof(g, u);
  bool haengen = u > 30;
  // Max und Moritz hinter dem Zaun, binden Brot an Faeden
  if (u < 30) {
    for (int b = 0; b < 2; b++) {
      Pose p = bp(b, u);
      p.armV = u > 6 && u < 12 ? 1.6f + fabsf(sinf(u * 5 + b)) * 0.3f : 0.8f;
      p.mund = u > 12 && u < 18 ? 0.6f : 0.15f;
      bub(g, b, 18 + b * 26, 214, 0.72f, 1, p);
    }
  } else {
    for (int b = 0; b < 2; b++) {  // sie verstecken sich und kichern
      Pose p = bp(b, u);
      p.zu = true;
      p.mund = 0.6f;
      bub(g, b, 10 + b * 22, 214 - kicher(u, b), 0.62f, 1, p);
    }
    zaun(g, -4, 60, 214);
  }
  // die Brotbrocken an den Faeden
  if (u > 8 && u < 22) {
    for (int i = 0; i < 4; i++) {
      float bx = 70 + i * 26;
      g.linie(bx, 210, bx + 13, 212, 0.8f, FARBE(0xE8E0D0));
      g.rundRechteck(bx - 3, 207, 6, 5, 2, FARBE(0xD8A860));
    }
  }
  if (!haengen) {
    // die Huehner kommen, picken, schlucken, flattern
    float flatter = seg(u, 22, 26);
    for (int i = 0; i < 4; i++) {
      bool hahn = i == 3;
      float x = lerp(300 + i * 20, 74 + i * 26, sanft(seg(u, 14 + i * 0.5f, 19 + i * 0.5f)));
      float pick = u > 19.5f && u < 22 ? huepf(fmodf(u * 2 + i * 0.3f, 1)) : 0;
      float y = 214 - flatter * (60 + i * 8) - (flatter > 0 ? fabsf(sinf(u * 14 + i)) * 6 : 0);
      x = lerp(x, 96 + i * 14, flatter);
      huhn(g, x, y, 1, -1, u, pick, hahn);
      if (flatter > 0) for (int f = 0; f < 3; f++) feder(g, x + sinf(u * 3 + f * 2 + i) * 16, y - 10 + f * 8, u * 2 + f, WEISS, 200);
    }
    if (flatter > 0) for (int i = 0; i < 4; i++) g.linie(96 + i * 14, 214 - flatter * 60, 96 + (i + 1) * 14, 214 - flatter * 64, 0.8f, FARBE(0xE8E0D0));
  } else {
    // sie haengen am duerren Ast
    for (int i = 0; i < 4; i++) huhnHaengt(g, 100 + i * 12, 106, 0.8f, u, sinf(u * 1.5f + i) * 0.6f);
    for (int f = 0; f < 6; f++) {
      float k = fmodf(u * 0.3f + f / 6.0f, 1);
      feder(g, 90 + f * 10 + sinf(k * 8) * 6, 120 + k * 90, k * 6, WEISS, (uint8_t)(220 * (1 - k)));
    }
    // die Witwe Bolte weint
    if (u > 33) {
      FrauPose w = fp(BOLTE, u);
      w.weint = true;
      w.armV = 2.4f;
      w.mund = 0.5f;
      frau(g, lerp(330, 196, sanft(seg(u, 33, 36))), 214, 1, -1, w);
    }
  }
  g.kameraAus();
  vers(g, {"Mancher gibt sich viele Müh'", "Mit dem lieben Federvieh;"}, aufzu(u, 0.6f, 6.4f));
  vers(g, {"Max und Moritz dachten nun:", "Was ist hier jetzt wohl zu tun?"}, aufzu(u, 6.6f, 12.4f));
  blase(g, 90, 52, {"Kikeriki! Kikikerikih!!"}, aufzu(u, 14, 17), 230, 170);
  vers(g, {"Jedes legt noch schnell ein Ei", "Und dann kommt der Tod herbei."}, aufzu(u, 27, 32.6f));
  vers(g, {"Fließet aus dem Aug', ihr Tränen!", "All mein Hoffen, all mein Sehnen,"}, aufzu(u, 37, 43.6f));
  vers(g, {"Meines Lebens schönster Traum", "Hängt an diesem Apfelbaum!"}, aufzu(u, 43.8f, 51.4f));
  blende(g, u, 52, 120, 150);
}

// Zweiter Streich: die gebratenen Huehner durch den Schornstein, der Spitz bekommt die Schuld
static void szeneStreich2(Gfx &g, float u) {
  if (u < 34) {
    kueche(g, u);
    int drin = 4;
    if (u > 14) drin = 3;
    if (u > 17) drin = 2;
    if (u > 20) drin = 1;
    if (u > 23) drin = 0;
    pfanne(g, 178, 134, drin, u);
    // die Angel durch den Schornstein
    if (u > 11 && u < 24) {
      float runter = fmodf(u - 11, 3) < 1.5f ? seg(fmodf(u - 11, 3), 0, 1.2f) : 1 - seg(fmodf(u - 11, 3), 1.5f, 2.8f);
      float hy = lerp(-4, 124, runter);
      g.linie(178, -4, 178, hy, 0.8f, FARBE(0x3A3436));
      g.bogen(178, hy + 3, 3, 0, 3.14f, 1.2f, FARBE(0x8E94A6));
      if (fmodf(u - 11, 3) > 1.5f) bratHuhn(g, 178, hy + 8, 0.9f);
    }
    // der Spitz schlaeft vor dem Herd; spaeter bekommt er die Schuld
    float wach = seg(u, 27, 28);
    if (u < 29) spitz(g, 110, 214, 1, 1, u, -1, wach < 1);
    // Witwe Bolte: geht in den Keller, kommt zurueck
    FrauPose w = fp(BOLTE, u);
    if (u < 8) {
      w.armV = 1.6f;
      frau(g, lerp(190, 52, seg(u, 4, 7.6f)), 214, 1, -1, w);
    } else if (u > 24) {
      float zur = sanft(seg(u, 24, 26));
      w.armV = u > 28 ? 2.6f + sinf(u * 14) * 0.4f : 1.6f;
      w.mund = u > 26 ? 0.8f : 0;
      w.boese = u > 26 ? 1 : 0;
      float wx = lerp(52, 150, zur);
      float hx, hy;
      frau(g, wx, 214, 1, 1, w, &hx, &hy);
      // Sauerkraut-Schuessel, dann der grosse Loeffel
      if (u < 27) g.ellipse(hx + 6, hy, 10, 4, FARBE(0xE8E0B0));
      else {
        g.linie(hx, hy, hx + 6, hy - 22, 2.6f, HOLZ);
        g.ellipse(hx + 7, hy - 25, 4, 6, HOLZ);
      }
      if (u > 29) {  // der Spitz flieht jaulend
        spitz(g, lerp(190, 330, seg(u, 29, 32)), 214 - fabsf(sinf(u * 12)) * 4, 1, 1, u, fmodf(u * 3, 1));
        for (int i = 0; i < 3; i++) g.stern(lerp(190, 330, seg(u, 29, 32)) - 10, 186 - i * 6, 3, 1.2f, 4, u * 6 + i, OUT);
      }
    }
    g.kameraAus();
    vers(g, {"Als die gute Witwe Bolte", "Sich von ihrem Schmerz erholte,"}, aufzu(u, 0.5f, 5.6f));
    vers(g, {"Max und Moritz rochen dieses;", "„Schnell aufs Dach gekrochen!“ hieß es."}, aufzu(u, 8.6f, 13.6f));
    blase(g, 220, 70, {"Ach, die Pfanne ist ja leer!"}, aufzu(u, 26, 28.6f), 160, 140);
    blase(g, 220, 70, {"Spitz, das hast du mir getan!"}, aufzu(u, 28.8f, 32.4f), 160, 140);
    if (u > 32) { g.rechteck(0, 0, 284, 240, 0, (uint8_t)(255 * seg(u, 32, 34))); }
  } else {
    // auf dem Dach und dann unter der Hecke: sie lassen es sich schmecken
    float v = u - 34;
    dorf(g, u);
    g.kontur(1.4f, OUT);
    float he[] = {60, 214, 70, 176, 120, 168, 190, 172, 220, 214};
    g.polygon(he, 5, FARBE(0x3E8A4B));
    g.konturAus();
    for (int i = 0; i < 9; i++) g.kreis(76 + i * 16, 178 + (i & 1) * 8, 8, FARBE(0x4CA85A));
    if (v > 3) { g.linie(150, 172, 156, 160, 2.4f, FARBE(0xC9702E)); g.kreis(157, 158, 2.4f, FARBE(0xF6E8D0)); }
    for (int b = 0; b < 2; b++) {
      Pose p = bp(b, u);
      p.armV = 1.7f + sinf(u * 5 + b) * 0.3f;
      p.zu = fmodf(u, 1.2f) < 0.6f;
      p.mund = 0.5f + 0.3f * sinf(u * 9 + b);
      float hx, hy;
      bub(g, b, 120 + b * 40, 214, 0.75f, b ? -1 : 1, p, &hx, &hy);
      bratHuhn(g, hx + (b ? -3 : 3), hy - 2, 0.75f);
    }
    g.kameraAus();
    if (v < 1) g.rechteck(0, 0, 284, 240, 0, (uint8_t)(255 * (1 - v)));
    vers(g, {"Und vom ganzen Hühnerschmaus", "Guckt nur noch ein Bein heraus."}, aufzu(v, 1, 9));
  }
  blende(g, u, 46, 150, 150);
}

// Dritter Streich: Schneider Boeck, der Steg und "Meck, meck, meck!"
static void szeneStreich3(Gfx &g, float u) {
  if (u < 38) {
    float eingesaegt = u > 9 ? 2 : 1;
    float bricht = seg(u, 23, 24);
    bach(g, u, eingesaegt, bricht);
    // die Buben saegen, dann rufen
    for (int b = 0; b < 2; b++) {
      Pose p = bp(b, u);
      if (u < 9) { p.armV = 1.2f + fabsf(sinf(u * 8)) * 0.5f; bub(g, b, 128 + b * 18, 214, 0.65f, b ? -1 : 1, p); }
      else if (u < 28) {
        p.mund = u > 12 && u < 18 ? 0.8f : 0.4f;
        p.armV = u > 12 && u < 18 ? 2.4f : 1.0f;
        bub(g, b, 26 + b * 20, 186 - kicher(u, b) * (u > 18), 0.62f, 1, p);
      } else {
        p.zu = true; p.mund = 0.7f;
        bub(g, b, 26 + b * 20, 186 - kicher(u, b), 0.62f, 1, p);
      }
    }
    if (u < 9) {  // Saege
      g.linie(132, 186, 156, 190, 2, FARBE(0xC0C8D4));
      for (int i = 0; i < 5; i++) g.linie(134 + i * 4, 189, 136 + i * 4, 191, 1, FARBE(0x8E94A6));
    }
    // Boeck rennt heraus mit der Elle
    if (u > 18) {
      Pose b = mp(BOECK, u);
      b.boese = 1;
      b.mund = 0.6f;
      b.armV = 2.4f;
      float bx = lerp(232, 150, sanft(seg(u, 18, 23)));
      float by = 186;
      b.lauf = u < 23 ? fmodf(u * 2.2f, 1) : -1;
      if (u > 23) { by = lerp(186, 236, hinein(seg(u, 23, 24.2f))); b.armV = 2.9f; b.armH = 2.9f; b.mund = 0.9f; }
      if (u < 26) {
        g.ausschnitt(0, 0, 284, 222);
        float hx, hy;
        schneiderlein(g, bx, by, 0.85f, -1, b, &hx, &hy);
        g.linie(hx, hy, hx - 14, hy - 22, 2, FARBE(0xE0C080));  // Elle
        g.ausschnittAus();
      }
      if (u > 24 && u < 26) {
        for (int i = 0; i < 10; i++) {
          float k = seg(u, 24, 26), a = 3.5f + i * 0.25f;
          g.kreis(150 + cosf(a) * k * 40, 214 + sinf(a) * k * 30, 4, FARBE(0xBFE6FF), (uint8_t)(255 * (1 - k)));
        }
      }
    }
    // zwei Gaense ziehen ihn heraus
    if (u > 26) {
      float k = sanft(seg(u, 26, 30));
      for (int i = 0; i < 2; i++) gans(g, lerp(330 + i * 30, 166 + i * 30, sanft(seg(u, 25, 28))), 222, 0.9f, -1, u + i, 1, k * 0.4f);
      Pose b = mp(BOECK, u);
      b.zu = true; b.mund = 0.5f; b.armV = 2.6f;
      if (k > 0) schneiderlein(g, lerp(160, 200, k), lerp(238, 186, k), 0.85f, -1, b);
    }
    g.kameraAus();
    vers(g, {"Jedermann im Dorfe kannte", "Einen, der sich Böck benannte."}, aufzu(u, 0.6f, 8.6f));
    blase(g, 90, 46, {"He, heraus! Du Ziegen-Böck!", "Schneider, Schneider,", "meck, meck, meck!!"}, aufzu(u, 12, 18), 40, 150);
    blase(g, 200, 60, {"Na wartet!"}, aufzu(u, 19, 22.4f), 190, 140);
    vers(g, {"Und schon ist er auf der Brücke,", "Kracks! Die Brücke bricht in Stücke!"}, aufzu(u, 23.4f, 30));
    if (u > 36) g.rechteck(0, 0, 284, 240, 0, (uint8_t)(255 * seg(u, 36, 38)));
  } else {
    // daheim: Frau Boeck buegelt ihm den Bauch warm
    float v = u - 38;
    stube(g, u);
    bett(g, 120, 214, 0);
    Pose b = mp(BOECK, u);
    b.zu = v > 3;
    b.mund = 0.3f;
    g.ellipse(110, 182, 44, 12, BOECK.weste);
    g.kreis(72, 172, 12, HAUT);
    g.ellipse(68, 164, 12, 6, BOECK.haar);
    g.bogen(76, 172, 2.4f, 3.5f, 5.9f, 1.2f, OUT);
    FrauPose f = fp(FRAU_BOECK, u);
    f.armV = 1.6f + sinf(u * 3) * 0.2f;
    float hx, hy;
    frau(g, 186, 214, 1, -1, f, &hx, &hy);
    float ex = lerp(hx - 4, 110 + sinf(u * 3) * 10, 0.8f);
    g.kontur(1.2f, OUT);
    float eisen[] = {ex - 12, 176, ex + 10, 176, ex + 6, 168, ex - 6, 168};
    g.polygon(eisen, 4, FARBE(0x5E6474));
    g.konturAus();
    g.linie(ex - 4, 166, ex + 4, 166, 2, HOLZ);
    for (int i = 0; i < 3; i++) {
      float k = fmodf(u * 0.7f + i * 0.33f, 1);
      g.kreis(ex - 6 + i * 6, 164 - k * 16, 2 + k * 2, WEISS, (uint8_t)(160 * (1 - k)));
    }
    g.kameraAus();
    if (v < 1) g.rechteck(0, 0, 284, 240, 0, (uint8_t)(255 * (1 - v)));
    vers(g, {"Bald im Dorf hinauf, hinunter,", "Hieß es: Böck ist wieder munter!"}, aufzu(v, 2, 11));
  }
  blende(g, u, 50, 142, 150);
}

// Vierter Streich: Lehrer Laempel und seine Pfeife
static void szeneStreich4(Gfx &g, float u) {
  stube(g, u);
  lehnstuhl(g, 200, 214);
  // Tisch mit der Pfeife
  g.kontur(1.4f, OUT);
  g.rundRechteck(70, 160, 80, 8, 2, HOLZ);
  g.konturAus();
  g.rechteck(80, 168, 6, 46, HOLZ_D);
  g.rechteck(134, 168, 6, 46, HOLZ_D);
  bool knall = u > 30;
  if (u < 26) {
    pfeife(g, 100, 158, 1, 1, false, u);
    // die Buben fuellen Pulver hinein
    if (u > 6 && u < 22) {
      for (int b = 0; b < 2; b++) {
        Pose p = bp(b, u);
        p.armV = 1.7f;
        p.mund = 0.2f;
        float hx, hy;
        bub(g, b, 60 + b * 80, 214, 0.72f, b ? -1 : 1, p, &hx, &hy);
        if (!b) {  // die Pulverflasche
          g.kontur(1, OUT);
          g.rundRechteck(hx - 3, hy - 12, 9, 13, 3, FARBE(0x3A3436));
          g.konturAus();
          if (u > 11 && u < 18) for (int i = 0; i < 4; i++) g.kreis(hx + 10 + i * 1.2f, hy - 2 + fmodf(u * 6 + i * 0.25f, 1) * 10, 1, SCHWARZ);
        }
      }
    }
    // Orgelmusik aus der Kirche
    for (int i = 0; i < 3; i++) {
      float ph = fmodf(u * 0.35f + i / 3.0f, 1);
      notenzeichen(g, 214 + ph * 20, 100 - ph * 40, FARBE(0x5A3418), (uint8_t)(200 * huepf(ph)));
    }
  } else {
    // Laempel kommt heim, setzt sich, zuendet die Pfeife an ... Rums!
    Pose l = mp(knall ? RUSS : LAEMPEL, u);
    l.armV = 1.5f;
    l.mund = knall ? 0.9f : 0.2f;
    l.zu = !knall && fmodf(u, 3) < 1.6f;
    float hx, hy;
    schneiderlein(g, 196, 206, 0.95f, -1, l, &hx, &hy);
    // Brille
    float bx = 196 - 4, by = 206 - 61 * 0.95f;
    g.bogen(bx, by, 4, 0, 6.29f, 1.2f, OUT);
    g.bogen(bx - 11, by, 4, 0, 6.29f, 1.2f, OUT);
    g.linie(bx - 7, by, bx - 4, by, 1, OUT);
    if (!knall) {
      pfeife(g, hx, hy, 1, -1, u > 28, u);
      if (u > 27.6f && u < 30) g.kreis(hx - 14, hy - 6, 2 + fabsf(sinf(u * 20)) * 1.5f, FARBE(0xFFB83D));
    } else {
      // die Pfeife ist zerbrochen, Rauch und Funken
      float k = seg(u, 30, 34);
      puff(g, 186, 150, k, FARBE(0x6E6870), 1.6f);
      if (u < 31) {
        g.kreis(186, 150, 60 * seg(u, 30, 30.4f), FARBE(0xFFE070), (uint8_t)(255 * (1 - seg(u, 30.4f, 31))));
        for (int i = 0; i < 10; i++) {
          float a = i * 0.628f, r = 20 + seg(u, 30, 31) * 50;
          g.stern(186 + cosf(a) * r, 150 + sinf(a) * r, 5, 2, 4, a, FARBE(0xFF8A2A));
        }
      }
      g.linie(hx, hy, hx - 8, hy + 2, 1.8f, FARBE(0x3A2A1A));
    }
    // die Buben lugen zum Fenster herein und kichern
    for (int b = 0; b < 2; b++) {
      g.ausschnitt(201, 45, 50, 60);
      Pose p = bp(b, u);
      p.zu = knall;
      p.mund = 0.6f;
      bub(g, b, 214 + b * 22, 136 - (knall ? kicher(u, b) : 0), 0.6f, -1, p);
      g.ausschnittAus();
    }
    if (u > 30 && u < 30.6f) g.wackeln(sinf(u * 90) * 5, cosf(u * 80) * 4);
  }
  g.kameraAus();
  vers(g, {"Also lautet ein Beschluss:", "Dass der Mensch was lernen muss."}, aufzu(u, 0.6f, 6));
  vers(g, {"Lämpel spielt die Orgel fein,", "Max und Moritz schleichen rein."}, aufzu(u, 6.2f, 13));
  vers(g, {"Nach vollbrachtem Kirchendienst", "Kommt der Lehrer heim und grinst."}, aufzu(u, 22, 28));
  if (u > 30 && u < 34) g.text(SCHRIFT_G, "Rums!!", 142, 70, FARBE(0xC0392B), 1, (uint8_t)(255 * aufzu(u, 30, 34)));
  vers(g, {"Rums!! - Da geht die Pfeife los", "Mit Getöse, schrecklich groß."}, aufzu(u, 34, 40));
  vers(g, {"Lämpel ist ganz schwarz und kahl,", "doch er lebt noch allemal!"}, aufzu(u, 40.2f, 45.6f));
  blende(g, u, 46, 160, 140);
}

// Fuenfter Streich: Onkel Fritz und die Maikaefer
static void szeneStreich5(Gfx &g, float u) {
  if (u < 14) {
    // die Buben schuetteln Maikaefer vom Baum in die Tuete
    dorf(g, u);
    laubbaum(g, 150, 214, 1.3f, FARBE(0x4CA85A));
    float sch = u > 3 && u < 10 ? sinf(u * 18) * 3 : 0;
    g.wackeln(sch, 0);
    for (int i = 0; i < 8; i++) {
      float k = fmodf(u * 0.6f + i / 8.0f, 1);
      if (u > 3) maikaefer(g, 130 + (i % 4) * 12 + sinf(k * 6) * 4, 120 + k * 80, 0.8f, u + i, 0);
    }
    for (int b = 0; b < 2; b++) {
      Pose p = bp(b, u);
      p.armV = b ? 2.6f : 1.4f;
      p.mund = 0.4f;
      float hx, hy;
      bub(g, b, 110 + b * 70, 214, 0.75f, b ? -1 : 1, p, &hx, &hy);
      if (!b) sack(g, hx + 6, hy + 14, 0.5f, WEISS);
    }
    g.kameraAus();
    vers(g, {"Wer im Dorfe oder Stadt", "Einen Onkel wohnen hat,"}, aufzu(u, 0.4f, 6.4f));
    vers(g, {"Max und Moritz schütteln munter", "Maikäfer vom Baum herunter."}, aufzu(u, 6.6f, 13.6f));
    if (u > 13) g.rechteck(0, 0, 284, 240, 0, (uint8_t)(255 * seg(u, 13, 14)));
    return;
  }
  float v = u - 14;
  stube(g, u, 0.8f);
  bett(g, 130, 214, v > 6 ? 1 : 0);
  // Onkel Fritz mit Zipfelmuetze und Kerze kommt ins Bett
  Pose f = mp(FRITZ, u);
  if (v < 6) {
    f.armV = 1.4f;
    float hx, hy;
    schneiderlein(g, lerp(330, 190, sanft(seg(v, 0, 4))), 214, 0.95f, -1, f, &hx, &hy);
    g.rechteck(hx - 3, hy - 12, 6, 12, WEISS);
    g.ellipse(hx + sinf(u * 9) * 0.6f, hy - 16, 2, 4, FARBE(0xFFB83D));
    g.kreis(hx, hy - 14, 26, FARBE(0xFFE0A0), 50);
  } else if (v < 18) {
    // im Bett: die Kaefer krabbeln heraus, einer auf die Nase
    g.kreis(92, 172, 12, HAUT);
    float mz[] = {80, 168, 104, 166, 92, 152, 76, 154};
    g.polygon(mz, 4, FRITZ.hut);
    g.kreis(70, 152, 3, FRITZ.feder);
    bool wach = v > 11;
    if (!wach) { g.bogen(96, 172, 2.4f, 3.5f, 5.9f, 1.2f, OUT); zzz(g, 100, 150, u); }
    else { g.kreis(96, 171, 2.4f, SCHWARZ); g.ellipse(98, 179, 2.4f, 3, FARBE(0x8A2A2A)); }
    for (int i = 0; i < 6; i++) {
      float k = seg(v, 7 + i * 0.6f, 11 + i * 0.6f);
      float kx = lerp(150 - i * 8, 102 + (i % 3) * 22, k), ky = lerp(186, 168 - (i % 2) * 18, k);
      if (i == 0) { kx = lerp(150, 100, k); ky = lerp(186, 168, k); }
      maikaefer(g, kx, ky, 0.7f, u + i, wach ? 1 : 0);
    }
  } else {
    // er springt auf und jagt sie mit dem Pantoffel hinaus
    f.armV = 2.4f + sinf(u * 12) * 0.5f;
    f.mund = 0.7f;
    f.boese = 1;
    float hx, hy;
    schneiderlein(g, 160 + sinf(u * 4) * 20, 214, 0.95f, 1, f, &hx, &hy);
    g.ellipse(hx + 4, hy - 4, 7, 3.5f, FARBE(0xC0392B), 255, 0.6f);
    for (int i = 0; i < 6; i++) {
      float k = seg(v, 18 + i * 0.5f, 22 + i * 0.5f);
      maikaefer(g, lerp(150 + i * 6, 226 + sinf(u * 3 + i) * 10, k), lerp(140 + i * 8, 70 - k * 30, k), 0.7f, u + i, 1);
    }
  }
  g.kameraAus();
  if (v < 1) g.rechteck(0, 0, 284, 240, 0, (uint8_t)(255 * (1 - v)));
  vers(g, {"Onkel Fritz mit Kerzenlicht", "geht zu Bett und ahnt es nicht."}, aufzu(v, 0.6f, 6));
  blase(g, 150, 120, {"Ach, was ist das?! Da!", "Ein Käfer auf der Nase!"}, aufzu(v, 12, 17.6f), 100, 160);
  vers(g, {"Onkel Fritz, in dieser Not,", "scheucht sie mit dem Pantoffel fort."}, aufzu(v, 18.2f, 22.4f));
  vers(g, {"Guckste wohl, jetzt ist's vorbei", "Mit der Käferkrabbelei!!"}, aufzu(v, 22.6f, 26.4f));
  vers(g, {"Onkel Fritz hat wieder Ruh'", "Und macht seine Augen zu."}, aufzu(v, 26.6f, 29.8f));
  blende(g, u, 44, 142, 150);
}

// Sechster Streich: beim Baecker zu Ostern
static void szeneStreich6(Gfx &g, float u) {
  backstube(g, u);
  kiste(g, 20, 214, 70, 46, WEISS, "Mehl");
  // der Teigtrog
  g.kontur(1.4f, OUT);
  float trog[] = {100, 180, 164, 180, 156, 214, 108, 214};
  g.polygon(trog, 4, HOLZ);
  g.konturAus();
  g.ellipse(132, 182, 30, 5, FARBE(0xF2E6C8));
  if (u < 10) {
    // durch den Schornstein - rutsch! - in die Mehlkiste
    for (int b = 0; b < 2; b++) {
      Pose p = bp(b, u);
      p.armV = 2.8f; p.armH = 2.8f;
      p.mund = 0.8f;
      float k = seg(u, 2 + b * 1.2f, 4 + b * 1.2f);
      float x = lerp(222, 54 + b * 8, k), y = lerp(0, 196, hinein(k));
      if (k > 0 && k < 1) bub(g, b, x, y, 0.65f, 1, p);
    }
    if (u > 4 && u < 7) puff(g, 56, 168, seg(u, 4, 6.5f), WEISS, 1.4f);
  } else if (u < 28) {
    // weiss wie Kreide steigen sie heraus und fallen in den Teig - ganz weiss und dann teigig
    float v = u - 10;
    for (int b = 0; b < 2; b++) {
      float k = seg(v, 2 + b * 0.6f, 5 + b * 0.6f);
      float x = lerp(54 + b * 10, 120 + b * 22, k), y = lerp(176, 196, k) - huepf(k) * 30;
      g.kontur(1.2f, OUT);
      g.ellipse(x, y - 10, 10, 13, k >= 1 ? FARBE(0xF2E6C8) : WEISS);
      g.kreis(x + 3, y - 26, 9, k >= 1 ? FARBE(0xF2E6C8) : WEISS);
      g.konturAus();
      g.kreis(x + 1, y - 27, 1.4f, SCHWARZ);
      g.kreis(x + 7, y - 27, 1.4f, SCHWARZ);
      if (b == 1) g.kurve(x + 4, y - 34, x + 8, y - 44, x + 16, y - 42, 3, k >= 1 ? FARBE(0xF2E6C8) : WEISS);  // die Tolle
    }
    // der Baecker kommt und formt zwei Brote
    if (v > 7) {
      Pose bk = mp(BAECKER, u);
      bk.armV = 1.6f + fabsf(sinf(u * 5)) * 0.4f;
      bk.mund = v > 9 && v < 13 ? 0.6f : 0.2f;
      float bx = lerp(330, 186, sanft(seg(v, 7, 9.5f)));
      schneiderlein(g, bx, 214, 0.95f, -1, bk);
      // Baeckermuetze
      g.kontur(1, OUT);
      g.rundRechteck(bx - 14, 214 - 90, 26, 12, 4, WEISS);
      g.konturAus();
      g.kreis(bx - 8, 214 - 92, 7, WEISS);
      g.kreis(bx + 4, 214 - 93, 7.5f, WEISS);
    }
    g.kameraAus();
    vers(g, {"In der schönen Osterzeit,", "Wenn die frommen Bäckersleut'"}, aufzu(u, 0.4f, 5));
    vers(g, {"Viele süße Zuckersachen", "Backen und zurechte machen,"}, aufzu(u, 5.2f, 10.2f));
    blase(g, 210, 60, {"Ei, da sind ja zwei Brote!"}, aufzu(v, 9, 13), 180, 140);
    vers(g, {"Ruff!! damit ins Ofenloch!", "Ruff!! heraus - da sind sie noch!"}, aufzu(v, 13.4f, 17.8f));
  } else {
    // aus dem Ofen: zwei braune Brote - sie knabbern sich heraus wie die Maeuse und laufen fort
    float v = u - 28;
    g.kontur(1.4f, OUT);
    for (int b = 0; b < 2; b++) {
      float x = 110 + b * 52;
      float raus = seg(v, 6, 11);
      g.ellipse(x, 200, 22, 14, FARBE(0xB8702E));
      if (raus > 0) {
        g.konturAus();
        Pose p = bp(b, u);
        p.mund = 0.6f;
        p.armV = 1.8f;
        float bx = lerp(x, x + (b ? 120 : -120), seg(v, 12, 16));
        g.ausschnitt(0, 0, 284, lerp(200, 240, raus));
        bub(g, b, bx, lerp(250, 214, raus), 0.65f, b ? 1 : -1, p);
        g.ausschnittAus();
        g.kontur(1.4f, OUT);
      }
      for (int i = 0; i < 3; i++) g.linie(x - 12 + i * 10, 194, x - 8 + i * 10, 190, 1.4f, FARBE(0xE0A050));
    }
    g.konturAus();
    if (v > 6 && v < 11) for (int i = 0; i < 8; i++) g.kreis(110 + (i % 2) * 52 + sinf(u * 10 + i) * 14, 188 - fmodf(u * 3 + i * 0.3f, 1) * 20, 1.5f, FARBE(0xC9884A));
    g.kameraAus();
    vers(g, {"Knusper, knasper! - wie zwei Mäuse", "Fressen sie durch das Gehäuse;"}, aufzu(v, 4, 11));
    vers(g, {"Und der Meister Bäcker schrie:", "Ach herrje! da laufen sie!!"}, aufzu(v, 12, 19));
  }
  blende(g, u, 50, 140, 150);
}

// Siebenter Streich: Bauer Mecke, die Saecke und die Muehle
static void szeneStreich7(Gfx &g, float u) {
  float drehen = u * 1.2f;
  if (u < 26) {
    himmel(g, 0, u, 196);
    sonne(g, 244, 30, 12, u);
    huegel(g, u * 4, 0.3f, 196, 26, 100, FARBE(0xC9B86A));
    boden(g, 196, FARBE(0xC9C06A));
    // Kornfeld
    for (int i = 0; i < 20; i++) g.linie(i * 15, 200, i * 15 + sinf(u * 2 + i) * 2, 184, 1.4f, FARBE(0xE0B84A));
    // die Saecke auf dem Wagen
    g.kontur(1.4f, OUT);
    g.rundRechteck(60, 186, 90, 14, 3, HOLZ);
    g.konturAus();
    g.kreis(76, 206, 10, HOLZ_D);
    g.kreis(134, 206, 10, HOLZ_D);
    for (int i = 0; i < 3; i++) sack(g, 76 + i * 28, 186, 0.9f, FARBE(0xD8C09A), i == 1 && u > 6);
    if (u > 6 && u < 14) for (int i = 0; i < 6; i++) g.kreis(110 + sinf(i) * 2, 182 + fmodf(u * 3 + i / 6.0f, 1) * 30, 1.4f, FARBE(0xE0B84A));
    // die Buben schneiden Loecher in die Saecke und naschen
    if (u < 14) {
      for (int b = 0; b < 2; b++) {
        Pose p = bp(b, u);
        p.armV = 1.4f;
        p.mund = 0.4f;
        bub(g, b, 160 + b * 26, 214, 0.7f, -1, p);
      }
    }
    // Bauer Mecke merkt es und steckt sie in den Sack
    if (u > 12) {
      Pose m = mp(MECKE, u);
      m.boese = 1;
      m.mund = u > 14 && u < 18 ? 0.7f : 0.3f;
      m.armV = 2.0f;
      float mx = lerp(330, 220, sanft(seg(u, 12, 14.5f)));
      float hx, hy;
      schneiderlein(g, mx, 214, 1, -1, m, &hx, &hy);
      if (u > 18) sack(g, hx - 6, hy + 30, 1, FARBE(0xD8C09A), false, true, u);
    }
    g.kameraAus();
    vers(g, {"Max und Moritz, wehe euch!", "Jetzt kommt euer letzter Streich!"}, aufzu(u, 0.4f, 6.4f));
    blase(g, 180, 56, {"Wart, ihr Racker!", "Euch steck ich in den Sack!"}, aufzu(u, 14.6f, 19.6f), 220, 140);
    if (u > 24) g.rechteck(0, 0, 284, 240, 0, (uint8_t)(255 * seg(u, 24, 26)));
  } else {
    // zur Muehle: Rickeracke! - nur angedeutet: Koerner rieseln heraus, die Enten picken
    float v = u - 26;
    himmel(g, 0, u, 196);
    sonne(g, 244, 30, 12, u);
    boden(g, 196);
    g.verlauf(208, 240, FARBE(0x6EB8E8), FARBE(0x3E88C8));
    muehle(g, 190, 208, u, drehen);
    // Meister Mueller schuettet den Sack in den Trichter
    Pose m = mp(MUELLER, u);
    m.armV = v < 6 ? 2.6f : 1.0f;
    m.mund = 0.3f;
    float hx, hy;
    schneiderlein(g, 70, 208, 0.85f, 1, m, &hx, &hy);
    if (v < 4) sack(g, lerp(hx, 190, seg(v, 1.6f, 3.6f)), lerp(hy + 20, 82, seg(v, 1.6f, 3.6f)), 0.8f, FARBE(0xD8C09A), false, true, u);
    // Rickeracke: die Muehle klappert
    if (v > 4 && v < 12) g.wackeln(sinf(u * 50) * 1.2f, 0);
    // Koerner rieseln aus dem Auslauf, die Enten picken
    if (v > 7) {
      for (int i = 0; i < 10; i++) {
        float k = fmodf(u * 1.4f + i / 10.0f, 1);
        g.kreis(234 + sinf(i * 3) * 4, 190 + k * 20, 1.3f, FARBE(0xC9A46A));
      }
      for (int i = 0; i < 12; i++) g.kreis(206 + zufall(i + 600) * 70, 206 + zufall(i + 620) * 6, 1.3f, FARBE(0xC9A46A));
      for (int i = 0; i < 3; i++) ente(g, lerp(330 + i * 26, 216 + i * 24, sanft(seg(v, 8, 11))), 214, 1, -1, u + i,
                                    v > 11 ? huepf(fmodf(u * 1.6f + i * 0.4f, 1)) : 0);
    }
    g.kameraAus();
    vers(g, {"Rickeracke! Rickeracke!", "Geht die Mühle mit Geknacke."}, aufzu(v, 4, 11));
    vers(g, {"Doch sogleich verzehret sie", "Meister Müllers Federvieh."}, aufzu(v, 12, 20));
    if (v < 1) g.rechteck(0, 0, 284, 240, 0, (uint8_t)(255 * (1 - v)));
  }
  blende(g, u, 50, 160, 150);
}

// Schluss: das ganze Dorf freut sich
static void szeneSchluss(Gfx &g, float u) {
  dorf(g, u);
  float hop[6];
  for (int i = 0; i < 6; i++) hop[i] = fabsf(sinf(u * 5 + i * 1.1f)) * 5;
  FrauPose w = fp(BOLTE, u);
  w.armV = 2.6f + sinf(u * 6) * 0.3f; w.mund = 0.5f;
  frau(g, 30, 214 - hop[0], 0.8f, 1, w);
  Pose b = mp(BOECK, u);
  b.armV = 2.6f; b.mund = 0.5f;
  schneiderlein(g, 72, 214 - hop[1], 0.78f, 1, b);
  Pose l = mp(LAEMPEL, u);
  l.armV = 2.2f; l.mund = 0.4f;
  schneiderlein(g, 112, 214 - hop[2], 0.8f, 1, l);
  Pose f = mp(FRITZ, u);
  f.armV = 2.4f; f.mund = 0.5f;
  schneiderlein(g, 168, 214 - hop[3], 0.78f, -1, f);
  Pose bk = mp(BAECKER, u);
  bk.armV = 2.6f; bk.mund = 0.5f;
  schneiderlein(g, 210, 214 - hop[4], 0.78f, -1, bk);
  Pose m = mp(MECKE, u);
  m.armV = 2.6f; m.mund = 0.5f;
  schneiderlein(g, 252, 214 - hop[5], 0.8f, -1, m);
  spitz(g, 140, 220, 0.8f, 1, u, fmodf(u * 2, 1));
  for (int i = 0; i < 8; i++) {
    float k = fmodf(u * 0.4f + i / 8.0f, 1);
    g.stern(30 + i * 32, 150 - k * 90, 4, 1.6f, 5, u + i, i & 1 ? GOLD : WEISS, (uint8_t)(255 * huepf(k)));
  }
  g.kameraAus();
  vers(g, {"Als man dies im Dorf erfuhr,", "War von Trauer keine Spur."}, aufzu(u, 0.6f, 7));
  vers(g, {"Gott sei Dank! Nun ist's vorbei", "Mit der Übeltäterei!!"}, aufzu(u, 7.2f, 15));
  if (u > 16) g.iris(142, 170, lerp(330, 0, sanft(seg(u, 16, 18))));
  if (u > 18) {
    uint8_t a = (uint8_t)(255 * sanft(seg(u, 18, 19)));
    g.text(SCHRIFT_G, "Ende", 142, 122, GOLD, 1, a);
    for (int i = 0; i < 6; i++) {
      float an = u * 1.2f + i * 1.047f;
      g.stern(142 + cosf(an) * 60, 112 + sinf(an) * 30, 5, 2, 5, an, GOLD, a);
    }
  }
}

static void bild(Gfx &g, float t) {
  if (t < P1) szeneVorwort(g, t - P0);
  else if (t < P2) szeneStreich1(g, t - P1);
  else if (t < P3) szeneStreich2(g, t - P2);
  else if (t < P4) szeneStreich3(g, t - P3);
  else if (t < P5) szeneStreich4(g, t - P4);
  else if (t < P6) szeneStreich5(g, t - P5);
  else if (t < P7) szeneStreich6(g, t - P6);
  else if (t < P8) szeneStreich7(g, t - P7);
  else szeneSchluss(g, t - P8);
}

// =====================================================================
// Musik und Geraeusche
// =====================================================================
// das freche Lied von Max und Moritz (F-Dur, hopsend)
static const Ton3 MEL_MM[] = {
  {72, .5, 53}, {77, .25, 0}, {77, .25, 0}, {76, .5, 0}, {74, .5, 0}, {72, .5, 48}, {69, .5, 0}, {72, 1, 0},
  {70, .5, 46}, {74, .25, 0}, {74, .25, 0}, {72, .5, 0}, {70, .5, 0}, {69, .5, 53}, {67, .5, 0}, {65, 1, 0},
  {72, .5, 53}, {77, .25, 0}, {79, .25, 0}, {81, .5, 0}, {79, .5, 0}, {77, .5, 46}, {74, .5, 0}, {76, .5, 48},
  {79, .5, 0}, {77, 1, 53}, {0, 1, 0}};
// schleichen (auf Zehenspitzen)
static const Ton3 MEL_SCHLEICH[] = {
  {65, .5, 41}, {0, .5, 0}, {68, .5, 0}, {0, .5, 0}, {67, .5, 43}, {0, .5, 0}, {65, .5, 0}, {64, .5, 0},
  {65, .5, 41}, {0, .5, 0}, {70, .5, 0}, {0, .5, 0}, {68, .5, 44}, {67, .5, 0}, {65, 1, 41}};
// Orgel (Choral)
static const Ton3 MEL_ORGEL[] = {
  {72, 2, 48}, {71, 1, 0}, {72, 1, 0}, {74, 2, 43}, {72, 2, 0}, {69, 2, 45}, {71, 1, 0}, {72, 1, 0}, {67, 4, 48}};

static void gackern(Synth &s, float x) {
  for (int i = 0; i < 5; i++) s.ton(x + i * 0.11f, 700 + (i % 2) * 180, 0.05f, 0.005f, 0.07f, DREIECK, GERAEUSCH, 520);
}
static void kikeriki(Synth &s, float x) {
  const float f[5] = {560, 700, 700, 840, 620};
  for (int i = 0; i < 5; i++) s.ton(x + i * 0.18f, f[i], 0.06f, 0.01f, i == 4 ? 0.5f : 0.16f, DREIECK, GERAEUSCH, f[i] * 1.1f);
}
static void meck(Synth &s, float x) {
  for (int i = 0; i < 3; i++)
    for (int k = 0; k < 4; k++) s.ton(x + i * 0.42f + k * 0.05f, 420 + (k & 1) * 40, 0.05f, 0.005f, 0.06f, SAEGE, GERAEUSCH, 400);
}
static void platsch(Synth &s, float x) {
  s.rauschen(x, 0.6f, BANDPASS, 900, 0.3f, 0.8f, 0.01f, 300);
  s.ton(x, 160, 0.2f, 0.005f, 0.3f, SINUS, GERAEUSCH, 60);
}
static void rums(Synth &s, float x) {
  s.ton(x, 90, 0.5f, 0.002f, 0.8f, SINUS, GERAEUSCH, 35);
  s.rauschen(x, 1.0f, TIEFPASS, 1800, 0.5f, 0.7f, 0.002f, 200);
}
static void quak(Synth &s, float x) {
  for (int i = 0; i < 2; i++) s.ton(x + i * 0.22f, 330, 0.06f, 0.01f, 0.14f, RECHTECK, GERAEUSCH, 260);
}

static void ton(Synth &s, float t0, float t1) {
  float a = P0;
  spieleMelodie(s, M(MEL_MM), a + 0.5f, a + 23.5f, 0.24f, t0, t1, 0.85f);
  a = P1;
  spieleMelodie(s, M(MEL_WANDER), a + 0.5f, a + 6, 0.3f, t0, t1, 0.7f);
  spieleMelodie(s, M(MEL_SCHLEICH), a + 6.4f, a + 13.6f, 0.26f, t0, t1, 0.8f);
  if (bei(a + 14, t0, t1)) kikeriki(s, a + 14);
  for (int i = 0; i < 4; i++) if (bei(a + 16 + i * 1.3f, t0, t1)) gackern(s, a + 16 + i * 1.3f);
  for (float x = ceilf(t0 / 0.25f) * 0.25f; x < t1; x += 0.25f)
    if (x > a + 19.6f && x < a + 22) s.note(x, 84 + ((int)(x * 4) % 3) * 2, 0.25f);  // picken
  for (int i = 0; i < 6; i++) if (bei(a + 22 + i * 0.35f, t0, t1)) gackern(s, a + 22 + i * 0.35f);  // Flattern
  if (bei(a + 26, t0, t1)) for (int k = 0; k < 4; k++) s.glocke(a + 26 + k * 0.5f, 79 - k * 4, 0.4f);  // Ei
  spieleMelodie(s, M(MEL_WIEGE), a + 37, a + 51.5f, 0.5f, t0, t1, 0.75f);
  a = P2;
  spieleMelodie(s, M(MEL_WANDER), a + 0.4f, a + 8, 0.3f, t0, t1, 0.7f);
  spieleMelodie(s, M(MEL_SCHLEICH), a + 8.4f, a + 24, 0.26f, t0, t1, 0.8f);
  for (float x = ceilf(t0 / 0.6f) * 0.6f; x < t1; x += 0.6f)
    if (x > a && x < a + 24) s.rauschen(x, 0.15f, HOCHPASS, 3000, 0.03f);  // es brutzelt
  for (int i = 0; i < 4; i++) if (bei(a + 13.4f + i * 3, t0, t1)) for (int k = 0; k < 6; k++) s.note(a + 13.4f + i * 3 + k * 0.08f, 72 + k * 2, 0.4f);
  if (bei(a + 26, t0, t1)) for (int k = 0; k < 6; k++) s.note(a + 26 + k * 0.1f, 76 - k * 3, 0.6f);  // leer!
  for (int i = 0; i < 6; i++) if (bei(a + 29 + i * 0.35f, t0, t1)) s.ton(a + 29 + i * 0.35f, 900 - i * 60, 0.05f, 0.005f, 0.12f, DREIECK, GERAEUSCH, 600);  // jaul
  spieleMelodie(s, M(MEL_MM), a + 35, a + 45.5f, 0.26f, t0, t1, 0.8f);
  a = P3;
  spieleMelodie(s, M(MEL_SCHLEICH), a + 0.4f, a + 9, 0.26f, t0, t1, 0.8f);
  for (float x = ceilf(t0 / 0.25f) * 0.25f; x < t1; x += 0.25f)
    if (x > a + 1 && x < a + 9) s.rauschen(x, 0.12f, BANDPASS, (int)(x * 4) & 1 ? 2400 : 1800, 0.07f);  // saegen
  if (bei(a + 14, t0, t1)) meck(s, a + 14);
  spieleMelodie(s, M(MEL_JAGD), a + 18.4f, a + 23, 0.16f, t0, t1, 0.8f);
  if (bei(a + 23, t0, t1)) { s.ton(a + 23, 200, 0.2f, 0.002f, 0.2f, SAEGE, GERAEUSCH, 120); }  // krach
  if (bei(a + 24, t0, t1)) platsch(s, a + 24);
  for (int i = 0; i < 4; i++) if (bei(a + 27 + i * 0.7f, t0, t1)) s.ton(a + 27 + i * 0.7f, 520, 0.06f, 0.01f, 0.18f, SAEGE, GERAEUSCH, 440);  // Gaense
  spieleMelodie(s, M(MEL_WIEGE), a + 39, a + 49.5f, 0.42f, t0, t1, 0.7f);
  a = P4;
  spieleMelodie(s, M(MEL_SCHLEICH), a + 6, a + 22, 0.28f, t0, t1, 0.7f);
  spieleMelodie(s, M(MEL_ORGEL), a + 0.4f, a + 26, 0.5f, t0, t1, 0.5f);
  for (float x = ceilf(t0 / 0.5f) * 0.5f; x < t1; x += 0.5f)
    if (x > a + 27.6f && x < a + 30) s.rauschen(x, 0.08f, HOCHPASS, 4000, 0.04f);  // Streichholz, zischen
  if (bei(a + 30, t0, t1)) rums(s, a + 30);
  spieleMelodie(s, M(MEL_MM), a + 35, a + 45.5f, 0.26f, t0, t1, 0.75f);
  a = P5;
  spieleMelodie(s, M(MEL_MM), a + 0.4f, a + 13, 0.24f, t0, t1, 0.8f);
  for (float x = ceilf(t0 / 0.13f) * 0.13f; x < t1; x += 0.13f)
    if (x > a + 3 && x < a + 10) s.rauschen(x, 0.05f, BANDPASS, 2000, 0.04f);  // Blaetter rascheln
  spieleMelodie(s, M(MEL_WIEGE), a + 14.5f, a + 25, 0.45f, t0, t1, 0.6f);
  for (float x = ceilf(t0 / 0.09f) * 0.09f; x < t1; x += 0.09f)
    if (x > a + 21 && x < a + 31) s.ton(x, 2600 + ((int)(x * 11) % 4) * 200, 0.012f, 0.002f, 0.02f, SINUS, GERAEUSCH);  // krabbeln
  if (bei(a + 26, t0, t1)) for (int k = 0; k < 6; k++) s.note(a + 26 + k * 0.07f, 84 + k * 2, 0.6f);  // Schreck
  for (float x = ceilf(t0 / 0.05f) * 0.05f; x < t1; x += 0.05f)
    if (x > a + 32 && x < a + 38) s.ton(x, 180 + ((int)(x * 20) % 3) * 30, 0.012f, 0.002f, 0.04f, SAEGE, GERAEUSCH);  // summen
  for (int i = 0; i < 4; i++) if (bei(a + 32 + i * 0.6f, t0, t1)) s.ton(a + 32 + i * 0.6f, 140, 0.15f, 0.002f, 0.1f, SINUS, GERAEUSCH, 80);  // Pantoffel
  spieleMelodie(s, M(MEL_WIEGE), a + 38.4f, a + 43.5f, 0.45f, t0, t1, 0.6f);
  a = P6;
  spieleMelodie(s, M(MEL_MM), a + 0.4f, a + 10, 0.26f, t0, t1, 0.75f);
  for (int b = 0; b < 2; b++) if (bei(a + 2 + b * 1.2f, t0, t1)) for (int k = 0; k < 8; k++) s.note(a + 2 + b * 1.2f + k * 0.06f, 88 - k * 3, 0.5f);
  if (bei(a + 4.2f, t0, t1)) s.rauschen(a + 4.2f, 0.5f, TIEFPASS, 1200, 0.2f);  // Mehl puff
  spieleMelodie(s, M(MEL_SCHLEICH), a + 10.4f, a + 23, 0.26f, t0, t1, 0.75f);
  for (float x : {a + 23.4f, a + 25.4f}) if (bei(x, t0, t1)) { s.ton(x, 120, 0.25f, 0.002f, 0.2f, SINUS, GERAEUSCH, 70); s.rauschen(x, 0.3f, TIEFPASS, 900, 0.12f); }  // Ruff!
  for (float x = ceilf(t0 / 0.11f) * 0.11f; x < t1; x += 0.11f)
    if (x > a + 32 && x < a + 39) s.rauschen(x, 0.03f, HOCHPASS, 3500, 0.06f);  // knusper
  spieleMelodie(s, M(MEL_MM), a + 39.5f, a + 49.5f, 0.22f, t0, t1, 0.8f);
  a = P7;
  spieleMelodie(s, M(MEL_SPUK), a + 0.4f, a + 6.4f, 0.3f, t0, t1, 0.75f);
  spieleMelodie(s, M(MEL_SCHLEICH), a + 6.6f, a + 14, 0.26f, t0, t1, 0.75f);
  for (float x = ceilf(t0 / 0.07f) * 0.07f; x < t1; x += 0.07f)
    if (x > a + 7 && x < a + 13) s.note(x, 100 + ((int)(x * 13) % 4), 0.06f);  // Koerner rieseln
  spieleMelodie(s, M(MEL_JAGD), a + 14.4f, a + 24, 0.18f, t0, t1, 0.8f);
  for (float x = ceilf(t0 / 0.2f) * 0.2f; x < t1; x += 0.2f)
    if (x > a + 30 && x < a + 38) { s.rauschen(x, 0.06f, BANDPASS, (int)(x * 5) & 1 ? 1500 : 700, 0.12f); s.ton(x, 110, 0.06f, 0.002f, 0.06f, SINUS, GERAEUSCH); }  // Rickeracke
  for (int i = 0; i < 5; i++) if (bei(a + 37 + i * 1.1f, t0, t1)) quak(s, a + 37 + i * 1.1f);
  spieleMelodie(s, M(MEL_WIEGE), a + 38, a + 49.5f, 0.45f, t0, t1, 0.6f);
  a = P8;
  spieleMelodie(s, M(MEL_F), a + 0.4f, a + 18, 0.26f, t0, t1, 0.9f);
  for (int i = 0; i < 8; i++) if (bei(a + 8 + i * 0.6f, t0, t1)) s.glocke(a + 8 + i * 0.6f, i & 1 ? 79 : 72, 0.7f);
  if (bei(a + 18.2f, t0, t1)) { s.glocke(a + 18.2f, 72); s.glocke(a + 18.5f, 76); s.glocke(a + 18.8f, 79); s.glocke(a + 19.2f, 84); }
}

// =====================================================================
// Startbild, Vorschau, Erkennungszeichen
// =====================================================================
static void titelSzene(Gfx &g, float t) {
  dorf(g, t);
  for (int i = 0; i < 3; i++) huhn(g, 40 + i * 26, 214, 0.9f, 1, t + i, huepf(fmodf(t * 0.8f + i * 0.3f, 1)) * 0.8f, i == 2);
  for (int b = 0; b < 2; b++) {
    Pose p = bp(b, t);
    p.mund = 0.6f;
    p.zu = fmodf(t, 2.4f) < 1.2f;
    p.armV = 1.3f + b * 0.4f;
    bub(g, b, 168 + b * 40, 222 - kicher(t, b), 0.9f, b ? -1 : 1, p);
  }
}
static void titelbild(Gfx &g, float t) {
  titelSzene(g, t);
  g.rundRechteck(8, 20, 176, 50, 14, FARBE(0x1E1A3A), 160);
  g.text(SCHRIFT_M, "Max und Moritz", 96, 52, GOLD, 1);
  float b = 1 + sinf(t * 4) * 0.08f;
  g.kontur(2, OUT);
  g.kreis(96, 96, 14 * b, FARBE(0xE8473D));
  g.konturAus();
  float tri[] = {91, 88, 91, 104, 104, 96};
  g.polygon(tri, 3, WEISS);
}
static void vorschau(Gfx &g, float t) { titelSzene(g, t); }

static void symbol(Gfx &g, float x, float y, float t) {
  for (int b = 0; b < 2; b++) {
    Pose p = bp(b, t);
    p.mund = 0.6f;
    p.zu = fmodf(t + b, 2.4f) < 1.0f;
    p.armV = 1.4f;
    bub(g, b, x - 22 + b * 44, y - kicher(t, b), 0.62f, b ? -1 : 1, p);
  }
  huhn(g, x, y, 0.8f, 1, t, huepf(fmodf(t * 0.8f, 1)) * 0.8f, true);
}

const Maerchen MAXMORITZ = {"Max und Moritz", PDAUER, FARBE(0xE07A2A), bild, ton, titelbild, vorschau, symbol,
                            LIED(MEL_MM, 0.3f), "Wilhelm Busch"};

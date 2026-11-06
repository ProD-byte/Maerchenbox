// Der Frieder und das Katherlieschen - ein Spieluhr-Film fuer die Maerchenbox, nach den Bruedern Grimm (KHM 59).
// Die Wurst, der Hund und das Bier, das Weizenmehl im Keller, die gelben Gickelinge und die Toepfe auf dem Zaun,
// die Butter in den Wagengleisen und die Kaese, die einander nachlaufen, die Haustuer auf dem Ruecken,
// die Nacht im Baum ueber den Dieben - Hutzeln, Essig und Tuer - und am Ende: "Bin ich's oder bin ich's nicht?"
#include "erzaehlen.h"

// ---------- Ablauf (Sekunden) ----------
static const float P1 = 0, P2 = 26, P3 = 52, P4 = 80, P5 = 110, P6 = 140, P7 = 166, P8 = 202, P9 = 224, P10 = 254,
                   P11 = 294, P12 = 322, P13 = 368, P14 = 402, P15 = 442, P16 = 472, P17 = 498, P18 = 532, P19 = 558,
                   P20 = 582, PDAUER = 614;

// ---------- Aussehen ----------
static Aussehen friederLook() {
  Aussehen a; a.weste = FARBE(0x8E4E2E); a.hemd = FARBE(0xF2E8D0); a.hose = FARBE(0x4A5A6E); a.haar = FARBE(0x6A4424);
  a.hut = FARBE(0x6E5A3A); a.hutD = FARBE(0x5A4A2E); a.feder = FARBE(0x6E5A3A); a.halstuch = FARBE(0xC0392B);
  return a;
}
static Aussehen diebLook(int i) {
  Aussehen a; a.weste = i & 1 ? FARBE(0x5A3A3A) : FARBE(0x3A4A3A); a.hemd = FARBE(0xC9B8A0); a.hose = FARBE(0x2E2A30);
  a.haar = FARBE(0x2E2220); a.hut = FARBE(0x1E1A22); a.hutD = FARBE(0x141016); a.feder = i & 1 ? FARBE(0xC0392B) : FARBE(0x6E8E4E);
  a.wangen = false; a.halstuch = i & 1 ? FARBE(0xE0A23C) : FARBE(0x6E8EB8);
  return a;
}
static Aussehen pfarrerLook() {
  Aussehen a; a.weste = FARBE(0x2A2A34); a.hemd = WEISS; a.hose = FARBE(0x2A2A34); a.haar = FARBE(0xC8C8C8);
  a.hut = FARBE(0x1E1A22); a.hutD = FARBE(0x141016); a.feder = FARBE(0x1E1A22); a.halstuch = WEISS;
  return a;
}
static Aussehen mannLook() {
  Aussehen a; a.weste = FARBE(0x5A7A3A); a.hemd = FARBE(0xF2E8D0); a.hose = FARBE(0x6E5A3A); a.haar = FARBE(0xC98A3A);
  a.muetze = 1;
  return a;
}
static const Aussehen FRIEDER = friederLook(), D0 = diebLook(0), D1 = diebLook(1), PFARRER = pfarrerLook(), MANN = mannLook();
static FrauLook kathLook(bool zer) {
  FrauLook l; l.kleid = FARBE(0x4E8EDC); l.schuerze = zer ? 0 : WEISS; l.haar = FARBE(0xF2C94E); l.frisur = 1;
  l.blume = zer ? 0 : FARBE(0xE8473D); l.flicken = zer;
  return l;
}
static const FrauLook KATH = kathLook(false), KATH_ZER = kathLook(true);

static const Farbe WURST = FARBE(0xB0503A), BIER = FARBE(0xE8B040), MEHL = FARBE(0xF6F2EA), KAESE = FARBE(0xFFD45A);

// =====================================================================
// Figuren
// =====================================================================
// Katherlieschen; zer = die Kleider im Halbschlaf zerschnitten (gezackter Saum)
static void kath(Gfx &g, float x, float y, float s, float dir, float t, float mund, float armV = 0.4f, float lauf = -1,
                 bool zer = false, float armH = 0.2f, bool zu = false, float *hx = nullptr, float *hy = nullptr) {
  FrauPose p = fposis(zer ? KATH_ZER : KATH, t);
  p.mund = mund;
  p.armV = armV;
  p.armH = armH;
  p.lauf = lauf;
  p.zu = zu || p.zu;
  frau(g, x, y, s, dir, p, hx, hy);
  if (zer) for (int i = 0; i < 5; i++) {
    float bx = x + (-16 + i * 8) * s;
    float z[] = {bx - 4 * s, y - 14 * s, bx + 4 * s, y - 14 * s, bx, y - 22 * s};
    g.polygon(z, 3, KATH.kleid);
    g.kreis(bx, y - 10 * s, 3 * s, FARBE(0xF6F0E4));
  }
}

// Frieder; was: 0 nichts, 1 Hacke geschultert
static void frieder(Gfx &g, float x, float y, float s, float dir, float t, float mund, float armV = 0.4f, float lauf = -1,
                    int was = 0, float boese = 0, float armH = 0.2f, bool zu = false) {
  Pose p = posis(FRIEDER, t);
  p.mund = mund;
  p.armV = was == 1 ? 0.7f : armV;
  p.armH = armH;
  p.lauf = lauf;
  p.boese = boese;
  p.zu = zu || p.zu;
  float hx, hy;
  schneiderlein(g, x, y, s, dir, p, &hx, &hy);
  if (was == 1) {
    g.linie(hx + 2 * s * dir, hy + 16 * s, hx - 4 * s * dir, hy - 46 * s, 2.4f * s, HOLZ_D);
    float b[] = {hx - 4 * s * dir, hy - 46 * s, hx - 18 * s * dir, hy - 40 * s, hx - 16 * s * dir, hy - 34 * s};
    g.polygon(b, 3, STEIN_D);
  }
}

static void dieb(Gfx &g, int i, float x, float y, float s, float dir, float t, float mund, float lauf = -1,
                 float armV = 0.4f, bool sack = false, bool kraxe = false) {
  Pose p = posis(i ? D1 : D0, t, i);
  p.mund = mund;
  p.lauf = lauf;
  p.armV = armV;
  p.boese = 0.5f;
  if (kraxe) {  // Tragegestell voller Toepfe
    g.kontur(1.2f * s, OUT);
    g.rechteck(x - (22 + (dir > 0 ? 0 : -8)) * s * dir - 4 * s, y - 80 * s, 14 * s, 56 * s, HOLZ, 255);
    g.konturAus();
    for (int k = 0; k < 3; k++) g.ellipse(x - 20 * s * dir, y - 74 * s + k * 16 * s, 9 * s, 6 * s, k & 1 ? FARBE(0xC0632E) : FARBE(0x9A7A5A));
  }
  schneiderlein(g, x, y, s, dir, p);
  if (sack) {
    g.kontur(1.2f * s, OUT);
    g.ellipse(x - 14 * s * dir, y - 44 * s, 11 * s, 13 * s, FARBE(0xC8A878));
    g.konturAus();
    g.kreis(x - 14 * s * dir, y - 58 * s, 3 * s, FARBE(0xA8885A));
    g.stern(x - 12 * s * dir, y - 44 * s, 4 * s, 1.4f * s, 4, t * 2, GOLD);
  }
}

static void hund(Gfx &g, float x, float y, float s, float dir, float t, float lauf = -1, bool wurst = false) {
  const Farbe fell = FARBE(0xB07A4A), hell = FARBE(0xE8C9A0), dunkel = FARBE(0x7A4E2A);
  auto PX = [&](float d) { return x + d * s * dir; };
  auto PY = [&](float d) { return y + d * s; };
  float hop = lauf >= 0 ? fabsf(sinf(lauf * 6.2832f)) * 5 : 0;
  g.kontur(1.2f * s, OUT);
  for (int i = 0; i < 4; i++) {
    float bx = i < 2 ? -13 + i * 6 : 8 + (i - 2) * 6;
    float sw = lauf >= 0 ? sinf(lauf * 6.2832f + i * 1.6f) * 7 : 0;
    g.linie(PX(bx), PY(-14 - hop), PX(bx + sw), PY(-2 - hop * 0.3f), 4 * s, i & 1 ? fell : dunkel);
  }
  float wedel = sinf(t * 10) * 6;
  g.kurve(PX(-15), PY(-18 - hop), PX(-24), PY(-26 - hop + wedel * 0.3f), PX(-27), PY(-30 - hop + wedel), 3.5f * s, fell);
  g.ellipse(PX(0), PY(-18 - hop), 17 * s, 8 * s, fell);
  float kx = PX(17), ky = PY(-26 - hop);
  g.kreis(kx, ky, 8 * s, fell);
  g.ellipse(kx + 8 * s * dir, ky + 2 * s, 6 * s, 4.5f * s, hell);
  g.konturAus();
  g.ellipse(kx - 3 * s * dir, ky + 1 * s, 3.5f * s, 7 * s, dunkel, 255, 0.3f * dir);
  g.kreis(kx + 13 * s * dir, ky + 1 * s, 1.8f * s, SCHWARZ);
  g.kreis(kx + 3 * s * dir, ky - 2 * s, 1.5f * s, SCHWARZ);
  if (wurst) {
    g.kontur(1 * s, OUT);
    g.ellipse(kx + 14 * s * dir, ky + 6 * s, 12 * s, 3.4f * s, WURST, 255, 0.15f * dir);
    g.konturAus();
  }
}

static void kuh(Gfx &g, float x, float y, float s, float dir, float t) {
  auto PX = [&](float d) { return x + d * s * dir; };
  auto PY = [&](float d) { return y + d * s; };
  const Farbe fell = WEISS, fleck = FARBE(0x8E5A3A), rosa = FARBE(0xF2A6B0);
  g.kontur(1.4f * s, OUT);
  for (int i = 0; i < 4; i++) {
    float bx = i < 2 ? -22 + i * 9 : 14 + (i - 2) * 9;
    g.linie(PX(bx), PY(-26), PX(bx), PY(-4), 7 * s, fell);
    g.ellipse(PX(bx), PY(-3), 4 * s, 3 * s, FARBE(0x3A3436));
  }
  g.kurve(PX(-30), PY(-40), PX(-40), PY(-34 + sinf(t * 2) * 3), PX(-38), PY(-16), 2.4f * s, fell);
  g.ellipse(PX(0), PY(-36), 32 * s, 16 * s, fell);
  g.konturAus();
  g.ellipse(PX(-12), PY(-40), 9 * s, 7 * s, fleck);
  g.ellipse(PX(10), PY(-32), 7 * s, 6 * s, fleck);
  float hx = PX(32), hy = PY(-40 + sinf(t * 0.8f) * 3);
  g.kontur(1.3f * s, OUT);
  g.ellipse(hx, hy, 12 * s, 11 * s, fell);
  g.konturAus();
  g.ellipse(hx + 6 * s * dir, hy + 6 * s, 9 * s, 6 * s, rosa);
  g.kreis(hx + 4 * s * dir, hy + 6 * s, 1.2f * s, OUT);
  g.kreis(hx + 9 * s * dir, hy + 6 * s, 1.2f * s, OUT);
  g.kreis(hx + 2 * s * dir, hy - 3 * s, 1.8f * s, SCHWARZ);
  g.linie(hx - 6 * s * dir, hy - 9 * s, hx - 10 * s * dir, hy - 16 * s, 2.4f * s, FARBE(0xE8DCC0));
  g.linie(hx + 4 * s * dir, hy - 10 * s, hx + 6 * s * dir, hy - 17 * s, 2.4f * s, FARBE(0xE8DCC0));
  g.kreis(PX(32), PY(-26), 3 * s, GOLD);
}

// =====================================================================
// Dinge
// =====================================================================
static void wurst(Gfx &g, float x, float y, float s, float w = 0) {
  g.kontur(1 * s, OUT);
  g.ellipse(x, y, 13 * s, 4 * s, WURST, 255, w);
  g.konturAus();
  g.ellipse(x - 3 * s, y - 1.5f * s, 6 * s, 1.2f * s, FARBE(0xD87A5A), 255, w);
}
static void topf(Gfx &g, float x, float y, float s, int i, bool ohneBoden = false) {
  Farbe c = i % 3 == 0 ? FARBE(0xC0632E) : i % 3 == 1 ? FARBE(0x9A7A5A) : FARBE(0xD8A060);
  g.kontur(1.2f * s, OUT);
  g.rundRechteck(x - 9 * s, y - 14 * s, 18 * s, 14 * s, 5 * s, c);
  g.konturAus();
  g.ellipse(x, y - 14 * s, 10 * s, 3 * s, ohneBoden ? FARBE(0x6E8EB8) : FARBE(0x5A3A2A));
  g.linie(x - 6 * s, y - 8 * s, x + 6 * s, y - 8 * s, 1.4f * s, WEISS, 160);
}
static void kaeselaib(Gfx &g, float x, float y, float r, float dreh) {
  g.kontur(1.2f, OUT);
  g.kreis(x, y, r, KAESE);
  g.konturAus();
  g.kreis(x, y, r * 0.55f, FARBE(0xF2C040));
  for (int i = 0; i < 3; i++) g.kreis(x + cosf(dreh + i * 2.1f) * r * 0.6f, y + sinf(dreh + i * 2.1f) * r * 0.6f, r * 0.14f, FARBE(0xE0A82A));
}
static void tuer(Gfx &g, float x, float y, float s, float w = 0) {  // (x, y) = Mitte
  float c = cosf(w), sn = sinf(w);
  auto P = [&](float dx, float dy, float *o) { o[0] = x + (dx * c - dy * sn) * s; o[1] = y + (dx * sn + dy * c) * s; };
  float q[8];
  P(-15, -28, q); P(15, -28, q + 2); P(15, 28, q + 4); P(-15, 28, q + 6);
  g.kontur(1.4f * s, OUT);
  g.polygon(q, 4, HOLZ);
  g.konturAus();
  for (int i = -1; i <= 1; i++) {
    float a[2], b[2];
    P(i * 7.5f, -26, a); P(i * 7.5f, 26, b);
    g.linie(a[0], a[1], b[0], b[1], 1.2f * s, HOLZ_D);
  }
  float k[2];
  P(10, 2, k);
  g.kreis(k[0], k[1], 2 * s, GOLD);
}
static void hutzeln(Gfx &g, float x, float y, float s) {
  g.kontur(1 * s, OUT);
  g.ellipse(x, y, 10 * s, 8 * s, FARBE(0xC8A878));
  g.konturAus();
  for (int i = 0; i < 4; i++) g.ellipse(x - 5 * s + i * 3.4f * s, y - 7 * s, 2.6f * s, 3.4f * s, FARBE(0x7A4E2A));
  g.kreis(x, y - 8 * s, 2 * s, FARBE(0xA8885A));
}
static void essigkrug(Gfx &g, float x, float y, float s, float kipp = 0) {
  g.kontur(1.1f * s, OUT);
  g.ellipse(x, y, 7 * s, 9 * s, FARBE(0xA8B8A0), 255, kipp);
  g.konturAus();
  g.ellipse(x + sinf(kipp) * 9 * s, y - cosf(kipp) * 9 * s, 3 * s, 2 * s, FARBE(0x7E8E7A), 255, kipp);
}
static void stein(Gfx &g, float x, float y, float r) {
  g.kontur(1, OUT);
  g.ellipse(x, y, r, r * 0.8f, STEIN);
  g.konturAus();
}

// =====================================================================
// Orte
// =====================================================================
// das Haeuschen von aussen mit dem Lattenzaun; toepfe = wie viele auf den Pfaehlen stecken
static void haus(Gfx &g, float t, int toepfe = 0, float nacht = 0, bool tuerWeg = false) {
  himmel(g, nacht, t, 200);
  if (nacht < 0.5f) sonne(g, 244, 32, 11, t);
  else { mond(g, 240, 36, 11); for (int i = 0; i < 16; i++) g.stern(zufall(i + 700) * 284, zufall(i + 720) * 90, 2, 0.8f, 4, 0, WEISS, 180); }
  wolke(g, 50, 50, 0.8f, (uint8_t)(255 * (1 - nacht)));
  boden(g, 200, mischeFarbe(GRAS, FARBE(0x2F4A40), nacht));
  Farbe w = mischeFarbe(FARBE(0xF0DCC0), FARBE(0x6E6A8E), nacht);
  g.kontur(1.8f, OUT);
  g.rechteck(120, 110, 120, 96, w);
  float d[] = {108, 112, 180, 58, 252, 112};
  g.polygon(d, 3, mischeFarbe(FARBE(0xC0503A), FARBE(0x5A3A4A), nacht));
  g.konturAus();
  g.rechteck(214, 60, 14, 30, FARBE(0x8E5A4A));
  for (int i = 0; i < 3; i++) {
    float k = fmodf(t * 0.4f + i * 0.33f, 1);
    g.kreis(221 + k * 10, 54 - k * 30, 4 + k * 4, WEISS, (uint8_t)(140 * (1 - k) * (1 - nacht)));
  }
  // Fenster und Tuer (zweiteilige Bauerntuer)
  g.rundRechteck(196, 130, 30, 28, 3, nacht > 0.5f ? FARBE(0xFFD27A) : FARBE(0x9FD8FF));
  g.rechteck(210, 130, 2, 28, HOLZ_D);
  g.kontur(1.4f, OUT);
  g.rechteck(140, 146, 34, 60, FARBE(0x2E2620));
  g.rechteck(140, 146, 34, 28, HOLZ);
  if (!tuerWeg) g.rechteck(140, 176, 34, 30, HOLZ);
  g.konturAus();
  // Zaun mit Pfaehlen
  g.rechteck(0, 186, 116, 4, HOLZ_D);
  g.rechteck(0, 200, 116, 4, HOLZ_D);
  for (int i = 0; i < 6; i++) {
    float px = 10 + i * 20;
    g.kontur(1, OUT);
    g.rechteck(px - 3, 168, 6, 46, HOLZ);
    g.konturAus();
    if (i < toepfe) topf(g, px, 172, 0.7f, i, true);
  }
  g.rechteck(0, 214, 284, 10, FARBE(0xC9B48A));
}

// Kueche: Herd mit Pfanne, Wuerste im Rauchfang, Kellertuer, Tisch
static void kueche(Gfx &g, float t, bool wurstDa = true, bool brutzelt = false, int wuersteOben = 3) {
  g.verlauf(0, 200, FARBE(0xE0C49A), FARBE(0xC0A07A));
  g.rechteck(0, 200, 284, 40, FARBE(0x8E6A46));
  // Herd
  g.kontur(2, OUT);
  g.rundRechteck(160, 120, 100, 82, 6, FARBE(0xA89888));
  g.konturAus();
  float rf[] = {150, 20, 270, 20, 252, 120, 168, 120};
  g.polygon(rf, 4, FARBE(0x988878));
  for (int i = 0; i < wuersteOben; i++) {
    g.linie(180 + i * 18, 40, 180 + i * 18, 52, 1.2f, OUT);
    wurst(g, 180 + i * 18, 64, 0.9f, 1.5f);
  }
  g.rundRechteck(176, 150, 68, 50, 18, FARBE(0x2E2620));
  for (int i = 0; i < 3; i++) {
    float h = 12 + 5 * sinf(t * 9 + i * 1.7f);
    float f[] = {192 + i * 18 - 7, 198, 192 + i * 18 + 7, 198, 192 + i * 18 + sinf(t * 7 + i) * 2, 198 - h};
    g.polygon(f, 3, FARBE(0xFF8A2A));
  }
  // Pfanne
  g.kontur(1.4f, OUT);
  g.ellipse(208, 122, 24, 6, FARBE(0x3A3A44));
  g.konturAus();
  g.linie(184, 122, 160, 118, 3, FARBE(0x3A3A44));
  if (wurstDa) wurst(g, 208, 118, 1.1f);
  if (brutzelt) for (int i = 0; i < 4; i++) {
    float k = fmodf(t * 0.9f + i * 0.25f, 1);
    g.kreis(196 + i * 8 + sinf(t * 4 + i) * 3, 112 - k * 30, 2 + k * 3, WEISS, (uint8_t)(160 * (1 - k)));
  }
  // Kellertuer
  g.kontur(1.6f, OUT);
  g.rundRechteck(20, 120, 40, 82, 6, HOLZ_D);
  g.konturAus();
  g.kreis(52, 162, 2.4f, GOLD);
  // Tisch
  g.kontur(1.4f, OUT);
  g.rechteck(82, 170, 60, 6, HOLZ);
  g.konturAus();
  g.rechteck(88, 176, 5, 26, HOLZ_D);
  g.rechteck(132, 176, 5, 26, HOLZ_D);
}

// Keller: Fass mit Hahn, Treppe, Bier auf dem Boden und Mehl darauf
static void keller(Gfx &g, float t, float hahn, float bier, float mehl, bool kanneUm = false) {
  g.verlauf(0, 210, FARBE(0x6E6258), FARBE(0x4E443C));
  for (int r = 0; r < 7; r++)
    for (int c = 0; c < 8; c++) g.rundRechteck(c * 40 + (r & 1) * 20 - 20, r * 30, 37, 27, 4, FARBE(0x7A6E62), 120);
  g.rechteck(0, 210, 284, 30, FARBE(0x3E3630));
  // Treppe links
  for (int i = 0; i < 6; i++) {
    g.kontur(1.2f, OUT);
    g.rechteck(0, 90 + i * 20, 70 - i * 10 + 30, 20, HOLZ);
    g.konturAus();
  }
  // Fass
  g.kontur(1.8f, OUT);
  g.ellipse(200, 150, 52, 44, HOLZ);
  g.konturAus();
  for (int i = 0; i < 3; i++) g.bogen(200, 150, 30 + i * 10, 4.4f, 5.0f, 3, HOLZ_D);
  g.linie(160, 112, 160, 188, 3, STEIN_D);
  g.linie(240, 112, 240, 188, 3, STEIN_D);
  g.rechteck(176, 196, 48, 14, HOLZ_D);
  // Hahn
  g.rechteck(140, 166, 12, 6, FARBE(0xC8A040));
  g.linie(146, 166, 146 + sinf(hahn * 1.5f) * 8, 160 - cosf(hahn * 1.5f) * 4, 2.4f, FARBE(0xC8A040));
  if (hahn > 0.5f) g.linie(141, 172, 141, 214, 3, BIER, 220);
  // Kanne
  if (!kanneUm) {
    g.kontur(1.2f, OUT);
    g.rundRechteck(132, 196, 18, 18, 4, FARBE(0x9AA6B8));
    g.konturAus();
  } else {
    g.kontur(1.2f, OUT);
    g.rundRechteck(118, 204, 22, 14, 4, FARBE(0x9AA6B8));
    g.konturAus();
  }
  // die Lache (Bier, dann weisses Mehl)
  if (bier > 0) {
    float h = 4 + bier * 18;
    g.rechteck(0, 236 - h, 284, h + 4, BIER, 230);
    g.ellipse(141, 236 - h, 10, 2, WEISS, 120);
  }
  if (mehl > 0) {
    float h = 4 + mehl * 18;
    g.rechteck(0, 236 - h, 284 * klemme(mehl * 1.2f), h + 4, MEHL);
    for (int i = 0; i < 10; i++) g.kreis(zufall(i + 5) * 284 * klemme(mehl * 1.2f), 236 - h, 4, MEHL);
  }
}

static void stall(Gfx &g, float t, float loch = 0, bool gold = true) {
  g.verlauf(0, 200, FARBE(0xA88E6A), FARBE(0x8E7454));
  for (int i = 0; i < 8; i++) g.rechteck(i * 38, 0, 3, 200, FARBE(0x7E6444));
  g.rechteck(0, 200, 284, 40, FARBE(0x8E7A4A));
  for (int i = 0; i < 12; i++) g.linie(zufall(i) * 284, 204 + zufall(i + 30) * 30, zufall(i) * 284 + 8, 200 + zufall(i + 30) * 30, 1.2f, FARBE(0xE8C870));
  kuh(g, 178, 210, 0.9f, -1, t);
  // Krippe
  g.kontur(1.6f, OUT);
  float kr[] = {60, 170, 140, 170, 130, 200, 70, 200};
  g.polygon(kr, 4, HOLZ);
  g.konturAus();
  for (int i = 0; i < 6; i++) g.linie(70 + i * 12, 172, 66 + i * 12, 164, 1.6f, FARBE(0xE8C870));
  if (loch > 0) {
    g.ellipse(100, 216, 26 * loch, 7 * loch, FARBE(0x4E3A26));
    if (gold) {
      g.kontur(1, OUT);
      g.rundRechteck(90, 204, 20, 14, 4, FARBE(0xC0632E));
      g.konturAus();
      for (int i = 0; i < 4; i++) g.kreis(94 + i * 4, 204, 3, GOLD);
      g.stern(100, 200, 4, 1.5f, 4, t * 2, WEISS);
    }
  }
}

// Weg am Berg (steigt nach rechts an) mit tiefen Wagengleisen
static void bergweg(Gfx &g, float t, float butter = 0) {
  himmel(g, 0, t, 200);
  sonne(g, 50, 34, 11, t);
  wolke(g, 190, 46, 0.9f);
  float b[] = {0, 240, 0, 210, 284, 120, 284, 240};
  g.polygon(b, 4, GRAS);
  float w[] = {0, 236, 0, 222, 284, 134, 284, 160};
  g.polygon(w, 4, FARBE(0xC9B48A));
  for (int k = 0; k < 2; k++) {
    float o = k * 10;
    g.linie(0, 226 + o, 284, 140 + o, 3.4f, FARBE(0x8E7454));
    if (butter > 0) g.linie(0, 226 + o, 284 * butter, 226 + o - 86 * butter, 2.4f, FARBE(0xFFE89A));
  }
  for (int i = 0; i < 3; i++) laubbaum(g, 160 + i * 50, 140 - i * 16, 0.5f);
}
static float bergY(float x) { return 229 - x * 0.303f; }  // Hoehe des Wegs bei x

static void waldNacht(Gfx &g, float t, float feuerAn = 0) {
  himmel(g, 1, t, 210);
  for (int i = 0; i < 20; i++) g.stern(zufall(i + 700) * 284, zufall(i + 720) * 100, 2, 0.8f, 4, 0, WEISS, 160);
  mond(g, 236, 34, 10);
  for (int i = 0; i < 6; i++) tanne(g, i * 56 - 20, 210, 0.9f, FARBE(0x1F3B45));
  boden(g, 206, FARBE(0x2F4A40));
  // der grosse Baum mit einem breiten Ast
  g.kontur(1.6f, OUT);
  g.rechteck(136, 60, 26, 150, FARBE(0x5A3E2A));
  g.linie(70, 128, 230, 128, 10, FARBE(0x5A3E2A));
  g.konturAus();
  for (int i = 0; i < 7; i++) g.kreis(70 + i * 26, 70 + (i & 1) * 12, 30, FARBE(0x24503E), 230);
  if (feuerAn > 0) {
    float x = 150, y = 226;
    for (int i = 0; i < 3; i++) g.linie(x - 12 + i * 8, y, x - 6 + i * 6, y - 6, 3, HOLZ_D);
    for (int i = 0; i < 3; i++) {
      float h = (14 + 6 * sinf(t * 9 + i * 1.7f)) * feuerAn;
      float f[] = {x - 8 + i * 8 - 5, y - 2, x - 8 + i * 8 + 5, y - 2, x - 8 + i * 8 + sinf(t * 7 + i) * 2, y - 2 - h};
      g.polygon(f, 3, i == 1 ? FARBE(0xFFD54A) : FARBE(0xFF8A2A));
    }
    g.kreis(x, y - 10, 50 * feuerAn, FARBE(0xFFB050), 40);
  }
}

static void feld(Gfx &g, float t, float abend = 0) {
  himmel(g, abend, t, 190);
  if (abend < 0.5f) sonne(g, 238, 34, 11, t);
  else mond(g, 238, 34, 10);
  boden(g, 190, mischeFarbe(FARBE(0xE8C060), FARBE(0x5A5A50), abend));
  for (int i = 0; i < 40; i++) {
    float x = zufall(i + 10) * 300 - 8, y = 186 + zufall(i + 50) * 20;
    float w = sinf(t * 1.5f + x * 0.05f) * 3;
    g.linie(x, y + 14, x + w, y - 6, 1.6f, mischeFarbe(FARBE(0xC89A30), FARBE(0x4A4A40), abend));
    g.ellipse(x + w, y - 8, 2, 4, mischeFarbe(FARBE(0xF2CC60), FARBE(0x6A6A58), abend));
  }
}

static void ruebenland(Gfx &g, float t) {
  himmel(g, 0.75f, t, 196);
  mond(g, 236, 34, 10);
  boden(g, 196, FARBE(0x4A5A3A));
  // Pfarrhaus mit Kirchturm hinten
  g.kontur(1.4f, OUT);
  g.rechteck(196, 100, 26, 96, FARBE(0x8E8AA8));
  float d[] = {192, 102, 209, 60, 226, 102};
  g.polygon(d, 3, FARBE(0x5A4A6E));
  g.rechteck(230, 146, 60, 50, FARBE(0x8E8AA8));
  g.konturAus();
  g.rundRechteck(240, 158, 14, 14, 3, FARBE(0xFFD27A));
  for (int r = 0; r < 2; r++)
    for (int i = 0; i < 9; i++) {
      float x = 14 + i * 22 + r * 11, y = 210 + r * 14;
      g.ellipse(x, y, 5, 3, FARBE(0xE8E0F0));
      for (int k = -1; k <= 1; k++) g.ellipse(x + k * 3, y - 8, 2.2f, 6, FARBE(0x5E9A4E), 255, k * 0.4f);
    }
}

// =====================================================================
// Szenen
// =====================================================================
// 1) Der Auftrag
static void szeneAuftrag(Gfx &g, float u) {
  haus(g, u);
  float k = seg(u, 21, 26);
  frieder(g, lerp(186, 330, k), 222, 0.92f, k > 0 ? 1 : -1, u, (u > 6.4f && u < 16) ? 0.4f + 0.2f * sinf(u * 8) : 0, 0.4f,
          k > 0 ? fmodf(u * 1.5f, 1) : -1, 1);
  kath(g, 132, 222, 0.85f, 1, u, u > 16.4f && u < 21.6f ? 0.5f : 0.2f, u > 21 ? 2.4f + sinf(u * 6) * 0.3f : 0.4f);
  g.kameraAus();
  vers(g, {"Der Frieder und das Katherlieschen", "waren ein junges Ehepaar."}, aufzu(u, 0.4f, 6));
  blase(g, 190, 50, {"Ich geh jetzt aufs Feld,", "Katherlieschen."}, aufzu(u, 6.4f, 10), 186, 130);
  blase(g, 180, 50, {"Wenn ich heimkomme, soll was", "Gebratenes auf dem Tisch stehen", "und ein frischer Trunk dazu!"}, aufzu(u, 10.4f, 16), 186, 130);
  blase(g, 110, 50, {"Geh nur, Friederchen, geh nur,", "will dir's schon recht machen!"}, aufzu(u, 16.4f, 21.6f), 132, 140);
}

// 2) Die Wurst in der Pfanne, das Bier im Keller
static void szeneWurst(Gfx &g, float u) {
  if (u < 14) {
    bool inPfanne = u > 5;
    kueche(g, u, inPfanne, inPfanne, inPfanne ? 2 : 3);
    float hx, hy;
    float k = seg(u, 2, 5);
    kath(g, lerp(120, 160, sanft(k)), 222, 0.85f, 1, u, u > 8.4f ? 0.5f : 0.2f, u < 5 ? 2.6f - k * 1 : 0.4f, -1, false, 0.2f, false, &hx, &hy);
    if (u > 2 && !inPfanne) wurst(g, hx + 4, hy, 0.9f, 1.2f - k * 1.2f);
    g.kameraAus();
    vers(g, {"Mittags holte sie eine Wurst", "aus dem Rauchfang und briet", "sie mit Butter in der Pfanne."}, aufzu(u, 0.4f, 8));
    blase(g, 110, 50, {"Bis die Wurst fertig ist,", "kann ich im Keller Bier zapfen."}, aufzu(u, 8.4f, 13.8f), 160, 140);
    return;
  }
  float v = u - 14;
  keller(g, u, sanft(seg(v, 3, 4)), 0, 0);
  bool rennt = v > 10;
  kath(g, rennt ? lerp(110, -40, seg(v, 10.5f, 12)) : 110, rennt ? lerp(222, 120, seg(v, 10.5f, 12)) : 222, 0.85f, rennt ? -1 : 1, u,
       v > 6.4f ? 0.7f : 0.2f, v > 2 && v < 5 ? 1.5f : 0.4f, rennt ? fmodf(u * 2, 1) : -1);
  if (v > 6.4f && v < 10) fragezeichen(g, 120, 120, 1);
  g.kameraAus();
  vers(g, {"Sie stellte die Kanne unters", "Fass und drehte den Hahn auf."}, aufzu(v, 0.4f, 6));
  blase(g, 150, 50, {"Der Hund oben ist nicht", "angebunden! Die Wurst!"}, aufzu(v, 6.4f, 11.6f), 116, 140);
}

// 3) Der Hund mit der Wurst
static void szeneHund(Gfx &g, float u) {
  if (u < 18) {
    if (u < 6) {
      kueche(g, u, false, false, 2);
      hund(g, lerp(200, 330, seg(u, 3, 6)), 222, 1, 1, u, u > 3 ? fmodf(u * 2, 1) : -1, true);
      kath(g, 40, 222, 0.85f, 1, u, 0.9f, 2.6f, -1);
    } else {
      float v = u - 6;
      // die Jagd uebers Feld
      himmel(g, 0, u, 200);
      sonne(g, 240, 32, 11, u);
      huegel(g, v * 40, 0.025f, 196, 26, 170, GRAS_H);
      boden(g, 200);
      float k = seg(v, 8, 12);
      hund(g, lerp(190, 330, k), 222, 1, 1, u, fmodf(u * 2.4f, 1), true);
      float kx = lerp(90, 150, seg(v, 0, 8));
      bool stop = v > 8;
      kath(g, kx, 222, 0.85f, 1, u, 0.8f, stop ? 0.4f : 2.4f, stop ? -1 : fmodf(u * 2, 1));
      if (stop) for (int i = 0; i < 2; i++) {
        float kk = fmodf(u * 1.4f + i * 0.5f, 1);
        tropfen(g, kx - 10 + i * 20, 150 + kk * 14, 2.4f, FARBE(0x7EC0EE), (uint8_t)(255 * (1 - kk)));
      }
      blumen(g, v * 60, 230, 6, 3);
    }
    g.kameraAus();
    vers(g, {"Und richtig: Der Hund hatte die", "Wurst schon im Maul!"}, aufzu(u, 0.4f, 6));
    vers(g, {"Sie lief ihm übers Feld nach,", "aber der Hund war schneller."}, aufzu(u, 6.4f, 13.4f));
    blase(g, 150, 60, {"Hin ist hin!"}, aufzu(u, 14, 17.8f), 150, 140);
    return;
  }
  float v = u - 18;
  keller(g, u, 1, seg(v, 0, 9), 0);
  g.kameraAus();
  vers(g, {"Unterdessen lief das Bier", "immerzu aus dem Fass..."}, aufzu(v, 0.4f, 9.6f));
}

// 4) Das Mehl im Keller
static void szeneMehl(Gfx &g, float u) {
  float mehl = seg(u, 10, 20);
  keller(g, u, u < 5 ? 1 : 0, 1, mehl, u > 13);
  bool sack = u > 6 && u < 20;
  float hx, hy;
  float kx = u < 6 ? 62 : u < 10 ? lerp(62, 110, seg(u, 6, 10)) : 110 + sinf(u * 1.2f) * 20 * seg(u, 13, 14);
  float ky = u < 6 ? 170 : u < 10 ? lerp(170, 206, seg(u, 6, 10)) : 206;
  kath(g, kx, ky, 0.85f, 1, u, (u > 0.4f && u < 6) || (u > 14 && u < 28) ? 0.5f + 0.2f * sinf(u * 8) : 0.2f,
       sack ? 1.6f : u > 22 ? 2.2f + sinf(u * 4) * 0.3f : 2.6f, (u > 6 && u < 10) ? fmodf(u * 1.4f, 1) : -1, false, 0.2f,
       u > 22, &hx, &hy);
  if (sack) {
    g.kontur(1.2f, OUT);
    g.ellipse(hx + 8, hy - 2, 13, 11, FARBE(0xE8DCC8), 255, u > 10 ? 0.6f : 0);
    g.konturAus();
    if (u > 10) for (int i = 0; i < 6; i++) {
      float kk = fmodf(u * 1.5f + i / 6.f, 1);
      g.kreis(hx + 18 + kk * 30, hy + kk * 40, 3 - kk * 2, MEHL, (uint8_t)(255 * (1 - kk)));
    }
  }
  if (u > 13 && u < 13.6f) wolkePuff(g, 130, 210, seg(u, 13, 13.6f), MEHL, 0.5f);
  if (u > 22) glitzern(g, 140, 220, 80, u, 6);
  g.kameraAus();
  blase(g, 110, 40, {"Spuk! Was fang ich jetzt an,", "dass es der Frieder nicht merkt?"}, aufzu(u, 0.4f, 6), 62, 100);
  vers(g, {"Da holte sie den Sack feines", "Weizenmehl vom Boden und", "streute es ins Bier."}, aufzu(u, 6.4f, 13));
  blase(g, 150, 60, {"Wo eins ist, muss das", "andere auch sein!"}, aufzu(u, 14.4f, 20), 120, 140);
  blase(g, 150, 60, {"Wie sauber und reinlich", "das aussieht!"}, aufzu(u, 22.4f, 28.6f), 120, 140);
}

// 5) Frieder kommt heim
static void szeneHeim(Gfx &g, float u) {
  kueche(g, u, false, false, 2);
  float k = seg(u, 0, 3);
  bool schimpft = u > 17.4f && u < 22.5f;
  frieder(g, lerp(330, 196, k), 222, 0.92f, -1, u, (u > 0.4f && u < 5) || schimpft ? 0.6f + 0.2f * sinf(u * 9) : u > 6 ? 0.9f : 0,
          schimpft ? 2.8f : u > 6 ? 2.2f : 0.4f, k < 1 ? fmodf(u * 1.5f, 1) : -1, 0, schimpft ? 1 : 0.3f, schimpft ? 2.8f : 0.2f);
  bool spricht = (u > 5.4f && u < 17) || u > 22.8f;
  kath(g, 120, 222, 0.85f, 1, u, spricht ? 0.4f + 0.3f * sinf(u * 9) : 0, spricht ? 1.4f + sinf(u * 3) * 0.4f : 0.4f);
  if (u > 22.8f) fragezeichen(g, 110, 120, 0.8f);
  g.kameraAus();
  blase(g, 200, 50, {"Nun, Frau, was hast du mir", "zurechtgemacht?"}, aufzu(u, 0.4f, 5), 200, 130);
  blase(g, 120, 50, {"Ach, Friederchen, die Wurst hat", "der Hund geholt, und das Bier", "ist derweil ausgelaufen..."}, aufzu(u, 5.4f, 11.5f), 120, 140);
  blase(g, 120, 50, {"...und da hab ich das Mehl", "draufgestreut, damit's trocken wird."}, aufzu(u, 11.8f, 17), 120, 140);
  blase(g, 190, 50, {"Katherlieschen! Das hättest", "du nicht tun dürfen!"}, aufzu(u, 17.4f, 22.5f), 196, 130);
  blase(g, 120, 50, {"Ja, Friederchen, das hab ich", "nicht gewusst. Hättest mir's", "sagen müssen!"}, aufzu(u, 22.8f, 29.6f), 120, 140);
}

// 6) Die gelben Gickelinge unter der Kuhkrippe
static void szeneGold(Gfx &g, float u) {
  float loch = seg(u, 5, 7);
  stall(g, u, loch > 0 && u < 18 ? loch : u >= 18 ? 0 : 0, u < 13);
  if (u >= 13) {  // wieder zugeschuettet
    g.ellipse(100, 214, 24, 5, FARBE(0x6E5A3A));
  }
  frieder(g, 136, 222, 0.92f, -1, u, (u > 6.4f && u < 18) ? 0.4f + 0.2f * sinf(u * 8) : 0, u < 6 ? 1.5f : u > 12.8f ? 1.2f : 0.4f, -1,
          0, u > 12.8f && u < 18 ? 0.8f : 0);
  if (u < 5) for (int i = 0; i < 5; i++) {
    float a = u * 2 + i * 1.256f;
    g.kreis(116 + cosf(a) * 8, 150 + sinf(a) * 4, 4, GOLD);
  }
  kath(g, 40 + 26, 222, 0.85f, 1, u, u > 18.4f ? 0.5f : 0.2f, u > 18.4f ? 1.2f : 0.4f);
  g.kameraAus();
  vers(g, {"Frieder wechselte sein Erspartes", "in lauter Gold um."}, aufzu(u, 0.4f, 6));
  blase(g, 180, 50, {"Das sind gelbe Gickelinge.", "Die vergrab ich hier im Stall", "unter der Kuhkrippe."}, aufzu(u, 6.4f, 12.5f), 140, 130);
  blase(g, 180, 50, {"Aber dass du ja davon bleibst,", "sonst geht's dir schlimm!"}, aufzu(u, 12.8f, 18), 140, 130);
  blase(g, 110, 50, {"Nein, Friederchen,", "das will ich gewiss nicht!"}, aufzu(u, 18.4f, 24.6f), 70, 140);
}

// 7) Die Kraemer mit den Toepfen
static void szeneKraemer(Gfx &g, float u) {
  haus(g, u);
  float k = seg(u, 0, 6);
  bool weg = u > 27;
  float r = seg(u, 27, 31);
  float x0 = weg ? lerp(170, 340, r) : lerp(340, 170, k);
  float lauf = (k < 1 || weg) ? fmodf(u * (weg ? 2.2f : 1.4f), 1) : -1;
  dieb(g, 0, x0, 222, 0.85f, weg ? 1 : -1, u, (u > 7.4f && u < 11) || (u > 17.4f && u < 22) ? 0.5f + 0.2f * sinf(u * 8) : 0,
       lauf, 0.4f, weg, !weg);
  dieb(g, 1, x0 + 36, 222, 0.85f, weg ? 1 : -1, u, 0, lauf, 0.4f, weg, !weg);
  if (weg) for (int i = 0; i < 6; i++) topf(g, 196 + (i % 3) * 22, 222 - (i / 3) * 14, 0.9f, i);
  kath(g, 110, 222, 0.85f, 1, u, (u > 11.4f && u < 17) || (u > 22.4f && u < 26.6f) ? 0.5f + 0.2f * sinf(u * 8) : 0.2f,
       u > 22.4f && u < 26.6f ? 1.6f : 0.4f);
  if (weg) wolkePuff(g, lerp(176, 340, r) - 20, 214, fmodf(u * 2, 1), FARBE(0xE8DCC0), 0.5f);
  g.kameraAus();
  vers(g, {"Als Frieder fort war, kamen", "Krämer mit irdenen Töpfen", "und Schüsseln ins Dorf."}, aufzu(u, 0.4f, 7));
  blase(g, 200, 50, {"Schöne Töpfe! Wollt ihr kaufen?"}, aufzu(u, 7.4f, 11), 200, 130);
  blase(g, 110, 50, {"Geld hab ich keins. Aber", "gelbe Gickelinge, wenn ihr", "die brauchen könnt!"}, aufzu(u, 11.4f, 17), 110, 140);
  blase(g, 200, 50, {"Gelbe Gickelinge? Warum nicht,", "lasst doch einmal sehen!"}, aufzu(u, 17.4f, 22), 200, 130);
  blase(g, 110, 50, {"Geht in den Stall und grabt", "unter der Kuhkrippe!"}, aufzu(u, 22.4f, 26.6f), 110, 140);
  vers(g, {"Die Krämer gruben das Gold aus", "und liefen damit davon."}, aufzu(u, 28, 35.6f));
}

// 8) Die Toepfe auf dem Zaun
static void szeneZaun(Gfx &g, float u) {
  int n = (int)(seg(u, 8, 16) * 6.99f);
  haus(g, u, n);
  for (int i = n; i < 6; i++) topf(g, 196 + (i % 3) * 22, 222 - (i / 3) * 14, 0.9f, i);
  float x = n < 6 ? 30 + n * 20 + 14 : 124;
  kath(g, x, 222, 0.85f, -1, u, u > 16 ? 0.6f : 0.2f, u > 8 && u < 16 ? 2.2f + sinf(u * 8) * 0.4f : 0.4f);
  if (u > 16) glitzern(g, 60, 172, 60, u, 6);
  g.kameraAus();
  vers(g, {"In der Küche war kein Platz", "für die vielen Töpfe."}, aufzu(u, 0.4f, 6));
  vers(g, {"Da schlug sie allen den Boden", "aus und steckte sie zum Schmuck", "auf die Zaunpfähle ums Haus."}, aufzu(u, 6.4f, 15.6f));
  blase(g, 150, 60, {"Wie schön das aussieht!"}, aufzu(u, 16, 21.6f), 124, 140);
}

// 9) Frieder sieht den Zierat
static void szeneZierat(Gfx &g, float u) {
  haus(g, u, 6);
  bool klagt = u > 14.4f && u < 20.5f;
  frieder(g, 196, 222, 0.92f, -1, u, (u > 0.4f && u < 4.5f) || klagt ? 0.6f + 0.2f * sinf(u * 9) : 0.6f,
          klagt ? 2.8f : u < 5 ? 1.8f : 0.4f, -1, 0, klagt ? 0.6f : 0.2f, klagt ? 2.8f : 0.2f);
  bool spricht = (u > 4.8f && u < 14) || (u > 20.8f && u < 29.6f);
  kath(g, 130, 222, 0.85f, 1, u, spricht ? 0.4f + 0.3f * sinf(u * 9) : 0, spricht ? 1.4f : 0.4f);
  g.kameraAus();
  blase(g, 200, 50, {"Katherlieschen, was hast du", "da gemacht?"}, aufzu(u, 0.4f, 4.5f), 200, 130);
  blase(g, 120, 50, {"Hab's gekauft, Friederchen -", "für die gelben Gickelinge!"}, aufzu(u, 4.8f, 9.5f), 130, 140);
  blase(g, 120, 50, {"Die Krämer haben sie sich", "selber ausgegraben."}, aufzu(u, 9.8f, 14), 130, 140);
  blase(g, 190, 50, {"Ach Frau, das war kein Gickeling,", "das war eitel Gold -", "all unser Vermögen!"}, aufzu(u, 14.4f, 20.5f), 196, 130);
  blase(g, 120, 50, {"Hättest mir's vorher", "sagen sollen!"}, aufzu(u, 20.8f, 24.8f), 130, 140);
  blase(g, 120, 50, {"Komm, wir laufen den", "Dieben nach!"}, aufzu(u, 25.2f, 29.6f), 130, 140);
}

// 10) Die Butter in den Gleisen, die Kaese laufen den Berg hinab
static void szeneButter(Gfx &g, float u) {
  float butter = seg(u, 16, 22);
  bergweg(g, u, butter);
  // Frieder geht voraus und ist bald oben verschwunden
  float fk = seg(u, 0, 7);
  if (fk < 1) frieder(g, lerp(150, 300, fk), bergY(lerp(150, 300, fk)) + 4, 0.8f, 1, u, 0, 0.4f, fmodf(u * 1.4f, 1));
  float kx = u < 6 ? lerp(40, 100, seg(u, 0, 6)) : u < 16 ? 100 : lerp(100, 150, butter);
  bool schmiert = u > 16 && u < 22;
  kath(g, kx, bergY(kx) + 4, 0.8f, u > 26 && u < 31 ? -1 : 1, u, (u > 6.4f && u < 16) || (u > 26 && u < 31) ? 0.4f + 0.3f * sinf(u * 9) : 0.2f,
       schmiert ? 1.2f + sinf(u * 8) * 0.3f : u > 26 && u < 31 ? 1.6f : 0.4f, u < 6 ? fmodf(u * 1.4f, 1) : -1, false, 0.2f,
       u > 6.4f && u < 12);
  // die Kaese rollen bergab, einer nach dem anderen
  for (int i = 0; i < 6; i++) {
    float st = 22 + (i == 0 ? 0 : 9 + i * 1.6f);
    float k = seg(u, st, st + 4);
    if (k <= 0 || k >= 1) {
      if (k <= 0 && u > 20) {  // noch im Korb
      }
      continue;
    }
    float x = lerp(kx - 6, -30, hinein(k));
    kaeselaib(g, x, bergY(x) - 6, 7, -u * 12);
  }
  g.kameraAus();
  vers(g, {"Sie nahmen Brot, Butter und", "Käse mit. Frieder ging voraus."}, aufzu(u, 0.4f, 6));
  blase(g, 140, 60, {"Ach, wie das arme Erdreich", "zerrissen und gedrückt ist!", "Das wird nie wieder heil."}, aufzu(u, 6.4f, 15.6f), 100, 160);
  vers(g, {"Aus Mitleid schmierte sie ihre", "Butter in die Wagengleise."}, aufzu(u, 16.4f, 22));
  blase(g, 170, 60, {"Ich geh den Weg nicht nochmal!", "Lauf du ihm nach und hol ihn!"}, aufzu(u, 26, 31), 150, 160);
  vers(g, {"Sie schickte einen Käse nach dem", "andern - keiner kam wieder."}, aufzu(u, 31.4f, 39.6f));
}

// 11) Trockenes Brot
static void szeneBrot(Gfx &g, float u) {
  bergweg(g, u, 1);
  float k = seg(u, 0, 3);
  bool schimpft = u > 15.8f && u < 19.5f;
  frieder(g, 210, bergY(210) + 4, 0.85f, -1, u, (u > 0.4f && u < 8) || schimpft ? 0.6f : u > 24 ? 0.3f + 0.3f * sinf(u * 7) : 0,
          schimpft ? 2.8f : u > 24 ? 1.5f + sinf(u * 4) * 0.3f : 0.4f, -1, 0, schimpft ? 0.8f : 0.2f, schimpft ? 2.8f : 0.2f);
  float kx = lerp(60, 160, k);
  bool spricht = (u > 8.4f && u < 15.5f) || (u > 19.8f && u < 24);
  kath(g, kx, bergY(kx) + 4, 0.8f, 1, u, spricht ? 0.4f + 0.3f * sinf(u * 9) : u > 24 ? 0.3f + 0.3f * sinf(u * 7 + 1) : 0,
       u > 4 && u < 8 ? 1.6f : u > 24 ? 1.5f + sinf(u * 4 + 1) * 0.3f : 0.4f, k < 1 ? fmodf(u * 1.4f, 1) : -1);
  if (u > 4) {
    g.kontur(1, OUT);
    g.ellipse(u < 8 ? 180 : 196, u < 8 ? 158 : 150, 8, 5, FARBE(0xC88A4A));
    g.konturAus();
  }
  g.kameraAus();
  blase(g, 200, 50, {"Gib mir was zu essen!"}, aufzu(u, 0.4f, 4), 210, 130);
  blase(g, 200, 50, {"Nur trockenes Brot?", "Wo ist Butter und Käse?"}, aufzu(u, 4.4f, 8), 210, 130);
  blase(g, 120, 50, {"Die Butter ist in den Gleisen,", "und die Käse kommen bald -", "einer holt den andern."}, aufzu(u, 8.4f, 15.5f), 160, 150);
  blase(g, 200, 50, {"Das hättest du nicht tun sollen!"}, aufzu(u, 15.8f, 19.5f), 210, 130);
  blase(g, 120, 50, {"Ja, Friederchen,", "hättest mir's sagen müssen!"}, aufzu(u, 19.8f, 24), 160, 150);
  vers(g, {"So aßen die beiden trockenes Brot."}, aufzu(u, 24.4f, 27.8f));
}

// 12) Die Haustuer
static void szeneTuer(Gfx &g, float u) {
  if (u < 15) {
    bergweg(g, u, 1);
    frieder(g, 210, bergY(210) + 4, 0.85f, -1, u, (u > 0.4f && u < 4.5f) || (u > 9.4f && u < 14.5f) ? 0.5f + 0.2f * sinf(u * 8) : 0,
            u > 9.4f ? 1.6f : 0.4f);
    kath(g, 160, bergY(160) + 4, 0.8f, 1, u, u > 4.8f && u < 9 ? 0.5f : 0.2f, 0.4f);
    g.kameraAus();
    blase(g, 200, 50, {"Hast du auch das Haus", "verschlossen?"}, aufzu(u, 0.4f, 4.5f), 210, 130);
    blase(g, 120, 50, {"Nein - hättest mir's", "vorher sagen sollen!"}, aufzu(u, 4.8f, 9), 160, 150);
    blase(g, 200, 50, {"So geh heim, verwahr das Haus", "und bring was anderes mit!"}, aufzu(u, 9.4f, 14.6f), 210, 130);
    return;
  }
  if (u < 30) {
    float v = u - 15;
    bool tuerAus = v > 6;
    haus(g, u, 6, 0, tuerAus);
    float hx, hy;
    float tr = seg(v, 9, 15);
    if (tuerAus) tuer(g, lerp(157, -30, tr) + 4, 176, 0.9f, 0.08f);
    kath(g, tuerAus ? lerp(157, -30, tr) : lerp(60, 157, seg(v, 0, 4)), 222, 0.85f, tuerAus ? -1 : 1, u, 0.3f,
         tuerAus ? 2.6f : v > 4 ? 1.8f : 0.4f, (v < 4 || tr > 0) ? fmodf(u * 1.4f, 1) : -1, false, tuerAus ? 2.6f : 0.2f,
         false, &hx, &hy);
    if (v < 9) { hutzeln(g, 100, 214, 0.8f); essigkrug(g, 120, 212, 0.8f); }
    g.kameraAus();
    vers(g, {"Sie packte Hutzeln und einen Krug", "Essig ein. Die obere Tür riegelte", "sie zu, die untere hob sie aus."}, aufzu(v, 0.4f, 9.4f));
    vers(g, {"Die nahm sie auf den Rücken."}, aufzu(v, 9.8f, 14.6f));
    return;
  }
  float v = u - 30;
  bergweg(g, u, 1);
  bool schimpft = v > 4.8f && v < 10;
  frieder(g, 210, bergY(210) + 4, 0.85f, -1, u, schimpft ? 0.6f + 0.2f * sinf(u * 9) : 0.4f, schimpft ? 2.8f : 0.4f, -1, 0,
          schimpft ? 0.8f : 0.2f, schimpft ? 2.8f : 0.2f);
  float k = seg(v, 0, 3);
  float kx = lerp(60, 150, k);
  tuer(g, kx + 4, bergY(kx) - 42, 0.85f, 0.08f);
  kath(g, kx, bergY(kx) + 4, 0.8f, 1, u, (v > 0.4f && v < 4.5f) || v > 10.4f ? 0.5f + 0.2f * sinf(u * 8) : 0.2f, 2.6f,
       k < 1 ? fmodf(u * 1.4f, 1) : -1, false, 2.6f);
  if (v > 13) { hutzeln(g, kx + 4, bergY(kx) - 24, 0.6f); essigkrug(g, kx + 14, bergY(kx) - 22, 0.6f); }
  g.kameraAus();
  blase(g, 120, 50, {"Da, Friederchen, hast du", "die Haustür - verwahr sie selbst!"}, aufzu(v, 0.4f, 4.5f), 150, 140);
  blase(g, 190, 50, {"Jetzt kann jeder hinein! Weil", "du sie gebracht hast, trag sie!"}, aufzu(v, 4.8f, 10), 210, 130);
  blase(g, 120, 50, {"Die Tür trag ich gern - Hutzeln", "und Essig häng ich daran."}, aufzu(v, 10.4f, 15.6f), 150, 140);
}

// 13) Die Nacht im Baum, die Diebe kommen
static void szeneBaum(Gfx &g, float u) {
  float feuer = seg(u, 13, 16);
  waldNacht(g, u, feuer);
  float auf = seg(u, 3, 7);
  // die beiden klettern auf den Ast
  float ky = lerp(226, 128, sanft(auf)), fy = lerp(226, 128, sanft(seg(u, 2, 6)));
  bool friederUnten = u > 16 && u < 21.4f;
  float fyy = friederUnten ? lerp(128, 226, sanft(seg(u, 16, 17.5f))) - sanft(seg(u, 20, 21.4f)) * 98 : fy;
  float fx = friederUnten ? lerp(190, 230, seg(u, 16, 17.5f)) : 190;
  frieder(g, fx, fyy, 0.7f, -1, u, 0, friederUnten ? 1.4f : u > 21.4f && u < 27 ? 2.6f : 0.4f, -1);
  if (u > 18.5f && u < 21.4f) stein(g, fx - 8, fyy - 34, 4);
  tuer(g, 104, ky - 36, 0.72f, 0.08f);
  kath(g, 108, ky, 0.7f, 1, u, 0.2f, 2.6f, -1, false, 2.6f);
  // die Diebe
  float dk = seg(u, 8, 12);
  for (int i = 0; i < 2; i++) {
    float x = lerp(330 + i * 40, 180 + i * 50, dk);
    bool spricht = i == 0 && u > 27.4f;
    dieb(g, i, x, 228, 0.78f, -1, u, spricht ? 0.5f + 0.3f * sinf(u * 9) : 0, dk < 1 ? fmodf(u * 1.4f, 1) : -1, 0.4f, true);
  }
  // die Steine fallen daneben
  for (int i = 0; i < 3; i++) {
    float k = seg(u, 22 + i * 1.4f, 23.2f + i * 1.4f);
    if (k > 0 && k < 1) stein(g, 200 + i * 10 + k * 30, lerp(120, 226, hinein(k)), 4);
  }
  g.kameraAus();
  vers(g, {"Sie fanden die Diebe nicht. Als", "es dunkel wurde, stiegen sie", "auf einen Baum."}, aufzu(u, 0.4f, 8));
  vers(g, {"Da kamen gerade die Diebe,", "setzten sich unter den Baum", "und machten ein Feuer."}, aufzu(u, 8.4f, 16));
  vers(g, {"Frieder warf mit Steinen nach", "ihnen, aber er traf nicht."}, aufzu(u, 21.4f, 27));
  blase(g, 190, 120, {"Bald ist Morgen. Der Wind", "schüttelt die Tannäpfel herunter."}, aufzu(u, 27.4f, 33.6f), 184, 166);
}

// gemeinsames Bild fuer die Nacht im Baum mit den Dieben am Feuer
static void baumBild(Gfx &g, float u, float kathMund, float kathArm, float friederMund, float friederArm, float diebMund,
                     bool tuerDa, bool hutzelnDa, bool essigDa, float diebeWeg = 0) {
  waldNacht(g, u, diebeWeg > 0 ? 1 - seg(diebeWeg, 0, 3) * 0.6f : 1);
  frieder(g, 190, 128, 0.7f, -1, u, friederMund, friederArm, -1, 0, friederMund > 0.4f ? 0.8f : 0);
  if (tuerDa) tuer(g, 104, 92, 0.72f, 0.08f);
  kath(g, 108, 128, 0.7f, 1, u, kathMund, kathArm, -1, false, tuerDa ? 2.6f : 0.4f);
  if (hutzelnDa) hutzeln(g, 104, 104, 0.55f);
  if (essigDa) essigkrug(g, 116, 104, 0.55f);
  for (int i = 0; i < 2; i++) {
    float x = 180 + i * 50 + (diebeWeg > 0 ? hinein(seg(diebeWeg, 0, 4)) * 260 : 0);
    dieb(g, i, x, 228, 0.78f, diebeWeg > 0 ? 1 : -1, u, i == 0 ? diebMund : diebMund * 0.6f,
         diebeWeg > 0 ? fmodf(u * 2.4f, 1) : -1, diebeWeg > 0 ? 2.8f : 0.4f, diebeWeg <= 0);
  }
}

// 14) Hutzeln und Essig
static void szeneHutzeln(Gfx &g, float u) {
  auto sp = [&](float a, float b) { return u > a && u < b ? 0.5f + 0.3f * sinf(u * 9) : 0.f; };
  bool hutzelnDa = u < 17, essigDa = u < 30.5f;
  baumBild(g, u, sp(0.4f, 5) + sp(9.8f, 13.5f) + sp(23.4f, 27.5f), u > 17 && u < 18 ? 1.4f : u > 30 && u < 32 ? 1.4f : 0.6f,
           sp(5.4f, 9.5f) + sp(13.8f, 17) + sp(27.8f, 30.5f), 0.4f, sp(19.4f, 23) + sp(33.4f, 38), true, hutzelnDa, essigDa);
  float hk = seg(u, 17, 19.4f);
  if (hk > 0 && hk < 1) for (int i = 0; i < 6; i++) g.ellipse(108 + zufall(i) * 40, lerp(110, 220, hinein(hk)) - zufall(i + 9) * 20, 2.4f, 3.2f, FARBE(0x7A4E2A));
  float ek = seg(u, 30.5f, 33.4f);
  if (ek > 0) essigkrug(g, 118, 106, 0.55f, 2.4f * klemme(ek * 3));
  if (ek > 0 && ek < 1) for (int i = 0; i < 8; i++) tropfen(g, 160 + zufall(i) * 60, lerp(110, 222, fmodf(ek * 2 + zufall(i + 4), 1)), 2, FARBE(0xC8E0A0));
  g.kameraAus();
  blase(g, 110, 60, {"Friederchen, ich muss die", "Hutzeln hinunterwerfen!"}, aufzu(u, 0.4f, 5), 108, 60);
  blase(g, 190, 60, {"Nein, jetzt nicht!", "Sie könnten uns verraten!"}, aufzu(u, 5.4f, 9.5f), 190, 60);
  blase(g, 110, 60, {"Ach, ich muss, sie drücken", "mich gar zu sehr!"}, aufzu(u, 9.8f, 13.5f), 108, 60);
  blase(g, 190, 60, {"So tu's in Henkers Namen!"}, aufzu(u, 13.8f, 17), 190, 60);
  blase(g, 190, 150, {"Die Vögel misten."}, aufzu(u, 19.4f, 23), 184, 166);
  blase(g, 110, 60, {"Ach, Friederchen, ich muss", "den Essig ausschütten!"}, aufzu(u, 23.4f, 27.5f), 108, 60);
  blase(g, 190, 60, {"In Henkers Namen!"}, aufzu(u, 27.8f, 30.5f), 190, 60);
  blase(g, 190, 150, {"Der Tau tröpfelt schon!"}, aufzu(u, 33.4f, 38), 184, 166);
}

// 15) Die Tuer faellt - die Diebe fliehen
static void szeneTuerFaellt(Gfx &g, float u) {
  auto sp = [&](float a, float b) { return u > a && u < b ? 0.5f + 0.3f * sinf(u * 9) : 0.f; };
  bool tuerDa = u < 17;
  float weg = u > 18.6f ? u - 18.6f : 0;
  baumBild(g, u, sp(0.4f, 6) + sp(9.8f, 13), 0.6f, sp(6.4f, 9.5f) + sp(13.4f, 17), 0.4f, u > 18.6f && u < 23 ? 0.9f : 0, tuerDa, false,
           false, weg);
  float k = seg(u, 17, 18.4f);
  if (k > 0 && k < 1) tuer(g, lerp(104, 170, k), lerp(92, 210, hinein(k)), 0.8f, k * 2.6f);
  if (u >= 18.4f) {
    tuer(g, 170, 220, 0.8f, 1.57f);
    staubwolke(g, 170, 220, 1.2f, u, (uint8_t)(220 * (1 - seg(u, 18.4f, 21))));
  }
  g.kameraAus();
  blase(g, 110, 60, {"Friederchen, die Tür drückt", "mich so. Ich muss sie", "hinunterwerfen!"}, aufzu(u, 0.4f, 6), 108, 60);
  blase(g, 190, 60, {"Nein! Halt sie fest!"}, aufzu(u, 6.4f, 9.5f), 190, 60);
  blase(g, 110, 60, {"Ach, sie fällt mir schon!"}, aufzu(u, 9.8f, 13), 108, 60);
  blase(g, 190, 60, {"So lass sie fallen", "in Teufels Namen!"}, aufzu(u, 13.4f, 17), 190, 60);
  blase(g, 200, 150, {"Der Teufel kommt", "vom Baum herab!"}, aufzu(u, 18.6f, 23), 200, 170);
  vers(g, {"Die Diebe liefen davon und", "ließen alles liegen."}, aufzu(u, 24.4f, 29.6f));
}

// 16) Am Morgen: das Gold ist wieder da
static void szeneMorgen(Gfx &g, float u) {
  if (u < 10) {
    waldNacht(g, u, 0);
    // Morgenroete ueber den Nachthimmel
    g.verlauf(0, 200, FARBE(0xFFC890), FARBE(0xFFE8C0));
    for (int i = 0; i < 6; i++) tanne(g, i * 56 - 20, 210, 0.9f, FARBE(0x2F8F4E));
    boden(g, 206);
    g.kontur(1.6f, OUT);
    g.rechteck(136, 60, 26, 150, FARBE(0x7A5A3A));
    g.konturAus();
    for (int i = 0; i < 7; i++) g.kreis(70 + i * 26, 70 + (i & 1) * 12, 30, FARBE(0x3E8E4E));
    tuer(g, 170, 220, 0.8f, 1.57f);
    for (int i = 0; i < 2; i++) {
      g.kontur(1.2f, OUT);
      g.ellipse(210 + i * 30, 214, 12, 13, FARBE(0xC8A878));
      g.konturAus();
      g.stern(210 + i * 30, 212, 5, 2, 4, u * 2 + i, GOLD);
    }
    frieder(g, 110, 222, 0.85f, 1, u, 0.8f, 2.4f + sinf(u * 4) * 0.3f);
    kath(g, 70, 222, 0.8f, 1, u, 0.6f, 2.2f);
    glitzern(g, 225, 205, 30, u, 6);
    g.kameraAus();
    vers(g, {"Am Morgen fanden die beiden", "ihr ganzes Gold wieder und", "trugen es heim."}, aufzu(u, 0.4f, 9.6f));
    return;
  }
  float v = u - 10;
  haus(g, u, 6);
  float k = seg(v, 0, 4);
  frieder(g, lerp(-20, 190, k), 222, 0.92f, -1 + 2 * (k < 1), u, v > 4.4f && v < 9.5f ? 0.5f + 0.2f * sinf(u * 8) : 0.3f, 0.4f,
          k < 1 ? fmodf(u * 1.4f, 1) : -1);
  tuer(g, lerp(-60, 130, k) + 4, 180, 0.85f, 0.08f);
  kath(g, lerp(-60, 130, k), 222, 0.85f, 1, u, v > 9.8f ? 0.5f + 0.2f * sinf(u * 8) : 0.3f, 2.6f, k < 1 ? fmodf(u * 1.4f, 1) : -1,
       false, 2.6f);
  g.kameraAus();
  blase(g, 200, 50, {"Katherlieschen, nun musst du", "aber auch fleißig arbeiten!"}, aufzu(v, 4.4f, 9.5f), 196, 130);
  blase(g, 110, 50, {"Ja, Friederchen, ich geh aufs", "Feld und schneide Frucht!"}, aufzu(v, 9.8f, 15.6f), 130, 140);
}

// 17) Auf dem Feld: essen, schlafen, Kleider zerschneiden - "Bin ich's oder bin ich's nicht?"
static void szeneFeld(Gfx &g, float u) {
  feld(g, u, seg(u, 22, 30) * 0.6f);
  bool zer = u > 19;
  bool schlaeft = u > 13 && u < 26;
  if (!schlaeft) {
    kath(g, 130, 222, 0.85f, 1, u, (u > 0.4f && u < 10) || u > 26.4f ? 0.5f + 0.2f * sinf(u * 8) : u > 10 ? 0.4f + 0.4f * sinf(u * 10) : 0,
         u > 10 && u < 13 ? 2.4f + sinf(u * 10) * 0.3f : u > 26.4f ? 2.6f : 0.4f, -1, zer, u > 26.4f ? 2.6f : 0.2f);
    if (u > 26.4f) fragezeichen(g, 150, 120, 1);
  } else {
    // sie sitzt im Halbschlaf und schneidet
    kath(g, 130, 230, 0.8f, 1, u, 0, u > 15 && u < 21 ? 1.2f + sinf(u * 6) * 0.4f : 0.4f, -1, zer, 0.2f, true);
    if (u > 15 && u < 21) {
      float sx = 146, sy = 186 + sinf(u * 6) * 4;  // Sichel
      g.bogen(sx, sy, 8, 3.6f, 6.0f, 2.4f, STEIN_D);
      g.linie(sx - 8, sy + 2, sx - 12, sy + 8, 2.6f, HOLZ_D);
      for (int i = 0; i < 3; i++) {
        float k = fmodf(u * 0.8f + i * 0.33f, 1);
        g.ellipse(130 + i * 8 + k * 20, 210 + k * 14, 4, 2, i & 1 ? WEISS : KATH.kleid, (uint8_t)(255 * (1 - k)), k * 3);
      }
    }
    zzz(g, 150, 130, u);
  }
  g.kameraAus();
  blase(g, 140, 50, {"Ess ich, eh ich schneid,", "oder schlaf ich, eh ich schneid?"}, aufzu(u, 0.4f, 6), 130, 140);
  blase(g, 140, 50, {"Hei, ich will erst essen!"}, aufzu(u, 6.4f, 10), 130, 140);
  vers(g, {"Da wurde sie schläfrig und", "zerschnitt im Halbschlaf ihre", "Kleider: Schürze, Rock und Hemd."}, aufzu(u, 13.4f, 21.6f));
  blase(g, 140, 50, {"Bin ich's, oder bin ich's nicht?", "Ach, ich bin's nicht!"}, aufzu(u, 26.4f, 33.6f), 130, 140);
}

// 18) Am Fenster: "Ist das Katherlieschen drinnen?"
static void szeneFenster(Gfx &g, float u) {
  haus(g, u, 6, 1);
  // Frieder schaut aus dem hellen Fenster
  Pose f = posis(FRIEDER, u);
  f.mund = u > 10.8f && u < 15 ? 0.5f + 0.2f * sinf(u * 8) : 0;
  f.aussehen = &FRIEDER;
  g.ausschnitt(196, 100, 30, 58);
  schneiderlein(g, 211, 186, 0.6f, -1, f);
  g.ausschnittAus();
  float k = seg(u, 0, 5), w = seg(u, 20, 25);
  float kx = w > 0 ? lerp(176, -40, w) : lerp(-30, 176, k);
  kath(g, kx, 222, 0.8f, w > 0 ? -1 : 1, u, (u > 6 && u < 10.5f) || (u > 15.4f && u < 20) ? 0.5f + 0.2f * sinf(u * 8) : 0.2f,
       u > 6 && u < 10.5f ? 2.2f : 0.4f, (k < 1 || w > 0) ? fmodf(u * 1.6f, 1) : -1, true);
  g.kameraAus();
  vers(g, {"Abends lief sie ins Dorf", "an Frieders Fenster."}, aufzu(u, 0.4f, 5.6f));
  blase(g, 120, 60, {"Friederchen! Ist das", "Katherlieschen drinnen?"}, aufzu(u, 6, 10.5f), 176, 140);
  blase(g, 200, 60, {"Ja, ja, das wird wohl drin", "liegen und schlafen."}, aufzu(u, 10.8f, 15), 210, 120);
  blase(g, 120, 60, {"Gut, dann bin ich gewiss", "schon zu Haus!"}, aufzu(u, 15.4f, 20), 176, 140);
}

// 19) Bei den Dieben im Ruebenland des Pfarrers
static void szeneRueben(Gfx &g, float u) {
  ruebenland(g, u);
  bool hockt = u > 10;
  float dk = seg(u, 9, 13);
  for (int i = 0; i < 2; i++) {
    if (dk >= 1) break;
    dieb(g, i, lerp(190 + i * 40, 330 + i * 40, dk), 222, 0.78f, dk > 0 ? 1 : -1, u, i == 0 && u > 4.8f && u < 9.5f ? 0.5f + 0.2f * sinf(u * 8) : 0,
         dk > 0 ? fmodf(u * 1.6f, 1) : -1, 0.4f);
  }
  if (!hockt) kath(g, 120, 222, 0.8f, 1, u, u > 0.4f && u < 4.5f ? 0.5f + 0.2f * sinf(u * 8) : 0.2f, u < 4.5f ? 1.4f : 0.4f, -1, true);
  else kath(g, 120, 236, 0.7f, 1, u, 0, 0.6f + sinf(u * 2) * 0.2f, -1, true);
  // der Mann am Zaun sieht sie
  if (u > 14) {
    Pose m = posis(MANN, u);
    m.mund = u > 16.4f ? 0.8f : 0.4f;
    m.armV = u > 16.4f ? 2.8f : 0.4f;
    m.armH = u > 16.4f ? 2.8f : 0.2f;
    float mk = seg(u, 20, 24);
    schneiderlein(g, lerp(214, 330, mk), 222, 0.8f, mk > 0 ? 1 : -1, m);
  }
  g.kameraAus();
  blase(g, 120, 60, {"Ich helf euch stehlen!"}, aufzu(u, 0.4f, 4.5f), 120, 140);
  blase(g, 190, 60, {"Geh hin zum Pfarrer und", "rupf uns Rüben!"}, aufzu(u, 4.8f, 9.5f), 190, 140);
  vers(g, {"Sie war zu faul, sich aufzurichten,", "und blieb gebückt hocken."}, aufzu(u, 10.4f, 16));
  blase(g, 110, 60, {"Herr Pfarrer, in Eurem Rübenland", "ist der Teufel und rupft!"}, aufzu(u, 16.4f, 22), 214, 140);
}

// 20) Der Pfarrer mit dem lahmen Fuss - und alle laufen davon
static void szenePfarrer(Gfx &g, float u) {
  ruebenland(g, u);
  bool steht = u > 9;
  float reck = seg(u, 9, 11);
  if (!steht) kath(g, 120, 236, 0.7f, 1, u, 0, 0.6f, -1, true);
  else kath(g, 120, lerp(236, 222, reck), lerp(0.7f, 0.85f, reck), -1, u, 0.6f, 2.8f, -1, true, 2.8f, u > 9 && u < 11);
  // der Mann traegt den Pfarrer auf dem Ruecken heran, dann laufen beide fort
  float k = seg(u, 0, 7), w = seg(u, 14, 30);
  Pose m = posis(MANN, u);
  m.armV = 1.8f;
  m.lauf = (k < 1 || w > 0) ? fmodf(u * 1.5f, 1) : -1;
  m.mund = w > 0 ? 0.9f : 0.3f;
  Pose p = posis(PFARRER, u);
  p.mund = u > 11.4f ? 0.9f : 0.2f;
  p.armV = u > 11.4f ? 2.8f : 1.4f;
  p.armH = u > 11.4f ? 2.8f : 0.2f;
  if (w <= 0) {
    float mx = lerp(330, 200, k);
    schneiderlein(g, mx, 222, 0.85f, -1, m);
    schneiderlein(g, mx + 6, 176, 0.65f, -1, p);
  } else {
    // der Pfarrer ist schneller als der Mann
    p.lauf = fmodf(u * 2.2f, 1);
    schneiderlein(g, lerp(200, 300, w), 222, 0.85f, 1, m);
    schneiderlein(g, lerp(186, 420, w), 222, 0.75f, 1, p);
    staubwolke(g, lerp(170, 400, w), 220, 0.8f, u);
  }
  g.kameraAus();
  vers(g, {"Der Pfarrer hatte einen lahmen", "Fuß, da trug ihn der Mann", "auf dem Rücken hinaus."}, aufzu(u, 0.4f, 8.6f));
  blase(g, 90, 60, {"Ach, der Teufel!"}, aufzu(u, 11.4f, 15), 200, 140);
  vers(g, {"Und der Pfarrer mit dem lahmen", "Fuß lief schneller als der Mann", "mit den gesunden Beinen."}, aufzu(u, 16, 27));
  endeBild(g, u, 28, 120, 170);
}

static void bild(Gfx &g, float t) {
  if (t < P2) szeneAuftrag(g, t - P1);
  else if (t < P3) szeneWurst(g, t - P2);
  else if (t < P4) szeneHund(g, t - P3);
  else if (t < P5) szeneMehl(g, t - P4);
  else if (t < P6) szeneHeim(g, t - P5);
  else if (t < P7) szeneGold(g, t - P6);
  else if (t < P8) szeneKraemer(g, t - P7);
  else if (t < P9) szeneZaun(g, t - P8);
  else if (t < P10) szeneZierat(g, t - P9);
  else if (t < P11) szeneButter(g, t - P10);
  else if (t < P12) szeneBrot(g, t - P11);
  else if (t < P13) szeneTuer(g, t - P12);
  else if (t < P14) szeneBaum(g, t - P13);
  else if (t < P15) szeneHutzeln(g, t - P14);
  else if (t < P16) szeneTuerFaellt(g, t - P15);
  else if (t < P17) szeneMorgen(g, t - P16);
  else if (t < P18) szeneFeld(g, t - P17);
  else if (t < P19) szeneFenster(g, t - P18);
  else if (t < P20) szeneRueben(g, t - P19);
  else szenePfarrer(g, t - P20);
}

// =====================================================================
// Musik
// =====================================================================
// Katherlieschens Liedchen (G-Dur, huepfend wie ein Laendler)
static const Ton3 MEL_KATH[] = {
  {67, .5, 43}, {71, .5, 0}, {74, .5, 0}, {71, .5, 0}, {72, 1, 48}, {69, .5, 0}, {72, .5, 0},
  {71, .5, 43}, {67, .5, 0}, {69, .5, 0}, {71, .5, 0}, {69, 1, 50}, {66, 1, 0},
  {67, .5, 43}, {71, .5, 0}, {74, .5, 0}, {79, .5, 0}, {78, .5, 48}, {76, .5, 0}, {74, .5, 0}, {72, .5, 0},
  {71, .5, 50}, {69, .5, 0}, {66, .5, 0}, {69, .5, 0}, {67, 2, 43}};
// Ach Friederchen (kleines Seufzer-Motiv)
static void seufzer(Synth &s, float x) { s.note(x, 76, 0.5f); s.note(x + 0.3f, 72, 0.45f); s.note(x + 0.6f, 67, 0.4f); }
static void wuff(Synth &s, float x) { s.ton(x, 260, 0.25f, 0.005f, 0.12f, SAEGE, GERAEUSCH, 160); s.rauschen(x, 0.1f, BANDPASS, 700, 0.1f); }
static void plumps(Synth &s, float x, float st = 1) { s.ton(x, 160, 0.4f * st, 0.002f, 0.25f, SINUS, GERAEUSCH, 50); s.rauschen(x, 0.3f, TIEFPASS, 700, 0.2f * st); }
static void ton(Synth &s, float t0, float t1) {
  float a = P1;
  spieleMelodie(s, M(MEL_KATH), a + 0.5f, a + 25.5f, 0.3f, t0, t1, 0.65f);
  a = P2;
  spieleMelodie(s, M(MEL_KATH), a + 0.4f, a + 13.6f, 0.3f, t0, t1, 0.55f);
  for (int i = 0; i < 20; i++) { float x = a + 5 + i * 0.4f; if (bei(x, t0, t1)) s.rauschen(x, 0.3f, HOCHPASS, 3000, 0.03f); }  // brutzeln
  if (bei(a + 17, t0, t1)) s.rauschen(a + 17, 8, BANDPASS, 1200, 0.05f, 2, 0.2f);  // das Bier laeuft
  if (bei(a + 20.4f, t0, t1)) seufzer(s, a + 20.4f);
  spieleMelodie(s, M(MEL_JAGD), a + 21, a + 25.8f, 0.2f, t0, t1, 0.5f);
  a = P3;
  if (bei(a + 1, t0, t1)) wuff(s, a + 1);
  if (bei(a + 1.4f, t0, t1)) wuff(s, a + 1.4f);
  spieleMelodie(s, M(MEL_JAGD), a + 2, a + 14, 0.2f, t0, t1, 0.6f);
  if (bei(a + 14, t0, t1)) seufzer(s, a + 14);
  if (bei(a + 18, t0, t1)) s.rauschen(a + 18, 9.6f, BANDPASS, 1100, 0.06f, 2, 0.4f);
  spieleMelodie(s, M(MEL_SPUK), a + 18.4f, a + 27.6f, 0.42f, t0, t1, 0.35f);
  a = P4;
  if (bei(a + 0.4f, t0, t1)) seufzer(s, a + 0.4f);
  if (bei(a + 10, t0, t1)) s.rauschen(a + 10, 10, HOCHPASS, 4000, 0.04f, 1, 0.5f);  // Mehl rieselt
  if (bei(a + 13, t0, t1)) { s.ton(a + 13, 900, 0.2f, 0.002f, 0.4f, DREIECK, GERAEUSCH, 700); plumps(s, a + 13.1f, 0.6f); }
  spieleMelodie(s, M(MEL_KATH), a + 14, a + 29.6f, 0.3f, t0, t1, 0.55f);
  a = P5;
  spieleMelodie(s, M(MEL_KATH), a + 0.4f, a + 17, 0.34f, t0, t1, 0.4f);
  if (bei(a + 17.4f, t0, t1)) for (int k = 0; k < 3; k++) s.note(a + 17.4f + k * 0.15f, 60 - k * 2, 0.6f);
  if (bei(a + 23, t0, t1)) seufzer(s, a + 23);
  a = P6;
  for (int i = 0; i < 8; i++) if (bei(a + 0.4f + i * 0.5f, t0, t1)) s.note(a + 0.4f + i * 0.5f, 84 + (i % 3) * 3, 0.35f);  // Gold klimpert
  for (int i = 0; i < 6; i++) if (bei(a + 5 + i * 0.35f, t0, t1)) s.rauschen(a + 5 + i * 0.35f, 0.12f, BANDPASS, 900, 0.1f);  // graben
  if (bei(a + 3, t0, t1)) s.ton(a + 3, 160, 0.2f, 0.05f, 0.9f, SAEGE, GERAEUSCH, 120);  // Muh
  spieleMelodie(s, M(MEL_WIEGE), a + 6.4f, a + 25.6f, 0.4f, t0, t1, 0.5f);
  a = P7;
  spieleMelodie(s, M(MEL_WANDER), a + 0.4f, a + 26.6f, 0.32f, t0, t1, 0.5f);
  for (int i = 0; i < 8; i++) if (bei(a + 0.4f + i * 0.7f, t0, t1)) s.rauschen(a + 0.4f + i * 0.7f, 0.06f, BANDPASS, 2400, 0.08f);  // Toepfe klappern
  spieleMelodie(s, M(MEL_JAGD), a + 27, a + 35.6f, 0.2f, t0, t1, 0.55f);
  a = P8;
  for (int i = 0; i < 6; i++) if (bei(a + 8 + i * 1.33f, t0, t1)) s.ton(a + 8 + i * 1.33f, 800, 0.2f, 0.002f, 0.2f, DREIECK, GERAEUSCH, 500);  // klonk
  spieleMelodie(s, M(MEL_KATH), a + 0.4f, a + 21.6f, 0.3f, t0, t1, 0.55f);
  a = P9;
  spieleMelodie(s, M(MEL_SPUK), a + 0.4f, a + 20.5f, 0.42f, t0, t1, 0.35f);
  if (bei(a + 20.8f, t0, t1)) seufzer(s, a + 20.8f);
  spieleMelodie(s, M(MEL_WANDER), a + 25.2f, a + 29.6f, 0.3f, t0, t1, 0.5f);
  a = P10;
  spieleMelodie(s, M(MEL_WANDER), a + 0.4f, a + 21.6f, 0.34f, t0, t1, 0.5f);
  for (int i = 0; i < 6; i++) {
    float st = a + 22 + (i == 0 ? 0 : 9 + i * 1.6f);
    for (int k = 0; k < 6; k++) if (bei(st + k * 0.5f, t0, t1)) s.ton(st + k * 0.5f, 220 - k * 15, 0.12f, 0.005f, 0.12f, SINUS, GERAEUSCH, 180);  // rollt
  }
  spieleMelodie(s, M(MEL_KATH), a + 22, a + 39.6f, 0.26f, t0, t1, 0.5f);
  a = P11;
  spieleMelodie(s, M(MEL_KATH), a + 0.4f, a + 15.5f, 0.34f, t0, t1, 0.4f);
  if (bei(a + 19.8f, t0, t1)) seufzer(s, a + 19.8f);
  spieleMelodie(s, M(MEL_WIEGE), a + 24, a + 27.8f, 0.4f, t0, t1, 0.45f);
  a = P12;
  spieleMelodie(s, M(MEL_KATH), a + 0.4f, a + 45.6f, 0.32f, t0, t1, 0.5f);
  if (bei(a + 21, t0, t1)) for (int k = 0; k < 3; k++) s.rauschen(a + 21 + k * 0.4f, 0.15f, BANDPASS, 500, 0.15f);  // Tuer aushaengen
  a = P13;
  spieleMelodie(s, M(MEL_N), a + 0.4f, a + 33.6f, 0.42f, t0, t1, 0.45f);
  for (int i = 0; i < 3; i++) if (bei(a + 23.2f + i * 1.4f, t0, t1)) plumps(s, a + 23.2f + i * 1.4f, 0.5f);
  if (bei(a + 13, t0, t1)) s.rauschen(a + 13, 20, BANDPASS, 1600, 0.03f, 1, 1);  // Feuer knistert
  a = P14;
  spieleMelodie(s, M(MEL_SPUK), a + 0.4f, a + 38, 0.42f, t0, t1, 0.35f);
  for (int i = 0; i < 6; i++) if (bei(a + 17.4f + i * 0.25f, t0, t1)) s.ton(a + 17.4f + i * 0.25f, 300, 0.12f, 0.002f, 0.1f, SINUS, GERAEUSCH, 200);
  for (int i = 0; i < 10; i++) if (bei(a + 30.5f + i * 0.25f, t0, t1)) s.ton(a + 30.5f + i * 0.25f, 1800 + zufall(i) * 800, 0.06f, 0.002f, 0.08f, SINUS, GERAEUSCH, 1400);
  a = P15;
  spieleMelodie(s, M(MEL_SPUK), a + 0.4f, a + 17, 0.42f, t0, t1, 0.4f);
  if (bei(a + 18.4f, t0, t1)) { plumps(s, a + 18.4f, 1.5f); s.rauschen(a + 18.4f, 0.6f, TIEFPASS, 1200, 0.5f); }
  spieleMelodie(s, M(MEL_JAGD), a + 19, a + 24, 0.18f, t0, t1, 0.6f);
  spieleMelodie(s, M(MEL_KATH), a + 24.4f, a + 29.6f, 0.3f, t0, t1, 0.6f);
  a = P16;
  for (int i = 0; i < 8; i++) if (bei(a + 0.4f + i * 0.4f, t0, t1)) s.note(a + 0.4f + i * 0.4f, 84 + (i % 4) * 2, 0.4f);
  spieleMelodie(s, M(MEL_F), a + 3.4f, a + 25.6f, 0.28f, t0, t1, 0.55f);
  a = P17;
  spieleMelodie(s, M(MEL_KATH), a + 0.4f, a + 13, 0.34f, t0, t1, 0.5f);
  spieleMelodie(s, M(MEL_WIEGE), a + 13.4f, a + 26, 0.45f, t0, t1, 0.45f);
  for (int i = 0; i < 10; i++) if (bei(a + 15 + i * 0.6f, t0, t1)) s.rauschen(a + 15 + i * 0.6f, 0.1f, HOCHPASS, 3400, 0.06f);  // schnipp
  if (bei(a + 26.4f, t0, t1)) seufzer(s, a + 26.4f);
  spieleMelodie(s, M(MEL_SPUK), a + 27.4f, a + 33.6f, 0.42f, t0, t1, 0.35f);
  a = P18;
  spieleMelodie(s, M(MEL_N), a + 0.4f, a + 20, 0.42f, t0, t1, 0.4f);
  for (int i = 0; i < 3; i++) if (bei(a + 5.4f + i * 0.2f, t0, t1)) s.rauschen(a + 5.4f + i * 0.2f, 0.05f, BANDPASS, 1400, 0.2f);  // klopf
  spieleMelodie(s, M(MEL_KATH), a + 20.4f, a + 25.8f, 0.24f, t0, t1, 0.5f);
  a = P19;
  spieleMelodie(s, M(MEL_SPUK), a + 0.4f, a + 16, 0.42f, t0, t1, 0.35f);
  spieleMelodie(s, M(MEL_JAGD), a + 16.4f, a + 23.6f, 0.2f, t0, t1, 0.5f);
  a = P20;
  spieleMelodie(s, M(MEL_WANDER), a + 0.4f, a + 9, 0.34f, t0, t1, 0.45f);
  if (bei(a + 11.4f, t0, t1)) for (int k = 0; k < 4; k++) s.note(a + 11.4f + k * 0.12f, 84 - k * 4, 0.6f);
  spieleMelodie(s, M(MEL_JAGD), a + 13, a + 22, 0.16f, t0, t1, 0.6f);
  spieleMelodie(s, M(MEL_KATH), a + 22.4f, a + 28, 0.28f, t0, t1, 0.65f);
  endeKlang(s, a + 28.4f, t0, t1);
}

// =====================================================================
// Startbild, Vorschau, Erkennungszeichen
// =====================================================================
static void titelSzene(Gfx &g, float t) {
  haus(g, t, 6);
  frieder(g, 196, 226, 0.9f, -1, t, 0.5f, 1.4f);
  tuer(g, 136, 184, 0.85f, 0.08f);
  kath(g, 132, 226, 0.85f, 1, t, 0.4f, 2.6f, -1, false, 2.6f);
  hund(g, 240, 226, 0.8f, -1, t, -1, true);
}
static void titelbild(Gfx &g, float t) {
  titelSzene(g, t);
  titelRahmen(g, t, "Der Frieder und das Katherlieschen", 262, SCHRIFT_B);
}
static void vorschau(Gfx &g, float t) { titelSzene(g, t); }
static void symbol(Gfx &g, float x, float y, float t) {
  tuer(g, x - 18, y - 36, 1.1f, 0.15f);
  kaeselaib(g, x + 26, y - 14, 13, t * 2);
  wurst(g, x + 24, y - 48, 1.2f, -0.3f);
}

const Maerchen KATHERLIESCHEN = {"Der Frieder und das Katherlieschen", PDAUER, FARBE(0x4E8EDC), bild, ton, titelbild, vorschau,
                                 symbol, LIED(MEL_KATH, 0.3f), nullptr};

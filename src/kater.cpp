// Der gestiefelte Kater - ein Spieluhr-Film fuer die Maerchenbox, nach den Bruedern Grimm.
// Der juengste Muellerssohn erbt nur einen Kater. Der laesst sich Stiefel machen, faengt Rebhuehner fuer den Koenig,
// macht seinen Herrn zum "Grafen", ueberlistet den Zauberer - und am Ende ist Hochzeit.
#include "erzaehlen.h"

// ---------- Ablauf (Sekunden) ----------
static const float P1 = 0, P2 = 24, P3 = 48, P4 = 72, P5 = 104, P6 = 128, P7 = 162, PDAUER = 190;

static Aussehen hansLook() { Aussehen a; a.weste = FARBE(0xE8E4DC); a.hemd = FARBE(0xF6F2EA); a.hose = FARBE(0xC9C0B0); a.haar = FARBE(0xC98A3A); a.muetze = 1; return a; }
static Aussehen grafLook() {
  Aussehen a; a.weste = FARBE(0x2E5FAE); a.hemd = WEISS; a.hose = FARBE(0x1E2E5A); a.haar = FARBE(0xC98A3A);
  a.hut = FARBE(0x1E2E5A); a.hutD = OUT; a.feder = WEISS; a.halstuch = WEISS;
  return a;
}
static Aussehen bauerLook(int i) { Aussehen a; a.weste = i ? FARBE(0x8E6E3E) : FARBE(0x6E8E4E); a.hose = FARBE(0x5A4A3A); a.haar = i ? FARBE(0x3A2A1A) : FARBE(0xA8702E); a.hut = FARBE(0xE8C860); a.hutD = FARBE(0xC0A040); a.feder = FARBE(0xE8C860); return a; }
static Aussehen zauberLook() {
  Aussehen a; a.weste = FARBE(0x3A2A6E); a.hemd = FARBE(0x5A3A8E); a.hose = FARBE(0x2A1E4A); a.haar = FARBE(0x9A9AA2);
  a.hut = FARBE(0x3A2A6E); a.hutD = FARBE(0x2A1E4A); a.feder = GOLD; a.wangen = false;
  return a;
}
static FrauLook prinzLook() { FrauLook l; l.kleid = FARBE(0x9AC8F2); l.haar = FARBE(0xF2C14E); l.krone = true; l.glanz = true; return l; }
static const Aussehen HANS = hansLook(), GRAF = grafLook(), B0 = bauerLook(0), B1 = bauerLook(1), ZAUBERER = zauberLook();
static const FrauLook PRINZESSIN = prinzLook();

// ---------- der Kater ----------
// steht auf zwei Beinen, mit roten Stiefeln und Federhut; stiefel = false: barfuss (am Anfang)
static void kater(Gfx &g, float x, float y, float s, float dir, float t, float lauf = -1, float mund = 0, float armV = 0.6f,
                  bool stiefel = true, bool hut = true, float verbeug = 0) {
  const Farbe fell = FARBE(0xE89A3A), hell = FARBE(0xFBE0B8), streif = FARBE(0xC0702A), stief = FARBE(0xA8282E);
  auto PX = [&](float d) { return x + d * s * dir; };
  auto PY = [&](float d) { return y + d * s; };
  float hop = lauf >= 0 ? fabsf(sinf(lauf * 6.28f)) * 3 * s : 0;
  // Schwanz
  g.kurve(PX(-10), PY(-22) - hop, PX(-26), PY(-26) - hop, PX(-24 + sinf(t * 2) * 4), PY(-48) - hop, 4 * s, fell);
  // Beine mit Stiefeln
  g.kontur(1.2f * s, OUT);
  for (int k = 0; k < 2; k++) {
    float sw = lauf >= 0 ? sinf(lauf * 6.28f + k * 3.14f) * 6 : 0;
    g.linie(PX(-5 + k * 10), PY(-20) - hop, PX(-5 + k * 10 + sw), PY(-6), 6 * s, fell);
    if (stiefel) {
      g.rundRechteck(PX(-5 + k * 10 + sw) - 4.5f * s, PY(-16), 9 * s, 13 * s, 2 * s, stief);
      g.ellipse(PX(-3 + k * 10 + sw), PY(-3), 7 * s, 3.5f * s, stief);
      g.rechteck(PX(-5 + k * 10 + sw) - 6 * s, PY(-18), 12 * s, 4 * s, FARBE(0x7A1A1E));
    } else g.ellipse(PX(-3 + k * 10 + sw), PY(-3), 5 * s, 3 * s, fell);
  }
  // Koerper (leicht nach vorn gebeugt beim Verbeugen)
  float bx = PX(verbeug * 6), by = PY(-30 + verbeug * 4) - hop;
  g.ellipse(bx, by, 12 * s, 15 * s, fell, 255, verbeug * 0.5f * dir);
  g.konturAus();
  g.ellipse(bx + 2 * s * dir, by + 2 * s, 7 * s, 10 * s, hell);
  // Arm
  float ax = bx + 6 * s * dir, ay = by - 8 * s;
  g.kontur(1.1f * s, OUT);
  g.linie(ax, ay, ax + sinf(armV) * 14 * s * dir, ay + cosf(armV) * 14 * s, 5 * s, fell);
  // Kopf
  float kx = bx + (2 + verbeug * 8) * s * dir, ky = by - (22 - verbeug * 6) * s;
  float o1[] = {kx - 9 * s * dir, ky - 3 * s, kx - 9 * s * dir, ky - 17 * s, kx - 2 * s * dir, ky - 8 * s};
  float o2[] = {kx + 2 * s * dir, ky - 8 * s, kx + 9 * s * dir, ky - 17 * s, kx + 10 * s * dir, ky - 3 * s};
  g.polygon(o1, 3, fell);
  g.polygon(o2, 3, fell);
  g.kreis(kx, ky, 11 * s, fell);
  g.konturAus();
  for (int i = 0; i < 3; i++) g.linie(kx - 4 * s + i * 4 * s, ky - 10 * s, kx - 4 * s + i * 4 * s, ky - 6 * s, 1.4f * s, streif);
  g.ellipse(kx + 4 * s * dir, ky + 4 * s, 6 * s, 4 * s, hell);
  g.kreis(kx + 6 * s * dir, ky + 2 * s, 1.4f * s, FARBE(0xE86F7A));
  for (int i = -1; i <= 1; i++) g.linie(kx + 7 * s * dir, ky + 4 * s, kx + 17 * s * dir, ky + 3 * s + i * 2.5f * s, 0.6f * s, OUT);
  for (int i = 0; i < 2; i++) {
    float ex = kx + (i ? 6 : -1) * s * dir, ey = ky - 2 * s;
    if (fmodf(t + 0.7f, 3.6f) < 0.13f) g.bogen(ex, ey + 0.5f * s, 1.8f * s, 3.5f, 5.9f, 1 * s, OUT);
    else { g.kreis(ex, ey, 2.4f * s, FARBE(0x9ACB4A)); g.ellipse(ex + 0.4f * s * dir, ey, 0.7f * s, 2 * s, SCHWARZ); }
  }
  if (mund > 0.05f) g.ellipse(kx + 5 * s * dir, ky + 7 * s, 2 * s, (0.8f + mund * 2.2f) * s, FARBE(0x8A2A2A));
  if (hut) {
    float hx = kx - 1 * s * dir, hy = ky - 12 * s;
    g.kontur(1.1f * s, OUT);
    g.ellipse(hx, hy, 15 * s, 4 * s, FARBE(0x3A2A4A));
    g.ellipse(hx, hy - 5 * s, 8 * s, 6 * s, FARBE(0x3A2A4A));
    g.konturAus();
    g.kurve(hx - 4 * s * dir, hy - 8 * s, hx - 16 * s * dir, hy - 20 * s, hx - 22 * s * dir, hy - 8 * s, 3 * s, FARBE(0xE8473D));
  }
}

static void rebhuhn(Gfx &g, float x, float y, float s, float dir, float t, float pick = 0) {
  g.kontur(1 * s, OUT);
  g.ellipse(x, y - 7 * s, 10 * s, 7 * s, FARBE(0xA88A6A));
  g.kreis(x + 8 * s * dir, y - 12 * s - pick * -6 * s, 4.5f * s, FARBE(0xC0703A));
  g.konturAus();
  g.ellipse(x - 2 * s * dir, y - 7 * s, 5 * s, 3 * s, FARBE(0x8E6E4E));
  float sn[] = {x + 12 * s * dir, y - 13 * s + pick * 6 * s, x + 15 * s * dir, y - 11 * s + pick * 6 * s, x + 12 * s * dir, y - 10 * s + pick * 6 * s};
  g.polygon(sn, 3, FARBE(0x5A4A3A));
  g.kreis(x + 9 * s * dir, y - 13 * s + pick * 6 * s, 0.9f * s, SCHWARZ);
}

static void sack(Gfx &g, float x, float y, float s, bool zu) {
  g.kontur(1.2f, OUT);
  g.ellipse(x, y - 12 * s, 14 * s, 12 * s, FARBE(0xC9A46A));
  g.konturAus();
  if (!zu) g.ellipse(x + 8 * s, y - 20 * s, 6 * s, 4 * s, FARBE(0x6E4A2A));
  else g.linie(x + 8 * s, y - 22 * s, x + 14 * s, y - 28 * s, 1.4f, FARBE(0x6E4A2A));
}

static void kutsche(Gfx &g, float x, float y, float t, bool leute = true) {
  einhorn(g, x + 80, y, 0.8f, fmodf(t * 1.6f, 1), 1, 0, 0, true);
  g.linie(x + 30, y - 34, x + 62, y - 34, 2, OUT);
  for (int i = 0; i < 2; i++) {
    float rx = x - 26 + i * 52;
    g.kontur(1.2f, OUT);
    g.kreis(rx, y - 12, 12, HOLZ);
    g.konturAus();
    g.kreis(rx, y - 12, 8, FARBE(0xF0DCC0));
    for (int k = 0; k < 4; k++) { float a = t * 6 + k * 0.785f; g.linie(rx - cosf(a) * 8, y - 12 - sinf(a) * 8, rx + cosf(a) * 8, y - 12 + sinf(a) * 8, 1.4f, HOLZ_D); }
  }
  g.kontur(1.6f, OUT);
  g.rundRechteck(x - 40, y - 66, 80, 46, 10, FARBE(0xE8B020));
  g.konturAus();
  g.rundRechteck(x - 30, y - 60, 26, 24, 4, FARBE(0x9FD8FF));
  g.rundRechteck(x + 4, y - 60, 26, 24, 4, FARBE(0x9FD8FF));
  float kr[] = {x - 8, y - 66, x - 8, y - 76, x - 3, y - 70, x, y - 78, x + 3, y - 70, x + 8, y - 76, x + 8, y - 66};
  g.polygon(kr, 7, GOLD);
  if (leute) {
    g.kreis(x - 17, y - 48, 7, HAUT);
    g.ellipse(x - 17, y - 54, 7, 4, FARBE(0xF2C14E));
    g.kreis(x + 17, y - 48, 7, HAUT);
    g.ellipse(x + 17, y - 52, 6, 5, WEISS);  // der Koenig mit Bart
  }
}

static void loewe(Gfx &g, float x, float y, float s, float dir, float t, float bruell) {
  auto PX = [&](float d) { return x + d * s * dir; };
  auto PY = [&](float d) { return y + d * s; };
  g.kontur(1.4f * s, OUT);
  for (int i = 0; i < 4; i++) g.linie(PX(i < 2 ? -22 + i * 9 : 12 + (i - 2) * 9), PY(-26), PX(i < 2 ? -22 + i * 9 : 12 + (i - 2) * 9), PY(-3), 7 * s, FARBE(0xE0A23C));
  g.kurve(PX(-28), PY(-34), PX(-44), PY(-30), PX(-46), PY(-46), 3 * s, FARBE(0xE0A23C));
  g.ellipse(PX(0), PY(-34), 30 * s, 14 * s, FARBE(0xE0A23C));
  for (int i = 0; i < 10; i++) { float a = i * 0.628f + sinf(t * 3) * 0.05f; g.kreis(PX(24) + cosf(a) * 18 * s, PY(-50) + sinf(a) * 18 * s, 8 * s, FARBE(0xA8602A)); }
  g.kreis(PX(26), PY(-50), 15 * s, FARBE(0xE8B04A));
  g.konturAus();
  g.ellipse(PX(34), PY(-44), 8 * s, 6 * s, FARBE(0xFBE0B8));
  g.kreis(PX(37), PY(-49), 2 * s, OUT);
  g.ellipse(PX(34), PY(-40), 6 * s, (1 + bruell * 6) * s, FARBE(0x8A2A2A));
  for (int i = 0; i < 2; i++) { g.kreis(PX(i ? 32 : 22), PY(-56), 2.4f * s, SCHWARZ); g.linie(PX(i ? 29 : 19), PY(-61), PX(i ? 35 : 25), PY(-59 - bruell * 2), 1.4f * s, OUT); }
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

static void muehle(Gfx &g, float x, float y, float t) {
  g.kontur(1.8f, OUT);
  float m[] = {x - 30, y, x + 30, y, x + 20, y - 90, x - 20, y - 90};
  g.polygon(m, 4, FARBE(0xF0E4D0));
  float d[] = {x - 26, y - 88, x, y - 116, x + 26, y - 88};
  g.polygon(d, 3, FARBE(0xA8452E));
  g.konturAus();
  g.rundRechteck(x - 9, y - 30, 18, 30, 8, HOLZ_D);
  for (int i = 0; i < 4; i++) {
    float a = t * 0.8f + i * 1.5708f;
    float ex = x + cosf(a) * 56, ey = y - 92 + sinf(a) * 56;
    g.linie(x, y - 92, ex, ey, 3, HOLZ_D);
    float px = -sinf(a) * 9, py = cosf(a) * 9;
    float fl[] = {x + cosf(a) * 14, y - 92 + sinf(a) * 14, ex, ey, ex + px, ey + py, x + cosf(a) * 14 + px, y - 92 + sinf(a) * 14 + py};
    g.polygon(fl, 4, FARBE(0xF6F2EA), 230);
  }
  g.kreis(x, y - 92, 4, OUT);
}

static void feld(Gfx &g, float t, float scroll, bool korn) {
  himmel(g, 0, t, 180);
  sonne(g, 250, 30, 11, t);
  wolke(g, fmodf(80 - scroll * 0.2f + 600, 400) - 60, 44, 0.8f);
  huegel(g, scroll * 0.3f, 0.03f, 180, 20, 150, GRAS_H);
  boden(g, 200);
  if (korn) {
    g.rechteck(0, 160, 284, 40, FARBE(0xE8C860));
    for (int i = 0; i < 30; i++) { float x = fmodf(i * 11 - scroll + 3300, 330) - 20; g.linie(x, 160, x + sinf(t * 2 + i) * 2, 148, 1.6f, FARBE(0xC0A040)); g.ellipse(x + sinf(t * 2 + i) * 2, 146, 2, 4, FARBE(0xD8B040)); }
  }
}

// =====================================================================
// Szenen
// =====================================================================
// 1) Das Erbe: Muehle, Esel - und fuer den Juengsten nur der Kater
static void szeneErbe(Gfx &g, float u) {
  feld(g, u, 0, false);
  muehle(g, 60, 200, u);
  Pose p = posis(HANS, u);
  p.mund = u > 8 && u < 14 ? 0.5f : 0;
  p.armV = u > 8 && u < 14 ? 1.8f : 0.3f;
  schneiderlein(g, 160, 214, 0.95f, 1, p);
  bool stiefel = u > 20;
  kater(g, 214, 214, 0.9f, -1, u, -1, u > 14.5f && u < 20 ? 0.6f : 0, u > 14.5f ? 1.6f : 0.4f, stiefel, stiefel, 0);
  if (u > 20) puff(g, 214, 186, seg(u, 20, 21.2f), FARBE(0xE8473D));
  g.kameraAus();
  vers(g, {"Ein Müller hinterließ seinen", "drei Söhnen die Mühle, den Esel", "und den Kater."}, aufzu(u, 0.4f, 7.6f));
  blase(g, 120, 60, {"Was soll ich nur mit", "einem Kater anfangen?"}, aufzu(u, 8, 14), 160, 130);
  blase(g, 200, 60, {"Lass mir ein Paar Stiefel", "machen - dann helfe ich dir!"}, aufzu(u, 14.5f, 20), 214, 140);
  blende(g, u, 24, 214, 170);
}

// 2) Der Kater faengt Rebhuehner mit dem Sack
static void szeneJagd(Gfx &g, float u) {
  feld(g, u, 0, false);
  for (int i = 0; i < 3; i++) laubbaum(g, 30 + i * 100, 200, 0.6f);
  bool zu = u > 13;
  sack(g, 150, 214, 1.1f, zu);
  for (int i = 0; i < 3; i++) {
    float ein = seg(u, 6 + i * 1.8f, 7.2f + i * 1.8f);
    if (ein >= 1) continue;
    float rx = lerp(40 + i * 30, 160, ein);
    rebhuhn(g, rx, 214 - huepf(ein) * 10, 0.9f, 1, u, ein > 0 ? 0 : huepf(fmodf(u * 1.2f + i * 0.3f, 1)));
  }
  float kx = u < 13 ? 240 : lerp(240, 172, seg(u, 13, 14));
  kater(g, kx, 214, 0.9f, -1, u, u > 13 && u < 14 ? fmodf(u * 2, 1) : -1, u > 14 ? 0.5f : 0, u > 13 ? 2.0f : 0.6f, true, true);
  if (u > 14 && u < 16) wolkePuff(g, 150, 196, seg(u, 14, 16));
  g.kameraAus();
  vers(g, {"Der Kater zog die Stiefel an,", "füllte Korn in einen Sack und", "legte sich auf die Lauer."}, aufzu(u, 0.4f, 6));
  vers(g, {"Die Rebhühner liefen hinein -", "zack, zog er den Sack zu!"}, aufzu(u, 14, 23.6f));
  blende(g, u, 24, 160, 170);
}

// 3) Beim Koenig: "Mein Herr, der Graf, schickt euch das."
static void szeneKoenig(Gfx &g, float u) {
  g.verlauf(0, 190, FARBE(0xF6E2B4), FARBE(0xE8C483));
  for (int i = 0; i < 4; i++) g.rechteck(20 + i * 82 - 10, 30, 20, 160, FARBE(0xFFF3D6));
  g.rechteck(0, 190, 284, 50, FARBE(0xC0392B));
  // Thron
  g.kontur(1.6f, OUT);
  g.rundRechteck(30, 100, 60, 100, 10, FARBE(0xE8B020));
  g.konturAus();
  g.rundRechteck(38, 108, 44, 60, 8, FARBE(0xC0392B));
  koenig(g, 60, 206, 1, 1, u > 14 ? 0.8f : 0);
  float ver = u > 4 && u < 7 ? sanft(seg(u, 4, 5)) * (1 - seg(u, 6, 7)) : 0;
  kater(g, 170, 214, 0.95f, -1, u, -1, u > 7 && u < 13 ? 0.6f : 0, u > 7 ? 1.6f : 0.6f, true, u < 4 || u > 7, ver);
  if (u > 4 && u < 7) {  // Hut in der Hand
    g.ellipse(196, 170, 10, 3, FARBE(0x3A2A4A));
  }
  sack(g, 124, 214, 1, u < 9);
  if (u > 9) for (int i = 0; i < 3; i++) rebhuhn(g, 106 + i * 16, 206 - (i & 1) * 6, 0.8f, -1, u);
  if (u > 15) for (int i = 0; i < 6; i++) g.ellipse(160 + (i % 3) * 8, 212 - (i / 3) * 4, 4, 2.5f, GOLD);
  g.kameraAus();
  vers(g, {"Der König aß Rebhühner für", "sein Leben gern."}, aufzu(u, 0.4f, 4));
  blase(g, 190, 60, {"Mein Herr, der Graf,", "schickt Euch das!"}, aufzu(u, 7, 13), 176, 130);
  blase(g, 80, 50, {"Bring ihm Gold dafür!"}, aufzu(u, 14, 19), 66, 120);
  blende(g, u, 24, 140, 160);
}

// 4) Am See: "Mein Herr Graf ertrinkt!" - neue Kleider und eine Fahrt in der Kutsche
static void szeneSee(Gfx &g, float u) {
  if (u < 18) {
    himmel(g, 0, u, 160);
    sonne(g, 250, 30, 11, u);
    boden(g, 160);
    g.ellipse(100, 200, 110, 34, FARBE(0x5BA8E0));
    g.ellipse(90, 196, 80, 20, FARBE(0x7EC0EE));
    // Hans im Wasser
    g.kreis(84, 196 + sinf(u * 2) * 2, 9, HAUT);
    g.ellipse(80, 189 + sinf(u * 2) * 2, 9, 5, HANS.haar);
    g.bogen(88, 200 + sinf(u * 2) * 2, 3, 0.4f, 2.7f, 1, OUT);
    if (u > 10) { g.linie(72, 192, 64, 178 + sinf(u * 8) * 3, 3, HAUT); }
    g.ellipse(84, 205, 18, 3, WEISS, 150);
    // der Kater versteckt die Kleider hinter dem Stein
    steinbrocken(g, 236, 196, 18);
    float k = seg(u, 4, 8);
    g.rundRechteck(lerp(200, 236, k) - 8, 184, 16, 10, 3, HANS.weste);
    kater(g, u < 10 ? lerp(180, 214, k) : 190, 196, 0.85f, u < 10 ? 1 : -1, u, u > 4 && u < 8 ? fmodf(u * 1.5f, 1) : -1, u > 10 ? 0.9f : 0, u > 10 ? 2.8f : 1.4f);
    if (u > 11) {
      kutsche(g, lerp(380, 260, seg(u, 11, 15)), 160, u);
    }
    g.kameraAus();
    vers(g, {"Der Kater sagte: Bade im See!", "Und er versteckte die Kleider."}, aufzu(u, 0.4f, 9.6f));
    blase(g, 150, 60, {"Hilfe! Hilfe!", "Mein Herr Graf ertrinkt!"}, aufzu(u, 10, 17.6f), 196, 140);
    return;
  }
  float v = u - 18;
  feld(g, u, v * 60, false);
  kutsche(g, 120, 214, u);
  // der Graf in neuen Kleidern sitzt nun mit drin - und der Kater rennt voraus
  g.kreis(137, 166, 7, HAUT);
  g.ellipse(137, 160, 7, 3, GRAF.hut);
  kater(g, 250 + sinf(v) * 6, 214, 0.75f, 1, u, fmodf(u * 2.4f, 1), 0, 1.6f);
  g.kameraAus();
  vers(g, {"Der König gab dem 'Grafen'", "prächtige Kleider - und die", "Prinzessin lächelte ihn an."}, aufzu(v, 0.4f, 13));
  blende(g, u, 32, 140, 150);
}

// 5) Auf dem Feld: "Sagt, das gehoert dem Grafen!"
static void szeneFeld(Gfx &g, float u) {
  feld(g, u, 0, true);
  for (int i = 0; i < 2; i++) {
    Pose p = posis(i ? B1 : B0, u, i);
    p.armV = u > 4 && u < 9 ? 2.2f : 1.2f + sinf(u * 2 + i) * 0.4f;
    p.mund = u > 14 && u < 20 ? 0.6f : 0;
    schneiderlein(g, 140 + i * 50, 214, 0.85f, 1, p);
    g.linie(156 + i * 50, 170, 170 + i * 50, 140, 2, HOLZ_D);  // Sense
    g.bogen(170 + i * 50, 152, 12, 4.2f, 5.6f, 2, STEIN);
  }
  float k = seg(u, 0, 3);
  kater(g, lerp(330, 256, k), 214, 0.9f, -1, u, k < 1 ? fmodf(u * 2.4f, 1) : -1, u > 4 && u < 9 ? 0.7f : 0, u > 4 && u < 9 ? 2.6f : 0.6f);
  if (u > 10) kutsche(g, lerp(-160, -14, seg(u, 10, 13)), 206, u);
  g.kameraAus();
  blase(g, 190, 60, {"Wenn der König fragt, sagt:", "Das gehört dem Grafen!"}, aufzu(u, 4, 9.6f), 250, 140);
  blase(g, 90, 80, {"Wem gehört das Feld?"}, aufzu(u, 13, 15.6f), 6, 150);
  blase(g, 200, 80, {"Dem Grafen!"}, aufzu(u, 16, 20), 170, 140);
  blende(g, u, 24, 160, 170);
}

// 6) Im Schloss des Zauberers: Loewe - und Maus
static void szeneZauberer(Gfx &g, float u) {
  g.verlauf(0, 190, FARBE(0x5A4A7A), FARBE(0x3A2E5A));
  for (int i = 0; i < 3; i++) { g.rundRechteck(40 + i * 90, 40, 26, 50, 13, FARBE(0x8EA6E8)); g.rechteck(52 + i * 90, 40, 2, 50, FARBE(0x5A4A7A)); }
  g.rechteck(0, 190, 284, 50, FARBE(0x6E5A8E));
  int form = u < 10 ? 0 : u < 21 ? 1 : 2;  // Zauberer, Loewe, Maus
  float zx = 90;
  if (form == 0) {
    Pose z = posis(ZAUBERER, u);
    z.armV = u > 6 ? 2.8f : 1.2f;
    z.mund = u > 3 && u < 6 ? 0.5f : 0.2f;
    z.boese = 0.6f;
    schneiderlein(g, zx, 214, 1.05f, 1, z);
  } else if (form == 1) {
    loewe(g, zx, 214, 1.05f, 1, u, u > 11 && u < 15 ? 0.9f : 0.2f);
  } else if (u < 25) {
    maus(g, lerp(zx, 196, seg(u, 22, 25)), 214, 1.2f, 1, fmodf(u * 3, 1));
  }
  puff(g, zx, 170, seg(u, 10, 11.2f), FARBE(0xB28DFF));
  puff(g, zx, 190, seg(u, 21, 22.2f), FARBE(0xB28DFF));
  // der Kater: erschrickt beim Loewen (springt hoch), faengt dann die Maus
  float ky = 214, kxk = 200;
  if (form == 1) ky = lerp(214, 120, sanft(seg(u, 11, 12))) ;
  if (form == 2) { ky = lerp(120, 214, hinein(seg(u, 21, 22))); kxk = u > 24.6f ? 200 : 200; }
  if (form == 1 || (form == 2 && u < 22)) {  // haengt am Balken oben
    g.rechteck(150, 96, 120, 8, HOLZ_D);
  }
  bool satt = u > 25;
  kater(g, kxk, ky, 0.95f, -1, u, -1, u > 4 && u < 9 ? 0.6f : form == 1 ? 0.8f : satt ? 0.3f : 0, form == 1 ? 3.0f : satt ? 2.6f + sinf(u * 6) * 0.2f : 1.4f);
  if (u > 24.8f && u < 26.5f) wolkePuff(g, 196, 200, seg(u, 24.8f, 26.5f));
  g.kameraAus();
  blase(g, 190, 50, {"Kannst du dich wirklich", "in jedes Tier verwandeln?"}, aufzu(u, 0.4f, 4), 200, 130);
  blase(g, 80, 50, {"Gewiss! Schau her!"}, aufzu(u, 4.4f, 9), 96, 130);
  blase(g, 200, 50, {"Und auch in etwas ganz", "Kleines? In eine Maus?"}, aufzu(u, 14.6f, 20.6f), 210, 100);
  vers(g, {"Da sprang der Kater - und die", "Maus war fort. Mitsamt dem Zauberer."}, aufzu(u, 25, 33.6f));
  blende(g, u, 34, 200, 170);
}

// 7) Der Koenig kommt an, Hochzeit
static void szeneHochzeit(Gfx &g, float u) {
  himmel(g, 0, u, 200);
  sonne(g, 250, 30, 11, u);
  boden(g, 200);
  schloss(g, 142, 200, u, 0);
  Pose p = posis(GRAF, u);
  p.armV = 1.2f;
  p.mund = 0.4f;
  schneiderlein(g, 116, 220, 0.85f, 1, p);
  FrauPose f = fposis(PRINZESSIN, u);
  f.armV = 1.2f;
  f.mund = 0.3f;
  frau(g, 162, 220, 0.8f, -1, f);
  koenig(g, 62, 220, 0.85f, 1, sinf(u * 2) * 0.5f + 0.5f);
  kater(g, 226, 220, 0.85f, -1, u, -1, 0.4f, 2.4f + sinf(u * 3) * 0.3f, true, true, u > 3 && u < 6 ? 0.7f : 0);
  herzen(g, 140, 170, u, 6, 50);
  if (u > 6) feuerwerk(g, 60, 60, fmodf(u * 0.5f, 1), FARBE(0xFF6B8A));
  if (u > 8) feuerwerk(g, 220, 50, fmodf(u * 0.5f + 0.5f, 1), GOLD);
  g.kameraAus();
  blase(g, 200, 70, {"Willkommen im Schloss", "meines Herrn, des Grafen!"}, aufzu(u, 0.4f, 5.6f), 226, 150);
  vers(g, {"Der Graf heiratete die Prinzessin,", "und der Kater wurde", "erster Minister des Königs."}, aufzu(u, 6, 18));
  endeBild(g, u, 22);
}

static void bild(Gfx &g, float t) {
  if (t < P2) szeneErbe(g, t - P1);
  else if (t < P3) szeneJagd(g, t - P2);
  else if (t < P4) szeneKoenig(g, t - P3);
  else if (t < P5) szeneSee(g, t - P4);
  else if (t < P6) szeneFeld(g, t - P5);
  else if (t < P7) szeneZauberer(g, t - P6);
  else szeneHochzeit(g, t - P7);
}

// =====================================================================
// Musik
// =====================================================================
// Kater-Marsch (D-Dur, keck)
static const Ton3 MEL_KATER[] = {
  {62, .5, 38}, {66, .25, 0}, {69, .25, 0}, {74, .5, 0}, {69, .5, 0}, {71, .5, 43}, {69, .5, 0}, {66, 1, 0},
  {67, .5, 43}, {69, .5, 0}, {71, .5, 0}, {73, .5, 0}, {74, .5, 45}, {76, .5, 0}, {73, 1, 0},
  {74, .5, 38}, {73, .5, 0}, {71, .5, 0}, {69, .5, 0}, {67, .5, 45}, {66, .5, 0}, {64, .5, 0}, {66, .5, 0}, {62, 2, 38}};
static void miau(Synth &s, float x) { s.ton(x, 700, 0.18f, 0.03f, 0.35f, DREIECK, GERAEUSCH, 980); s.ton(x + 0.2f, 980, 0.12f, 0.01f, 0.3f, DREIECK, GERAEUSCH, 620); }
static void ton(Synth &s, float t0, float t1) {
  float a = P1;
  spieleMelodie(s, M(MEL_WANDER), a + 0.5f, a + 14, 0.36f, t0, t1, 0.5f);
  if (bei(a + 14.6f, t0, t1)) miau(s, a + 14.6f);
  if (bei(a + 20, t0, t1)) for (int k = 0; k < 6; k++) s.note(a + 20 + k * 0.07f, 79 + k * 2, 0.6f);
  spieleMelodie(s, M(MEL_KATER), a + 20.4f, a + 24, 0.24f, t0, t1, 0.8f);
  a = P2;
  spieleMelodie(s, M(MEL_KATER), a + 0.4f, a + 6, 0.24f, t0, t1, 0.7f);
  for (int i = 0; i < 3; i++) if (bei(a + 6 + i * 1.8f, t0, t1)) { s.note(a + 6 + i * 1.8f, 84, 0.4f); s.note(a + 6.15f + i * 1.8f, 86, 0.35f); }  // pick pick
  if (bei(a + 14, t0, t1)) { s.rauschen(a + 14, 0.3f, BANDPASS, 1200, 0.25f, 1, 0.002f, 400); s.note(a + 14.2f, 72, 0.6f); }
  spieleMelodie(s, M(MEL_KATER), a + 15, a + 23.5f, 0.24f, t0, t1, 0.75f);
  a = P3;
  spieleMelodie(s, M(MEL_KEGEL), a + 0.4f, a + 23.5f, 0.32f, t0, t1, 0.5f);
  for (int i = 0; i < 6; i++) if (bei(a + 15 + i * 0.25f, t0, t1)) s.note(a + 15 + i * 0.25f, 86 + (i % 3) * 3, 0.35f);
  a = P4;
  spieleMelodie(s, M(MEL_WIEGE), a + 0.4f, a + 9.6f, 0.42f, t0, t1, 0.45f);
  if (bei(a + 10, t0, t1)) { miau(s, a + 10); miau(s, a + 11); }
  spieleMelodie(s, M(MEL_JAGD), a + 11, a + 17.5f, 0.16f, t0, t1, 0.6f);
  spieleMelodie(s, M(MEL_KATER), a + 18.4f, a + 31.5f, 0.24f, t0, t1, 0.75f);
  a = P5;
  spieleMelodie(s, M(MEL_KATER), a + 0.2f, a + 23.5f, 0.24f, t0, t1, 0.6f);
  a = P6;
  spieleMelodie(s, M(MEL_SPUK), a + 0.4f, a + 33.5f, 0.42f, t0, t1, 0.45f);
  if (bei(a + 10, t0, t1)) s.rauschen(a + 10, 0.8f, BANDPASS, 900, 0.25f, 1, 0.01f, 3000);
  if (bei(a + 11.2f, t0, t1)) { s.ton(a + 11.2f, 120, 0.4f, 0.05f, 1.4f, SAEGE, GERAEUSCH, 70); s.rauschen(a + 11.2f, 1.2f, TIEFPASS, 500, 0.25f); }  // Bruellen
  if (bei(a + 11.4f, t0, t1)) miau(s, a + 11.4f);
  if (bei(a + 21, t0, t1)) s.rauschen(a + 21, 0.8f, BANDPASS, 900, 0.25f, 1, 0.01f, 3000);
  for (int i = 0; i < 4; i++) if (bei(a + 22.2f + i * 0.2f, t0, t1)) s.note(a + 22.2f + i * 0.2f, 96, 0.25f);  // piep
  if (bei(a + 24.8f, t0, t1)) s.rauschen(a + 24.8f, 0.25f, BANDPASS, 1400, 0.3f);
  a = P7;
  spieleMelodie(s, M(MEL_KATER), a + 0.4f, a + 21.5f, 0.26f, t0, t1, 0.8f);
  endeKlang(s, a + 22.2f, t0, t1);
}

// =====================================================================
// Startbild, Vorschau, Erkennungszeichen
// =====================================================================
static void titelSzene(Gfx &g, float t) {
  himmel(g, 0, t, 200);
  sonne(g, 250, 30, 11, t);
  boden(g, 200);
  schloss(g, 70, 200, t, 0);
  kater(g, 190, 222, 1.4f, -1, t, -1, 0.3f, 2.2f + sinf(t * 2) * 0.3f);
}
static void titelbild(Gfx &g, float t) { titelSzene(g, t); titelRahmen(g, t, "Der gestiefelte Kater", 210, SCHRIFT_M); }
static void vorschau(Gfx &g, float t) { titelSzene(g, t); }
static void symbol(Gfx &g, float x, float y, float t) { kater(g, x, y, 1.15f, 1, t, -1, 0.2f, 2.2f + sinf(t * 2) * 0.3f); }

const Maerchen KATER = {"Der gestiefelte Kater", PDAUER, FARBE(0xE89A3A), bild, ton, titelbild, vorschau, symbol,
                        LIED(MEL_KATER, 0.24f), nullptr};

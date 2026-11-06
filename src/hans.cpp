// Hans im Glueck - ein Spieluhr-Film fuer die Maerchenbox, nach den Bruedern Grimm.
// Gold gegen Pferd, Pferd gegen Kuh, Kuh gegen Schwein, Schwein gegen Gans, Gans gegen Schleifstein -
// der Stein faellt in den Brunnen, und Hans ist der gluecklichste Mensch unter der Sonne.
#include "erzaehlen.h"

// ---------- Ablauf (Sekunden) ----------
static const float P1 = 0, P2 = 20, P3 = 48, P4 = 78, P5 = 100, P6 = 124, P7 = 148, PDAUER = 180;

static Aussehen hansLook() {
  Aussehen a; a.weste = FARBE(0xE07A2A); a.hemd = FARBE(0xFFF6E8); a.hose = FARBE(0x5A6E8E); a.haar = FARBE(0xE0A23C);
  a.hut = FARBE(0x3E7FDC); a.hutD = FARBE(0x2E5FAE); a.feder = GOLD;
  return a;
}
static Aussehen leuteLook(int i) {
  Aussehen a;
  const Farbe w[] = {FARBE(0x8E2A2A), FARBE(0x5A6E3A), FARBE(0xE8E4DC), FARBE(0x3E7FDC), FARBE(0x6E5A8E)};
  const Farbe h[] = {FARBE(0x2E2220), FARBE(0x8E5A3A), FARBE(0x9A9A9A), FARBE(0xC98A3A), FARBE(0x3A2A1A)};
  a.weste = w[i]; a.haar = h[i]; a.muetze = i == 0 ? 0 : 1; a.hut = FARBE(0x1E1A22); a.hutD = OUT; a.feder = WEISS;
  a.halstuch = i == 4 ? FARBE(0xE8473D) : 0;
  return a;
}
static const Aussehen HANS = hansLook(), REITER = leuteLook(0), BAUER = leuteLook(1), MEISTER = leuteLook(2), BURSCHE = leuteLook(3), SCHLEIFER = leuteLook(4);
static FrauLook mutterLook() { FrauLook l; l.kleid = FARBE(0x8E6EA8); l.schuerze = WEISS; l.haar = FARBE(0xC8C8C8); l.frisur = 2; return l; }
static const FrauLook MUTTER = mutterLook();

// ---------- Dinge und Tiere ----------
static void goldklumpen(Gfx &g, float x, float y, float s, float t) {
  g.kontur(1.4f, FARBE(0x9A6A1E));
  g.ellipse(x, y, 14 * s, 11 * s, FARBE(0xF2C230));
  g.konturAus();
  g.kreis(x - 4 * s, y - 3 * s, 4 * s, FARBE(0xFFE070));
  g.stern(x + 5 * s, y - 5 * s, 5 * s, 1.4f * s, 4, t * 2, WEISS, (uint8_t)(150 + 100 * sinf(t * 3)));
}

static void kuh(Gfx &g, float x, float y, float s, float dir, float t, float lauf = -1, float tritt = 0, float mund = 0) {
  auto PX = [&](float d) { return x + d * s * dir; };
  auto PY = [&](float d) { return y + d * s; };
  const Farbe fell = WEISS, fleck = FARBE(0x3A3436), rosa = FARBE(0xF2A6B0);
  g.kontur(1.4f * s, OUT);
  for (int i = 0; i < 4; i++) {
    float bx = i < 2 ? -22 + i * 9 : 14 + (i - 2) * 9;
    float sw = lauf >= 0 ? sinf(lauf * 6.2832f + i * 1.6f) * 6 : 0;
    float hoch = i == 0 ? tritt : 0;
    g.linie(PX(bx), PY(-26), PX(bx + sw - hoch * 18), PY(-4 - hoch * 12), 7 * s, fell);
    g.ellipse(PX(bx + sw - hoch * 18), PY(-3 - hoch * 12), 4 * s, 3 * s, fleck);
  }
  g.kurve(PX(-30), PY(-40), PX(-40), PY(-34), PX(-38), PY(-16), 2.4f * s, fell);
  g.ellipse(PX(0), PY(-36), 32 * s, 16 * s, fell);
  g.konturAus();
  g.ellipse(PX(-12), PY(-40), 9 * s, 7 * s, fleck);
  g.ellipse(PX(10), PY(-32), 7 * s, 6 * s, fleck);
  g.ellipse(PX(0), PY(-20), 7 * s, 5 * s, rosa);  // Euter
  float hx = PX(32), hy = PY(-44);
  g.kontur(1.3f * s, OUT);
  g.ellipse(hx, hy, 12 * s, 11 * s, fell);
  g.konturAus();
  g.ellipse(hx + 6 * s * dir, hy + 6 * s, 9 * s, 6 * s, rosa);
  g.kreis(hx + 4 * s * dir, hy + 6 * s, 1.2f * s, OUT);
  g.kreis(hx + 9 * s * dir, hy + 6 * s, 1.2f * s, OUT);
  g.kreis(hx + 2 * s * dir, hy - 3 * s, 1.8f * s, SCHWARZ);
  g.linie(hx - 6 * s * dir, hy - 9 * s, hx - 10 * s * dir, hy - 16 * s, 2.4f * s, FARBE(0xE8DCC0));
  g.linie(hx + 4 * s * dir, hy - 10 * s, hx + 6 * s * dir, hy - 17 * s, 2.4f * s, FARBE(0xE8DCC0));
  g.ellipse(hx - 10 * s * dir, hy - 4 * s, 5 * s, 2.5f * s, fell);
  if (mund > 0.05f) g.ellipse(hx + 7 * s * dir, hy + 10 * s, 3 * s, mund * 3 * s, FARBE(0x8A2A2A));
  g.kreis(PX(32), PY(-30), 3 * s, GOLD);  // Glocke
}

static void schwein(Gfx &g, float x, float y, float s, float dir, float t, float lauf = -1) {
  auto PX = [&](float d) { return x + d * s * dir; };
  auto PY = [&](float d) { return y + d * s; };
  const Farbe rosa = FARBE(0xF6B0B8), dunkel = FARBE(0xE08A98);
  g.bogen(PX(-22), PY(-22), 3.5f * s, 0, 5.5f, 1.4f * s, dunkel);
  g.kontur(1.2f * s, OUT);
  for (int i = 0; i < 4; i++) {
    float bx = i < 2 ? -14 + i * 7 : 10 + (i - 2) * 7;
    float sw = lauf >= 0 ? sinf(lauf * 6.2832f + i * 1.6f) * 4 : 0;
    g.linie(PX(bx), PY(-12), PX(bx + sw), PY(-2), 5 * s, dunkel);
  }
  g.ellipse(PX(0), PY(-18), 22 * s, 12 * s, rosa);
  g.kreis(PX(20), PY(-22), 10 * s, rosa);
  g.ellipse(PX(29), PY(-20), 4 * s, 5 * s, dunkel);
  g.konturAus();
  float o[] = {PX(14), PY(-30), PX(18), PY(-38), PX(22), PY(-30)};
  g.polygon(o, 3, dunkel);
  g.kreis(PX(29), PY(-22), 0.9f * s, OUT);
  g.kreis(PX(29), PY(-18), 0.9f * s, OUT);
  g.kreis(PX(22), PY(-25), 1.5f * s, SCHWARZ);
  g.bogen(PX(23), PY(-17), 3 * s, 0.4f, 2.7f, 1 * s, OUT);
}

static void gans(Gfx &g, float x, float y, float s, float dir, float t, float lauf = -1) {
  auto PX = [&](float d) { return x + d * s * dir; };
  auto PY = [&](float d) { return y + d * s; };
  float w = lauf >= 0 ? fabsf(sinf(lauf * 6.28f)) * 1.5f : 0;
  for (int k = 0; k < 2; k++) g.linie(PX(-2 + k * 5), PY(-6), PX(-2 + k * 5 + (lauf >= 0 ? sinf(lauf * 6.28f + k * 3) * 3 : 0)), PY(0), 2 * s, FARBE(0xF28A2A));
  g.kontur(1.1f * s, OUT);
  g.ellipse(PX(0), PY(-12 - w), 14 * s, 8 * s, WEISS);
  g.konturAus();
  float sw[] = {PX(-12), PY(-14 - w), PX(-19), PY(-20 - w), PX(-14), PY(-9 - w)};
  g.polygon(sw, 3, WEISS);
  g.linie(PX(9), PY(-15 - w), PX(12), PY(-30 - w), 4.5f * s, WEISS);
  g.kontur(1, OUT);
  g.kreis(PX(12), PY(-32 - w), 4.5f * s, WEISS);
  g.konturAus();
  float sn[] = {PX(15), PY(-33 - w), PX(22), PY(-31 - w), PX(15), PY(-29 - w)};
  g.polygon(sn, 3, FARBE(0xF28A2A));
  g.kreis(PX(13), PY(-34 - w), 0.9f * s, SCHWARZ);
}

static void stein(Gfx &g, float x, float y, float s) {
  g.kontur(1.4f, OUT);
  g.ellipse(x, y, 14 * s, 10 * s, STEIN);
  g.konturAus();
  g.ellipse(x - 3 * s, y - 3 * s, 6 * s, 3 * s, FARBE(0xC0C4CE));
  g.kreis(x, y, 2.4f * s, STEIN_D);
}

// Hans traegt etwas; was: 0 Gold (auf der Schulter), 4 Gans (unterm Arm), 5 Steine
static void hans(Gfx &g, float x, float y, float s, float dir, float t, float lauf, float mund, int was, float schweiss = 0,
                 bool zu = false) {
  Pose p = posis(HANS, t);
  p.lauf = lauf;
  p.mund = mund;
  p.zu = zu || p.zu;
  p.armV = was == 0 ? 2.8f : was == 4 ? 1.2f : was == 5 ? 1.6f : 0.4f;
  p.armH = was == 0 ? 2.8f : 0.2f;
  float hop = lauf >= 0 ? fabsf(sinf(lauf * 6.28f)) * 1.5f : 0;
  schneiderlein(g, x, y - hop, s, dir, p);
  if (was == 0) goldklumpen(g, x - 2 * s * dir, y - 96 * s - hop, 1.4f * s, t);
  if (was == 4) gans(g, x + 16 * s * dir, y - 22 * s - hop, 0.75f * s, dir, t);
  if (was == 5) { stein(g, x + 14 * s * dir, y - 30 * s - hop, 0.9f * s); stein(g, x + 18 * s * dir, y - 46 * s - hop, 0.7f * s); }
  if (schweiss > 0) for (int i = 0; i < 2; i++) {
    float k = fmodf(t * 1.4f + i * 0.5f, 1);
    tropfen(g, x - (10 - i * 22) * s * dir, y - 70 * s + k * 14, 2.4f, FARBE(0x7EC0EE), (uint8_t)(255 * (1 - k) * schweiss));
  }
}

static void weg(Gfx &g, float t, float scroll, float sonneY = 30) {
  himmel(g, 0, t, 190);
  sonne(g, 248, sonneY, 11, t);
  wolke(g, fmodf(70 - scroll * 0.15f + 600, 400) - 60, 44, 0.8f);
  wolke(g, fmodf(260 - scroll * 0.15f + 600, 400) - 60, 64, 0.6f);
  huegel(g, scroll * 0.3f, 0.025f, 190, 26, 170, GRAS_H);
  for (int i = 0; i < 4; i++) laubbaum(g, fmodf(i * 120 - scroll * 0.7f + 960, 480) - 60, 196, 0.55f + (i & 1) * 0.15f);
  boden(g, 200);
  g.rechteck(0, 210, 284, 12, FARBE(0xD8C49A));
  blumen(g, scroll, 228, 6, 4);
}

// =====================================================================
// Szenen
// =====================================================================
// 1) Sieben Jahre gedient - der Lohn: ein Klumpen Gold, so gross wie sein Kopf
static void szeneLohn(Gfx &g, float u) {
  weg(g, u, 0);
  g.kontur(1.6f, OUT);
  g.rechteck(10, 130, 76, 82, FARBE(0xF0DCC0));
  float d[] = {2, 132, 48, 94, 94, 132};
  g.polygon(d, 3, FARBE(0xC0503A));
  g.konturAus();
  g.rundRechteck(36, 168, 22, 44, 8, HOLZ_D);
  Pose m = posis(MEISTER, u);
  m.armV = u > 6 && u < 10 ? 1.6f : 0.4f;
  m.mund = u > 6 && u < 10 ? 0.5f : 0;
  schneiderlein(g, 92, 222, 0.9f, 1, m);
  bool hat = u > 9;
  if (!hat) goldklumpen(g, 118, 168, 1.4f, u);
  hans(g, 160, 222, 0.95f, -1, u, -1, u > 2 && u < 6 ? 0.5f : hat ? 0.6f : 0, hat ? 0 : 9);
  g.kameraAus();
  blase(g, 190, 60, {"Herr, meine Zeit ist um.", "Ich will heim zu meiner Mutter."}, aufzu(u, 0.4f, 5.6f), 160, 130);
  vers(g, {"Für sieben Jahre treue Arbeit", "bekam Hans einen Klumpen Gold,", "so groß wie sein Kopf."}, aufzu(u, 6, 18));
  blende(g, u, 20, 160, 160);
}

// 2) Gold gegen Pferd - das Pferd wirft ihn ab
static void szenePferd(Gfx &g, float u) {
  if (u < 13) {
    weg(g, u, u * 8);
    float k = seg(u, 0, 4);
    einhorn(g, lerp(330, 210, k), 222, 0.9f, k < 1 ? fmodf(u * 1.6f, 1) : -1, -1, 0, 0, true);
    Pose r = posis(REITER, u);
    r.mund = u > 4.5f && u < 8 ? 0.5f : 0;
    bool getauscht = u > 9;
    if (!getauscht) schneiderlein(g, lerp(330, 210, k) + 4, 186, 0.75f, -1, r);
    else {
      r.armV = 2.8f; r.armH = 2.8f;
      schneiderlein(g, 160, 222, 0.85f, 1, r);
      goldklumpen(g, 160, 128, 1.2f, u);
    }
    hans(g, getauscht ? 214 : 110, getauscht ? 186 : 222, getauscht ? 0.75f : 0.95f, getauscht ? -1 : 1, u, -1,
         u > 1 && u < 4.4f ? 0.5f : getauscht ? 0.7f : 0, getauscht ? 9 : 0, getauscht ? 0 : 1);
    g.kameraAus();
    blase(g, 110, 50, {"Ach, wie schön ist das Reiten!"}, aufzu(u, 0.6f, 4.4f), 110, 110);
    blase(g, 200, 50, {"Ich gebe dir mein Pferd", "für den Klumpen Gold."}, aufzu(u, 4.6f, 8.6f), 214, 150);
    return;
  }
  float v = u - 13;
  weg(g, u, 104 + v * 60);
  // im Graben: Hans liegt kopfueber, das Pferd rennt weg
  bool geworfen = v > 4;
  float px = geworfen ? lerp(150, 330, seg(v, 4, 9)) : 150;
  einhorn(g, px, 222, 0.9f, fmodf(u * 2.2f, 1), 1, 0, geworfen ? 0 : v * 0.3f, true);
  if (!geworfen) hans(g, 150, 186 - fabsf(sinf(v * 8)) * 6, 0.75f, 1, u, -1, 0.9f, 9);
  else {
    float k = seg(v, 4, 5);
    float hx = lerp(150, 70, k), hy = lerp(186, 226, k) - huepf(k) * 40;
    Pose p = posis(HANS, u);
    p.mund = 0.9f; p.armV = 2.8f; p.armH = 2.8f; p.zu = true;
    schneiderlein(g, hx, hy, 0.85f, -1, p);
    if (v > 5 && v < 8) for (int i = 0; i < 4; i++) g.stern(70 + cosf(i * 1.57f + v * 3) * 18, 150 + sinf(i * 1.57f + v * 3) * 6, 3, 1.2f, 4, v * 6, GOLD);
  }
  g.kameraAus();
  vers(g, {"Hans wollte schneller reiten -", "da warf ihn das Pferd ab."}, aufzu(v, 0.4f, 9));
  blase(g, 110, 60, {"Das Reiten ist nichts für mich!"}, aufzu(v, 9.4f, 14.6f), 76, 150);
  blende(g, u, 28, 90, 170);
}

// 3) Pferd gegen Kuh - die Kuh tritt ihn
static void szeneKuh(Gfx &g, float u) {
  weg(g, u, 300);
  if (u < 14) {
    kuh(g, 180, 222, 0.9f, -1, u, -1, 0, u > 9 && u < 11 ? 0.5f : 0);
    Pose b = posis(BAUER, u);
    b.mund = u > 4.6f && u < 9 ? 0.5f : 0;
    schneiderlein(g, 250, 222, 0.85f, -1, b);
    einhorn(g, u < 10 ? 70 : lerp(70, 330, seg(u, 10, 14)), 222, 0.8f, u > 10 ? fmodf(u * 1.4f, 1) : -1, 1, 0, 0, true);
    hans(g, 116, 222, 0.9f, 1, u, -1, u > 0.4f && u < 4.4f ? 0.5f : 0, 9);
    g.kameraAus();
    blase(g, 110, 60, {"Eine Kuh! Dann hätte ich", "Milch, Butter und Käse!"}, aufzu(u, 0.4f, 4.4f), 116, 130);
    blase(g, 220, 50, {"Gern - Kuh gegen Pferd!"}, aufzu(u, 4.6f, 9), 250, 130);
    return;
  }
  float v = u - 14;
  float tr = v > 6 && v < 7.2f ? huepf(seg(v, 6, 7.2f)) : 0;
  kuh(g, 160, 222, 0.9f, 1, u, -1, tr);
  // Hans hockt hinter der Kuh und will melken
  float k = seg(v, 6.3f, 7.4f);
  Pose p = posis(HANS, u);
  p.armV = 1.6f;
  p.mund = v > 6.3f ? 0.9f : 0.3f;
  p.zu = v > 6.3f && v < 9;
  float hx = lerp(124, 40, k), hy = lerp(230, 230, k) - huepf(k) * 50;
  schneiderlein(g, hx, hy, 0.75f, 1, p);
  eimer(g, 140, 222, 0.6f, 0);
  if (v > 7 && v < 10) for (int i = 0; i < 4; i++) g.stern(40 + cosf(i * 1.57f + v * 3) * 16, 150 + sinf(i * 1.57f + v * 3) * 6, 3, 1.2f, 4, v * 6, GOLD);
  g.kameraAus();
  vers(g, {"Hans war durstig und wollte", "melken. Aber es kam kein Tropfen."}, aufzu(v, 0.4f, 6));
  vers(g, {"Dafür trat ihn die Kuh,", "dass er umfiel."}, aufzu(v, 7.4f, 13));
  blende(g, u, 30, 80, 170);
}

// 4) Kuh gegen Schwein
static void szeneSchwein(Gfx &g, float u) {
  weg(g, u, 520 + u * 10);
  Pose m = posis(REITER, u, 1);
  m.mund = u > 5 && u < 10 ? 0.5f : 0;
  bool getauscht = u > 11;
  float k = seg(u, 11, 17);
  schneiderlein(g, 230, 222, 0.85f, -1, m);
  if (!getauscht) {
    schwein(g, 190, 222, 0.9f, -1, u);
    kuh(g, 90, 222, 0.75f, 1, u);
  } else {
    kuh(g, lerp(250, 330, k), 222, 0.75f, 1, u, fmodf(u, 1));
    schwein(g, lerp(150, 150, k), 222, 0.9f, -1, u, fmodf(u * 1.4f, 1));
  }
  hans(g, getauscht ? 104 : 130, 222, 0.9f, 1, u, -1, u > 0.4f && u < 5 ? 0.5f : getauscht ? 0.6f : 0, 9);
  g.kameraAus();
  blase(g, 110, 60, {"So ein fettes Schwein!", "Das gäbe Würste!"}, aufzu(u, 0.4f, 5), 130, 130);
  blase(g, 210, 50, {"Ich tausch es dir", "gegen die Kuh."}, aufzu(u, 5.2f, 10), 230, 130);
  vers(g, {"Hans freute sich: Was hab ich", "doch für ein Glück!"}, aufzu(u, 12, 21.6f));
  blende(g, u, 22, 130, 170);
}

// 5) Schwein gegen Gans
static void szeneGans(Gfx &g, float u) {
  weg(g, u, 720 + u * 10);
  Pose b = posis(BURSCHE, u);
  b.mund = u > 0.4f && u < 10 ? 0.5f : 0;
  b.armV = u < 12 ? 1.2f : 0.4f;
  bool getauscht = u > 12;
  schneiderlein(g, 210, 222, 0.85f, -1, b);
  if (!getauscht) { gans(g, 196, 186, 0.75f, -1, u); schwein(g, 100, 222, 0.85f, 1, u); }
  else schwein(g, lerp(190, 330, seg(u, 13, 18)), 222, 0.85f, 1, u, fmodf(u * 1.4f, 1));
  hans(g, 136, 222, 0.9f, 1, u, -1, u > 10 && u < 12 ? 0.6f : getauscht ? 0.6f : 0, getauscht ? 4 : 9);
  g.kameraAus();
  blase(g, 190, 50, {"Im Dorf wurde ein Schwein", "gestohlen - vielleicht deins!"}, aufzu(u, 0.4f, 5.4f), 210, 130);
  blase(g, 190, 50, {"Nimm lieber meine Gans", "und gib mir das Schwein."}, aufzu(u, 5.6f, 10), 210, 130);
  vers(g, {"Eine schöne fette Gans -", "Hans war wieder glücklich."}, aufzu(u, 13, 22.6f));
  blende(g, u, 24, 136, 170);
}

// 6) Gans gegen Schleifstein
static void szeneSchleifer(Gfx &g, float u) {
  weg(g, u, 920);
  // der Scherenschleifer am Schleifrad, singend
  g.kontur(1.4f, OUT);
  g.rechteck(196, 180, 50, 8, HOLZ);
  g.konturAus();
  g.rechteck(200, 188, 5, 34, HOLZ_D);
  g.rechteck(238, 188, 5, 34, HOLZ_D);
  g.kontur(1.2f, OUT);
  g.kreis(221, 168, 16, STEIN);
  g.konturAus();
  for (int i = 0; i < 4; i++) { float a = u * 6 + i * 1.57f; g.linie(221, 168, 221 + cosf(a) * 14, 168 + sinf(a) * 14, 1.6f, STEIN_D); }
  if (u < 14) for (int i = 0; i < 4; i++) g.stern(206 + zufall(i + (int)(u * 8)) * 10, 158 + zufall(i * 7 + (int)(u * 8)) * 6, 2, 0.8f, 4, u, GOLD);
  Pose s = posis(SCHLEIFER, u);
  s.armV = 1.6f + sinf(u * 6) * 0.3f;
  s.mund = u < 4 || (u > 6 && u < 11) ? 0.5f + 0.3f * sinf(u * 8) : 0;
  schneiderlein(g, 260, 222, 0.85f, -1, s);
  bool getauscht = u > 12;
  if (getauscht) gans(g, 240, 222, 0.7f, 1, u);
  hans(g, 140, 222, 0.9f, 1, u, -1, u > 4 && u < 6 ? 0.6f : getauscht ? 0.6f : 0, getauscht ? 5 : 4);
  for (int i = 0; i < 3; i++) if (u < 4) notenzeichen(g, 250 + i * 10, 120 - fmodf(u * 20 + i * 12, 40), OUT, 200);
  g.kameraAus();
  vers(g, {"Ein Scherenschleifer sang:", "Ich schleife die Schere, ich", "drehe geschwind!"}, aufzu(u, 0.4f, 4));
  blase(g, 200, 50, {"Werd Schleifer wie ich -", "gib mir die Gans für den Stein!"}, aufzu(u, 6.4f, 11.6f), 260, 130);
  vers(g, {"Dazu gab er ihm noch einen", "Feldstein. Jetzt war es schwer."}, aufzu(u, 13, 22.6f));
  blende(g, u, 24, 140, 170);
}

// 7) Am Brunnen: die Steine fallen hinein - Hans ist frei und gluecklich
static void szeneBrunnen(Gfx &g, float u) {
  weg(g, u, 1100, 30 + u * 0.4f);
  if (u < 16) {  // Brunnen
  g.kontur(1.6f, OUT);
  g.rechteck(116, 176, 70, 40, STEIN);
  g.konturAus();
  for (int i = 0; i < 3; i++) g.linie(116, 186 + i * 10, 186, 186 + i * 10, 1.4f, STEIN_D);
  g.ellipse(151, 176, 35, 6, FARBE(0x2A3A5A));
  g.rechteck(118, 126, 5, 50, HOLZ_D);
  g.rechteck(179, 126, 5, 50, HOLZ_D);
  float d[] = {110, 130, 151, 110, 192, 130};
  g.kontur(1.2f, OUT);
  g.polygon(d, 3, FARBE(0xA8452E));
  g.konturAus();
  }
  if (u < 16) {
    float fall = seg(u, 6, 7.4f);
    hans(g, 210, 222, 0.9f, -1, u, -1, u > 7 ? 0.8f : 0.3f, fall > 0 ? 9 : 5, u < 6 ? 1 : 0, u > 8 && u < 14);
    if (fall > 0 && fall < 1) { stein(g, lerp(196, 150, fall), lerp(192, 180, fall) + fall * fall * 10, 0.9f); stein(g, lerp(192, 156, fall), lerp(176, 182, fall) + fall * fall * 10, 0.7f); }
    if (u > 7.4f && u < 9.4f) for (int i = 0; i < 6; i++) tropfen(g, 151 + cosf(i * 1.05f) * seg(u, 7.4f, 9.4f) * 30, 170 - huepf(seg(u, 7.4f, 9.4f)) * 30, 3, FARBE(0x7EC0EE));
    g.kameraAus();
    vers(g, {"Durstig beugte sich Hans über", "den Brunnen - plumps! fielen", "beide Steine hinein."}, aufzu(u, 0.4f, 7.4f));
    blase(g, 190, 50, {"So glücklich wie ich gibt es", "keinen Menschen unter der Sonne!"}, aufzu(u, 8.4f, 15.6f), 212, 130);
    return;
  }
  float v = u - 16;
  float k = seg(v, 0, 8);
  // er springt froh nach Hause, die Mutter wartet an der Tuer
  g.kontur(1.6f, OUT);
  g.rechteck(206, 130, 72, 82, FARBE(0xF0DCC0));
  float dd[] = {198, 132, 242, 96, 286, 132};
  g.polygon(dd, 3, FARBE(0xC0503A));
  g.konturAus();
  g.rundRechteck(230, 168, 22, 44, 8, HOLZ_D);
  FrauPose m = fposis(MUTTER, u);
  m.armV = k > 0.8f ? 1.8f + sinf(u * 3) * 0.2f : 2.6f + sinf(u * 5) * 0.3f;
  m.armH = k > 0.8f ? 1.8f : 0.3f;
  m.mund = 0.5f;
  frau(g, 222, 222, 0.85f, -1, m);
  Pose p = posis(HANS, u);
  p.lauf = k < 1 ? fmodf(u * 1.6f, 1) : -1;
  p.armV = 2.4f + sinf(u * 4) * 0.4f;
  p.mund = 0.7f;
  float hop = k < 1 ? fabsf(sinf(u * 6)) * 8 : 0;
  schneiderlein(g, lerp(40, 182, k), 222 - hop, 0.9f, 1, p);
  if (k >= 1) herzen(g, 206, 150, u, 5, 26);
  g.kameraAus();
  vers(g, {"Mit leichtem Herzen und frei", "von aller Last sprang er fort,", "bis er daheim bei der Mutter war."}, aufzu(v, 0.4f, 12));
  endeBild(g, v, 14, 206, 170);
}

static void bild(Gfx &g, float t) {
  if (t < P2) szeneLohn(g, t - P1);
  else if (t < P3) szenePferd(g, t - P2);
  else if (t < P4) szeneKuh(g, t - P3);
  else if (t < P5) szeneSchwein(g, t - P4);
  else if (t < P6) szeneGans(g, t - P5);
  else if (t < P7) szeneSchleifer(g, t - P6);
  else szeneBrunnen(g, t - P7);
}

// =====================================================================
// Musik
// =====================================================================
// Hansens Wanderlied (C-Dur, froh und einfach)
static const Ton3 MEL_HANS[] = {
  {67, .5, 48}, {67, .5, 0}, {69, .5, 0}, {67, .5, 0}, {72, 1, 48}, {71, 1, 43},
  {67, .5, 43}, {67, .5, 0}, {69, .5, 0}, {67, .5, 0}, {74, 1, 43}, {72, 1, 48},
  {67, .5, 41}, {76, .5, 0}, {74, .5, 0}, {72, .5, 0}, {71, .5, 43}, {69, .5, 0}, {77, .5, 41}, {76, .5, 0}, {72, .5, 43}, {74, .5, 0}, {72, 2, 48}};
static void plumps(Synth &s, float x) { s.ton(x, 180, 0.4f, 0.002f, 0.25f, SINUS, GERAEUSCH, 60); s.rauschen(x, 0.3f, TIEFPASS, 700, 0.2f); }
static void ton(Synth &s, float t0, float t1) {
  float a = P1;
  spieleMelodie(s, M(MEL_HANS), a + 0.5f, a + 19.5f, 0.3f, t0, t1, 0.7f);
  if (bei(a + 9, t0, t1)) for (int k = 0; k < 5; k++) s.note(a + 9 + k * 0.08f, 84 + k * 2, 0.5f);
  a = P2;
  spieleMelodie(s, M(MEL_HANS), a + 0.4f, a + 13, 0.3f, t0, t1, 0.6f);
  for (int i = 0; i < 18; i++) {  // Hufgetrappel
    float x = a + 13 + i * 0.26f;
    if (bei(x, t0, t1)) s.rauschen(x, 0.05f, BANDPASS, 1600 + (i & 1) * 400, 0.12f);
  }
  if (bei(a + 18, t0, t1)) plumps(s, a + 18);
  spieleMelodie(s, M(MEL_SPUK), a + 19, a + 27.5f, 0.42f, t0, t1, 0.35f);
  a = P3;
  spieleMelodie(s, M(MEL_HANS), a + 0.4f, a + 14, 0.3f, t0, t1, 0.6f);
  if (bei(a + 9.8f, t0, t1)) s.ton(a + 9.8f, 160, 0.2f, 0.05f, 0.9f, SAEGE, GERAEUSCH, 120);  // Muuuh
  if (bei(a + 20.3f, t0, t1)) { s.ton(a + 20.3f, 900, 0.15f, 0.002f, 0.2f, SAEGE, GERAEUSCH, 300); plumps(s, a + 20.6f); }
  a = P4;
  spieleMelodie(s, M(MEL_HANS), a + 0.4f, a + 21.5f, 0.3f, t0, t1, 0.65f);
  if (bei(a + 12, t0, t1)) for (int i = 0; i < 3; i++) s.ton(a + 12 + i * 0.25f, 300, 0.15f, 0.01f, 0.15f, SAEGE, GERAEUSCH, 200);  // grunz
  a = P5;
  spieleMelodie(s, M(MEL_HANS), a + 0.4f, a + 23.5f, 0.3f, t0, t1, 0.65f);
  if (bei(a + 13, t0, t1)) { s.ton(a + 13, 620, 0.15f, 0.01f, 0.2f, RECHTECK, GERAEUSCH, 520); s.ton(a + 13.3f, 620, 0.15f, 0.01f, 0.2f, RECHTECK, GERAEUSCH, 520); }  // schnatter
  a = P6;
  spieleMelodie(s, M(MEL_KEGEL), a + 0.4f, a + 12, 0.3f, t0, t1, 0.6f);
  for (int i = 0; i < 12; i++) if (bei(a + 0.4f + i * 1.1f, t0, t1)) s.rauschen(a + 0.4f + i * 1.1f, 0.5f, BANDPASS, 3200, 0.05f);  // Schleifen
  spieleMelodie(s, M(MEL_HANS), a + 13, a + 23.5f, 0.34f, t0, t1, 0.5f);
  a = P7;
  if (bei(a + 7.4f, t0, t1)) { plumps(s, a + 7.4f); plumps(s, a + 7.7f); }
  spieleMelodie(s, M(MEL_HANS), a + 8.4f, a + 30, 0.26f, t0, t1, 0.8f);
  endeKlang(s, a + 30.4f, t0, t1);
}

// =====================================================================
// Startbild, Vorschau, Erkennungszeichen
// =====================================================================
static void titelSzene(Gfx &g, float t) {
  weg(g, t, t * 6);
  einhorn(g, 60, 226, 0.6f, -1, 1, 0, 0, true);
  kuh(g, 230, 226, 0.6f, -1, t);
  gans(g, 186, 226, 0.6f, -1, t);
  hans(g, 128, 226, 0.95f, 1, t, fmodf(t, 1), 0.5f, 0);
}
static void titelbild(Gfx &g, float t) { titelSzene(g, t); titelRahmen(g, t, "Hans im Glück", 170); }
static void vorschau(Gfx &g, float t) { titelSzene(g, t); }
static void symbol(Gfx &g, float x, float y, float t) {
  goldklumpen(g, x, y - 30, 2.4f, t);
  glitzern(g, x, y - 30, 40, t, 5);
}

const Maerchen HANS_GLUECK = {"Hans im Glück", PDAUER, FARBE(0xF2C230), bild, ton, titelbild, vorschau, symbol,
                              LIED(MEL_HANS, 0.3f), nullptr};

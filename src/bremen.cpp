// Die Bremer Stadtmusikanten - ein Spieluhr-Film fuer die Maerchenbox, nach dem Maerchen der Brueder Grimm.
// Esel, Hund, Katze und Hahn: "Etwas Besseres als den Tod findest du ueberall", die Pyramide am Raeuberhaus,
// die Musik, und der Raeuber, der in der Nacht von Hexe, Messer, Ungetuem und Richter erzaehlt.
#include "erzaehlen.h"

// ---------- Ablauf (Sekunden) ----------
static const float P1 = 0, P2 = 28, P3 = 64, P4 = 94, P5 = 126, P6 = 156, P7 = 204, P8 = 248, PDAUER = 290;

static Aussehen muellerLook() {
  Aussehen a; a.weste = FARBE(0xE8E4DC); a.hemd = FARBE(0xF6F2EA); a.hose = FARBE(0xD8D2C6); a.haar = FARBE(0xE0DCD4);
  a.muetze = 1;
  return a;
}
static Aussehen raeuberLook(int i) {
  Aussehen a; a.weste = i & 1 ? FARBE(0x5A3A3A) : FARBE(0x3A3A4A); a.hemd = FARBE(0xC9B8A0); a.hose = FARBE(0x2E2A30);
  a.haar = FARBE(0x2E2220); a.hut = FARBE(0x1E1A22); a.hutD = FARBE(0x141016); a.feder = i & 1 ? FARBE(0xC0392B) : FARBE(0x6E8E4E);
  a.wangen = false; a.halstuch = i & 1 ? FARBE(0xC0392B) : FARBE(0xE0A23C);
  return a;
}
static const Aussehen MUELLER = muellerLook(), R0 = raeuberLook(0), R1 = raeuberLook(1);

// =====================================================================
// Die vier Tiere
// =====================================================================
static void esel(Gfx &g, float x, float y, float s, float dir, float t, float lauf = -1, float mund = 0, bool zu = false,
                 float vorne = 0) {
  const Farbe fell = FARBE(0x9A9AA2), hell = FARBE(0xD6D6DC), dunkel = FARBE(0x6E6E78);
  auto PX = [&](float d) { return x + d * s * dir; };
  auto PY = [&](float d) { return y + d * s; };
  g.kontur(1.4f * s, OUT);
  for (int i = 0; i < 4; i++) {
    float bx = i < 2 ? -20 + i * 9 : 14 + (i - 2) * 9;
    float sw = lauf >= 0 ? sinf(lauf * 6.2832f + i * 1.6f) * 8 : 0;
    float hoch = (i >= 2 ? vorne : 0) * 30;
    g.linie(PX(bx), PY(-30 - hoch * 0.4f), PX(bx + sw + hoch * 0.4f), PY(-4 - hoch), 6 * s, i & 1 ? fell : dunkel);
    g.ellipse(PX(bx + sw + 1 + hoch * 0.4f), PY(-3 - hoch), 4.5f * s, 3 * s, FARBE(0x3A3436));
  }
  g.kurve(PX(-26), PY(-38), PX(-36), PY(-34), PX(-38), PY(-22), 3 * s, fell);  // Schwanz
  g.kreis(PX(-38), PY(-20), 3.5f * s, dunkel);
  g.ellipse(PX(0), PY(-40 - vorne * 8), 30 * s, 14 * s, fell, 255, -vorne * 0.35f * dir);
  // Hals und Kopf
  float hx = PX(24), hy = PY(-56 - vorne * 20);
  g.linie(PX(18), PY(-44 - vorne * 14), hx, hy, 12 * s, fell);
  // lange Ohren
  float o1[] = {hx - 4 * s * dir, hy - 8 * s, hx - 10 * s * dir, hy - 34 * s, hx + 2 * s * dir, hy - 10 * s};
  float o2[] = {hx + 2 * s * dir, hy - 9 * s, hx + 4 * s * dir, hy - 36 * s, hx + 9 * s * dir, hy - 8 * s};
  g.polygon(o1, 3, fell);
  g.polygon(o2, 3, fell);
  g.ellipse(hx + 8 * s * dir, hy + 2 * s, 15 * s, 9 * s, fell, 255, 0.35f * dir);
  g.konturAus();
  g.ellipse(hx + 17 * s * dir, hy + 6 * s, 7 * s, 5.5f * s, hell, 255, 0.35f * dir);  // Maul
  g.kreis(hx + 20 * s * dir, hy + 4 * s, 1.3f * s, OUT);
  if (mund > 0.05f) g.ellipse(hx + 17 * s * dir, hy + 10 * s, 5 * s, (1 + mund * 4) * s, FARBE(0x7A2A2A));
  if (zu) g.bogen(hx + 6 * s * dir, hy - 2 * s, 2.4f * s, 3.5f, 5.9f, 1.4f * s, OUT);
  else {
    g.kreis(hx + 6 * s * dir, hy - 2 * s, 2.6f * s, SCHWARZ);
    g.kreis(hx + 7 * s * dir, hy - 3 * s, 0.9f * s, WEISS);
  }
  // Maehne
  for (int i = 0; i < 5; i++) g.kreis(PX(12 + i * 2.5f), PY(-50 - i * 3 - vorne * (12 + i * 2)), 3 * s, dunkel);
}

static void hund(Gfx &g, float x, float y, float s, float dir, float t, float lauf = -1, float mund = 0, bool liegt = false,
                 bool zu = false) {
  const Farbe fell = FARBE(0xB07A4A), hell = FARBE(0xE8C9A0), dunkel = FARBE(0x7A4E2A);
  auto PX = [&](float d) { return x + d * s * dir; };
  auto PY = [&](float d) { return y + d * s; };
  float ly = liegt ? 8 : 0;
  g.kontur(1.2f * s, OUT);
  if (!liegt)
    for (int i = 0; i < 4; i++) {
      float bx = i < 2 ? -13 + i * 6 : 8 + (i - 2) * 6;
      float sw = lauf >= 0 ? sinf(lauf * 6.2832f + i * 1.6f) * 6 : 0;
      g.linie(PX(bx), PY(-14), PX(bx + sw), PY(-2), 4 * s, i & 1 ? fell : dunkel);
    }
  float wedel = sinf(t * 8) * 6;
  g.kurve(PX(-15), PY(-18 + ly), PX(-24), PY(-26 + ly + wedel * 0.3f), PX(-27), PY(-30 + ly + wedel), 3.5f * s, fell);
  g.ellipse(PX(0), PY(-18 + ly), 17 * s, 8 * s, fell);
  float kx = PX(17), ky = PY(-26 + ly * 1.4f);
  g.kreis(kx, ky, 8 * s, fell);
  g.ellipse(kx + 8 * s * dir, ky + 2 * s, 6 * s, 4.5f * s, hell);
  g.konturAus();
  g.ellipse(kx - 3 * s * dir, ky + 1 * s, 3.5f * s, 7 * s, dunkel, 255, 0.3f * dir);  // Schlappohr
  g.kreis(kx + 13 * s * dir, ky + 1 * s, 1.8f * s, SCHWARZ);
  if (mund > 0.05f) {  // Zunge (hechelt)
    g.ellipse(kx + 9 * s * dir, ky + 6 * s, 3 * s, (1.5f + mund * 3) * s, FARBE(0xE86F7A));
  }
  if (zu) g.bogen(kx + 3 * s * dir, ky - 2 * s, 1.8f * s, 3.5f, 5.9f, 1.1f * s, OUT);
  else g.kreis(kx + 3 * s * dir, ky - 2 * s, 1.5f * s, SCHWARZ);
}

static void katze(Gfx &g, float x, float y, float s, float dir, float t, float lauf = -1, float mund = 0, bool trueb = false,
                  float augenGluehen = 0) {
  const Farbe fell = FARBE(0x8E8E96), streif = FARBE(0x5E5E66), hell = FARBE(0xD8D8DC);
  auto PX = [&](float d) { return x + d * s * dir; };
  auto PY = [&](float d) { return y + d * s; };
  g.kontur(1.1f * s, OUT);
  for (int i = 0; i < 4; i++) {
    float bx = i < 2 ? -9 + i * 5 : 6 + (i - 2) * 5;
    float sw = lauf >= 0 ? sinf(lauf * 6.2832f + i * 1.6f) * 5 : 0;
    g.linie(PX(bx), PY(-10), PX(bx + sw), PY(-1), 3 * s, fell);
  }
  g.kurve(PX(-11), PY(-12), PX(-22), PY(-14), PX(-20 + sinf(t * 2) * 3), PY(-30), 3 * s, fell);
  g.ellipse(PX(0), PY(-13), 13 * s, 7 * s, fell);
  float kx = PX(12), ky = PY(-22);
  g.kreis(kx, ky, 7.5f * s, fell);
  float o1[] = {kx - 6 * s * dir, ky - 3 * s, kx - 6 * s * dir, ky - 13 * s, kx - 1 * s * dir, ky - 6 * s};
  float o2[] = {kx + 1 * s * dir, ky - 6 * s, kx + 6 * s * dir, ky - 13 * s, kx + 7 * s * dir, ky - 3 * s};
  g.polygon(o1, 3, fell);
  g.polygon(o2, 3, fell);
  g.konturAus();
  for (int i = 0; i < 3; i++) g.linie(PX(-6 + i * 5), PY(-19), PX(-4 + i * 5), PY(-8), 1.6f * s, streif);
  g.ellipse(kx + 3 * s * dir, ky + 3 * s, 4 * s, 3 * s, hell);
  for (int i = -1; i <= 1; i++) g.linie(kx + 5 * s * dir, ky + 3 * s, kx + 13 * s * dir, ky + 2 * s + i * 2.5f * s, 0.6f * s, OUT);
  if (augenGluehen > 0) {
    for (int i = 0; i < 2; i++) {
      float ex = kx + (i ? 4.5f : -1.5f) * s * dir;
      g.kreis(ex, ky - 1.5f * s, 6 * s, FARBE(0xFFD54A), (uint8_t)(70 * augenGluehen));
      g.kreis(ex, ky - 1.5f * s, 2.2f * s, FARBE(0xFFE070));
      g.ellipse(ex, ky - 1.5f * s, 0.6f * s, 1.8f * s, SCHWARZ);
    }
  } else
    for (int i = 0; i < 2; i++) {
      float ex = kx + (i ? 4.5f : -1.5f) * s * dir;
      if (trueb) g.bogen(ex, ky - 0.5f * s, 1.6f * s, 3.6f, 5.8f, 1 * s, OUT);
      else { g.kreis(ex, ky - 1.5f * s, 1.8f * s, FARBE(0x9ACB4A)); g.ellipse(ex, ky - 1.5f * s, 0.5f * s, 1.5f * s, SCHWARZ); }
    }
  if (trueb) g.bogen(kx + 3 * s * dir, ky + 7 * s, 2.4f * s, 3.6f, 5.8f, 1 * s, OUT);  // mault
  if (mund > 0.05f) g.ellipse(kx + 3 * s * dir, ky + 5 * s, 1.6f * s, (0.8f + mund * 2) * s, FARBE(0x8A2A2A));
}

static void hahn(Gfx &g, float x, float y, float s, float dir, float t, float kraeht = 0, bool zu = false) {
  auto PX = [&](float d) { return x + d * s * dir; };
  auto PY = [&](float d) { return y + d * s; };
  Farbe fed = FARBE(0xC9702E);
  g.linie(PX(-2), PY(-8), PX(-3), PY(-1), 1.4f * s, FARBE(0xE0A23C));
  g.linie(PX(3), PY(-8), PX(4), PY(-1), 1.4f * s, FARBE(0xE0A23C));
  for (int i = 0; i < 4; i++)
    g.kurve(PX(-9), PY(-16), PX(-18 - i * 2), PY(-28 + i * 4), PX(-16 - i * 3), PY(-12 + i * 3), 3 * s,
            i & 1 ? FARBE(0x2E6E4E) : FARBE(0x1E3A5A));
  g.kontur(1.1f * s, OUT);
  g.ellipse(PX(0), PY(-14), 11 * s, 8 * s, fed);
  g.konturAus();
  g.ellipse(PX(-2), PY(-14), 6 * s, 4 * s, FARBE(0xA8582A));
  float kx = PX(8), ky = PY(-24 - kraeht * 3);
  g.kontur(1.0f * s, OUT);
  g.kreis(kx, ky, 5 * s, FARBE(0xE0A050));
  g.konturAus();
  g.kreis(kx + 0.5f * s * dir, ky - 5 * s, 3.2f * s, FARBE(0xE0302A));
  g.kreis(kx - 2 * s * dir, ky - 5.5f * s, 2.6f * s, FARBE(0xE0302A));
  float auf = kraeht * 3;
  float sn1[] = {kx + 4 * s * dir, ky - 1 * s, kx + 9 * s * dir, ky - 0.5f * s - auf * s, kx + 4 * s * dir, ky + 1 * s};
  float sn2[] = {kx + 4 * s * dir, ky + 1 * s, kx + 9 * s * dir, ky + 1.5f * s + auf * s, kx + 4 * s * dir, ky + 2.5f * s};
  g.polygon(sn1, 3, FARBE(0xF2B230));
  g.polygon(sn2, 3, FARBE(0xF2B230));
  g.kreis(kx + 3 * s * dir, ky + 3.5f * s, 1.6f * s, FARBE(0xE0302A));
  if (zu) g.bogen(kx + 1.5f * s * dir, ky - 0.5f * s, 1.5f * s, 3.5f, 5.9f, 1 * s, OUT);
  else g.kreis(kx + 1.5f * s * dir, ky - 1 * s, 1.1f * s, SCHWARZ);
}

// die vier Musikanten im Zug hintereinander, alle laufen
static void zug(Gfx &g, float x, float t, int wieviele, bool laufen) {
  float l = laufen ? fmodf(t * 1.6f, 1) : -1;
  if (wieviele >= 4) hahn(g, x - 140, 214 - (laufen ? fabsf(sinf(t * 10)) * 2 : 0), 1, 1, t);
  if (wieviele >= 3) katze(g, x - 106, 214, 1, 1, t, laufen ? fmodf(t * 1.8f, 1) : -1);
  if (wieviele >= 2) hund(g, x - 64, 214, 1, 1, t, laufen ? fmodf(t * 1.7f, 1) : -1);
  esel(g, x, 214, 1, 1, t, l);
}

// ---------- Kulissen ----------
static void landweg(Gfx &g, float t, float scroll, float abend = 0) {
  himmel(g, abend, t, 190);
  if (abend < 0.6f) sonne(g, 248, 30, 12, t);
  wolke(g, fmodf(80 - scroll * 0.1f + 600, 340) - 30, 38, 0.8f);
  huegel(g, scroll * 0.3f, 0.4f, 190, 28, 100, mischeFarbe(FARBE(0x8ACB6A), FARBE(0x3A5A5A), abend));
  boden(g, 190, mischeFarbe(GRAS, FARBE(0x3A5A4A), abend));
  g.rechteck(0, 206, 284, 18, mischeFarbe(FARBE(0xD8C49A), FARBE(0x6E6450), abend));  // Weg
  for (int i = 0; i < 4; i++) laubbaum(g, fmodf(i * 90 - scroll + 900, 360) - 40, 200, 0.7f, mischeFarbe(FARBE(0x4CB85A), FARBE(0x2E5A4A), abend));
  // Wegweiser nach Bremen
  float wx = fmodf(200 - scroll + 1800, 400) - 60;
  g.rechteck(wx, 160, 4, 46, HOLZ_D);
  g.kontur(1, OUT);
  float sch[] = {wx - 14, 160, wx + 30, 160, wx + 36, 167, wx + 30, 174, wx - 14, 174};
  g.polygon(sch, 5, HOLZ);
  g.konturAus();
  g.text(SCHRIFT_K, "Bremen", wx + 10, 172, FARBE(0xF6E8D0), 1);
}

static void muehlenhof(Gfx &g, float t) {
  himmel(g, 0, t, 196);
  sonne(g, 248, 30, 12, t);
  boden(g, 196);
  g.kontur(1.8f, OUT);
  g.rechteck(150, 110, 90, 86, FARBE(0xE0D4BC));
  float d[] = {140, 112, 195, 70, 250, 112};
  g.polygon(d, 3, FARBE(0x8E5A3A));
  g.konturAus();
  g.rundRechteck(182, 150, 26, 46, 10, HOLZ);
  // Windmuehlenfluegel
  for (int i = 0; i < 4; i++) {
    float a = t * 0.8f + i * 1.5708f;
    float ex = 195 + cosf(a) * 56, ey = 96 + sinf(a) * 56;
    g.linie(195, 96, ex, ey, 3, HOLZ_D);
    float q[] = {195 + cosf(a) * 14 - sinf(a) * 6, 96 + sinf(a) * 14 + cosf(a) * 6, ex - sinf(a) * 9, ey + cosf(a) * 9, ex, ey,
                 195 + cosf(a) * 14, 96 + sinf(a) * 14};
    g.polygon(q, 4, FARBE(0xF2EEE4), 220);
  }
  g.kreis(195, 96, 5, HOLZ_D);
}

static void hoftor(Gfx &g, float t) {
  himmel(g, 0, t, 196);
  sonne(g, 248, 30, 12, t);
  boden(g, 196);
  g.kontur(1.6f, OUT);
  g.rechteck(10, 120, 80, 76, FARBE(0xF4E4C4));
  float d[] = {2, 122, 50, 86, 98, 122};
  g.polygon(d, 3, FARBE(0xC0503A));
  g.rechteck(120, 120, 14, 80, FARBE(0xC9B8A0));
  g.rechteck(230, 120, 14, 80, FARBE(0xC9B8A0));
  g.rechteck(116, 114, 132, 10, HOLZ_D);  // das Tor, oben sitzt der Hahn
  g.konturAus();
  for (int i = 0; i < 6; i++) g.rechteck(136 + i * 16, 132, 8, 64, HOLZ);
}

static void nachtwald(Gfx &g, float t, float licht) {
  himmel(g, 1, t, 200);
  for (int i = 0; i < 26; i++) g.stern(zufall(i + 700) * 284, zufall(i + 720) * 110, 2, 0.8f, 4, 0, WEISS, (uint8_t)(140 + 100 * sinf(t + i)));
  mond(g, 40, 36, 11);
  for (int i = 0; i < 6; i++) tanne(g, i * 54 + 10, 200, 0.85f + (i % 2) * 0.15f, FARBE(0x1F3B45));
  boden(g, 200, FARBE(0x2F4A40));
  // grosser Baum, auf dem die Katze und der Hahn sitzen
  g.linie(150, 200, 152, 90, 10, FARBE(0x3E2E26));
  g.linie(150, 120, 116, 104, 4, FARBE(0x3E2E26));
  g.linie(152, 110, 190, 92, 4, FARBE(0x3E2E26));
  g.kreis(150, 70, 34, FARBE(0x1F4A3A));
  g.kreis(118, 90, 22, FARBE(0x245040));
  g.kreis(188, 80, 24, FARBE(0x245040));
  if (licht > 0) {  // in der Ferne ein Lichtlein
    g.kreis(254, 166, 9, FARBE(0xFFD27A), (uint8_t)(70 * licht));
    g.kreis(254, 166, 3, FARBE(0xFFE9A0), (uint8_t)(255 * licht));
  }
}

static void raeuberhaus(Gfx &g, float t, float nacht, float scherben) {
  himmel(g, nacht, t, 200);
  for (int i = 0; i < 20; i++) g.stern(zufall(i + 700) * 284, zufall(i + 720) * 90, 2, 0.8f, 4, 0, WEISS, 180);
  mond(g, 248, 32, 10);
  boden(g, 200, FARBE(0x2F4A40));
  for (int i = 0; i < 3; i++) tanne(g, i * 30 - 6, 200, 0.8f, FARBE(0x1F3B45));
  g.kontur(1.8f, OUT);
  g.rechteck(70, 100, 190, 100, FARBE(0x8E7A6A));
  float d[] = {60, 102, 165, 54, 270, 102};
  g.polygon(d, 3, FARBE(0x4E3A3A));
  g.konturAus();
  g.rundRechteck(196, 150, 30, 50, 10, FARBE(0x3E2E26));
  // das helle Fenster
  g.kontur(1.6f, OUT);
  g.rundRechteck(100, 118, 76, 56, 4, FARBE(0xFFD27A));
  g.konturAus();
  g.kreis(138, 146, 60, FARBE(0xFFD27A), 40);
  if (scherben < 1) {
    g.rechteck(136, 118, 4, 56, HOLZ_D);
    g.rechteck(100, 144, 76, 4, HOLZ_D);
  }
}

// Blick durchs Fenster: Raeuber am gedeckten Tisch
static void raeuberAmTisch(Gfx &g, float t, float schreck) {
  g.ausschnitt(100, 118, 76, 56);
  g.verlauf(118, 174, FARBE(0xE8C080), FARBE(0xC09050));
  for (int i = 0; i < 3; i++) {
    Pose r = posis(i & 1 ? R1 : R0, t, i);
    r.mund = schreck > 0 ? 0.9f : 0.4f + 0.3f * fabsf(sinf(t * 5 + i));
    r.armV = schreck > 0 ? 2.8f : 1.5f + sinf(t * 4 + i) * 0.3f;
    r.armH = schreck > 0 ? 2.8f : 0.4f;
    float rx = 112 + i * 24 + schreck * 60;
    schneiderlein(g, rx, 196, 0.62f, i == 2 ? -1 : 1, r);
  }
  g.rechteck(100, 162, 76, 12, HOLZ);
  g.ellipse(124, 160, 9, 4, FARBE(0xC9702E));  // Braten
  g.rechteck(146, 152, 4, 9, FARBE(0x6E1E3A));
  g.ellipse(160, 160, 7, 3, FARBE(0xE8D8A0));
  g.ausschnittAus();
}

// die Pyramide: Esel unten mit den Vorderfuessen am Fenster, Hund, Katze, Hahn
static void pyramide(Gfx &g, float x, float y, float t, float musik, float hops = 0) {
  float m = musik > 0 ? 0.8f * fabsf(sinf(t * 9)) : 0;
  esel(g, x, y - hops, 1, 1, t, -1, m, false, 1);
  float ex = x + 6, ey = y - 66 - hops;  // Ruecken
  hund(g, ex, ey, 0.95f, 1, t, -1, m);
  katze(g, ex + 4, ey - 24, 0.95f, 1, t, -1, m);
  hahn(g, ex + 12, ey - 46, 0.95f, 1, t, musik > 0 ? 0.6f + 0.4f * sinf(t * 9) : 0);
  if (musik > 0)
    for (int i = 0; i < 6; i++) {
      float ph = fmodf(t * 0.7f + i / 6.0f, 1);
      notenzeichen(g, x + 30 + ph * 60 + sinf(ph * 7 + i) * 10, y - 120 - ph * 60, i & 1 ? FARBE(0xE8473D) : GOLD, (uint8_t)(255 * huepf(ph)));
    }
}

static void stubeDunkel(Gfx &g, float t, float hell) {
  g.verlauf(0, 200, mischeFarbe(FARBE(0x2A2434), FARBE(0x8E7A6A), hell), mischeFarbe(FARBE(0x1E1828), FARBE(0x6E5A4A), hell));
  g.rechteck(0, 200, 284, 40, mischeFarbe(FARBE(0x2E2620), FARBE(0x6E5A46), hell));
  // Herd mit Asche
  g.kontur(1.6f, OUT);
  g.rundRechteck(30, 140, 70, 62, 4, mischeFarbe(FARBE(0x3E3640), FARBE(0xA89888), hell));
  g.konturAus();
  g.rundRechteck(42, 164, 46, 36, 14, FARBE(0x141016));
  for (int i = 0; i < 3; i++) g.kreis(52 + i * 12, 194, 2.4f, FARBE(0xFF7A2A), (uint8_t)(120 + 100 * sinf(t * 3 + i)));  // Glut
  // Tuer
  g.rundRechteck(200, 120, 40, 80, 10, mischeFarbe(FARBE(0x2E2220), HOLZ, hell));
  // Hahnenbalken
  g.rechteck(110, 60, 120, 6, HOLZ_D);
}

// =====================================================================
// Szenen
// =====================================================================
// 1) Der alte Esel will nach Bremen
static void szeneEsel(Gfx &g, float u) {
  muehlenhof(g, u);
  Pose m = posis(MUELLER, u);
  m.boese = 0.6f;
  m.mund = u > 6 && u < 11 ? 0.5f : 0;
  m.armV = u > 6 && u < 11 ? 1.6f : 0.6f;
  schneiderlein(g, 214, 214, 0.95f, -1, m);
  float ex = u < 14 ? 110 : lerp(110, 330, seg(u, 16, 27));
  esel(g, ex, 214, 1, u < 14 ? -1 : 1, u, u > 16 ? fmodf(u * 1.6f, 1) : -1, u > 11 && u < 15 ? 0.5f : 0, u < 6);
  if (u < 11) for (int i = 0; i < 2; i++) { g.kontur(1, OUT); g.ellipse(100 + i * 18, 166, 9, 11, FARBE(0xD8C09A)); g.konturAus(); }  // Saecke
  g.kameraAus();
  vers(g, {"Es hatte ein Mann einen Esel, der ihm", "schon lange Jahre die Säcke getragen."}, aufzu(u, 0.4f, 6));
  vers(g, {"Nun aber gingen die Kräfte des Esels", "zu Ende. Da lief er fort."}, aufzu(u, 6.2f, 11));
  blase(g, 110, 70, {"Ich gehe nach Bremen und", "werde dort Stadtmusikant!"}, aufzu(u, 11.2f, 16), 130, 150);
  blende(g, u, 28, 140, 150);
}

// 2) Der Jagdhund
static void szeneHund(Gfx &g, float u) {
  float scroll = u < 6 ? u * 40 : 240;
  landweg(g, u, scroll);
  float ex = u < 6 ? 120 : 120;
  esel(g, ex, 214, 1, 1, u, u < 6 ? fmodf(u * 1.6f, 1) : -1, (u > 6 && u < 9) || (u > 17 && u < 26) ? 0.4f + 0.3f * fabsf(sinf(u * 7)) : 0);
  bool mit = u > 30;
  float hx = mit ? lerp(200, 70, seg(u, 30, 34)) : 200;
  hund(g, hx, 214, 1, mit ? 1 : -1, u, mit ? fmodf(u * 1.7f, 1) : -1, u < 30 ? 0.5f + 0.4f * fabsf(sinf(u * 6)) : 0.3f, u < 26);
  g.kameraAus();
  blase(g, 110, 70, {"Nun, was jappst du so, Packan?"}, aufzu(u, 6, 9.4f), 150, 150);
  blase(g, 200, 70, {"Ach, weil ich alt bin und jeden", "Tag schwächer werde, hat mich", "mein Herr wollen totschlagen."}, aufzu(u, 9.6f, 16.6f), 210, 180);
  blase(g, 110, 60, {"Ich gehe nach Bremen. Geh mit!", "Ich spiele die Laute,", "und du schlägst die Pauken."}, aufzu(u, 17, 25.6f), 150, 150);
  blase(g, 200, 70, {"Gern!"}, aufzu(u, 26, 29.6f), 210, 180);
  blende(g, u, 36, 150, 150);
}

// 3) Die Katze mit dem Gesicht wie drei Tage Regenwetter
static void szeneKatze(Gfx &g, float u) {
  landweg(g, u, 400 + (u < 5 ? u * 40 : 200));
  // ein kleines Regenwoelkchen ueber der Katze
  bool mit = u > 25;
  float kx = mit ? lerp(210, 40, seg(u, 25, 29)) : 210;
  if (!mit) {
    wolke(g, 214, 140, 0.5f);
    for (int i = 0; i < 5; i++) g.linie(204 + i * 5, 150 + fmodf(u * 40 + i * 9, 30), 202 + i * 5, 156 + fmodf(u * 40 + i * 9, 30), 1, FARBE(0x8FD8FF));
  }
  zug(g, 136, u, 2, u < 5);
  katze(g, kx, 214, 1.1f, mit ? 1 : -1, u, mit ? fmodf(u * 1.8f, 1) : -1, u > 10 && u < 16 ? 0.5f : 0, !mit && u < 22);
  g.kameraAus();
  vers(g, {"Da saß eine Katze an dem Weg", "und machte ein Gesicht", "wie drei Tage Regenwetter."}, aufzu(u, 0.4f, 6.4f));
  blase(g, 110, 60, {"Nun, was ist dir in die Quere", "gekommen, alter Bartputzer?"}, aufzu(u, 6.6f, 10.4f), 150, 150);
  blase(g, 200, 70, {"Weil ich zu Jahren komme,", "hat mich meine Frau", "ersäufen wollen."}, aufzu(u, 10.6f, 16), 214, 180);
  blase(g, 110, 60, {"Geh mit uns nach Bremen,", "du verstehst dich doch", "auf die Nachtmusik!"}, aufzu(u, 16.2f, 22.6f), 150, 150);
  blende(g, u, 30, 150, 150);
}

// 4) Der Hahn auf dem Tor
static void szeneHahn(Gfx &g, float u) {
  hoftor(g, u);
  bool mit = u > 24;
  float hx = mit ? lerp(186, 40, seg(u, 24, 28)) : 186;
  float hy = mit ? lerp(114, 214, seg(u, 24, 25.5f)) : 114;
  float kraeh = !mit && (u < 6 || (u > 9 && u < 13)) ? 0.6f + 0.4f * fabsf(sinf(u * 8)) : 0;
  hahn(g, hx, hy, 1.2f, mit ? 1 : -1, u, kraeh);
  zug(g, 140, u, 3, false);
  g.kameraAus();
  blase(g, 186, 60, {"Kikeriki!"}, aufzu(u, 1, 5), 196, 82);
  blase(g, 90, 70, {"Du schreist einem durch", "Mark und Bein. Was hast du vor?"}, aufzu(u, 5.4f, 9), 110, 150);
  blase(g, 200, 60, {"Morgen soll ich in", "die Suppe, da schrei ich,", "solang ich noch kann!"}, aufzu(u, 9.2f, 15), 196, 82);
  blase(g, 110, 56, {"Ei was, du Rotkopf, zieh mit uns!", "Etwas Besseres als den Tod", "findest du überall."}, aufzu(u, 15.2f, 23.6f), 110, 150);
  blende(g, u, 32, 140, 150);
}

// 5) Nacht im Wald, der Hahn sieht ein Licht
static void szeneNacht(Gfx &g, float u) {
  nachtwald(g, u, seg(u, 10, 12));
  esel(g, 100, 214, 0.95f, 1, u, -1, 0, true);
  hund(g, 180, 214, 0.95f, -1, u, -1, 0, true, true);
  katze(g, 116, 106, 0.85f, 1, u, -1, 0, true);
  hahn(g, 168, 64, 0.9f, 1, u, u > 10 && u < 13 ? 0.4f : 0, u < 9);
  if (u < 9) zzz(g, 120, 160, u);
  g.kameraAus();
  vers(g, {"Abends kamen sie in einen Wald,", "wo sie übernachten wollten."}, aufzu(u, 0.4f, 8));
  blase(g, 186, 40, {"Ich sehe ein Licht!", "Da muss ein Haus sein."}, aufzu(u, 12, 17.6f), 176, 56);
  blase(g, 90, 110, {"So wollen wir hingehen!"}, aufzu(u, 18, 22), 120, 166);
  blende(g, u, 30, 142, 140);
}

// 6) Am Raeuberhaus: die Pyramide, die Musik, die Scheiben klirren
static void szeneRaeuber(Gfx &g, float u) {
  float scherben = seg(u, 26, 27);
  raeuberhaus(g, u, 0.9f, scherben);
  raeuberAmTisch(g, u, u > 27 ? seg(u, 27, 29) : 0);
  if (u < 14) {
    // der Esel schaut hinein
    esel(g, 76, 214, 1, 1, u, u < 3 ? fmodf(u * 1.6f, 1) : -1, u > 4 && u < 11 ? 0.4f : 0, false, seg(u, 2, 3.5f));
    hund(g, 30, 214, 0.9f, 1, u);
    katze(g, 8, 214, 0.9f, 1, u);
    hahn(g, 50, 160, 0.9f, 1, u);
  } else {
    // sie steigen aufeinander
    float k = seg(u, 14, 18);
    float m = u > 20 && u < 27 ? 1 : 0;
    if (k < 1) {
      esel(g, 76, 214, 1, 1, u, -1, 0, false, 1);
      hund(g, lerp(30, 82, k), lerp(214, 148, k), 0.95f, 1, u);
      katze(g, lerp(8, 86, seg(u, 15, 18)), lerp(214, 124, seg(u, 15, 18)), 0.95f, 1, u);
      hahn(g, lerp(50, 94, seg(u, 16, 18)), lerp(160, 102, seg(u, 16, 18)), 0.95f, 1, u);
    } else if (u < 27) {
      pyramide(g, 76, 214, u, m);
    } else {
      // sie stuerzen durchs Fenster hinein
      float f = seg(u, 27, 28.4f);
      g.ausschnitt(0, 0, 284, 240);
      pyramide(g, 76 + f * 40, 214, u, 1, huepf(f) * 20);
    }
    if (u > 26.6f && u < 30)
      for (int i = 0; i < 12; i++) {  // Scherben
        float k2 = seg(u, 26.6f, 29), a = i * 0.52f;
        float sx = 138 + cosf(a) * k2 * 70, sy = 146 + sinf(a) * k2 * 50 + k2 * k2 * 40;
        float q[] = {sx, sy - 3, sx + 3, sy + 2, sx - 2, sy + 3};
        g.polygon(q, 3, FARBE(0xDFF4FF), (uint8_t)(255 * (1 - k2)));
      }
  }
  // die Raeuber fliehen in den Wald
  if (u > 30) {
    for (int i = 0; i < 3; i++) {
      Pose r = posis(i & 1 ? R1 : R0, u, i);
      r.lauf = fmodf(u * 2.4f + i * 0.3f, 1);
      r.armV = 2.8f; r.armH = 2.8f; r.mund = 0.9f;
      schneiderlein(g, lerp(214, -60, seg(u, 30 + i * 0.4f, 36 + i * 0.4f)), 214, 0.85f, -1, r);
    }
  }
  if (u > 26.6f && u < 27.2f) g.wackeln(sinf(u * 90) * 4, cosf(u * 80) * 3);
  g.kameraAus();
  blase(g, 160, 40, {"Was ich sehe? Einen gedeckten", "Tisch mit schönem Essen", "und Trinken, und Räuber daran."}, aufzu(u, 4, 11), 112, 120);
  vers(g, {"Da ratschlagten die Tiere, wie sie", "es anfangen müssten ..."}, aufzu(u, 11.4f, 18));
  if (u > 20 && u < 27) g.text(SCHRIFT_B, "Iah! Wau! Miau! Kikeriki!", 142, 30, WEISS, 1, (uint8_t)(255 * aufzu(u, 20, 27)));
  vers(g, {"Die Räuber fuhren bei dem", "entsetzlichen Geschrei in die Höhe", "und flohen in den Wald."}, aufzu(u, 30, 38));
  blende(g, u, 48, 140, 150);
}

// 7) In der Nacht: der Raeuber im dunklen Haus
static void szeneNachts(Gfx &g, float u) {
  stubeDunkel(g, u, 0.1f);
  // die Tiere haben sich schlafen gelegt
  bool los = u > 18;
  katze(g, 64, 160, 0.9f, 1, u, -1, los && u < 24 ? 0.8f : 0, !los, u > 12 ? 1 : 0);
  hund(g, 190, 214, 0.9f, 1, u, -1, los && u > 21 && u < 25 ? 0.6f : 0, !(u > 20 && u < 26), !(u > 20 && u < 26));
  hahn(g, 170, 60, 0.85f, -1, u, u > 25 && u < 29 ? 0.8f : 0, u < 25);
  g.ausschnitt(0, 0, 284, 240);
  // der Raeuber schleicht herein
  Pose r = posis(R0, u);
  float rx;
  if (u < 12) { rx = lerp(260, 110, sanft(seg(u, 2, 12))); r.lauf = u > 2 ? fmodf(u * 1.1f, 1) : -1; r.armV = 1.5f; }
  else if (u < 18) { rx = 110; r.armV = 1.7f; }  // haelt ein Schwefelhoelzchen an die "Kohlen"
  else { rx = lerp(110, 330, seg(u, 19, 31)); r.lauf = fmodf(u * 2.6f, 1); r.armV = 2.8f; r.armH = 2.8f; r.mund = 0.9f; }
  float hx, hy;
  schneiderlein(g, rx, 214, 0.9f, u < 18 ? -1 : 1, r, &hx, &hy);
  if (u > 12 && u < 18) { g.linie(hx, hy, hx - 10, hy - 6, 1.2f, HOLZ); g.kreis(hx - 11, hy - 7, 2, FARBE(0xFFB83D)); }
  // der Esel draussen auf dem Mist gibt einen Tritt
  if (u > 24 && u < 28) for (int i = 0; i < 4; i++) g.stern(rx - 6, 140 - i * 6, 3, 1.2f, 4, u * 6 + i, GOLD);
  g.ausschnittAus();
  if (u > 18 && u < 19.5f) wolkePuff(g, 80, 150, seg(u, 18, 19.5f), FARBE(0x6E6870));
  g.kameraAus();
  vers(g, {"Als Mitternacht vorbei war, schickte", "der Hauptmann einen Räuber zurück."}, aufzu(u, 0.4f, 8));
  vers(g, {"Er hielt die feurigen Katzenaugen", "für glühende Kohlen ..."}, aufzu(u, 8.4f, 17.6f));
  if (u > 18 && u < 31) {
    g.text(SCHRIFT_M, u < 21 ? "Pfff! Kratz!" : u < 24 ? "Wau! Zwick!" : u < 27 ? "Rums!" : "Kikeriki!", 142, 40, WEISS, 1,
           (uint8_t)(255 * aufzu(fmodf(u - 18, 3), 0, 3)));
  }
  blende(g, u, 44, 142, 150);
}

// 8) Der Raeuber erzaehlt dem Hauptmann - und die vier bleiben im Haus
static void szeneErzaehlt(Gfx &g, float u) {
  if (u < 22) {
    nachtwald(g, u, 0);
    Pose h = posis(R1, u);
    h.boese = 0.6f;
    h.armV = 1.2f;
    schneiderlein(g, 70, 214, 1, 1, h);
    Pose r = posis(R0, u, 1);
    r.armV = 2.2f + sinf(u * 6) * 0.4f;
    r.mund = 0.6f + 0.3f * fabsf(sinf(u * 7));
    schneiderlein(g, 200, 214, 0.95f, -1, r);
    for (int i = 0; i < 3; i++) g.linie(190 + i * 6, 120, 192 + i * 6, 108, 1.2f, WEISS);  // zitternd
    g.kameraAus();
    blase(g, 150, 42, {"Ach, in dem Haus sitzt eine", "gräuliche Hexe, die hat mir", "das Gesicht zerkratzt;"}, aufzu(u, 0.6f, 6), 196, 130);
    blase(g, 150, 42, {"vor der Tür steht ein Mann mit", "einem Messer, der hat mich", "ins Bein gestochen;"}, aufzu(u, 6.2f, 11), 196, 130);
    blase(g, 150, 42, {"auf dem Hof liegt ein schwarzes", "Ungetüm, das hat mit einem", "Holzprügel auf mich losgeschlagen;"}, aufzu(u, 11.2f, 16), 196, 130);
    blase(g, 150, 42, {"und oben auf dem Dache sitzt", "der Richter, der rief:", "Bringt mir den Schelm her!"}, aufzu(u, 16.2f, 21.6f), 196, 130);
    if (u > 21) g.rechteck(0, 0, 284, 240, 0, (uint8_t)(255 * seg(u, 21, 22)));
    return;
  }
  float v = u - 22;
  raeuberhaus(g, u, 0, 0);
  for (int i = 0; i < 2; i++) laubbaum(g, 20 + i * 250, 214, 0.9f, FARBE(0x4CB85A));
  float hop = fabsf(sinf(u * 5)) * 4;
  esel(g, 70, 222 - hop, 0.8f, 1, u, -1, 0.4f);
  hund(g, 130, 222 - fabsf(sinf(u * 5 + 1)) * 4, 0.9f, 1, u, -1, 0.5f);
  katze(g, 170, 222 - fabsf(sinf(u * 5 + 2)) * 4, 0.9f, -1, u);
  hahn(g, 214, 222 - fabsf(sinf(u * 5 + 3)) * 4, 0.9f, -1, u, 0.4f);
  for (int i = 0; i < 5; i++) {
    float ph = fmodf(u * 0.6f + i / 5.0f, 1);
    notenzeichen(g, 60 + i * 40 + sinf(ph * 6) * 8, 170 - ph * 70, i & 1 ? FARBE(0xE8473D) : GOLD, (uint8_t)(255 * huepf(ph)));
  }
  g.kameraAus();
  if (v < 1) g.rechteck(0, 0, 284, 240, 0, (uint8_t)(255 * (1 - v)));
  vers(g, {"Den vier Bremer Musikanten", "gefiel's darin so wohl, dass sie", "nicht wieder hinaus wollten."}, aufzu(v, 1, 8));
  vers(g, {"Und der das zuletzt erzählt hat,", "dem ist der Mund noch warm."}, aufzu(v, 8.2f, 14.6f));
  endeBild(g, v, 15);
}

static void bild(Gfx &g, float t) {
  if (t < P2) szeneEsel(g, t - P1);
  else if (t < P3) szeneHund(g, t - P2);
  else if (t < P4) szeneKatze(g, t - P3);
  else if (t < P5) szeneHahn(g, t - P4);
  else if (t < P6) szeneNacht(g, t - P5);
  else if (t < P7) szeneRaeuber(g, t - P6);
  else if (t < P8) szeneNachts(g, t - P7);
  else szeneErzaehlt(g, t - P8);
}

// =====================================================================
// Musik
// =====================================================================
// Marsch der Stadtmusikanten (C-Dur)
static const Ton3 MEL_BREMEN[] = {
  {72, .5, 48}, {72, .5, 0}, {76, .5, 0}, {79, .5, 0}, {79, 1, 43}, {76, 1, 0},
  {77, .5, 41}, {77, .5, 0}, {74, .5, 0}, {77, .5, 0}, {76, 1, 48}, {72, 1, 0},
  {72, .5, 48}, {76, .5, 0}, {79, .5, 0}, {84, .5, 0}, {83, .5, 41}, {79, .5, 0}, {77, .5, 0}, {74, .5, 0},
  {72, 1, 48}, {79, 1, 43}, {72, 2, 48}};

static void iah(Synth &s, float x) { s.ton(x, 520, 0.08f, 0.02f, 0.3f, SAEGE, GERAEUSCH, 300); s.ton(x + 0.35f, 260, 0.08f, 0.02f, 0.4f, SAEGE, GERAEUSCH, 380); }
static void wau(Synth &s, float x) { for (int i = 0; i < 2; i++) s.ton(x + i * 0.25f, 300, 0.1f, 0.005f, 0.12f, RECHTECK, GERAEUSCH, 180); }
static void miau(Synth &s, float x) { s.ton(x, 600, 0.05f, 0.05f, 0.4f, DREIECK, GERAEUSCH, 900); }
static void kikeriki(Synth &s, float x) {
  const float f[5] = {560, 700, 700, 840, 620};
  for (int i = 0; i < 5; i++) s.ton(x + i * 0.18f, f[i], 0.06f, 0.01f, i == 4 ? 0.5f : 0.16f, DREIECK, GERAEUSCH, f[i] * 1.1f);
}

static void ton(Synth &s, float t0, float t1) {
  float a = P1;
  spieleMelodie(s, M(MEL_WIEGE), a + 0.5f, a + 11, 0.45f, t0, t1, 0.7f);
  if (bei(a + 11.4f, t0, t1)) iah(s, a + 11.4f);
  spieleMelodie(s, M(MEL_BREMEN), a + 16, a + 27.5f, 0.24f, t0, t1, 0.85f);
  for (float x = ceilf(t0 / 0.4f) * 0.4f; x < t1; x += 0.4f)
    if (x > a + 16 && x < a + 27) s.ton(x, 900, 0.03f, 0.002f, 0.04f, SINUS, GERAEUSCH);  // Hufe
  a = P2;
  spieleMelodie(s, M(MEL_BREMEN), a + 0.4f, a + 6, 0.24f, t0, t1, 0.8f);
  for (float x = ceilf(t0 / 0.3f) * 0.3f; x < t1; x += 0.3f)
    if (x > a + 6 && x < a + 26) s.rauschen(x, 0.1f, BANDPASS, 1200, 0.03f);  // jappen
  spieleMelodie(s, M(MEL_WIEGE), a + 9.6f, a + 16.6f, 0.4f, t0, t1, 0.6f);
  if (bei(a + 26.2f, t0, t1)) wau(s, a + 26.2f);
  spieleMelodie(s, M(MEL_BREMEN), a + 30, a + 35.5f, 0.24f, t0, t1, 0.85f);
  a = P3;
  spieleMelodie(s, M(MEL_WIEGE), a + 0.4f, a + 16, 0.5f, t0, t1, 0.55f);
  for (float x = ceilf(t0 / 0.12f) * 0.12f; x < t1; x += 0.12f)
    if (x > a && x < a + 22) s.ton(x, 3000 + ((int)(x * 31) % 5) * 300, 0.006f, 0.001f, 0.02f, SINUS, GERAEUSCH);  // Regentropfen
  if (bei(a + 22.8f, t0, t1)) miau(s, a + 22.8f);
  spieleMelodie(s, M(MEL_BREMEN), a + 25, a + 29.5f, 0.24f, t0, t1, 0.85f);
  a = P4;
  if (bei(a + 1, t0, t1)) kikeriki(s, a + 1);
  if (bei(a + 9.4f, t0, t1)) kikeriki(s, a + 9.4f);
  spieleMelodie(s, M(MEL_WANDER), a + 15.4f, a + 23.6f, 0.3f, t0, t1, 0.75f);
  if (bei(a + 23.8f, t0, t1)) kikeriki(s, a + 23.8f);
  spieleMelodie(s, M(MEL_BREMEN), a + 24.6f, a + 31.5f, 0.24f, t0, t1, 0.9f);
  a = P5;
  spieleMelodie(s, M(MEL_N), a + 0.5f, a + 11, 0.45f, t0, t1, 0.65f);
  if (bei(a + 11.6f, t0, t1)) for (int k = 0; k < 6; k++) s.glocke(a + 11.6f + k * 0.15f, 84 + k * 2, 0.4f);  // Licht!
  spieleMelodie(s, M(MEL_BREMEN), a + 18, a + 29.5f, 0.28f, t0, t1, 0.7f);
  a = P6;
  spieleMelodie(s, M(MEL_SPUK), a + 0.4f, a + 19.6f, 0.36f, t0, t1, 0.7f);
  for (int i = 0; i < 4; i++) {  // das Konzert
    float b = a + 20 + i * 1.7f;
    if (bei(b, t0, t1)) iah(s, b);
    if (bei(b + 0.3f, t0, t1)) wau(s, b + 0.3f);
    if (bei(b + 0.8f, t0, t1)) miau(s, b + 0.8f);
    if (bei(b + 1.0f, t0, t1)) kikeriki(s, b + 1.0f);
  }
  if (bei(a + 26.6f, t0, t1)) for (int k = 0; k < 14; k++) s.glocke(a + 26.6f + k * 0.05f, 96 - k * 2, 0.5f);  // klirr
  spieleMelodie(s, M(MEL_JAGD), a + 30, a + 38, 0.16f, t0, t1, 0.85f);
  spieleMelodie(s, M(MEL_BREMEN), a + 39, a + 47.5f, 0.26f, t0, t1, 0.8f);
  a = P7;
  spieleMelodie(s, M(MEL_SPUK), a + 0.4f, a + 17.6f, 0.38f, t0, t1, 0.6f);
  for (int i = 0; i < 4; i++) {
    float b = a + 18 + i * 3;
    if (bei(b, t0, t1)) { if (i == 0) miau(s, b); if (i == 1) wau(s, b); if (i == 2) { s.ton(b, 100, 0.3f, 0.002f, 0.3f, SINUS, GERAEUSCH, 50); } if (i == 3) kikeriki(s, b); }
  }
  spieleMelodie(s, M(MEL_JAGD), a + 30, a + 43.5f, 0.15f, t0, t1, 0.8f);
  a = P8;
  spieleMelodie(s, M(MEL_SPUK), a + 0.4f, a + 21.5f, 0.42f, t0, t1, 0.55f);
  spieleMelodie(s, M(MEL_BREMEN), a + 22.4f, a + 37, 0.24f, t0, t1, 0.9f);
  endeKlang(s, a + 37.2f, t0, t1);
}

// =====================================================================
// Startbild, Vorschau, Erkennungszeichen
// =====================================================================
static void titelSzene(Gfx &g, float t) {
  landweg(g, t, 0);
  float m = fmodf(t, 4) < 2 ? 1 : 0;
  pyramide(g, 150, 222, t, m);
}
static void titelbild(Gfx &g, float t) { titelSzene(g, t); titelRahmen(g, t, "Die Bremer Stadtmusikanten", 216, SCHRIFT_B); }
static void vorschau(Gfx &g, float t) { titelSzene(g, t); }
static void symbol(Gfx &g, float x, float y, float t) {
  float z = 0.48f;
  g.ansicht(x - 60 * z, y - 214 * z + 4, z);
  pyramide(g, 50, 214, t, fmodf(t, 4) < 2 ? 1 : 0);
  g.kameraAus();
}

const Maerchen BREMEN = {"Die Bremer Stadtmusikanten", PDAUER, FARBE(0x6E8E4E), bild, ton, titelbild, vorschau, symbol,
                         LIED(MEL_BREMEN, 0.28f), nullptr};

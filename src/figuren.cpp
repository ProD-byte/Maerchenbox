// Gemeinsame Bausteine fuer alle Maerchen (siehe figuren.h).
#include "figuren.h"
#include "schriften.h"

// =====================================================================
// Figuren
// =====================================================================

// Arm vom Schultergelenk; gibt die Handposition zurueck
void arm(Gfx &g, float sx, float sy, float s, float dir, float winkel, float &hx, float &hy, Farbe aermel, Farbe haut) {
  float l = 15 * s;
  hx = sx + sinf(winkel) * l * dir;
  hy = sy + cosf(winkel) * l;
  g.kontur(1.4f * s, OUT);
  g.linie(sx, sy, hx, hy, 6.5f * s, aermel);
  g.kreis(hx, hy, 3.8f * s, haut);
  g.konturAus();
}

// Das Schneiderlein. (x, y) = zwischen den Fuessen am Boden, s = Groesse (1 = ca. 95 px), dir = +1 schaut rechts
// teil: 0 = alles, 1 = ohne Arme, 2 = nur die Arme (zum Zeichnen vor einem Tisch)
void schneiderlein(Gfx &g, float x, float y, float s, float dir, const Pose &p, float *handX,
                          float *handY, int teil) {
  auto PX = [&](float dx) { return x + dx * s * dir; };
  auto PY = [&](float dy) { return y + dy * s; };
  static const Aussehen STANDARD;
  const Aussehen &L = p.aussehen ? *p.aussehen : STANDARD;
  float hx, hy;
  if (teil == 2) {
    arm(g, PX(-8), PY(-38), s, dir, p.armH * -1, hx, hy, L.hemd, L.haut);
  } else {
  // Beine
  float sw = p.lauf >= 0 ? sinf(p.lauf * 6.2831853f) * 7 : 0;
  g.kontur(1.4f * s, OUT);
  g.linie(PX(-4), PY(-22), PX(-4 - sw), PY(-5), 6 * s, L.hose);
  g.linie(PX(4), PY(-22), PX(4 + sw), PY(-5), 6 * s, L.hose);
  g.ellipse(PX(-3 - sw), PY(-3), 6 * s, 3.5f * s, SCHUH);
  g.ellipse(PX(5 + sw), PY(-3), 6 * s, 3.5f * s, SCHUH);
  // hinterer Arm
  g.konturAus();
  if (teil == 0) arm(g, PX(-8), PY(-38), s, dir, p.armH * -1, hx, hy, L.hemd, L.haut);
  if (p.ranzen) {
    g.kontur(1.3f * s, OUT);
    g.ellipse(PX(-12), PY(-33), 7 * s, 9 * s, FARBE(0xB0703A));
    g.konturAus();
    g.linie(PX(-12), PY(-42), PX(-12), PY(-38), 2 * s, FARBE(0x7E4620));
  }
  // Koerper
  g.kontur(1.4f * s, OUT);
  g.ellipse(PX(0), PY(-31), 12.5f * s, 14 * s, L.weste);
  g.konturAus();
  g.ellipse(PX(2), PY(-33), 4 * s, 9 * s, L.hemd);
  g.kreis(PX(1), PY(-36), 1.3f * s, GOLD);
  g.kreis(PX(1), PY(-30), 1.3f * s, GOLD);
  if (p.guertel) {
    g.rechteck(PX(-12.5f) < PX(12.5f) ? PX(-12.5f) : PX(12.5f), PY(-25), 25 * s, 5 * s, FARBE(0x6B3B1C));
    g.rundRechteck(PX(-3), PY(-26), 6 * s, 7 * s, 1.5f * s, GOLD);
  }
  // Kopf
  float kx = PX(1), ky = PY(-60);
  g.kontur(1.5f * s, OUT);
  g.ellipse(PX(-4), PY(-64), 16 * s, 14 * s, L.haar);
  g.kreis(kx, ky, 17 * s, L.haut);
  g.konturAus();
  g.ellipse(PX(-13), PY(-60), 4 * s, 5 * s, L.haut);  // Ohr
  // Augen
  for (int i = 0; i < 2; i++) {
    float ex = PX(i ? 9.5f : -1.5f), ey = PY(-61);
    if (p.zu) {
      g.bogen(ex, ey + 1 * s, 3.2f * s, 3.5f, 5.9f, 1.8f * s, OUT);
    } else {
      g.ellipse(ex, ey, 3.2f * s, 4.2f * s, SCHWARZ);
      g.kreis(ex + (p.blick * 1.2f + 1) * s * dir, ey - 1.5f * s, 1.3f * s, WEISS);
    }
    if (p.boese > 0) {
      float b = p.boese;
      float inn = i ? -1 : 1;  // Innenseite tiefer
      g.linie(ex - 3.5f * s, ey - (7 - b * (inn > 0 ? -1.5f : 1.5f)) * s, ex + 3.5f * s,
              ey - (7 + b * (inn > 0 ? -1.5f : 1.5f)) * s, 1.8f * s, OUT);
    }
  }
  if (L.wangen) {
    g.kreis(PX(-4), PY(-53), 3.5f * s, WANGE, 140);
    g.kreis(PX(14), PY(-53), 3 * s, WANGE, 140);
  }
  // Mund
  if (p.mund > 0.05f) {
    g.ellipse(PX(5), PY(-50), 3.5f * s, (1.5f + p.mund * 3) * s, FARBE(0x8A2A2A));
  } else {
    g.bogen(PX(5), PY(-53), 4.5f * s, 0.5f, 2.6f, 1.8f * s, OUT);
  }
  if (L.halstuch) {
    float ht[] = {PX(-9), PY(-45), PX(11), PY(-45), PX(1), PY(-36)};
    g.polygon(ht, 3, L.halstuch);
  }
  if (L.muetze == 0) {  // Zipfelmuetze mit Feder
    g.kontur(1.5f * s, OUT);
    float m[] = {PX(-16), PY(-68), PX(16), PY(-70), PX(9), PY(-84), PX(-4), PY(-93), PX(-16), PY(-99), PX(-10), PY(-86)};
    g.polygon(m, 6, L.hut);
    g.konturAus();
    g.ellipse(PX(0), PY(-69), 17.5f * s, 4 * s, L.hutD);
    g.kreis(PX(-16), PY(-99), 3 * s, GOLD);
    g.kurve(PX(8), PY(-72), PX(16), PY(-90), PX(24), PY(-96), 3 * s, L.feder);
  } else if (L.muetze == 1) {  // Strubbelhaare
    g.kontur(1.3f * s, OUT);
    g.ellipse(PX(0), PY(-72), 16 * s, 7 * s, L.haar);
    g.konturAus();
    // drei geschwungene Locken
    g.kurve(PX(-10), PY(-74), PX(-14), PY(-84), PX(-6), PY(-86), 4.5f * s, L.haar);
    g.kurve(PX(-1), PY(-76), PX(0), PY(-88), PX(8), PY(-86), 4.5f * s, L.haar);
    g.kurve(PX(8), PY(-74), PX(14), PY(-82), PX(18), PY(-78), 4 * s, L.haar);
  } else if (L.muetze == 3) {  // Krone (Prinz)
    g.kontur(1.3f * s, OUT);
    g.ellipse(PX(-2), PY(-70), 16 * s, 8 * s, L.haar);
    float k[] = {PX(-12), PY(-72), PX(-12), PY(-86), PX(-6), PY(-79), PX(0), PY(-89), PX(6), PY(-79), PX(12), PY(-86), PX(12), PY(-72)};
    g.polygon(k, 7, GOLD);
    g.konturAus();
    g.kreis(PX(0), PY(-77), 2 * s, FARBE(0xE8473D));
  } else {  // Schlafmuetze, haengt nach hinten
    g.kontur(1.5f * s, OUT);
    float m[] = {PX(-16), PY(-66), PX(16), PY(-70), PX(6), PY(-86), PX(-14), PY(-88), PX(-26), PY(-74), PX(-14), PY(-80)};
    g.polygon(m, 6, L.hut);
    g.konturAus();
    g.ellipse(PX(0), PY(-68), 17.5f * s, 4 * s, WEISS);
    g.kreis(PX(-26), PY(-73), 4 * s, WEISS);
  }
  }
  if (teil == 1) return;
  // vorderer Arm
  arm(g, PX(9), PY(-38), s, dir, p.armV, hx, hy, L.hemd, L.haut);
  if (p.tuch > 0) {
    float q[] = {hx, hy, hx + 14 * s * dir, hy - 10 * s, hx + 20 * s * dir, hy + 4 * s, hx + 4 * s * dir, hy + 12 * s};
    g.polygon(q, 4, FARBE(0xE0533F));
    g.kreis(hx + 9 * s * dir, hy, 1.8f * s, WEISS);
    g.kreis(hx + 13 * s * dir, hy + 4 * s, 1.8f * s, WEISS);
  }
  if (handX) *handX = hx;
  if (handY) *handY = hy;
}

// Die Bauersfrau (nur Oberkoerper, schaut zum Fenster herein)
void bauersfrau(Gfx &g, float x, float y, float winke) {
  g.kontur(1.4f, OUT);
  g.ellipse(x, y + 26, 18, 16, FARBE(0xC8507A));      // Kleid
  g.ellipse(x, y + 30, 10, 12, WEISS);               // Schuerze
  g.kreis(x, y, 14, HAUT);
  g.konturAus();
  float tuch[] = {x - 16, y - 2, x - 12, y - 14, x, y - 18, x + 12, y - 14, x + 16, y - 2, x + 10, y - 8, x - 10, y - 8};
  g.polygon(tuch, 7, FARBE(0xE8473D));
  g.kreis(x - 6, y - 12, 1.5f, WEISS);
  g.kreis(x + 5, y - 13, 1.5f, WEISS);
  g.ellipse(x - 5, y + 1, 2.4f, 3, SCHWARZ);
  g.ellipse(x + 5, y + 1, 2.4f, 3, SCHWARZ);
  g.kreis(x - 9, y + 6, 3, WANGE, 140);
  g.kreis(x + 9, y + 6, 3, WANGE, 140);
  g.bogen(x, y + 4, 4, 0.5f, 2.6f, 1.6f, OUT);
  if (winke > 0) {
    float a = 2.6f + sinf(winke * 12) * 0.4f;
    float hx = x + 16 + sinf(a) * 14, hy = y + 18 + cosf(a) * 14;
    g.kontur(1.3f, OUT);
    g.linie(x + 14, y + 20, hx, hy, 6, FARBE(0xC8507A));
    g.kreis(hx, hy, 3.5f, HAUT);
    g.konturAus();
  }
}

void musglas(Gfx &g, float x, float y, float s) {
  g.kontur(1.3f * s, OUT);
  g.rundRechteck(x - 8 * s, y - 16 * s, 16 * s, 16 * s, 4 * s, MUS);
  g.rundRechteck(x - 9 * s, y - 20 * s, 18 * s, 5 * s, 2 * s, WEISS);
  g.konturAus();
  g.kreis(x - 4 * s, y - 17.5f * s, 1.5f * s, MUS);
  g.kreis(x + 3 * s, y - 17.5f * s, 1.5f * s, MUS);
  g.rundRechteck(x - 5 * s, y - 11 * s, 10 * s, 6 * s, 1.5f * s, FARBE(0xFFF1C9));
  g.ellipse(x - 4 * s, y - 13 * s, 1.5f * s, 3 * s, WEISS, 160);
}

void brot(Gfx &g, float x, float y, float mus) {
  g.kontur(1.3f, OUT);
  g.rundRechteck(x - 15, y - 8, 30, 9, 4, FARBE(0xC98A45));
  g.konturAus();
  g.rundRechteck(x - 13, y - 7, 26, 6, 3, FARBE(0xF3D29A));
  if (mus > 0) {
    float w = 26 * mus;
    g.rundRechteck(x - 13, y - 8.5f, w, 5, 2.5f, MUS);
    if (mus > 0.6f) g.ellipse(x - 13 + w * 0.6f, y - 3, 1.6f, 2.5f, MUS);
    g.ellipse(x - 8, y - 7.5f, 3, 1, WEISS, 120);
  }
}

// Fliege; liegt = 1 liegt auf dem Ruecken (benommen)
void fliege(Gfx &g, float x, float y, float t, float liegt, int nr) {
  if (liegt > 0.5f) {
    for (int i = 0; i < 3; i++) g.linie(x - 3 + i * 3, y - 2, x - 4 + i * 3.5f, y - 6 + sinf(t * 9 + i) * 0.6f, 1, SCHWARZ);
    g.ellipse(x, y, 5, 3.5f, FARBE(0x34345A));
    g.ellipse(x + 4, y + 1, 3, 3, FARBE(0x34345A));
    g.linie(x + 3, y, x + 5.5f, y + 2, 0.9f, WEISS);
    g.linie(x + 5.5f, y, x + 3, y + 2, 0.9f, WEISS);
    for (int i = 0; i < 3; i++) {  // Sternchen um den Kopf
      float a = t * 4 + i * 2.09f + nr;
      g.stern(x + 2 + cosf(a) * 7, y - 7 + sinf(a) * 2.5f, 2.2f, 1, 5, a, GOLD);
    }
    return;
  }
  float fl = sinf(t * 70 + nr) * 0.5f + 0.5f;
  g.ellipse(x - 2, y - 4, 4, 2 + fl * 3, FARBE(0xDDF3FF), 170, -0.5f);
  g.ellipse(x + 2, y - 4, 4, 2 + (1 - fl) * 3, FARBE(0xDDF3FF), 170, 0.5f);
  g.ellipse(x, y, 4.5f, 3.5f, FARBE(0x34345A));
  g.kreis(x + 3.5f, y - 1, 2.6f, FARBE(0x34345A));
  g.kreis(x + 4.3f, y - 1.6f, 1.2f, WEISS);
}

void notenzeichen(Gfx &g, float x, float y, Farbe c, uint8_t a) {
  g.ellipse(x, y, 3.2f, 2.4f, c, a, -0.4f);
  g.linie(x + 2.6f, y - 1, x + 2.6f, y - 11, 1.4f, c, a);
  g.linie(x + 2.6f, y - 11, x + 6.5f, y - 8, 1.6f, c, a);
}

void herz(Gfx &g, float x, float y, float r, Farbe c, uint8_t a) {
  g.kreis(x - r * 0.5f, y, r * 0.55f, c, a);
  g.kreis(x + r * 0.5f, y, r * 0.55f, c, a);
  float q[] = {x - r * 1.03f, y + r * 0.15f, x + r * 1.03f, y + r * 0.15f, x, y + r * 1.2f};
  g.polygon(q, 3, c, a);
}

void wolke(Gfx &g, float x, float y, float s, uint8_t a) {
  const Farbe schatten = FARBE(0xD8E4F2);
  g.rundRechteck(x - 4 * s, y + 1 * s, 30 * s, 9 * s, 4.5f * s, schatten, a);
  g.kreis(x + 1 * s, y + 1 * s, 9 * s, schatten, a);
  g.kreis(x + 21 * s, y + 1 * s, 8 * s, schatten, a);
  g.kreis(x, y - 1 * s, 8.5f * s, WEISS, a);
  g.kreis(x + 10 * s, y - 5 * s, 11 * s, WEISS, a);
  g.kreis(x + 21 * s, y - 1 * s, 7.5f * s, WEISS, a);
  g.kreis(x + 7 * s, y - 9 * s, 4 * s, FARBE(0xFFFFFF), (uint8_t)(a * 0.8f));
}

void sonne(Gfx &g, float x, float y, float r, float t) {
  for (int i = 0; i < 10; i++) {
    float a = t * 0.4f + i * 0.628f;
    g.linie(x + cosf(a) * r * 1.25f, y + sinf(a) * r * 1.25f, x + cosf(a) * r * 1.6f, y + sinf(a) * r * 1.6f,
            r * 0.22f, FARBE(0xFFC93A));
  }
  g.kreis(x, y, r, FARBE(0xFFD93D));
  g.ellipse(x - r * 0.35f, y - r * 0.1f, r * 0.1f, r * 0.15f, OUT);
  g.ellipse(x + r * 0.35f, y - r * 0.1f, r * 0.1f, r * 0.15f, OUT);
  g.bogen(x, y + r * 0.1f, r * 0.4f, 0.5f, 2.6f, r * 0.1f, OUT);
  g.kreis(x - r * 0.55f, y + r * 0.25f, r * 0.15f, WANGE, 120);
  g.kreis(x + r * 0.55f, y + r * 0.25f, r * 0.15f, WANGE, 120);
}


// =====================================================================
// Kulissen-Bausteine
// =====================================================================
void himmel(Gfx &g, float nacht, float t, float bis) {
  Farbe o = mischeFarbe(FARBE(0x7CCBFF), FARBE(0x101848), nacht);
  Farbe u = mischeFarbe(FARBE(0xD2F0FF), FARBE(0x34306E), nacht);
  g.verlauf(0, bis, o, u);
  if (nacht > 0.2f)
    for (int i = 0; i < 26; i++) {
      float fu = 0.5f + 0.5f * sinf(t * 2.5f + i * 1.9f);
      g.stern(zufall(i + 300) * 284, zufall(i + 400) * 120, 1.6f + fu * 1.6f, 0.7f, 4, 0, WEISS,
              (uint8_t)(255 * klemme((nacht - 0.2f) * 1.6f) * (0.4f + 0.6f * fu)));
    }
}

// Hügelreihe, die beim Laufen mit dem Faktor f verschoben wird
void huegel(Gfx &g, float scroll, float f, float y, float h, float abstand, Farbe c) {
  float off = fmodf(scroll * f, abstand);
  const Farbe hell = mischeFarbe(c, FARBE(0xF6FFD0), 0.3f);
  for (int k = -1; k < 284 / abstand + 2; k++) {
    float x = k * abstand - off;
    g.ellipse(x, y, abstand * 0.75f, h, c);
    g.ellipse(x - abstand * 0.12f, y - h * 0.45f, abstand * 0.4f, h * 0.35f, hell, 110);
  }
}

void laubbaum(Gfx &g, float x, float y, float s, Farbe krone) {
  const Farbe dunkel = mischeFarbe(krone, FARBE(0x1E3A28), 0.35f), hell = mischeFarbe(krone, FARBE(0xF6FFD0), 0.4f);
  const Farbe rinde = FARBE(0x8E5B3A), rindeD = FARBE(0x6A4028);
  g.ellipse(x + 4 * s, y, 26 * s, 5 * s, FARBE(0x1E3A28), 50);  // Schatten am Boden
  // Stamm: unten breiter, mit Wurzeln und Rinde
  g.kontur(1.5f, OUT);
  float st[] = {x - 8 * s, y, x + 8 * s, y, x + 5 * s, y - 42 * s, x - 5 * s, y - 42 * s};
  g.polygon(st, 4, rinde);
  g.konturAus();
  float w1[] = {x - 8 * s, y, x - 15 * s, y + 1 * s, x - 6 * s, y - 8 * s};
  float w2[] = {x + 8 * s, y, x + 14 * s, y + 1 * s, x + 6 * s, y - 7 * s};
  g.polygon(w1, 3, rinde);
  g.polygon(w2, 3, rinde);
  g.rechteck(x + 1 * s, y - 40 * s, 3.5f * s, 38 * s, rindeD, 160);
  g.linie(x - 3 * s, y - 30 * s, x - 2 * s, y - 20 * s, 1.1f * s, rindeD);
  g.linie(x + 2 * s, y - 14 * s, x + 3 * s, y - 6 * s, 1.1f * s, rindeD);
  g.linie(x, y - 36 * s, x - 10 * s, y - 50 * s, 3 * s, rinde);  // Ast
  // Krone: hintere dunkle Lage, vordere Lage, Lichter
  g.kontur(1.5f, OUT);
  g.kreis(x - 18 * s, y - 50 * s, 17 * s, dunkel);
  g.kreis(x + 18 * s, y - 52 * s, 17 * s, dunkel);
  g.kreis(x + 4 * s, y - 74 * s, 19 * s, dunkel);
  g.konturAus();
  g.kreis(x - 14 * s, y - 52 * s, 15 * s, krone);
  g.kreis(x + 14 * s, y - 54 * s, 15 * s, krone);
  g.kreis(x - 4 * s, y - 70 * s, 18 * s, krone);
  g.kreis(x + 8 * s, y - 64 * s, 14 * s, krone);
  g.kreis(x - 8 * s, y - 76 * s, 9 * s, hell, 170);
  g.kreis(x - 18 * s, y - 58 * s, 7 * s, hell, 150);
  g.kreis(x + 10 * s, y - 70 * s, 6 * s, hell, 130);
  g.ellipse(x + 2 * s, y - 42 * s, 22 * s, 5 * s, dunkel, 150);  // Schatten unter der Krone
  if (s > 0.35f)
    for (int i = 0; i < 7; i++)  // einzelne Blaetter
      g.ellipse(x - 20 * s + zufall(i + (int)(x * 3)) * 40 * s, y - 80 * s + zufall(i + 17 + (int)x) * 38 * s, 2.4f * s, 1.4f * s,
                i & 1 ? dunkel : hell, 200, i);
}

void tanne(Gfx &g, float x, float y, float s, Farbe c) {
  const Farbe dunkel = mischeFarbe(c, FARBE(0x0E1E18), 0.4f), hell = mischeFarbe(c, FARBE(0xE8FFD8), 0.3f);
  g.ellipse(x + 3 * s, y, 22 * s, 4 * s, FARBE(0x1E3A28), 50);
  g.rechteck(x - 4 * s, y - 14 * s, 8 * s, 14 * s, FARBE(0x6E4426));
  g.rechteck(x + 1 * s, y - 14 * s, 3 * s, 14 * s, FARBE(0x4E2E18), 160);
  for (int i = 0; i < 4; i++) {
    float w = (31 - i * 6.5f) * s, yy = y - 10 * s - i * 16 * s, h = 28 * s;
    // Zweig-Lage mit gezacktem Rand
    float q[] = {x - w, yy, x - w * 0.6f, yy - 3 * s, x - w * 0.3f, yy + 1 * s, x, yy - 3 * s, x + w * 0.3f, yy + 1 * s,
                 x + w * 0.6f, yy - 3 * s, x + w, yy, x, yy - h};
    g.kontur(1.3f, OUT);
    g.polygon(q, 8, c);
    g.konturAus();
    float sch[] = {x, yy - h, x + w, yy, x + w * 0.6f, yy - 3 * s, x + w * 0.3f, yy + 1 * s, x, yy - 3 * s};
    g.polygon(sch, 5, dunkel, 150);
    g.linie(x - 2 * s, yy - h + 4 * s, x - w * 0.55f, yy - 4 * s, 1.4f * s, hell, 160);
  }
}

void blumen(Gfx &g, float scroll, float y, int n, int seed) {
  for (int i = 0; i < n; i++) {
    float x = fmodf(zufall(i + seed) * 400 - scroll + 4000, 400) - 60;
    float yy = y + zufall(i + seed + 50) * 30;
    Farbe c = i % 3 == 0 ? FARBE(0xFF7FA8) : i % 3 == 1 ? GOLD : WEISS;
    g.linie(x, yy, x, yy - 7, 1.3f, GRAS_D);
    g.ellipse(x + 3, yy - 3, 3, 1.4f, GRAS_D, 255, -0.5f);
    for (int k = 0; k < 5; k++) g.kreis(x + cosf(k * 1.2566f) * 2.4f, yy - 8 + sinf(k * 1.2566f) * 2.4f, 1.8f, c);
    g.kreis(x, yy - 8, 1.3f, i % 3 == 1 ? FARBE(0xE07A2A) : GOLD);
  }
}

void boden(Gfx &g, float y, Farbe c) {
  const Farbe hell = mischeFarbe(c, FARBE(0xF6FFD0), 0.3f), dunkel = mischeFarbe(c, FARBE(0x1E3A28), 0.22f);
  g.verlauf(y, 240, c, dunkel);
  g.rechteck(0, y, 284, 3, hell);
  // Grasbueschel an der Kante und ein paar im Feld
  for (int i = 0; i < 24; i++) {
    float x = i * 12.3f + zufall(i + 900) * 6, h = 3 + zufall(i + 930) * 4;
    float b[] = {x - 2.4f, y + 1, x + 2.4f, y + 1, x + 0.8f, y - h};
    g.polygon(b, 3, i & 1 ? hell : dunkel);
  }
  for (int i = 0; i < 10; i++) {
    float x = zufall(i + 960) * 284, yy = y + 12 + zufall(i + 980) * (230 - y);
    g.linie(x, yy, x - 2, yy - 4, 1, dunkel, 180);
    g.linie(x + 2, yy, x + 3, yy - 4, 1, dunkel, 180);
  }
}

void mond(Gfx &g, float x, float y, float r) {
  g.kreis(x, y, r * 1.5f, FARBE(0xFFF6C8), 40);
  g.kreis(x, y, r, FARBE(0xFFF1B0));
  g.kreis(x + r * 0.35f, y + r * 0.3f, r * 0.18f, FARBE(0xEADB95));
  g.kreis(x - r * 0.4f, y - r * 0.35f, r * 0.12f, FARBE(0xEADB95));
  g.bogen(x - r * 0.3f, y - r * 0.05f, r * 0.15f, 0.3f, 2.8f, r * 0.07f, OUT);  // schlafende Augen
  g.bogen(x + r * 0.25f, y - r * 0.05f, r * 0.15f, 0.3f, 2.8f, r * 0.07f, OUT);
}

void steinbrocken(Gfx &g, float x, float y, float r) {
  g.kontur(1.3f, OUT);
  g.ellipse(x, y, r, r * 0.85f, STEIN);
  g.konturAus();
  g.ellipse(x - r * 0.3f, y - r * 0.3f, r * 0.35f, r * 0.2f, WEISS, 110);
}

void kaese(Gfx &g, float x, float y, float s) {
  g.kontur(1.3f * s, OUT);
  float q[] = {x - 10 * s, y, x + 10 * s, y, x + 10 * s, y - 8 * s, x - 10 * s, y - 14 * s};
  g.polygon(q, 4, FARBE(0xFFD43B));
  g.konturAus();
  g.kreis(x - 4 * s, y - 6 * s, 2 * s, FARBE(0xE8B820));
  g.kreis(x + 4 * s, y - 3 * s, 1.5f * s, FARBE(0xE8B820));
}

void vogel(Gfx &g, float x, float y, float s, float flug, float dir) {
  float w = sinf(flug * 20) * 0.9f;
  g.kontur(1.1f * s, OUT);
  g.ellipse(x - 2 * s * dir, y - 2 * s + w * 3 * s, 5 * s, 3 * s, FARBE(0xF2B705), 255, w * dir);
  g.kreis(x, y, 6 * s, FARBE(0xFFD43B));
  g.konturAus();
  float b[] = {x + 5 * s * dir, y - 1 * s, x + 10 * s * dir, y + 1 * s, x + 5 * s * dir, y + 2.5f * s};
  g.polygon(b, 3, FARBE(0xF28A1E));
  g.kreis(x + 2.5f * s * dir, y - 2 * s, 1.3f * s, SCHWARZ);
  g.kreis(x - 1 * s * dir, y + 2 * s, 1.6f * s, WANGE, 140);
}

void tropfen(Gfx &g, float x, float y, float r, Farbe c, uint8_t a) {
  g.kreis(x, y, r, c, a);
  float q[] = {x - r * 0.85f, y - r * 0.4f, x + r * 0.85f, y - r * 0.4f, x, y - r * 2.4f};
  g.polygon(q, 3, c, a);
}

void staubwolke(Gfx &g, float x, float y, float s, float t, uint8_t a) {
  for (int i = 0; i < 7; i++) {
    float an = i * 0.9f + t * 6;
    g.kreis(x + cosf(an) * 18 * s, y + sinf(an * 1.3f) * 10 * s, (12 + 4 * sinf(t * 9 + i)) * s, FARBE(0xEFE6D6), a);
  }
}

void fragezeichen(Gfx &g, float x, float y, float s, uint8_t a) {
  g.kontur(1, OUT);
  g.kreis(x, y - 10 * s, 9 * s, WEISS, a);
  g.konturAus();
  g.text(SCHRIFT_M, "?", x, y - 3 * s, OUT, 1, a);
}

void zzz(Gfx &g, float x, float y, float t, uint8_t a) {
  for (int i = 0; i < 3; i++) {
    float ph = fmodf(t * 0.5f + i / 3.0f, 1);
    g.text(i == 2 ? SCHRIFT_M : SCHRIFT_K, "z", x + ph * 22 + sinf(ph * 8) * 3, y - ph * 40, WEISS,
           1, (uint8_t)(a * huepf(ph)));
  }
}

// =====================================================================
// Der Riese
// =====================================================================

static Farbe riesenHaut = RIESE_HAUT;  // Hautfarbe des gerade gezeichneten Riesen

void riesenArm(Gfx &g, float sx, float sy, float s, float dir, float w, Farbe hemd, float *hx,
                      float *hy) {
  float x = sx + sinf(w) * 46 * s * dir, y = sy + cosf(w) * 46 * s;
  g.kontur(1.8f, OUT);
  g.linie(sx, sy, x, y, 17 * s, hemd);
  g.kreis(x, y, 10 * s, riesenHaut);
  g.konturAus();
  if (hx) *hx = x;
  if (hy) *hy = y;
}

// (x, y) = Boden in der Mitte, s = 1: etwa 165 px hoch (stehend)
void riese(Gfx &g, float x, float y, float s, float dir, const RiesePose &p, float *handX,
                  float *handY) {
  auto PX = [&](float dx) { return x + dx * s * dir; };
  auto PY = [&](float dy) { return y + dy * s; };
  // 0 gruen, 1 blau, 2 blasser Geist mit weissem Bart
  Farbe hemd = p.farbe == 2 ? FARBE(0xB9C3DA) : p.farbe ? FARBE(0x5C7FB8) : FARBE(0x6E9A3C);
  Farbe hose = p.farbe == 2 ? FARBE(0x8D98B8) : p.farbe ? FARBE(0x4A3B6B) : FARBE(0x6B4A2E);
  Farbe bart = p.farbe == 2 ? FARBE(0xF4F6FA) : p.farbe ? FARBE(0x5B4636) : RIESE_BART;
  riesenHaut = p.farbe == 2 ? FARBE(0xDCE2EE) : RIESE_HAUT;
  float koerperY = p.sitzt ? -62 : -82;
  // Beine
  g.kontur(1.8f, OUT);
  if (p.sitzt) {
    g.linie(PX(-10), PY(-32), PX(30), PY(-34), 22 * s, hose);
    g.linie(PX(30), PY(-34), PX(36), PY(-6), 20 * s, hose);
    g.ellipse(PX(42), PY(-5), 17 * s, 9 * s, SCHUH);
  } else {
    float sw = p.lauf >= 0 ? sinf(p.lauf * 6.2831853f) * 14 : 0;
    g.linie(PX(-12), PY(-50), PX(-12 - sw), PY(-10), 20 * s, hose);
    g.linie(PX(12), PY(-50), PX(12 + sw), PY(-10), 20 * s, hose);
    g.ellipse(PX(-10 - sw), PY(-6), 17 * s, 9 * s, SCHUH);
    g.ellipse(PX(16 + sw), PY(-6), 17 * s, 9 * s, SCHUH);
  }
  g.konturAus();
  riesenArm(g, PX(-26), PY(koerperY - 22), s, dir, -p.armH, hemd);
  // Koerper
  g.kontur(1.8f, OUT);
  g.ellipse(PX(0), PY(koerperY), 36 * s, 40 * s, hemd);
  g.konturAus();
  g.rechteck(PX(-36) < PX(36) ? PX(-36) : PX(36), PY(koerperY + 18), 72 * s, 9 * s, FARBE(0x5A3A1E));
  g.rundRechteck(PX(-7), PY(koerperY + 16), 14 * s, 13 * s, 3 * s, FARBE(0xC9C9C9));
  for (int i = 0; i < 3; i++) g.kreis(PX(12 - i * 10), PY(koerperY - 22 + i * 4), 3 * s, FARBE(0x557A2E), 160);  // Flicken
  // Kopf
  float kx = PX(4), ky = PY(koerperY - 58);
  float kw = p.kopf;
  auto KX = [&](float dx, float dy) { return kx + (dx * cosf(kw) - dy * sinf(kw)) * s * dir; };
  auto KY = [&](float dx, float dy) { return ky + (dx * sinf(kw) + dy * cosf(kw)) * s; };
  g.kontur(1.8f, OUT);
  g.kreis(KX(0, 0), KY(0, 0), 28 * s, riesenHaut);
  g.ellipse(KX(0, 20), KY(0, 20), 26 * s, 18 * s, bart);
  g.konturAus();
  // Haare
  for (int i = 0; i < 5; i++) g.kreis(KX(-18 + i * 9, -24 + (i == 2) * -3), KY(-18 + i * 9, -24), 9 * s, bart);
  g.ellipse(KX(-26, 0), KY(-26, 0), 6 * s, 8 * s, riesenHaut);  // Ohr
  // Augen
  for (int i = 0; i < 2; i++) {
    float ex = KX(i ? 14 : -4, -4), ey = KY(i ? 14 : -4, -4);
    if (p.zu) g.bogen(ex, ey, 4.5f * s, 0.4f, 2.7f, 2.2f * s, OUT);
    else {
      float r = 4 + p.staunt * 2.5f;
      g.kreis(ex, ey, (r + 1.5f) * s, WEISS);
      g.kreis(ex + 1 * s * dir, ey, (r - 1) * s, SCHWARZ);
      g.kreis(ex + 2 * s * dir, ey - 1.5f * s, 1.3f * s, WEISS);
    }
    float b = p.boese * (i ? -1 : 1);
    g.linie(ex - 6 * s, ey - (9 + p.staunt * 4 - b * 3) * s, ex + 6 * s, ey - (9 + p.staunt * 4 + b * 3) * s, 4 * s, bart);
  }
  g.kontur(1.5f, OUT);
  g.ellipse(KX(8, 6), KY(8, 6), 7 * s, 6 * s, FARBE(0xE8957A));  // Nase
  g.konturAus();
  if (p.mund > 0.05f) g.ellipse(KX(6, 16), KY(6, 16), 7 * s, (2 + p.mund * 6) * s, FARBE(0x7A2A2A));
  else g.bogen(KX(6, 10), KY(6, 10), 8 * s, 0.5f, 2.6f, 2.5f * s, OUT);
  float hx, hy;
  riesenArm(g, PX(26), PY(koerperY - 22), s, dir, p.armV, hemd, &hx, &hy);
  if (handX) *handX = hx;
  if (handY) *handY = hy;
}

// =====================================================================
// Einhorn, Wildschwein, Koenig, Prinzessin
// =====================================================================
void einhorn(Gfx &g, float x, float y, float s, float lauf, float dir, float senken, float zappel, bool pferd) {
  auto PX = [&](float dx) { return x + dx * s * dir; };
  auto PY = [&](float dy) { return y + dy * s; };
  const Farbe fell = FARBE(0xFFFFFF), schatten = FARBE(0xE6E1F5);
  const Farbe regenbogen[] = {FARBE(0xFF6B8A), FARBE(0xFFB347), FARBE(0xFFE066), FARBE(0x7ED957), FARBE(0x6EC6FF), FARBE(0xB28DFF)};
  const Farbe grau[] = {FARBE(0xE6E6EE), FARBE(0xD6D8E2), FARBE(0xE6E6EE), FARBE(0xD6D8E2), FARBE(0xE6E6EE), FARBE(0xD6D8E2)};
  const Farbe *regen = pferd ? grau : regenbogen;
  // Schweif
  for (int i = 0; i < 4; i++) g.kurve(PX(-30), PY(-46), PX(-46), PY(-40 + i * 4), PX(-44 + sinf(lauf * 6 + i) * 4), PY(-20 + i * 3), 5 * s, regen[i]);
  // Beine (Galopp)
  g.kontur(1.4f, OUT);
  for (int i = 0; i < 4; i++) {
    float bx = i < 2 ? -20 + i * 8 : 16 + (i - 2) * 8;
    float sw = lauf >= 0 ? sinf(lauf * 6.2831853f + i * 1.6f) * 10 : sinf(zappel * 30 + i) * 6 * (zappel > 0);
    g.linie(PX(bx), PY(-34), PX(bx + sw), PY(-4), 6 * s, i & 1 ? fell : schatten);
    g.ellipse(PX(bx + sw), PY(-3), 4 * s, 3 * s, FARBE(0xC9B8F0));
  }
  g.ellipse(PX(0), PY(-42), 32 * s, 16 * s, fell);
  // Hals und Kopf
  float hk = senken;  // 0 = Kopf hoch, 1 = gesenkt (Angriff)
  float kx = PX(lerp(32, 40, hk)), ky = PY(lerp(-74, -50, hk));
  float hals[] = {PX(18), PY(-52), PX(26), PY(-34), kx + (-2) * s * dir, ky + 6 * s, kx - 10 * s * dir, ky - 2 * s};
  g.polygon(hals, 4, fell);
  g.ellipse(kx, ky, 14 * s, 10 * s, fell, 255, -0.3f * dir + hk * 0.6f * dir);
  g.konturAus();
  g.ellipse(kx + 9 * s * dir, ky + 3 * s, 5 * s, 4 * s, FARBE(0xFFD1E0));
  g.kreis(kx + 1 * s * dir, ky - 3 * s, 2.6f * s, SCHWARZ);
  g.kreis(kx + 2 * s * dir, ky - 4 * s, 1 * s, WEISS);
  // Horn
  if (!pferd) {
  float hornW = lerp(-1.0f, -0.2f, hk);
  float hx = kx + cosf(hornW) * 30 * s * dir, hy = ky + sinf(hornW) * 30 * s - 4 * s;
  float horn[] = {kx + 2 * s * dir, ky - 9 * s, kx + 8 * s * dir, ky - 5 * s, hx, hy};
  g.kontur(1.2f, OUT);
  g.polygon(horn, 3, GOLD);
  g.konturAus();
  }
  // Maehne
  for (int i = 0; i < 6; i++) {
    float u = i / 5.0f;
    g.kreis(lerp(PX(16), kx - 8 * s * dir, u), lerp(PY(-56), ky - 8 * s, u) - 2 * s, 6 * s, regen[i]);
  }
}

void wildschwein(Gfx &g, float x, float y, float s, float lauf, float dir, float wut) {
  auto PX = [&](float dx) { return x + dx * s * dir; };
  auto PY = [&](float dy) { return y + dy * s; };
  const Farbe fell = FARBE(0x7A5236), fell_d = FARBE(0x5E3E28);
  g.kontur(1.4f, OUT);
  for (int i = 0; i < 4; i++) {
    float bx = i < 2 ? -18 + i * 9 : 12 + (i - 2) * 9;
    float sw = lauf >= 0 ? sinf(lauf * 6.2831853f + i * 1.7f) * 7 : 0;
    g.linie(PX(bx), PY(-16), PX(bx + sw), PY(-3), 6 * s, fell_d);
  }
  // Borsten auf dem Ruecken
  float b[24];
  for (int i = 0; i < 6; i++) {
    b[4 * i] = PX(-22 + i * 8); b[4 * i + 1] = PY(-34 + fabsf(i - 2.5f) * 1.5f);
    b[4 * i + 2] = PX(-18 + i * 8); b[4 * i + 3] = PY(-44 + fabsf(i - 2.5f) * 2);
  }
  g.polygon(b, 12, fell_d);
  g.ellipse(PX(0), PY(-25), 28 * s, 16 * s, fell);
  g.kreis(PX(24), PY(-26), 13 * s, fell);
  g.konturAus();
  float ohr[] = {PX(18), PY(-36), PX(24), PY(-48), PX(28), PY(-36)};
  g.polygon(ohr, 3, fell_d);
  g.kontur(1.2f, OUT);
  g.ellipse(PX(36), PY(-22), 6 * s, 7 * s, FARBE(0xE8A0A0));
  g.konturAus();
  g.kreis(PX(37), PY(-24), 1.3f * s, OUT);
  g.kreis(PX(37), PY(-19), 1.3f * s, OUT);
  // Hauer
  float h1[] = {PX(31), PY(-15), PX(34), PY(-15), PX(30), PY(-25)};
  g.polygon(h1, 3, WEISS);
  // Auge mit boeser Braue
  g.kreis(PX(26), PY(-30), 2.6f * s, SCHWARZ);
  g.linie(PX(21), PY(-35 + wut), PX(31), PY(-32 - wut * 2), 2 * s, OUT);
  // Ringelschwanz
  g.bogen(PX(-30), PY(-30), 4 * s, 0, 4.5f, 1.8f * s, fell_d);
}

void koenig(Gfx &g, float x, float y, float s, float dir, float winke) {
  auto PX = [&](float dx) { return x + dx * s * dir; };
  auto PY = [&](float dy) { return y + dy * s; };
  g.kontur(1.4f * s, OUT);
  float mantel[] = {PX(-18), PY(0), PX(18), PY(0), PX(12), PY(-46), PX(-12), PY(-46)};
  g.polygon(mantel, 4, FARBE(0xC0392B));
  g.konturAus();
  g.rechteck(PX(-18) < PX(18) ? PX(-18) : PX(18), PY(-6), 36 * s, 6 * s, WEISS);
  g.ellipse(PX(0), PY(-44), 15 * s, 5 * s, WEISS);
  g.kontur(1.5f * s, OUT);
  g.kreis(PX(0), PY(-62), 15 * s, HAUT);
  g.konturAus();
  g.ellipse(PX(0), PY(-52), 12 * s, 9 * s, WEISS);  // Bart
  g.kreis(PX(-5), PY(-64), 2.2f * s, SCHWARZ);
  g.kreis(PX(6), PY(-64), 2.2f * s, SCHWARZ);
  g.kreis(PX(1), PY(-58), 3 * s, WANGE);
  float k[] = {PX(-12), PY(-74), PX(-12), PY(-88), PX(-6), PY(-80), PX(0), PY(-90), PX(6), PY(-80), PX(12), PY(-88), PX(12), PY(-74)};
  g.kontur(1.2f * s, OUT);
  g.polygon(k, 7, GOLD);
  g.konturAus();
  g.kreis(PX(0), PY(-79), 2 * s, FARBE(0xE8473D));
  float a = winke > 0 ? 2.5f + sinf(winke * 10) * 0.4f : 0.4f;
  g.kontur(1.3f * s, OUT);
  g.linie(PX(10), PY(-38), PX(10) + sinf(a) * 15 * s * dir, PY(-38) + cosf(a) * 15 * s, 7 * s, FARBE(0xC0392B));
  g.kreis(PX(10) + sinf(a) * 15 * s * dir, PY(-38) + cosf(a) * 15 * s, 3.5f * s, HAUT);
  g.konturAus();
}

void prinzessin(Gfx &g, float x, float y, float s, float dir, float winke) {
  auto PX = [&](float dx) { return x + dx * s * dir; };
  auto PY = [&](float dy) { return y + dy * s; };
  g.kontur(1.4f * s, OUT);
  g.ellipse(PX(-2), PY(-56), 16 * s, 22 * s, FARBE(0xF2C14E));  // Haare hinten
  float kleid[] = {PX(-20), PY(0), PX(20), PY(0), PX(8), PY(-40), PX(-8), PY(-40)};
  g.polygon(kleid, 4, FARBE(0xFF8FC0));
  g.konturAus();
  g.ellipse(PX(0), PY(-8), 20 * s, 5 * s, FARBE(0xFF6FAE));
  g.kontur(1.5f * s, OUT);
  g.kreis(PX(0), PY(-60), 14 * s, HAUT);
  g.konturAus();
  g.ellipse(PX(-1), PY(-71), 14 * s, 6 * s, FARBE(0xF2C14E));
  g.kreis(PX(-4), PY(-61), 2.2f * s, SCHWARZ);
  g.kreis(PX(6), PY(-61), 2.2f * s, SCHWARZ);
  g.kreis(PX(-8), PY(-55), 3 * s, WANGE, 150);
  g.kreis(PX(10), PY(-55), 3 * s, WANGE, 150);
  g.bogen(PX(1), PY(-57), 4 * s, 0.5f, 2.6f, 1.5f * s, OUT);
  float k[] = {PX(-8), PY(-74), PX(-8), PY(-84), PX(-3), PY(-78), PX(0), PY(-86), PX(3), PY(-78), PX(8), PY(-84), PX(8), PY(-74)};
  g.polygon(k, 7, GOLD);
  float a = winke > 0 ? 2.5f + sinf(winke * 10) * 0.4f : 0.5f;
  g.kontur(1.3f * s, OUT);
  g.linie(PX(8), PY(-38), PX(8) + sinf(a) * 14 * s * dir, PY(-38) + cosf(a) * 14 * s, 5 * s, FARBE(0xFF8FC0));
  g.kreis(PX(8) + sinf(a) * 14 * s * dir, PY(-38) + cosf(a) * 14 * s, 3 * s, HAUT);
  g.konturAus();
}

void kapelle(Gfx &g, float x, float y, float tuer, float wackel) {
  float w = wackel;
  g.kontur(1.8f, OUT);
  g.rechteck(x - 46 + w, y - 76, 92, 76, FARBE(0xFFF3DD));
  float dach[] = {x - 56 + w, y - 74, x + w, y - 118, x + 56 + w, y - 74};
  g.polygon(dach, 3, FARBE(0x5B7FC7));
  g.rechteck(x - 12 + w, y - 148, 24, 34, FARBE(0xFFF3DD));
  float turm[] = {x - 16 + w, y - 146, x + w, y - 172, x + 16 + w, y - 146};
  g.polygon(turm, 3, FARBE(0x5B7FC7));
  g.konturAus();
  g.kreis(x + w, y - 134, 6, GOLD);
  // Tuer (offen: dunkel, zu: Holz)
  g.rundRechteck(x - 14 + w, y - 44, 28, 44, 12, FARBE(0x3A2A40));
  if (tuer > 0) g.rundRechteck(x - 14 + w, y - 44, 28 * tuer, 44, 12 * tuer, HOLZ);
  // Fenster rechts oben
  g.kontur(1.4f, OUT);
  g.kreis(x + 30 + w, y - 56, 9, FARBE(0x8FC8FF));
  g.konturAus();
  g.linie(x + 30 + w, y - 65, x + 30 + w, y - 47, 1.5f, OUT);
  g.linie(x + 21 + w, y - 56, x + 39 + w, y - 56, 1.5f, OUT);
}

void schloss(Gfx &g, float x, float y, float t, float nacht) {
  Farbe mauer = mischeFarbe(FARBE(0xF4E3C3), FARBE(0x8E86B8), nacht * 0.6f);
  Farbe dach = mischeFarbe(FARBE(0xE0533F), FARBE(0x8A3F5A), nacht * 0.6f);
  g.kontur(1.8f, OUT);
  g.rechteck(x - 80, y - 70, 160, 70, mauer);
  for (int i = 0; i < 3; i++) {
    float tx = x - 70 + i * 70, th = i == 1 ? 120 : 95;
    g.rechteck(tx - 14, y - th, 28, th, mauer);
    float d[] = {tx - 19, y - th, tx, y - th - 34, tx + 19, y - th};
    g.polygon(d, 3, dach);
  }
  g.konturAus();
  for (int i = 0; i < 3; i++) {  // Fahnen
    float tx = x - 70 + i * 70, th = (i == 1 ? 120 : 95) + 34;
    g.linie(tx, y - th, tx, y - th - 14, 1.5f, OUT);
    float wv = sinf(t * 5 + i) * 3;
    float f[] = {tx, y - th - 14, tx + 14, y - th - 11 + wv, tx, y - th - 7};
    g.polygon(f, 3, i == 1 ? GOLD : FARBE(0x3E7FDC));
  }
  for (int i = 0; i < 4; i++) g.rechteck(x - 80 + i * 44, y - 80, 20, 12, mauer);  // Zinnen
  g.rundRechteck(x - 18, y - 44, 36, 44, 16, FARBE(0x7E4620));
  Farbe fenster = mischeFarbe(FARBE(0x8FC8FF), FARBE(0xFFD54A), nacht);
  for (int i = 0; i < 3; i++) g.rundRechteck(x - 76 + i * 70, y - 82 - (i == 1) * 20, 12, 16, 6, fenster);
}

void feuerwerk(Gfx &g, float x, float y, float u, Farbe c) {
  if (u <= 0 || u >= 1) return;
  if (u < 0.25f) {  // Rakete steigt
    float k = u / 0.25f;
    g.kreis(x, lerp(240, y, hinaus(k)), 2.5f, GOLD);
    return;
  }
  float k = (u - 0.25f) / 0.75f;
  float r = hinaus(k) * 34;
  uint8_t a = (uint8_t)(255 * (1 - k));
  for (int i = 0; i < 14; i++) {
    float an = i * 0.4488f;
    g.kreis(x + cosf(an) * r, y + sinf(an) * r + k * k * 12, 2.6f * (1 - k * 0.5f), c, a);
  }
  g.kreis(x, y, 6 * (1 - k), WEISS, a);
}

// Iris auf und zu am Anfang/Ende einer Szene
void blende(Gfx &g, float u, float dauer, float cx, float cy) {
  g.kameraAus();
  if (u < 1.2f) g.iris(cx, cy, lerp(0, 330, sanft(u / 1.2f)));
  else if (u > dauer - 1.2f) g.iris(cx, cy, lerp(330, 0, sanft((u - (dauer - 1.2f)) / 1.2f)));
}


// =====================================================================
// Frauenfigur (Aschenputtel, Stiefmutter, Schwestern, spaeter Rotkaeppchen, Gretel ...)
// =====================================================================
void frau(Gfx &g, float x, float y, float s, float dir, const FrauPose &p, float *handX, float *handY) {
  static const FrauLook STANDARD;
  const FrauLook &L = p.look ? *p.look : STANDARD;
  auto PX = [&](float dx) { return x + dx * s * dir; };
  auto PY = [&](float dy) { return y + dy * s; };
  float sw = p.lauf >= 0 ? sinf(p.lauf * 6.2831853f) * 5 : 0;
  float wehen = p.drehen + sw * 0.3f;
  // Fuesse
  g.kontur(1.2f * s, OUT);
  g.ellipse(PX(-4 - sw * 0.6f), PY(-2.5f), 5 * s, 3 * s, L.schuh);
  g.ellipse(PX(5 + sw * 0.6f), PY(-2.5f), 5 * s, 3 * s, L.schuh);
  g.konturAus();
  // hinterer Arm
  float hx, hy;
  arm(g, PX(-7), PY(-52), s * 0.95f, dir, -p.armH, hx, hy, L.kleid, L.haut);
  // Haare hinten (lang / Zopf)
  if (L.frisur == 0) {
    g.kontur(1.3f * s, OUT);
    g.ellipse(PX(-3), PY(-62), 16 * s, 22 * s, L.haar);
    g.konturAus();
  } else if (L.frisur == 1) {
    for (int i = 0; i < 4; i++) g.kreis(PX(-12 - i * 1.5f), PY(-62 + i * 7), (4.5f - i * 0.5f) * s, L.haar);
  }
  // Rock
  float rb = 20 * L.rockBreite;
  float rock[] = {PX(-9), PY(-42), PX(9), PY(-42), PX(rb + wehen), PY(-6), PX(rb * 0.5f + wehen), PY(-3), PX(0 + wehen * 0.5f), PY(-5),
                  PX(-rb * 0.5f + wehen), PY(-3), PX(-rb + wehen), PY(-6)};
  g.kontur(1.4f * s, OUT);
  g.polygon(rock, 7, L.kleid);
  g.ellipse(PX(0), PY(-48), 9.5f * s, 10 * s, L.kleid);
  g.konturAus();
  if (L.schuerze) {
    float sch[] = {PX(-6), PY(-40), PX(6), PY(-40), PX(10 + wehen), PY(-10), PX(-10 + wehen), PY(-10)};
    g.polygon(sch, 4, L.schuerze);
  }
  if (L.flicken) {
    g.rechteck(PX(-12) < PX(-6) ? PX(-12) : PX(-6), PY(-22), 6 * s, 6 * s, mischeFarbe(L.kleid, OUT, 0.25f));
    g.rechteck(PX(4) < PX(10) ? PX(4) : PX(10), PY(-14), 6 * s, 5 * s, mischeFarbe(L.kleid, WEISS, 0.3f));
  }
  if (L.glanz) {  // Gold- und Silberglanz
    for (int i = 0; i < 6; i++) {
      float gx = PX(-12 + zufall(i + 40) * 24), gy = PY(-40 + zufall(i + 50) * 34);
      float f = 0.5f + 0.5f * sinf(p.zeit * 5 + i * 1.7f);
      g.stern(gx, gy, 1.6f + f * 2 * s, 0.7f, 4, 0, WEISS, (uint8_t)(120 + 130 * f));
    }
  }
  g.rechteck(PX(-9) < PX(9) ? PX(-9) : PX(9), PY(-40), 18 * s, 2.5f * s, mischeFarbe(L.kleid, OUT, 0.25f));  // Guertel
  // Kopf
  float kx = PX(1), ky = PY(-71);
  g.kontur(1.4f * s, OUT);
  g.kreis(kx, ky, 15 * s, L.haut);
  g.konturAus();
  // Augen
  for (int i = 0; i < 2; i++) {
    float ex = PX(i ? 8.5f : -1.5f), ey = PY(-72);
    if (p.zu) g.bogen(ex, ey + 1 * s, 2.8f * s, 3.5f, 5.9f, 1.6f * s, OUT);
    else {
      g.ellipse(ex, ey, 2.8f * s, 3.7f * s, SCHWARZ);
      g.kreis(ex + (p.blick + 1) * 1.1f * s * dir, ey - 1.3f * s, 1.1f * s, WEISS);
      g.linie(ex + 1.5f * s * dir, ey - 3.5f * s, ex + 3.5f * s * dir, ey - 5 * s, 1 * s, OUT);  // Wimper
    }
    if (p.boese > 0) {
      float b = p.boese * (i ? 1 : -1);
      g.linie(ex - 3 * s, ey - (6.5f + b * 1.5f) * s, ex + 3 * s, ey - (6.5f - b * 1.5f) * s, 1.6f * s, OUT);
    }
    if (p.weint) {
      float k = fmodf(p.zeit * 1.3f + i * 0.5f, 1);
      g.kreis(ex, ey + (4 + k * 10) * s, 1.4f * s, FARBE(0x8FD8FF), (uint8_t)(255 * (1 - k)));
    }
  }
  if (L.wangen) {
    g.kreis(PX(-5), PY(-65), 3 * s, WANGE, 130);
    g.kreis(PX(12), PY(-65), 2.6f * s, WANGE, 130);
  }
  if (p.mund > 0.05f) g.ellipse(PX(4), PY(-63), 2.6f * s, (1 + p.mund * 2.5f) * s, FARBE(0x8A2A2A));
  else if (p.boese > 0.5f) g.bogen(PX(4), PY(-60), 3.5f * s, 3.7f, 5.7f, 1.5f * s, OUT);
  else g.bogen(PX(4), PY(-65.5f), 3.6f * s, 0.5f, 2.6f, 1.5f * s, OUT);
  // Haare vorn / Kopfschmuck
  if (L.frisur == 3) {  // Kopftuch
    g.kontur(1.2f * s, OUT);
    float t[] = {PX(-16), PY(-70), PX(-12), PY(-84), PX(1), PY(-89), PX(14), PY(-84), PX(16), PY(-74), PX(8), PY(-80), PX(-6), PY(-80)};
    g.polygon(t, 7, L.tuch ? L.tuch : FARBE(0x8E7E6A));
    g.konturAus();
    g.kreis(PX(-16), PY(-72), 3 * s, L.tuch ? L.tuch : FARBE(0x8E7E6A));
  } else {
    g.kontur(1.2f * s, OUT);
    g.ellipse(PX(1), PY(-81), 15 * s, 7 * s, L.haar);
    g.konturAus();
    g.ellipse(PX(-12), PY(-74), 4 * s, 7 * s, L.haar);
    if (L.frisur == 2) {  // Dutt
      g.kontur(1.2f * s, OUT);
      g.kreis(PX(-4), PY(-90), 7 * s, L.haar);
      g.konturAus();
    }
  }
  if (L.krone) {
    float k[] = {PX(-8), PY(-83), PX(-8), PY(-93), PX(-3), PY(-87), PX(1), PY(-95), PX(5), PY(-87), PX(10), PY(-93), PX(10), PY(-83)};
    g.kontur(1, OUT);
    g.polygon(k, 7, GOLD);
    g.konturAus();
  }
  if (L.blume) g.kreis(PX(-10), PY(-82), 3 * s, L.blume);
  // vorderer Arm
  arm(g, PX(8), PY(-52), s * 0.95f, dir, p.armV, hx, hy, L.kleid, L.haut);
  if (handX) *handX = hx;
  if (handY) *handY = hy;
}

// weisse Taube (fliegt: fluegel 0..1 schlaegt, sitzt: fluegel < 0)
void taube(Gfx &g, float x, float y, float s, float dir, float flug, float pick) {
  g.kontur(1 * s, OUT);
  if (flug >= 0) {
    float w = sinf(flug * 22) * 0.9f;
    g.ellipse(x - 2 * s * dir, y - 4 * s - w * 5 * s, 8 * s, 3 * s, FARBE(0xE8ECF4), 255, -0.6f * dir + w * dir);
  }
  g.ellipse(x, y, 8 * s, 5.5f * s, WEISS);
  float k = y - 5 * s + pick * 5 * s;
  g.kreis(x + 6 * s * dir, k, 4 * s, WEISS);
  g.konturAus();
  g.ellipse(x - 2 * s * dir, y - 1 * s, 5 * s, 3 * s, FARBE(0xE0E6F0));
  float sch[] = {x + 9 * s * dir, k - 0.5f * s, x + 13 * s * dir, k + 1 * s, x + 9 * s * dir, k + 1.5f * s};
  g.polygon(sch, 3, FARBE(0xF2A93B));
  g.kreis(x + 7 * s * dir, k - 1 * s, 0.9f * s, SCHWARZ);
  float sw[] = {x - 7 * s * dir, y - 1 * s, x - 13 * s * dir, y - 4 * s, x - 13 * s * dir, y + 2 * s};
  g.polygon(sw, 3, WEISS);
}

void eimer(Gfx &g, float x, float y, float s, float kipp) {
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

void fisch(Gfx &g, float x, float y, float s, float w, uint8_t a) {
  g.ellipse(x, y, 6 * s, 3.2f * s, FARBE(0xFF9A3C), a, w);
  float c = cosf(w), sn = sinf(w);
  float sch[] = {x - 5 * s * c, y - 5 * s * sn, x - 10 * s * c - 4 * s * sn, y - 10 * s * sn + 4 * s * c,
                 x - 10 * s * c + 4 * s * sn, y - 10 * s * sn - 4 * s * c};
  g.polygon(sch, 3, FARBE(0xFF7A1C), a);
  g.kreis(x + 3 * s * c, y + 3 * s * sn - 1 * s, 1 * s, SCHWARZ, a);
}

// =====================================================================
// Haeuser und Oefen
// =====================================================================
void fenster(Gfx &g, float x, float y, float w, float h, float licht, bool vorhang, bool blumen) {
  const Farbe rahmen = FARBE(0xF2EEE4), glas = mischeFarbe(FARBE(0x9FD8FF), FARBE(0xFFD27A), licht);
  g.kontur(1.4f, OUT);
  g.rundRechteck(x - 2, y - 2, w + 4, h + 4, 3, rahmen);
  g.konturAus();
  g.rechteck(x, y, w, h, glas);
  if (licht < 0.5f) {  // Spiegelung des Himmels
    float r[] = {x + w * 0.15f, y + h, x + w * 0.45f, y, x + w * 0.65f, y, x + w * 0.35f, y + h};
    g.polygon(r, 4, WEISS, 90);
  } else g.kreis(x + w / 2, y + h / 2, w * 1.2f, FARBE(0xFFD27A), (uint8_t)(50 * licht));
  if (vorhang) {
    float v1[] = {x, y, x + w * 0.32f, y, x + w * 0.12f, y + h * 0.75f, x, y + h * 0.8f};
    float v2[] = {x + w, y, x + w * 0.68f, y, x + w * 0.88f, y + h * 0.75f, x + w, y + h * 0.8f};
    g.polygon(v1, 4, FARBE(0xE8473D), 210);
    g.polygon(v2, 4, FARBE(0xE8473D), 210);
    g.rechteck(x, y, w, 3, FARBE(0xC0392B));
  }
  g.rechteck(x + w / 2 - 1.2f, y, 2.4f, h, rahmen);
  g.rechteck(x, y + h / 2 - 1.2f, w, 2.4f, rahmen);
  g.kontur(1, OUT);
  g.rechteck(x - 5, y + h + 2, w + 10, 4, FARBE(0xC8B8A0));  // Fensterbank
  g.konturAus();
  if (blumen) {
    g.rechteck(x - 2, y + h + 6, w + 4, 6, HOLZ);
    for (int i = 0; i < 5; i++) {
      float bx = x + 1 + i * (w - 2) / 4;
      g.kreis(bx, y + h + 4, 2.6f, GRAS_D);
      g.kreis(bx, y + h + 1, 2, i & 1 ? FARBE(0xFF7FA8) : FARBE(0xE8473D));
    }
  }
}

void holztuer(Gfx &g, float x, float y, float w, float h, Farbe holz, bool rund) {
  const Farbe dunkel = mischeFarbe(holz, OUT, 0.35f), hell = mischeFarbe(holz, WEISS, 0.15f);
  g.kontur(1.4f, OUT);
  g.rundRechteck(x, y - h, w, h, rund ? w * 0.45f : 2, holz);
  g.konturAus();
  for (int i = 1; i < 4; i++) g.linie(x + w * i / 4, y - h + (rund ? w * 0.2f : 2), x + w * i / 4, y - 1, 1, dunkel);
  g.rechteck(x + 1, y - h * 0.75f, w - 2, 2.4f, OUT, 160);  // Beschlaege
  g.rechteck(x + 1, y - h * 0.3f, w - 2, 2.4f, OUT, 160);
  g.rechteck(x + 2, y - h + w * 0.3f, w * 0.3f, h * 0.9f - w * 0.3f, hell, 70);
  g.kreis(x + w * 0.78f, y - h * 0.48f, 1.8f, GOLD);
  g.ellipse(x + w / 2, y + 1, w * 0.7f, 2.4f, STEIN);  // Trittstein
}

void ziegeldach(Gfx &g, float xl, float xr, float y, float hoehe, Farbe c, float schnee) {
  const Farbe dunkel = mischeFarbe(c, OUT, 0.3f), hell = mischeFarbe(c, WEISS, 0.18f);
  float xm = (xl + xr) / 2, ue = (xr - xl) * 0.06f;
  float d[] = {xl - ue, y + 2, xm, y - hoehe, xr + ue, y + 2};
  g.kontur(1.6f, OUT);
  g.polygon(d, 3, c);
  g.konturAus();
  // Ziegelreihen
  int reihen = (int)(hoehe / 7);
  const float fuge = fmaxf(6, (xr - xl) / 14);
  for (int r = 1; r <= reihen; r++) {
    float k = r / (float)(reihen + 1), yy = y - hoehe + k * hoehe;
    float hb = (xr - xl + 2 * ue) / 2 * k;
    g.linie(xm - hb + 2, yy, xm + hb - 2, yy, 1, dunkel, 200);
    for (float zx = xm - hb + 4 + (r & 1) * fuge * 0.5f; zx < xm + hb - 3; zx += fuge) g.linie(zx, yy, zx, yy - 5, 0.8f, dunkel, 140);
  }
  float l[] = {xl - ue, y + 2, xm, y - hoehe, xm - 3, y - hoehe + 4, xl - ue + 3, y + 2};
  g.polygon(l, 4, hell, 160);  // Licht an der linken Kante
  g.rechteck(xl - ue, y, xr - xl + 2 * ue, 3, dunkel);  // Traufe
  if (schnee > 0) {
    float s1[] = {xl - ue, y + 2, xm, y - hoehe, xr + ue, y + 2, xm, y - hoehe + hoehe * 0.35f * schnee};
    g.polygon(s1, 4, FARBE(0xF2F6FF));
  }
}

void hauswand(Gfx &g, float x, float y, float w, float h, Farbe c, bool fachwerk) {
  const Farbe schatten = mischeFarbe(c, OUT, 0.15f);
  g.kontur(1.6f, OUT);
  g.rechteck(x, y, w, h, c);
  g.konturAus();
  for (int i = 0; i < 8; i++) g.ellipse(x + zufall(i + (int)x) * w, y + zufall(i + 7 + (int)y) * h * 0.8f, 3, 1.4f, schatten, 90);
  g.rechteck(x + w - 6, y, 6, h, schatten, 80);  // Schattenseite
  if (fachwerk) {
    const Farbe balken = FARBE(0x6A4028);
    g.rechteck(x, y, w, 3, balken);
    g.rechteck(x, y + h * 0.48f, w, 3, balken);
    for (int i = 0; i <= 3; i++) g.rechteck(x + i * (w - 3) / 3, y, 3, h - 10, balken);
    g.linie(x + 2, y + h * 0.48f, x + (w - 3) / 3, y + 2, 2.4f, balken);
    g.linie(x + w - 2, y + h * 0.48f, x + w - (w - 3) / 3, y + 2, 2.4f, balken);
  }
  // Sockel aus Feldsteinen
  g.rechteck(x, y + h - 10, w, 10, FARBE(0x9A968E));
  for (int i = 0; i < (int)(w / 12) + 1; i++) {
    g.ellipse(x + 6 + i * 12, y + h - 5 - (i & 1) * 1.5f, 5, 3.2f, i & 1 ? STEIN : FARBE(0xB8B4AC));
  }
}

void schornstein(Gfx &g, float x, float y, float t, float rauch, float nacht) {
  g.kontur(1.4f, OUT);
  g.rechteck(x - 6, y - 26, 12, 26, FARBE(0xA8452E));
  g.rechteck(x - 8, y - 30, 16, 5, FARBE(0x8E3A26));
  g.konturAus();
  for (int r = 0; r < 4; r++) g.linie(x - 6, y - 20 + r * 6, x + 6, y - 20 + r * 6, 0.8f, FARBE(0x7A2E1E));
  for (int i = 0; i < 4 && rauch > 0.02f; i++) {
    float k = fmodf(t * 0.35f + i * 0.25f, 1);
    g.kreis(x + k * 14 + sinf(t + i) * 2, y - 34 - k * 34, 3 + k * 6, mischeFarbe(WEISS, FARBE(0x8E8AA8), nacht),
            (uint8_t)(150 * (1 - k) * rauch));
  }
}

void haeuschen(Gfx &g, float x, float y, float w, float h, Farbe wand, Farbe dach, float t, float nacht, bool fachwerk,
               float schnee) {
  float xl = x - w / 2;
  schornstein(g, x + w * 0.25f, y - h - h * 0.35f, t, 1, nacht);
  hauswand(g, xl, y - h, w, h, wand, fachwerk);
  ziegeldach(g, xl, xl + w, y - h, h * 0.75f, dach, schnee);
  float fw = w * 0.2f, fh = h * 0.32f;
  fenster(g, xl + w * 0.1f, y - h * 0.78f, fw, fh, nacht, true, true);
  fenster(g, xl + w * 0.7f, y - h * 0.78f, fw, fh, nacht, true, true);
  holztuer(g, x - w * 0.11f, y - 1, w * 0.22f, h * 0.5f);
}

void flammen(Gfx &g, float x, float y, float breite, float hoehe, float t, float staerke) {
  if (staerke <= 0) return;
  g.kreis(x, y - hoehe * 0.4f, breite * 1.3f, FARBE(0xFFB050), (uint8_t)(40 * staerke));  // Schein
  const Farbe lagen[] = {FARBE(0xE8402A), FARBE(0xFF8A2A), FARBE(0xFFD54A), FARBE(0xFFF6C8)};
  for (int l = 0; l < 4; l++) {
    float bw = breite * (1 - l * 0.22f), bh = hoehe * (1 - l * 0.2f) * staerke;
    int n = 5 - l;
    for (int i = 0; i < n; i++) {
      float fx = x - bw / 2 + (i + 0.5f) * bw / n;
      float hh = bh * (0.65f + 0.35f * sinf(t * (8 + l) + i * 1.9f + l));
      float f[] = {fx - bw / n * 0.75f, y, fx + bw / n * 0.75f, y, fx + sinf(t * 7 + i + l) * bw * 0.06f, y - hh};
      g.polygon(f, 3, lagen[l]);
    }
  }
  for (int i = 0; i < 4; i++) {  // Funken
    float k = fmodf(t * 0.9f + i * 0.27f, 1);
    g.kreis(x + sinf(t * 3 + i * 2) * breite * 0.4f, y - hoehe * staerke - k * hoehe * 0.8f, 1.2f, FARBE(0xFFE070), (uint8_t)(230 * (1 - k) * staerke));
  }
}

void herd(Gfx &g, float x, float y, float w, float h, float t, float feuer, Farbe stein, bool rauchfang) {
  const Farbe fuge = mischeFarbe(stein, OUT, 0.35f), hell = mischeFarbe(stein, WEISS, 0.2f);
  if (rauchfang) {  // Rauchfang ueber dem Herd
    float rf[] = {x - w * 0.05f, y - h - 4, x + w * 1.05f, y - h - 4, x + w * 0.8f, y - h - 70, x + w * 0.2f, y - h - 70};
    g.kontur(1.6f, OUT);
    g.polygon(rf, 4, mischeFarbe(stein, OUT, 0.12f));
    g.konturAus();
    g.rechteck(x + w * 0.28f, y - h - 120, w * 0.44f, 52, mischeFarbe(stein, OUT, 0.12f));
    g.linie(x - w * 0.05f, y - h - 6, x + w * 1.05f, y - h - 6, 3, fuge);
  }
  g.kontur(2, OUT);
  g.rundRechteck(x, y - h, w, h, 5, stein);
  g.konturAus();
  // Mauersteine
  for (int r = 0; r * 9 < h - 4; r++) {
    float yy = y - h + 4 + r * 9;
    g.linie(x + 2, yy, x + w - 2, yy, 1, fuge, 200);
    for (float zx = x + 4 + (r & 1) * 8; zx < x + w - 3; zx += 16) g.linie(zx, yy, zx, yy + 9, 1, fuge, 200);
  }
  g.rechteck(x + 3, y - h + 2, w * 0.4f, 3, hell, 160);
  // Herdplatte
  g.kontur(1.4f, OUT);
  g.rechteck(x - 4, y - h - 5, w + 8, 7, FARBE(0x4A4A54));
  g.konturAus();
  g.ellipse(x + w * 0.3f, y - h - 2, w * 0.13f, 2, FARBE(0x6E6E78));
  g.ellipse(x + w * 0.7f, y - h - 2, w * 0.13f, 2, FARBE(0x6E6E78));
  // Feuerloch mit Bogen
  float ox = x + w * 0.18f, ow = w * 0.64f, oy = y - 6, oh = h * 0.55f;
  g.rundRechteck(ox - 3, oy - oh - 3, ow + 6, oh + 6, ow * 0.4f, fuge);
  g.rundRechteck(ox, oy - oh, ow, oh, ow * 0.38f, FARBE(0x1E1612));
  // Holzscheite und Glut
  for (int i = 0; i < 3; i++) {
    g.kontur(0.8f, OUT);
    g.ellipse(ox + ow * (0.25f + i * 0.25f), oy - 4, ow * 0.16f, 3.2f, HOLZ_D, 255, (i - 1) * 0.3f);
    g.konturAus();
    g.kreis(ox + ow * (0.25f + i * 0.25f) + ow * 0.14f, oy - 4, 2, FARBE(0xC8A070));
  }
  for (int i = 0; i < 6; i++) g.kreis(ox + ow * (0.15f + i * 0.14f), oy - 1, 2, FARBE(0xFF7A2A), (uint8_t)(200 * feuer * (0.6f + 0.4f * sinf(t * 6 + i))));
  flammen(g, ox + ow / 2, oy - 5, ow * 0.85f, oh * 0.8f, t, feuer);
  g.rechteck(x + 2, y - 4, w - 4, 4, fuge);  // Sockel
}

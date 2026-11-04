// Gemeinsame Bausteine fuer alle Maerchen (siehe figuren.h).
#include "figuren.h"
#include "schriften.h"

// =====================================================================
// Figuren
// =====================================================================

// Arm vom Schultergelenk; gibt die Handposition zurueck
void arm(Gfx &g, float sx, float sy, float s, float dir, float winkel, float &hx, float &hy) {
  float l = 15 * s;
  hx = sx + sinf(winkel) * l * dir;
  hy = sy + cosf(winkel) * l;
  g.kontur(1.4f * s, OUT);
  g.linie(sx, sy, hx, hy, 6.5f * s, HEMD);
  g.kreis(hx, hy, 3.8f * s, HAUT);
  g.konturAus();
}

// Das Schneiderlein. (x, y) = zwischen den Fuessen am Boden, s = Groesse (1 = ca. 95 px), dir = +1 schaut rechts
// teil: 0 = alles, 1 = ohne Arme, 2 = nur die Arme (zum Zeichnen vor einem Tisch)
void schneiderlein(Gfx &g, float x, float y, float s, float dir, const Pose &p, float *handX,
                          float *handY, int teil) {
  auto PX = [&](float dx) { return x + dx * s * dir; };
  auto PY = [&](float dy) { return y + dy * s; };
  float hx, hy;
  if (teil == 2) {
    arm(g, PX(-8), PY(-38), s, dir, p.armH * -1, hx, hy);
  } else {
  // Beine
  float sw = p.lauf >= 0 ? sinf(p.lauf * 6.2831853f) * 7 : 0;
  g.kontur(1.4f * s, OUT);
  g.linie(PX(-4), PY(-22), PX(-4 - sw), PY(-5), 6 * s, HOSE);
  g.linie(PX(4), PY(-22), PX(4 + sw), PY(-5), 6 * s, HOSE);
  g.ellipse(PX(-3 - sw), PY(-3), 6 * s, 3.5f * s, SCHUH);
  g.ellipse(PX(5 + sw), PY(-3), 6 * s, 3.5f * s, SCHUH);
  // hinterer Arm
  g.konturAus();
  if (teil == 0) arm(g, PX(-8), PY(-38), s, dir, p.armH * -1, hx, hy);
  if (p.ranzen) {
    g.kontur(1.3f * s, OUT);
    g.ellipse(PX(-12), PY(-33), 7 * s, 9 * s, FARBE(0xB0703A));
    g.konturAus();
    g.linie(PX(-12), PY(-42), PX(-12), PY(-38), 2 * s, FARBE(0x7E4620));
  }
  // Koerper
  g.kontur(1.4f * s, OUT);
  g.ellipse(PX(0), PY(-31), 12.5f * s, 14 * s, WESTE);
  g.konturAus();
  g.ellipse(PX(2), PY(-33), 4 * s, 9 * s, HEMD);
  g.kreis(PX(1), PY(-36), 1.3f * s, GOLD);
  g.kreis(PX(1), PY(-30), 1.3f * s, GOLD);
  if (p.guertel) {
    g.rechteck(PX(-12.5f) < PX(12.5f) ? PX(-12.5f) : PX(12.5f), PY(-25), 25 * s, 5 * s, FARBE(0x6B3B1C));
    g.rundRechteck(PX(-3), PY(-26), 6 * s, 7 * s, 1.5f * s, GOLD);
  }
  // Kopf
  float kx = PX(1), ky = PY(-60);
  g.kontur(1.5f * s, OUT);
  g.ellipse(PX(-4), PY(-64), 16 * s, 14 * s, HAAR);
  g.kreis(kx, ky, 17 * s, HAUT);
  g.konturAus();
  g.ellipse(PX(-13), PY(-60), 4 * s, 5 * s, HAUT);  // Ohr
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
  g.kreis(PX(-4), PY(-53), 3.5f * s, WANGE, 140);
  g.kreis(PX(14), PY(-53), 3 * s, WANGE, 140);
  // Mund
  if (p.mund > 0.05f) {
    g.ellipse(PX(5), PY(-50), 3.5f * s, (1.5f + p.mund * 3) * s, FARBE(0x8A2A2A));
  } else {
    g.bogen(PX(5), PY(-53), 4.5f * s, 0.5f, 2.6f, 1.8f * s, OUT);
  }
  // Zipfelmuetze mit Feder
  g.kontur(1.5f * s, OUT);
  float m[] = {PX(-16), PY(-68), PX(16), PY(-70), PX(9), PY(-84), PX(-4), PY(-93), PX(-16), PY(-99), PX(-10), PY(-86)};
  g.polygon(m, 6, HUT);
  g.konturAus();
  g.ellipse(PX(0), PY(-69), 17.5f * s, 4 * s, HUT_D);
  g.kreis(PX(-16), PY(-99), 3 * s, GOLD);
  g.kurve(PX(8), PY(-72), PX(16), PY(-90), PX(24), PY(-96), 3 * s, FEDER);
  }
  if (teil == 1) return;
  // vorderer Arm
  arm(g, PX(9), PY(-38), s, dir, p.armV, hx, hy);
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
  g.kreis(x, y, 9 * s, WEISS, a);
  g.kreis(x + 10 * s, y - 4 * s, 11 * s, WEISS, a);
  g.kreis(x + 21 * s, y, 8 * s, WEISS, a);
  g.rundRechteck(x - 2 * s, y, 26 * s, 8 * s, 4 * s, WEISS, a);
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
  for (int k = -1; k < 284 / abstand + 2; k++) {
    float x = k * abstand - off;
    g.ellipse(x, y, abstand * 0.75f, h, c);
  }
}

void laubbaum(Gfx &g, float x, float y, float s, Farbe krone) {
  g.kontur(1.5f, OUT);
  g.rundRechteck(x - 6 * s, y - 40 * s, 12 * s, 40 * s, 4 * s, FARBE(0x8E5B3A));
  g.kreis(x - 16 * s, y - 48 * s, 18 * s, krone);
  g.kreis(x + 16 * s, y - 50 * s, 18 * s, krone);
  g.kreis(x, y - 66 * s, 22 * s, krone);
  g.konturAus();
  g.kreis(x - 6 * s, y - 72 * s, 7 * s, FARBE(0x8FDC7C), 150);
  g.kreis(x + 14 * s, y - 56 * s, 5 * s, FARBE(0x8FDC7C), 120);
}

void tanne(Gfx &g, float x, float y, float s, Farbe c) {
  g.rechteck(x - 4 * s, y - 14 * s, 8 * s, 14 * s, FARBE(0x6E4426));
  g.kontur(1.3f, OUT);
  for (int i = 0; i < 3; i++) {
    float w = (30 - i * 7) * s, yy = y - 12 * s - i * 20 * s;
    float q[] = {x - w, yy, x + w, yy, x, yy - 32 * s};
    g.polygon(q, 3, c);
  }
  g.konturAus();
}

void blumen(Gfx &g, float scroll, float y, int n, int seed) {
  for (int i = 0; i < n; i++) {
    float x = fmodf(zufall(i + seed) * 400 - scroll + 4000, 400) - 60;
    float yy = y + zufall(i + seed + 50) * 30;
    g.linie(x, yy, x, yy - 6, 1.3f, GRAS_D);
    g.kreis(x, yy - 7, 3, i % 3 == 0 ? FARBE(0xFF7FA8) : i % 3 == 1 ? GOLD : WEISS);
  }
}

void boden(Gfx &g, float y, Farbe c) {
  g.rechteck(0, y, 284, 240 - y, c);
  g.rechteck(0, y, 284, 3, GRAS_H);
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

void riesenArm(Gfx &g, float sx, float sy, float s, float dir, float w, Farbe hemd, float *hx,
                      float *hy) {
  float x = sx + sinf(w) * 46 * s * dir, y = sy + cosf(w) * 46 * s;
  g.kontur(1.8f, OUT);
  g.linie(sx, sy, x, y, 17 * s, hemd);
  g.kreis(x, y, 10 * s, RIESE_HAUT);
  g.konturAus();
  if (hx) *hx = x;
  if (hy) *hy = y;
}

// (x, y) = Boden in der Mitte, s = 1: etwa 165 px hoch (stehend)
void riese(Gfx &g, float x, float y, float s, float dir, const RiesePose &p, float *handX,
                  float *handY) {
  auto PX = [&](float dx) { return x + dx * s * dir; };
  auto PY = [&](float dy) { return y + dy * s; };
  Farbe hemd = p.farbe ? FARBE(0x5C7FB8) : FARBE(0x6E9A3C);
  Farbe hose = p.farbe ? FARBE(0x4A3B6B) : FARBE(0x6B4A2E);
  Farbe bart = p.farbe ? FARBE(0x5B4636) : RIESE_BART;
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
  g.kreis(KX(0, 0), KY(0, 0), 28 * s, RIESE_HAUT);
  g.ellipse(KX(0, 20), KY(0, 20), 26 * s, 18 * s, bart);
  g.konturAus();
  // Haare
  for (int i = 0; i < 5; i++) g.kreis(KX(-18 + i * 9, -24 + (i == 2) * -3), KY(-18 + i * 9, -24), 9 * s, bart);
  g.ellipse(KX(-26, 0), KY(-26, 0), 6 * s, 8 * s, RIESE_HAUT);  // Ohr
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
void einhorn(Gfx &g, float x, float y, float s, float lauf, float dir, float senken, float zappel) {
  auto PX = [&](float dx) { return x + dx * s * dir; };
  auto PY = [&](float dy) { return y + dy * s; };
  const Farbe fell = FARBE(0xFFFFFF), schatten = FARBE(0xE6E1F5);
  const Farbe regen[] = {FARBE(0xFF6B8A), FARBE(0xFFB347), FARBE(0xFFE066), FARBE(0x7ED957), FARBE(0x6EC6FF), FARBE(0xB28DFF)};
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
  float hornW = lerp(-1.0f, -0.2f, hk);
  float hx = kx + cosf(hornW) * 30 * s * dir, hy = ky + sinf(hornW) * 30 * s - 4 * s;
  float horn[] = {kx + 2 * s * dir, ky - 9 * s, kx + 8 * s * dir, ky - 5 * s, hx, hy};
  g.kontur(1.2f, OUT);
  g.polygon(horn, 3, GOLD);
  g.konturAus();
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


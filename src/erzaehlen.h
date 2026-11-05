#pragma once
// Gemeinsame Erzaehl-Bausteine fuer die Maerchen: Sprechblasen, Verse oben im Bild, Glitzern, Puff,
// Posen-Helfer und das Startbild mit Titel. (static inline: jede Maerchendatei bekommt ihre eigene Kopie.)
#include "maerchen.h"
#include "schriften.h"
#include "figuren.h"
#include "melodien.h"
#include <initializer_list>
#include <stdio.h>

// 0..1..0: geht bei a federnd auf, bei b wieder zu
static inline float aufzu(float u, float a, float b) { return federnd(seg(u, a, a + 0.4f)) * (1 - seg(u, b - 0.3f, b)); }

// Sprechblase mit Spitze zum Sprecher (sx, sy); Zeilen hoechstens etwa 34 Zeichen
static inline void blase(Gfx &g, float x, float y, std::initializer_list<const char *> zeilen, float k, float sx,
                         float sy, Farbe f = WEISS) {
  if (k <= 0.05f) return;
  int w = 0, n = 0;
  for (const char *z : zeilen) { int b = g.textBreite(SCHRIFT_K, z); if (b > w) w = b; n++; }
  float bw = (w + 18) * k, bh = (8 + n * 18) * k;
  if (x - bw / 2 < 3) x = 3 + bw / 2;
  if (x + bw / 2 > 281) x = 281 - bw / 2;
  if (y - bh / 2 < 3) y = 3 + bh / 2;
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

// Erzaehltext oben im Bild auf einem Pergamentstreifen (wie in einem Bilderbuch)
static inline void vers(Gfx &g, std::initializer_list<const char *> zeilen, float k) {
  if (k <= 0.02f) return;
  int n = (int)zeilen.size();
  float h = 9 + n * 17;
  uint8_t a = (uint8_t)(255 * klemme(k));
  g.kameraAus();
  g.kontur(1.2f, FARBE(0xB8862B));
  g.rundRechteck(6, 4, 272, h, 7, FARBE(0xFBF1DC), a);
  g.konturAus();
  g.kreis(13, 4 + h / 2, 2.2f, FARBE(0xC8485E), a);
  g.kreis(271, 4 + h / 2, 2.2f, FARBE(0xC8485E), a);
  if (k > 0.8f) {
    blaseLesbar = 1;
    int i = 0;
    for (const char *z : zeilen) g.text(SCHRIFT_K, z, 142, 18 + i++ * 17, FARBE(0x5A3418), 1);
  }
}

static inline void glitzern(Gfx &g, float x, float y, float r, float t, int n, uint8_t a = 255) {
  for (int i = 0; i < n; i++) {
    float an = t * 2 + i * 6.2831853f / n;
    float rr = r * (0.6f + 0.4f * sinf(t * 3 + i));
    g.stern(x + cosf(an) * rr, y + sinf(an) * rr * 0.8f, 4, 1.6f, 4, an, i & 1 ? GOLD : WEISS, a);
  }
}

// Verwandlung / Zauber: Sterne fliegen auseinander
static inline void puff(Gfx &g, float x, float y, float k, Farbe c = GOLD) {
  if (k <= 0 || k >= 1) return;
  for (int i = 0; i < 10; i++) {
    float a = i * 0.628f;
    g.stern(x + cosf(a) * k * 50, y + sinf(a) * k * 44, 6 * (1 - k) + 2, 2, 5, a + k * 4, i & 1 ? c : WEISS,
            (uint8_t)(255 * (1 - k)));
  }
  g.kreis(x, y, 34 * (1 - k), WEISS, (uint8_t)(180 * (1 - k)));
}

// Staub- oder Rauchwolke aus Kugeln
static inline void wolkePuff(Gfx &g, float x, float y, float k, Farbe c = WEISS, float gr = 1) {
  if (k <= 0 || k >= 1) return;
  for (int i = 0; i < 9; i++) {
    float a = i * 0.698f;
    g.kreis(x + cosf(a) * k * 34 * gr, y + sinf(a) * k * 28 * gr, (9 - 6 * k) * gr, c, (uint8_t)(220 * (1 - k)));
  }
}

static inline void herzen(Gfx &g, float cx, float cy, float t, int n, float breite = 30) {
  for (int i = 0; i < n; i++) {
    float k = fmodf(t * 0.45f + i / (float)n, 1);
    herz(g, cx + sinf(k * 7 + i) * breite, cy - k * 100, 5 + (i % 3), FARBE(0xFF5A7A), (uint8_t)(255 * huepf(k)));
  }
}

static inline Pose posis(const Aussehen &a, float t, float versatz = 0) {
  Pose p;
  p.aussehen = &a;
  p.zu = fmodf(t + 0.4f + versatz, 3.9f) < 0.13f;
  return p;
}
static inline FrauPose fposis(const FrauLook &l, float t, float versatz = 0) {
  FrauPose p;
  p.look = &l;
  p.zeit = t;
  p.zu = fmodf(t + 1.1f + versatz, 4.3f) < 0.13f;
  return p;
}

// Startbild: Titelkasten oben links und roter Startknopf
static inline void titelRahmen(Gfx &g, float t, const char *titel, float breite, const Schrift &sch = SCHRIFT_G) {
  float mx = 8 + breite / 2;
  g.rundRechteck(8, 20, breite, 50, 14, FARBE(0x1E1A3A), 160);
  g.text(sch, titel, mx, &sch == &SCHRIFT_G ? 55 : 52, GOLD, 1);
  float b = 1 + sinf(t * 4) * 0.08f;
  g.kontur(2, OUT);
  g.kreis(mx, 96, 14 * b, FARBE(0xE8473D));
  g.konturAus();
  float tri[] = {mx - 5, 88, mx - 5, 104, mx + 8, 96};
  g.polygon(tri, 3, WEISS);
}

// Ende-Bild: Iris schliesst sich, goldenes "Ende" mit Sternen
static inline void endeBild(Gfx &g, float u, float ab, float cx = 142, float cy = 170) {
  if (u > ab) g.iris(cx, cy, lerp(330, 0, sanft(seg(u, ab, ab + 2))));
  if (u > ab + 2) {
    uint8_t a = (uint8_t)(255 * sanft(seg(u, ab + 2, ab + 3)));
    g.text(SCHRIFT_G, "Ende", 142, 122, GOLD, 1, a);
    for (int i = 0; i < 6; i++) {
      float an = u * 1.2f + i * 1.047f;
      g.stern(142 + cosf(an) * 60, 112 + sinf(an) * 30, 5, 2, 5, an, GOLD, a);
    }
  }
}
static inline void endeKlang(Synth &s, float x, float t0, float t1) {
  if (bei(x, t0, t1)) { s.glocke(x, 72); s.glocke(x + 0.3f, 76); s.glocke(x + 0.6f, 79); s.glocke(x + 1.0f, 84); }
}

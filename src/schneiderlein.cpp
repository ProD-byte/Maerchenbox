// Das tapfere Schneiderlein - ein Spieluhr-Film fuer die Box.
// Teil 1: Werkstatt, Mus, sieben auf einen Streich, der Guertel.
#include "maerchen.h"
#include "schriften.h"
#include "figuren.h"
#include "melodien.h"
#include <stdio.h>
#include <initializer_list>

// ---------- Sprechblasen ----------
static float aufzu(float u, float a, float b) { return federnd(seg(u, a, a + 0.4f)) * (1 - seg(u, b - 0.3f, b)); }

// Sprechblase mit einer oder mehreren Zeilen (Bildschirmkoordinaten), Spitze zeigt nach (sx, sy)
static void blase(Gfx &g, float x, float y, std::initializer_list<const char *> zeilen, float k, float sx, float sy,
                  Farbe f = FARBE(0xFFFFFF)) {
  if (k <= 0.05f) return;
  int w = 0, n = 0;
  for (const char *z : zeilen) { int b = g.textBreite(SCHRIFT_K, z); if (b > w) w = b; n++; }
  float bw = (w + 18) * k, bh = (8 + n * 18) * k;
  if (x - bw / 2 < 3) x = 3 + bw / 2;
  if (x + bw / 2 > 281) x = 281 - bw / 2;
  float sp[] = {x - 6, y + bh / 2 - 2, x + 6, y + bh / 2 - 2, sx, sy};
  g.kontur(1.4f, FARBE(0x3A2418));
  g.rundRechteck(x - bw / 2, y - bh / 2, bw, bh, 9 * k, f);
  g.konturAus();
  g.polygon(sp, 3, f);
  if (k > 0.85f) {
    blaseLesbar = 1;
    int i = 0;
    for (const char *z : zeilen) g.text(SCHRIFT_K, z, x, y - bh / 2 + 18 + i++ * 18, FARBE(0x3A2418), 1);
  }
}

// =====================================================================
// Kulissen
// =====================================================================
// Werkstatt: Fenster links (Innenraum 24..104 x 42..108), Tisch, Regal
static const float TISCH_Y = 150;
static const float FY = 176;  // Fusshoehe des Schneiderleins hinter dem Tisch
static void werkstattHinten(Gfx &g, float t, float frauX, float frauWinke) {
  g.verlauf(0, 190, FARBE(0xF8E2B8), FARBE(0xEDC48D));
  g.rechteck(0, 160, 284, 30, FARBE(0xD9A36A));
  for (int i = 0; i < 12; i++) g.rechteck(i * 26 + 6, 162, 2, 26, FARBE(0xC48E57));
  g.rechteck(0, 190, 284, 50, FARBE(0xB67A45));
  for (int i = 0; i < 4; i++) g.rechteck(0, 200 + i * 12, 284, 1.5f, FARBE(0x9C6436));
  // Fenster
  g.kontur(2, OUT);
  g.rundRechteck(16, 34, 96, 82, 6, HOLZ);
  g.konturAus();
  g.ausschnitt(24, 42, 80, 66);
  g.verlauf(42, 108, FARBE(0x8FD3FF), FARBE(0xD8F1FF));
  sonne(g, 88, 56, 8, t);
  wolke(g, fmodf(t * 4, 120) - 20, 60, 0.7f);
  g.ellipse(64, 118, 70, 22, FARBE(0x7CCB5E));
  if (frauX > -40) bauersfrau(g, frauX, 82, frauWinke);
  g.ausschnittAus();
  g.rechteck(62, 42, 4, 66, HOLZ);
  g.rechteck(24, 73, 80, 4, HOLZ);
  g.rundRechteck(12, 112, 104, 8, 3, HOLZ_D);
  // Blumentopf auf dem Fensterbrett
  g.kontur(1.2f, OUT);
  g.rundRechteck(90, 98, 14, 14, 3, FARBE(0xD9693A));
  g.konturAus();
  for (int i = 0; i < 3; i++) {
    float bx = 92 + i * 5, by = 90 - (i == 1) * 6;
    g.linie(97, 99, bx, by, 1.5f, FARBE(0x3E9A4B));
    g.kreis(bx, by, 3.5f, i == 1 ? FARBE(0xFFD54A) : FARBE(0xFF7FA8));
  }
  // Regal mit Stoffballen
  g.rechteck(186, 70, 88, 6, HOLZ_D);
  const Farbe stoffe[] = {FARBE(0xE8473D), FARBE(0x3E7FDC), FARBE(0xFFC93A), FARBE(0x3DAE5B), FARBE(0xB05FD8)};
  for (int i = 0; i < 5; i++) {
    g.kontur(1.2f, OUT);
    g.rundRechteck(190 + i * 16, 44, 13, 26, 4, stoffe[i]);
    g.konturAus();
    g.rechteck(190 + i * 16, 52, 13, 2, WEISS, 120);
  }
  // Schere an der Wand
  g.kreis(150, 52, 5, FARBE(0x9AA6B8));
  g.kreis(160, 52, 5, FARBE(0x9AA6B8));
  g.kreis(150, 52, 2.5f, FARBE(0xF8E2B8));
  g.kreis(160, 52, 2.5f, FARBE(0xF8E2B8));
  g.linie(153, 48, 162, 30, 3, FARBE(0xC7D0DC));
  g.linie(157, 48, 148, 30, 3, FARBE(0xC7D0DC));
}

static void tisch(Gfx &g) {
  g.kontur(1.6f, OUT);
  g.rundRechteck(110, TISCH_Y, 168, 14, 4, HOLZ);
  g.konturAus();
  // Vorderwand der Werkbank (verdeckt die Beine)
  g.kontur(1.6f, OUT);
  g.rundRechteck(118, TISCH_Y + 12, 152, 36, 3, HOLZ_D);
  g.konturAus();
  g.rundRechteck(128, TISCH_Y + 18, 62, 24, 3, FARBE(0x8E5426));
  g.rundRechteck(198, TISCH_Y + 18, 62, 24, 3, FARBE(0x8E5426));
  g.kreis(159, TISCH_Y + 30, 2.5f, GOLD);
  g.kreis(229, TISCH_Y + 30, 2.5f, GOLD);
  g.rechteck(122, TISCH_Y + 48, 10, 6, HOLZ_D);
  g.rechteck(256, TISCH_Y + 48, 10, 6, HOLZ_D);
  g.rechteck(110, TISCH_Y + 2, 168, 3, FARBE(0xC07A3E));
}

// Stoff (Jacke) auf dem Tisch, an der genaeht wird
static void jacke(Gfx &g) {
  g.kontur(1.3f, OUT);
  float j[] = {128, TISCH_Y + 1, 134, TISCH_Y - 9, 150, TISCH_Y - 12, 166, TISCH_Y - 9, 172, TISCH_Y + 1};
  g.polygon(j, 5, FARBE(0x7E57C2));
  g.konturAus();
  for (int i = 0; i < 3; i++) g.kreis(150, TISCH_Y - 8 + i * 3.5f, 1.1f, GOLD);
}

// =====================================================================
// Ablauf (Sekunden)
// =====================================================================
static const float T_SZENE2 = 34, T_KLATSCH = 46.8f, T_ZAEHL = 48, T_FREUDE = 52, T_GUERTEL = 56, T_STOLZ = 64,
                   DAUER = 376;
static void teil2(Gfx &g, float t);                // Szenen ab dem Aufbruch (schneiderlein_teil2.inc)
static void ton2(Synth &s, float t0, float t1);
static const float BROT_X = 236, BROT_Y = TISCH_Y + 1;

// Flugbahn der Fliege i
static void fliegenPos(int i, float t, float &x, float &y) {
  float an = T_SZENE2 + i * 0.9f;  // kommt durchs Fenster herein
  float u = seg(t, an, an + 2.2f);
  float ox = 70, oy = 75;
  float kreisX = BROT_X + cosf(t * 2.3f + i * 0.9f) * (16 + i * 2), kreisY = BROT_Y - 22 + sinf(t * 3.1f + i * 1.7f) * 9;
  // Verscheuchen
  float weg = huepf(seg(t, 41.6f, 45));
  kreisX += cosf(i * 0.9f) * weg * 40;
  kreisY += sinf(i * 1.3f) * weg * 30 - weg * 20;
  x = lerp(ox, kreisX, sanft(u)) + sinf(u * 9 + i) * 10 * (1 - u);
  y = lerp(oy, kreisY, sanft(u)) + cosf(u * 7 + i) * 8 * (1 - u);
}

static float zuckeHand(float t, float a, float b) { return t > a && t < b ? fabsf(sinf((t - a) * 8)) : 0; }

static void bild(Gfx &g, float t) {
  // ---------- Szene "Guertel" (Nahaufnahme) ----------
  if (t >= T_GUERTEL && t < T_STOLZ) {
    float u = t - T_GUERTEL;
    g.verlauf(0, 240, FARBE(0xF8E2B8), FARBE(0xE8B77E));
    for (int i = 0; i < 10; i++) {
      float a = u * 0.3f + i * 0.628f;
      g.linie(142, 120, 142 + cosf(a) * 300, 120 + sinf(a) * 300, 18, FARBE(0xFFF1D0), 90);
    }
    float bandY = 102;
    g.kontur(2, OUT);
    g.rundRechteck(-10, bandY, 304, 44, 8, FARBE(0x8A4E26));
    g.konturAus();
    g.rechteck(-10, bandY + 6, 304, 1.5f, FARBE(0xC98A45));
    g.rechteck(-10, bandY + 37, 304, 1.5f, FARBE(0xC98A45));
    for (int i = 0; i < 30; i++) {
      g.rechteck(i * 10, bandY + 3, 5, 1.5f, GOLD, 200);
      g.rechteck(i * 10 + 3, bandY + 40, 5, 1.5f, GOLD, 200);
    }
    const char *spruch = "Siebene auf einen Streich!";
    int n = (int)klemme((u - 0.6f) / 0.26f, 0, 26);
    char teil[32];
    snprintf(teil, sizeof teil, "%.*s", n, spruch);
    int voll = g.textBreite(SCHRIFT_M, spruch);
    float tx = 142 - voll / 2.0f;
    g.text(SCHRIFT_M, teil, tx + 1.5f, bandY + 30.5f, FARBE(0x5A2E12));
    g.text(SCHRIFT_M, teil, tx, bandY + 29, GOLD);
    // Nadel mit Faden an der Schreibstelle
    float nx = tx + g.textBreite(SCHRIFT_M, teil) + 4, ny = bandY + 14 + sinf(u * 18) * 6;
    if (n < 26) {
      g.kurve(nx, ny, nx + 30, ny - 50, nx + 60, ny - 70, 1.4f, GOLD);
      g.linie(nx - 3, ny + 8, nx + 6, ny - 14, 2.2f, FARBE(0xC7D0DC));
    }
    // Sterne beim Fertigwerden
    if (u > 7) for (int i = 0; i < 8; i++) {
        float a = i * 0.785f + u;
        float r = 40 + (u - 7) * 120;
        g.stern(142 + cosf(a) * r, 124 + sinf(a) * r * 0.6f, 6, 2.5f, 5, u * 2, GOLD);
      }
    return;
  }

  if (t >= T_STOLZ) { teil2(g, t); return; }

  // ---------- Werkstatt (Szene 1 und 2) ----------
  // langsamer Zoom aufs Brot, als die Fliegen kommen; danach zurueck
  float z1 = 0.18f * sanft(seg(t, 29, 33)) - 0.18f * sanft(seg(t, 36.5f, 39));
  float z2 = 0.5f * sanft(seg(t, T_KLATSCH + 0.9f, T_ZAEHL)) - 0.5f * sanft(seg(t, T_FREUDE, T_FREUDE + 1.2f));
  {
    float zoom = 1 + z1 + z2;
    float cx = lerp(142, 190, z1 / 0.18f) + z2 / 0.5f * (BROT_X - 6 - 142);
    float cy = lerp(120, 115, z1 / 0.18f) + z2 / 0.5f * (BROT_Y - 22 - 120);
    // nicht ueber den Rand der Kulisse hinaus
    float hw = 142 / zoom, hh = 120 / zoom;
    cx = klemme(cx, hw, 284 - hw);
    cy = klemme(cy, hh, 240 - hh);
    g.kamera(cx, cy, zoom);
  }
  // Schlag: wackeln
  if (t > T_KLATSCH && t < T_KLATSCH + 0.45f) {
    float w = 1 - (t - T_KLATSCH) / 0.45f;
    g.wackeln(sinf(t * 90) * 5 * w, cosf(t * 70) * 4 * w);
  }

  float frauX = -60, frauWinke = 0;
  if (t > 8 && t < 22) frauX = lerp(-20, 56, sanft(seg(t, 8, 12))) + sanft(seg(t, 18.5f, 22)) * 90;
  if (t > 13 && t < 19) frauWinke = t;
  werkstattHinten(g, t, frauX, frauWinke);

  // Pose des Schneiderleins
  Pose p;
  bool naeht = (t > 2 && t < 13) || (t > 25.5f && t < 40);
  float stich = naeht ? fmodf(t, 0.8f) / 0.8f : 0;
  p.armV = naeht ? 1.2f + sinf(stich * 6.2831853f) * 0.55f : 1.0f;
  p.armH = 0.9f;
  p.blick = -0.6f;
  if (t > 12 && t < 19) { p.armH = 2.4f + sinf(t * 11) * 0.35f; p.blick = -1; p.mund = 0.5f; }  // winkt zum Fenster
  if (t > 20.5f && t < 24) { p.armV = 1.5f + sinf(t * 14) * 0.25f; p.blick = 1; }            // streicht Mus
  if (t > 24 && t < 25.5f) { p.zu = true; p.mund = 0.8f; }                                      // lecker!
  if (t > 38.5f) { p.blick = 1; p.boese = sanft(seg(t, 38.5f, 40)); }                          // die Fliegen!
  if (t > 41.5f && t < 43.5f) p.armV = 2.2f + sinf(t * 20) * 0.6f;                              // husch!
  if (t > 44.5f && t < T_KLATSCH) { p.armV = lerp(2.2f, 3.0f, seg(t, 44.5f, 45.2f)); p.tuch = 1; }  // holt aus
  if (t >= T_KLATSCH - 0.15f && t < T_KLATSCH + 1) { p.armV = 1.75f; p.tuch = 1; }               // KLATSCH
  if (t > T_KLATSCH + 1) { p.boese = 0; p.mund = 0.9f; p.blick = 1; }                           // staunt
  if (t > T_FREUDE) { p.zu = fmodf(t, 0.6f) < 0.3f; p.armV = 2.8f; p.armH = 2.8f; }            // jubelt
  // blinzeln
  if (!p.zu && fmodf(t + 1.3f, 4.2f) < 0.12f) p.zu = true;
  float hop = t > T_FREUDE ? huepf(fmodf(t, 0.6f) / 0.6f) * 10 : 0;
  float dir = t > 20 && t < 24 ? 1 : -1;  // dreht sich zum Brot
  if (t > 38.5f) dir = 1;
  float sx = dir > 0 ? 200 : 205;
  schneiderlein(g, sx, FY - hop, 1, dir, p, nullptr, nullptr, 1);
  tisch(g);
  jacke(g);
  schneiderlein(g, sx, FY - hop, 1, dir, p, nullptr, nullptr, 2);
  // Faden beim Naehen
  if (naeht && dir < 0) {
    float l = 15, w = p.armV;
    float hx = sx + 9 * dir + sinf(w) * l * dir, hy = FY - hop - 38 + cosf(w) * l;
    g.linie(hx, hy, 152, TISCH_Y - 8, 0.9f, FARBE(0xFFF1D0));
    g.linie(hx - 2, hy - 3, hx + 1, hy + 4, 1.6f, FARBE(0xC7D0DC));
  }
  // Pfeif-Noten
  if (naeht)
    for (int i = 0; i < 3; i++) {
      float ph = fmodf(t * 0.45f + i / 3.0f, 1);
      notenzeichen(g, 190 - ph * 30 + sinf(ph * 9) * 5, 112 - ph * 60, i & 1 ? FARBE(0x3E7FDC) : FARBE(0xE8473D),
                   (uint8_t)(230 * huepf(ph)));
    }


  // Glas fliegt zum Tisch, Muenze zurueck
  float glasU = seg(t, 15, 16.6f);
  if (t > 15 && t < 34) {
    float gx = lerp(80, 262, sanft(glasU)), gy = lerp(90, TISCH_Y + 1, glasU) - sinf(glasU * 3.14159f) * 45;
    musglas(g, gx, gy);
  }
  if (t > 16.6f && t < 18.6f) {
    float u = seg(t, 16.6f, 18.4f);
    float mx = lerp(210, 70, u), my = lerp(120, 90, u) - sinf(u * 3.14159f) * 40;
    g.kontur(1, OUT);
    g.kreis(mx, my, 5, GOLD);
    g.konturAus();
    g.kreis(mx - 1.5f, my - 1.5f, 1.5f, WEISS);
  }
  // Brot
  if (t > 19) {
    float bx = lerp(300, BROT_X, hinaus(seg(t, 19, 20)));
    float mus = seg(t, 20.8f, 23.8f);
    brot(g, bx, BROT_Y, mus);
    if (t > 20.5f && t < 24) {  // Messer
      float mx = BROT_X - 10 + sinf(t * 7) * 9;
      g.linie(mx, BROT_Y - 9, mx + 10, BROT_Y - 22, 2.6f, FARBE(0xC7D0DC));
      g.linie(mx + 10, BROT_Y - 22, mx + 15, BROT_Y - 30, 3.5f, HOLZ_D);
    }
  }
  // Herzchen: lecker
  if (t > 24 && t < 26.5f)
    for (int i = 0; i < 4; i++) {
      float u = seg(t, 24 + i * 0.25f, 25.8f + i * 0.25f);
      herz(g, 215 + i * 9 - u * 6, 120 - u * 45, 4 + i, FARBE(0xFF5A7A), (uint8_t)(255 * (1 - u)));
    }

  // Fliegen
  if (t > T_SZENE2) {
    for (int i = 0; i < 7; i++) {
      if (t < T_SZENE2 + i * 0.9f) continue;
      if (t < T_KLATSCH) {
        float fx, fy;
        fliegenPos(i, t, fx, fy);
        fliege(g, fx, fy, t, 0, i);
      } else {
        float lx = BROT_X - 33 + i * 11, ly = BROT_Y - 2 - (i & 1) * 3;
        fliege(g, lx, ly, t, 1, i);
        if (t > T_ZAEHL + i * 0.5f) {  // Zaehlen
          float u = seg(t, T_ZAEHL + i * 0.5f, T_ZAEHL + i * 0.5f + 0.35f);
          char z[4];
          snprintf(z, sizeof z, "%d", i + 1);
          float sx2 = g.zoom();
          (void)sx2;
          // Zahl in Bildschirmkoordinaten ueber der Fliege
          float zx = g.bildX(lx), zy = g.bildY(ly) - 16 - (i & 1) * 16 - federnd(u) * 8;
          g.text(SCHRIFT_M, z, zx * 1 + 0, zy, OUT, 1);
          g.text(SCHRIFT_M, z, zx - 1, zy - 1, GOLD, 1);
        }
      }
    }
  }
  // Klatsch: Sternexplosion und heller Blitz
  if (t > T_KLATSCH && t < T_KLATSCH + 0.8f) {
    float u = seg(t, T_KLATSCH, T_KLATSCH + 0.8f);
    g.stern(BROT_X, BROT_Y - 10, 18 + u * 30, 8 + u * 12, 8, 0.3f, FARBE(0xFFF3A0), (uint8_t)(255 * (1 - u)));
    g.kameraAus();
    g.rechteck(0, 0, 284, 240, WEISS, (uint8_t)(200 * (1 - u)));
  }
  // Freude: Konfetti
  if (t > T_FREUDE) {
    g.kameraAus();
    const Farbe k[] = {FARBE(0xE8473D), FARBE(0x3E7FDC), GOLD, FARBE(0x3DAE5B), FARBE(0xFF7FA8)};
    for (int i = 0; i < 26; i++) {
      float fx = zufall(i) * 284 + sinf(t * 2 + i) * 10;
      float fy = fmodf(zufall(i + 99) * 240 + (t - T_FREUDE) * (50 + zufall(i + 7) * 40), 260) - 10;
      g.rechteck(fx, fy, 4, 6, k[i % 5]);
    }
  }
  // Iris am Anfang
  g.kameraAus();
  blase(g, 92, 24, {"Gut Mus feil! Gut Mus feil!"}, aufzu(t, 12, 14.6f), 84, 60);
  blase(g, 170, 40, {"Hier herauf, liebe Frau!"}, aufzu(t, 14.4f, 16.6f), 196, 96);
  blase(g, 180, 40, {"Das soll mir schmecken -", "erst den Wams fertig machen!"}, aufzu(t, 25.6f, 29.4f), 196, 96);
  blase(g, 150, 40, {"Wer hat euch eingeladen?"}, aufzu(t, 40.2f, 42.8f), 206, 96);
  blase(g, 120, 30, {"Bist du so ein Kerl?", "Das soll die ganze Stadt erfahren!"}, aufzu(t, T_FREUDE + 0.2f, T_FREUDE + 3.8f), 170, 70);
  if (t < 2.5f) g.iris(190, 120, lerp(0, 300, sanft(seg(t, 0, 2.5f))));
  // Uebergang zur Guertel-Nahaufnahme
  if (t > T_GUERTEL - 0.6f) g.rechteck(0, 0, 284, 240, FARBE(0xF8E2B8), (uint8_t)(255 * seg(t, T_GUERTEL - 0.6f, T_GUERTEL)));
}

// =====================================================================
// Musik
// =====================================================================
// Summen der Fliegen (e-Moll, neckisch)
static const Ton3 MEL_SUMM[] = {
  {76, .5, 52}, {79, .5, 0}, {76, .5, 0}, {79, .5, 0}, {78, .5, 47}, {75, .5, 0}, {71, 1, 0},
  {76, .5, 52}, {79, .5, 0}, {83, .5, 0}, {81, .5, 0}, {79, .5, 48}, {76, .5, 0}, {72, 1, 0}};



static void ton(Synth &s, float t0, float t1) {
  spieleMelodie(s, M(MEL_A), 1.0f, T_SZENE2, 0.3f, t0, t1, 1);
  spieleMelodie(s, M(MEL_SUMM), T_SZENE2 + 0.3f, T_KLATSCH - 0.6f, 0.22f, t0, t1, 0.8f);
  spieleMelodie(s, M(MEL_F), T_FREUDE, T_GUERTEL, 0.22f, t0, t1, 1);
  spieleMelodie(s, M(MEL_A), T_GUERTEL + 0.4f, T_STOLZ, 0.26f, t0, t1, 0.8f);

  // Naehstiche: feines Ticken
  for (float x = ceilf(t0 / 0.8f) * 0.8f; x < t1; x += 0.8f)
    if ((x > 2 && x < 13) || (x > 25.5f && x < 40)) s.ton(x + 0.4f, 2900, 0.03f, 0.002f, 0.04f);
  // Frau ruft (zwei helle Toene)
  if (bei(12.1f, t0, t1)) { s.note(12.1f, 84, 0.7f); s.note(12.35f, 88, 0.7f); s.note(12.6f, 84, 0.7f); }
  // Glas landet, Muenze klingt
  if (bei(16.6f, t0, t1)) { s.note(16.6f, 67, 0.9f); s.ton(16.6f, 180, 0.12f, 0.002f, 0.12f, SINUS, GERAEUSCH, 90); }
  if (bei(18.4f, t0, t1)) { s.note(18.4f, 96, 0.8f); s.note(18.5f, 100, 0.6f); }
  // Mus streichen: weiches Wischen
  for (int i = 0; i < 6; i++) {
    float x = 20.9f + i * 0.45f;
    if (bei(x, t0, t1)) s.rauschen(x, 0.25f, BANDPASS, 900, 0.05f, 0.8f, 0.08f, 1600);
  }
  // lecker: aufsteigende Noten
  if (bei(24, t0, t1)) for (int i = 0; i < 4; i++) s.note(24 + i * 0.12f, 79 + i * 2, 0.6f);
  // Fliegen kommen: Triller
  for (int i = 0; i < 7; i++) {
    float x = T_SZENE2 + i * 0.9f;
    if (bei(x, t0, t1)) for (int k = 0; k < 6; k++) s.note(x + k * 0.05f, k & 1 ? 89 : 88, 0.28f);
  }
  // Husch!
  if (bei(41.6f, t0, t1)) for (int k = 0; k < 8; k++) s.note(41.6f + k * 0.04f, 91 - k * 2, 0.4f);
  // Ausholen: steigendes Glissando
  if (bei(44.6f, t0, t1)) for (int k = 0; k < 8; k++) s.note(44.6f + k * 0.09f, 60 + k * 2, 0.5f);
  // KLATSCH
  if (bei(T_KLATSCH, t0, t1)) {
    s.ton(T_KLATSCH, 110, 0.4f, 0.002f, 0.35f, SINUS, GERAEUSCH, 45);
    s.rauschen(T_KLATSCH, 0.2f, TIEFPASS, 1400, 0.35f, 0.7f, 0.002f, 200);
    s.note(T_KLATSCH + 0.02f, 48, 1);
    s.note(T_KLATSCH + 0.02f, 55, 0.8f);
    s.glocke(T_KLATSCH + 0.05f, 84, 0.8f);
  }
  // Zaehlen 1..7: Tonleiter
  const int leiter[] = {72, 74, 76, 77, 79, 81, 83};
  for (int i = 0; i < 7; i++) {
    float x = T_ZAEHL + i * 0.5f;
    if (bei(x, t0, t1)) s.note(x, leiter[i], 1);
  }
  if (bei(T_ZAEHL + 3.6f, t0, t1)) { s.glocke(T_ZAEHL + 3.6f, 84); s.note(T_ZAEHL + 3.6f, 76); s.note(T_ZAEHL + 3.6f, 79); }
  // Guertel sticken: je Buchstabe ein Pling
  for (int i = 0; i < 26; i++) {
    float x = T_GUERTEL + 0.6f + i * 0.26f;
    if (bei(x, t0, t1)) s.ton(x, 2600 + (i % 5) * 200, 0.05f, 0.002f, 0.08f);
  }
  if (bei(T_GUERTEL + 7.2f, t0, t1)) for (int k = 0; k < 4; k++) s.glocke(T_GUERTEL + 7.2f + k * 0.15f, 72 + k * 4, 0.8f);
  // Fortsetzung folgt: Glocken
  ton2(s, t0, t1);
}

// =====================================================================
// Startbild
// =====================================================================
static void titelSzene(Gfx &g, float t) {
  g.verlauf(0, 170, FARBE(0x7CCBFF), FARBE(0xCFEFFF));
  sonne(g, 238, 44, 18, t);
  wolke(g, fmodf(t * 6, 360) - 60, 34, 1);
  wolke(g, fmodf(t * 4 + 180, 360) - 60, 70, 0.7f, 220);
  g.ellipse(50, 210, 170, 70, FARBE(0x8FD46A));
  g.ellipse(250, 220, 160, 70, FARBE(0x7CCB5E));
  for (int i = 0; i < 8; i++) g.kreis(10 + i * 38, 214 + (i & 1) * 6, 5, i & 1 ? FARBE(0xFF7FA8) : FARBE(0xFFD54A));
  Pose p;
  p.guertel = true;
  p.armV = 2.5f + sinf(t * 6) * 0.45f;
  p.mund = 0.4f;
  p.zu = fmodf(t, 3.7f) < 0.12f;
  float hop = huepf(fmodf(t, 1.6f) / 1.6f) * 4;
  schneiderlein(g, 72, 222 - hop, 1.15f, 1, p);
}

static void titelbild(Gfx &g, float t) {
  titelSzene(g, t);
  // Titel
  g.text(SCHRIFT_M, "Das tapfere", 190, 104, OUT, 1);
  g.text(SCHRIFT_M, "Das tapfere", 189, 103, WEISS, 1);
  g.text(SCHRIFT_G, "Schneiderlein", 182, 136, OUT, 1);
  g.text(SCHRIFT_G, "Schneiderlein", 180, 134, GOLD, 1);
  // Start-Knopf
  float b = 1 + sinf(t * 4) * 0.08f;
  g.kontur(2, OUT);
  g.kreis(190, 178, 17 * b, FARBE(0xE8473D));
  g.konturAus();
  float tri[] = {184, 168 * 1.0f, 184, 188, 200, 178};
  g.polygon(tri, 3, WEISS);
}

#include "schneiderlein_teil2.inc"

// Vorschau im Maerchenbuch: das winkende Schneiderlein auf der Wiese
static void vorschau(Gfx &g, float t) { titelSzene(g, t); }

static void symbol(Gfx &g, float x, float y, float t) {
  Pose p;
  p.armV = 2.2f + sinf(t * 3) * 0.4f;
  p.guertel = true;
  p.mund = 0.4f;
  schneiderlein(g, x - 22, y, 0.62f, 1, p);
  for (int i = 0; i < 7; i++) {
    float a = t * 1.5f + i * 0.9f;
    fliege(g, x + 26 + cosf(a) * 20, y - 44 + sinf(a * 1.3f) * 16, t + i, 0, i);
  }
}

const Maerchen SCHNEIDERLEIN = {"Das tapfere Schneiderlein", DAUER, FARBE(0x3DAE5B), bild, ton, titelbild, vorschau, symbol, LIED(MEL_A, 0.36f)};

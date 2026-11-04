#pragma once
// Gemeinsame Bausteine fuer alle Maerchen: Farben, Figuren, Kulissen, Effekte.
// Figuren stehen mit (x, y) auf dem Boden; s = Groesse, dir = +1 schaut nach rechts.
#include "gfx.h"

// ---------- Farben ----------
constexpr Farbe OUT = FARBE(0x3A2418);     // Umrisse
constexpr Farbe HAUT = FARBE(0xFFD7B5);
constexpr Farbe WANGE = FARBE(0xFF8C7A);
constexpr Farbe HAAR = FARBE(0x7A4524);
constexpr Farbe HUT = FARBE(0x3DAE5B);
constexpr Farbe HUT_D = FARBE(0x2C8A45);
constexpr Farbe FEDER = FARBE(0xE8473D);
constexpr Farbe WESTE = FARBE(0x3E7FDC);
constexpr Farbe HEMD = FARBE(0xFFF6E8);
constexpr Farbe HOSE = FARBE(0x8E5B3A);
constexpr Farbe SCHUH = FARBE(0x4A2E1E);
constexpr Farbe GOLD = FARBE(0xFFD54A);
constexpr Farbe WEISS = FARBE(0xFFFFFF);
constexpr Farbe SCHWARZ = FARBE(0x1E1A2E);
constexpr Farbe MUS = FARBE(0xD8283F);
constexpr Farbe HOLZ = FARBE(0xA8652F);
constexpr Farbe HOLZ_D = FARBE(0x7E4620);

constexpr Farbe GRAS = FARBE(0x7CCB5E), GRAS_H = FARBE(0x9ADB74), GRAS_D = FARBE(0x5DAE48);
constexpr Farbe STEIN = FARBE(0xA3A8B5), STEIN_D = FARBE(0x7D8392);
constexpr Farbe RIESE_HAUT = FARBE(0xF2B48C), RIESE_BART = FARBE(0xB5652E);

// ---------- Figuren ----------
// Aussehen der Grundfigur (Standard = das Schneiderlein)
struct Aussehen {
  Farbe weste = WESTE, hemd = HEMD, hose = HOSE, haar = HAAR;
  Farbe hut = HUT, hutD = HUT_D, feder = FEDER;
  int muetze = 0;            // 0 Zipfelmuetze mit Feder, 1 keine (Strubbelhaare), 2 Schlafmuetze
  Farbe halstuch = 0;        // 0 = keins
};

struct Pose {
  float lauf = -1;      // Laufphase 0..1, -1 = steht
  float armV = 0.4f;    // vorderer Arm: Winkel (0 = haengt, 1.57 = waagrecht nach vorn, 3 = hoch)
  float armH = 0.2f;    // hinterer Arm
  float mund = 0;       // 0 Laecheln, 1 offen (staunen/lachen)
  float boese = 0;      // Augenbrauen 0..1
  float blick = 0;      // Pupillen -1..1 (nach hinten/vorn)
  bool zu = false;      // Augen zu (blinzeln, freuen)
  bool guertel = false;
  bool ranzen = false;  // Beutel auf dem Ruecken (Kaese und Vogel)
  float tuch = 0;       // 1 = haelt ein rotes Tuch in der vorderen Hand
  const Aussehen *aussehen = nullptr;  // nullptr = Schneiderlein
};
struct RiesePose {
  bool sitzt = false;
  float lauf = -1;
  float armV = 0.3f, armH = 0.2f;   // wie beim Schneiderlein
  float mund = 0;                    // 0 grinsen, 1 offen
  bool zu = false;                   // Augen zu (schlafen)
  float boese = 0, staunt = 0;
  float kopf = 0;                    // Kopf neigen (Bogenmass)
  int farbe = 0;                     // 0 gruen, 1 blau (zweiter Riese), 2 blasser Geist
};

void arm(Gfx &g, float sx, float sy, float s, float dir, float winkel, float &hx, float &hy, Farbe aermel = HEMD);
void schneiderlein(Gfx &g, float x, float y, float s, float dir, const Pose &p, float *handX = nullptr,
                          float *handY = nullptr, int teil = 0);
void bauersfrau(Gfx &g, float x, float y, float winke);
void musglas(Gfx &g, float x, float y, float s = 1);
void brot(Gfx &g, float x, float y, float mus);
void fliege(Gfx &g, float x, float y, float t, float liegt = 0, int nr = 0);
void notenzeichen(Gfx &g, float x, float y, Farbe c, uint8_t a);
void herz(Gfx &g, float x, float y, float r, Farbe c, uint8_t a);
void wolke(Gfx &g, float x, float y, float s, uint8_t a = 255);
void sonne(Gfx &g, float x, float y, float r, float t);
void himmel(Gfx &g, float nacht, float t, float bis = 190);
void laubbaum(Gfx &g, float x, float y, float s, Farbe krone = FARBE(0x4CB85A));
void tanne(Gfx &g, float x, float y, float s, Farbe c = FARBE(0x2F8F4E));
void huegel(Gfx &g, float scroll, float f, float y, float h, float abstand, Farbe c);
void blumen(Gfx &g, float scroll, float y, int n, int seed);
void boden(Gfx &g, float y, Farbe c = GRAS);
void mond(Gfx &g, float x, float y, float r);
void steinbrocken(Gfx &g, float x, float y, float r);
void kaese(Gfx &g, float x, float y, float s);
void vogel(Gfx &g, float x, float y, float s, float flug, float dir);
void tropfen(Gfx &g, float x, float y, float r, Farbe c, uint8_t a = 255);
void staubwolke(Gfx &g, float x, float y, float s, float t, uint8_t a = 220);
void fragezeichen(Gfx &g, float x, float y, float s, uint8_t a = 255);
void zzz(Gfx &g, float x, float y, float t, uint8_t a = 255);
void riesenArm(Gfx &g, float sx, float sy, float s, float dir, float w, Farbe hemd, float *hx = nullptr,
                      float *hy = nullptr);
void riese(Gfx &g, float x, float y, float s, float dir, const RiesePose &p, float *handX = nullptr,
                  float *handY = nullptr);
void einhorn(Gfx &g, float x, float y, float s, float lauf, float dir, float senken = 0, float zappel = 0);
void wildschwein(Gfx &g, float x, float y, float s, float lauf, float dir, float wut = 1);
void koenig(Gfx &g, float x, float y, float s, float dir, float winke);
void prinzessin(Gfx &g, float x, float y, float s, float dir, float winke);
void kapelle(Gfx &g, float x, float y, float tuer, float wackel);
void schloss(Gfx &g, float x, float y, float t, float nacht);
void feuerwerk(Gfx &g, float x, float y, float u, Farbe c);
void blende(Gfx &g, float u, float dauer, float cx = 142, float cy = 120);

#pragma once
// Ein Maerchen ist ein kleiner Film, den die Box live zeichnet und vertont.
#include "gfx.h"
#include "synth.h"

// wird von jeder Sprechblase gesetzt, die gerade lesbar ist (fuer die Zeitlupe beim Lesen)
extern volatile uint8_t blaseLesbar;

#define LIED(x, schlag) x, (int)(sizeof(x) / sizeof(x[0])), schlag

struct Maerchen {
  const char *titel;
  float dauer;                                  // Sekunden
  Farbe farbe;                                  // Farbe im Menue
  void (*bild)(Gfx &g, float t);                // Bild zur Zeit t (wird je Streifen aufgerufen)
  void (*ton)(Synth &s, float t0, float t1);    // alles planen, was in [t0, t1) beginnt
  void (*titelbild)(Gfx &g, float t);           // Startbild, t = laufende Uhr in Sekunden
  void (*vorschau)(Gfx &g, float t);            // kleines Bild im Maerchenbuch (ohne Text, Welt 284 x 240)
  void (*symbol)(Gfx &g, float x, float y, float t);  // grosses Erkennungszeichen im Buch, (x, y) = Mitte unten, ca. 110 x 80
  const Ton3 *lied;                             // leise Erkennungsmelodie, solange die Seite offen ist
  int liedN;
  float liedSchlag;                             // Sekunden je Schlag
  const char *autor;                            // fuer das Buchcover (nullptr = Brueder Grimm)
};

extern const Maerchen SCHNEIDERLEIN;
extern const Maerchen FUERCHTEN;
extern const Maerchen ASCHENPUTTEL;
extern const Maerchen ROSENROT;
extern const Maerchen GEISSLEIN;
extern const Maerchen RAPUNZEL_M;
extern const Maerchen SCHNEEWITTCHEN;
extern const Maerchen CINDERELLA;
extern const Maerchen MAXMORITZ;
extern const Maerchen ROTKAEPPCHEN;
extern const Maerchen BREMEN;
extern const Maerchen FRAUHOLLE;
extern const Maerchen HAENSEL_GRETEL;
extern const Maerchen DORNROESCHEN;
extern const Maerchen FROSCHKOENIG;
extern const Maerchen HASE_IGEL;
extern const Maerchen STERNTALER;
extern const Maerchen SUESSERBREI;
extern const Maerchen ENTLEIN;
extern const Maerchen ERBSE;
extern const Maerchen RUMPELSTILZCHEN;
extern const Maerchen KAISER_KLEIDER;
extern const Maerchen SCHWEINCHEN;
extern const Maerchen GOLDLOECKCHEN;
extern const Maerchen TISCHLEIN;
extern const Maerchen KATER;
extern const Maerchen HANS_GLUECK;
extern const Maerchen DROSSELBART;
extern const Maerchen ZERTANZT;
extern const Maerchen DAEUMELINCHEN;
extern const Maerchen KRAUTESEL;
extern const Maerchen KATHERLIESCHEN;
extern const Maerchen FROST;
extern const Maerchen TALER;
extern const Maerchen SCHNEEKOENIGIN;

#pragma once
// Ein Maerchen ist ein kleiner Film, den die Box live zeichnet und vertont.
#include "gfx.h"
#include "synth.h"

struct Maerchen {
  const char *titel;
  float dauer;                                  // Sekunden
  Farbe farbe;                                  // Farbe im Menue
  void (*bild)(Gfx &g, float t);                // Bild zur Zeit t (wird je Streifen aufgerufen)
  void (*ton)(Synth &s, float t0, float t1);    // alles planen, was in [t0, t1) beginnt
  void (*titelbild)(Gfx &g, float t);           // Startbild, t = laufende Uhr in Sekunden
  void (*vorschau)(Gfx &g, float t);            // kleines Bild im Maerchenbuch (ohne Text, Welt 284 x 240)
};

extern const Maerchen SCHNEIDERLEIN;
extern const Maerchen FUERCHTEN;
extern const Maerchen ASCHENPUTTEL;
extern const Maerchen ROSENROT;

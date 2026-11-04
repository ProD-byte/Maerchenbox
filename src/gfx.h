#pragma once
// Kleine Grafik-Engine fuer das 284 x 240 Display.
//
// Gezeichnet wird streifenweise: Das Bild wird in Streifen (z. B. 40 Zeilen) zerlegt, fuer jeden
// Streifen wird die ganze Szene gezeichnet (alles ausserhalb wird sofort weggeschnitten) und der
// Streifen per DMA zum Display geschickt, waehrend der naechste entsteht.
//
// Alle Flaechen sind Polygone mit weichen Kanten (4-fache Unterabtastung in y, exakte Deckung in x).
// Weltkoordinaten: Szenen zeichnen in einer 284 x 240 Welt; die Kamera (kamera()) verschiebt/zoomt.

#include <stdint.h>
#include <math.h>

typedef uint16_t Farbe;  // RGB565
constexpr Farbe RGB(uint8_t r, uint8_t g, uint8_t b) {
  return (Farbe)(((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3));
}
constexpr Farbe FARBE(uint32_t c) { return RGB((c >> 16) & 255, (c >> 8) & 255, c & 255); }
Farbe mischeFarbe(Farbe a, Farbe b, float u);  // a -> b, u = 0..1

struct Glyph { uint16_t code; uint8_t w, h; int8_t x, y; uint8_t adv; uint32_t off; };
struct Schrift { const Glyph *g; const uint8_t *bits; uint16_t n; uint8_t asc, desc; };

class Gfx {
 public:
  static constexpr int W = 284, H = 240;
  static constexpr int MAXP = 96;  // max. Ecken je Polygon

  // Streifen setzen: buf hat (yb - ya) Zeilen; Pixel in Display-Byte-Reihenfolge
  void streifen(uint16_t *buf, int ya, int yb);

  // Kamera: Weltpunkt (cx, cy) in die Bildmitte, zoom 1 = 1 Welt-Pixel je Bildpunkt
  void kamera(float cx, float cy, float zoom);
  void kameraAus() { kamera(W / 2, H / 2, 1); }
  // Welt so abbilden: Bildschirm = Welt * zoom + (x, y)  (z. B. fuer ein kleines Bild im Bild)
  void ansicht(float x, float y, float zoom) { sc = zoom; ox = x; oy = y; }
  // Wackeln (Bildschirm-Pixel), z. B. beim Schlag
  void wackeln(float dx, float dy) { wx = dx; wy = dy; }

  // --- Flaechen (Weltkoordinaten) ---
  void fuellen(Farbe c);
  void verlauf(float ya, float yb, Farbe oben, Farbe unten);  // waagrechte Baender, ganze Breite
  void rechteck(float x, float y, float w, float h, Farbe c, uint8_t a = 255);
  void rundRechteck(float x, float y, float w, float h, float r, Farbe c, uint8_t a = 255);
  void kreis(float cx, float cy, float r, Farbe c, uint8_t a = 255);
  void ellipse(float cx, float cy, float rx, float ry, Farbe c, uint8_t a = 255, float winkel = 0);
  void polygon(const float *xy, int n, Farbe c, uint8_t a = 255);
  void linie(float x0, float y0, float x1, float y1, float dicke, Farbe c, uint8_t a = 255);  // runde Enden
  void bogen(float cx, float cy, float r, float w0, float w1, float dicke, Farbe c, uint8_t a = 255);
  // geschwungene Linie durch 3 Punkte (quadratische Kurve)
  void kurve(float x0, float y0, float x1, float y1, float x2, float y2, float dicke, Farbe c, uint8_t a = 255);
  void stern(float cx, float cy, float r1, float r2, int zacken, float winkel, Farbe c, uint8_t a = 255);

  // --- Text (Bildschirmkoordinaten, unabhaengig von der Kamera) ---
  // ausrichtung: 0 links, 1 Mitte, 2 rechts; y = Grundlinie
  void text(const Schrift &s, const char *utf8, float x, float y, Farbe c, int ausrichtung = 0, uint8_t a = 255);
  int textBreite(const Schrift &s, const char *utf8);

  // Kontur-Hilfe: Ist sie aktiv, zeichnen kreis/ellipse/... zuerst eine etwas groessere dunkle Flaeche.
  void kontur(float dicke, Farbe c) { kDicke = dicke; kFarbe = c; }
  void konturAus() { kDicke = 0; }

  // Ausschnitt (Weltkoordinaten): nur innerhalb wird gezeichnet
  void ausschnitt(float x, float y, float w, float h);
  void ausschnittAus() { kx0 = 0; ky0 = 0; kx1 = W; ky1 = H; }
  // schwarze Blende ausserhalb eines Kreises (Bildschirmkoordinaten), wie am Filmanfang/-ende
  void iris(float cx, float cy, float r, Farbe c = 0);

  float zoom() const { return sc; }
  float bildX(float x) const { return X(x); }  // Welt -> Bildschirm
  float bildY(float y) const { return Y(y); }

 private:
  uint16_t *buf = nullptr;
  int y0 = 0, y1 = 0;
  int kx0 = 0, ky0 = 0, kx1 = W, ky1 = H;
  float sc = 1, ox = 0, oy = 0, wx = 0, wy = 0;
  float kDicke = 0;
  Farbe kFarbe = 0;
  float pts[MAXP * 2];
  float cov[W + 2];

  inline float X(float x) const { return x * sc + ox + wx; }
  inline float Y(float y) const { return y * sc + oy + wy; }
  void fuellePoly(const float *xy, int n, Farbe c, uint8_t a);  // Bildschirmkoordinaten
  int ellipsePunkte(float cx, float cy, float rx, float ry, float winkel, float *out);
  void zeile(int y, int xa, int xb, Farbe c, uint8_t a);
  bool draussen(float ymin, float ymax) const;  // liegt der y-Bereich ganz ausserhalb?
};

// ---------- Hilfen fuer Animationen ----------
inline float klemme(float v, float a = 0, float b = 1) { return v < a ? a : v > b ? b : v; }
inline float lerp(float a, float b, float u) { return a + (b - a) * u; }
inline float seg(float t, float a, float b) { return klemme((t - a) / (b - a)); }  // 0..1 zwischen a und b
inline float sanft(float u) { u = klemme(u); return u * u * (3 - 2 * u); }
inline float hinaus(float u) { u = klemme(u); return 1 - (1 - u) * (1 - u); }
inline float hinein(float u) { u = klemme(u); return u * u; }
inline float federnd(float u) {  // schiesst ueber und federt zurueck
  u = klemme(u);
  const float c1 = 1.70158f, c3 = c1 + 1;
  return 1 + c3 * powf(u - 1, 3) + c1 * powf(u - 1, 2);
}
inline float huepf(float u) { return fabsf(sinf(u * 3.14159265f)); }  // 0..1..0
// feste Zufallszahl 0..1 je Nummer (jedes Bild gleich)
inline float zufall(uint32_t i) {
  i ^= i >> 16; i *= 0x7feb352d; i ^= i >> 15; i *= 0x846ca68b; i ^= i >> 16;
  return (i & 0xFFFFFF) / 16777216.0f;
}

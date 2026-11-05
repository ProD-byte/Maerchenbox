#pragma once
// Spieluhr-Synthesizer (24 kHz mono): Toene mit Huellkurve, gefiltertes Rauschen fuer Geraeusche,
// kleiner Hall. Nachgebaut nach der WebAudio-Klangerzeugung der Maerchenbox.
//
// Zeiten sind Maerchen-Sekunden (ab Start des Maerchens). Das Maerchen plant seine Toene in
// Bloecken: ton(synth, t0, t1) soll alles ausloesen, was in [t0, t1) beginnt.
#include <stdint.h>

enum Welle : uint8_t { SINUS, DREIECK, SAEGE, RECHTECK };
enum Filter : uint8_t { TIEFPASS, HOCHPASS, BANDPASS };
enum Bus : uint8_t { MUSIK, GERAEUSCH };

class Synth {
 public:
  static constexpr int RATE = 24000;
  static constexpr int STIMMEN = 40;

  void start();                       // Maerchenzeit auf 0, alles leise
  float zeit() const { return (float)pos / RATE; }  // aktuelle Maerchenzeit
  void render(int16_t *aus, int n);   // n Samples erzeugen, Zeit laeuft weiter
  void stille();                      // alle Stimmen aus

  // Ton: Frequenz f (Hz), Anstieg a und Abklingen d in Sekunden, optional Gleiten nach f2
  void ton(float t, float f, float spitze, float a, float d, Welle w = SINUS, Bus bus = GERAEUSCH, float f2 = 0);
  // Rauschen durch ein Filter, optional Filterfrequenz gleitend nach f2
  void rauschen(float t, float dauer, Filter typ, float f, float spitze, float q = 1, float a = 0.002f, float f2 = 0,
                Bus bus = GERAEUSCH);
  // Spieluhr-Note (Midi-Nummer, 60 = c'), staerke 0..1
  void note(float t, int midi, float staerke = 1);
  // Glocke (laengerer Nachklang, Obertoene wie eine Kirchenglocke im Kleinen)
  void glocke(float t, int midi, float staerke = 1);

  static float mtof(float m);

  // Zeitlupe: ton() und rauschen() bekommen Filmzeit; daraus wird Klangzeit = s0 + (t - f0) / tempo.
  // Ab Werk 1:1. Das Abspielen setzt es je Block neu.
  void abbildung(float f0, float s0, float tempo) { abF0 = f0; abS0 = s0; abTempo = tempo; }
  void abbildungAus() { abF0 = 0; abS0 = 0; abTempo = 1; }

 private:
  struct Stimme {
    bool aktiv, rausch;
    uint8_t welle, bus, ftyp;
    uint32_t start, n, na, nd;   // Startsample, Fortschritt, Anstiegs-/Abklinglaenge
    float phase, inc, glide;     // Oszillator
    float env, ka, kd;           // Huellkurve
    float b0, b1, b2, a1, a2, z1, z2, ff, fglide, q;  // Filter
    uint32_t zufall;
  };
  Stimme st[STIMMEN];
  uint32_t pos = 0;
  float abF0 = 0, abS0 = 0, abTempo = 1;
  float wann(float t) const { return abS0 + (t - abF0) / abTempo; }
  Stimme *frei(uint32_t start);
  void filterSetzen(Stimme &s);
  // Hall
  float kamm[4][760];
  int kl[4] = {607, 647, 695, 738}, ki[4] = {0, 0, 0, 0};
  float kz[4] = {0, 0, 0, 0};
  float allp[2][310];
  int al[2] = {303, 240}, ai[2] = {0, 0};
};

// ---------- Melodien ----------
// Note: Midi-Nummer (0 = Pause), Laenge in Schlaegen, Bass-Midi (0 = keiner)
struct Ton3 { uint8_t m; float laenge; uint8_t bass; };
struct Melodie { const Ton3 *t; int n; };
// spielt die Melodie (in Schleife) ab tStart, schlag = Sekunden je Schlag; plant alles in [t0, t1)
void spieleMelodie(Synth &s, const Melodie &mel, float tStart, float tEnde, float schlag, float t0, float t1,
                   float staerke = 1);

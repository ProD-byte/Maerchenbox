// Die Wichtelmaenner - ein Spieluhr-Film fuer die Maerchenbox, nach den Bruedern Grimm (KHM 39, erstes Maerchen).
// Der arme Schuster und das Leder fuer ein einziges Paar Schuhe, die fertigen Schuhe am Morgen,
// die Nacht vor Weihnachten hinter den Kleidern, die zwei Maennlein, die neuen Kleider -
// "Sind wir nicht Knaben glatt und fein?" - und der Tanz zur Tuer hinaus.
#include "erzaehlen.h"

// ---------- Ablauf (Sekunden) ----------
static const float P1 = 0, P2 = 22, P3 = 44, P4 = 66, P5 = 88, P6 = 114, P7 = 138, P8 = 164, PDAUER = 186;

static Aussehen schusterLook() {
  Aussehen a; a.weste = FARBE(0x6E4A2E); a.hemd = FARBE(0xF2E8D0); a.hose = FARBE(0x4A4A5A); a.haar = FARBE(0x8E8E8E);
  a.muetze = 2;
  return a;
}
static Aussehen kaeuferLook() {
  Aussehen a; a.weste = FARBE(0x2E5FAE); a.hemd = WEISS; a.hose = FARBE(0x3A3A4A); a.haar = FARBE(0x3A2A20);
  a.hut = FARBE(0x1E1A22); a.hutD = OUT; a.feder = FARBE(0xE8473D); a.halstuch = GOLD;
  return a;
}
static const Aussehen SCHUSTER = schusterLook(), KAEUFER = kaeuferLook();
static FrauLook frauLook() { FrauLook l; l.kleid = FARBE(0x8E4E6E); l.schuerze = WEISS; l.haar = FARBE(0xA8A8A8); l.frisur = 2; return l; }
static const FrauLook FRAU = frauLook();

// =====================================================================
// Figuren und Dinge
// =====================================================================
static void schuster(Gfx &g, float x, float y, float s, float dir, float t, float mund, float armV = 0.4f, bool zu = false,
                     float armH = 0.2f) {
  Pose p = posis(SCHUSTER, t);
  p.mund = mund;
  p.armV = armV;
  p.armH = armH;
  p.zu = zu || p.zu;
  schneiderlein(g, x, y, s, dir, p);
  // Schuerze aus Leder
  g.rundRechteck(x - 9 * s, y - 34 * s, 18 * s, 24 * s, 3 * s, FARBE(0x8E6A44), 220);
}
static void frauS(Gfx &g, float x, float y, float s, float dir, float t, float mund, float armV = 0.4f, bool zu = false) {
  FrauPose p = fposis(FRAU, t, 2);
  p.mund = mund;
  p.armV = armV;
  p.zu = zu || p.zu;
  frau(g, x, y, s, dir, p);
}
// Wichtelmann: klein, mit Knollnase; angezogen = mit Roeckchen und Zipfelmuetze
static void wichtel(Gfx &g, float x, float y, float s, float dir, float t, int nr, float arbeit, bool angezogen, float huepf = 0,
                    float mund = 0) {
  auto PX = [&](float d) { return x + d * s * dir; };
  auto PY = [&](float d) { return y + d * s; };
  float h = huepf > 0 ? fabsf(sinf(t * 8 + nr)) * 10 * huepf : 0;
  const Farbe haut = FARBE(0xF2C8A8), rock = nr ? FARBE(0x3E8E4E) : FARBE(0xC0392B), muetze = nr ? FARBE(0xC0392B) : FARBE(0x3E7FDC);
  float b = huepf > 0 ? sinf(t * 8 + nr) * 5 : 0;
  g.kontur(1 * s, OUT);
  g.linie(PX(-3), PY(-10 - h), PX(-4 - b), PY(-h), 3 * s, angezogen ? FARBE(0x5A4A3A) : haut);
  g.linie(PX(3), PY(-10 - h), PX(4 + b), PY(-h), 3 * s, angezogen ? FARBE(0x5A4A3A) : haut);
  if (angezogen) { g.ellipse(PX(-4 - b), PY(-1 - h), 3 * s, 2 * s, FARBE(0x3A2418)); g.ellipse(PX(4 + b), PY(-1 - h), 3 * s, 2 * s, FARBE(0x3A2418)); }
  g.ellipse(PX(0), PY(-16 - h), 7 * s, 8 * s, angezogen ? rock : haut);
  float aw = arbeit > 0 ? 1.4f + sinf(t * 14 + nr) * 0.6f : huepf > 0 ? 2.6f + sinf(t * 8) * 0.3f : 0.4f;
  g.linie(PX(5), PY(-20 - h), PX(5) + sinf(aw) * 9 * s * dir, PY(-20 - h) + cosf(aw) * 9 * s, 2.4f * s, angezogen ? rock : haut);
  g.kreis(PX(1), PY(-31 - h), 8 * s, haut);
  g.konturAus();
  g.ellipse(PX(6), PY(-29 - h), 3 * s, 2.4f * s, FARBE(0xE8A08A));
  g.kreis(PX(3), PY(-33 - h), 1.4f * s, SCHWARZ);
  g.ellipse(PX(-2), PY(-24 - h), 5 * s, 3 * s, FARBE(0xE8E8E8));  // Baertchen
  if (mund > 0.05f) g.ellipse(PX(3), PY(-26 - h), 2 * s, (0.6f + mund * 2) * s, FARBE(0x7A2A2A));
  if (angezogen) {
    float m[] = {PX(-8), PY(-35 - h), PX(9), PY(-35 - h), PX(-4), PY(-54 - h)};
    g.polygon(m, 3, muetze);
    g.kreis(PX(-4), PY(-54 - h), 2 * s, WEISS);
  } else g.ellipse(PX(-3), PY(-38 - h), 6 * s, 3 * s, FARBE(0xE8E8E8));
  if (arbeit > 0) {  // Hammer bzw. Nadel
    float hx = PX(5) + sinf(aw) * 9 * s * dir, hy = PY(-20 - h) + cosf(aw) * 9 * s;
    if (nr) g.linie(hx, hy, hx + 6 * s * dir, hy - 6 * s, 1.2f * s, FARBE(0xD8DCE4));
    else { g.linie(hx, hy, hx + 6 * s * dir, hy - 6 * s, 1.6f * s, HOLZ_D); g.rechteck(hx + 4 * s * dir - 2 * s, hy - 9 * s, 5 * s, 4 * s, STEIN_D); }
  }
}
static void schuhe(Gfx &g, float x, float y, float s, float fertig, float t = 0) {
  for (int i = 0; i < 2; i++) {
    float sx = x + i * 16 * s;
    if (fertig < 1) {  // zugeschnittenes Leder
      g.ellipse(sx, y, 7 * s, 3 * s, FARBE(0xA8744A), (uint8_t)(255 * (1 - fertig)));
    }
    if (fertig > 0) {
      uint8_t a = (uint8_t)(255 * klemme(fertig));
      g.kontur(1 * s, OUT);
      g.ellipse(sx, y - 3 * s, 8 * s, 4 * s, FARBE(0x5A3A22), a);
      g.konturAus();
      g.rundRechteck(sx - 7 * s, y - 10 * s, 7 * s, 8 * s, 2 * s, FARBE(0x6E4A2E), a);
      g.kreis(sx + 2 * s, y - 5 * s, 1.2f * s, GOLD, a);
    }
  }
  if (fertig >= 1) g.stern(x + 8 * s, y - 14 * s, 4 * s, 1.4f * s, 4, t * 2, WEISS, (uint8_t)(150 + 100 * sinf(t * 3)));
}
static void kleidchen(Gfx &g, float x, float y, float s) {
  g.rundRechteck(x - 10 * s, y - 16 * s, 9 * s, 12 * s, 2 * s, FARBE(0xC0392B));
  g.rundRechteck(x + 1 * s, y - 16 * s, 9 * s, 12 * s, 2 * s, FARBE(0x3E8E4E));
  g.rechteck(x - 10 * s, y - 4 * s, 20 * s, 4 * s, FARBE(0x3E7FDC));
  float m[] = {x - 4 * s, y - 16 * s, x + 4 * s, y - 16 * s, x, y - 24 * s};
  g.polygon(m, 3, FARBE(0xC0392B));
  for (int i = 0; i < 2; i++) g.ellipse(x - 6 * s + i * 12 * s, y + 2 * s, 3 * s, 2 * s, FARBE(0x3A2418));
}

// die Werkstatt; nacht 0..1, kerze, kleider = Kleider an der Wand (Versteck)
static void werkstatt(Gfx &g, float t, float nacht, bool kerze, bool kleider = true, bool weihnacht = false) {
  g.verlauf(0, 200, mischeFarbe(FARBE(0xD8B88A), FARBE(0x3E3448), nacht), mischeFarbe(FARBE(0xB8986A), FARBE(0x2E2638), nacht));
  g.rechteck(0, 200, 284, 40, mischeFarbe(FARBE(0x8E6A46), FARBE(0x3A2E28), nacht));
  // Fenster
  g.kontur(1.6f, OUT);
  g.rundRechteck(30, 50, 60, 56, 4, HOLZ);
  g.konturAus();
  g.rundRechteck(35, 55, 50, 46, 2, mischeFarbe(FARBE(0x9FD8FF), FARBE(0x22285A), nacht));
  if (nacht > 0.5f) { g.kreis(70, 68, 6, FARBE(0xFFF1B0)); for (int i = 0; i < 6; i++) g.kreis(40 + zufall(i) * 40, 58 + fmodf(t * 8 + i * 7, 40), 1.4f, WEISS); }
  g.rechteck(58, 55, 3, 46, HOLZ);
  // Werktisch
  g.kontur(1.6f, OUT);
  g.rechteck(110, 160, 110, 10, HOLZ);
  g.konturAus();
  g.rechteck(118, 170, 6, 32, HOLZ_D);
  g.rechteck(206, 170, 6, 32, HOLZ_D);
  // Leisten und Werkzeug an der Wand
  for (int i = 0; i < 4; i++) g.ellipse(130 + i * 20, 110, 6, 3, FARBE(0x8E6A44));
  g.linie(120, 96, 220, 96, 2, HOLZ_D);
  // Kleider an der Wand (das Versteck)
  if (kleider) {
    g.linie(232, 70, 284, 70, 2.4f, HOLZ_D);
    g.rundRechteck(234, 72, 24, 90, 6, FARBE(0x5A6E8E));
    g.rundRechteck(256, 72, 26, 100, 6, FARBE(0x8E4E6E));
  }
  if (kerze) {
    g.rechteck(196, 146, 5, 14, WEISS);
    g.ellipse(198.5f, 141, 2.4f, 4 + sinf(t * 9), FARBE(0xFFB83D));
    g.kreis(198, 142, 50, FARBE(0xFFD27A), 40);
  }
  if (weihnacht) {  // ein Tannenzweig mit Stern
    tanne(g, 260, 210, 0.5f);
    g.stern(260, 160, 5, 2, 5, t, GOLD);
  }
}

// =====================================================================
// Szenen
// =====================================================================
// 1) Leder fuer ein einziges Paar
static void szeneArm(Gfx &g, float u) {
  bool nacht = u > 12;
  werkstatt(g, u, nacht ? seg(u, 12, 14) : 0, nacht);
  bool schneidet = u > 3 && u < 9;
  schuster(g, 90, 222, 0.88f, 1, u, 0, schneidet ? 1.6f + sinf(u * 6) * 0.2f : u > 14 ? 2.6f : 0.4f, u > 14, u > 14 ? 2.6f : 0.2f);
  schuhe(g, 150, 160, 1.1f, 0);
  if (schneidet) g.linie(120, 150, 132 + sinf(u * 6) * 6, 158, 1.4f, STEIN);
  g.kameraAus();
  vers(g, {"Ein Schuster war ohne seine Schuld", "so arm geworden, dass er nur noch", "Leder zu einem Paar Schuhe hatte."}, aufzu(u, 0.4f, 9));
  vers(g, {"Abends schnitt er es zu. Dann legte", "er sich mit gutem Gewissen schlafen."}, aufzu(u, 9.4f, 14.6f));
  blase(g, 110, 50, {"Lieber Gott, morgen früh", "nähe ich die Schuhe."}, aufzu(u, 15, 21.6f), 90, 130);
}

// 2) Am Morgen: die Schuhe sind fertig
static void szeneMorgen(Gfx &g, float u) {
  werkstatt(g, u, 0, false);
  bool kaeufer = u > 10;
  schuster(g, 90, 222, 0.88f, 1, u, u > 1 && u < 8 ? 0.8f : 0.4f, u > 1 && u < 8 ? 2.4f : 0.4f, false, u > 1 && u < 8 ? 2.4f : 0.2f);
  if (!kaeufer || u < 15) schuhe(g, 150, 160, 1.1f, 1, u);
  if (kaeufer) {
    Pose p = posis(KAEUFER, u);
    float k = seg(u, 10, 12);
    p.lauf = k < 1 ? fmodf(u * 1.4f, 1) : -1;
    p.mund = u > 12.4f && u < 17 ? 0.5f : 0;
    p.armV = u > 15 ? 1.6f : 0.4f;
    schneiderlein(g, lerp(320, 210, k), 222, 0.88f, -1, p);
    if (u > 15) { schuhe(g, 190, 176, 0.9f, 1, u); for (int i = 0; i < 4; i++) g.kreis(116 + i * 5, 176 - i * 3, 3, GOLD); }
  }
  g.kameraAus();
  vers(g, {"Am Morgen standen die Schuhe", "fertig auf dem Tisch - kein Stich", "daneben, wie ein Meisterstück!"}, aufzu(u, 0.4f, 9.6f));
  blase(g, 220, 50, {"Die gefallen mir! Ich zahle", "gern mehr als gewöhnlich."}, aufzu(u, 12.4f, 17.6f), 210, 140);
  vers(g, {"Nun konnte er Leder zu zwei", "Paar Schuhen kaufen."}, aufzu(u, 17.8f, 21.6f));
}

// 3) Jede Nacht fertig - der Schuster wird wohlhabend
static void szeneReich(Gfx &g, float u) {
  float nacht = huepf(fmodf(u / 7, 1));
  werkstatt(g, u, nacht, nacht > 0.5f);
  int paare = u < 7 ? 2 : u < 14 ? 4 : 8;
  float fertig = fmodf(u, 7) > 3.5f ? 1 : 0;
  for (int i = 0; i < paare; i++) schuhe(g, 120 + (i % 4) * 24, 160 - (i / 4) * 16, 0.7f, fertig, u + i);
  schuster(g, 80, 222, 0.88f, 1, u, fertig > 0 ? 0.6f : 0.2f, fertig > 0 ? 2.2f : 0.4f, nacht > 0.5f);
  if (u > 14) for (int i = 0; i < 8; i++) g.kreis(60 + (i % 4) * 6, 214 - (i / 4) * 4, 3, GOLD);
  g.kameraAus();
  vers(g, {"Und so ging es jede Nacht: was er", "abends zuschnitt, war am Morgen", "fertig."}, aufzu(u, 0.4f, 10));
  vers(g, {"Bald hatte er sein ehrliches", "Auskommen und wurde ein", "wohlhabender Mann."}, aufzu(u, 10.4f, 21.6f));
}

// 4) Vor Weihnachten: "Wollen wir nicht aufbleiben?"
static void szeneAufbleiben(Gfx &g, float u) {
  float n = seg(u, 0, 3) * 0.8f;
  werkstatt(g, u, n, true, true, true);
  schuhe(g, 150, 160, 1.1f, 0);
  bool versteckt = u > 15;
  float k = seg(u, 13, 15);
  frauS(g, versteckt ? 250 : lerp(170, 250, k), 222, 0.85f, -1, u, (u > 0.4f && u < 8) ? 0.5f + 0.2f * sinf(u * 8) : 0, 0.4f);
  schuster(g, versteckt ? 236 : lerp(100, 236, k), 222, 0.88f, versteckt ? -1 : 1, u, u > 8.4f && u < 12 ? 0.5f : 0, 0.4f);
  if (versteckt) {  // die Kleider wieder davor
    g.rundRechteck(234, 72, 24, 150, 6, FARBE(0x5A6E8E));
    g.rundRechteck(256, 72, 26, 150, 6, FARBE(0x8E4E6E));
    for (int i = 0; i < 2; i++) g.kreis(244 + i * 22, 120, 2, WEISS);
  }
  g.kameraAus();
  blase(g, 190, 50, {"Wie wär's, wenn wir heute Nacht", "aufblieben, um zu sehen, wer", "uns so hilft?"}, aufzu(u, 0.4f, 8), 170, 140);
  blase(g, 110, 50, {"Das ist eine gute Idee!"}, aufzu(u, 8.4f, 12), 100, 130);
  vers(g, {"Es war kurz vor Weihnachten. Sie", "versteckten sich hinter den Kleidern."}, aufzu(u, 13, 21.6f));
}

// 5) Mitternacht: die zwei Wichtelmaenner
static void szeneMitternacht(Gfx &g, float u) {
  werkstatt(g, u, 0.85f, true, false, true);
  g.rundRechteck(234, 72, 24, 150, 6, FARBE(0x5A6E8E));
  g.rundRechteck(256, 72, 26, 150, 6, FARBE(0x8E4E6E));
  for (int i = 0; i < 2; i++) g.kreis(244 + i * 22, 120, 2, WEISS);
  float k = seg(u, 2, 6);
  float fertig = seg(u, 10, 20);
  bool weg = u > 21;
  schuhe(g, 150, 160, 1.1f, fertig, u);
  if (!weg) {
    bool sitzt = k >= 1;
    wichtel(g, sitzt ? 136 : lerp(-20, 136, k), sitzt ? 160 : 222 - huepf(k) * 40, 0.9f, 1, u, 0, sitzt && fertig < 1 ? 1 : 0, false, sitzt ? 0 : 0.6f);
    wichtel(g, sitzt ? 182 : lerp(-40, 182, k), sitzt ? 160 : 222 - huepf(k) * 40, 0.9f, -1, u, 1, sitzt && fertig < 1 ? 1 : 0, false, sitzt ? 0 : 0.6f);
  } else {
    float w = seg(u, 21, 25);
    wichtel(g, lerp(136, -30, w), 222, 0.9f, -1, u, 0, 0, false, 1);
    wichtel(g, lerp(182, -10, w), 222, 0.9f, -1, u, 1, 0, false, 1);
  }
  if (fertig > 0 && fertig < 1) for (int i = 0; i < 3; i++) g.stern(150 + zufall(i + (int)(u * 6)) * 30, 150 + zufall(i * 3 + (int)(u * 6)) * 10, 2, 0.8f, 4, u, GOLD);
  g.kameraAus();
  vers(g, {"Um Mitternacht kamen zwei kleine,", "niedliche nackte Männlein."}, aufzu(u, 0.4f, 8));
  vers(g, {"Sie nähten und klopften so", "flink, dass der Schuster staunte."}, aufzu(u, 8.4f, 20));
  vers(g, {"Als alles fertig war, sprangen", "sie schnell fort."}, aufzu(u, 20.4f, 25.6f));
}

// 6) Die Frau naeht Kleider fuer die Maennlein
static void szeneNaehen(Gfx &g, float u) {
  werkstatt(g, u, 0, false, true, true);
  frauS(g, 170, 222, 0.85f, -1, u, (u > 0.4f && u < 12) ? 0.5f + 0.2f * sinf(u * 8) : 0, u > 12 ? 1.4f + sinf(u * 8) * 0.2f : 0.6f);
  schuster(g, 96, 222, 0.88f, 1, u, u > 12.4f && u < 17 ? 0.4f : 0, u > 12 ? 1.5f + sinf(u * 10) * 0.3f : 0.4f);
  float k = seg(u, 14, 22);
  if (k > 0) kleidchen(g, 150, 158, 1.1f * k + 0.1f);
  g.kameraAus();
  blase(g, 190, 50, {"Die kleinen Männer haben uns", "reich gemacht - und sie laufen", "nackt herum und frieren!"}, aufzu(u, 0.4f, 7), 170, 140);
  blase(g, 190, 50, {"Ich näh ihnen Hemdlein, Rock,", "Wams und Höslein und strick", "jedem ein Paar Strümpfe!"}, aufzu(u, 7.4f, 13), 170, 140);
  blase(g, 110, 50, {"Und ich mach jedem", "ein Paar Schühlein dazu."}, aufzu(u, 13.4f, 17.6f), 96, 140);
  vers(g, {"Am Abend legten sie die Geschenke", "statt des Leders auf den Tisch."}, aufzu(u, 18, 23.6f));
}

// 7) "Sind wir nicht Knaben glatt und fein?"
static void szeneLied(Gfx &g, float u) {
  werkstatt(g, u, 0.85f, true, false, true);
  g.rundRechteck(234, 72, 24, 150, 6, FARBE(0x5A6E8E));
  g.rundRechteck(256, 72, 26, 150, 6, FARBE(0x8E4E6E));
  for (int i = 0; i < 2; i++) g.kreis(244 + i * 22, 120, 2, WEISS);
  bool an = u > 9;
  if (!an) kleidchen(g, 160, 158, 1.2f);
  bool singt = u > 11;
  wichtel(g, 140, 160, 0.9f, 1, u, 0, 0, an, singt ? 0.8f : 0, singt ? 0.5f + 0.3f * sinf(u * 9) : 0.3f);
  wichtel(g, 184, 160, 0.9f, -1, u, 1, 0, an, singt ? 0.8f : 0, singt ? 0.5f + 0.3f * sinf(u * 9 + 1) : 0.3f);
  puff(g, 162, 140, seg(u, 8.4f, 10));
  if (singt) for (int i = 0; i < 3; i++) {
    float ph = fmodf(u * 0.5f + i * 0.33f, 1);
    notenzeichen(g, 162 + sinf(ph * 6) * 20, 120 - ph * 40, GOLD, (uint8_t)(230 * huepf(ph)));
  }
  g.kameraAus();
  vers(g, {"Erst wunderten sich die Männlein -", "dann zogen sie sich geschwind an."}, aufzu(u, 0.4f, 8.4f));
  blase(g, 142, 60, {"Sind wir nicht Knaben", "glatt und fein?", "Was sollen wir länger", "Schuster sein!"}, aufzu(u, 11, 25.6f), 162, 130);
}

// 8) Der Tanz zur Tuer hinaus
static void szeneTanz(Gfx &g, float u) {
  werkstatt(g, u, 0.85f, true, false, true);
  g.rundRechteck(234, 72, 24, 150, 6, FARBE(0x5A6E8E));
  g.rundRechteck(256, 72, 26, 150, 6, FARBE(0x8E4E6E));
  float k = seg(u, 0, 8);
  float x1 = lerp(140, -30, k), x2 = lerp(184, -10, k);
  float y1 = k < 0.5f ? 160 + huepf(k * 2) * 0 : 222, y2 = y1;
  if (k > 0.25f && k < 0.5f) { y1 = lerp(160, 222, (k - 0.25f) * 4); y2 = y1; }
  wichtel(g, x1, y1, 0.9f, -1, u, 0, 0, true, 1, 0.6f);
  wichtel(g, x2, y2, 0.9f, -1, u, 1, 0, true, 1, 0.6f);
  if (u > 9) {  // spaeter: der Schuster im Glueck
    frauS(g, 180, 222, 0.85f, -1, u, 0.5f, 1.2f);
    schuster(g, 120, 222, 0.88f, 1, u, 0.5f, 1.2f);
    herzen(g, 150, 150, u, 4, 20);
  }
  g.kameraAus();
  vers(g, {"Sie hüpften und tanzten über Stühle", "und Bänke - und zur Tür hinaus."}, aufzu(u, 0.4f, 8.6f));
  vers(g, {"Von nun an kamen sie nicht wieder.", "Dem Schuster aber ging es wohl,", "solang er lebte."}, aufzu(u, 9, 18));
  endeBild(g, u, 18.4f, 150, 170);
}

static void bild(Gfx &g, float t) {
  if (t < P2) szeneArm(g, t - P1);
  else if (t < P3) szeneMorgen(g, t - P2);
  else if (t < P4) szeneReich(g, t - P3);
  else if (t < P5) szeneAufbleiben(g, t - P4);
  else if (t < P6) szeneMitternacht(g, t - P5);
  else if (t < P7) szeneNaehen(g, t - P6);
  else if (t < P8) szeneLied(g, t - P7);
  else szeneTanz(g, t - P8);
}

// =====================================================================
// Musik
// =====================================================================
// Wichtellied (D-Dur, flink und spitz)
static const Ton3 MEL_WICHTEL[] = {
  {74, .5, 50}, {76, .25, 0}, {78, .25, 0}, {81, .5, 0}, {78, .5, 0}, {79, .5, 43}, {76, .5, 0}, {74, 1, 0},
  {73, .5, 45}, {76, .25, 0}, {79, .25, 0}, {81, .5, 0}, {79, .5, 0}, {78, .5, 50}, {76, .5, 0}, {74, 1, 0}};
static void klopf(Synth &s, float x) { s.rauschen(x, 0.04f, BANDPASS, 1800, 0.18f); s.ton(x, 900, 0.06f, 0.001f, 0.05f, SINUS, GERAEUSCH); }
static void ton(Synth &s, float t0, float t1) {
  float a = P1;
  spieleMelodie(s, M(MEL_WIEGE), a + 0.5f, a + 21.6f, 0.44f, t0, t1, 0.5f);
  for (int i = 0; i < 6; i++) if (bei(a + 3 + i * 1, t0, t1)) s.rauschen(a + 3 + i, 0.3f, BANDPASS, 2400, 0.05f);  // schneiden
  a = P2;
  if (bei(a + 0.6f, t0, t1)) for (int j = 0; j < 5; j++) s.note(a + 0.6f + j * 0.07f, 84 + j * 2, 0.45f);
  spieleMelodie(s, M(MEL_WICHTEL), a + 1, a + 9.6f, 0.3f, t0, t1, 0.5f);
  if (bei(a + 10, t0, t1)) for (int j = 0; j < 3; j++) s.rauschen(a + 10 + j * 0.2f, 0.05f, BANDPASS, 1400, 0.2f);  // klopf an
  for (int i = 0; i < 4; i++) if (bei(a + 15.4f + i * 0.2f, t0, t1)) s.note(a + 15.4f + i * 0.2f, 88 + (i & 1) * 3, 0.3f);
  spieleMelodie(s, M(MEL_F), a + 12, a + 21.6f, 0.3f, t0, t1, 0.4f);
  a = P3;
  spieleMelodie(s, M(MEL_WICHTEL), a + 0.4f, a + 21.6f, 0.34f, t0, t1, 0.45f);
  a = P4;
  spieleMelodie(s, M(MEL_WIEGE), a + 0.4f, a + 21.6f, 0.44f, t0, t1, 0.45f);
  a = P5;
  for (int i = 0; i < 12; i++) if (bei(a + 0.4f + i * 0.45f, t0, t1)) s.glocke(a + 0.4f + i * 0.45f, 60, 0.3f);  // zwoelf Schlaege
  for (int i = 0; i < 40; i++) if (bei(a + 10 + i * 0.25f, t0, t1)) klopf(s, a + 10 + i * 0.25f);
  spieleMelodie(s, M(MEL_WICHTEL), a + 6, a + 25.6f, 0.26f, t0, t1, 0.5f);
  a = P6;
  spieleMelodie(s, M(MEL_WIEGE), a + 0.4f, a + 23.6f, 0.42f, t0, t1, 0.55f);
  a = P7;
  if (bei(a + 8.4f, t0, t1)) for (int j = 0; j < 6; j++) s.note(a + 8.4f + j * 0.07f, 84 + j * 2, 0.45f);
  spieleMelodie(s, M(MEL_WICHTEL), a + 11, a + 25.6f, 0.24f, t0, t1, 0.7f);
  a = P8;
  spieleMelodie(s, M(MEL_WICHTEL), a + 0.4f, a + 9, 0.22f, t0, t1, 0.6f);
  spieleMelodie(s, M(MEL_F), a + 9, a + 18, 0.3f, t0, t1, 0.6f);
  endeKlang(s, a + 18.8f, t0, t1);
}

// =====================================================================
// Startbild, Vorschau, Erkennungszeichen
// =====================================================================
static void titelSzene(Gfx &g, float t) {
  werkstatt(g, t, 0.85f, true, true, true);
  schuhe(g, 150, 160, 1.1f, 1, t);
  wichtel(g, 132, 160, 0.9f, 1, t, 0, 1, true);
  wichtel(g, 192, 160, 0.9f, -1, t, 1, 1, true);
}
static void titelbild(Gfx &g, float t) { titelSzene(g, t); titelRahmen(g, t, "Die Wichtelmänner", 200, SCHRIFT_M); }
static void vorschau(Gfx &g, float t) { titelSzene(g, t); }
static void symbol(Gfx &g, float x, float y, float t) {
  schuhe(g, x - 26, y - 10, 2.2f, 1, t);
  glitzern(g, x, y - 30, 40, t, 5);
}

const Maerchen WICHTEL = {"Die Wichtelmänner", PDAUER, FARBE(0xC0392B), bild, ton, titelbild, vorschau, symbol,
                          LIED(MEL_WICHTEL, 0.3f), nullptr};

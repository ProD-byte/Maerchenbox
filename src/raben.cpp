// Die sieben Raben - ein Spieluhr-Film fuer die Maerchenbox, nach den Bruedern Grimm (KHM 25, Ausgabe 1857).
// Der Krug faellt in den Brunnen, der Vater flucht, sieben Raben fliegen davon. Das Schwesterchen zieht mit
// Ringlein, Brot, Wasser und Stuehlchen zur Sonne, zum Mond und zu den Sternen, der Morgenstern schenkt das
// Hinkelbeinchen, das Fingerchen am Glasberg (nur angedeutet), das Zwerglein, die Tellerchen und Becherchen.
#include "erzaehlen.h"

// ---------- Ablauf (Sekunden) ----------
static const float P1 = 0, P2 = 22, P3 = 44, P4 = 66, P5 = 88, P6 = 110, P7 = 132, P8 = 154, P9 = 176, P10 = 198,
                   P11 = 220, P12 = 242, P13 = 264, PDAUER = 286;

static Aussehen bruderLook(int i) {
  Aussehen a;
  const Farbe w[] = {FARBE(0x3E7FDC), FARBE(0xE07A2A), FARBE(0x5DAE48), FARBE(0x8E4E6E), FARBE(0xC0392B), FARBE(0x6E5A3A), FARBE(0x2E8E8E)};
  const Farbe h[] = {FARBE(0x8E5A2A), FARBE(0xE0A23C), FARBE(0x3A2A20), FARBE(0xC98A3A), FARBE(0x5A3A26), FARBE(0xF2C94E), FARBE(0x6E4A2E)};
  a.weste = w[i]; a.haar = h[i]; a.muetze = 1;
  return a;
}
static Aussehen vaterLook() {
  Aussehen a; a.weste = FARBE(0x5A4A3A); a.hemd = FARBE(0xE8D8C0); a.hose = FARBE(0x3A3A4A); a.haar = FARBE(0x6E5A4A); a.muetze = 1;
  return a;
}
static const Aussehen BR[] = {bruderLook(0), bruderLook(1), bruderLook(2), bruderLook(3), bruderLook(4), bruderLook(5), bruderLook(6)};
static const Aussehen VATER = vaterLook();
static FrauLook schwLook() {
  FrauLook l; l.kleid = FARBE(0xE8E0F0); l.schuerze = FARBE(0x8EB8E8); l.haar = FARBE(0xF2C94E); l.frisur = 1;
  return l;
}
static FrauLook mutterLook() { FrauLook l; l.kleid = FARBE(0x8E6EA8); l.schuerze = WEISS; l.haar = FARBE(0x8E5A2A); l.frisur = 2; return l; }
static const FrauLook SCHW = schwLook(), MUTTER = mutterLook();
static const Farbe RABE = FARBE(0x2A2A3A), GLAS = FARBE(0xBFE6FF);

// =====================================================================
// Figuren und Dinge
// =====================================================================
static void bruder(Gfx &g, int i, float x, float y, float s, float dir, float t, float mund, float armV = 0.4f, float lauf = -1,
                   float armH = 0.2f) {
  Pose p = posis(BR[i], t, i * 0.7f);
  p.mund = mund;
  p.armV = armV;
  p.armH = armH;
  p.lauf = lauf;
  schneiderlein(g, x, y, s, dir, p);
}
static void schwester(Gfx &g, float x, float y, float s, float dir, float t, float mund, float armV = 0.4f, float lauf = -1,
                      bool weint = false, float armH = 0.2f, bool zu = false) {
  FrauPose p = fposis(SCHW, t);
  p.mund = mund;
  p.armV = armV;
  p.armH = armH;
  p.lauf = lauf;
  p.weint = weint;
  p.zu = zu || p.zu;
  frau(g, x, y, s, dir, p);
}
static void rabe(Gfx &g, float x, float y, float s, float dir, float t, float flug = -1, float mund = 0) {
  auto PX = [&](float d) { return x + d * s * dir; };
  auto PY = [&](float d) { return y + d * s; };
  float f = flug >= 0 ? sinf(flug * 14) : 0.2f;
  if (flug < 0) {
    g.linie(PX(-2), PY(-2), PX(-2), PY(4), 1.6f * s, FARBE(0x5A4A3A));
    g.linie(PX(3), PY(-2), PX(3), PY(4), 1.6f * s, FARBE(0x5A4A3A));
  }
  float sch[] = {PX(-10), PY(-8), PX(-22), PY(-4), PX(-10), PY(-2)};
  g.polygon(sch, 3, RABE);
  g.ellipse(PX(0), PY(-8), 11 * s, 7 * s, RABE);
  float fl[] = {PX(-4), PY(-10), PX(-12), PY(-22 - f * 14), PX(8), PY(-10)};
  g.polygon(fl, 3, FARBE(0x3A3A4E));
  g.kreis(PX(9), PY(-15), 6 * s, RABE);
  float sn[] = {PX(13), PY(-17), PX(21), PY(-15 + mund * 2), PX(13), PY(-12)};
  g.polygon(sn, 3, FARBE(0x5A5A5A));
  g.kreis(PX(10), PY(-17), 1.4f * s, WEISS);
}
static void krug(Gfx &g, float x, float y, float s, float kipp = 0) {
  g.kontur(1 * s, OUT);
  g.ellipse(x, y, 7 * s, 9 * s, FARBE(0xC0632E), 255, kipp);
  g.konturAus();
  g.ellipse(x + sinf(kipp) * 9 * s, y - cosf(kipp) * 9 * s, 3.5f * s, 2 * s, FARBE(0x8E4A22), 255, kipp);
}
static void ring(Gfx &g, float x, float y, float s, float t) {
  g.bogen(x, y, 4 * s, 0, 6.29f, 1.8f * s, GOLD);
  g.stern(x + 3 * s, y - 3 * s, 3 * s, 1 * s, 4, t * 2, WEISS);
}
static void hinkelbein(Gfx &g, float x, float y, float s, float t) {
  g.linie(x - 7 * s, y, x + 7 * s, y, 2.4f * s, FARBE(0xF2EEE4));
  g.kreis(x - 8 * s, y - 1.5f * s, 2 * s, FARBE(0xF2EEE4));
  g.kreis(x - 8 * s, y + 1.5f * s, 2 * s, FARBE(0xF2EEE4));
  g.kreis(x + 8 * s, y, 2.4f * s, FARBE(0xF2EEE4));
  glitzern(g, x, y, 10 * s, t, 3);
}
static void stuehlchen(Gfx &g, float x, float y, float s, Farbe c = HOLZ) {
  g.rechteck(x - 7 * s, y - 10 * s, 14 * s, 3 * s, c);
  g.rechteck(x - 6 * s, y - 7 * s, 2 * s, 7 * s, HOLZ_D);
  g.rechteck(x + 4 * s, y - 7 * s, 2 * s, 7 * s, HOLZ_D);
  g.rechteck(x - 7 * s, y - 20 * s, 2 * s, 12 * s, HOLZ_D);
}
static void gesicht(Gfx &g, float x, float y, float r, float t, int wer, float mund) {  // 0 Sonne, 1 Mond
  if (wer == 0) {
    for (int i = 0; i < 14; i++) {
      float a = t * 0.5f + i * 0.449f;
      float f[] = {x + cosf(a - 0.12f) * r * 1.05f, y + sinf(a - 0.12f) * r * 1.05f, x + cosf(a) * r * 1.55f, y + sinf(a) * r * 1.55f,
                   x + cosf(a + 0.12f) * r * 1.05f, y + sinf(a + 0.12f) * r * 1.05f};
      g.polygon(f, 3, FARBE(0xFF8A2A));
    }
    g.kreis(x, y, r, FARBE(0xFFB83D));
    g.kreis(x, y, r * 0.86f, FARBE(0xFFD54A));
  } else {
    g.kreis(x, y, r * 1.3f, FARBE(0xC8D8F0), 60);
    g.kreis(x, y, r, FARBE(0xE8ECF4));
    for (int i = 0; i < 4; i++) g.kreis(x - r * 0.4f + i * r * 0.25f, y + r * 0.4f - (i & 1) * r * 0.5f, r * 0.12f, FARBE(0xC8CCDC));
  }
  // Augen (boese) und Mund
  for (int k = -1; k <= 1; k += 2) {
    g.ellipse(x + k * r * 0.35f, y - r * 0.15f, r * 0.12f, r * 0.15f, SCHWARZ);
    g.linie(x + k * r * 0.5f, y - r * 0.42f, x + k * r * 0.15f, y - r * 0.32f, r * 0.06f, wer ? FARBE(0x7A8AA8) : FARBE(0xC0502A));
  }
  g.ellipse(x, y + r * 0.4f, r * 0.3f, r * (0.06f + mund * 0.18f), FARBE(0x7A2A2A));
}
static void zwerg(Gfx &g, float x, float y, float s, float dir, float t, float mund, float armV = 0.6f) {
  auto PX = [&](float d) { return x + d * s * dir; };
  auto PY = [&](float d) { return y + d * s; };
  g.kontur(1.2f * s, OUT);
  g.ellipse(PX(0), PY(-14), 11 * s, 14 * s, FARBE(0x6E5A3A));
  g.linie(PX(8), PY(-18), PX(8) + sinf(armV) * 12 * s * dir, PY(-18) + cosf(armV) * 12 * s, 4 * s, FARBE(0x6E5A3A));
  g.kreis(PX(2), PY(-34), 10 * s, HAUT);
  g.konturAus();
  float b[] = {PX(-6), PY(-32), PX(10), PY(-32), PX(2), PY(-14)};
  g.polygon(b, 3, WEISS);
  float m[] = {PX(-10), PY(-38), PX(12), PY(-38), PX(-2), PY(-60)};
  g.polygon(m, 3, FARBE(0xC0392B));
  g.kreis(PX(6), PY(-36), 1.4f * s, SCHWARZ);
  g.ellipse(PX(9), PY(-33), 3 * s, 2 * s, FARBE(0xE8A08A));
  if (mund > 0.05f) g.ellipse(PX(5), PY(-28), 2.4f * s, (0.8f + mund * 2) * s, FARBE(0x7A2A2A));
}

// =====================================================================
// Orte
// =====================================================================
static void stube(Gfx &g, float t) {
  g.verlauf(0, 200, FARBE(0xD8B88A), FARBE(0xB8986A));
  g.rechteck(0, 200, 284, 40, FARBE(0x8E6A46));
  g.kontur(1.6f, OUT);
  g.rundRechteck(190, 50, 60, 54, 4, HOLZ);
  g.konturAus();
  g.rundRechteck(195, 55, 50, 44, 2, FARBE(0x9FD8FF));
  g.rechteck(218, 55, 3, 44, HOLZ);
  g.kontur(1.4f, OUT);
  g.rundRechteck(30, 164, 60, 30, 6, HOLZ);  // Wiege
  g.konturAus();
  g.bogen(60, 194, 30, 3.4f, 6.0f, 2.4f, HOLZ_D);
  g.rundRechteck(40, 156, 40, 14, 6, WEISS);
  g.kreis(48, 160, 5, HAUT);
}
static void brunnenPlatz(Gfx &g, float t, float krugFaellt) {
  himmel(g, 0, t, 200);
  sonne(g, 240, 32, 11, t);
  boden(g, 200);
  laubbaum(g, 40, 204, 0.8f);
  g.kontur(1.6f, OUT);
  g.rechteck(116, 166, 70, 40, STEIN);
  g.konturAus();
  for (int i = 0; i < 3; i++) g.linie(116, 176 + i * 10, 186, 176 + i * 10, 1.4f, STEIN_D);
  g.ellipse(151, 166, 35, 6, FARBE(0x2A3A5A));
  g.rechteck(118, 116, 5, 50, HOLZ_D);
  g.rechteck(179, 116, 5, 50, HOLZ_D);
  float d[] = {110, 120, 151, 100, 192, 120};
  g.kontur(1.2f, OUT);
  g.polygon(d, 3, FARBE(0xA8452E));
  g.konturAus();
  (void)krugFaellt;
}
static void weg(Gfx &g, float t, float scroll, float nacht) {
  himmel(g, nacht, t, 196);
  huegel(g, scroll * 0.3f, 0.025f, 190, 26, 170, mischeFarbe(GRAS_H, FARBE(0x2F4A40), nacht));
  for (int i = 0; i < 4; i++) laubbaum(g, fmodf(i * 120 - scroll * 0.7f + 960, 480) - 60, 196, 0.55f + (i & 1) * 0.15f,
                                       mischeFarbe(FARBE(0x4CB85A), FARBE(0x1F3B45), nacht));
  boden(g, 200, mischeFarbe(GRAS, FARBE(0x2F4A40), nacht));
  g.rechteck(0, 208, 284, 12, mischeFarbe(FARBE(0xD8C49A), FARBE(0x4A4A50), nacht));
}
static void glasberg(Gfx &g, float t, float tor) {
  himmel(g, 0.7f, t, 220);
  for (int i = 0; i < 16; i++) g.stern(zufall(i + 700) * 284, zufall(i + 720) * 90, 2, 0.8f, 4, 0, WEISS, 180);
  float b[] = {10, 210, 90, 70, 150, 40, 210, 80, 280, 210};
  g.kontur(1.6f, FARBE(0x6E9AC8));
  g.polygon(b, 5, GLAS, 210);
  g.konturAus();
  float h[] = {90, 70, 150, 40, 130, 110, 80, 150};
  g.polygon(h, 4, WEISS, 120);
  for (int i = 0; i < 5; i++) g.stern(60 + i * 40, 120 + (i & 1) * 40, 4, 1.2f, 4, t + i, WEISS, (uint8_t)(160 + 90 * sinf(t * 2 + i)));
  g.rechteck(0, 206, 284, 34, FARBE(0x5A6E8E));
  // das Tor
  g.kontur(1.6f, OUT);
  g.rundRechteck(130, 150, 40, 56, 16, FARBE(0x8ECCF0));
  g.konturAus();
  if (tor > 0) {
    g.rundRechteck(130, 150, 40 * tor, 56, 16, FARBE(0x2A3A5A));
    g.kreis(150, 178, 40, FARBE(0xFFE8A0), (uint8_t)(60 * tor));
  }
}
static void zwergStube(Gfx &g, float t, float gegessen, bool ringImBecher) {
  g.verlauf(0, 206, FARBE(0x9ACCE8), FARBE(0x6EA0C8));
  g.rechteck(0, 206, 284, 34, FARBE(0x5A7A9A));
  g.kontur(1.6f, OUT);
  g.rechteck(40, 150, 210, 8, HOLZ);
  g.konturAus();
  g.rechteck(50, 158, 6, 48, HOLZ_D);
  g.rechteck(234, 158, 6, 48, HOLZ_D);
  for (int i = 0; i < 7; i++) {
    float x = 58 + i * 29;
    g.ellipse(x, 148, 10, 3, WEISS);
    if (gegessen <= i / 7.0f) g.ellipse(x, 146, 6, 2, FARBE(0xC8A060));
    g.kontur(0.8f, OUT);
    g.rundRechteck(x + 6, 132, 7, 14, 2, FARBE(0xE8C84A));
    g.konturAus();
  }
  if (ringImBecher) g.bogen(241.5f, 132, 2.4f, 0, 6.29f, 1.2f, WEISS);
}

// =====================================================================
// Szenen
// =====================================================================
static void szeneGeburt(Gfx &g, float u) {
  stube(g, u);
  FrauPose m = fposis(MUTTER, u);
  m.armV = 1.2f;
  frau(g, 110, 222, 0.85f, 1, m);
  Pose v = posis(VATER, u);
  v.mund = u > 12.4f ? 0.5f : 0;
  v.armV = u > 12.4f ? 1.4f : 0.4f;
  schneiderlein(g, 160, 222, 0.88f, -1, v);
  for (int i = 0; i < 7; i++) bruder(g, i, 196 + (i % 4) * 22, 222 - (i / 4) * 8, 0.5f, -1, u, 0.2f);
  g.kameraAus();
  vers(g, {"Ein Mann hatte sieben Söhne und", "wünschte sich so sehr ein Mädchen."}, aufzu(u, 0.4f, 6));
  vers(g, {"Endlich kam eines - aber es war", "klein und schwach und sollte gleich", "die Nottaufe bekommen."}, aufzu(u, 6.4f, 12));
  blase(g, 180, 50, {"Lauf zur Quelle, hol", "Taufwasser im Krug!"}, aufzu(u, 12.4f, 21.6f), 160, 140);
}

static void szeneKrug(Gfx &g, float u) {
  float fall = seg(u, 8, 10);
  brunnenPlatz(g, u, fall);
  for (int i = 0; i < 7; i++) {
    float drueck = u > 4 && u < 8 ? sinf(u * 10 + i) * 4 : 0;
    bool erschreckt = u > 10;
    bruder(g, i, 60 + i * 30 + drueck, 222, 0.6f, i < 3 ? 1 : -1, u, erschreckt ? 0.8f : u > 4 ? 0.6f : 0.3f, erschreckt ? 2.6f : u > 4 && u < 8 ? 1.6f : 0.4f,
           u < 4 ? fmodf(u * 1.6f + i * 0.1f, 1) : -1, erschreckt ? 2.6f : 0.2f);
  }
  if (fall < 1) krug(g, lerp(150, 151, fall), lerp(150, 172, hinein(fall)), 1, fall * 2);
  if (fall > 0.8f && u < 12) for (int i = 0; i < 6; i++) tropfen(g, 151 + cosf(i * 1.05f) * seg(u, 9.6f, 11) * 24, 160 - huepf(seg(u, 9.6f, 11)) * 20, 2.4f, FARBE(0x7EC0EE));
  g.kameraAus();
  vers(g, {"Die anderen sechs liefen mit, und", "jeder wollte zuerst schöpfen..."}, aufzu(u, 0.4f, 7.6f));
  vers(g, {"Da fiel der Krug in den Brunnen.", "Nun wagte keiner heimzugehen."}, aufzu(u, 10.4f, 21.6f));
}

static void szeneFluch(Gfx &g, float u) {
  stube(g, u);
  Pose v = posis(VATER, u);
  v.mund = u > 0.4f && u < 12 ? 0.6f + 0.2f * sinf(u * 9) : 0.9f;
  v.armV = u > 6 && u < 12 ? 2.6f : u > 12 ? 2.8f : 0.4f;
  v.armH = u > 12 ? 2.8f : 0.2f;
  v.boese = u < 12 ? 1 : 0;
  schneiderlein(g, 150, 222, 0.88f, -1, v);
  float k = seg(u, 12, 18);
  for (int i = 0; i < 7; i++) {
    float a = i * 0.9f;
    float x = lerp(190 + i * 8, 330 + i * 16, k) + cosf(a + u * 2) * 10, y = lerp(110, 90 + i * 6, k) + sinf(a + u * 3) * 8;
    if (u > 12) rabe(g, x, y + 40, 0.9f, 1, u, u + i);
  }
  g.kameraAus();
  blase(g, 160, 50, {"Gewiss haben sie's wieder über", "einem Spiel vergessen, die", "gottlosen Jungen!"}, aufzu(u, 0.4f, 6), 150, 140);
  blase(g, 160, 50, {"Ich wollte, dass die Jungen", "alle zu Raben würden!"}, aufzu(u, 6.4f, 11.6f), 150, 140);
  vers(g, {"Kaum war das Wort heraus, flogen", "sieben kohlschwarze Raben davon."}, aufzu(u, 12.4f, 21.6f));
}

static void szeneEntschluss(Gfx &g, float u) {
  stube(g, u);
  FrauPose m = fposis(MUTTER, u);
  m.weint = u < 8;
  frau(g, 200, 222, 0.85f, -1, m);
  schwester(g, 120, 222, 0.8f, 1, u, u > 12.4f ? 0.5f : 0, u > 12.4f ? 1.6f : 0.4f, -1, u > 6 && u < 12);
  g.kameraAus();
  vers(g, {"Die Eltern trösteten sich mit dem", "Töchterchen, das gesund und", "schöner wurde von Tag zu Tag."}, aufzu(u, 0.4f, 6));
  vers(g, {"Erst spät erfuhr es von seinen", "sieben Brüdern - und dass sie", "um seinetwillen fort waren."}, aufzu(u, 6.4f, 12));
  blase(g, 110, 50, {"Ich muss meine Brüder", "erlösen - koste es, was es will!"}, aufzu(u, 12.4f, 21.6f), 120, 140);
}

static void szeneAufbruch(Gfx &g, float u) {
  weg(g, u, u * 30, 0);
  schwester(g, 130, 222, 0.8f, 1, u, 0.2f, 1.2f, fmodf(u * 1.4f, 1));
  // was es mitnimmt
  float x = 120, y = 150;
  ring(g, x - 14, y + 8, 1, u);
  g.ellipse(x + 26, 178, 8, 5, FARBE(0xC88A4A));
  krug(g, x + 30, 196, 0.6f);
  stuehlchen(g, x - 24, 214, 0.8f);
  g.kameraAus();
  vers(g, {"Es nahm nichts mit als ein Ringlein", "der Eltern, einen Laib Brot, ein", "Krüglein Wasser..."}, aufzu(u, 0.4f, 11));
  vers(g, {"...und ein Stühlchen", "für die Müdigkeit."}, aufzu(u, 11.4f, 21.6f));
}

static void szeneSonne(Gfx &g, float u) {
  g.verlauf(0, 240, FARBE(0xFF8A3A), FARBE(0xFFD27A));
  gesicht(g, 190, 90, 46, u, 0, u > 4 ? 0.6f + 0.3f * sinf(u * 6) : 0.2f);
  g.rechteck(0, 206, 284, 34, FARBE(0xE8A040));
  bool flieht = u > 11;
  schwester(g, flieht ? lerp(90, -40, seg(u, 11, 15)) : 90, 222, 0.8f, flieht ? -1 : 1, u, u > 5 ? 0.6f : 0.2f, flieht ? 2.4f : 0.4f,
            flieht ? fmodf(u * 2, 1) : -1);
  for (int i = 0; i < 3; i++) tropfen(g, 80 + i * 10, 140 + fmodf(u * 30 + i * 8, 16), 2, FARBE(0x7EC0EE));
  g.kameraAus();
  vers(g, {"Es ging immer zu, weit, weit, bis", "an der Welt Ende. Da kam es zur", "Sonne, aber die war zu heiß..."}, aufzu(u, 0.4f, 10.6f));
  vers(g, {"...und fürchterlich und fraß die", "kleinen Kinder. Geschwind lief es", "weg und hin zum Mond."}, aufzu(u, 11, 21.6f));
}

static void szeneMond(Gfx &g, float u) {
  g.verlauf(0, 240, FARBE(0x1E2A4E), FARBE(0x3A4A7A));
  for (int i = 0; i < 20; i++) g.stern(zufall(i + 700) * 284, zufall(i + 720) * 200, 2, 0.8f, 4, 0, WEISS, 160);
  gesicht(g, 190, 90, 40, u, 1, u > 8 ? 0.6f + 0.3f * sinf(u * 6) : 0.1f);
  g.rechteck(0, 206, 284, 34, FARBE(0x4A5A7A));
  bool flieht = u > 14;
  schwester(g, flieht ? lerp(90, -40, seg(u, 14, 18)) : 90, 222, 0.8f, flieht ? -1 : 1, u, 0.5f, flieht ? 2.4f : 1.4f,
            flieht ? fmodf(u * 2, 1) : -1, false, flieht ? 0.2f : 1.4f);
  for (int i = 0; i < 6; i++) g.stern(90 + cosf(u + i) * 24, 170 + sinf(u * 1.3f + i) * 24, 2.4f, 1, 6, u, WEISS, 180);
  g.kameraAus();
  vers(g, {"Aber der Mond war gar zu kalt", "und auch grausig und bös."}, aufzu(u, 0.4f, 7.6f));
  blase(g, 190, 150, {"Ich rieche, rieche", "Menschenfleisch!"}, aufzu(u, 8, 13.6f), 190, 130);
  vers(g, {"Da lief es geschwind fort", "und kam zu den Sternen."}, aufzu(u, 14, 21.6f));
}

static void szeneSterne(Gfx &g, float u) {
  g.verlauf(0, 240, FARBE(0x2A3A6E), FARBE(0x5A6EA8));
  g.rechteck(0, 206, 284, 34, FARBE(0x6E7EA8));
  // jeder Stern auf seinem Stuehlchen
  for (int i = 0; i < 5; i++) {
    float x = 30 + i * 56, y = 200;
    stuehlchen(g, x, y, 1.2f, FARBE(0xC8A060));
    bool morgen = i == 3;
    g.stern(x, y - 26 - (morgen ? 4 : 0), morgen ? 16 : 11, morgen ? 6 : 4, 5, sinf(u + i) * 0.2f, morgen ? FARBE(0xFFF0A0) : GOLD);
    g.kreis(x - 3, y - 27, 1.2f, SCHWARZ);
    g.kreis(x + 3, y - 27, 1.2f, SCHWARZ);
    g.bogen(x, y - 24, 3, 0.4f, 2.7f, 1, OUT);
  }
  schwester(g, 110, 222, 0.75f, 1, u, 0.3f, u > 12 && u < 14 ? 1.6f : 0.4f);
  float bk = seg(u, 12, 14);
  if (bk < 1) hinkelbein(g, lerp(198, 126, bk), lerp(150, 168, bk), 1, u);
  g.kameraAus();
  vers(g, {"Die Sterne waren ihm freundlich", "und gut, und jeder saß auf", "seinem besonderen Stühlchen."}, aufzu(u, 0.4f, 6));
  blase(g, 200, 50, {"Wenn du das Beinchen nicht hast,", "kannst du den Glasberg nicht", "aufschließen. Da sind deine Brüder."}, aufzu(u, 6.4f, 12), 198, 150);
  vers(g, {"Das Mädchen wickelte das Beinchen", "wohl in ein Tüchlein und ging fort."}, aufzu(u, 14, 21.6f));
}

static void szeneGlasberg(Gfx &g, float u) {
  float tor = seg(u, 15, 17);
  glasberg(g, u, tor);
  bool sucht = u > 5 && u < 11;
  schwester(g, 110, 222, 0.78f, 1, u, sucht ? 0.6f : 0.2f, sucht ? 1.4f : u > 12 && u < 15 ? 1.8f : 0.4f, -1, u > 8 && u < 13, 0.2f, u > 12 && u < 15);
  if (sucht) g.rechteck(124, 170, 14, 10, WEISS);
  if (u > 12 && u < 16) glitzern(g, 140, 178, 16, u, 5);
  g.kameraAus();
  vers(g, {"Am Glasberg war das Tor verschlossen.", "Sie wickelte das Tüchlein auf -", "aber es war leer!"}, aufzu(u, 0.4f, 11.6f));
  vers(g, {"Da opferte das gute Schwesterchen", "ihr kleines Fingerchen als Schlüssel -", "und das Tor ging glücklich auf."}, aufzu(u, 12, 21.6f));
}

static void szeneZwerg(Gfx &g, float u) {
  zwergStube(g, u, seg(u, 13, 19), u > 19);
  zwerg(g, 216, 222, 1.1f, -1, u, (u > 0.4f && u < 4) || (u > 7.4f && u < 12.6f) ? 0.5f + 0.3f * sinf(u * 9) : 0, u > 7.4f ? 1.4f : 0.6f);
  schwester(g, u > 13 ? lerp(90, 200, seg(u, 13, 19)) : 90, 222, 0.78f, 1, u, (u > 4.2f && u < 7) ? 0.5f : 0.2f, u > 13 ? 1.4f : 0.4f);
  if (u > 18.4f && u < 19.4f) ring(g, 242, lerp(110, 132, seg(u, 18.4f, 19.4f)), 0.8f, u);
  g.kameraAus();
  blase(g, 200, 50, {"Mein Kind, was suchst du?"}, aufzu(u, 0.4f, 4), 216, 160);
  blase(g, 110, 60, {"Ich suche meine Brüder,", "die sieben Raben."}, aufzu(u, 4.2f, 7), 90, 150);
  blase(g, 200, 50, {"Die Herren Raben sind nicht", "zu Haus. Willst du warten,", "so tritt ein."}, aufzu(u, 7.4f, 12.6f), 216, 160);
  vers(g, {"Von jedem Tellerchen aß es ein", "Bröckchen, aus jedem Becherchen", "trank es ein Schlückchen..."}, aufzu(u, 13, 18.4f));
  vers(g, {"...und ins letzte ließ es", "das Ringlein fallen."}, aufzu(u, 18.6f, 21.6f));
}

static void szeneRaben(Gfx &g, float u) {
  zwergStube(g, u, 1, true);
  // die Raben kommen geflogen und setzen sich an den Tisch
  for (int i = 0; i < 7; i++) {
    float k = seg(u, 0.5f + i * 0.4f, 3 + i * 0.4f);
    float x = lerp(320 + i * 20, 58 + i * 29, k), y = lerp(20 + i * 6, 128, sanft(k));
    bool spricht = (i == 0 && u > 4 && u < 9) || (i == 1 && u > 9.2f && u < 14) || (i == 6 && u > 14.2f);
    rabe(g, x, y, 0.85f, -1, u, k < 1 ? u + i : -1, spricht ? 0.5f + 0.5f * sinf(u * 9) : 0);
  }
  zwerg(g, 260, 222, 0.9f, -1, u, 0);
  g.ausschnitt(0, 160, 40, 80);
  g.ausschnittAus();
  g.kameraAus();
  blase(g, 90, 40, {"Wer hat von meinem Tellerchen", "gegessen?"}, aufzu(u, 4, 9), 58, 110);
  blase(g, 110, 40, {"Wer hat aus meinem", "Becherchen getrunken?"}, aufzu(u, 9.2f, 14), 87, 110);
  blase(g, 180, 40, {"Das ist unseres Vaters Ringlein!", "Wäre doch unser Schwesterlein da,", "so wären wir erlöst!"}, aufzu(u, 14.2f, 21.6f), 232, 110);
}

static void szeneErloest(Gfx &g, float u) {
  zwergStube(g, u, 1, false);
  bool mensch = u > 6;
  schwester(g, 60, 222, 0.78f, 1, u, 0.5f, u > 4 ? 2.2f : 0.4f, u < 4 ? fmodf(u * 1.4f, 1) : -1);
  for (int i = 0; i < 7; i++) {
    float x = 96 + i * 24;
    if (!mensch) rabe(g, 58 + i * 29, 128, 0.85f, -1, u);
    else bruder(g, i, x, 222, 0.62f, -1, u, 0.6f, 1.4f + sinf(u * 4 + i) * 0.4f);
  }
  for (int i = 0; i < 7; i++) puff(g, 58 + i * 29, 140, seg(u, 5.4f + i * 0.1f, 7 + i * 0.1f));
  if (mensch) herzen(g, 140, 150, u, 6, 40);
  g.kameraAus();
  vers(g, {"Da trat das Mädchen hinter der Tür", "hervor - und alle Raben bekamen", "ihre menschliche Gestalt wieder."}, aufzu(u, 0.4f, 12));
  vers(g, {"Und sie herzten und küssten", "einander und zogen fröhlich heim."}, aufzu(u, 12.4f, 21.6f));
}

static void szeneHeim(Gfx &g, float u) {
  weg(g, u, 200 + u * 30, 0);
  sonne(g, 240, 34, 11, u);
  schwester(g, 60, 222, 0.75f, 1, u, 0.5f, 1.2f, fmodf(u * 1.4f, 1));
  for (int i = 0; i < 7; i++) bruder(g, i, 90 + i * 24, 222, 0.58f, 1, u, 0.5f, 1.2f, fmodf(u * 1.4f + i * 0.13f, 1));
  herzen(g, 140, 150, u, 4, 40);
  g.kameraAus();
  vers(g, {"Und daheim freuten sich Vater", "und Mutter über alle acht Kinder."}, aufzu(u, 0.4f, 10));
  endeBild(g, u, 12, 140, 170);
}

static void bild(Gfx &g, float t) {
  if (t < P2) szeneGeburt(g, t - P1);
  else if (t < P3) szeneKrug(g, t - P2);
  else if (t < P4) szeneFluch(g, t - P3);
  else if (t < P5) szeneEntschluss(g, t - P4);
  else if (t < P6) szeneAufbruch(g, t - P5);
  else if (t < P7) szeneSonne(g, t - P6);
  else if (t < P8) szeneMond(g, t - P7);
  else if (t < P9) szeneSterne(g, t - P8);
  else if (t < P10) szeneGlasberg(g, t - P9);
  else if (t < P11) szeneZwerg(g, t - P10);
  else if (t < P12) szeneRaben(g, t - P11);
  else if (t < P13) szeneErloest(g, t - P12);
  else szeneHeim(g, t - P13);
}

// =====================================================================
// Musik
// =====================================================================
// Rabenweise (e-Moll, wehmuetig)
static const Ton3 MEL_RABEN[] = {
  {64, 1, 52}, {67, .5, 0}, {71, 1.5, 0}, {69, .5, 45}, {67, .5, 0}, {66, 1, 0}, {64, 1, 52}, {71, 1, 47},
  {72, .5, 45}, {71, .5, 0}, {69, 1, 0}, {67, .5, 48}, {66, .5, 0}, {64, 2, 52}};
static void krah(Synth &s, float x) { s.ton(x, 700, 0.12f, 0.005f, 0.15f, SAEGE, GERAEUSCH, 500); }
static void ton(Synth &s, float t0, float t1) {
  float a = P1;
  spieleMelodie(s, M(MEL_WIEGE), a + 0.5f, a + 21.6f, 0.44f, t0, t1, 0.55f);
  a = P2;
  spieleMelodie(s, M(MEL_KEGEL), a + 0.4f, a + 8, 0.3f, t0, t1, 0.45f);
  if (bei(a + 9.8f, t0, t1)) { s.ton(a + 9.8f, 180, 0.4f, 0.002f, 0.25f, SINUS, GERAEUSCH, 60); s.rauschen(a + 9.8f, 0.4f, TIEFPASS, 700, 0.25f); }
  spieleMelodie(s, M(MEL_SPUK), a + 10.4f, a + 21.6f, 0.42f, t0, t1, 0.35f);
  a = P3;
  spieleMelodie(s, M(MEL_SPUK), a + 0.4f, a + 11.6f, 0.4f, t0, t1, 0.4f);
  if (bei(a + 12, t0, t1)) for (int j = 0; j < 4; j++) s.note(a + 12 + j * 0.15f, 64 - j * 3, 0.6f);
  for (int i = 0; i < 7; i++) if (bei(a + 12.6f + i * 0.4f, t0, t1)) krah(s, a + 12.6f + i * 0.4f);
  for (int i = 0; i < 10; i++) if (bei(a + 13 + i * 0.3f, t0, t1)) s.rauschen(a + 13 + i * 0.3f, 0.1f, BANDPASS, 800, 0.08f);  // Fluegel
  a = P4;
  spieleMelodie(s, M(MEL_RABEN), a + 0.4f, a + 21.6f, 0.42f, t0, t1, 0.55f);
  a = P5;
  spieleMelodie(s, M(MEL_WANDER), a + 0.4f, a + 21.6f, 0.34f, t0, t1, 0.5f);
  a = P6;
  if (bei(a, t0, t1)) s.rauschen(a, 22, BANDPASS, 1200, 0.05f, 1, 1);
  spieleMelodie(s, M(MEL_SPUK), a + 4, a + 21.6f, 0.4f, t0, t1, 0.35f);
  a = P7;
  if (bei(a, t0, t1)) s.rauschen(a, 22, BANDPASS, 600, 0.05f, 0.8f, 1, 1200);
  spieleMelodie(s, M(MEL_N), a + 0.4f, a + 21.6f, 0.44f, t0, t1, 0.4f);
  a = P8;
  for (int i = 0; i < 10; i++) if (bei(a + 0.4f + i * 2, t0, t1)) s.note(a + 0.4f + i * 2, 88 + (i % 3) * 3, 0.3f);
  spieleMelodie(s, M(MEL_WIEGE), a + 0.4f, a + 21.6f, 0.42f, t0, t1, 0.5f);
  a = P9;
  spieleMelodie(s, M(MEL_RABEN), a + 0.4f, a + 14, 0.44f, t0, t1, 0.45f);
  if (bei(a + 15, t0, t1)) for (int j = 0; j < 6; j++) s.glocke(a + 15 + j * 0.12f, 79 + j * 2, 0.4f);
  a = P10;
  spieleMelodie(s, M(MEL_KEGEL), a + 0.4f, a + 21.6f, 0.34f, t0, t1, 0.35f);
  if (bei(a + 19, t0, t1)) s.note(a + 19, 91, 0.6f);  // pling
  a = P11;
  for (int i = 0; i < 10; i++) if (bei(a + 0.5f + i * 0.35f, t0, t1)) s.rauschen(a + 0.5f + i * 0.35f, 0.1f, BANDPASS, 800, 0.08f);
  for (int i = 0; i < 3; i++) if (bei(a + 4 + i * 5, t0, t1)) krah(s, a + 4 + i * 5);
  spieleMelodie(s, M(MEL_RABEN), a + 4, a + 21.6f, 0.42f, t0, t1, 0.4f);
  a = P12;
  if (bei(a + 5.4f, t0, t1)) for (int j = 0; j < 8; j++) s.note(a + 5.4f + j * 0.08f, 79 + j * 2, 0.5f);
  spieleMelodie(s, M(MEL_F), a + 6.4f, a + 21.6f, 0.28f, t0, t1, 0.65f);
  a = P13;
  spieleMelodie(s, M(MEL_WANDER), a + 0.4f, a + 11.6f, 0.3f, t0, t1, 0.65f);
  endeKlang(s, a + 12.4f, t0, t1);
}

// =====================================================================
// Startbild, Vorschau, Erkennungszeichen
// =====================================================================
static void titelSzene(Gfx &g, float t) {
  glasberg(g, t, 0);
  schwester(g, 100, 226, 0.8f, 1, t, 0.3f, 1.2f);
  for (int i = 0; i < 7; i++) rabe(g, 150 + cosf(t * 0.6f + i * 0.9f) * 70, 110 + sinf(t * 0.8f + i * 0.9f) * 30, 0.8f, cosf(t * 0.6f + i * 0.9f + 1.57f) > 0 ? -1 : 1, t, t + i);
}
static void titelbild(Gfx &g, float t) { titelSzene(g, t); titelRahmen(g, t, "Die sieben Raben", 196, SCHRIFT_M); }
static void vorschau(Gfx &g, float t) { titelSzene(g, t); }
static void symbol(Gfx &g, float x, float y, float t) {
  for (int i = 0; i < 3; i++) rabe(g, x - 30 + i * 30, y - 20 - (i == 1 ? 14 : 0), 1.3f, 1, t, t + i);
  ring(g, x, y - 4, 2, t);
}

const Maerchen RABEN = {"Die sieben Raben", PDAUER, FARBE(0x3A3A4E), bild, ton, titelbild, vorschau, symbol,
                        LIED(MEL_RABEN, 0.42f), nullptr};

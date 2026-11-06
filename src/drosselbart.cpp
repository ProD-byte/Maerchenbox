// Koenig Drosselbart - ein Spieluhr-Film fuer die Maerchenbox, nach den Bruedern Grimm.
// Die stolze Prinzessin verspottet alle Freier, muss den Spielmann heiraten, Toepfe verkaufen und in der Kueche helfen -
// bis sich zeigt: Spielmann, Husar und Koenig Drosselbart sind ein und derselbe.
#include "erzaehlen.h"

// ---------- Ablauf (Sekunden) ----------
static const float P1 = 0, P2 = 32, P3 = 58, P4 = 88, P5 = 118, P6 = 154, PDAUER = 180;

static FrauLook prinzLook() { FrauLook l; l.kleid = FARBE(0xE86FA8); l.haar = FARBE(0xF2C14E); l.krone = true; l.glanz = true; return l; }
static FrauLook armLook() { FrauLook l; l.kleid = FARBE(0x9A8A7A); l.schuerze = FARBE(0xE8DCC0); l.haar = FARBE(0xF2C14E); l.frisur = 3; l.tuch = FARBE(0x8E6E5A); l.flicken = true; return l; }
static FrauLook brautLook() { FrauLook l; l.kleid = WEISS; l.haar = FARBE(0xF2C14E); l.krone = true; l.glanz = true; l.rockBreite = 1.4f; l.blume = FARBE(0xFF8FC0); return l; }
static const FrauLook PRINZ = prinzLook(), ARM = armLook(), BRAUT = brautLook();
static Aussehen drosselLook() {
  Aussehen a; a.weste = FARBE(0x2E5FAE); a.hemd = GOLD; a.hose = FARBE(0x1E2E5A); a.haar = FARBE(0x8E5A2A); a.muetze = 3; a.halstuch = WEISS;
  return a;
}
static Aussehen spielLook() { Aussehen a; a.weste = FARBE(0x7A6A5A); a.hemd = FARBE(0xC9B8A0); a.hose = FARBE(0x5A4A3A); a.haar = FARBE(0x8E5A2A); a.hut = FARBE(0x5A4A3A); a.hutD = OUT; a.feder = FARBE(0x9A9A9A); a.wangen = false; return a; }
static Aussehen husarLook() { Aussehen a; a.weste = FARBE(0xC0392B); a.hemd = GOLD; a.hose = FARBE(0x1E2E5A); a.haar = FARBE(0x8E5A2A); a.hut = FARBE(0x1E1A22); a.hutD = OUT; a.feder = WEISS; return a; }
static Aussehen freierLook(int i) {
  Aussehen a;
  const Farbe w[] = {FARBE(0x5A8E4E), FARBE(0xE0A23C), FARBE(0x6E5A8E)};
  a.weste = w[i]; a.hemd = WEISS; a.haar = i == 1 ? FARBE(0x2E2220) : FARBE(0xC98A3A); a.muetze = 3;
  return a;
}
static const Aussehen DROSSEL = drosselLook(), SPIELMANN = spielLook(), HUSAR = husarLook(), F0 = freierLook(0), F1 = freierLook(1), F2 = freierLook(2);

// Koenig Drosselbart mit seinem spitzen Kinnbart (sitzt auf der Figur schneiderlein)
static void drossel(Gfx &g, float x, float y, float s, float dir, const Pose &p) {
  schneiderlein(g, x, y, s, dir, p);
  const Aussehen &L = *p.aussehen;
  auto PX = [&](float d) { return x + d * s * dir; };
  auto PY = [&](float d) { return y + d * s; };
  float b[] = {PX(-6), PY(-46), PX(14), PY(-46), PX(9), PY(-30)};
  g.polygon(b, 3, L.haar);
}

static void geige(Gfx &g, float x, float y, float s, float t, bool spielt) {
  g.kontur(1, OUT);
  g.ellipse(x, y, 7 * s, 5 * s, FARBE(0xA8602A), 255, -0.5f);
  g.konturAus();
  g.linie(x + 5 * s, y - 3 * s, x + 16 * s, y - 10 * s, 2 * s, OUT);
  if (spielt) g.linie(x - 6 * s + sinf(t * 8) * 4, y - 8 * s, x + 8 * s + sinf(t * 8) * 4, y + 4 * s, 1 * s, FARBE(0xE8DCC0));
}

static void topf(Gfx &g, float x, float y, float s, Farbe c) {
  g.kontur(1.1f, OUT);
  float q[] = {x - 9 * s, y - 14 * s, x + 9 * s, y - 14 * s, x + 7 * s, y, x - 7 * s, y};
  g.polygon(q, 4, c);
  g.konturAus();
  g.ellipse(x, y - 14 * s, 9 * s, 2.5f * s, mischeFarbe(c, WEISS, 0.3f));
}

static void scherbe(Gfx &g, float x, float y, float w, Farbe c) {
  float q[] = {x + cosf(w) * 6, y + sinf(w) * 6, x + cosf(w + 2.2f) * 5, y + sinf(w + 2.2f) * 5, x + cosf(w + 4) * 4, y + sinf(w + 4) * 4};
  g.polygon(q, 3, c);
}

static void saal(Gfx &g, float t) {
  g.verlauf(0, 190, FARBE(0xF6E2B4), FARBE(0xE8C483));
  for (int i = 0; i < 4; i++) { float x = 20 + i * 82; g.rechteck(x - 10, 30, 20, 160, FARBE(0xFFF3D6)); g.rechteck(x - 14, 26, 28, 8, FARBE(0xE8D3A2)); }
  for (int i = 0; i < 7; i++) g.kurve(i * 44, 20, i * 44 + 22, 34, i * 44 + 44, 20, 3, FARBE(0xC0392B));
  g.rechteck(0, 190, 284, 50, FARBE(0xC9A46A));
  for (int r = 0; r < 3; r++) for (int c = 0; c < 12; c++) if ((r + c) & 1) g.rechteck(c * 26 - r * 8, 190 + r * 17, 26, 17, FARBE(0xA8823E));
}

static void land(Gfx &g, float t, float scroll, int was) {
  himmel(g, 0, t, 190);
  sonne(g, 248, 30, 11, t);
  wolke(g, fmodf(60 - scroll * 0.2f + 600, 400) - 60, 44, 0.8f);
  huegel(g, scroll * 0.3f, 0.025f, 190, 24, 160, GRAS_H);
  if (was == 0) for (int i = 0; i < 5; i++) tanne(g, fmodf(i * 70 - scroll * 0.7f + 700, 350) - 30, 196, 0.7f);
  if (was == 1) blumen(g, scroll * 0.5f, 200, 10, 7);
  if (was == 2) {  // die grosse Stadt
    for (int i = 0; i < 5; i++) {
      float x = 20 + i * 54, h = 50 + (i % 3) * 16;
      g.kontur(1.2f, OUT);
      g.rechteck(x, 190 - h, 40, h, i & 1 ? FARBE(0xF4E4C4) : FARBE(0xF0D8C0));
      float d[] = {x - 4, 190 - h, x + 20, 166 - h, x + 44, 190 - h};
      g.polygon(d, 3, i % 3 ? FARBE(0xC0503A) : FARBE(0x5B7FC7));
      g.konturAus();
    }
  }
  boden(g, 200);
  g.rechteck(0, 210, 284, 12, FARBE(0xD8C49A));
}

// =====================================================================
// Szenen
// =====================================================================
// 1) Die Freier: "Das Weinfass!" - "Koenig Drosselbart!"
static void szeneFreier(Gfx &g, float u) {
  saal(g, u);
  koenig(g, 34, 214, 0.9f, 1, 0);
  FrauPose p = fposis(PRINZ, u);
  p.armV = (u > 6 && u < 10) || (u > 12 && u < 16) || (u > 18 && u < 26) ? 2.0f : 0.5f;
  p.mund = p.armV > 1 ? 0.7f : 0.1f;
  p.zu = u > 22 && fmodf(u, 0.6f) < 0.3f;  // sie lacht
  frau(g, 84, 214, 0.85f, 1, p);
  const Aussehen *fr[3] = {&F0, &F1, &F2};
  float fx[3] = {142, 190, 230};
  for (int i = 0; i < 3; i++) {
    Pose f = posis(*fr[i], u, i);
    f.mund = 0;
    f.boese = (i == 0 && u > 7) || (i == 1 && u > 13) ? 0.5f : 0;
    float sx = i == 1 ? 1.05f : 0.9f;
    if (i < 2) schneiderlein(g, fx[i], 214, sx * (i == 0 ? 1.08f : 1), -1, f);
    else { Pose d = posis(DROSSEL, u); d.mund = u > 24 ? 0.3f : 0; d.blick = -1; drossel(g, fx[i] + 22, 214, 0.95f, -1, d); }
  }
  g.kameraAus();
  vers(g, {"Eine schöne Prinzessin war so", "stolz, dass ihr kein Freier", "gut genug war."}, aufzu(u, 0.4f, 6));
  blase(g, 110, 50, {"Der Dicke? Ein Weinfass!"}, aufzu(u, 6.2f, 10), 96, 130);
  blase(g, 120, 50, {"Lang und schwank", "hat keinen Gang!"}, aufzu(u, 12, 16), 96, 130);
  blase(g, 130, 44, {"Und der hat ein Kinn wie", "die Drossel einen Schnabel -", "König Drosselbart!"}, aufzu(u, 18, 25), 96, 130);
  vers(g, {"Alle lachten. Seitdem hieß", "er König Drosselbart."}, aufzu(u, 25.4f, 31.6f));
  blende(g, u, 32, 140, 150);
}

// 2) Der Vater: Du heiratest den ersten Bettler - der Spielmann
static void szeneSpielmann(Gfx &g, float u) {
  saal(g, u);
  koenig(g, 60, 214, 1, 1, u > 12 ? 0.6f : 0);
  FrauPose p = fposis(PRINZ, u);
  p.weint = u > 4 && u < 10;
  p.zeit = u;
  p.mund = u > 14 ? 0.1f : 0.3f;
  frau(g, 116, 214, 0.85f, 1, p);
  float k = seg(u, 8, 12);
  Pose s = posis(SPIELMANN, u);
  s.lauf = k > 0 && k < 1 ? fmodf(u * 1.2f, 1) : -1;
  s.armV = 1.4f; s.armH = 1.6f;
  float sx = lerp(320, 196, k);
  schneiderlein(g, sx, 214, 0.95f, -1, s);
  geige(g, sx - 12, 166, 1.1f, u, u > 12 && u < 20);
  for (int i = 0; i < 3; i++) if (u > 12 && u < 20) notenzeichen(g, sx - 30 + i * 12, 130 - fmodf(u * 20 + i * 12, 40), OUT, 200);
  g.kameraAus();
  blase(g, 110, 50, {"Zur Strafe heiratest du den", "ersten Bettler, der kommt!"}, aufzu(u, 0.4f, 7.4f), 66, 130);
  vers(g, {"Da kam ein Spielmann und sang", "unter dem Fenster."}, aufzu(u, 8, 14));
  vers(g, {"Der König hielt Wort: Noch am", "selben Tag war Hochzeit,", "und sie musste mit ihm gehen."}, aufzu(u, 14.4f, 24));
  blende(g, u, 26, 160, 160);
}

// 3) Unterwegs: "Wem gehoert ...?" - "Dem Koenig Drosselbart"
static void szeneWeg(Gfx &g, float u) {
  int was = u < 8.6f ? 0 : u < 17.2f ? 1 : 2;
  land(g, u, u * 26, was);
  FrauPose p = fposis(PRINZ, u);
  p.lauf = fmodf(u * 1.1f, 1);
  p.mund = fmodf(u, 8.6f) < 3 ? 0.5f : 0.1f;
  p.weint = u > 25;
  frau(g, 104, 222, 0.8f, 1, p);
  Pose s = posis(SPIELMANN, u);
  s.lauf = fmodf(u * 1.1f + 0.3f, 1);
  s.mund = fmodf(u, 8.6f) > 3.2f && fmodf(u, 8.6f) < 7.6f ? 0.5f : 0;
  schneiderlein(g, 160, 222, 0.9f, 1, s);
  g.kameraAus();
  const char *ding[3] = {"Wem gehört der schöne Wald?", "Wem gehört die schöne Wiese?", "Wem gehört die schöne Stadt?"};
  int i = was;
  float v = u - i * 8.6f;
  blase(g, 100, 60, {ding[i]}, aufzu(v, 0.4f, 3.2f), 104, 130);
  blase(g, 170, 50, {"Dem König Drosselbart. Hättst du", "ihn genommen, wär er dein."}, aufzu(v, 3.4f, 7.8f), 164, 130);
  vers(g, {"Ach, ich arme Jungfer zart,", "ach, hätt ich genommen", "den König Drosselbart!"}, aufzu(u, 25.6f, 29.8f));
}

// 4) Das kleine Haus, Toepfe auf dem Markt - der Husar reitet hindurch
static void szeneMarkt(Gfx &g, float u) {
  if (u < 9) {
    land(g, u, 0, 0);
    g.kontur(1.6f, OUT);
    g.rechteck(150, 150, 70, 60, FARBE(0xC9A46A));
    float d[] = {140, 152, 185, 120, 230, 152};
    g.polygon(d, 3, FARBE(0x8E6E3E));
    g.konturAus();
    g.rundRechteck(176, 172, 18, 38, 6, HOLZ_D);
    FrauPose p = fposis(PRINZ, u);
    p.mund = 0.6f;
    frau(g, 110, 222, 0.8f, 1, p);
    Pose s = posis(SPIELMANN, u);
    schneiderlein(g, 240, 222, 0.85f, -1, s);
    g.kameraAus();
    blase(g, 100, 60, {"Wem gehört das elende", "kleine Häuschen?"}, aufzu(u, 0.4f, 4), 110, 130);
    blase(g, 200, 60, {"Das ist mein und dein Haus."}, aufzu(u, 4.2f, 8.6f), 240, 130);
    return;
  }
  float v = u - 9;
  land(g, u, 0, 2);
  // Marktstand mit Toepfen
  const Farbe tf[] = {FARBE(0x5BB8E8), FARBE(0xE07A2A), FARBE(0xF6F2EA), FARBE(0x3DAE5B), FARBE(0xE86FA8), FARBE(0xC9742E)};
  float krach = 12;
  for (int i = 0; i < 10; i++) {
    float x = 60 + (i % 5) * 20, y = 214 - (i / 5) * 2;
    if (v < krach) topf(g, x + (i / 5) * 10, y, 0.9f, tf[i % 6]);
    else for (int k = 0; k < 3; k++) {
      float f = seg(v, krach, krach + 1.2f);
      scherbe(g, x + (zufall(i * 3 + k) - 0.5f) * 60 * f, y - huepf(f) * 30 * zufall(i + k * 7) - 2, i + k * 2 + v * f, tf[i % 6]);
    }
  }
  FrauPose p = fposis(ARM, u);
  p.weint = v > krach + 1;
  p.zeit = u;
  p.mund = v > krach ? 0.9f : 0.2f;
  p.armV = v > krach && v < krach + 3 ? 2.8f : 0.6f;
  frau(g, 34, 214, 0.8f, 1, p);
  if (v > 9) {
    float hk = seg(v, 9, 15);
    float hx = lerp(330, -80, hk);
    einhorn(g, hx, 222, 0.9f, fmodf(u * 2.2f, 1), -1, 0, 0, true);
    Pose h = posis(HUSAR, u);
    h.mund = 0.6f;
    schneiderlein(g, hx - 2, 186, 0.75f, -1, h);
  }
  if (v > krach && v < krach + 2) wolkePuff(g, 110, 200, seg(v, krach, krach + 2), FARBE(0xE8D8B8));
  g.kameraAus();
  vers(g, {"Sie konnte nicht kochen, nicht", "flechten, nicht spinnen. So", "verkaufte sie Töpfe auf dem Markt."}, aufzu(v, 0.4f, 9));
  vers(g, {"Da ritt ein Husar mitten", "durch die Töpfe!"}, aufzu(v, 9.4f, 14));
  blase(g, 90, 80, {"Ach, was wird mein", "Mann nur sagen?"}, aufzu(v, 14.4f, 20.6f), 40, 140);
  blende(g, u, 30, 60, 170);
}

// 5) Kuechenmagd im Schloss - der Koenig bittet zum Tanz: "Ich bin der Spielmann!"
static void szeneSchloss(Gfx &g, float u) {
  saal(g, u);
  bool tanz = u > 10;
  // Toepfchen mit Speiseresten in ihrer Schuerzentasche fallen beim Tanzen heraus
  float fall = seg(u, 17, 18.4f);
  FrauPose p = fposis(ARM, u);
  p.mund = u > 17 && u < 24 ? 0.9f : 0.2f;
  p.weint = u > 19 && u < 25;
  p.zeit = u;
  p.drehen = tanz && u < 17 ? sinf(u * 4) : 0;
  p.armV = u > 4 && u < 10 ? 1.6f : 1.0f;
  float fx = tanz ? 120 + (u < 17 ? sinf(u * 1.3f) * 14 : 0) : 70;
  frau(g, fx, 214, 0.85f, 1, p);
  if (fall > 0) {
    for (int i = 0; i < 2; i++) {
      float x = fx + 12 + i * 10 + fall * (i ? 30 : -10), y = lerp(176, 212, hinein(fall));
      topf(g, x, y, 0.6f, i ? FARBE(0x5BB8E8) : FARBE(0xE07A2A));
      if (fall >= 1) for (int k = 0; k < 4; k++) g.kreis(x - 8 + k * 5 + zufall(k + i * 4) * 6, 212, 2.5f, FARBE(0xC9742E));
    }
  }
  Pose d = posis(DROSSEL, u);
  d.armV = tanz ? 1.6f : 0.6f;
  d.mund = (u > 4 && u < 10) || (u > 24 && u < 33) ? 0.5f : 0.1f;
  float dx = tanz ? 168 + (u < 17 ? sinf(u * 1.3f) * 14 : 0) : lerp(320, 210, seg(u, 2, 6));
  drossel(g, dx, 214, 0.95f, -1, d);
  if (u > 20 && u < 23) for (int i = 0; i < 6; i++) g.kreis(40 + i * 40, 120 + sinf(u * 4 + i) * 6, 6, HAUT, 160);  // die Leute lachen
  g.kameraAus();
  vers(g, {"Nun wurde sie Küchenmagd im", "Schloss. Da war die Hochzeit", "des Königssohns."}, aufzu(u, 0.4f, 4));
  blase(g, 200, 60, {"Komm, tanz mit mir!"}, aufzu(u, 6, 9.6f), dx, 130);
  vers(g, {"Da fielen ihr die Töpfchen", "aus der Tasche - alle lachten!"}, aufzu(u, 18, 24));
  blase(g, 170, 46, {"Ich bin der Spielmann, ich bin der", "Husar - und König Drosselbart!", "Alles war, um deinen Stolz zu beugen."}, aufzu(u, 24.4f, 34), dx, 130);
  blende(g, u, 36, 140, 150);
}

// 6) Die richtige Hochzeit
static void szeneHochzeit(Gfx &g, float u) {
  saal(g, u);
  koenig(g, 40, 214, 0.9f, 1, 0.5f + sinf(u * 2) * 0.5f);
  FrauPose p = fposis(BRAUT, u);
  p.drehen = sinf(u * 3) * 0.6f;
  p.mund = 0.4f;
  p.armV = 1.4f;
  frau(g, 124, 214, 0.9f, 1, p);
  Pose d = posis(DROSSEL, u);
  d.armV = 1.4f; d.mund = 0.4f;
  drossel(g, 176, 214, 1, -1, d);
  for (int i = 0; i < 3; i++) { Pose g2 = posis(i ? F0 : F1, u, i); g2.armV = 2.4f + sinf(u * 3 + i) * 0.3f; g2.mund = 0.6f; schneiderlein(g, 230 + i * 20, 214, 0.6f, -1, g2); }
  herzen(g, 150, 160, u, 6, 40);
  if (u > 4) feuerwerk(g, 70, 70, fmodf(u * 0.5f, 1), FARBE(0xFF6B8A));
  g.kameraAus();
  vers(g, {"Da wurde die Hochzeit erst", "richtig gefeiert - und sie", "war von Herzen froh."}, aufzu(u, 0.4f, 14));
  endeBild(g, u, 18, 150, 150);
}

static void bild(Gfx &g, float t) {
  if (t < P2) szeneFreier(g, t - P1);
  else if (t < P3) szeneSpielmann(g, t - P2);
  else if (t < P4) szeneWeg(g, t - P3);
  else if (t < P5) szeneMarkt(g, t - P4);
  else if (t < P6) szeneSchloss(g, t - P5);
  else szeneHochzeit(g, t - P6);
}

// =====================================================================
// Musik
// =====================================================================
// das Lied des Spielmanns (a-Moll -> C-Dur)
static const Ton3 MEL_DROSSEL[] = {
  {69, 1, 45}, {72, .5, 0}, {71, .5, 0}, {69, 1, 0}, {64, 1, 40}, {65, .5, 41}, {67, .5, 0}, {69, .5, 0}, {71, .5, 0}, {72, 2, 48},
  {74, 1, 43}, {72, .5, 0}, {71, .5, 0}, {72, 1, 45}, {69, 1, 0}, {71, .5, 40}, {69, .5, 0}, {68, .5, 0}, {71, .5, 0}, {69, 2, 45}};
// Hochzeitswalzer (C-Dur, 3/4)
static const Ton3 MEL_WALZER_D[] = {
  {72, 1, 48}, {76, 1, 0}, {79, 1, 0}, {77, 2, 41}, {74, 1, 0}, {71, 1, 43}, {74, 1, 0}, {77, 1, 0}, {76, 2, 48}, {72, 1, 0},
  {72, 1, 45}, {76, 1, 0}, {81, 1, 0}, {79, 2, 41}, {77, 1, 0}, {76, 1, 43}, {74, 1, 0}, {71, 1, 0}, {72, 3, 48}};
static void scheppern(Synth &s, float x) {
  for (int i = 0; i < 8; i++) { s.note(x + i * 0.06f, 84 + (i * 5) % 12, 0.4f); s.rauschen(x + i * 0.06f, 0.1f, BANDPASS, 2400 + i * 300, 0.12f); }
}
static void ton(Synth &s, float t0, float t1) {
  float a = P1;
  spieleMelodie(s, M(MEL_KEGEL), a + 0.4f, a + 31.5f, 0.34f, t0, t1, 0.45f);
  for (int i = 0; i < 3; i++) if (bei(a + 8 + i * 6, t0, t1)) for (int k = 0; k < 4; k++) s.note(a + 8 + i * 6 + k * 0.12f, 76 - k * 2, 0.35f);  // hihi
  a = P2;
  if (bei(a + 0.4f, t0, t1)) { s.note(a + 0.4f, 52, 0.7f); s.note(a + 0.4f, 59, 0.6f); }
  spieleMelodie(s, M(MEL_DROSSEL), a + 12, a + 25.5f, 0.32f, t0, t1, 0.8f);
  a = P3;
  spieleMelodie(s, M(MEL_DROSSEL), a + 0.4f, a + 25, 0.4f, t0, t1, 0.5f);
  spieleMelodie(s, M(MEL_WIEGE), a + 25.4f, a + 29.8f, 0.42f, t0, t1, 0.6f);
  a = P4;
  spieleMelodie(s, M(MEL_DROSSEL), a + 0.4f, a + 18, 0.42f, t0, t1, 0.45f);
  for (int i = 0; i < 18; i++) { float x = a + 18 + i * 0.26f; if (bei(x, t0, t1)) s.rauschen(x, 0.05f, BANDPASS, 1600 + (i & 1) * 400, 0.12f); }
  if (bei(a + 21, t0, t1)) scheppern(s, a + 21);
  spieleMelodie(s, M(MEL_WIEGE), a + 23.4f, a + 29.5f, 0.42f, t0, t1, 0.5f);
  a = P5;
  spieleMelodie(s, M(MEL_WALZER_D), a + 6, a + 17, 0.3f, t0, t1, 0.7f);
  if (bei(a + 18.2f, t0, t1)) scheppern(s, a + 18.2f);
  spieleMelodie(s, M(MEL_DROSSEL), a + 24.4f, a + 35.5f, 0.34f, t0, t1, 0.75f);
  a = P6;
  spieleMelodie(s, M(MEL_WALZER_D), a + 0.4f, a + 18, 0.28f, t0, t1, 0.8f);
  endeKlang(s, a + 18.2f, t0, t1);
}

// =====================================================================
// Startbild, Vorschau, Erkennungszeichen
// =====================================================================
static void titelSzene(Gfx &g, float t) {
  saal(g, t);
  FrauPose p = fposis(PRINZ, t);
  p.armV = 1.8f; p.mund = 0.4f;
  frau(g, 110, 222, 0.95f, 1, p);
  Pose d = posis(DROSSEL, t);
  d.mund = 0.2f;
  drossel(g, 190, 222, 1.1f, -1, d);
}
static void titelbild(Gfx &g, float t) { titelSzene(g, t); titelRahmen(g, t, "König Drosselbart", 200, SCHRIFT_M); }
static void vorschau(Gfx &g, float t) { titelSzene(g, t); }
static void symbol(Gfx &g, float x, float y, float t) {
  Pose d = posis(DROSSEL, t);
  d.mund = 0.2f;
  drossel(g, x, y, 1, 1, d);
}

const Maerchen DROSSELBART = {"König Drosselbart", PDAUER, FARBE(0x2E5FAE), bild, ton, titelbild, vorschau, symbol,
                              LIED(MEL_DROSSEL, 0.34f), nullptr};

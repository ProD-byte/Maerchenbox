// Rumpelstilzchen - ein Spieluhr-Film fuer die Maerchenbox, nach dem Maerchen der Brueder Grimm.
// Stroh zu Gold - schnurr, schnurr, schnurr -, das Halsband, der Ring, das Versprechen, drei Tage Raten,
// das Feuer im Wald: "Ach, wie gut, dass niemand weiss, dass ich Rumpelstilzchen heiss!"
#include "erzaehlen.h"

// ---------- Ablauf (Sekunden) ----------
static const float P1 = 0, P2 = 28, P3 = 74, P4 = 102, P5 = 132, P6 = 164, P7 = 194, PDAUER = 236;

static FrauLook muellerinLook() {
  FrauLook l; l.kleid = FARBE(0x7A9ACB); l.schuerze = FARBE(0xF6F0E4); l.haar = FARBE(0xC98A3A); l.frisur = 1;
  return l;
}
static FrauLook koeniginLook() { FrauLook l = muellerinLook(); l.kleid = FARBE(0x9A3A6E); l.schuerze = 0; l.krone = true; l.glanz = true; l.rockBreite = 1.3f; l.frisur = 2; return l; }
static const FrauLook MUELLERIN = muellerinLook(), KOENIGIN = koeniginLook();
static Aussehen muellerLook() {
  Aussehen a; a.weste = FARBE(0xE8E4DC); a.hemd = FARBE(0xF6F2EA); a.hose = FARBE(0xD8D2C6); a.haar = FARBE(0xE0DCD4);
  a.hut = FARBE(0xF2EEE6); a.hutD = FARBE(0xD8D2C6); a.feder = FARBE(0xD8D2C6);
  return a;
}
static Aussehen boteLook() {
  Aussehen a; a.weste = FARBE(0x3E6EA8); a.hemd = FARBE(0xF2E8D0); a.hose = FARBE(0x5A4636); a.haar = FARBE(0x6E4A2A);
  a.hut = FARBE(0x3E6EA8); a.hutD = FARBE(0x2E5A8E); a.feder = WEISS;
  return a;
}
static Aussehen maennchenLook() {
  Aussehen a; a.weste = FARBE(0x8E3A2A); a.hemd = FARBE(0xC9A46A); a.hose = FARBE(0x4A5A2A); a.haar = FARBE(0x8E8E8E);
  a.hut = FARBE(0x6E2A6E); a.hutD = FARBE(0x4E1A4E); a.feder = FARBE(0xE0A23C); a.haut = FARBE(0xF2C9A8);
  return a;
}
static const Aussehen MUELLER = muellerLook(), BOTE = boteLook(), MAENNCHEN = maennchenLook();

// ---------- Figuren und Dinge ----------
// das Maennchen: klein, mit spitzem Hut, langer Nase und grauem Bart
static void maennchen(Gfx &g, float x, float y, float s, float dir, float t, Pose p) {
  p.aussehen = &MAENNCHEN;
  schneiderlein(g, x, y, s, dir, p);
  auto PX = [&](float d) { return x + d * s * dir; };
  auto PY = [&](float d) { return y + d * s; };
  g.ellipse(PX(16), PY(-58), 5 * s, 3 * s, MAENNCHEN.haut, 255, 0.3f * dir);  // lange Nase
  float b[] = {PX(-2), PY(-50), PX(14), PY(-50), PX(6), PY(-30)};
  g.polygon(b, 3, FARBE(0xBDBDBD));  // spitzer Bart
}

static void spinnrad(Gfx &g, float x, float y, float dreh) {
  g.kontur(1.2f, OUT);
  g.kreis(x, y - 30, 20, HOLZ);
  g.konturAus();
  g.kreis(x, y - 30, 16, FARBE(0xE8D2B0));
  for (int i = 0; i < 6; i++) { float a = dreh + i * 1.047f; g.linie(x, y - 30, x + cosf(a) * 16, y - 30 + sinf(a) * 16, 1.6f, HOLZ_D); }
  g.linie(x - 10, y, x, y - 30, 2, HOLZ_D);
  g.linie(x + 10, y, x, y - 30, 2, HOLZ_D);
  g.ellipse(x + 26, y - 30, 4, 7, GOLD);  // die Spule mit Gold
}

static void stroh(Gfx &g, float x, float y, float menge, float gold) {
  if (menge <= 0) return;
  Farbe c = mischeFarbe(FARBE(0xD8B860), GOLD, gold);
  g.kontur(1.2f, OUT);
  g.ellipse(x, y - 14 * menge, 50 * menge + 10, 16 * menge + 4, c);
  g.konturAus();
  for (int i = 0; i < 12; i++) {
    float a = 3.3f + i * 0.25f;
    g.linie(x + cosf(a) * 40 * menge, y - 14 * menge + sinf(a) * 14 * menge, x + cosf(a) * 52 * menge, y - 14 * menge + sinf(a) * 20 * menge, 1.2f, mischeFarbe(FARBE(0xB8984E), GOLD, gold));
  }
  if (gold > 0.5f) for (int i = 0; i < 4; i++) g.stern(x - 30 + i * 20, y - 20 * menge, 3, 1.2f, 4, i, WEISS, 200);
}

static void kammer(Gfx &g, float t, float nacht) {
  g.verlauf(0, 200, mischeFarbe(FARBE(0xD9CCB8), FARBE(0x4E4466), nacht), mischeFarbe(FARBE(0xC0B19A), FARBE(0x3A3050), nacht));
  for (int r = 0; r < 6; r++) for (int c = 0; c < 6; c++) g.rundRechteck(c * 52 - (r & 1) * 26 + 2, r * 32 + 2, 48, 28, 3, mischeFarbe(FARBE(0xCFC0A8), FARBE(0x443A5A), nacht));
  g.rechteck(0, 200, 284, 40, FARBE(0x6E5A46));
  g.kontur(1.6f, OUT);
  g.rundRechteck(20, 40, 44, 66, 20, FARBE(0x8E8676));
  g.konturAus();
  g.rundRechteck(26, 46, 32, 54, 16, mischeFarbe(FARBE(0x9FD8FF), FARBE(0x22285A), nacht));
  if (nacht > 0.5f) mond(g, 42, 64, 6);
  g.rundRechteck(236, 110, 40, 90, 18, HOLZ);  // die Tuer
}

static void schmuck(Gfx &g, float x, float y, bool ring) {
  if (ring) { g.bogen(x, y, 4, 0, 6.29f, 1.6f, GOLD); g.stern(x, y - 4, 2.4f, 1, 4, 0, FARBE(0xE8473D)); }
  else for (int i = 0; i < 7; i++) g.kreis(x - 9 + i * 3, y + fabsf(i - 3) * 1.2f, 1.6f, WEISS);  // Perlenhalsband
}

static void waldNacht(Gfx &g, float t) {
  himmel(g, 1, t, 200);
  for (int i = 0; i < 20; i++) g.stern(zufall(i + 700) * 284, zufall(i + 720) * 100, 2, 0.8f, 4, 0, WEISS, 180);
  mond(g, 240, 34, 11);
  for (int i = 0; i < 6; i++) tanne(g, i * 54 + 6, 200, 0.85f + (i % 2) * 0.15f, FARBE(0x1F3B45));
  boden(g, 200, FARBE(0x2F4A40));
  // das kleine Haus
  g.kontur(1.4f, OUT);
  g.rechteck(200, 158, 50, 42, FARBE(0x6E5A46));
  float d[] = {194, 160, 225, 136, 256, 160};
  g.polygon(d, 3, FARBE(0x4E3A3A));
  g.konturAus();
  g.rundRechteck(216, 176, 14, 24, 6, FARBE(0xFFD27A));
}

static void feuer(Gfx &g, float x, float y, float t) {
  for (int i = 0; i < 3; i++) g.linie(x - 12 + i * 8, y, x - 6 + i * 6, y - 6, 3, HOLZ_D);
  for (int i = 0; i < 3; i++) {
    float h = 18 + 6 * sinf(t * 9 + i * 1.7f);
    float f[] = {x - 13 + i * 9, y - 2, x - 3 + i * 9, y - 2, x - 8 + i * 9 + sinf(t * 7 + i) * 2, y - 2 - h};
    g.polygon(f, 3, i == 1 ? FARBE(0xFFD54A) : FARBE(0xFF8A2A));
  }
  g.kreis(x, y - 12, 40, FARBE(0xFFB050), 50);
}

// =====================================================================
// Szenen
// =====================================================================
// 1) Der Mueller prahlt vor dem Koenig; die Kammer voll Stroh
static void szeneProbe(Gfx &g, float u) {
  if (u < 14) {
    himmel(g, 0, u, 196);
    sonne(g, 248, 30, 11, u);
    schloss(g, 200, 196, u, 0);
    boden(g, 196);
    Pose m = posis(MUELLER, u);
    m.mund = u > 2 && u < 8 ? 0.5f + 0.3f * fabsf(sinf(u * 8)) : 0.2f;
    m.armV = u > 2 && u < 8 ? 2.4f : 1.2f;
    schneiderlein(g, 80, 214, 0.95f, 1, m);
    FrauPose t = fposis(MUELLERIN, u);
    t.armV = 0.8f;
    frau(g, 40, 214, 0.85f, 1, t);
    koenig(g, 160, 214, 0.95f, -1, u > 8 ? 0.6f : 0);
    g.kameraAus();
    blase(g, 110, 50, {"Ich habe eine Tochter,", "die kann Stroh zu Gold spinnen!"}, aufzu(u, 2, 8), 90, 140);
    blase(g, 180, 50, {"Das ist eine Kunst,", "die mir wohl gefällt!"}, aufzu(u, 8.2f, 13.6f), 160, 140);
    return;
  }
  float v = u - 14;
  kammer(g, u, 0.4f);
  stroh(g, 160, 214, 1, 0);
  spinnrad(g, 80, 214, 0);
  koenig(g, 230, 214, 0.95f, -1, 0.5f);
  FrauPose t = fposis(MUELLERIN, u);
  t.weint = v > 8;
  t.armV = 1.2f;
  frau(g, 110, 214, 0.85f, 1, t);
  g.kameraAus();
  if (v < 1) g.rechteck(0, 0, 284, 240, 0, (uint8_t)(255 * (1 - v)));
  blase(g, 190, 50, {"Jetzt mache dich an die Arbeit!", "Bis morgen früh soll dieses", "Stroh zu Gold versponnen sein."}, aufzu(v, 1, 7.6f), 220, 140);
  vers(g, {"Da saß nun die arme", "Müllerstochter und wusste", "keinen Rat."}, aufzu(v, 8, 13.6f));
  blende(g, u, 28, 140, 160);
}

// 2) Die erste Nacht: das Maennchen - das Halsband - schnurr, schnurr, schnurr
static void szeneNacht1(Gfx &g, float u) {
  kammer(g, u, 0.9f);
  float gold = seg(u, 22, 40);
  stroh(g, 170, 214, 1 - gold * 0.85f, 0);
  for (int i = 0; i < (int)(gold * 8); i++) {  // Spulen voll Gold
    g.kontur(1, FARBE(0x9A6A1E));
    g.ellipse(130 + (i % 4) * 14, 210 - (i / 4) * 12, 5, 7, GOLD);
    g.konturAus();
  }
  bool spinnt = u > 22 && u < 40;
  spinnrad(g, 80, 214, spinnt ? u * 10 : 0);
  FrauPose t = fposis(MUELLERIN, u);
  t.weint = u < 14;
  t.mund = (u > 8 && u < 11) || (u > 15 && u < 18) ? 0.5f : 0;
  t.zu = u > 40;
  float thx, thy;
  frau(g, 30, 214, 0.8f, 1, t, &thx, &thy);
  if (u < 18) schmuck(g, thx - 4, thy - 20, false);
  // das Maennchen kommt zur Tuer herein
  Pose p = posis(MAENNCHEN, u);
  float mx = u < 22 ? lerp(260, 150, sanft(seg(u, 2, 6))) : 104;
  p.lauf = u > 2 && u < 6 ? fmodf(u * 1.8f, 1) : -1;
  p.mund = (u > 6 && u < 8) || (u > 11 && u < 15) ? 0.6f : (spinnt ? 0.3f : 0);
  p.armV = spinnt ? 1.6f + sinf(u * 10) * 0.3f : 1.2f;
  maennchen(g, mx, 214, 0.62f, spinnt ? -1 : -1, u, p);
  if (u > 18 && u < 21) schmuck(g, lerp(thx, mx, seg(u, 18, 20)), lerp(thy - 20, 180, seg(u, 18, 20)), false);
  if (spinnt) glitzern(g, 106, 180, 18, u, 4);
  g.kameraAus();
  blase(g, 180, 60, {"Guten Abend, Jungfer Müllerin,", "warum weint Sie so sehr?"}, aufzu(u, 6, 8.2f), mx, 150);
  blase(g, 80, 60, {"Ach, ich soll Stroh zu Gold", "spinnen und verstehe das nicht."}, aufzu(u, 8.4f, 11), 40, 140);
  blase(g, 180, 60, {"Was gibst du mir,", "wenn ich dir's spinne?"}, aufzu(u, 11.2f, 15), mx, 150);
  blase(g, 80, 60, {"Mein Halsband."}, aufzu(u, 15.2f, 18), 40, 140);
  vers(g, {"Schnurr, schnurr, schnurr, dreimal", "gezogen, war die Spule voll."}, aufzu(u, 23, 32));
  vers(g, {"Und am Morgen war alles Stroh", "versponnen und alle Spulen", "voll Gold."}, aufzu(u, 32.4f, 45.6f));
  blende(g, u, 46, 120, 160);
}

// 3) Die zweite und die dritte Nacht: der Ring - und das Versprechen
static void szeneNacht23(Gfx &g, float u) {
  kammer(g, u, 0.9f);
  stroh(g, 170, 214, u < 13 ? 1.2f - seg(u, 4, 12) : 1.4f - seg(u, 20, 27) * 1.2f, 0);
  spinnrad(g, 80, 214, (u > 4 && u < 12) || (u > 20 && u < 27) ? u * 10 : 0);
  FrauPose t = fposis(MUELLERIN, u);
  t.mund = (u > 1 && u < 3.6f) || (u > 15 && u < 20) ? 0.5f : 0;
  t.weint = u > 13 && u < 16;
  float thx, thy;
  frau(g, 30, 214, 0.8f, 1, t, &thx, &thy);
  if (u < 3.6f) schmuck(g, thx + 2, thy, true);
  Pose p = posis(MAENNCHEN, u);
  bool spinnt = (u > 4 && u < 12) || (u > 20 && u < 27);
  p.armV = spinnt ? 1.6f + sinf(u * 10) * 0.3f : 1.2f;
  p.mund = u > 13 && u < 15 ? 0.6f : (spinnt ? 0.3f : 0);
  maennchen(g, 104, 214, 0.62f, -1, u, p);
  if (spinnt) glitzern(g, 106, 180, 18, u, 4);
  g.kameraAus();
  blase(g, 80, 60, {"Den Ring von meinem Finger."}, aufzu(u, 1, 3.6f), 40, 140);
  vers(g, {"In der dritten Nacht hatte sie", "nichts mehr, was sie geben konnte."}, aufzu(u, 11.6f, 13.2f));
  blase(g, 180, 60, {"So versprich mir, wenn du", "Königin wirst, dein erstes Kind!"}, aufzu(u, 13.2f, 16.6f), 110, 150);
  vers(g, {"Wer weiß, wie das noch geht,", "dachte die Müllerstochter", "und versprach es."}, aufzu(u, 16.8f, 21));
  vers(g, {"Der König hielt Hochzeit mit ihr,", "und sie ward eine Königin."}, aufzu(u, 22, 27.8f));
  blende(g, u, 28, 120, 160);
}

// 4) Ein Jahr spaeter: das Maennchen will das Kind - drei Tage Zeit
static void szeneKind(Gfx &g, float u) {
  g.verlauf(0, 190, FARBE(0xF6E2B4), FARBE(0xE8C483));
  for (int i = 0; i < 4; i++) { float x = 20 + i * 82; g.rechteck(x - 10, 30, 20, 160, FARBE(0xFFF3D6)); }
  g.rechteck(0, 190, 284, 50, FARBE(0xC9A46A));
  // die Wiege
  g.kontur(1.2f, OUT);
  float w[] = {50, 196, 96, 196, 90, 212, 56, 212};
  g.polygon(w, 4, GOLD);
  g.konturAus();
  g.kreis(62, 191, 6, HAUT);
  g.ellipse(76, 193, 12, 5, FARBE(0xF6B8D2));
  FrauPose k = fposis(KOENIGIN, u);
  k.weint = u > 16 && u < 22;
  k.armV = u > 6 && u < 11 ? 2.0f : 1.4f;
  k.mund = (u > 6 && u < 11) || (u > 16 && u < 18) ? 0.5f : 0;
  frau(g, 120, 214, 0.95f, 1, k);
  Pose p = posis(MAENNCHEN, u);
  p.mund = (u > 2 && u < 6) || (u > 11 && u < 16) || (u > 22 && u < 29) ? 0.6f : 0;
  p.armV = 1.6f;
  maennchen(g, lerp(320, 200, sanft(seg(u, 0, 2))), 214, 0.62f, -1, u, p);
  if (u > 6 && u < 11) for (int i = 0; i < 6; i++) g.stern(150 + i * 8, 170 - (i & 1) * 8, 3, 1.2f, 4, u + i, GOLD);  // Schaetze
  g.kameraAus();
  vers(g, {"Über ein Jahr bekam sie ein", "schönes Kind, da trat", "das Männchen ein."}, aufzu(u, 0.2f, 2));
  blase(g, 200, 60, {"Nun gib mir, was du", "versprochen hast!"}, aufzu(u, 2, 6), 200, 150);
  blase(g, 110, 50, {"Ich biete dir alle", "Reichtümer des Königreichs!"}, aufzu(u, 6.2f, 11), 124, 130);
  blase(g, 200, 50, {"Nein, etwas Lebendes ist mir", "lieber als alle Schätze der Welt."}, aufzu(u, 11.2f, 16), 200, 150);
  blase(g, 200, 50, {"Drei Tage will ich dir Zeit lassen.", "Wenn du bis dahin meinen Namen", "weißt, so sollst du dein Kind behalten."}, aufzu(u, 22.2f, 29.6f), 200, 150);
  blende(g, u, 30, 120, 160);
}

// 5) Das Raten: Kaspar, Melchior, Balzer ... Rippenbiest, Hammelswade, Schnuerbein
static void szeneRaten(Gfx &g, float u) {
  g.verlauf(0, 190, FARBE(0xF6E2B4), FARBE(0xE8C483));
  for (int i = 0; i < 4; i++) { float x = 20 + i * 82; g.rechteck(x - 10, 30, 20, 160, FARBE(0xFFF3D6)); }
  g.rechteck(0, 190, 284, 50, FARBE(0xC9A46A));
  FrauPose k = fposis(KOENIGIN, u);
  k.mund = fmodf(u, 3) < 1.6f ? 0.5f : 0;
  k.armV = 1.6f;
  frau(g, 100, 214, 0.95f, 1, k);
  Pose p = posis(MAENNCHEN, u);
  p.mund = fmodf(u, 3) > 1.8f ? 0.7f : 0;
  p.armV = fmodf(u, 3) > 1.8f ? 2.4f + sinf(u * 12) * 0.3f : 1.2f;  // haha!
  maennchen(g, 190, 214 - (fmodf(u, 3) > 1.8f ? fabsf(sinf(u * 10)) * 6 : 0), 0.62f, -1, u, p);
  static const char *NAMEN[6] = {"Heißt du Kaspar?", "Heißt du Melchior?", "Heißt du Balzer?",
                                 "Heißt du Rippenbiest?", "Heißt du Hammelswade?", "Heißt du Schnürbein?"};
  g.kameraAus();
  for (int i = 0; i < 6; i++) {
    float a = 2 + i * 4.6f;
    blase(g, 110, 60, {NAMEN[i]}, aufzu(u, a, a + 1.8f), 104, 140);
    blase(g, 200, 70, {"So heiß ich nicht!"}, aufzu(u, a + 2, a + 4), 190, 160);
  }
  if (u < 2) vers(g, {"Die Königin besann sich", "auf alle Namen, die sie wusste."}, aufzu(u, 0.2f, 2));
  blende(g, u, 32, 140, 160);
}

// 6) Der Bote im Wald: "Ach, wie gut, dass niemand weiss, dass ich Rumpelstilzchen heiss!"
static void szeneFeuer(Gfx &g, float u) {
  waldNacht(g, u);
  feuer(g, 140, 214, u);
  Pose p = posis(MAENNCHEN, u);
  float phase = u * 2.6f;
  p.armV = 2.4f + sinf(phase) * 0.4f;
  p.armH = 2.4f - sinf(phase) * 0.4f;
  p.mund = 0.5f + 0.3f * fabsf(sinf(u * 6));
  float mx = 140 + cosf(u * 0.9f) * 44;
  maennchen(g, mx, 214 - fabsf(sinf(phase)) * 14, 0.62f, sinf(u * 0.9f) > 0 ? -1 : 1, u, p);  // huepft auf einem Bein
  // der Bote hinter dem Baum
  Pose b = posis(BOTE, u);
  b.mund = 0.6f;
  b.armV = 2.2f;
  g.ausschnitt(0, 0, 54, 240);
  schneiderlein(g, 40, 214, 0.9f, 1, b);
  g.ausschnittAus();
  tanne(g, 20, 214, 0.9f, FARBE(0x1F3B45));
  g.kameraAus();
  vers(g, {"Ein Bote sah im Wald ein Feuer,", "um das Feuer sprang ein Männchen."}, aufzu(u, 0.4f, 4));
  blase(g, 150, 46, {"Heute back ich, morgen brau ich,", "übermorgen hol ich der", "Königin ihr Kind!"}, aufzu(u, 4.2f, 12), mx, 150);
  blase(g, 150, 46, {"Ach, wie gut, dass niemand weiß,", "dass ich Rumpelstilzchen heiß!"}, aufzu(u, 12.2f, 22), mx, 150);
  vers(g, {"Da lief der Bote, so schnell er", "konnte, zur Königin zurück."}, aufzu(u, 23, 29.6f));
  blende(g, u, 30, 140, 170);
}

// 7) Der dritte Tag: Kunz? Heinz? "Heisst du etwa Rumpelstilzchen?"
static void szeneName(Gfx &g, float u) {
  g.verlauf(0, 190, FARBE(0xF6E2B4), FARBE(0xE8C483));
  for (int i = 0; i < 4; i++) { float x = 20 + i * 82; g.rechteck(x - 10, 30, 20, 160, FARBE(0xFFF3D6)); }
  g.rechteck(0, 190, 284, 50, FARBE(0xC9A46A));
  FrauPose k = fposis(KOENIGIN, u);
  k.mund = (u > 1 && u < 3) || (u > 5 && u < 7) || (u > 9 && u < 12) ? 0.6f : 0.2f;
  k.armV = u > 9 ? 1.8f : 1.4f;
  frau(g, 100, 214, 0.95f, 1, k);
  float wut = seg(u, 12, 13);
  float weg = seg(u, 18, 19.5f);
  if (weg < 1) {
    Pose p = posis(MAENNCHEN, u);
    p.boese = wut;
    p.mund = u < 12 ? (fmodf(u, 4) > 3 ? 0.5f : 0) : 0.9f;
    p.armV = wut > 0 ? 2.6f + sinf(u * 16) * 0.5f : 1.2f;
    p.armH = wut > 0 ? 2.6f - sinf(u * 16) * 0.5f : 0.3f;
    float stampf = u > 14 ? fabsf(sinf(u * 9)) * 6 : 0;
    g.ausschnitt(0, 0, 284, 214);
    maennchen(g, 190, 214 - stampf + seg(u, 16, 19) * 60, 0.62f, -1, u, p);  // er stampft sich in den Boden
    g.ausschnittAus();
  }
  if (u > 16 && u < 20) wolkePuff(g, 190, 206, seg(u, 16, 20), FARBE(0xC9B8A0), 1.2f);
  if (u > 14 && u < 18.5f) g.wackeln(sinf(u * 60) * 2, cosf(u * 50) * 2);
  if (u > 20) herzen(g, 110, 150, u, 5, 30);
  g.kameraAus();
  blase(g, 110, 60, {"Heißt du Kunz?"}, aufzu(u, 1, 3), 104, 140);
  blase(g, 200, 70, {"Nein."}, aufzu(u, 3.2f, 4.8f), 190, 160);
  blase(g, 110, 60, {"Heißt du Heinz?"}, aufzu(u, 5, 7), 104, 140);
  blase(g, 200, 70, {"Nein."}, aufzu(u, 7.2f, 8.8f), 190, 160);
  blase(g, 110, 60, {"Heißt du etwa", "Rumpelstilzchen?"}, aufzu(u, 9, 12), 104, 140);
  blase(g, 190, 60, {"Das hat dir der Teufel gesagt!", "Das hat dir der Teufel gesagt!"}, aufzu(u, 12.2f, 16), 190, 160);
  vers(g, {"Er stieß vor Zorn mit dem rechten", "Fuß so tief in die Erde ..."}, aufzu(u, 16.2f, 21));
  vers(g, {"... und ward nicht mehr gesehen.", "Die Königin aber behielt", "ihr Kind."}, aufzu(u, 21.2f, 32));
  endeBild(g, u, 36);
}

static void bild(Gfx &g, float t) {
  if (t < P2) szeneProbe(g, t - P1);
  else if (t < P3) szeneNacht1(g, t - P2);
  else if (t < P4) szeneNacht23(g, t - P3);
  else if (t < P5) szeneKind(g, t - P4);
  else if (t < P6) szeneRaten(g, t - P5);
  else if (t < P7) szeneFeuer(g, t - P6);
  else szeneName(g, t - P7);
}

// =====================================================================
// Musik
// =====================================================================
// Rumpelstilzchens Tanz (a-Moll, huepfend, ein bisschen frech)
static const Ton3 MEL_RUMPEL[] = {
  {69, .5, 45}, {72, .25, 0}, {76, .25, 0}, {81, .5, 0}, {76, .5, 0}, {77, .5, 50}, {76, .5, 0}, {74, 1, 0},
  {71, .5, 44}, {74, .25, 0}, {77, .25, 0}, {83, .5, 0}, {77, .5, 0}, {76, .5, 45}, {74, .5, 0}, {72, 1, 0},
  {69, .5, 45}, {72, .25, 0}, {76, .25, 0}, {81, .5, 0}, {79, .5, 0}, {77, .5, 41}, {76, .5, 0}, {74, .5, 40}, {71, .5, 0}, {69, 1, 45}};

static void schnurr(Synth &s, float x) { s.rauschen(x, 0.5f, BANDPASS, 700, 0.06f, 1.2f, 0.05f, 1400); s.glocke(x + 0.45f, 91, 0.25f); }

static void ton(Synth &s, float t0, float t1) {
  float a = P1;
  spieleMelodie(s, M(MEL_F), a + 0.5f, a + 13.5f, 0.28f, t0, t1, 0.7f);
  spieleMelodie(s, M(MEL_SPUK), a + 14.6f, a + 27.5f, 0.4f, t0, t1, 0.55f);
  a = P2;
  if (bei(a + 2, t0, t1)) s.ton(a + 2, 300, 0.12f, 0.01f, 0.5f, SAEGE, GERAEUSCH, 200);  // die Tuer knarrt
  spieleMelodie(s, M(MEL_RUMPEL), a + 6, a + 21, 0.3f, t0, t1, 0.6f);
  for (float x = ceilf(t0 / 0.6f) * 0.6f; x < t1; x += 0.6f)
    if (x > a + 22 && x < a + 40) schnurr(s, x);
  spieleMelodie(s, M(MEL_WIEGE), a + 40, a + 45.5f, 0.45f, t0, t1, 0.55f);
  a = P3;
  for (float x = ceilf(t0 / 0.6f) * 0.6f; x < t1; x += 0.6f)
    if ((x > a + 4 && x < a + 12) || (x > a + 20 && x < a + 27)) schnurr(s, x);
  spieleMelodie(s, M(MEL_SPUK), a + 12.4f, a + 20, 0.36f, t0, t1, 0.6f);
  a = P4;
  spieleMelodie(s, M(MEL_WIEGE), a + 0.4f, a + 2, 0.45f, t0, t1, 0.6f);
  spieleMelodie(s, M(MEL_RUMPEL), a + 2, a + 29.5f, 0.32f, t0, t1, 0.6f);
  a = P5;
  for (int i = 0; i < 6; i++) {
    float x = a + 2 + i * 4.6f + 2;
    if (bei(x, t0, t1)) for (int k = 0; k < 4; k++) s.note(x + k * 0.12f, 81 - k * 2 + (k & 1) * 4, 0.6f);  // haha!
  }
  spieleMelodie(s, M(MEL_RUMPEL), a + 0.4f, a + 31.5f, 0.36f, t0, t1, 0.45f);
  a = P6;
  for (float x = ceilf(t0 / 0.3f) * 0.3f; x < t1; x += 0.3f)
    if (x > a && x < a + 30) s.rauschen(x, 0.1f, HOCHPASS, 2500, 0.02f);  // Feuer knistert
  spieleMelodie(s, M(MEL_RUMPEL), a + 4, a + 22, 0.24f, t0, t1, 0.85f);
  spieleMelodie(s, M(MEL_JAGD), a + 23, a + 29.5f, 0.16f, t0, t1, 0.8f);
  a = P7;
  for (float k : {3.2f, 7.2f}) if (bei(a + k, t0, t1)) s.note(a + k, 64, 0.6f);  // nein
  if (bei(a + 12, t0, t1)) for (int k = 0; k < 10; k++) s.note(a + 12 + k * 0.06f, 52 + k * 3, 0.8f);
  for (float x = ceilf(t0 / 0.35f) * 0.35f; x < t1; x += 0.35f)
    if (x > a + 14 && x < a + 18) s.ton(x, 90, 0.3f, 0.002f, 0.2f, SINUS, GERAEUSCH, 50);  // stampf
  if (bei(a + 18.6f, t0, t1)) { s.ton(a + 18.6f, 70, 0.5f, 0.002f, 0.6f, SINUS, GERAEUSCH, 30); s.rauschen(a + 18.6f, 0.8f, TIEFPASS, 900, 0.3f); }
  spieleMelodie(s, M(MEL_F), a + 21, a + 36, 0.26f, t0, t1, 0.85f);
  endeKlang(s, a + 38.2f, t0, t1);
}

// =====================================================================
// Startbild, Vorschau, Erkennungszeichen
// =====================================================================
static void titelSzene(Gfx &g, float t) {
  waldNacht(g, t);
  feuer(g, 130, 222, t);
  Pose p = posis(MAENNCHEN, t);
  float ph = t * 2.6f;
  p.armV = 2.4f + sinf(ph) * 0.4f;
  p.armH = 2.4f - sinf(ph) * 0.4f;
  p.mund = 0.6f;
  maennchen(g, 180, 222 - fabsf(sinf(ph)) * 12, 0.75f, -1, t, p);
}
static void titelbild(Gfx &g, float t) { titelSzene(g, t); titelRahmen(g, t, "Rumpelstilzchen", 200); }
static void vorschau(Gfx &g, float t) { titelSzene(g, t); }
static void symbol(Gfx &g, float x, float y, float t) {
  spinnrad(g, x - 20, y, t * 4);
  for (int i = 0; i < 5; i++) {
    g.kontur(1, FARBE(0x9A6A1E));
    g.ellipse(x + 24 + (i % 3) * 12, y - 6 - (i / 3) * 12, 5, 7, GOLD);
    g.konturAus();
  }
  glitzern(g, x + 30, y - 20, 18, t, 4);
}

const Maerchen RUMPELSTILZCHEN = {"Rumpelstilzchen", PDAUER, FARBE(0x8E3A2A), bild, ton, titelbild, vorschau, symbol,
                                  LIED(MEL_RUMPEL, 0.32f), nullptr};

// Die Prinzessin auf der Erbse - ein Spieluhr-Film fuer die Maerchenbox, nach dem Maerchen von Hans Christian Andersen.
// Der Prinz sucht eine wirkliche Prinzessin, das Gewitter, die Erbse unter zwanzig Matratzen und zwanzig
// Eiderdaunenbetten - "ganz braun und blau" - und die Erbse kommt in die Kunstkammer.
#include "erzaehlen.h"

// ---------- Ablauf (Sekunden) ----------
static const float P1 = 0, P2 = 28, P3 = 56, P4 = 86, P5 = 106, PDAUER = 152;

static Aussehen prinzLook() {
  Aussehen a; a.weste = FARBE(0x8E2F6A); a.hemd = WEISS; a.hose = FARBE(0x2E3A6E); a.haar = FARBE(0x3A2A20); a.muetze = 3;
  a.halstuch = GOLD;
  return a;
}
static const Aussehen PRINZ = prinzLook();
static FrauLook prinzessinLook() {
  FrauLook l; l.kleid = FARBE(0x9AC8F2); l.haar = FARBE(0xC98A3A); l.frisur = 0; l.krone = false;
  return l;
}
static FrauLook nassLook() { FrauLook l = prinzessinLook(); l.kleid = FARBE(0x6E8EB0); l.haar = FARBE(0x8A5A2E); return l; }
static FrauLook nachthemdLook() { FrauLook l = prinzessinLook(); l.kleid = FARBE(0xF2EEFF); l.frisur = 1; return l; }
static FrauLook brautLook() { FrauLook l = prinzessinLook(); l.kleid = WEISS; l.glanz = true; l.krone = true; l.rockBreite = 1.4f; l.frisur = 2; return l; }
static FrauLook koeniginLook() {
  FrauLook l; l.kleid = FARBE(0x6E3A8E); l.haar = FARBE(0xBDBDBD); l.frisur = 2; l.krone = true; l.rockBreite = 1.3f;
  return l;
}
static FrauLook falschLook(int i) {
  static const uint32_t K[3] = {0xF2A0C8, 0xF2C94E, 0x7CCB5E};
  FrauLook l; l.kleid = FARBE(K[i]); l.haar = i == 1 ? FARBE(0x3A2A20) : FARBE(0xF2E08A); l.frisur = 2; l.krone = true; l.glanz = true;
  return l;
}
static const FrauLook PRINZESSIN = prinzessinLook(), NASS = nassLook(), NACHTHEMD = nachthemdLook(), BRAUT = brautLook(),
                      KOENIGIN = koeniginLook();
static const FrauLook FALSCH[3] = {falschLook(0), falschLook(1), falschLook(2)};

// ---------- Dinge ----------
static void erbse(Gfx &g, float x, float y, float r, float t) {
  g.kontur(1, FARBE(0x2E6E2E));
  g.kreis(x, y, r, FARBE(0x6EC84E));
  g.konturAus();
  g.kreis(x - r * 0.35f, y - r * 0.35f, r * 0.3f, WEISS, 170);
}

// das hohe Bett: unten die Erbse, dann 20 Matratzen und 20 Eiderdaunenbetten
static void hohesBett(Gfx &g, float x, float y, float lagen, float t, float wackel = 0) {
  g.kontur(1.6f, OUT);
  g.rundRechteck(x - 50, y - 16, 100, 16, 3, HOLZ);
  g.konturAus();
  g.rechteck(x - 50, y - 70, 6, 70, HOLZ_D);
  g.rechteck(x + 44, y - 70, 6, 70, HOLZ_D);
  int n = (int)(lagen * 40);
  static const uint32_t FARBEN[6] = {0xF2A0C8, 0x9AC8F2, 0xF2E08A, 0xB8E8A0, 0xE8B8F0, 0xF6D8B0};
  for (int i = 0; i < n; i++) {
    float ly = y - 18 - i * 3.6f;
    float w = 88 + (i < 20 ? 0 : 6) + sinf(i * 1.3f) * 2;
    float dx = sinf(i * 0.7f + wackel * 6) * wackel * 2 * (i / 40.0f);
    g.rundRechteck(x - w / 2 + dx, ly - 3.6f, w, 3.8f, 1.6f, FARBE(FARBEN[i % 6]));
    if (i >= 20) for (int k = 0; k < 3; k++) g.kreis(x - 28 + k * 28 + dx, ly - 2, 0.9f, WEISS);  // Daunenbett
  }
}

static void leiter(Gfx &g, float x, float y0, float y1) {
  g.linie(x - 8, y0, x - 8, y1, 2, HOLZ_D);
  g.linie(x + 8, y0, x + 8, y1, 2, HOLZ_D);
  for (float y = y0 - 8; y > y1; y -= 12) g.linie(x - 8, y, x + 8, y, 1.6f, HOLZ_D);
}

// ---------- Kulissen ----------
static void schlossSaal(Gfx &g, float t) {
  g.verlauf(0, 190, FARBE(0xF6E2B4), FARBE(0xE8C483));
  for (int i = 0; i < 4; i++) {
    float x = 20 + i * 82;
    g.rechteck(x - 10, 30, 20, 160, FARBE(0xFFF3D6));
    g.rechteck(x - 14, 26, 28, 8, FARBE(0xE8D3A2));
  }
  g.rechteck(0, 190, 284, 50, FARBE(0xC9A46A));
  for (int r = 0; r < 3; r++) for (int c = 0; c < 12; c++) if ((r + c) & 1) g.rechteck(c * 26 - r * 8, 190 + r * 17, 26, 17, FARBE(0xA8823E));
}

static void schlafgemach(Gfx &g, float t, float nacht) {
  g.verlauf(0, 200, mischeFarbe(FARBE(0xE8D8F0), FARBE(0x3A3458), nacht), mischeFarbe(FARBE(0xD0C0E0), FARBE(0x2A2448), nacht));
  g.rechteck(0, 200, 284, 40, mischeFarbe(FARBE(0x9E7656), FARBE(0x4E3E36), nacht));
  g.kontur(1.6f, OUT);
  g.rundRechteck(30, 40, 50, 74, 22, HOLZ);
  g.konturAus();
  g.rundRechteck(36, 46, 38, 62, 18, mischeFarbe(FARBE(0x9FD8FF), FARBE(0x22285A), nacht));
  if (nacht > 0.5f) mond(g, 55, 66, 7);
}

static void gewitter(Gfx &g, float t, float blitz) {
  g.verlauf(0, 196, mischeFarbe(FARBE(0x2A2C44), WEISS, blitz * 0.6f), mischeFarbe(FARBE(0x3E405A), WEISS, blitz * 0.4f));
  for (int i = 0; i < 4; i++) wolke(g, fmodf(i * 80 + t * 6, 340) - 30, 30 + (i & 1) * 16, 1.1f, 200);
  schloss(g, 160, 196, t, 0.8f);
  boden(g, 196, FARBE(0x2F4A40));
  for (int i = 0; i < 40; i++) {  // Regen
    float x = fmodf(zufall(i + 10) * 300 + t * 60, 300) - 8, y = fmodf(zufall(i + 30) * 240 + t * 300, 250) - 6;
    g.linie(x, y, x - 3, y + 9, 1, FARBE(0x9FC8F2), 200);
  }
  if (blitz > 0.5f) {
    float b[] = {200, 0, 186, 50, 198, 50, 180, 100, 210, 40, 196, 40, 214, 0};
    g.polygon(b, 7, FARBE(0xFFF6A0));
  }
}

// =====================================================================
// Szenen
// =====================================================================
// 1) Der Prinz sucht eine wirkliche Prinzessin
static void szeneSuche(Gfx &g, float u) {
  float scroll = u * 30;
  himmel(g, 0, u, 190);
  sonne(g, 248, 30, 11, u);
  huegel(g, scroll * 0.3f, 0.4f, 190, 28, 100, FARBE(0x8ACB6A));
  for (int i = 0; i < 3; i++) schloss(g, fmodf(i * 150 - scroll * 0.5f + 1500, 450) - 60, 190, u, 0);
  boden(g, 190);
  g.rechteck(0, 206, 284, 16, FARBE(0xD8C49A));
  Pose p = posis(PRINZ, u);
  p.armV = 1.4f;
  schneiderlein(g, 90, 214 - 44, 0.72f, 1, p);
  einhorn(g, 84, 214, 0.95f, fmodf(u * 1.6f, 1), 1, 0, 0, true);
  // unterwegs: Prinzessinnen, aber immer war etwas nicht ganz richtig
  for (int i = 0; i < 3; i++) {
    float k = seg(u, 6 + i * 6, 12 + i * 6);
    if (k <= 0 || k >= 1) continue;
    FrauPose f = fposis(FALSCH[i], u, i);
    f.armV = 1.6f;
    f.mund = 0.4f;
    f.boese = i == 1 ? 0.8f : 0;
    float fx = lerp(320, -40, k);
    frau(g, fx, 214, 0.85f, -1, f);
    fragezeichen(g, 110, 130, 0.9f, (uint8_t)(255 * huepf(k)));
  }
  g.kameraAus();
  vers(g, {"Es war einmal ein Prinz, der wollte", "eine Prinzessin heiraten, aber es", "sollte eine wirkliche sein."}, aufzu(u, 0.4f, 6));
  vers(g, {"Er reiste in der ganzen Welt", "herum, aber überall war", "etwas im Wege."}, aufzu(u, 6.2f, 18));
  vers(g, {"Da kam er wieder nach Hause", "und war ganz betrübt."}, aufzu(u, 18.4f, 27.6f));
  blende(g, u, 28, 140, 160);
}

// 2) Das Gewitter: es klopft am Stadttor - eine triefnasse Prinzessin
static void szeneGewitter(Gfx &g, float u) {
  float blitz = (fmodf(u, 6) < 0.15f || fmodf(u + 0.3f, 6) < 0.1f) ? 1 : 0;
  gewitter(g, u, blitz);
  // Stadttor
  g.kontur(1.6f, OUT);
  g.rundRechteck(60, 130, 60, 70, 26, FARBE(0x6E6680));
  g.konturAus();
  g.rundRechteck(70, 144, 40, 56, 18, mischeFarbe(HOLZ_D, FARBE(0xFFD27A), seg(u, 10, 12)));
  if (u > 10) koenig(g, 90, 214, 0.85f, 1, 0.3f);
  FrauPose p = fposis(NASS, u);
  p.armV = 1.2f;
  p.mund = u > 13 && u < 19 ? 0.5f : 0;
  float px = lerp(330, 150, sanft(seg(u, 2, 8)));
  p.lauf = u > 2 && u < 8 ? fmodf(u * 1.4f, 1) : -1;
  frau(g, px, 214, 0.9f, -1, p);
  for (int i = 0; i < 6; i++) {  // das Wasser laeuft ihr aus Haar und Kleidern
    float k = fmodf(u * 1.4f + i / 6.0f, 1);
    g.kreis(px - 10 + i * 4, 140 + k * 70, 1.6f, FARBE(0x9FC8F2), (uint8_t)(255 * (1 - k)));
  }
  if (u > 8 && u < 10) for (int i = 0; i < 3; i++) g.stern(118, 166 - i * 6, 3, 1.2f, 4, u * 6 + i, WEISS);  // klopf
  g.kameraAus();
  vers(g, {"Eines Abends zog ein furchtbares", "Gewitter auf; es blitzte", "und donnerte."}, aufzu(u, 0.4f, 7.6f));
  vers(g, {"Da klopfte es an das Stadttor,", "und der alte König ging hin."}, aufzu(u, 8, 12.6f));
  blase(g, 190, 70, {"Ich bin eine wirkliche", "Prinzessin!"}, aufzu(u, 13, 19), 160, 140);
  vers(g, {"Das Wasser lief ihr aus den", "Haaren und Kleidern in die", "Schuhe hinein."}, aufzu(u, 19.4f, 27.6f));
  blende(g, u, 28, 140, 160);
}

// 3) Die Koenigin legt eine Erbse ins Bett - zwanzig Matratzen, zwanzig Eiderdaunenbetten
static void szeneBett(Gfx &g, float u) {
  schlafgemach(g, u, 0);
  float lagen = seg(u, 6, 20);
  hohesBett(g, 180, 214, lagen, u);
  if (u < 8) erbse(g, 180, 194, 3 + (u < 5 ? 2 : 0), u);  // erst gross zu sehen, dann klein
  if (u < 6) glitzern(g, 180, 194, 12, u, 4);
  FrauPose k = fposis(KOENIGIN, u);
  k.armV = u < 20 ? 1.6f + fabsf(sinf(u * 4)) * 0.4f : 1.2f;
  k.mund = u > 1 && u < 5 ? 0.4f : 0;
  float hx, hy;
  frau(g, 90, 214, 0.95f, 1, k, &hx, &hy);
  if (u < 4) erbse(g, hx + 6, hy, 3, u);
  // die Prinzessin steigt die Leiter hinauf
  if (u > 20) {
    leiter(g, 130, 214, 214 - 18 - lagen * 144);
    FrauPose p = fposis(NACHTHEMD, u);
    p.armV = 2.8f; p.armH = 2.8f;
    float k2 = seg(u, 21, 28);
    frau(g, 138, 214 - k2 * 130, 0.6f, 1, p);
  }
  g.kameraAus();
  vers(g, {"Das werden wir schon erfahren,", "dachte die alte Königin."}, aufzu(u, 0.4f, 5.6f));
  vers(g, {"Sie legte eine Erbse in die", "Bettstelle, darauf zwanzig Matratzen", "und zwanzig Eiderdaunenbetten."}, aufzu(u, 6, 18));
  vers(g, {"Darauf musste die Prinzessin", "die ganze Nacht liegen."}, aufzu(u, 20.4f, 29.6f));
  blende(g, u, 30, 180, 120);
}

// 4) Die Nacht: sie waelzt sich hin und her und tut kein Auge zu
static void szeneNacht(Gfx &g, float u) {
  schlafgemach(g, u, 0.9f);
  hohesBett(g, 180, 214, 1, u, fabsf(sinf(u * 1.2f)) * 0.5f);
  float oy = 214 - 18 - 40 * 3.6f;
  float dreh = sinf(u * 1.2f);
  g.kreis(160 + dreh * 6, oy - 6, 7, HAUT);
  g.ellipse(156 + dreh * 6, oy - 10, 8, 4, PRINZESSIN.haar);
  g.kreis(162 + dreh * 6, oy - 7, 1, SCHWARZ);  // die Augen bleiben offen
  g.kreis(166 + dreh * 6, oy - 7, 1, SCHWARZ);
  g.rundRechteck(150, oy - 6, 70, 6, 3, FARBE(0xF2EEFF));
  for (int i = 0; i < 3; i++) g.stern(150 + i * 14, oy - 22 - fabsf(sinf(u * 3 + i)) * 4, 3, 1.2f, 4, u * 3 + i, GOLD, 200);
  // ganz unten, ganz klein: die Erbse leuchtet
  erbse(g, 180, 194, 2.4f, u);
  g.kreis(180, 194, 6, FARBE(0x9AE87A), (uint8_t)(80 + 60 * sinf(u * 3)));
  g.kameraAus();
  vers(g, {"Die ganze Nacht konnte sie", "kein Auge zutun."}, aufzu(u, 1, 18));
  blende(g, u, 20, 170, 60);
}

// 5) Am Morgen: "ganz braun und blau"; eine wirkliche Prinzessin; die Erbse in der Kunstkammer
static void szeneMorgen(Gfx &g, float u) {
  if (u < 30) {
    schlossSaal(g, u);
    koenig(g, 40, 214, 0.9f, 1, u > 12 ? 0.5f + 0.5f * sinf(u * 4) : 0);
    FrauPose k = fposis(KOENIGIN, u);
    k.mund = u > 1 && u < 3.6f ? 0.4f : 0;
    k.armV = u > 12 ? 2.2f : 1.2f;
    frau(g, 76, 214, 0.9f, 1, k);
    FrauPose p = fposis(u > 14 ? BRAUT : PRINZESSIN, u);
    p.mund = u > 3.8f && u < 12 ? 0.6f : 0.3f;
    p.armV = u > 3.8f && u < 12 ? 2.2f : 1.4f;
    p.zu = u > 3.8f && u < 6;
    frau(g, 156, 214, 0.9f, 1, p);
    if (u > 3.8f && u < 12)  // braun und blaue Flecken
      for (int i = 0; i < 4; i++) g.kreis(150 + (i % 2) * 10, 160 + (i / 2) * 14, 2.2f, i & 1 ? FARBE(0x6E5AA8) : FARBE(0x9A6A3A), 200);
    if (u > 13 && u < 15) puff(g, 156, 150, seg(u, 13, 15));
    Pose pr = posis(PRINZ, u);
    pr.mund = u > 12 ? 0.6f : 0.2f;
    pr.armV = u > 12 ? 2.4f : 1.2f;
    schneiderlein(g, 200, 214, 0.95f, -1, pr);
    if (u > 14) herzen(g, 180, 150, u, 5, 30);
    g.kameraAus();
    blase(g, 90, 46, {"Wie hast du geschlafen?"}, aufzu(u, 1, 3.6f), 80, 130);
    blase(g, 170, 40, {"Oh, schrecklich schlecht! Ich habe", "auf etwas Hartem gelegen, so dass", "ich ganz braun und blau bin!"}, aufzu(u, 3.8f, 12), 160, 130);
    vers(g, {"Da sahen sie, dass es eine", "wirkliche Prinzessin war."}, aufzu(u, 12.4f, 20));
    vers(g, {"So empfindlich konnte niemand", "sein als eine wirkliche", "Prinzessin."}, aufzu(u, 20.2f, 29.6f));
    if (u > 29) g.rechteck(0, 0, 284, 240, 0, (uint8_t)(255 * seg(u, 29, 30)));
    return;
  }
  // die Erbse kommt in die Kunstkammer
  float v = u - 30;
  schlossSaal(g, u);
  g.kontur(1.6f, OUT);
  g.rundRechteck(112, 150, 60, 50, 4, FARBE(0xC9A46A));
  g.konturAus();
  g.rundRechteck(118, 110, 48, 44, 8, FARBE(0xDFF4FF), 160);  // Glaskasten
  g.rechteck(118, 152, 48, 3, GOLD);
  erbse(g, 142, 140, 5, u);
  glitzern(g, 142, 132, 18, u, 5);
  g.kameraAus();
  if (v < 1) g.rechteck(0, 0, 284, 240, 0, (uint8_t)(255 * (1 - v)));
  vers(g, {"Die Erbse kam in die Kunstkammer,", "wo sie noch zu sehen ist,", "wenn sie niemand genommen hat."}, aufzu(v, 1, 9));
  vers(g, {"Sieh, das ist eine wahre Geschichte!"}, aufzu(v, 9.2f, 13));
  endeBild(g, v, 13.4f);
}

static void bild(Gfx &g, float t) {
  if (t < P2) szeneSuche(g, t - P1);
  else if (t < P3) szeneGewitter(g, t - P2);
  else if (t < P4) szeneBett(g, t - P3);
  else if (t < P5) szeneNacht(g, t - P4);
  else szeneMorgen(g, t - P5);
}

// =====================================================================
// Musik
// =====================================================================
// das Erbsen-Menuett (B-Dur, zierlich)
static const Ton3 MEL_ERBSE[] = {
  {70, .5, 46}, {74, .5, 0}, {77, 1, 0}, {75, .5, 0}, {74, .5, 0}, {72, 1, 41}, {74, .5, 0}, {75, .5, 0}, {77, 1, 0},
  {79, .5, 43}, {77, .5, 0}, {75, 1, 0}, {74, .5, 0}, {72, .5, 0}, {70, 2, 46}};

static void donner(Synth &s, float x) {
  s.ton(x, 60, 0.4f, 0.05f, 1.6f, SINUS, GERAEUSCH, 40);
  s.rauschen(x, 1.8f, TIEFPASS, 400, 0.3f, 0.7f, 0.05f, 150);
}

static void ton(Synth &s, float t0, float t1) {
  float a = P1;
  spieleMelodie(s, M(MEL_ERBSE), a + 0.5f, a + 18, 0.3f, t0, t1, 0.8f);
  for (float x = ceilf(t0 / 0.227f) * 0.227f; x < t1; x += 0.227f)
    if (x > a && x < a + 18) s.note(x, 79, 0.12f);  // Hufe
  spieleMelodie(s, M(MEL_WIEGE), a + 18.4f, a + 27.6f, 0.45f, t0, t1, 0.6f);
  a = P2;
  for (float x = ceilf(t0 / 0.05f) * 0.05f; x < t1; x += 0.05f)
    if (x > a && x < a + 28) s.rauschen(x, 0.06f, HOCHPASS, 3500, 0.015f);  // Regen
  for (int i = 0; i < 5; i++) if (bei(a + 0.2f + i * 6, t0, t1)) donner(s, a + 0.2f + i * 6);
  for (int i = 0; i < 3; i++) if (bei(a + 8.4f + i * 0.3f, t0, t1)) s.ton(a + 8.4f + i * 0.3f, 160, 0.3f, 0.002f, 0.12f, SINUS, GERAEUSCH, 90);  // klopf
  spieleMelodie(s, M(MEL_ERBSE), a + 13, a + 19, 0.34f, t0, t1, 0.6f);
  a = P3;
  if (bei(a + 0.6f, t0, t1)) s.glocke(a + 0.6f, 96, 0.5f);  // die kleine Erbse
  for (int i = 0; i < 40; i++) {
    float x = a + 6 + i * 0.35f;
    if (bei(x, t0, t1)) s.note(x, 60 + (i % 20), 0.25f);  // jede Lage einen Ton hoeher
  }
  spieleMelodie(s, M(MEL_ERBSE), a + 20.6f, a + 29.6f, 0.3f, t0, t1, 0.7f);
  a = P4;
  spieleMelodie(s, M(MEL_WIEGE), a + 0.4f, a + 19.6f, 0.55f, t0, t1, 0.5f);
  for (int i = 0; i < 6; i++) if (bei(a + 2 + i * 3, t0, t1)) s.rauschen(a + 2 + i * 3, 0.5f, BANDPASS, 800, 0.05f);  // waelzt sich
  a = P5;
  spieleMelodie(s, M(MEL_ERBSE), a + 0.4f, a + 3.6f, 0.3f, t0, t1, 0.6f);
  if (bei(a + 12.4f, t0, t1)) for (int k = 0; k < 14; k++) s.glocke(a + 12.4f + k * 0.12f, 72 + (k % 7) * 2, 0.5f);
  spieleMelodie(s, M(MEL_F), a + 14.4f, a + 29.6f, 0.26f, t0, t1, 0.85f);
  spieleMelodie(s, M(MEL_ERBSE), a + 31, a + 43, 0.32f, t0, t1, 0.75f);
  endeKlang(s, a + 45.6f, t0, t1);
}

// =====================================================================
// Startbild, Vorschau, Erkennungszeichen
// =====================================================================
static void titelSzene(Gfx &g, float t) {
  schlafgemach(g, t, 0.3f);
  hohesBett(g, 170, 222, 0.85f, t, 0);
  float oy = 222 - 18 - 34 * 3.6f;
  g.kreis(150, oy - 6, 7, HAUT);
  g.ellipse(146, oy - 10, 8, 4, PRINZESSIN.haar);
  g.rundRechteck(140, oy - 6, 70, 6, 3, FARBE(0xF2EEFF));
  erbse(g, 170, 202, 3, t);
  g.kreis(170, 202, 7, FARBE(0x9AE87A), (uint8_t)(80 + 60 * sinf(t * 3)));
}
static void titelbild(Gfx &g, float t) { titelSzene(g, t); titelRahmen(g, t, "Die Prinzessin auf der Erbse", 224, SCHRIFT_B); }
static void vorschau(Gfx &g, float t) { titelSzene(g, t); }
static void symbol(Gfx &g, float x, float y, float t) {
  // ein Turm aus bunten Matratzen und ganz unten die Erbse
  static const uint32_t FARBEN[6] = {0xF2A0C8, 0x9AC8F2, 0xF2E08A, 0xB8E8A0, 0xE8B8F0, 0xF6D8B0};
  for (int i = 0; i < 12; i++) g.rundRechteck(x - 30 + sinf(i * 1.3f + t) * 1.5f, y - 12 - i * 6, 60, 6, 2.5f, FARBE(FARBEN[i % 6]));
  g.kontur(1.2f, OUT);
  g.rundRechteck(x - 36, y - 10, 72, 10, 3, HOLZ);
  g.konturAus();
  float b = 1 + sinf(t * 3) * 0.1f;
  erbse(g, x + 44, y - 8, 7 * b, t);
  g.kreis(x + 44, y - 8, 12, FARBE(0x9AE87A), 80);
  g.stern(x - 8, y - 90, 7, 2.6f, 5, -1.5708f, GOLD);
}

const Maerchen ERBSE = {"Die Prinzessin auf der Erbse", PDAUER, FARBE(0x9AC8F2), bild, ton, titelbild, vorschau, symbol,
                        LIED(MEL_ERBSE, 0.32f), "H. C. Andersen"};

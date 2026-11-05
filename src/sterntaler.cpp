// Die Sterntaler - ein Spieluhr-Film fuer die Maerchenbox, nach dem Maerchen der Brueder Grimm.
// Das arme Maedchen verschenkt alles: das Brot, das Muetzchen, das Leibchen, das Roecklein, das Hemdlein -
// da fallen die Sterne vom Himmel und sind lauter blanke Taler.
#include "erzaehlen.h"

// ---------- Ablauf (Sekunden) ----------
static const float P1 = 0, P2 = 22, P3 = 42, P4 = 62, P5 = 88, P6 = 112, PDAUER = 152;

// Das Maedchen: was sie noch anhat, haengt davon ab, was sie schon verschenkt hat (0..5)
static FrauLook maedchen(int verschenkt) {
  FrauLook l;
  l.haar = FARBE(0xF2C94E);
  l.kleid = FARBE(0x9A6A4A);           // Roecklein
  l.schuerze = FARBE(0xC9884A);        // Leibchen
  l.frisur = 3;
  l.tuch = FARBE(0xC0392B);            // Muetzchen
  l.flicken = true;
  if (verschenkt >= 2) { l.frisur = 1; l.tuch = 0; }
  if (verschenkt >= 3) l.schuerze = 0;
  if (verschenkt >= 4) { l.kleid = FARBE(0xE8E0D0); l.flicken = false; }   // nur noch das Hemdlein
  if (verschenkt >= 5) l.kleid = FARBE(0x6E7090);                          // im Dunkeln kaum zu sehen
  return l;
}
static FrauLook leinen() { FrauLook l = maedchen(0); l.kleid = WEISS; l.schuerze = 0; l.frisur = 0; l.tuch = 0; l.glanz = true; l.flicken = false; return l; }
static const FrauLook M0 = maedchen(0), M1 = maedchen(1), M2 = maedchen(2), M3 = maedchen(3), M4 = maedchen(4), M5 = maedchen(5),
                      LEINEN = leinen();

static Aussehen bettlerLook() {
  Aussehen a; a.weste = FARBE(0x6E6A60); a.hemd = FARBE(0xA89A80); a.hose = FARBE(0x4E4A44); a.haar = FARBE(0xBDBDBD);
  a.muetze = 1; a.wangen = false;
  return a;
}
static Aussehen kindJ() { Aussehen a; a.weste = FARBE(0x8E8676); a.hemd = FARBE(0xC9C0B0); a.hose = FARBE(0x5E5A54); a.haar = FARBE(0x6E4A2A); a.muetze = 1; return a; }
static const Aussehen BETTLER = bettlerLook(), KIND_J = kindJ();
static FrauLook kindM() { FrauLook l; l.kleid = FARBE(0xA8A090); l.haar = FARBE(0x8A5A2E); l.frisur = 1; l.flicken = true; return l; }
static FrauLook kindM2() { FrauLook l = kindM(); l.schuerze = FARBE(0xC9884A); return l; }
static const FrauLook KIND_M = kindM(), KIND_M2 = kindM2();

// ---------- Kulissen ----------
static void feld(Gfx &g, float t, float abend) {
  himmel(g, abend, t, 196);
  if (abend < 0.5f) sonne(g, 248, 30, 12, t);
  huegel(g, 0, 0.3f, 196, 26, 110, mischeFarbe(FARBE(0x8ACB6A), FARBE(0x3A4A5A), abend));
  boden(g, 196, mischeFarbe(GRAS, FARBE(0x3A5A4A), abend));
  g.rechteck(0, 206, 284, 16, mischeFarbe(FARBE(0xD8C49A), FARBE(0x6E6450), abend));
  for (int i = 0; i < 3; i++) laubbaum(g, 30 + i * 110, 200, 0.7f, mischeFarbe(FARBE(0x4CB85A), FARBE(0x2E5A4A), abend));
}

static void nachtwald(Gfx &g, float t, float sterne) {
  himmel(g, 1, t, 200);
  for (int i = 0; i < 40; i++) {
    float fu = 0.5f + 0.5f * sinf(t * 2 + i * 1.7f);
    g.stern(zufall(i + 700) * 284, zufall(i + 720) * 120, 1.6f + fu * 1.6f * (1 + sterne), 0.7f, 4, 0, i % 3 ? WEISS : GOLD,
            (uint8_t)(120 + 120 * fu));
  }
  mond(g, 40, 36, 11);
  for (int i = 0; i < 6; i++) tanne(g, i * 54 + 6, 200, 0.85f + (i % 2) * 0.15f, FARBE(0x1F3B45));
  boden(g, 200, FARBE(0x2F4A40));
}

static void taler(Gfx &g, float x, float y, float r, float t) {
  g.kontur(1, FARBE(0x9A6A1E));
  g.kreis(x, y, r, GOLD);
  g.konturAus();
  g.kreis(x - r * 0.3f, y - r * 0.3f, r * 0.3f, WEISS, 170);
  g.stern(x, y, r * 0.6f, r * 0.25f, 5, t, FARBE(0xE0A23C));
}

static FrauPose mp(const FrauLook &l, float t) { return fposis(l, t, 1); }

// eine Gabe-Szene: das Maedchen gibt dem Beduerftigen etwas
static void gabe(Gfx &g, float u, const FrauLook &vorher, const FrauLook &nachher, float wann, Farbe was, bool kopf) {
  FrauPose p = mp(u < wann ? vorher : nachher, u);
  p.armV = u > wann - 1 && u < wann + 1.5f ? 1.7f : 0.8f;
  p.mund = 0.2f;
  float hx, hy;
  frau(g, 110, 214, 0.85f, 1, p, &hx, &hy);
  float k = seg(u, wann - 0.6f, wann + 1);
  if (k > 0 && k < 1) {
    float x = lerp(hx + 4, 180, k), y = lerp(hy, kopf ? 150 : 170, k) - huepf(k) * 16;
    g.rundRechteck(x - 6, y - 4, 12, 8, 3, was);
  }
  if (u > wann + 1 && u < wann + 4) herzen(g, 140, 160, u, 2, 14);
}

// =====================================================================
// Szenen
// =====================================================================
// 1) Das arme Maedchen
static void szeneAllein(Gfx &g, float u) {
  feld(g, u, 0);
  FrauPose p = mp(M0, u);
  p.lauf = u > 8 ? fmodf(u * 1.2f, 1) : -1;
  p.armV = 1.3f;
  float hx, hy;
  frau(g, u > 8 ? lerp(80, 160, seg(u, 8, 21)) : 80, 214, 0.85f, 1, p, &hx, &hy);
  g.ellipse(hx + 4, hy, 6, 4, FARBE(0xC9884A));  // das Stueckchen Brot
  g.kameraAus();
  vers(g, {"Es war einmal ein kleines Mädchen,", "dem waren Vater und Mutter", "gestorben."}, aufzu(u, 0.4f, 7));
  vers(g, {"Es hatte nichts mehr als die", "Kleider auf dem Leib und", "ein Stückchen Brot."}, aufzu(u, 7.2f, 14));
  vers(g, {"Weil es so gut und fromm war,", "ging es hinaus ins Feld."}, aufzu(u, 14.2f, 21.6f));
  blende(g, u, 22, 140, 160);
}

// 2) Der arme Mann: das Brot
static void szeneBrot(Gfx &g, float u) {
  feld(g, u, 0.1f);
  Pose b = posis(BETTLER, u);
  b.armV = 1.6f;
  b.mund = u > 2 && u < 7 ? 0.5f : 0;
  schneiderlein(g, 190, 214, 0.95f, -1, b);
  gabe(g, u, M0, M1, 8, FARBE(0xC9884A), false);
  g.kameraAus();
  blase(g, 180, 50, {"Ach, gib mir etwas zu essen,", "ich bin so hungrig!"}, aufzu(u, 2, 7), 190, 140);
  blase(g, 100, 60, {"Gott segne dir's!"}, aufzu(u, 9, 13), 110, 140);
  vers(g, {"Es reichte ihm das ganze", "Stückchen Brot und ging weiter."}, aufzu(u, 13.4f, 19.6f));
  blende(g, u, 20, 140, 160);
}

// 3) Das Kind, das am Kopf friert: das Muetzchen
static void szeneMuetze(Gfx &g, float u) {
  feld(g, u, 0.2f);
  Pose k = posis(KIND_J, u);
  k.armV = 2.2f;
  k.mund = u > 2 && u < 7 ? 0.5f : 0;
  k.zu = u > 10;
  schneiderlein(g, 186, 214, 0.7f, -1, k);
  if (u > 9) {  // jetzt hat das Kind das rote Muetzchen auf
    g.kontur(1, OUT);
    float m[] = {170, 214 - 56, 192, 214 - 58, 184, 214 - 66, 174, 214 - 64};
    g.polygon(m, 4, FARBE(0xC0392B));
    g.konturAus();
  }
  gabe(g, u, M1, M2, 8, FARBE(0xC0392B), true);
  g.kameraAus();
  blase(g, 180, 60, {"Es friert mich so an meinem", "Kopfe, schenk mir etwas,", "womit ich ihn bedecken kann!"}, aufzu(u, 2, 7.6f), 186, 150);
  vers(g, {"Da tat es seine Mütze ab", "und gab sie ihm."}, aufzu(u, 11, 19.6f));
  blende(g, u, 20, 140, 160);
}

// 4) Ein Kind ohne Leibchen, eines ohne Roecklein
static void szeneLeibchen(Gfx &g, float u) {
  feld(g, u, 0.4f + seg(u, 0, 26) * 0.3f);
  if (u < 13) {
    FrauPose k = fposis(u > 7 ? KIND_M2 : KIND_M, u);  // danach traegt das Kind das Leibchen
    k.armV = 2.0f;
    k.mund = u > 1.6f && u < 5.4f ? 0.5f : 0;
    k.weint = u < 6;
    frau(g, 186, 214, 0.66f, -1, k);
    gabe(g, u, M2, M3, 6, FARBE(0xC9884A), false);
    g.kameraAus();
    blase(g, 180, 70, {"Mich friert so ohne Leibchen!"}, aufzu(u, 1.6f, 5.4f), 186, 150);
  } else {
    float v = u - 13;
    Pose k = posis(KIND_J, u, 2);
    k.armV = 2.0f;
    k.mund = v > 1.6f && v < 5.4f ? 0.5f : 0;
    schneiderlein(g, 186, 214, 0.66f, -1, k);
    gabe(g, v, M3, M4, 6, FARBE(0x9A6A4A), false);
    g.kameraAus();
    blase(g, 180, 70, {"Gib mir doch dein Röcklein!"}, aufzu(v, 1.6f, 5.4f), 186, 150);
    vers(g, {"Und es gab auch das noch hin."}, aufzu(v, 7, 12.6f));
  }
  blende(g, u, 26, 140, 160);
}

// 5) Im dunklen Wald: das Hemdlein
static void szeneHemd(Gfx &g, float u) {
  nachtwald(g, u, 0);
  FrauPose k = fposis(KIND_M, u, 3);
  k.armV = 2.0f;
  k.mund = u > 2 && u < 7 ? 0.5f : 0;
  frau(g, 190, 214, 0.66f, -1, k);
  FrauPose p = mp(u < 13 ? M4 : M5, u);
  p.armV = u > 11 && u < 14 ? 1.7f : 0.8f;
  p.mund = 0.2f;
  p.zu = u > 8 && u < 11;
  frau(g, 110, 214, 0.85f, 1, p);
  if (u > 12 && u < 14) g.rundRechteck(lerp(124, 180, seg(u, 12, 14)) - 6, 166, 12, 8, 3, FARBE(0xE8E0D0));
  g.kameraAus();
  vers(g, {"Endlich kam es in einen Wald,", "und es war schon dunkel geworden."}, aufzu(u, 0.2f, 1.9f));
  blase(g, 180, 70, {"Gib mir doch dein Hemdlein!"}, aufzu(u, 2, 7), 190, 160);
  vers(g, {"Es ist dunkle Nacht,", "da sieht dich niemand, du kannst", "wohl dein Hemd weggeben."}, aufzu(u, 7.4f, 12.6f));
  vers(g, {"Und es zog das Hemd ab", "und gab es auch noch hin."}, aufzu(u, 13, 23.6f));
  blende(g, u, 24, 140, 160);
}

// 6) Die Sterne fallen vom Himmel und sind lauter blanke Taler
static void szeneTaler(Gfx &g, float u) {
  nachtwald(g, u, seg(u, 0, 6));
  bool neu = u > 9;
  FrauPose p = mp(neu ? LEINEN : M5, u);
  p.armV = 2.0f + sinf(u * 2) * 0.2f;
  p.armH = 2.0f - sinf(u * 2) * 0.2f;
  p.mund = 0.5f;
  p.zu = u < 9;
  p.drehen = neu ? sinf(u * 3) * 3 : 0;
  frau(g, 142, 214, 0.9f, 1, p);
  if (u > 8 && u < 11) puff(g, 142, 160, seg(u, 8, 11), GOLD);
  if (neu) glitzern(g, 142, 160, 34, u, 8);
  // die fallenden Sterne werden zu Talern
  for (int i = 0; i < 18; i++) {
    float start = 2 + zufall(i + 200) * 20;
    float k = seg(u, start, start + 3);
    if (k <= 0) continue;
    float x = 40 + zufall(i + 220) * 204, y = lerp(-10, 214 - (i % 3) * 3, hinein(k));
    if (k < 0.6f) g.stern(x, y, 5, 2, 5, u * 3 + i, GOLD);
    else taler(g, x, y, 5, u + i);
  }
  // sie sammelt die Taler in ihr neues Hemdlein
  int haufen = (int)(seg(u, 14, 30) * 12);
  for (int i = 0; i < haufen; i++) taler(g, 124 + (i % 6) * 7, 210 - (i / 6) * 5, 4, u + i);
  g.kameraAus();
  vers(g, {"Da fielen auf einmal die Sterne", "vom Himmel und waren lauter", "harte, blanke Taler."}, aufzu(u, 1, 9));
  vers(g, {"Und es hatte ein neues Hemdlein", "an, und das war vom", "allerfeinsten Linnen."}, aufzu(u, 9.4f, 18));
  vers(g, {"Da sammelte es die Taler hinein", "und war reich für sein Lebtag."}, aufzu(u, 18.4f, 30));
  endeBild(g, u, 34);
}

static void bild(Gfx &g, float t) {
  if (t < P2) szeneAllein(g, t - P1);
  else if (t < P3) szeneBrot(g, t - P2);
  else if (t < P4) szeneMuetze(g, t - P3);
  else if (t < P5) szeneLeibchen(g, t - P4);
  else if (t < P6) szeneHemd(g, t - P5);
  else szeneTaler(g, t - P6);
}

// =====================================================================
// Musik
// =====================================================================
// Sterntaler (e-Moll, zart; am Ende E-Dur)
static const Ton3 MEL_STERN[] = {
  {71, 1, 52}, {76, 1, 0}, {74, .5, 0}, {72, .5, 0}, {71, 1, 0}, {67, 1, 48}, {69, .5, 0}, {71, .5, 0}, {72, 2, 0},
  {71, 1, 52}, {69, .5, 0}, {67, .5, 0}, {66, 1, 0}, {67, 1, 47}, {69, 1, 0}, {71, .5, 0}, {69, .5, 0}, {64, 2, 52}};
static const Ton3 MEL_STERN_DUR[] = {
  {76, 1, 52}, {80, 1, 0}, {83, .5, 0}, {81, .5, 0}, {80, 1, 0}, {76, 1, 49}, {78, .5, 0}, {80, .5, 0}, {81, 2, 0},
  {80, 1, 52}, {78, .5, 0}, {76, .5, 0}, {75, 1, 0}, {76, 1, 47}, {78, 1, 0}, {80, .5, 0}, {78, .5, 0}, {76, 2, 52}};

static void gabeKlang(Synth &s, float x, float t0, float t1) {
  if (bei(x, t0, t1)) for (int k = 0; k < 5; k++) s.glocke(x + k * 0.12f, 79 + k * 2, 0.35f);
}

static void ton(Synth &s, float t0, float t1) {
  spieleMelodie(s, M(MEL_STERN), P1 + 0.5f, P6, 0.42f, t0, t1, 0.75f);
  gabeKlang(s, P2 + 8, t0, t1);
  gabeKlang(s, P3 + 8, t0, t1);
  gabeKlang(s, P4 + 6, t0, t1);
  gabeKlang(s, P4 + 19, t0, t1);
  gabeKlang(s, P5 + 13, t0, t1);
  float a = P6;
  for (int i = 0; i < 18; i++) {
    float x = a + 2 + zufall(i + 200) * 20 + 3;
    if (bei(x, t0, t1)) s.glocke(x, 84 + (i % 5) * 2, 0.35f);  // Taler klingen
  }
  if (bei(a + 8.4f, t0, t1)) for (int k = 0; k < 14; k++) s.glocke(a + 8.4f + k * 0.12f, 76 + (k % 7) * 2, 0.5f);  // das Hemdlein
  spieleMelodie(s, M(MEL_STERN_DUR), a + 10, a + 36, 0.4f, t0, t1, 0.85f);
  endeKlang(s, a + 36.2f, t0, t1);
}

// =====================================================================
// Startbild, Vorschau, Erkennungszeichen
// =====================================================================
static void titelSzene(Gfx &g, float t) {
  nachtwald(g, t, 1);
  FrauPose p = mp(LEINEN, t);
  p.armV = 2.1f;
  p.armH = 2.1f;
  p.mund = 0.4f;
  frau(g, 142, 222, 0.95f, 1, p);
  for (int i = 0; i < 10; i++) {
    float k = fmodf(t * 0.25f + i / 10.0f, 1);
    float x = 50 + zufall(i + 220) * 184, y = lerp(-10, 214, k);
    taler(g, x, y, 5, t + i);
  }
}
static void titelbild(Gfx &g, float t) { titelSzene(g, t); titelRahmen(g, t, "Die Sterntaler", 184, SCHRIFT_M); }
static void vorschau(Gfx &g, float t) { titelSzene(g, t); }
static void symbol(Gfx &g, float x, float y, float t) {
  for (int i = 0; i < 7; i++) {
    float k = fmodf(t * 0.3f + i / 7.0f, 1);
    float px = x - 46 + zufall(i + 30) * 92, py = y - 76 + k * 70;
    if (k < 0.5f) g.stern(px, py, 6, 2.4f, 5, t + i, GOLD);
    else taler(g, px, py, 6, t + i);
  }
  for (int i = 0; i < 5; i++) taler(g, x - 16 + i * 8, y - 4 - (i & 1) * 3, 6, t + i);
}

const Maerchen STERNTALER = {"Die Sterntaler", PDAUER, FARBE(0x3A4A8E), bild, ton, titelbild, vorschau, symbol,
                             LIED(MEL_STERN, 0.42f), nullptr};

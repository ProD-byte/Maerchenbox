// Der suesse Brei - ein Spieluhr-Film fuer die Maerchenbox, nach dem Maerchen der Brueder Grimm.
// "Toepfchen, koche!" - "Toepfchen, steh!" - und als die Mutter das Wort vergisst, fliesst der Brei durchs ganze Staedtchen.
#include "erzaehlen.h"

// ---------- Ablauf (Sekunden) ----------
static const float P1 = 0, P2 = 28, P3 = 52, P4 = 100, PDAUER = 132;

static const Farbe BREI = FARBE(0xF6E2A8), BREI_D = FARBE(0xE0C47A);

static FrauLook maedchenLook() {
  FrauLook l; l.kleid = FARBE(0x5B8FD7); l.schuerze = FARBE(0xF6F0E4); l.haar = FARBE(0xC98A3A); l.frisur = 1; l.flicken = true;
  return l;
}
static FrauLook mutterLook() {
  FrauLook l; l.kleid = FARBE(0x8E6A4A); l.schuerze = FARBE(0xE8D8B8); l.haar = FARBE(0x8A5A2E); l.frisur = 3; l.tuch = FARBE(0xC9B07A);
  l.flicken = true;
  return l;
}
static FrauLook alteLook() {
  FrauLook l; l.kleid = FARBE(0x6E5A8E); l.schuerze = FARBE(0xC9C0E0); l.haar = FARBE(0xF2F2F2); l.frisur = 3; l.tuch = FARBE(0x9A8AB8);
  l.glanz = true;
  return l;
}
static const FrauLook MAEDCHEN = maedchenLook(), MUTTER = mutterLook(), ALTE = alteLook();

// ---------- Dinge ----------
static void toepfchen(Gfx &g, float x, float y, float s, float t, float kocht) {
  g.kontur(1.4f * s, OUT);
  float k[] = {x - 14 * s, y - 20 * s, x + 14 * s, y - 20 * s, x + 11 * s, y, x - 11 * s, y};
  g.polygon(k, 4, FARBE(0xC0632E));
  g.konturAus();
  g.ellipse(x, y - 20 * s, 15 * s, 4 * s, FARBE(0xA0502A));
  g.bogen(x - 15 * s, y - 13 * s, 4 * s, 1.6f, 4.7f, 2 * s, FARBE(0xA0502A));
  g.bogen(x + 15 * s, y - 13 * s, 4 * s, -1.5f, 1.6f, 2 * s, FARBE(0xA0502A));
  for (int i = 0; i < 3; i++) g.kreis(x - 6 * s + i * 6 * s, y - 10 * s, 1.6f * s, FARBE(0xF2C94E));  // Punkte
  if (kocht > 0) {
    g.ellipse(x, y - 21 * s - kocht * 4 * s, 13 * s, (3 + kocht * 4) * s, BREI);
    for (int i = 0; i < 3; i++) {
      float ph = fmodf(t * 0.7f + i * 0.33f, 1);
      g.kurve(x - 6 * s + i * 6 * s, y - 26 * s - ph * 16 * s, x - 9 * s + i * 6 * s, y - 32 * s - ph * 16 * s,
              x - 4 * s + i * 6 * s, y - 38 * s - ph * 16 * s, 1.2f * s, WEISS, (uint8_t)(160 * (1 - ph) * kocht));
    }
  }
}

static void schuessel(Gfx &g, float x, float y, float voll) {
  g.kontur(1.2f, OUT);
  float q[] = {x - 14, y - 10, x + 14, y - 10, x + 9, y, x - 9, y};
  g.polygon(q, 4, WEISS);
  g.konturAus();
  if (voll > 0) g.ellipse(x, y - 10, 12, 3 * voll, BREI);
}

// der Brei fliesst: Hoehe h (vom Boden), mit welliger Oberflaeche
static void breiFlut(Gfx &g, float h, float t, float xa = 0, float xb = 284) {
  if (h <= 0) return;
  float pts[2 * 22];
  int n = 0;
  for (int i = 0; i <= 18; i++) {
    float x = xa + (xb - xa) * i / 18.0f;
    pts[2 * n] = x;
    pts[2 * n + 1] = 240 - h + sinf(x * 0.08f + t * 2) * 3 + sinf(x * 0.03f - t) * 2;
    n++;
  }
  pts[2 * n] = xb; pts[2 * n + 1] = 240; n++;
  pts[2 * n] = xa; pts[2 * n + 1] = 240; n++;
  g.kontur(1.2f, BREI_D);
  g.polygon(pts, n, BREI);
  g.konturAus();
  for (int i = 0; i < 5; i++) {
    float bx = xa + fmodf(i * 61 + t * 12, xb - xa);
    g.kreis(bx, 240 - h + 4, 3 + (i % 2) * 2, WEISS, 140);
  }
}

// ---------- Kulissen ----------
static void wald(Gfx &g, float t) {
  himmel(g, 0, t, 200);
  sonne(g, 248, 30, 11, t);
  for (int i = 0; i < 6; i++) tanne(g, i * 52 + 4, 200, 0.8f + (i % 3) * 0.18f, FARBE(0x2F8F4E));
  boden(g, 200);
  for (int i = 0; i < 6; i++) g.kreis(30 + i * 46, 210, 4, FARBE(0xC0392B));  // Beeren
}

static void stube(Gfx &g, float t) {
  g.verlauf(0, 200, FARBE(0xE8D2B0), FARBE(0xD6B88E));
  g.rechteck(0, 200, 284, 40, FARBE(0x9E7656));
  g.kontur(1.8f, OUT);
  g.rundRechteck(20, 120, 90, 82, 6, FARBE(0xA89888));
  g.konturAus();
  g.rundRechteck(34, 150, 62, 50, 18, FARBE(0x2E2620));
  for (int i = 0; i < 3; i++) g.ellipse(50 + i * 16, 194, 5, 3 + sinf(t * 8 + i), FARBE(0xFF8A2A));
  g.kontur(1.6f, OUT);
  g.rundRechteck(180, 40, 70, 60, 4, HOLZ);
  g.konturAus();
  g.rundRechteck(185, 45, 60, 50, 2, FARBE(0x9FD8FF));
  g.rechteck(213, 45, 3, 50, HOLZ);
  // Tisch
  g.kontur(1.4f, OUT);
  g.rundRechteck(130, 160, 120, 8, 2, HOLZ);
  g.konturAus();
  g.rechteck(140, 168, 6, 46, HOLZ_D);
  g.rechteck(234, 168, 6, 46, HOLZ_D);
}

static void gasse(Gfx &g, float t, float scroll) {
  himmel(g, 0, t, 196);
  sonne(g, 248, 30, 11, t);
  for (int i = 0; i < 6; i++) {
    float x = fmodf(i * 70 - scroll + 4200, 420) - 70;
    Farbe w = i & 1 ? FARBE(0xF4E4C4) : FARBE(0xF0D8C0);
    g.kontur(1.6f, OUT);
    g.rechteck(x, 110 + (i % 3) * 8, 62, 90 - (i % 3) * 8, w);
    float d[] = {x - 6, 112 + (i % 3) * 8, x + 31, 78 + (i % 3) * 8, x + 68, 112 + (i % 3) * 8};
    g.polygon(d, 3, i % 3 == 0 ? FARBE(0xC0503A) : i % 3 == 1 ? FARBE(0x9A5A3A) : FARBE(0x5B7FC7));
    g.konturAus();
    g.rundRechteck(x + 22, 160, 18, 40, 8, HOLZ);
    g.rundRechteck(x + 8, 132 + (i % 3) * 4, 14, 14, 2, FARBE(0x8FD3FF));
    g.rundRechteck(x + 40, 132 + (i % 3) * 4, 14, 14, 2, FARBE(0x8FD3FF));
  }
  g.rechteck(0, 200, 284, 40, FARBE(0xB8A890));
  for (int i = 0; i < 14; i++) g.ellipse(fmodf(i * 24 - scroll + 3360, 336) - 26, 214 + (i & 1) * 10, 9, 4, FARBE(0xA89880));  // Pflaster
}

// =====================================================================
// Szenen
// =====================================================================
// 1) Im Wald: die alte Frau schenkt dem Maedchen das Toepfchen
static void szeneWald(Gfx &g, float u) {
  wald(g, u);
  FrauPose a = fposis(ALTE, u);
  a.armV = u > 6 && u < 10 ? 1.8f : 1.2f;
  a.mund = (u > 6 && u < 10) || (u > 10.4f && u < 20) ? 0.5f : 0;
  float ahx, ahy;
  frau(g, 190, 214, 0.95f, -1, a, &ahx, &ahy);
  FrauPose m = fposis(MAEDCHEN, u, 1);
  m.armV = u > 8 ? 1.6f : 0.8f;
  m.mund = u > 20.4f && u < 24 ? 0.5f : 0.1f;
  m.weint = u < 5;
  float mhx, mhy;
  frau(g, lerp(-20, 110, sanft(seg(u, 0, 3))), 214, 0.82f, 1, m, &mhx, &mhy);
  float k = seg(u, 8, 9.6f);
  if (u < 8) toepfchen(g, ahx - 6, ahy + 8, 0.7f, u, 0);
  else toepfchen(g, lerp(ahx - 6, mhx + 6, k), lerp(ahy + 8, mhy + 8, k), 0.7f, u, 0);
  if (u > 8 && u < 11) glitzern(g, 150, 170, 20, u, 5);
  g.kameraAus();
  vers(g, {"Es war ein armes, frommes Mädchen,", "das hatte nichts mehr zu essen."}, aufzu(u, 0.4f, 6));
  blase(g, 180, 40, {"Das ist ein Töpfchen. Sagst du:", "Töpfchen, koche! - so kocht es", "guten, süßen Hirsebrei."}, aufzu(u, 6.2f, 13), 190, 130);
  blase(g, 180, 40, {"Und sagst du:", "Töpfchen, steh! -", "so hört es wieder auf."}, aufzu(u, 13.2f, 20), 190, 130);
  blase(g, 100, 60, {"Danke schön!"}, aufzu(u, 20.4f, 24), 110, 140);
  blende(g, u, 28, 150, 160);
}

// 2) Daheim: "Toepfchen, koche!" - "Toepfchen, steh!"
static void szeneKoche(Gfx &g, float u) {
  stube(g, u);
  float kocht = seg(u, 4, 6) * (1 - seg(u, 15, 17));
  toepfchen(g, 190, 160, 1, u, kocht);
  schuessel(g, 160, 160, seg(u, 7, 10));
  schuessel(g, 220, 160, seg(u, 8, 11));
  FrauPose m = fposis(MAEDCHEN, u, 1);
  m.mund = (u > 2 && u < 5) || (u > 13 && u < 16) ? 0.6f : (u > 17 ? 0.3f * fabsf(sinf(u * 5)) : 0);
  m.armV = u > 17 ? 1.6f + sinf(u * 5) * 0.2f : 1.0f;
  frau(g, 140, 214, 0.82f, 1, m);
  FrauPose mu = fposis(MUTTER, u);
  mu.mund = u > 17 ? 0.3f * fabsf(sinf(u * 5 + 1)) : (u > 6 && u < 12 ? 0.6f : 0);
  mu.armV = u > 6 && u < 12 ? 2.2f : 1.4f;
  frau(g, 250, 214, 0.95f, -1, mu);
  g.kameraAus();
  blase(g, 110, 60, {"Töpfchen, koche!"}, aufzu(u, 2, 5), 130, 140);
  blase(g, 230, 50, {"Ei, wie gut!"}, aufzu(u, 6.4f, 12), 246, 130);
  blase(g, 110, 60, {"Töpfchen, steh!"}, aufzu(u, 13, 16), 130, 140);
  vers(g, {"Nun waren sie ihrer Armut und", "ihres Hungers ledig und", "aßen süßen Brei, so oft sie wollten."}, aufzu(u, 17, 23.6f));
  blende(g, u, 24, 190, 150);
}

// 3) Die Mutter allein: sie weiss das Wort nicht mehr - der Brei fliesst ueber
static void szeneUeber(Gfx &g, float u) {
  if (u < 22) {
    stube(g, u);
    float kocht = seg(u, 3, 5);
    float flut = seg(u, 6, 21);
    toepfchen(g, 190, 160, 1, u, kocht);
    if (u > 6) {  // ueber den Rand
      g.ellipse(190, 136 - flut * 10, 18 + flut * 30, 8 + flut * 10, BREI);
      g.rundRechteck(150 - flut * 20, 156, 80 + flut * 40, 10 + flut * 50, 8, BREI);
    }
    breiFlut(g, flut * 70, u);
    FrauPose mu = fposis(MUTTER, u);
    mu.mund = (u > 1 && u < 4) || u > 9 ? 0.6f : 0.2f;
    mu.armV = u > 9 ? 2.6f + sinf(u * 9) * 0.3f : 1.4f;
    mu.armH = u > 9 ? 2.6f - sinf(u * 9) * 0.3f : 0.4f;
    frau(g, 90, 214 - flut * 10, 0.95f, 1, mu);
    g.kameraAus();
    vers(g, {"Einmal war das Mädchen", "ausgegangen, da sprach die Mutter:"}, aufzu(u, 0.2f, 1.6f));
    blase(g, 100, 60, {"Töpfchen, koche!"}, aufzu(u, 1.6f, 4.4f), 96, 140);
    blase(g, 100, 60, {"Halt! Hör auf!", "Wie hieß nur das Wort?"}, aufzu(u, 9.6f, 15), 96, 140);
    vers(g, {"Aber sie wusste das Wort nicht,", "und es kochte immer fort."}, aufzu(u, 15.4f, 21.6f));
    return;
  }
  // der Brei fliesst aus dem Haus, durch die Gasse, durchs ganze Staedtchen
  float v = u - 22;
  float scroll = v * 30;
  gasse(g, u, scroll);
  breiFlut(g, 20 + v * 2.2f, u);
  for (int i = 0; i < 3; i++) {  // die Leute staunen aus den Fenstern
    float x = fmodf(i * 140 - scroll + 4200, 420) - 70 + 14;
    g.kreis(x + 1, 140, 5, HAUT);
    g.ellipse(x + 3, 143, 1.4f, 2.2f, FARBE(0x8A2A2A));
  }
  g.kameraAus();
  vers(g, {"Der Brei stieg über den Rand,", "füllte die Küche und", "das ganze Haus,"}, aufzu(v, 0.6f, 9));
  vers(g, {"dann das zweite Haus und", "die Straße, als wollt's die", "ganze Welt satt machen."}, aufzu(v, 9.2f, 18));
  vers(g, {"Und niemand wusste sich zu helfen."}, aufzu(v, 18.2f, 25.6f));
}

// 4) Das Maedchen kommt: "Toepfchen, steh!" - wer in die Stadt will, muss sich durchessen
static void szeneSteh(Gfx &g, float u) {
  gasse(g, u, 0);
  float h = 74 - seg(u, 8, 26) * 40;
  breiFlut(g, h, u);
  FrauPose m = fposis(MAEDCHEN, u, 1);
  m.mund = u > 3 && u < 6.4f ? 0.7f : 0.3f;
  m.armV = u > 3 && u < 6.4f ? 2.4f : 1.2f;
  frau(g, 60, 238 - h + 10, 0.82f, 1, m);
  // die Leute essen sich durch
  for (int i = 0; i < 4; i++) {
    float x = 120 + i * 40;
    float k = seg(u, 9 + i, 26);
    if (k <= 0) continue;
    g.kreis(x, 238 - h + 4, 8, HAUT);
    g.ellipse(x - 1, 238 - h - 2, 8, 4, FARBE(0x8A5A2E));
    g.ellipse(x + 3, 238 - h + 7, 2.4f, 1 + fabsf(sinf(u * 8 + i)) * 2, FARBE(0x8A2A2A));
    g.linie(x + 10, 238 - h + 10, x + 16, 238 - h - 4, 2, FARBE(0xC0C8D4));  // Loeffel
  }
  g.kameraAus();
  vers(g, {"Endlich, als nur noch ein", "einziges Haus übrig war,", "kam das Kind heim."}, aufzu(u, 0.4f, 3));
  blase(g, 120, 90, {"Töpfchen, steh!"}, aufzu(u, 3, 6.4f), 70, 150);
  vers(g, {"Und wer wieder in die Stadt", "wollte, der musste sich", "durchessen."}, aufzu(u, 8, 26));
  endeBild(g, u, 27);
}

static void bild(Gfx &g, float t) {
  if (t < P2) szeneWald(g, t - P1);
  else if (t < P3) szeneKoche(g, t - P2);
  else if (t < P4) szeneUeber(g, t - P3);
  else szeneSteh(g, t - P4);
}

// =====================================================================
// Musik
// =====================================================================
// das Breilied (C-Dur, gemuetlich, wird beim Ueberlaufen immer schneller)
static const Ton3 MEL_BREI[] = {
  {72, .5, 48}, {74, .5, 0}, {76, .5, 0}, {77, .5, 0}, {79, 1, 43}, {79, 1, 0},
  {81, .5, 41}, {81, .5, 0}, {81, .5, 0}, {81, .5, 0}, {79, 2, 48},
  {77, .5, 41}, {77, .5, 0}, {77, .5, 0}, {77, .5, 0}, {76, 1, 48}, {76, 1, 0},
  {74, .5, 43}, {74, .5, 0}, {74, .5, 0}, {74, .5, 0}, {72, 2, 48}};

static void blubb(Synth &s, float x, float tief) { s.ton(x, 220 * tief, 0.05f, 0.005f, 0.12f, SINUS, GERAEUSCH, 380 * tief); }

static void ton(Synth &s, float t0, float t1) {
  float a = P1;
  spieleMelodie(s, M(MEL_WIEGE), a + 0.5f, a + 6, 0.45f, t0, t1, 0.6f);
  if (bei(a + 8.4f, t0, t1)) for (int k = 0; k < 8; k++) s.glocke(a + 8.4f + k * 0.12f, 79 + k * 2, 0.4f);
  spieleMelodie(s, M(MEL_BREI), a + 10, a + 27.5f, 0.32f, t0, t1, 0.7f);
  a = P2;
  for (float x = ceilf(t0 / 0.35f) * 0.35f; x < t1; x += 0.35f)
    if (x > a + 4 && x < a + 15) blubb(s, x, 1);
  spieleMelodie(s, M(MEL_BREI), a + 6, a + 23.5f, 0.3f, t0, t1, 0.8f);
  a = P3;
  for (float x = ceilf(t0 / 0.2f) * 0.2f; x < t1; x += 0.2f)
    if (x > a + 3 && x < a + 48) blubb(s, x, 0.8f + 0.2f * sinf(x * 3));
  spieleMelodie(s, M(MEL_BREI), a + 5, a + 22, 0.26f, t0, t1, 0.7f);
  spieleMelodie(s, M(MEL_BREI), a + 22, a + 47.5f, 0.17f, t0, t1, 0.8f);
  a = P4;
  if (bei(a + 3.2f, t0, t1)) for (int k = 0; k < 8; k++) s.glocke(a + 3.2f + k * 0.12f, 84 - k * 2, 0.45f);  // steh!
  spieleMelodie(s, M(MEL_BREI), a + 7, a + 27, 0.34f, t0, t1, 0.85f);
  endeKlang(s, a + 29.2f, t0, t1);
}

// =====================================================================
// Startbild, Vorschau, Erkennungszeichen
// =====================================================================
static void titelSzene(Gfx &g, float t) {
  gasse(g, t, 40);
  breiFlut(g, 36, t);
  toepfchen(g, 150, 214, 1.6f, t, 1);
  g.ellipse(150, 176, 22, 12, BREI);
  FrauPose m = fposis(MAEDCHEN, t, 1);
  m.armV = 2.2f;
  m.mund = 0.5f;
  frau(g, 80, 214, 0.9f, 1, m);
}
static void titelbild(Gfx &g, float t) { titelSzene(g, t); titelRahmen(g, t, "Der süße Brei", 180, SCHRIFT_M); }
static void vorschau(Gfx &g, float t) { titelSzene(g, t); }
static void symbol(Gfx &g, float x, float y, float t) {
  toepfchen(g, x, y - 6, 2.2f, t, 1);
  // Brei quillt ueber den Rand
  g.ellipse(x, y - 60 - sinf(t * 2) * 3, 30, 12, BREI);
  g.ellipse(x - 26, y - 44, 8, 16, BREI);
  g.ellipse(x + 28, y - 40, 8, 20, BREI);
  g.ellipse(x, y - 2, 46, 6, BREI);
}

const Maerchen SUESSERBREI = {"Der süße Brei", PDAUER, FARBE(0xE0C47A), bild, ton, titelbild, vorschau, symbol, LIED(MEL_BREI, 0.32f),
                              nullptr};

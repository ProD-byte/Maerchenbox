// Des Kaisers neue Kleider - ein Spieluhr-Film fuer die Maerchenbox, nach dem Maerchen von Hans Christian Andersen.
// Zwei Betrueger weben Stoff, den nur Kluge sehen koennen; der Minister sieht nichts, der Kaiser sieht nichts -
// und beim Festzug ruft ein Kind: "Aber er hat ja gar nichts an!"
#include "erzaehlen.h"

// ---------- Ablauf (Sekunden) ----------
static const float P1 = 0, P2 = 26, P3 = 58, P4 = 88, P5 = 122, PDAUER = 186;

static Aussehen kaiserLook() {
  Aussehen a; a.weste = FARBE(0x7A2A8E); a.hemd = GOLD; a.hose = FARBE(0x3A2A6E); a.haar = FARBE(0x8E8E8E); a.muetze = 3;
  a.halstuch = FARBE(0xF2F2F2);
  return a;
}
static Aussehen kaiser2Look() { Aussehen a = kaiserLook(); a.weste = FARBE(0x2E8E6E); a.hemd = FARBE(0xF2C94E); a.hose = FARBE(0x1E5A4A); a.halstuch = FARBE(0xE8473D); return a; }
static Aussehen unterLook() {  // nur in Unterwaesche
  Aussehen a = kaiserLook(); a.weste = FARBE(0xF6F2EA); a.hemd = FARBE(0xF6F2EA); a.hose = FARBE(0xF6F2EA); a.halstuch = 0;
  return a;
}
static Aussehen betruegerLook(int i) {
  Aussehen a; a.weste = i ? FARBE(0x5A6E3A) : FARBE(0x6E4A3A); a.hemd = FARBE(0xE8DCC0); a.hose = FARBE(0x3A3436);
  a.haar = i ? FARBE(0xC98A3A) : FARBE(0x2E2220); a.hut = i ? FARBE(0x3A4A2A) : FARBE(0x4A2A2A); a.hutD = OUT; a.feder = FARBE(0xE0A23C);
  a.wangen = false;
  return a;
}
static Aussehen ministerLook() {
  Aussehen a; a.weste = FARBE(0x2E3A6E); a.hemd = WEISS; a.hose = FARBE(0x2E3A6E); a.haar = FARBE(0xE8E8E8); a.muetze = 1;
  a.halstuch = WEISS; a.wangen = false;
  return a;
}
static Aussehen vaterLook() { Aussehen a; a.weste = FARBE(0x8E5A3A); a.hose = FARBE(0x4A3B2A); a.haar = FARBE(0x5A3A26); a.muetze = 1; return a; }
static Aussehen kindLook() { Aussehen a; a.weste = FARBE(0x5B8FD7); a.hose = FARBE(0x4A4A5A); a.haar = FARBE(0xF2C14E); a.muetze = 1; return a; }
static Aussehen kammerLook() { Aussehen a; a.weste = FARBE(0xC0392B); a.hemd = GOLD; a.hose = WEISS; a.haar = WEISS; a.muetze = 1; return a; }
static const Aussehen KAISER = kaiserLook(), KAISER2 = kaiser2Look(), UNTER = unterLook(), B0 = betruegerLook(0),
                      B1 = betruegerLook(1), MINISTER = ministerLook(), VATER = vaterLook(), KIND = kindLook(), KAMMER = kammerLook();

// ---------- Dinge ----------
static void webstuhl(Gfx &g, float x, float y, float t, bool leer = true) {
  g.kontur(1.4f, OUT);
  g.rechteck(x - 40, y - 90, 6, 90, HOLZ_D);
  g.rechteck(x + 34, y - 90, 6, 90, HOLZ_D);
  g.rechteck(x - 44, y - 94, 88, 8, HOLZ);
  g.rechteck(x - 44, y - 40, 88, 8, HOLZ);
  g.konturAus();
  if (!leer) for (int i = 0; i < 10; i++) g.linie(x - 32 + i * 7, y - 86, x - 32 + i * 7, y - 40, 0.8f, GOLD);
  // das Weberschiffchen fliegt hin und her - durch nichts
  float sx = x + sinf(t * 3) * 30;
  g.ellipse(sx, y - 62, 6, 2.4f, HOLZ);
}

static void spiegel(Gfx &g, float x, float y) {
  g.kontur(1.6f, OUT);
  g.ellipse(x, y - 60, 24, 40, GOLD);
  g.konturAus();
  g.ellipse(x, y - 60, 19, 34, FARBE(0xC9E0F0));
  g.linie(x - 8, y - 82, x - 2, y - 88, 2, WEISS, 160);
  g.rechteck(x - 3, y - 20, 6, 20, HOLZ_D);
}

static void saal(Gfx &g, float t) {
  g.verlauf(0, 190, FARBE(0xF6E2B4), FARBE(0xE8C483));
  for (int i = 0; i < 4; i++) {
    float x = 20 + i * 82;
    g.rechteck(x - 10, 30, 20, 160, FARBE(0xFFF3D6));
    g.rechteck(x - 14, 26, 28, 8, FARBE(0xE8D3A2));
  }
  g.rechteck(0, 190, 284, 50, FARBE(0xC9A46A));
  for (int r = 0; r < 3; r++) for (int c = 0; c < 12; c++) if ((r + c) & 1) g.rechteck(c * 26 - r * 8, 190 + r * 17, 26, 17, FARBE(0xA8823E));
}

static void webstube(Gfx &g, float t) {
  g.verlauf(0, 200, FARBE(0xD9C8A8), FARBE(0xC0A880));
  g.rechteck(0, 200, 284, 40, FARBE(0x8E7656));
  g.kontur(1.6f, OUT);
  g.rundRechteck(220, 40, 50, 60, 4, HOLZ);
  g.konturAus();
  g.rundRechteck(225, 45, 40, 50, 2, FARBE(0x9FD8FF));
  // Truhe mit Seide und Gold (die Betrueger stecken alles ein)
  g.kontur(1.4f, OUT);
  g.rundRechteck(20, 176, 50, 26, 4, HOLZ);
  g.konturAus();
  g.rechteck(24, 172, 42, 6, GOLD);
  g.rechteck(30, 168, 30, 5, FARBE(0xE86FA8));
}

static void strasse(Gfx &g, float t, float scroll) {
  himmel(g, 0, t, 170);
  sonne(g, 248, 28, 11, t);
  for (int i = 0; i < 6; i++) {
    float x = fmodf(i * 70 - scroll + 4200, 420) - 70;
    g.kontur(1.4f, OUT);
    g.rechteck(x, 90 + (i % 3) * 8, 62, 80 - (i % 3) * 8, i & 1 ? FARBE(0xF4E4C4) : FARBE(0xF0D8C0));
    float d[] = {x - 6, 92 + (i % 3) * 8, x + 31, 60 + (i % 3) * 8, x + 68, 92 + (i % 3) * 8};
    g.polygon(d, 3, i % 3 == 0 ? FARBE(0xC0503A) : i % 3 == 1 ? FARBE(0x9A5A3A) : FARBE(0x5B7FC7));
    g.konturAus();
    for (int k = 0; k < 3; k++) g.rechteck(x + 6 + k * 20, 98 + (i % 3) * 8, 6, 8, FARBE(0xE8473D + k * 0x1100));  // Fahnen
  }
  g.rechteck(0, 170, 284, 70, FARBE(0xB8A890));
  g.rechteck(0, 196, 284, 3, FARBE(0xE8473D));  // roter Teppich
  g.rechteck(0, 199, 284, 20, FARBE(0xC0392B));
}

// =====================================================================
// Szenen
// =====================================================================
// 1) Der Kaiser liebt neue Kleider ueber alles
static void szeneEitel(Gfx &g, float u) {
  saal(g, u);
  spiegel(g, 200, 214);
  bool zwei = fmodf(u, 6) > 3;  // er zieht sich ein ums andere Mal um
  Pose k = posis(zwei ? KAISER2 : KAISER, u);
  k.armV = 1.6f + sinf(u * 2) * 0.3f;
  k.mund = 0.4f;
  float hop = fabsf(sinf(u * 2)) * 2;
  schneiderlein(g, 130, 214 - hop, 1, 1, k);
  if (fmodf(u, 3) < 0.4f) puff(g, 130, 150, fmodf(u, 3) / 0.4f);
  g.kameraAus();
  vers(g, {"Vor vielen Jahren lebte ein Kaiser,", "der hielt so ungeheuer viel auf", "hübsche neue Kleider."}, aufzu(u, 0.4f, 10));
  vers(g, {"Für jede Stunde des Tages", "hatte er einen anderen Rock."}, aufzu(u, 10.4f, 20));
  blende(g, u, 26, 140, 150);
}

// 2) Die zwei Betrueger: Stoff, den nur Kluge sehen koennen
static void szeneBetrueger(Gfx &g, float u) {
  saal(g, u);
  Pose k = posis(KAISER, u);
  k.mund = u > 18 && u < 22 ? 0.5f : 0.1f;
  k.armV = u > 18 ? 2.0f : 1.2f;
  schneiderlein(g, 210, 214, 1, -1, k);
  for (int i = 0; i < 2; i++) {
    Pose b = posis(i ? B1 : B0, u, i);
    b.mund = (u > 3 && u < 17 && ((int)(u / 3.5f) & 1) == i) ? 0.6f : 0.15f;
    b.armV = 1.8f + sinf(u * 3 + i) * 0.3f;
    b.boese = 0.3f;
    schneiderlein(g, 70 + i * 50, 214, 0.95f, 1, b);
  }
  if (u > 22) for (int i = 0; i < 6; i++) {  // der Kaiser gibt Gold
    float k2 = seg(u, 22 + i * 0.4f, 24 + i * 0.4f);
    g.kreis(lerp(196, 120, k2), lerp(170, 176, k2) - huepf(k2) * 20, 3, GOLD);
  }
  g.kameraAus();
  vers(g, {"Eines Tages kamen zwei Betrüger,", "die gaben sich für Weber aus."}, aufzu(u, 0.4f, 3.4f));
  blase(g, 100, 46, {"Wir weben den schönsten Stoff,", "den man sich denken kann!"}, aufzu(u, 3.6f, 8), 76, 140);
  blase(g, 120, 46, {"Wer für sein Amt nicht taugt", "oder unverzeihlich dumm ist,", "kann ihn nicht sehen!"}, aufzu(u, 8.2f, 15), 120, 140);
  blase(g, 200, 46, {"Das wären prächtige Kleider!", "Webt mir den Stoff!"}, aufzu(u, 18, 22.6f), 210, 120);
  vers(g, {"Sie bekamen viel Geld, feinste Seide", "und Gold - und steckten alles ein."}, aufzu(u, 23, 31.6f));
  blende(g, u, 32, 140, 150);
}

// 3) Der alte Minister sieht nichts - und lobt den Stoff
static void szeneMinister(Gfx &g, float u) {
  webstube(g, u);
  webstuhl(g, 150, 214, u);
  for (int i = 0; i < 2; i++) {
    Pose b = posis(i ? B1 : B0, u, i);
    b.armV = 1.6f + sinf(u * 5 + i * 2) * 0.4f;  // sie tun, als ob sie woben
    b.mund = i == 0 && u > 12 && u < 17 ? 0.6f : 0.2f;
    schneiderlein(g, i ? 206 : 96, 214, 0.9f, i ? -1 : 1, b);
  }
  Pose m = posis(MINISTER, u);
  m.mund = (u > 4 && u < 11) || (u > 17 && u < 24) ? 0.5f : 0;
  m.armV = u > 17 ? 1.8f : 0.8f;
  m.blick = 1;
  float mx = lerp(-30, 40, sanft(seg(u, 0, 3)));
  float hx, hy;
  schneiderlein(g, mx, 214, 0.95f, 1, m, &hx, &hy);
  // Brille
  g.bogen(mx + 2, 214 - 58, 4, 0, 6.29f, 1.2f, OUT);
  g.bogen(mx + 12, 214 - 58, 4, 0, 6.29f, 1.2f, OUT);
  if (u > 4 && u < 11) fragezeichen(g, mx + 10, 120, 0.9f, (uint8_t)(255 * aufzu(u, 4, 11)));
  g.kameraAus();
  vers(g, {"Der Kaiser schickte seinen alten,", "ehrlichen Minister zu den Webern."}, aufzu(u, 0.4f, 4));
  blase(g, 70, 50, {"Gott bewahre!", "Ich sehe ja nichts!"}, aufzu(u, 4.2f, 9), mx + 6, 150, FARBE(0xE8F0FF));
  vers(g, {"Aber das sagte er nicht."}, aufzu(u, 9.2f, 12));
  blase(g, 150, 50, {"Seht nur das Muster!", "Und diese Farben!"}, aufzu(u, 12.2f, 17), 96, 150);
  blase(g, 70, 50, {"Oh, es ist niedlich,", "ganz allerliebst!"}, aufzu(u, 17.2f, 24), mx + 6, 150);
  blende(g, u, 30, 150, 150);
}

// 4) Der Kaiser selbst sieht nichts - und laesst sich die neuen Kleider anziehen
static void szeneAnprobe(Gfx &g, float u) {
  if (u < 14) {
    webstube(g, u);
    webstuhl(g, 150, 214, u);
    for (int i = 0; i < 2; i++) {
      Pose b = posis(i ? B1 : B0, u, i);
      b.armV = 2.4f;  // halten den "Stoff" hoch
      b.mund = 0.4f;
      schneiderlein(g, i ? 206 : 96, 214, 0.9f, i ? -1 : 1, b);
    }
    Pose k = posis(KAISER, u);
    k.mund = u > 3 && u < 8 ? 0.4f : (u > 8 ? 0.6f : 0);
    k.armV = 1.4f;
    k.blick = 1;
    schneiderlein(g, 40, 214, 1, 1, k);
    if (u > 3 && u < 8) fragezeichen(g, 50, 120, 1, (uint8_t)(255 * aufzu(u, 3, 8)));
    g.kameraAus();
    blase(g, 60, 50, {"Was! Ich sehe gar nichts!", "Bin ich dumm?"}, aufzu(u, 3, 8), 50, 140, FARBE(0xE8F0FF));
    blase(g, 70, 50, {"Oh, es ist sehr hübsch!"}, aufzu(u, 8.2f, 13.6f), 50, 140);
    if (u > 13) g.rechteck(0, 0, 284, 240, 0, (uint8_t)(255 * seg(u, 13, 14)));
    return;
  }
  float v = u - 14;
  saal(g, u);
  spiegel(g, 236, 214);
  Pose k = posis(UNTER, u);
  k.armV = 1.6f + sinf(u * 2) * 0.3f;
  k.mund = 0.4f;
  schneiderlein(g, 150, 214, 1, 1, k);
  for (int i = 0; i < 2; i++) {
    Pose b = posis(i ? B1 : B0, u, i);
    b.armV = 1.8f + sinf(u * 6 + i * 2) * 0.4f;  // ziehen ihm das "Nichts" an
    b.mund = 0.4f;
    schneiderlein(g, i ? 196 : 104, 214, 0.9f, i ? -1 : 1, b);
  }
  for (int i = 0; i < 4; i++) g.stern(150 + sinf(u * 2 + i * 1.5f) * 30, 150 + cosf(u * 2 + i * 1.5f) * 30, 3, 1.2f, 4, u + i, GOLD, 180);
  g.kameraAus();
  if (v < 1) g.rechteck(0, 0, 284, 240, 0, (uint8_t)(255 * (1 - v)));
  blase(g, 110, 46, {"Hier sind die Beinkleider,", "hier ist der Rock, hier der Mantel!", "Es ist so leicht wie Spinnwebe!"}, aufzu(v, 1, 9), 104, 140);
  vers(g, {"Und der Kaiser drehte und wendete", "sich vor dem Spiegel."}, aufzu(v, 9.4f, 19.6f));
  blende(g, u, 34, 150, 150);
}

// 5) Der Festzug: "Aber er hat ja gar nichts an!"
static void szeneFestzug(Gfx &g, float u) {
  float scroll = u * 14;
  strasse(g, u, scroll);
  // die Leute am Rand jubeln
  for (int i = 0; i < 6; i++) {
    float x = fmodf(i * 52 - scroll * 0.2f + 1040, 312) - 14;
    if (i == 2) continue;
    Pose p = posis(i & 1 ? MINISTER : VATER, u, i);
    p.armV = u < 30 ? 2.4f + sinf(u * 6 + i) * 0.3f : 1.4f;
    p.mund = u > 34 ? 0.7f : 0.4f;
    schneiderlein(g, x, 188, 0.6f, 1, p);
  }
  // Vater mit dem Kind auf den Schultern
  Pose v = posis(VATER, u);
  v.mund = u > 26 && u < 32 ? 0.6f : 0.2f;
  v.armV = 2.6f;
  schneiderlein(g, 60, 190, 0.66f, 1, v);
  Pose kd = posis(KIND, u);
  kd.mund = u > 18 && u < 26 ? 0.8f : 0.3f;
  kd.armV = u > 18 && u < 26 ? 1.8f : 1.2f;
  schneiderlein(g, 62, 150, 0.45f, 1, kd);
  // der Kaiser im Zug, die Kammerherren tragen die Schleppe, die es gar nicht gibt
  Pose k = posis(UNTER, u);
  k.armV = 0.6f;
  k.mund = u > 36 && u < 42 ? 0.6f : 0.1f;
  k.boese = u > 34 ? 0.3f : 0;
  k.lauf = fmodf(u * 1.2f, 1);
  schneiderlein(g, 170, 222, 1, 1, k);
  for (int i = 0; i < 2; i++) {
    Pose kh = posis(KAMMER, u, i);
    kh.armV = 1.5f;  // halten mit spitzen Fingern - nichts
    kh.lauf = fmodf(u * 1.2f + i * 0.5f, 1);
    schneiderlein(g, 110 + i * 26, 222, 0.75f, 1, kh);
  }
#ifdef BOARD_RUND
  // rund: enger zusammen und in zwei Reihen, damit alle Rufe im Kreis liegen und sich nicht ueberdecken
  if (u > 34) for (int i = 0; i < 6; i++) g.text(SCHRIFT_K, i & 1 ? "hihi" : "nichts an!", 50 + i * 37, 152 - (i & 1) * 22, OUT, 1, (uint8_t)(255 * aufzu(u, 34, 46)));
#else
  if (u > 34) for (int i = 0; i < 6; i++) g.text(SCHRIFT_K, i & 1 ? "hihi" : "nichts an!", 30 + i * 44, 150 - (i & 1) * 12, OUT, 1, (uint8_t)(255 * aufzu(u, 34, 46)));
#endif
  g.kameraAus();
  vers(g, {"Der Kaiser ging in der Prozession,", "und alle Leute riefen:"}, aufzu(u, 0.4f, 5));
  blase(g, 150, 46, {"Wie prächtig sind des Kaisers", "neue Kleider!"}, aufzu(u, 5.2f, 12), 120, 160);
  blase(g, 80, 70, {"Aber er hat ja gar nichts an!"}, aufzu(u, 18, 26), 64, 128);
  blase(g, 110, 60, {"Herr Gott, hört die Stimme", "der Unschuld!"}, aufzu(u, 26.2f, 32), 64, 160);
  vers(g, {"Aber er hat ja gar nichts an!", "rief zuletzt das ganze Volk."}, aufzu(u, 34, 44));
  vers(g, {"Er hielt sich noch stolzer, und die", "Kammerherren trugen die Schleppe,", "die gar nicht da war."}, aufzu(u, 44.4f, 56));
  endeBild(g, u, 58);
  if (u < 1.2f) g.iris(142, 150, lerp(0, 330, sanft(u / 1.2f)));
}

static void bild(Gfx &g, float t) {
  if (t < P2) szeneEitel(g, t - P1);
  else if (t < P3) szeneBetrueger(g, t - P2);
  else if (t < P4) szeneMinister(g, t - P3);
  else if (t < P5) szeneAnprobe(g, t - P4);
  else szeneFestzug(g, t - P5);
}

// =====================================================================
// Musik
// =====================================================================
// der Kaisermarsch (D-Dur, etwas zu feierlich)
static const Ton3 MEL_KAISER[] = {
  {74, .75, 50}, {74, .25, 0}, {78, 1, 0}, {81, 1, 45}, {78, .5, 0}, {81, .5, 0}, {86, 2, 50},
  {83, .75, 43}, {81, .25, 0}, {79, 1, 0}, {78, 1, 45}, {76, .5, 0}, {78, .5, 0}, {74, 2, 50}};

static void ton(Synth &s, float t0, float t1) {
  float a = P1;
  spieleMelodie(s, M(MEL_KAISER), a + 0.5f, a + 25.5f, 0.32f, t0, t1, 0.8f);
  for (int i = 0; i < 8; i++) if (bei(a + 3 + i * 3, t0, t1)) for (int k = 0; k < 4; k++) s.glocke(a + 3 + i * 3 + k * 0.06f, 84 + k * 3, 0.3f);  // umziehen
  a = P2;
  spieleMelodie(s, M(MEL_SPUK), a + 0.4f, a + 17, 0.36f, t0, t1, 0.6f);
  spieleMelodie(s, M(MEL_KAISER), a + 18, a + 22.6f, 0.3f, t0, t1, 0.7f);
  for (int i = 0; i < 6; i++) if (bei(a + 22 + i * 0.4f, t0, t1)) s.glocke(a + 22 + i * 0.4f, 88 + (i % 3) * 2, 0.4f);  // Gold
  a = P3;
  for (float x = ceilf(t0 / 0.33f) * 0.33f; x < t1; x += 0.33f)
    if (x > a && x < a + 28) s.ton(x, 180, 0.08f, 0.002f, 0.06f, SINUS, GERAEUSCH, 120);  // klapp, klapp - der leere Webstuhl
  spieleMelodie(s, M(MEL_SPUK), a + 4, a + 12, 0.36f, t0, t1, 0.5f);
  spieleMelodie(s, M(MEL_F), a + 17, a + 24, 0.28f, t0, t1, 0.6f);
  a = P4;
  spieleMelodie(s, M(MEL_SPUK), a + 2, a + 8, 0.36f, t0, t1, 0.5f);
  spieleMelodie(s, M(MEL_KAISER), a + 8.2f, a + 33.5f, 0.3f, t0, t1, 0.75f);
  a = P5;
  spieleMelodie(s, M(MEL_KAISER), a + 0.4f, a + 17.6f, 0.3f, t0, t1, 0.85f);
  for (float x = ceilf(t0 / 0.6f) * 0.6f; x < t1; x += 0.6f)
    if (x > a && x < a + 17) s.ton(x, 120, 0.12f, 0.002f, 0.1f, SINUS, GERAEUSCH, 80);  // Trommel
  if (bei(a + 18, t0, t1)) for (int k = 0; k < 6; k++) s.note(a + 18 + k * 0.1f, 88 + (k & 1) * 4, 0.6f);  // das Kind!
  for (float x = ceilf(t0 / 0.12f) * 0.12f; x < t1; x += 0.12f)
    if (x > a + 34 && x < a + 44) s.note(x, 84 + ((int)(x * 9) % 5) * 2, 0.15f);  // Gekicher
  spieleMelodie(s, M(MEL_KAISER), a + 44.4f, a + 58, 0.4f, t0, t1, 0.7f);
  endeKlang(s, a + 60.2f, t0, t1);
}

// =====================================================================
// Startbild, Vorschau, Erkennungszeichen
// =====================================================================
static void titelSzene(Gfx &g, float t) {
  strasse(g, t, 0);
  Pose k = posis(UNTER, t);
  k.armV = 1.2f;
  k.mund = 0.3f;
  schneiderlein(g, 160, 226, 1.1f, 1, k);
  for (int i = 0; i < 2; i++) {
    Pose kh = posis(KAMMER, t, i);
    kh.armV = 1.5f;
    schneiderlein(g, 96 + i * 28, 226, 0.8f, 1, kh);
  }
}
static void titelbild(Gfx &g, float t) { titelSzene(g, t); titelRahmen(g, t, "Des Kaisers neue Kleider", 210, SCHRIFT_B); }
static void vorschau(Gfx &g, float t) { titelSzene(g, t); }
static void symbol(Gfx &g, float x, float y, float t) {
  // die Krone ueber einem leeren Kleiderbuegel
  float b = sinf(t * 2) * 3;
  g.kontur(1.4f, OUT);
  float k[] = {x - 22, y - 62 + b, x - 22, y - 84 + b, x - 11, y - 72 + b, x, y - 90 + b, x + 11, y - 72 + b, x + 22, y - 84 + b, x + 22, y - 62 + b};
  g.polygon(k, 7, GOLD);
  g.konturAus();
  g.kreis(x, y - 72 + b, 3, FARBE(0xE8473D));
  g.bogen(x, y - 40, 4, 3.4f, 6.6f, 2, FARBE(0x8E94A6));
  g.linie(x - 34, y - 22, x, y - 36, 2.4f, HOLZ);
  g.linie(x + 34, y - 22, x, y - 36, 2.4f, HOLZ);
  g.linie(x - 34, y - 22, x + 34, y - 22, 2.4f, HOLZ);
  for (int i = 0; i < 4; i++) g.stern(x - 24 + i * 16, y - 10 - (i & 1) * 6, 3, 1.2f, 4, t + i, WEISS, (uint8_t)(150 + 100 * sinf(t * 3 + i)));
}

const Maerchen KAISER_KLEIDER = {"Des Kaisers neue Kleider", PDAUER, FARBE(0x7A2A8E), bild, ton, titelbild, vorschau, symbol,
                                 LIED(MEL_KAISER, 0.32f), "H. C. Andersen"};

// Haensel und Gretel - ein Spieluhr-Film fuer die Maerchenbox, nach dem Maerchen der Brueder Grimm.
// Die Kieselsteine im Mondschein, die Brotkrumen, der weisse Vogel, das Haeuschen aus Brot und Kuchen,
// "Knusper, knusper, Kneischen", das Knoechlein, der Ofen (nur angedeutet), das Entchen und die Maus am Ende.
#include "erzaehlen.h"

// ---------- Ablauf (Sekunden) ----------
static const float P1 = 0, P2 = 30, P3 = 62, P4 = 86, P5 = 122, P6 = 152, P7 = 190, P8 = 226, PDAUER = 262;

// ---------- Aussehen ----------
static Aussehen haenselLook() {
  Aussehen a; a.weste = FARBE(0x5A7A3A); a.hemd = FARBE(0xF2E8D0); a.hose = FARBE(0x6E5A3A); a.haar = FARBE(0xC98A3A);
  a.muetze = 1;
  return a;
}
static Aussehen vaterLook() {
  Aussehen a; a.weste = FARBE(0x8E4E2E); a.hemd = FARBE(0xE8D8B8); a.hose = FARBE(0x4A3B2A); a.haar = FARBE(0x5A3A26);
  a.muetze = 1;
  return a;
}
static const Aussehen HAENSEL = haenselLook(), VATER = vaterLook();
static FrauLook gretelLook() {
  FrauLook l; l.kleid = FARBE(0xC84A5A); l.schuerze = FARBE(0xF6F0E4); l.haar = FARBE(0xF2C94E); l.frisur = 1;
  return l;
}
static FrauLook stiefLook() {
  FrauLook l; l.kleid = FARBE(0x5A4A6E); l.haar = FARBE(0x3A2A20); l.frisur = 2; l.wangen = false;
  return l;
}
static FrauLook hexeLook() {
  FrauLook l; l.kleid = FARBE(0x3E3A4E); l.schuerze = FARBE(0x5E5A3E); l.haar = FARBE(0xBDBDBD); l.frisur = 3;
  l.tuch = FARBE(0x4E3A5A); l.wangen = false; l.haut = FARBE(0xD8D8B0);
  return l;
}
static const FrauLook GRETEL = gretelLook(), STIEF = stiefLook(), HEXE = hexeLook();

// ---------- Figuren ----------
static void haensel(Gfx &g, float x, float y, float dir, const Pose &p, float *hx = nullptr, float *hy = nullptr) {
  schneiderlein(g, x, y, 0.72f, dir, p, hx, hy);
}
static void gretel(Gfx &g, float x, float y, float dir, const FrauPose &p, float *hx = nullptr, float *hy = nullptr) {
  frau(g, x, y, 0.7f, dir, p, hx, hy);
}
// die Hexe: gebueckt, mit langer Nase und Kruecke
static void hexe(Gfx &g, float x, float y, float s, float dir, float t, float mund, float armV, float boese = 1) {
  FrauPose p = fposis(HEXE, t, 3);
  p.mund = mund;
  p.armV = armV;
  p.boese = boese;
  float hx, hy;
  frau(g, x, y, s, dir, p, &hx, &hy);
  // Nase und Warze
  g.kontur(1, OUT);
  g.ellipse(x + 10 * s * dir, y - 70 * s, 5 * s, 3.4f * s, HEXE.haut, 255, 0.5f * dir);
  g.konturAus();
  g.kreis(x + 12 * s * dir, y - 72 * s, 1.2f * s, FARBE(0x6E5A3A));
  // Kruecke in der hinteren Hand
  g.linie(x - 14 * s * dir, y - 40 * s, x - 18 * s * dir, y, 2.4f * s, HOLZ_D);
  g.bogen(x - 12 * s * dir, y - 42 * s, 4 * s, 3.2f, 6.2f, 2.4f * s, HOLZ_D);
}

static void kiesel(Gfx &g, float x, float y, float glanz) {
  g.ellipse(x, y, 3, 2, FARBE(0xF2F2F6));
  if (glanz > 0) {
    g.kreis(x, y, 6, FARBE(0xE8F0FF), (uint8_t)(70 * glanz));
    g.stern(x, y - 1, 3, 1, 4, 0, WEISS, (uint8_t)(200 * glanz));
  }
}

static void weisserVogel(Gfx &g, float x, float y, float t, bool fliegt) {
  float f = fliegt ? sinf(t * 14) : 0.4f;
  g.ellipse(x, y, 7, 4, WEISS);
  g.kreis(x + 6, y - 3, 3.4f, WEISS);
  float sn[] = {x + 9, y - 3.5f, x + 13, y - 2.5f, x + 9, y - 1.5f};
  g.polygon(sn, 3, FARBE(0xF2B230));
  g.kreis(x + 7, y - 4, 0.8f, SCHWARZ);
  float fl[] = {x - 2, y - 2, x - 10, y - 10 - f * 6, x + 4, y - 3};
  g.polygon(fl, 3, FARBE(0xF2F2F6));
  if (!fliegt) {  // singt
    for (int i = 0; i < 2; i++) {
      float ph = fmodf(t * 0.5f + i * 0.5f, 1);
      notenzeichen(g, x + 12 + ph * 16, y - 8 - ph * 20, FARBE(0xB28DFF), (uint8_t)(230 * huepf(ph)));
    }
  }
}

static void ente(Gfx &g, float x, float y, float s, float t) {
  float w = sinf(t * 2) * 1.5f;
  g.kontur(1.1f, OUT);
  g.ellipse(x, y - 8 + w, 22 * s, 10 * s, WEISS);
  g.kreis(x + 18 * s, y - 22 * s + w, 7 * s, WEISS);
  g.konturAus();
  float sn[] = {x + 24 * s, y - 22 * s + w, x + 32 * s, y - 20 * s + w, x + 24 * s, y - 18 * s + w};
  g.polygon(sn, 3, FARBE(0xF28A2A));
  g.kreis(x + 20 * s, y - 24 * s + w, 1.2f * s, SCHWARZ);
}

// ---------- Kulissen ----------
static void huette(Gfx &g, float t, float nacht) {
  g.verlauf(0, 200, mischeFarbe(FARBE(0xC9B08A), FARBE(0x3A3050), nacht), mischeFarbe(FARBE(0xA88E6A), FARBE(0x2A2240), nacht));
  g.rechteck(0, 200, 284, 40, FARBE(0x6E5A46));
  g.kontur(1.6f, OUT);
  g.rundRechteck(190, 50, 60, 54, 4, HOLZ);
  g.konturAus();
  g.rundRechteck(195, 55, 50, 44, 2, mischeFarbe(FARBE(0x9FD8FF), FARBE(0x22285A), nacht));
  if (nacht > 0.5f) g.kreis(226, 70, 8, FARBE(0xFFF1B0));
  g.rechteck(218, 55, 3, 44, HOLZ);
  // Kinderbett
  g.kontur(1.4f, OUT);
  g.rundRechteck(20, 176, 90, 26, 4, HOLZ);
  g.konturAus();
  g.rundRechteck(24, 166, 26, 14, 6, WEISS);
  g.rundRechteck(40, 168, 66, 16, 6, FARBE(0x8E9A6A));
  // Herd mit wenig Feuer
  g.rundRechteck(130, 150, 46, 52, 4, FARBE(0x7A6A5A));
  g.ellipse(153, 194, 10, 3 + sinf(t * 7) * 1.2f, FARBE(0xFF8A2A));
}

static void wald(Gfx &g, float t, float nacht, float scroll = 0) {
  himmel(g, nacht, t, 200);
  if (nacht > 0.5f) {
    mond(g, 240, 34, 12);
    for (int i = 0; i < 18; i++) g.stern(zufall(i + 700) * 284, zufall(i + 720) * 100, 2, 0.8f, 4, 0, WEISS, 180);
  } else sonne(g, 248, 30, 11, t);
  Farbe tan = mischeFarbe(FARBE(0x2F8F4E), FARBE(0x1F3B45), nacht);
  for (int i = 0; i < 7; i++) tanne(g, fmodf(i * 48 - scroll * 0.6f + 1000, 336) - 26, 200, 0.85f + (i % 3) * 0.15f, tan);
  boden(g, 200, mischeFarbe(GRAS, FARBE(0x2F4A40), nacht));
  g.rechteck(0, 206, 284, 14, mischeFarbe(FARBE(0xC9B48A), FARBE(0x4A4A50), nacht));  // Weg
}

static void feuer(Gfx &g, float x, float y, float t, float gross = 1) {
  for (int i = 0; i < 3; i++) g.linie(x - 12 + i * 8, y, x - 6 + i * 6, y - 6, 3, HOLZ_D);
  for (int i = 0; i < 3; i++) {
    float h = (14 + 6 * sinf(t * 9 + i * 1.7f)) * gross;
    float f[] = {x - 8 + i * 8 - 5, y - 2, x - 8 + i * 8 + 5, y - 2, x - 8 + i * 8 + sinf(t * 7 + i) * 2, y - 2 - h};
    g.polygon(f, 3, i == 1 ? FARBE(0xFFD54A) : FARBE(0xFF8A2A));
  }
  g.kreis(x, y - 10, 30 * gross, FARBE(0xFFB050), 40);
}

// das Haeuschen aus Brot, mit Kuchen gedeckt, die Fenster aus hellem Zucker
static void kuchenhaus(Gfx &g, float x, float y, float t, float knabber) {
  const Farbe lebkuchen = FARBE(0xB8702E), guss = FARBE(0xFFF8F0);
  g.kontur(1.8f, OUT);
  g.rechteck(x - 50, y - 70, 100, 70, lebkuchen);
  float d[] = {x - 62, y - 68, x, y - 118, x + 62, y - 68};
  g.polygon(d, 3, FARBE(0xE8A0B8));  // Kuchendach mit rosa Guss
  g.konturAus();
  // Zuckerguss-Wellen an der Dachkante
  for (int i = 0; i < 9; i++) g.kreis(x - 56 + i * 14, y - 68, 6, guss);
  for (int i = 0; i < 7; i++) {  // Bonbons auf dem Dach
    float bx = x - 36 + i * 12, by = y - 82 - (3 - fabsf(i - 3)) * 8;
    g.kreis(bx, by, 4, i % 3 == 0 ? FARBE(0xE8473D) : i % 3 == 1 ? FARBE(0x5BB8E8) : GOLD);
  }
  // Fenster aus hellem Zucker
  for (int i = 0; i < 2; i++) {
    float fx = x - 36 + i * 52;
    g.kontur(1.2f, OUT);
    g.rundRechteck(fx, y - 56, 20, 20, 3, FARBE(0xDFF4FF));
    g.konturAus();
    g.rechteck(fx + 9, y - 56, 2, 20, guss);
    g.rechteck(fx, y - 47, 20, 2, guss);
    g.stern(fx + 5, y - 51, 2.5f, 1, 4, t, WEISS, 200);
  }
  // Tuer aus Schokolade
  g.rundRechteck(x - 10, y - 38, 20, 38, 9, FARBE(0x5A3420));
  g.kreis(x + 5, y - 18, 2, GOLD);
  // Lebkuchen-Herzen und Punkte
  for (int i = 0; i < 6; i++) g.kreis(x - 44 + i * 18, y - 8, 2.4f, guss);
  herz(g, x - 30, y - 22, 5, FARBE(0xE8473D), 255);
  herz(g, x + 30, y - 22, 5, FARBE(0xE8473D), 255);
  if (knabber > 0) g.kreis(x - 44, y - 34, 6 * knabber, FARBE(0x8E5A3A));  // abgebissene Ecke
  // Zaun aus Zuckerstangen
  for (int i = 0; i < 5; i++) {
    float zx = x + 70 + i * 12;
    g.linie(zx, y, zx, y - 26, 3, WEISS);
    for (int k = 0; k < 4; k++) g.linie(zx - 1.5f, y - 4 - k * 6, zx + 1.5f, y - 7 - k * 6, 1.4f, FARBE(0xE8473D));
  }
}

static void stall(Gfx &g, float x, float y) {
  g.kontur(1.4f, OUT);
  g.rechteck(x - 30, y - 60, 60, 4, HOLZ_D);
  g.konturAus();
  for (int i = 0; i < 6; i++) g.rechteck(x - 28 + i * 11, y - 58, 3, 58, HOLZ_D);
  g.rechteck(x - 30, y - 2, 60, 4, HOLZ_D);
}

static void hexenOfen(Gfx &g, float x, float y, float t, float tuer, float feuer) {
  g.kontur(1.8f, OUT);
  g.rundRechteck(x - 50, y - 90, 100, 90, 24, FARBE(0x8E7A6A));
  g.konturAus();
  g.rundRechteck(x - 30, y - 60, 60, 44, 18, FARBE(0x2E2220));
  for (int i = 0; i < 3; i++) {
    float h = (10 + 6 * sinf(t * 9 + i)) * feuer;
    float f[] = {x - 16 + i * 16 - 6, y - 18, x - 16 + i * 16 + 6, y - 18, x - 16 + i * 16, y - 18 - h};
    g.polygon(f, 3, FARBE(0xFF8A2A));
  }
  if (tuer > 0) {  // die eiserne Tuer
    g.kontur(1.4f, OUT);
    g.rundRechteck(x - 30, y - 60, 60 * tuer, 44, 18, FARBE(0x4E4A54));
    g.konturAus();
    if (tuer > 0.9f) g.rechteck(x + 22, y - 42, 12, 4, FARBE(0x8E94A6));  // Riegel
  }
  g.rechteck(x + 30, y - 120, 16, 34, FARBE(0x7A6A5A));
}

static void hexenstube(Gfx &g, float t) {
  g.verlauf(0, 200, FARBE(0x6E5A4E), FARBE(0x4E3E36));
  g.rechteck(0, 200, 284, 40, FARBE(0x3E3028));
  // Regal mit Glaesern
  g.rechteck(20, 60, 90, 4, HOLZ_D);
  for (int i = 0; i < 4; i++) {
    g.kontur(1, OUT);
    g.rundRechteck(26 + i * 22, 42, 14, 18, 4, i & 1 ? FARBE(0x7AB86A) : FARBE(0xB28DFF));
    g.konturAus();
  }
  // Spinnweben in der Ecke
  for (int i = 0; i < 4; i++) g.linie(0, 0, 30 - i * 6, i * 9 + 4, 0.6f, FARBE(0xC9C9C9), 160);
  g.bogen(0, 0, 18, 0, 1.57f, 0.6f, FARBE(0xC9C9C9), 160);
}

static void wasser(Gfx &g, float t) {
  himmel(g, 0, t, 186);
  sonne(g, 248, 30, 11, t);
  for (int i = 0; i < 4; i++) tanne(g, i * 70 + 20, 186, 0.7f, FARBE(0x2F8F4E));
  g.verlauf(186, 240, FARBE(0x6EB8E8), FARBE(0x3E88C8));
  for (int i = 0; i < 9; i++) {
    float x = fmodf(i * 37 + t * 14, 300) - 10;
    g.linie(x, 196 + (i % 3) * 12, x + 14, 196 + (i % 3) * 12, 1.4f, FARBE(0xBFE6FF), 160);
  }
  g.ellipse(20, 196, 40, 10, GRAS_D);
  g.ellipse(264, 196, 40, 10, GRAS_D);
}

// =====================================================================
// Szenen
// =====================================================================
// 1) In der Nacht: die Eltern beraten, Haensel sammelt Kieselsteine im Mondschein
static void szeneNacht(Gfx &g, float u) {
  if (u < 16) {
    huette(g, u, 0.8f);
    g.kreis(38, 166, 7, HAUT);
    g.ellipse(36, 161, 8, 4, HAENSEL.haar);
    g.kreis(54, 168, 6.5f, HAUT);
    g.ellipse(52, 164, 7, 4, GRETEL.haar);
    g.kreis(41, 166, 1, OUT);
    g.kreis(57, 168, 1, OUT);  // sie sind wach und hoeren alles
    FrauPose s = fposis(STIEF, u);
    s.boese = 0.8f;
    s.mund = u > 3 && u < 10 ? 0.5f : 0;
    s.armV = 1.4f;
    frau(g, 160, 214, 0.95f, 1, s);
    Pose v = posis(VATER, u);
    v.mund = u > 10.2f && u < 14 ? 0.4f : 0;
    v.zu = true;
    schneiderlein(g, 214, 214, 1, -1, v);
    g.kameraAus();
    vers(g, {"Vor einem großen Walde wohnte", "ein armer Holzhacker mit", "seinen zwei Kindern."}, aufzu(u, 0.2f, 2.9f));
    blase(g, 130, 46, {"Wir wollen die Kinder morgen", "früh in den Wald führen,", "sie finden den Weg nicht heim."}, aufzu(u, 3, 10), 160, 140);
    blase(g, 220, 60, {"Ach, die armen Kinder ..."}, aufzu(u, 10.2f, 14), 214, 140);
    if (u > 15) g.rechteck(0, 0, 284, 240, 0, (uint8_t)(255 * seg(u, 15, 16)));
    return;
  }
  float v = u - 16;
  // draussen im Mondschein: die weissen Kieselsteine glaenzen wie Batzen
  wald(g, u, 1);
  g.kontur(1.6f, OUT);
  g.rechteck(10, 130, 70, 70, FARBE(0x8E7A6A));
  float d[] = {2, 132, 45, 98, 88, 132};
  g.polygon(d, 3, FARBE(0x4E3A3A));
  g.konturAus();
  for (int i = 0; i < 12; i++) kiesel(g, 100 + zufall(i + 50) * 170, 210 + zufall(i + 70) * 20, 0.6f + 0.4f * sinf(u * 2 + i));
  Pose h = posis(HAENSEL, u);
  float hx = lerp(70, 210, seg(v, 0, 11));
  h.lauf = fmodf(v, 2) < 1 ? fmodf(u * 1.4f, 1) : -1;
  h.armV = fmodf(v, 2) < 1 ? 0.6f : 0.2f;
  haensel(g, hx, 214, 1, h);
  g.kameraAus();
  if (v < 1) g.rechteck(0, 0, 284, 240, 0, (uint8_t)(255 * (1 - v)));
  vers(g, {"Der Mond schien ganz hell, und", "die weißen Kieselsteine", "glänzten wie lauter Batzen."}, aufzu(v, 1, 9.6f));
  blende(g, u, 30, 140, 150);
}

// 2) Im Wald: die Kieselsteine weisen den Weg nach Hause
static void szeneKiesel(Gfx &g, float u) {
  if (u < 18) {
    float scroll = u * 30;
    wald(g, u, 0, scroll);
    Pose v = posis(VATER, u);
    v.lauf = fmodf(u * 1.4f, 1);
    v.armV = 1.2f;
    float hx, hy;
    schneiderlein(g, 200, 214, 1, 1, v, &hx, &hy);
    g.linie(hx, hy, hx + 10, hy - 22, 2.5f, HOLZ_D);  // Axt
    FrauPose s = fposis(STIEF, u);
    s.lauf = fmodf(u * 1.4f + 0.5f, 1);
    frau(g, 160, 214, 0.95f, 1, s);
    FrauPose gr = fposis(GRETEL, u, 1);
    gr.lauf = fmodf(u * 1.6f, 1);
    gretel(g, 110, 214, 1, gr);
    Pose h = posis(HAENSEL, u);
    h.lauf = fmodf(u * 1.6f + 0.3f, 1);
    h.armV = fmodf(u, 2) < 0.6f ? 1.0f : 0.4f;
    float hhx, hhy;
    haensel(g, 70, 214, -1, h, &hhx, &hhy);
    for (int i = 0; i < 10; i++) {  // die Steinchen, die er hinter sich fallen laesst
      float sx = 60 - i * 40 + fmodf(scroll, 40);
      kiesel(g, sx, 214, 0);
    }
    g.kameraAus();
    vers(g, {"Hänsel blieb stehen und guckte nach", "dem Häuschen zurück ..."}, aufzu(u, 1, 8));
    vers(g, {"... und warf jedesmal einen", "Kieselstein auf den Weg."}, aufzu(u, 8.2f, 15));
    if (u > 17) g.rechteck(0, 0, 284, 240, 0, (uint8_t)(255 * seg(u, 17, 18)));
    return;
  }
  float v = u - 18;
  wald(g, u, 1);
  feuer(g, 80, 214, u, 1 - seg(v, 0, 8) * 0.7f);
  for (int i = 0; i < 8; i++) kiesel(g, 130 + i * 22, 214 + (i & 1) * 4, 0.7f + 0.3f * sinf(u * 2 + i));
  FrauPose gr = fposis(GRETEL, u, 1);
  Pose h = posis(HAENSEL, u);
  float k = seg(v, 6, 13);
  gr.weint = v < 5;
  gr.lauf = k > 0 && k < 1 ? fmodf(u * 1.5f, 1) : -1;
  h.lauf = gr.lauf;
  h.mund = v > 2 && v < 6 ? 0.5f : 0.2f;
  h.armV = v > 2 && v < 6 ? 1.6f : 0.6f;
  gretel(g, lerp(104, 300, k), 214, 1, gr);
  haensel(g, lerp(126, 330, k), 214, 1, h);
  g.kameraAus();
  if (v < 1) g.rechteck(0, 0, 284, 240, 0, (uint8_t)(255 * (1 - v)));
  blase(g, 150, 70, {"Wart nur, bis der Mond aufgeht,", "dann wollen wir den Weg finden."}, aufzu(v, 1.6f, 6), 130, 150);
  blende(g, u, 32, 140, 150);
}

// 3) Das zweite Mal: Brotkrumen - die Voegel picken sie auf
static void szeneKrumen(Gfx &g, float u) {
  wald(g, u, seg(u, 8, 12) * 0.9f);
  for (int i = 0; i < 9; i++) {
    float bx = 30 + i * 28;
    bool weg = u > 4 + i * 0.6f;
    if (!weg) g.kreis(bx, 214, 2, FARBE(0xE8C080));
  }
  for (int i = 0; i < 4; i++) {
    float k = seg(u, 3 + i * 0.8f, 9 + i * 0.8f);
    float vx = lerp(-20 + i * 14, 300, k), vy = 200 - huepf(fmodf(k * 6, 1)) * 10 - (k > 0.9f ? (k - 0.9f) * 400 : 0);
    if (k > 0 && k < 1) vogel(g, vx, vy, 0.8f, fmodf(u * 3 + i * 0.3f, 1), 1);
  }
  FrauPose gr = fposis(GRETEL, u, 1);
  Pose h = posis(HAENSEL, u);
  gr.weint = u > 12;
  h.armV = u > 12 ? 2.0f : 0.6f;
  h.mund = u > 12 ? 0.5f : 0.1f;
  gretel(g, 110, 214, 1, gr);
  haensel(g, 140, 214, -1, h);
  if (u > 12) fragezeichen(g, 140, 130, 1, (uint8_t)(255 * aufzu(u, 12, 22)));
  g.kameraAus();
  vers(g, {"Diesmal streute Hänsel Bröcklein", "Brot auf den Weg."}, aufzu(u, 0.4f, 5));
  vers(g, {"Aber die vieltausend Vögel", "hatten sie weggepickt."}, aufzu(u, 6, 12));
  vers(g, {"Sie gingen die ganze Nacht", "und noch einen Tag und", "fanden nicht aus dem Wald."}, aufzu(u, 13, 22));
  blende(g, u, 24, 140, 150);
}

// 4) Der weisse Vogel fuehrt sie zum Haeuschen; "Knusper, knusper, Kneischen"
static void szeneHaeuschen(Gfx &g, float u) {
  wald(g, u, 0);
  float hausDa = seg(u, 4, 6);
  if (hausDa > 0) kuchenhaus(g, 196, 214, u, seg(u, 12, 14));
  if (u < 6) weisserVogel(g, lerp(60, 200, seg(u, 0, 5)), 120 - sinf(u * 2) * 10, u, true);
  else weisserVogel(g, 208, 98, u, false);  // er setzt sich aufs Dach
  FrauPose gr = fposis(GRETEL, u, 1);
  Pose h = posis(HAENSEL, u);
  float k = seg(u, 1, 7);
  gr.lauf = k < 1 ? fmodf(u * 1.5f, 1) : -1;
  h.lauf = gr.lauf;
  if (u > 9) { h.armV = 1.8f; gr.armV = 1.8f; h.mund = 0.6f * fabsf(sinf(u * 6)); gr.mund = 0.6f * fabsf(sinf(u * 6 + 1)); }
  if (u > 26) { h.mund = 0.6f; gr.mund = 0.6f; }
  gretel(g, lerp(-20, 100, k), 214, 1, gr);
  float hhx, hhy;
  haensel(g, lerp(-40, 136, k), 214, 1, h, &hhx, &hhy);
  if (u > 10) g.rundRechteck(hhx - 4, hhy - 6, 10, 8, 2, FARBE(0xB8702E));  // ein Stueck Dach
  g.kameraAus();
  vers(g, {"Da sahen sie ein schönes schneeweißes", "Vöglein, das sang so schön."}, aufzu(u, 0.4f, 5));
  vers(g, {"Das Häuschen war aus Brot gebaut,", "mit Kuchen gedeckt, die Fenster", "waren von hellem Zucker."}, aufzu(u, 5.4f, 12));
  blase(g, 200, 40, {"Knusper, knusper, Kneischen,", "wer knuspert an meinem Häuschen?"}, aufzu(u, 16, 22.6f), 196, 172);
  blase(g, 90, 70, {"Der Wind, der Wind,", "das himmlische Kind!"}, aufzu(u, 23, 30), 116, 150);
  blende(g, u, 36, 160, 150);
}

// 5) Die Hexe; Haensel im Stall; "streck deinen Finger heraus" - das Knoechlein
static void szeneStall(Gfx &g, float u) {
  if (u < 10) {
    wald(g, u, 0);
    kuchenhaus(g, 196, 214, u, 1);
    float hk = seg(u, 0.5f, 3);
    hexe(g, lerp(196, 166, hk), 214, 1, -1, u, u > 3 && u < 9 ? 0.5f : 0, 1.4f, 0.2f);  // freundlich tut sie
    FrauPose gr = fposis(GRETEL, u, 1);
    gr.armV = 2.4f; gr.mund = 0.7f;
    gretel(g, 90, 214, 1, gr);
    Pose h = posis(HAENSEL, u);
    h.armV = 2.4f; h.mund = 0.7f;
    haensel(g, 118, 214, 1, h);
    g.kameraAus();
    blase(g, 180, 40, {"Ei, ihr lieben Kinder,", "wer hat euch hierher gebracht?", "Kommt nur herein!"}, aufzu(u, 3, 9.6f), 170, 140);
    return;
  }
  float v = u - 10;
  hexenstube(g, u);
  stall(g, 70, 214);
  Pose h = posis(HAENSEL, u);
  h.armV = v > 8 && v < 16 ? 1.6f : 0.8f;
  h.mund = v > 16 ? 0.4f : 0.1f;
  float hx, hy;
  g.ausschnitt(40, 154, 62, 60);
  haensel(g, 70, 214, 1, h, &hx, &hy);
  g.ausschnittAus();
  if (v > 8 && v < 16) {  // das Knoechlein durchs Gitter
    g.linie(hx, hy, hx + 14, hy - 2, 3, FARBE(0xF6EED6));
    g.kreis(hx + 15, hy - 2, 2.4f, FARBE(0xF6EED6));
  }
  hexe(g, 150, 214, 1, -1, u, v > 4 && v < 8 ? 0.6f : (v > 16 ? 0.4f : 0), v > 8 && v < 16 ? 1.7f : 1.2f);
  FrauPose gr = fposis(GRETEL, u, 1);
  gr.weint = true;
  gr.armV = 1.4f;
  gretel(g, 236, 214, -1, gr);
  g.kameraAus();
  if (v < 1) g.rechteck(0, 0, 284, 240, 0, (uint8_t)(255 * (1 - v)));
  vers(g, {"Die Alte war aber eine böse Hexe.", "Hänsel sperrte sie in einen Stall."}, aufzu(v, 0.4f, 4));
  blase(g, 160, 50, {"Hänsel, streck deine Finger", "heraus, damit ich fühle,", "ob du bald fett bist!"}, aufzu(v, 4.2f, 8.4f), 156, 140);
  vers(g, {"Hänsel streckte ihr aber ein Knöchlein", "heraus. Die Alte hatte trübe Augen ..."}, aufzu(v, 8.6f, 15.6f));
  vers(g, {"... und meinte, es wären Hänsels", "Finger, und verwunderte sich."}, aufzu(v, 15.8f, 20));
  blende(g, u, 30, 140, 150);
}

// 6) Der Ofen: "Dumme Gans!" - Gretel gibt ihr einen Stoss, die eiserne Tuer faellt zu (nur angedeutet)
static void szeneOfen(Gfx &g, float u) {
  hexenstube(g, u);
  float tuer = seg(u, 22.4f, 23.2f);
  hexenOfen(g, 190, 214, u, tuer, 1);
  FrauPose gr = fposis(GRETEL, u, 1);
  gr.mund = (u > 6.2f && u < 11) ? 0.5f : 0.1f;
  gr.armV = u > 20 && u < 23 ? 1.8f : 1.0f;
  float gx = u > 20 ? lerp(110, 140, seg(u, 20, 21.5f)) : 110;
  gretel(g, gx, 214, 1, gr);
  if (u < 22) {
    float hk = u > 15 ? seg(u, 16, 21) : 0;
    float hx = lerp(150, 182, hk);
    if (u > 21) g.ausschnitt(0, 0, 284, 160);
    hexe(g, hx, 214 - hk * 30, 1 - hk * 0.4f, 1, u, u > 1 && u < 6 ? 0.5f : u > 11 && u < 16 ? 0.7f : 0, u > 14 ? 2.6f : 1.4f);
    g.ausschnittAus();
  }
  if (u > 22 && u < 25) wolkePuff(g, 190, 170, seg(u, 22, 25), FARBE(0xE8E4EE), 1.4f);
  if (u > 22.8f && u < 23.3f) g.wackeln(sinf(u * 90) * 4, 3);
  g.kameraAus();
  blase(g, 190, 46, {"Kriech hinein und sieh zu,", "ob recht eingeheizt ist!"}, aufzu(u, 1, 6), 166, 140);
  blase(g, 90, 60, {"Ich weiß nicht, wie ich's", "machen soll. Wie komm ich", "da hinein?"}, aufzu(u, 6.2f, 11), 110, 150);
  blase(g, 190, 46, {"Dumme Gans! Die Öffnung ist", "groß genug, siehst du wohl,", "ich könnte selbst hinein."}, aufzu(u, 11.2f, 16.6f), 166, 140);
  vers(g, {"Da gab ihr Gretel einen Stoß,", "machte die eiserne Tür zu", "und schob den Riegel vor."}, aufzu(u, 22.4f, 30));
  if (u > 30) g.rechteck(0, 0, 284, 240, 0, (uint8_t)(255 * seg(u, 30, 32)));
  if (u < 1.2f) g.iris(140, 150, lerp(0, 330, sanft(u / 1.2f)));
  if (u > 32) {
    g.text(SCHRIFT_M, "Die böse Hexe war fort.", 142, 126, WEISS, 1, (uint8_t)(255 * aufzu(u, 32, 38)));
  }
}

// 7) Erloest! Perlen und Edelsteine; das Entchen traegt sie uebers Wasser
static void szeneEntchen(Gfx &g, float u) {
  if (u < 14) {
    hexenstube(g, u);
    stall(g, 70, 214);
    float auf = seg(u, 1, 3);
    Pose h = posis(HAENSEL, u);
    h.armV = auf > 0.5f ? 2.6f : 1.2f;
    h.mund = 0.6f;
    float hx = lerp(70, 130, auf);
    haensel(g, hx, 214 - huepf(seg(u, 2, 4)) * 20, 1, h);
    FrauPose gr = fposis(GRETEL, u, 1);
    gr.armV = 2.4f; gr.mund = 0.6f;
    gretel(g, 170, 214 - huepf(fmodf(u * 1.2f, 1)) * 8, -1, gr);
    for (int i = 0; i < 12; i++) {  // Perlen und Edelsteine
      float k = fmodf(u * 0.5f + i / 12.0f, 1);
      g.kreis(120 + i * 8, 186 - k * 60, 2.4f, i % 3 == 0 ? WEISS : i % 3 == 1 ? FARBE(0xE8473D) : FARBE(0x5BB8E8), (uint8_t)(255 * huepf(k)));
    }
    g.kameraAus();
    blase(g, 150, 50, {"Hänsel, wir sind erlöst!"}, aufzu(u, 2, 7), 170, 140);
    vers(g, {"In allen Ecken standen Kasten", "mit Perlen und Edelsteinen."}, aufzu(u, 7.2f, 13.6f));
    return;
  }
  float v = u - 14;
  wasser(g, u);
  float k = seg(v, 8, 18);
  float ex = lerp(60, 230, k);
  ente(g, ex, 226, 2.0f, u);
  FrauPose gr = fposis(GRETEL, u, 1);
  gr.mund = v < 8 ? 0.4f * fabsf(sinf(u * 5)) : 0.3f;
  Pose h = posis(HAENSEL, u);
  h.mund = gr.mund;
  if (k <= 0) { gretel(g, 24, 206, 1, gr); haensel(g, 46, 206, 1, h); }
  else if (k < 1) { gretel(g, ex - 14, 214, 1, gr); haensel(g, ex + 8, 214, 1, h); }
  else { gretel(g, 252, 206, 1, gr); haensel(g, 272, 206, 1, h); }
  g.kameraAus();
  if (v < 1) g.rechteck(0, 0, 284, 240, 0, (uint8_t)(255 * (1 - v)));
  blase(g, 140, 50, {"Entchen, Entchen, da steht", "Gretel und Hänsel. Kein Steg", "und keine Brücke, nimm uns", "auf deinen weißen Rücken!"}, aufzu(v, 1, 8), 40, 150);
  blende(g, u, 36, 160, 150);
}

// 8) Heim zum Vater - "Mein Maerchen ist aus, dort lauft eine Maus"
static void szeneHeim(Gfx &g, float u) {
  wald(g, u, 0);
  g.kontur(1.6f, OUT);
  g.rechteck(176, 130, 80, 70, FARBE(0xC9B08A));
  float d[] = {168, 132, 216, 96, 264, 132};
  g.polygon(d, 3, FARBE(0x8E5A3A));
  g.konturAus();
  g.rundRechteck(206, 160, 20, 40, 8, HOLZ);
  Pose v = posis(VATER, u);
  v.armV = 2.4f + sinf(u * 5) * 0.3f;
  v.mund = 0.6f;
  v.zu = u > 3 && u < 8;
  schneiderlein(g, 150, 214, 1, -1, v);
  FrauPose gr = fposis(GRETEL, u, 1);
  gr.armV = 2.4f; gr.mund = 0.5f;
  gretel(g, 92, 214 - fabsf(sinf(u * 5)) * 4, 1, gr);
  Pose h = posis(HAENSEL, u);
  h.armV = 2.4f; h.mund = 0.5f;
  float hx, hy;
  haensel(g, 118, 214 - fabsf(sinf(u * 5 + 1)) * 4, 1, h, &hx, &hy);
  for (int i = 0; i < 6; i++) {
    float k = fmodf(u * 0.5f + i / 6.0f, 1);
    g.kreis(110 + sinf(k * 6 + i) * 30, 170 - k * 80, 2.4f, i & 1 ? WEISS : FARBE(0xE8473D), (uint8_t)(255 * huepf(k)));
  }
  herzen(g, 130, 140, u, 4, 30);
  // die Maus
  if (u > 12) {
    float mx = lerp(-20, 300, seg(u, 12, 20));
    g.kurve(mx - 6, 224, mx - 14, 218, mx - 18, 226, 1, FARBE(0xC9A0A0));
    g.ellipse(mx, 222, 7, 4, FARBE(0xA8A8B0));
    g.kreis(mx + 5, 218, 2.6f, FARBE(0xE8C0C0));
    g.kreis(mx + 7, 221, 0.9f, SCHWARZ);
  }
  g.kameraAus();
  vers(g, {"Da hatten alle Sorgen ein Ende,", "und sie lebten in lauter", "Freude zusammen."}, aufzu(u, 0.6f, 11));
  vers(g, {"Mein Märchen ist aus, dort lauft eine", "Maus, wer sie fängt, darf sich eine", "große Pelzkappe daraus machen."}, aufzu(u, 11.4f, 22));
  endeBild(g, u, 24);
  if (u < 1.2f) g.iris(142, 150, lerp(0, 330, sanft(u / 1.2f)));
}

static void bild(Gfx &g, float t) {
  if (t < P2) szeneNacht(g, t - P1);
  else if (t < P3) szeneKiesel(g, t - P2);
  else if (t < P4) szeneKrumen(g, t - P3);
  else if (t < P5) szeneHaeuschen(g, t - P4);
  else if (t < P6) szeneStall(g, t - P5);
  else if (t < P7) szeneOfen(g, t - P6);
  else if (t < P8) szeneEntchen(g, t - P7);
  else szeneHeim(g, t - P8);
}

// =====================================================================
// Musik
// =====================================================================
// Haensel und Gretel (C-Dur, wie ein Kinderlied)
static const Ton3 MEL_HG[] = {
  {79, .5, 48}, {76, .5, 0}, {76, 1, 0}, {77, .5, 41}, {74, .5, 0}, {74, 1, 0}, {72, .5, 48}, {74, .5, 0}, {76, .5, 0},
  {77, .5, 0}, {79, .5, 43}, {79, .5, 0}, {79, 1, 0}, {79, .5, 48}, {76, .5, 0}, {76, 1, 0}, {77, .5, 41}, {74, .5, 0},
  {74, 1, 0}, {72, .5, 48}, {76, .5, 0}, {79, .5, 0}, {79, .5, 0}, {72, 2, 48}};

static void knusper(Synth &s, float x) {
  for (int i = 0; i < 6; i++) s.rauschen(x + i * 0.14f, 0.04f, HOCHPASS, 3500, 0.08f);
}

static void ton(Synth &s, float t0, float t1) {
  float a = P1;
  spieleMelodie(s, M(MEL_SPUK), a + 0.5f, a + 15, 0.42f, t0, t1, 0.55f);
  spieleMelodie(s, M(MEL_N), a + 16.5f, a + 29.5f, 0.42f, t0, t1, 0.7f);
  for (int i = 0; i < 7; i++) if (bei(a + 17 + i * 1.6f, t0, t1)) s.glocke(a + 17 + i * 1.6f, 88 + (i % 3) * 2, 0.25f);  // Kiesel
  a = P2;
  spieleMelodie(s, M(MEL_HG), a + 0.4f, a + 17, 0.3f, t0, t1, 0.75f);
  for (float x = ceilf(t0 / 2.0f) * 2.0f; x < t1; x += 2.0f)
    if (x > a && x < a + 17) s.ton(x, 900, 0.03f, 0.002f, 0.05f, SINUS, GERAEUSCH);  // Steinchen faellt
  for (float x = ceilf(t0 / 0.3f) * 0.3f; x < t1; x += 0.3f)
    if (x > a + 18 && x < a + 26) s.rauschen(x, 0.1f, HOCHPASS, 2500, 0.02f);  // Feuer knistert
  spieleMelodie(s, M(MEL_N), a + 19.5f, a + 31.5f, 0.4f, t0, t1, 0.6f);
  a = P3;
  spieleMelodie(s, M(MEL_HG), a + 0.4f, a + 5, 0.3f, t0, t1, 0.7f);
  for (float x = ceilf(t0 / 0.2f) * 0.2f; x < t1; x += 0.2f)
    if (x > a + 4 && x < a + 11) s.ton(x, 2400 + ((int)(x * 9) % 3) * 300, 0.02f, 0.002f, 0.05f, SINUS, GERAEUSCH);  // pick, pick
  spieleMelodie(s, M(MEL_SPUK), a + 12, a + 23.5f, 0.42f, t0, t1, 0.6f);
  a = P4;
  for (int i = 0; i < 8; i++) if (bei(a + 0.4f + i * 0.7f, t0, t1)) s.note(a + 0.4f + i * 0.7f, 84 + (i % 4) * 3, 0.5f);  // das Voeglein singt
  spieleMelodie(s, M(MEL_HG), a + 6, a + 15.5f, 0.28f, t0, t1, 0.8f);
  if (bei(a + 10.4f, t0, t1)) knusper(s, a + 10.4f);
  if (bei(a + 13.4f, t0, t1)) knusper(s, a + 13.4f);
  spieleMelodie(s, M(MEL_SPUK), a + 16, a + 22.6f, 0.36f, t0, t1, 0.6f);
  spieleMelodie(s, M(MEL_HG), a + 23, a + 35.5f, 0.3f, t0, t1, 0.75f);
  if (bei(a + 26, t0, t1)) knusper(s, a + 26);
  a = P5;
  spieleMelodie(s, M(MEL_SPUK), a + 0.4f, a + 29.5f, 0.4f, t0, t1, 0.65f);
  a = P6;
  spieleMelodie(s, M(MEL_SPUK), a + 0.4f, a + 21, 0.34f, t0, t1, 0.7f);
  for (float x = ceilf(t0 / 0.3f) * 0.3f; x < t1; x += 0.3f)
    if (x > a && x < a + 22) s.rauschen(x, 0.1f, HOCHPASS, 2500, 0.02f);
  if (bei(a + 22.6f, t0, t1)) { s.ton(a + 22.6f, 120, 0.35f, 0.002f, 0.4f, SINUS, GERAEUSCH, 60); s.rauschen(a + 22.6f, 0.5f, TIEFPASS, 1200, 0.25f); }  // die Tuer
  if (bei(a + 23.3f, t0, t1)) s.ton(a + 23.3f, 1400, 0.08f, 0.002f, 0.1f, SINUS, GERAEUSCH, 1300);  // Riegel
  spieleMelodie(s, M(MEL_WIEGE), a + 31, a + 37.5f, 0.42f, t0, t1, 0.6f);
  a = P7;
  spieleMelodie(s, M(MEL_F), a + 0.4f, a + 13, 0.26f, t0, t1, 0.85f);
  for (float x = ceilf(t0 / 0.12f) * 0.12f; x < t1; x += 0.12f)
    if (x > a + 7 && x < a + 12) s.glocke(x, 88 + ((int)(x * 11) % 5) * 2, 0.12f);  // Edelsteine
  spieleMelodie(s, M(MEL_HG), a + 14.5f, a + 35.5f, 0.32f, t0, t1, 0.75f);
  for (int i = 0; i < 3; i++) if (bei(a + 22 + i * 3, t0, t1)) s.ton(a + 22 + i * 3, 330, 0.06f, 0.01f, 0.14f, RECHTECK, GERAEUSCH, 260);  // quak
  a = P8;
  spieleMelodie(s, M(MEL_HG), a + 0.4f, a + 24, 0.28f, t0, t1, 0.85f);
  for (float x = ceilf(t0 / 0.15f) * 0.15f; x < t1; x += 0.15f)
    if (x > a + 12 && x < a + 20) s.note(x, 98 + ((int)(x * 7) % 3) * 2, 0.08f);  // die Maus piepst
  endeKlang(s, a + 26.2f, t0, t1);
}

// =====================================================================
// Startbild, Vorschau, Erkennungszeichen
// =====================================================================
static void titelSzene(Gfx &g, float t) {
  wald(g, t, 0);
  kuchenhaus(g, 190, 214, t, 0);
  weisserVogel(g, 202, 98, t, false);
  FrauPose gr = fposis(GRETEL, t, 1);
  gr.armV = 1.8f; gr.mund = 0.4f;
  gretel(g, 70, 222, 1, gr);
  Pose h = posis(HAENSEL, t);
  h.armV = 1.8f; h.mund = 0.4f;
  haensel(g, 100, 222, 1, h);
}
static void titelbild(Gfx &g, float t) { titelSzene(g, t); titelRahmen(g, t, "Hänsel und Gretel", 196, SCHRIFT_M); }
static void vorschau(Gfx &g, float t) { titelSzene(g, t); }
static void symbol(Gfx &g, float x, float y, float t) {
  float z = 0.6f;
  g.ansicht(x - 190 * z, y - 214 * z, z);
  kuchenhaus(g, 190, 214, t, 0);
  g.kameraAus();
  glitzern(g, x, y - 50, 40, t, 4, 200);
}

const Maerchen HAENSEL_GRETEL = {"Hänsel und Gretel", PDAUER, FARBE(0xB8702E), bild, ton, titelbild, vorschau, symbol,
                                 LIED(MEL_HG, 0.3f), nullptr};

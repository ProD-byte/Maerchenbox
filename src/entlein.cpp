// Das haessliche Entlein - ein Spieluhr-Film fuer die Maerchenbox, nach dem Maerchen von Hans Christian Andersen.
// Das grosse Ei, der Entenhof, die Flucht ins Moor, der Kater und die Henne, die Schwaene im Herbst, der Winter -
// und im Fruehling sieht es sein Bild im Wasser: ein Schwan.
#include "erzaehlen.h"

// ---------- Ablauf (Sekunden) ----------
static const float P1 = 0, P2 = 34, P3 = 62, P4 = 92, P5 = 120, P6 = 152, PDAUER = 200;

// ---------- Tiere ----------
static void ente(Gfx &g, float x, float y, float s, float dir, float t, float schwimm, float mund = 0, Farbe fed = WEISS) {
  auto PX = [&](float d) { return x + d * s * dir; };
  auto PY = [&](float d) { return y + d * s; };
  float w = schwimm > 0 ? sinf(t * 2) * 1.2f : 0;
  if (schwimm <= 0) {
    g.linie(PX(-1), PY(-4), PX(-1), PY(0), 1.6f * s, FARBE(0xF28A2A));
    g.linie(PX(4), PY(-4), PX(4), PY(0), 1.6f * s, FARBE(0xF28A2A));
  }
  g.kontur(1.1f * s, OUT);
  g.ellipse(PX(0), PY(-9) + w, 13 * s, 7 * s, fed);
  float sw[] = {PX(-11), PY(-10) + w, PX(-17), PY(-15) + w, PX(-13), PY(-6) + w};
  g.polygon(sw, 3, fed);
  g.kreis(PX(10), PY(-19) + w, 6 * s, fed);
  g.konturAus();
  float sn[] = {PX(14), PY(-20) + w, PX(22), PY(-18 + mund * 2) + w, PX(14), PY(-16) + w};
  g.polygon(sn, 3, FARBE(0xF28A2A));
  g.kreis(PX(11), PY(-21) + w, 1.2f * s, SCHWARZ);
}

static void kueken(Gfx &g, float x, float y, float s, float dir, float t, float schwimm, int i) {
  auto PX = [&](float d) { return x + d * s * dir; };
  auto PY = [&](float d) { return y + d * s; };
  float w = schwimm > 0 ? sinf(t * 3 + i) * 0.8f : -huepf(fmodf(t * 1.5f + i * 0.3f, 1)) * 2;
  g.kontur(1, OUT);
  g.ellipse(PX(0), PY(-5) + w, 6 * s, 4.5f * s, FARBE(0xF6D84A));
  g.kreis(PX(5), PY(-10) + w, 3.6f * s, FARBE(0xF6D84A));
  g.konturAus();
  float sn[] = {PX(8), PY(-10) + w, PX(11), PY(-9) + w, PX(8), PY(-8) + w};
  g.polygon(sn, 3, FARBE(0xF28A2A));
  g.kreis(PX(6), PY(-11) + w, 0.8f * s, SCHWARZ);
}

// das haessliche Entlein: grau, zerzaust, zu gross
static void entlein(Gfx &g, float x, float y, float s, float dir, float t, float schwimm, bool traurig, float wachs = 0) {
  auto PX = [&](float d) { return x + d * s * dir; };
  auto PY = [&](float d) { return y + d * s; };
  const Farbe fed = mischeFarbe(FARBE(0x8E8A86), FARBE(0xB8B4AE), wachs), dunkel = FARBE(0x6E6A66);
  float w = schwimm > 0 ? sinf(t * 2.4f) * 1 : 0;
  if (schwimm <= 0) {
    g.linie(PX(-1), PY(-5), PX(-2), PY(0), 1.8f * s, FARBE(0x8E7A5A));
    g.linie(PX(4), PY(-5), PX(5), PY(0), 1.8f * s, FARBE(0x8E7A5A));
  }
  g.kontur(1.1f * s, OUT);
  g.ellipse(PX(0), PY(-10) + w, 12 * s, 8 * s, fed);
  float hals = 10 + wachs * 6;
  g.linie(PX(6), PY(-13) + w, PX(10), PY(-13 - hals) + w, 4 * s, fed);
  g.kreis(PX(11), PY(-15 - hals) + w, 5.5f * s, fed);
  g.konturAus();
  for (int i = 0; i < 5; i++) g.linie(PX(-8 + i * 4), PY(-16) + w, PX(-9 + i * 4), PY(-20 - (i & 1) * 2) + w, 1.2f * s, dunkel);  // zerzaust
  g.linie(PX(9), PY(-20 - hals) + w, PX(8), PY(-24 - hals) + w, 1.2f * s, dunkel);
  float sn[] = {PX(15), PY(-16 - hals) + w, PX(21), PY(-15 - hals) + w, PX(15), PY(-13 - hals) + w};
  g.polygon(sn, 3, FARBE(0x5E5A56));
  if (traurig) {
    g.bogen(PX(12), PY(-16 - hals) + w, 1.4f * s, 3.6f, 5.8f, 1 * s, OUT);
    float k = fmodf(t * 1.2f, 1);
    g.kreis(PX(12), PY(-14 - hals) + w + k * 8 * s, 1 * s, FARBE(0x8FD8FF), (uint8_t)(255 * (1 - k)));
  } else g.kreis(PX(12), PY(-17 - hals) + w, 1.2f * s, SCHWARZ);
}

static void schwan(Gfx &g, float x, float y, float s, float dir, float t, float flug = -1, float neig = 0) {
  auto PX = [&](float d) { return x + d * s * dir; };
  auto PY = [&](float d) { return y + d * s; };
  float w = flug < 0 ? sinf(t * 1.6f + x) * 1.2f : 0;
  g.kontur(1.2f * s, OUT);
  if (flug >= 0) {  // im Flug: Fluegel schlagen
    float f = sinf(flug * 6.2832f);
    float fl[] = {PX(-6), PY(-10), PX(-24), PY(-30 - f * 14), PX(10), PY(-12)};
    g.polygon(fl, 3, WEISS);
    g.ellipse(PX(0), PY(-10), 16 * s, 6 * s, WEISS);
    g.linie(PX(12), PY(-12), PX(30), PY(-16), 3.5f * s, WEISS);
    g.kreis(PX(32), PY(-17), 4 * s, WEISS);
    g.konturAus();
    float sn[] = {PX(35), PY(-18), PX(41), PY(-16), PX(35), PY(-15)};
    g.polygon(sn, 3, FARBE(0xF28A2A));
    return;
  }
  g.ellipse(PX(0), PY(-12) + w, 20 * s, 10 * s, WEISS);
  float sw[] = {PX(-16), PY(-14) + w, PX(-26), PY(-24) + w, PX(-20), PY(-8) + w};
  g.polygon(sw, 3, WEISS);
  g.ellipse(PX(-4), PY(-16) + w, 12 * s, 6 * s, FARBE(0xF2F2F6));  // Fluegel
  // der lange, gebogene Hals
  float hx = PX(22 - neig * 6), hy = PY(-42 + neig * 18) + w;
  g.kurve(PX(12), PY(-16) + w, PX(22), PY(-26) + w, hx, hy, 5 * s, WEISS);
  g.kreis(hx, hy, 5 * s, WEISS);
  g.konturAus();
  float sn[] = {hx + 3 * s * dir, hy - 1 * s, hx + 11 * s * dir, hy + 1.5f * s + neig * 3 * s, hx + 3 * s * dir, hy + 2.5f * s};
  g.polygon(sn, 3, FARBE(0xF28A2A));
  g.kreis(hx + 3 * s * dir, hy - 0.5f * s, 1.6f * s, SCHWARZ);  // schwarzer Hoecker
  g.kreis(hx + 1 * s * dir, hy - 1.5f * s, 1 * s, SCHWARZ);
}

static void henne(Gfx &g, float x, float y, float s, float dir, float t, float pick, float mund = 0) {
  auto PX = [&](float d) { return x + d * s * dir; };
  auto PY = [&](float d) { return y + d * s; };
  g.linie(PX(-2), PY(-8), PX(-3), PY(-1), 1.4f * s, FARBE(0xE0A23C));
  g.linie(PX(3), PY(-8), PX(4), PY(-1), 1.4f * s, FARBE(0xE0A23C));
  float sw[] = {PX(-8), PY(-15), PX(-15), PY(-24), PX(-13), PY(-12)};
  g.polygon(sw, 3, FARBE(0xB8703E));
  g.kontur(1.1f * s, OUT);
  g.ellipse(PX(0), PY(-14), 11 * s, 8 * s, FARBE(0xB8703E));
  float kx = PX(8 + pick * 4), ky = PY(-22 + pick * 12);
  g.kreis(kx, ky, 5 * s, FARBE(0xB8703E));
  g.konturAus();
  g.kreis(kx + 0.5f * s * dir, ky - 5 * s, 2.4f * s, FARBE(0xE0302A));
  float sn[] = {kx + 4 * s * dir, ky - 1 * s, kx + 9 * s * dir, ky + mund * 2 * s, kx + 4 * s * dir, ky + 2 * s};
  g.polygon(sn, 3, FARBE(0xF2B230));
  g.kreis(kx + 1.5f * s * dir, ky - 1 * s, 1.1f * s, SCHWARZ);
}

static void kater(Gfx &g, float x, float y, float s, float dir, float t, float buckel) {
  auto PX = [&](float d) { return x + d * s * dir; };
  auto PY = [&](float d) { return y + d * s; };
  const Farbe fell = FARBE(0x3A3436);
  g.kontur(1.1f * s, OUT);
  for (int i = 0; i < 4; i++) g.linie(PX(-9 + i * 6), PY(-10 - buckel * 4), PX(-9 + i * 6), PY(-1), 3 * s, fell);
  g.kurve(PX(-11), PY(-12), PX(-22), PY(-18), PX(-18), PY(-32), 3 * s, fell);
  g.ellipse(PX(0), PY(-14 - buckel * 6), 13 * s, (7 + buckel * 4) * s, fell);
  float kx = PX(12), ky = PY(-22);
  g.kreis(kx, ky, 7.5f * s, fell);
  float o1[] = {kx - 6 * s * dir, ky - 3 * s, kx - 6 * s * dir, ky - 13 * s, kx - 1 * s * dir, ky - 6 * s};
  float o2[] = {kx + 1 * s * dir, ky - 6 * s, kx + 6 * s * dir, ky - 13 * s, kx + 7 * s * dir, ky - 3 * s};
  g.polygon(o1, 3, fell);
  g.polygon(o2, 3, fell);
  g.konturAus();
  for (int i = 0; i < 2; i++) {
    float ex = kx + (i ? 4.5f : -1.5f) * s * dir;
    g.kreis(ex, ky - 1.5f * s, 1.8f * s, FARBE(0x9ACB4A));
    g.ellipse(ex, ky - 1.5f * s, 0.5f * s, 1.5f * s, SCHWARZ);
  }
}

// ---------- Kulissen ----------
static void teich(Gfx &g, float t, float jahreszeit) {
  // jahreszeit: 0 Sommer, 1 Herbst, 2 Winter, 3 Fruehling
  Farbe gras = jahreszeit < 1 ? GRAS : jahreszeit < 2 ? mischeFarbe(GRAS, FARBE(0xC0902E), jahreszeit - 1 + 0.4f)
             : jahreszeit < 3 ? WEISS : mischeFarbe(WEISS, FARBE(0x8ADB6A), klemme(jahreszeit - 3 + 0.6f));
  himmel(g, jahreszeit >= 2 && jahreszeit < 3 ? 0.3f : 0, t, 190);
  sonne(g, 248, 30, 11, t);
  for (int i = 0; i < 3; i++) {
    Farbe kr = jahreszeit < 1 ? FARBE(0x4CB85A) : jahreszeit < 2 ? FARBE(0xD08A2E) : jahreszeit < 3 ? FARBE(0xE8E8F0) : FARBE(0x8EDB7A);
    laubbaum(g, 20 + i * 120, 190, 0.8f, kr);
  }
  boden(g, 190, gras);
  bool eis = jahreszeit >= 2 && jahreszeit < 3;
  g.ellipse(160, 222, 130, 26, eis ? FARBE(0xD8EEFF) : FARBE(0x6EB8E8));
  g.ellipse(160, 220, 120, 20, eis ? FARBE(0xE8F6FF) : FARBE(0x8ECCF0));
  if (!eis)
    for (int i = 0; i < 6; i++) {
      float x = fmodf(i * 43 + t * 10, 220) + 50;
      g.linie(x, 214 + (i % 3) * 6, x + 12, 214 + (i % 3) * 6, 1.2f, FARBE(0xBFE6FF), 160);
    }
  // Schilf am Rand
  for (int i = 0; i < 9; i++) {
    float x = 30 + i * 6 + (i > 4 ? 190 : 0);
    g.linie(x, 214, x + sinf(t + i) * 2, 186 - (i % 3) * 6, 1.6f, jahreszeit < 1 || jahreszeit >= 3 ? FARBE(0x5DAE48) : FARBE(0xA88A4E));
    g.ellipse(x + sinf(t + i) * 2, 186 - (i % 3) * 6, 2, 5, FARBE(0x6E4A2A));
  }
  if (jahreszeit >= 1 && jahreszeit < 2)
    for (int i = 0; i < 8; i++) {
      float x = fmodf(zufall(i + 40) * 300 + t * 10, 300) - 10, y = fmodf(zufall(i + 60) * 200 + t * 20, 200);
      g.ellipse(x, y, 3, 2, i & 1 ? FARBE(0xE07A2A) : FARBE(0xC9A43A), 220, t + i);
    }
  if (eis) for (int i = 0; i < 24; i++) g.kreis(zufall(i + 30) * 284, fmodf(zufall(i + 50) * 240 + t * 24, 240), 1.6f, WEISS);
}

static void hof(Gfx &g, float t) {
  himmel(g, 0, t, 190);
  sonne(g, 248, 30, 11, t);
  g.kontur(1.6f, OUT);
  g.rechteck(160, 110, 110, 80, FARBE(0xF4E4C4));
  float d[] = {150, 112, 215, 70, 280, 112};
  g.polygon(d, 3, FARBE(0xC0503A));
  g.konturAus();
  g.rundRechteck(204, 150, 22, 40, 8, HOLZ);
  boden(g, 190, FARBE(0xB8A06A));
  for (int i = 0; i < 22; i++) g.linie(i * 13, 196, i * 13 + 3, 190, 1, FARBE(0x9A8A4E));
  // Zaun / Hecke
  for (int i = 0; i < 6; i++) g.kreis(10 + i * 18, 186, 12, FARBE(0x4E8A4A));
}

static void huette(Gfx &g, float t) {
  g.verlauf(0, 200, FARBE(0xC9B08A), FARBE(0xA88E6A));
  g.rechteck(0, 200, 284, 40, FARBE(0x6E5A46));
  for (int i = 0; i < 6; i++) g.linie(0, 40 + i * 28, 284, 40 + i * 28, 1, FARBE(0x9A8466), 120);  // Balken
  g.rechteck(0, 196, 284, 5, FARBE(0x5A4A38));
  g.kontur(1.6f, OUT);
  g.rundRechteck(190, 40, 60, 54, 4, HOLZ);
  g.konturAus();
  fenster(g, 195, 45, 50, 44, 0, true, false);
  herd(g, 20, 202, 70, 52, t, 0.5f, FARBE(0x7A6A5A), false);  // Ofen
}

// =====================================================================
// Szenen
// =====================================================================
// 1) Die Eier schluepfen - zuletzt das grosse Ei
static void szeneEier(Gfx &g, float u) {
  teich(g, u, 0);
  // Nest im Schilf
  g.ellipse(110, 214, 40, 10, FARBE(0xC9A46A));
  ente(g, 110, 206, 1.3f, 1, u, 0, u > 18 && u < 24 ? 0.6f : 0, FARBE(0xF2EEE4));
  for (int i = 0; i < 4; i++) {
    float auf = seg(u, 3 + i * 1.5f, 4 + i * 1.5f);
    float x = 70 + i * 14;
    if (auf < 1) {
      g.kontur(1, OUT);
      g.ellipse(x, 212, 5, 6.5f, FARBE(0xF2EEE4));
      g.konturAus();
      if (auf > 0) g.linie(x - 4, 210, x + 4, 212, 1, OUT);
    } else kueken(g, x - 30 + i * 4, 214, 1.2f, -1, u, 0, i);
  }
  // das grosse Ei
  float gross = seg(u, 14, 16);
  if (gross < 1) {
    g.kontur(1.2f, OUT);
    g.ellipse(150, 210, 9, 12, FARBE(0xE8E2D8));
    g.konturAus();
    if (u > 12) g.linie(144, 206, 156, 212, 1.2f, OUT);
  } else entlein(g, 160, 216, 1.1f, 1, u, 0, false);
  g.kameraAus();
  vers(g, {"Es war so herrlich draußen", "auf dem Lande! Es war Sommer."}, aufzu(u, 0.4f, 3));
  vers(g, {"Pip, pip! sagte es, und alle", "Eidotter waren lebendig geworden."}, aufzu(u, 3.2f, 10));
  vers(g, {"Endlich platzte das große Ei.", "Das Junge war groß und hässlich."}, aufzu(u, 14, 20));
  blase(g, 110, 140, {"Das ist ja ein entsetzlich", "großes Entlein!"}, aufzu(u, 20, 26), 130, 170);
  blase(g, 110, 140, {"Aber es schwimmt schön.", "Es ist mein eigenes Kind!"}, aufzu(u, 26.4f, 33), 130, 170);
  blende(g, u, 34, 140, 170);
}

// 2) Auf dem Entenhof: alle picken und lachen
static void szeneHof(Gfx &g, float u) {
  hof(g, u);
  float pick = huepf(fmodf(u * 1.4f, 1));
  for (int i = 0; i < 3; i++) henne(g, 60 + i * 30, 214, 1, 1, u + i, pick * (i != 1));
  ente(g, 210, 214, 1.1f, -1, u, 0, u > 4 && u < 10 ? 0.6f : 0);
  for (int i = 0; i < 4; i++) kueken(g, 230 + i * 12, 214, 1.1f, -1, u, 0, i);
  entlein(g, 150, 216, 1.1f, 1, u, 0, u > 10, 0);
  if (u > 6) for (int i = 0; i < 3; i++) g.stern(150 + (i - 1) * 10, 180 - i * 4, 3, 1.2f, 4, u * 6 + i, OUT);  // gepickt
  g.kameraAus();
  vers(g, {"Das arme Entlein wurde von", "allen gebissen, gestoßen", "und gehänselt."}, aufzu(u, 0.4f, 6));
  blase(g, 200, 120, {"Es ist zu groß", "und ungewöhnlich!"}, aufzu(u, 6.2f, 11), 210, 180);
  vers(g, {"Selbst seine Geschwister lachten.", "Da flog es über den Zaun davon."}, aufzu(u, 13, 24));
  if (u > 20) { g.rechteck(0, 0, 284, 240, 0, (uint8_t)(255 * seg(u, 26, 28))); }
  if (u < 1.2f) g.iris(150, 170, lerp(0, 330, sanft(u / 1.2f)));
}

// 3) Im Moor: die Jagd (nur angedeutet), "wie haesslich ich bin"
static void szeneMoor(Gfx &g, float u) {
  teich(g, u, 0.4f);
  entlein(g, 140, 220, 1.1f, 1, u, 1, u > 12, 0);
  for (int i = 0; i < 2; i++) ente(g, 210 + i * 30, 222, 1, -1, u + i, 1, 0, FARBE(0x8E6E4E));  // Wildenten
  if (u > 10 && u < 22) {  // ein grosser Hund schnueffelt und geht wieder
    float k = seg(u, 10, 22);
    float hx = k < 0.5f ? lerp(-40, 110, k * 2) : lerp(110, -50, (k - 0.5f) * 2);
    g.kontur(1.2f, OUT);
    g.ellipse(hx, 196, 22, 10, FARBE(0x8E6E4E));
    g.kreis(hx + (k < 0.5f ? 22 : -22), 186, 9, FARBE(0x8E6E4E));
    g.konturAus();
    for (int i = 0; i < 4; i++) g.linie(hx - 14 + i * 9, 202, hx - 14 + i * 9, 214, 4, FARBE(0x7A5A3E));
  }
  g.kameraAus();
  vers(g, {"Es kam hinaus in das große Moor,", "wo die wilden Enten wohnten."}, aufzu(u, 0.6f, 9));
  blase(g, 100, 120, {"Wie hässlich ich bin, dass mich", "sogar der Hund nicht mag!"}, aufzu(u, 22, 29.6f), 150, 180);
  blende(g, u, 30, 140, 180);
}

// 4) Bei der alten Frau: der Kater und die Henne
static void szeneHuette(Gfx &g, float u) {
  huette(g, u);
  kater(g, 200, 214, 1.4f, -1, u, u > 8 && u < 13 ? 1 : 0);
  henne(g, 150, 214, 1.4f, -1, u, 0, u > 2 && u < 7 ? 0.6f : 0);
  entlein(g, 70, 216, 1.2f, 1, u, 0, u > 14, 0);
  g.kameraAus();
  blase(g, 150, 110, {"Kannst du Eier legen?"}, aufzu(u, 2, 6.6f), 158, 170);
  blase(g, 210, 110, {"Kannst du einen Buckel", "machen und schnurren?"}, aufzu(u, 7, 12.6f), 214, 170);
  blase(g, 80, 120, {"Ich glaube, ich gehe", "hinaus in die weite Welt."}, aufzu(u, 14, 22), 80, 180);
  blende(g, u, 28, 100, 180);
}

// 5) Herbst: die Schwaene ziehen fort; Winter: das Eis
static void szeneWinter(Gfx &g, float u) {
  float jz = u < 14 ? 1.2f : 2.5f;
  teich(g, u, jz);
  if (u < 14) {
    for (int i = 0; i < 3; i++) {
      float k = seg(u, 1 + i * 0.6f, 12 + i * 0.6f);
      schwan(g, lerp(-40, 330, k), 70 + i * 16 - k * 30, 0.9f, 1, u, fmodf(u * 0.9f + i * 0.3f, 1));
    }
    entlein(g, 140, 220, 1.1f, 1, u, 1, false, 0.2f);
    g.kameraAus();
    vers(g, {"Da kamen herrliche, große Vögel,", "blendend weiß, mit langen Hälsen."}, aufzu(u, 0.6f, 7));
    vers(g, {"Es waren Schwäne. Sie flogen", "fort nach warmen Ländern."}, aufzu(u, 7.2f, 13.6f));
  } else {
    float v = u - 14;
    entlein(g, 160, 214, 1.1f, 1, u, 0, true, 0.4f);
    g.ellipse(160, 214, 18, 4, FARBE(0xC9E6FF));  // festgefroren
    g.kameraAus();
    vers(g, {"Der Winter wurde so kalt, so kalt!", "Das Entlein fror im Eise fest."}, aufzu(v, 0.6f, 8));
    vers(g, {"Ein Bauer brach das Eis entzwei", "und nahm es mit nach Hause."}, aufzu(v, 8.2f, 15.6f));
  }
  blende(g, u, 32, 160, 180);
}

// 6) Im Fruehling: sein Bild im Wasser - ein Schwan!
static void szeneSchwan(Gfx &g, float u) {
  teich(g, u, 3.4f);
  for (int i = 0; i < 3; i++) schwan(g, 190 + i * 30, 222, 1, -1, u + i, -1, u > 14 && u < 20 ? 0.6f : 0);
  // das Entlein ist gewachsen: im Wasser sieht man sein Bild
  bool erkannt = u > 12;
  float sx = lerp(40, 130, sanft(seg(u, 0, 6)));
  float neig = u > 6 && u < 14 ? seg(u, 6, 8) : 0;
  if (!erkannt) entlein(g, sx, 222, 1.2f, 1, u, 1, true, 1);
  else schwan(g, sx, 222, 1.05f, 1, u, -1, neig);
  // Spiegelbild (nur angedeutet: heller Schwanenumriss im Wasser)
  if (u > 8) {
    float a = seg(u, 8, 12);
    g.ellipse(sx, 230, 18, 4, WEISS, (uint8_t)(160 * a));
    g.linie(sx + 12, 230, sx + 20, 238, 3, WEISS, (uint8_t)(140 * a));
  }
  if (erkannt) glitzern(g, sx, 190, 30, u, 6, (uint8_t)(255 * (1 - seg(u, 20, 26))));
  // Kinder am Ufer
  if (u > 24) {
    for (int i = 0; i < 2; i++) {
      FrauLook l;
      l.kleid = i ? FARBE(0xF28AB8) : FARBE(0x5B8FD7);
      l.haar = i ? FARBE(0xF2C94E) : FARBE(0x8A5A2E);
      l.frisur = 1;
      FrauPose p = fposis(l, u, i);
      p.armV = 2.4f;
      p.mund = 0.6f;
      frau(g, 30 + i * 28, 196, 0.6f, 1, p);
    }
  }
  if (u > 30) herzen(g, sx + 10, 180, u, 4, 24);
  g.kameraAus();
  vers(g, {"Als die Sonne wieder warm schien,", "flog es zu den herrlichen", "Schwänen."}, aufzu(u, 0.4f, 6));
  vers(g, {"Aber was sah es in dem klaren", "Wasser? Es war kein hässlicher", "grauer Vogel mehr."}, aufzu(u, 6.2f, 13));
  vers(g, {"Es war selbst ein Schwan!"}, aufzu(u, 13.2f, 20));
  blase(g, 70, 120, {"Da ist ein neuer!", "Der neue ist der schönste!"}, aufzu(u, 24.4f, 31), 50, 170);
  vers(g, {"So viel Glück hatte es sich nicht", "träumen lassen, als es noch das", "hässliche Entlein war!"}, aufzu(u, 31.4f, 42));
  endeBild(g, u, 44);
}

static void bild(Gfx &g, float t) {
  if (t < P2) szeneEier(g, t - P1);
  else if (t < P3) szeneHof(g, t - P2);
  else if (t < P4) szeneMoor(g, t - P3);
  else if (t < P5) szeneHuette(g, t - P4);
  else if (t < P6) szeneWinter(g, t - P5);
  else szeneSchwan(g, t - P6);
}

// =====================================================================
// Musik
// =====================================================================
// das Lied vom Entlein (d-Moll, am Schluss als Schwanenlied in D-Dur)
static const Ton3 MEL_ENTE[] = {
  {74, 1, 50}, {72, .5, 0}, {70, .5, 0}, {69, 1, 0}, {65, 1, 46}, {67, .5, 0}, {69, .5, 0}, {70, 2, 0},
  {69, 1, 50}, {67, .5, 0}, {65, .5, 0}, {64, 1, 0}, {62, 1, 46}, {64, 1, 0}, {65, .5, 0}, {64, .5, 0}, {62, 2, 50}};
static const Ton3 MEL_SCHWAN[] = {
  {74, 1, 50}, {78, 1, 0}, {81, 1, 0}, {86, 2, 45}, {85, 1, 0}, {83, 2, 47}, {81, 1, 0}, {78, 3, 50},
  {79, 1, 43}, {81, 1, 0}, {83, 1, 0}, {85, 2, 45}, {81, 1, 0}, {78, 2, 50}, {76, 1, 0}, {74, 3, 50}};

static void pip(Synth &s, float x) { s.note(x, 96, 0.3f); s.note(x + 0.12f, 98, 0.3f); }
static void quak(Synth &s, float x) { for (int i = 0; i < 2; i++) s.ton(x + i * 0.22f, 330, 0.05f, 0.01f, 0.14f, RECHTECK, GERAEUSCH, 260); }

static void ton(Synth &s, float t0, float t1) {
  float a = P1;
  spieleMelodie(s, M(MEL_WIEGE), a + 0.5f, a + 13, 0.42f, t0, t1, 0.6f);
  for (int i = 0; i < 4; i++) if (bei(a + 4 + i * 1.5f, t0, t1)) pip(s, a + 4 + i * 1.5f);
  if (bei(a + 15, t0, t1)) { s.ton(a + 15, 400, 0.08f, 0.002f, 0.08f, SINUS, GERAEUSCH, 300); }  // knack
  spieleMelodie(s, M(MEL_ENTE), a + 16, a + 33.5f, 0.36f, t0, t1, 0.7f);
  if (bei(a + 20.2f, t0, t1)) quak(s, a + 20.2f);
  a = P2;
  for (float x = ceilf(t0 / 0.4f) * 0.4f; x < t1; x += 0.4f)
    if (x > a + 1 && x < a + 12) s.ton(x, 700 + ((int)(x * 5) % 2) * 160, 0.03f, 0.003f, 0.06f, DREIECK, GERAEUSCH, 540);  // gackern
  spieleMelodie(s, M(MEL_ENTE), a + 12, a + 27.5f, 0.36f, t0, t1, 0.7f);
  a = P3;
  spieleMelodie(s, M(MEL_SPUK), a + 0.4f, a + 21, 0.4f, t0, t1, 0.55f);
  for (int i = 0; i < 3; i++) if (bei(a + 12 + i * 2, t0, t1)) s.ton(a + 12 + i * 2, 260, 0.08f, 0.005f, 0.12f, RECHTECK, GERAEUSCH, 160);  // wau
  spieleMelodie(s, M(MEL_ENTE), a + 22, a + 29.5f, 0.42f, t0, t1, 0.6f);
  a = P4;
  if (bei(a + 2.2f, t0, t1)) for (int i = 0; i < 4; i++) s.ton(a + 2.2f + i * 0.12f, 760 + (i % 2) * 140, 0.04f, 0.003f, 0.07f, DREIECK, GERAEUSCH, 600);
  if (bei(a + 7.4f, t0, t1)) s.rauschen(a + 7.4f, 1.6f, TIEFPASS, 300, 0.08f, 0.7f, 0.3f);  // schnurr
  spieleMelodie(s, M(MEL_WANDER), a + 14.5f, a + 27.5f, 0.34f, t0, t1, 0.6f);
  a = P5;
  spieleMelodie(s, M(MEL_SCHWAN), a + 0.4f, a + 13.6f, 0.34f, t0, t1, 0.6f);
  for (float x = ceilf(t0 / 0.6f) * 0.6f; x < t1; x += 0.6f)
    if (x > a + 14 && x < a + 30) s.rauschen(x, 0.4f, BANDPASS, 2400, 0.03f);  // Winterwind
  spieleMelodie(s, M(MEL_ENTE), a + 14.4f, a + 31.5f, 0.5f, t0, t1, 0.5f);
  a = P6;
  spieleMelodie(s, M(MEL_ENTE), a + 0.4f, a + 11, 0.4f, t0, t1, 0.6f);
  if (bei(a + 12.2f, t0, t1)) for (int k = 0; k < 16; k++) s.glocke(a + 12.2f + k * 0.12f, 74 + (k % 8) * 2, 0.5f);  // ein Schwan!
  spieleMelodie(s, M(MEL_SCHWAN), a + 14.6f, a + 45, 0.36f, t0, t1, 0.85f);
  endeKlang(s, a + 46.2f, t0, t1);
}

// =====================================================================
// Startbild, Vorschau, Erkennungszeichen
// =====================================================================
static void titelSzene(Gfx &g, float t) {
  teich(g, t, 3.4f);
  schwan(g, 200, 222, 1.1f, -1, t);
  entlein(g, 110, 222, 1.2f, 1, t, 1, false, 0);
  for (int i = 0; i < 3; i++) kueken(g, 40 + i * 14, 224, 1.1f, 1, t, 1, i);
}
static void titelbild(Gfx &g, float t) { titelSzene(g, t); titelRahmen(g, t, "Das hässliche Entlein", 212, SCHRIFT_M); }
static void vorschau(Gfx &g, float t) { titelSzene(g, t); }
static void symbol(Gfx &g, float x, float y, float t) {
  g.ellipse(x, y - 2, 56, 6, FARBE(0x8ECCF0));
  entlein(g, x - 26, y, 1.3f, 1, t, 1, false, 0);
  schwan(g, x + 24, y, 1.2f, -1, t);
}

const Maerchen ENTLEIN = {"Das hässliche Entlein", PDAUER, FARBE(0x8ECCF0), bild, ton, titelbild, vorschau, symbol,
                          LIED(MEL_SCHWAN, 0.34f), "H. C. Andersen"};

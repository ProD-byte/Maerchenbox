// Das Maerchen vom goldenen Taler - ein Spieluhr-Film fuer die Maerchenbox, nach Hans Fallada
// ("Geschichte vom goldenen Taler", Geschichten aus der Murkelei, 1947).
// Anna Barbara und die Grossmutter, Hans Geiz aus "Ueberall" und der Schimmel Unverzagt, die Hoellenhunde
// Neid und Gier, Kupfer, Silber und Gold putzen, das Putzwassermaennlein in der Flasche, die Ratten,
// der Taler mit dem roten Fleck - eine Traene und ein Kuss.
#include "erzaehlen.h"

// ---------- Ablauf (Sekunden) ----------
static const float P1 = 0, P2 = 24, P3 = 48, P4 = 74, P5 = 98, P6 = 124, P7 = 150, P8 = 178, P9 = 204, P10 = 232,
                   P11 = 256, P12 = 280, P13 = 310, PDAUER = 336;

// ---------- Aussehen ----------
static Aussehen geizLook() {
  Aussehen a; a.weste = FARBE(0x4A4A3A); a.hemd = FARBE(0xC8C0A0); a.hose = FARBE(0x3A3A30); a.haar = FARBE(0x6E6A4A);
  a.hut = FARBE(0x2A2A24); a.hutD = FARBE(0x1A1A16); a.feder = FARBE(0x6E6A4A); a.haut = FARBE(0xE8D890); a.wangen = false;
  return a;
}
static Aussehen gesellLook() {
  Aussehen a; a.weste = FARBE(0x3E8E6E); a.hemd = WEISS; a.hose = FARBE(0x5A4A3A); a.haar = FARBE(0xC98A3A);
  a.muetze = 1;
  return a;
}
static const Aussehen GEIZ = geizLook(), GESELL = gesellLook();
static FrauLook annaLook() {
  FrauLook l; l.kleid = FARBE(0x6E7EA8); l.schuerze = FARBE(0xF2E8D0); l.haar = FARBE(0x8E5A2A); l.frisur = 1;
  return l;
}
static FrauLook omaLook() {
  FrauLook l; l.kleid = FARBE(0x4A3A4E); l.schuerze = FARBE(0xC8B8A0); l.haar = FARBE(0xE0E0E0); l.frisur = 2; l.wangen = false;
  return l;
}
static const FrauLook ANNA = annaLook(), OMA = omaLook();

static const Farbe ERDE = FARBE(0x4E3A2A), ERDE_D = FARBE(0x3A2A1E), KUPFER = FARBE(0xB87A3A), GRUENSPAN = FARBE(0x6EA88E),
                   SILBER = FARBE(0xD8DCE4), PUTZ = FARBE(0xA8D8F0);

// =====================================================================
// Figuren
// =====================================================================
static void anna(Gfx &g, float x, float y, float s, float dir, float t, float mund, float armV = 0.4f, float lauf = -1,
                 bool weint = false, float armH = 0.2f, bool zu = false) {
  FrauPose p = fposis(ANNA, t);
  p.mund = mund;
  p.armV = armV;
  p.armH = armH;
  p.lauf = lauf;
  p.weint = weint;
  p.zu = zu || p.zu;
  frau(g, x, y, s, dir, p);
}
static void oma(Gfx &g, float x, float y, float s, float dir, float t, float mund, float armV = 0.4f, bool zu = false) {
  FrauPose p = fposis(OMA, t, 2);
  p.mund = mund;
  p.armV = armV;
  p.zu = zu || p.zu;
  frau(g, x, y, s, dir, p);
  g.linie(x - 14 * s * dir, y - 40 * s, x - 18 * s * dir, y, 2.4f * s, HOLZ_D);
}
static void geiz(Gfx &g, float x, float y, float s, float dir, float t, float mund, float armV = 0.4f, float lauf = -1,
                 float boese = 0.6f, bool nass = false) {
  Pose p = posis(GEIZ, t);
  p.mund = mund;
  p.armV = armV;
  p.lauf = lauf;
  p.boese = boese;
  p.armH = nass ? 2.8f : 0.2f;
  if (nass) p.armV = 2.8f;
  schneiderlein(g, x, y, s * 0.95f, dir, p);
  // spitze Nase und hagere Wangen
  g.ellipse(x + 15 * s * dir, y - 58 * s, 5 * s, 2.4f * s, GEIZ.haut, 255, 0.3f * dir);
  if (nass) for (int i = 0; i < 6; i++) tropfen(g, x + (i - 3) * 5 * s, y - 70 * s + fmodf(t * 30 + i * 7, 20) * s, 2 * s, PUTZ);
}
// das Putzwassermaennlein: winzig, gruenlich, mit grossem Kopf (in der Flasche oder daneben)
static void maennlein(Gfx &g, float x, float y, float s, float dir, float t, float mund, float armV = 0.4f) {
  auto PX = [&](float d) { return x + d * s * dir; };
  auto PY = [&](float d) { return y + d * s; };
  const Farbe k = FARBE(0x9AC88E), d = FARBE(0x6E9A62);
  g.kontur(1 * s, OUT);
  g.linie(PX(-3), PY(-10), PX(-4), PY(0), 2.4f * s, d);
  g.linie(PX(3), PY(-10), PX(4), PY(0), 2.4f * s, d);
  g.ellipse(PX(0), PY(-16), 6 * s, 8 * s, k);
  g.linie(PX(5), PY(-20), PX(5) + sinf(armV) * 9 * s * dir, PY(-20) + cosf(armV) * 9 * s, 2.2f * s, k);
  g.kreis(PX(0), PY(-32), 10 * s, k);
  g.konturAus();
  g.kreis(PX(3), PY(-34), 2 * s, SCHWARZ);
  g.kreis(PX(-4), PY(-34), 2 * s, SCHWARZ);
  g.ellipse(PX(6), PY(-30), 3 * s, 2 * s, d);  // Knollnase
  if (mund > 0.05f) g.ellipse(PX(1), PY(-26), 3 * s, (0.8f + mund * 2.4f) * s, FARBE(0x5A2A3A));
  else g.bogen(PX(1), PY(-28), 3 * s, 0.4f, 2.7f, 1 * s, OUT);
  for (int i = 0; i < 3; i++) g.linie(PX(-6 + i * 5), PY(-41), PX(-8 + i * 6), PY(-46), 1.2f * s, d);
}
static void flasche(Gfx &g, float x, float y, float s, float t, bool mitMann, float mund = 0) {
  g.kontur(1.2f * s, OUT);
  g.rundRechteck(x - 12 * s, y - 40 * s, 24 * s, 40 * s, 8 * s, PUTZ, 120);
  g.rechteck(x - 5 * s, y - 52 * s, 10 * s, 13 * s, PUTZ, 120);
  g.konturAus();
  g.rundRechteck(x - 11 * s, y - 22 * s, 22 * s, 21 * s, 7 * s, FARBE(0x7EC0E8), 160);
  if (mitMann) maennlein(g, x, y - 3 * s, 0.6f * s, 1, t, mund);
  g.rechteck(x - 6 * s, y - 56 * s, 12 * s, 5 * s, HOLZ);
  g.linie(x - 6 * s, y - 34 * s, x - 6 * s, y - 10 * s, 2 * s, WEISS, 140);
}
static void schimmel(Gfx &g, float x, float y, float s, float dir, float lauf, float senken = 0) {
  einhorn(g, x, y, s, lauf, dir, senken, 0, true);
  for (int i = 0; i < 3; i++) g.bogen(x - 2 * s * dir + i * 6 * s * dir, y - 44 * s, 10 * s, 1.0f, 2.1f, 1.2f * s, FARBE(0xC8C4D8));  // Rippen
}
// Hoellenhund: schwarz-rot, atmet Feuer
static void hoellenhund(Gfx &g, float x, float y, float s, float dir, float t, float feuer, float weicht = 0) {
  auto PX = [&](float d) { return x + d * s * dir; };
  auto PY = [&](float d) { return y + d * s; };
  const Farbe fell = FARBE(0x7A3A3E), rot = FARBE(0xFFD54A);
  g.kontur(1.2f * s, OUT);
  for (int i = 0; i < 4; i++) g.linie(PX(i < 2 ? -12 + i * 6 : 8 + (i - 2) * 6), PY(-14), PX(i < 2 ? -12 + i * 6 : 8 + (i - 2) * 6), PY(-2), 4 * s, fell);
  g.kurve(PX(-15), PY(-18), PX(-26), PY(-26), PX(-30), PY(-34 + sinf(t * 9) * 4), 3 * s, fell);
  g.ellipse(PX(0), PY(-18), 17 * s, 8 * s, fell);
  float kx = PX(17 - weicht * 6), ky = PY(-26 + weicht * 4);
  g.kreis(kx, ky, 8 * s, fell);
  g.ellipse(kx + 8 * s * dir, ky + 2 * s, 6 * s, 4.5f * s, fell);
  g.konturAus();
  float o[] = {kx - 4 * s * dir, ky - 6 * s, kx - 2 * s * dir, ky - 16 * s, kx + 3 * s * dir, ky - 7 * s};
  g.polygon(o, 3, fell);
  g.kreis(kx + 3 * s * dir, ky - 2 * s, 1.8f * s, rot);
  if (feuer > 0) for (int i = 0; i < 4; i++) {
    float k = fmodf(t * 2.4f + i * 0.25f, 1);
    g.kreis(kx + (14 + k * 30) * s * dir, ky + 4 * s - k * 6 * s, (3 + k * 6) * s, k < 0.5f ? FARBE(0xFFD54A) : FARBE(0xFF7A2A),
            (uint8_t)(230 * (1 - k) * feuer));
  }
}
static void ratte(Gfx &g, float x, float y, float s, float dir, float lauf) {
  auto PX = [&](float d) { return x + d * s * dir; };
  auto PY = [&](float d) { return y + d * s; };
  float hop = fabsf(sinf(lauf * 6.28f)) * 2;
  g.kurve(PX(-10), PY(-5 - hop), PX(-22), PY(-2), PX(-28), PY(-8), 1.4f * s, FARBE(0xE8A0A8));
  g.kontur(1 * s, OUT);
  g.ellipse(PX(0), PY(-6 - hop), 11 * s, 6 * s, FARBE(0x7A7A80));
  g.ellipse(PX(10), PY(-7 - hop), 6 * s, 4 * s, FARBE(0x7A7A80), 255, 0.3f * dir);
  g.konturAus();
  g.kreis(PX(6), PY(-12 - hop), 3 * s, FARBE(0xE8A0A8));
  g.kreis(PX(12), PY(-9 - hop), 1.2f * s, SCHWARZ);
}
// der goldene Taler; fleck = roter Fleck 0..1
static void taler(Gfx &g, float x, float y, float r, float t, float fleck = 0, float glanz = 1) {
  if (glanz > 0) g.kreis(x, y, r * 2.2f, FARBE(0xFFF0A0), (uint8_t)(50 * glanz));
  g.kontur(1.6f, FARBE(0x9A6A1E));
  g.kreis(x, y, r, GOLD);
  g.konturAus();
  g.kreis(x, y, r * 0.78f, FARBE(0xFFE070));
  g.stern(x, y, r * 0.5f, r * 0.22f, 5, 0, FARBE(0xE0A82A));
  if (fleck > 0) g.ellipse(x + r * 0.3f, y + r * 0.2f, r * 0.28f, r * 0.22f, FARBE(0xC0392B), (uint8_t)(255 * fleck));
  if (glanz > 0) g.stern(x - r * 0.5f, y - r * 0.5f, r * 0.5f, r * 0.12f, 4, t * 2, WEISS, (uint8_t)(220 * glanz));
}
static void muenzen(Gfx &g, float x, float y, int n, Farbe c, Farbe c2, float s = 1) {
  for (int i = 0; i < n; i++) {
    float mx = x + (i % 7) * 7 * s - (i / 7) * 3 * s, my = y - (i / 7) * 5 * s;
    g.ellipse(mx, my, 5 * s, 2.4f * s, (i * 7) % 3 ? c : c2);
  }
}

// =====================================================================
// Orte
// =====================================================================
static void omaStube(Gfx &g, float t, float nacht = 0, float kerze = 1) {
  g.verlauf(0, 200, mischeFarbe(FARBE(0xC8A47A), FARBE(0x3A3050), nacht), mischeFarbe(FARBE(0xA8845A), FARBE(0x2A2240), nacht));
  g.rechteck(0, 200, 284, 40, mischeFarbe(FARBE(0x7A5A3A), FARBE(0x3A2E28), nacht));
  g.kontur(1.6f, OUT);
  g.rundRechteck(176, 50, 60, 54, 4, HOLZ);
  g.konturAus();
  g.rundRechteck(181, 55, 50, 44, 2, mischeFarbe(FARBE(0x9FD8FF), FARBE(0x22285A), nacht));
  for (int i = 0; i < 6; i++) g.kreis(186 + zufall(i) * 40, 60 + fmodf(t * 8 + i * 7, 36), 1.4f, WEISS);
  g.rechteck(204, 55, 3, 44, HOLZ);
  // Bett
  g.kontur(1.4f, OUT);
  g.rundRechteck(20, 170, 110, 32, 4, HOLZ);
  g.konturAus();
  g.rundRechteck(24, 160, 30, 14, 6, WEISS);
  g.rundRechteck(46, 164, 80, 16, 6, FARBE(0x8E6E8E));
  // Tisch mit Kerze
  g.rechteck(150, 170, 50, 5, HOLZ);
  g.rechteck(156, 175, 4, 27, HOLZ_D);
  g.rechteck(190, 175, 4, 27, HOLZ_D);
  g.rechteck(172, 156, 6, 14, WEISS);
  if (kerze > 0) {
    g.ellipse(175, 150, 3, 5 + sinf(t * 9), FARBE(0xFFB83D), (uint8_t)(255 * kerze));
    g.kreis(175, 152, 40, FARBE(0xFFD27A), (uint8_t)(40 * kerze));
  }
}

static void kirchhof(Gfx &g, float t) {
  himmel(g, 0.5f, t, 200);
  g.kontur(1.6f, OUT);
  g.rechteck(186, 80, 40, 122, FARBE(0xA8A8B8));
  float d[] = {180, 82, 206, 30, 232, 82};
  g.polygon(d, 3, FARBE(0x5A5A6E));
  g.konturAus();
  g.linie(206, 18, 206, 32, 2, OUT);
  g.linie(200, 24, 212, 24, 2, OUT);
  g.rechteck(0, 200, 284, 40, FARBE(0xF2F6FF));
  for (int i = 0; i < 3; i++) {  // Grabkreuze im Schnee
    float x = 40 + i * 34;
    g.linie(x, 170, x, 202, 3, HOLZ_D);
    g.linie(x - 9, 180, x + 9, 180, 3, HOLZ_D);
    g.ellipse(x, 170, 4, 2, WEISS);
  }
  g.rechteck(0, 188, 140, 4, STEIN_D);
  for (int i = 0; i < 30; i++) g.kreis(fmodf(zufall(i) * 300 + t * 10, 300) - 8, fmodf(zufall(i + 40) * 240 + t * 22, 250) - 6, 1.6f, WEISS, 220);
}

static void kistenSchlitten(Gfx &g, float x, float y, float s, float dir, float t, float lauf, bool pferd = true) {
  if (pferd) schimmel(g, x + 58 * s * dir, y, 0.6f * s, dir, lauf);
  g.kontur(1.4f * s, OUT);
  g.rechteck(x - 28 * s, y - 32 * s, 56 * s, 24 * s, FARBE(0x8E7A5A));
  g.konturAus();
  for (int i = 0; i < 3; i++) g.linie(x - 28 * s, y - 26 * s + i * 7 * s, x + 28 * s, y - 26 * s + i * 7 * s, 1.2f * s, FARBE(0x6E5A3E));
  g.linie(x - 32 * s, y - 3 * s, x + 32 * s, y - 3 * s, 2.4f * s, HOLZ_D);
  if (pferd) g.linie(x + 28 * s * dir, y - 24 * s, x + 46 * s * dir, y - 34 * s, 1.4f * s, HOLZ_D);
}

// die Halle unter der Erde: Gerumpel, Hoellenhunde am Feuer
static void halle(Gfx &g, float t, float hell = 1) {
  g.verlauf(0, 210, mischeFarbe(ERDE_D, FARBE(0x1E1612), 1 - hell), mischeFarbe(ERDE, FARBE(0x2E2218), 1 - hell));
  for (int i = 0; i < 12; i++) g.ellipse(zufall(i + 3) * 284, zufall(i + 9) * 190, 14, 8, ERDE_D, 160);
  g.rechteck(0, 206, 284, 34, FARBE(0x3A2A1E));
  // Wurzeln von oben
  for (int i = 0; i < 6; i++) g.kurve(30 + i * 46, 0, 36 + i * 46, 20, 28 + i * 46, 40, 2, FARBE(0x6E5038));
  // Gerumpel: Schuhe, Matratze, Flaschen, Papier
  g.kontur(1.2f, OUT);
  g.rundRechteck(10, 176, 70, 18, 6, FARBE(0x9A8A6E));
  g.rundRechteck(16, 160, 60, 18, 6, FARBE(0x8E7A9A));
  g.konturAus();
  for (int i = 0; i < 4; i++) g.ellipse(30 + i * 12, 200, 6, 3, FARBE(0x4A2E1E));
  for (int i = 0; i < 5; i++) g.rundRechteck(222 + i * 10, 182 - (i & 1) * 6, 7, 20, 3, i & 1 ? FARBE(0x5A8E5A) : FARBE(0x8E6A3A), 200);
  for (int i = 0; i < 6; i++) g.rechteck(200 + zufall(i) * 60, 198 + zufall(i + 4) * 6, 12, 8, FARBE(0xE8E0C8), 220);
  // Feuerschein
  g.kreis(142, 196, 70, FARBE(0xFF8A3A), 30);
}

// =====================================================================
// Szenen
// =====================================================================
// 1) Bei der Grossmutter: "Wenn wir bloss den goldenen Taler haetten!"
static void szeneOma(Gfx &g, float u) {
  omaStube(g, u);
  oma(g, 170, 222, 0.85f, -1, u, (u > 6.4f && u < 11) || (u > 17.4f && u < 22) ? 0.5f + 0.2f * sinf(u * 8) : 0, 0.8f);
  anna(g, 100, 222, 0.75f, 1, u, (u > 11.4f && u < 17) ? 0.5f : 0.2f, u > 11.4f && u < 17 ? 1.4f : 0.4f);
  g.kameraAus();
  vers(g, {"Anna Barbara hatte keine Eltern", "mehr. Sie lebte bei ihrer", "steinalten Großmutter."}, aufzu(u, 0.4f, 6));
  blase(g, 180, 50, {"Ja, Kind, wenn wir bloß den", "goldenen Taler hätten!"}, aufzu(u, 6.4f, 11), 170, 140);
  blase(g, 110, 50, {"Großmutter, mich friert,", "und das Brot ist alle..."}, aufzu(u, 11.4f, 17), 100, 150);
  blase(g, 180, 50, {"Ja, Kind, wenn wir bloß den", "goldenen Taler hätten!"}, aufzu(u, 17.4f, 23.6f), 170, 140);
}

// 2) Die Grossmutter schlaeft ein (nur angedeutet) und gibt ihr den Auftrag
static void szeneAbschied(Gfx &g, float u) {
  float kerze = 1 - seg(u, 17, 19);
  omaStube(g, u, 0.8f, kerze);
  // die Grossmutter liegt im Bett
  g.kreis(40, 158, 9, OMA.haut);
  g.ellipse(36, 152, 9, 5, OMA.haar);
  g.bogen(43, 158, 2, 3.5f, 5.9f, 1, OUT);
  if (u > 4 && u < 14) g.ellipse(47, 163, 2, 1 + 1.4f * fabsf(sinf(u * 7)), FARBE(0x7A2A2A));
  anna(g, 130, 222, 0.8f, -1, u, 0, 1.2f, -1, u > 14);
  if (u > 19) for (int i = 0; i < 3; i++) g.stern(40 + i * 14, 120 - fmodf(u * 6 + i * 10, 40), 3, 1.2f, 5, u, GOLD, 200);
  g.kameraAus();
  vers(g, {"In einer kalten Winternacht", "wurde die Großmutter sehr schwach."}, aufzu(u, 0.4f, 4.4f));
  blase(g, 110, 60, {"Begrab mich bei deinen Eltern.", "Dann geh in die Welt und diene", "um den goldenen Taler -"}, aufzu(u, 4.6f, 9.6f), 50, 150);
  blase(g, 110, 60, {"um den dienst du so lange,", "bis du ihn bekommst."}, aufzu(u, 9.8f, 14), 50, 150);
  vers(g, {"Dann schlief die Großmutter ein", "und wachte nicht wieder auf."}, aufzu(u, 15, 23.6f));
}

// 3) Am Kirchhof: Hans Geiz mit dem Schimmel Unverzagt
static void szeneGeiz(Gfx &g, float u) {
  kirchhof(g, u);
  float k = seg(u, 4, 9);
  kistenSchlitten(g, lerp(340, 166, k), 222, 1, -1, u, k < 1 ? fmodf(u * 1.2f, 1) : -1);
  geiz(g, k < 1 ? lerp(340, 166, k) : 196, k < 1 ? 196 : 222, k < 1 ? 0.6f : 0.85f, -1, u,
       (u > 9.4f && u < 14) || (u > 14.4f && u < 20) ? 0.5f + 0.2f * sinf(u * 8) : 0, u > 14.4f && u < 20 ? 1.6f : 0.4f);
  anna(g, 110, 222, 0.8f, 1, u, u > 20.4f ? 0.4f : 0, 0.4f, -1, u < 4);
  g.kameraAus();
  vers(g, {"Als Anna vom Grab kam, hielt vor", "dem Kirchhof ein seltsamer Schlitten:", "eine alte Futterkiste auf Kufen."}, aufzu(u, 0.4f, 9));
  blase(g, 190, 50, {"Ich heiße Hans Geiz und wohne", "in der großen Ortschaft Überall."}, aufzu(u, 9.4f, 14), 196, 140);
  blase(g, 190, 50, {"Diene mir drei Jahre, dann", "bekommst du den goldenen Taler!"}, aufzu(u, 14.4f, 20), 196, 140);
  vers(g, {"Der dürre Schimmel hieß Unverzagt.", "Anna stieg in die Kiste."}, aufzu(u, 20.4f, 25.8f));
}

// 4) Die Fahrt nach "Ueberall" - hinab ins Erdloch
static void szeneFahrt(Gfx &g, float u) {
  if (u < 14) {
    himmel(g, 0.6f, u, 240);
    for (int i = 0; i < 60; i++) {
      float x = fmodf(zufall(i) * 300 - u * 120, 300);
      if (x < 0) x += 300;
      g.kreis(x - 8, fmodf(zufall(i + 40) * 240 + u * 30, 240), 1.8f, WEISS, 220);
    }
    g.rechteck(0, 200, 284, 40, FARBE(0xF2F6FF));
    kistenSchlitten(g, 120, 222, 1, 1, u, fmodf(u * 1.2f, 1));
    geiz(g, 128, 196, 0.55f, 1, u, u > 6.4f && u < 9.6f ? 0.5f : 0, 1.4f);
    anna(g, 108, 194, 0.5f, 1, u, u > 4 && u < 6.4f ? 0.4f : 0, 0.4f);
    g.kameraAus();
    vers(g, {"Sie fuhren und fuhren durch", "das Schneetreiben, ohne Ende."}, aufzu(u, 0.4f, 4));
    blase(g, 100, 60, {"Wo ist denn Überall?"}, aufzu(u, 4.2f, 6.4f), 108, 150);
    blase(g, 160, 60, {"Überall!"}, aufzu(u, 6.6f, 9.6f), 130, 150);
    vers(g, {"Endlich hielt Hans Geiz an.", "Er stellte den Schimmel in den Schnee."}, aufzu(u, 9.8f, 13.8f));
    return;
  }
  float v = u - 14;
  himmel(g, 0.6f, u, 200);
  g.rechteck(0, 200, 284, 40, FARBE(0xF2F6FF));
  g.ellipse(150, 214, 30, 9, FARBE(0x1E1612));  // das Erdloch
  schimmel(g, 236, 222, 0.6f, -1, -1, 0.6f);
  float spr = seg(v, 4, 6);
  if (v < 3) geiz(g, 150, 222 + seg(v, 1, 3) * 50, 0.8f, 1, u, 0, 2.6f);
  if (spr < 1) anna(g, lerp(90, 150, spr), lerp(222, 260, hinein(spr)) - huepf(spr) * 20, 0.8f, 1, u, 0.6f, 2.6f, spr > 0 ? 0.3f : -1, false, 2.6f);
  g.kameraAus();
  vers(g, {"Geiz sprang in ein Erdloch.", "Das war sein Haus. Anna sprang", "ihm mutig nach."}, aufzu(v, 0.4f, 9.6f));
}

// 5) Die Halle: Neid und Gier, Gerumpel und ein karges Abendessen
static void szeneHalle(Gfx &g, float u) {
  halle(g, u);
  hoellenhund(g, 110, 222, 1, 1, u, 1);
  hoellenhund(g, 190, 222, 1, -1, u + 0.5f, 1);
  bool essen = u > 14;
  geiz(g, essen ? 236 : 150, 222, 0.85f, -1, u, (u > 6.4f && u < 13) ? 0.5f + 0.2f * sinf(u * 8) : 0, u > 6.4f && u < 13 ? 1.6f : 0.4f);
  anna(g, essen ? 150 : 60, 222, 0.8f, 1, u, essen ? 0.3f : 0.5f, essen ? 1.4f : 0.4f);
  if (essen) {  // schimmliges Brot, Speckschwarte, halb fauler Apfel
    g.ellipse(168, 176, 8, 5, FARBE(0xA88A5A));
    g.kreis(166, 174, 2, FARBE(0x8EAA6E));
    g.ellipse(182, 178, 6, 2, FARBE(0xE8C8A8));
    g.kreis(176, 168, 5, FARBE(0xC0503A));
    g.kreis(178, 166, 2.4f, FARBE(0x7A5A3A));
  }
  g.kameraAus();
  vers(g, {"In der Halle heizten zwei Höllen-", "hunde mit ihrem Atem: Neid und Gier."}, aufzu(u, 0.4f, 6));
  blase(g, 170, 50, {"Hier kommt nichts weg -", "denn nichts wird vergessen", "auf dieser Welt."}, aufzu(u, 6.4f, 13), 150, 140);
  vers(g, {"Zum Abendbrot gab es schimmliges", "Brot, eine Speckschwarte und einen", "halb faulen Apfel."}, aufzu(u, 14, 25.6f));
}

// 6) Die Arbeit: Kupfergeld blank putzen
static void szeneKupfer(Gfx &g, float u) {
  halle(g, u, 0.8f);
  hoellenhund(g, 236, 222, 0.8f, -1, u, 0.5f);
  int blank = (int)(seg(u, 10, 24) * 20);
  muenzen(g, 60, 214, 20 - blank, GRUENSPAN, FARBE(0x5A8E7A));
  muenzen(g, 160, 214, blank, KUPFER, FARBE(0xE0A060));
  geiz(g, 200, 222, 0.85f, -1, u, (u > 0.4f && u < 10) ? 0.5f + 0.2f * sinf(u * 8) : 0, u < 10 ? 1.6f : 0.4f);
  anna(g, 120, 222, 0.8f, 1, u, 0, u > 10 ? 1.2f + sinf(u * 8) * 0.3f : 0.4f);
  flasche(g, 96, 214, 0.5f, u, false);
  if (u > 10) glitzern(g, 170, 200, 20, u, 3);
  g.kameraAus();
  blase(g, 170, 50, {"Putz das Kupfergeld blank!", "Ein Jahr Arbeit bringt ein", "Drittel vom goldenen Taler."}, aufzu(u, 0.4f, 6), 200, 140);
  blase(g, 170, 50, {"Und iss Brot und Milch nie", "ganz auf - sonst wächst", "nichts nach!"}, aufzu(u, 6.4f, 10), 200, 140);
  vers(g, {"Mit Tuch und Putzwasser rieb Anna", "den grünen Grünspan von den", "Münzen, Tag für Tag."}, aufzu(u, 10.4f, 25.6f));
}

// 7) Das Putzwassermaennlein
static void szeneMaennlein(Gfx &g, float u) {
  halle(g, u, 0.8f);
  muenzen(g, 150, 214, 18, KUPFER, FARBE(0xE0A060));
  bool draussen = u > 8;
  if (!draussen) flasche(g, 170, 214, 1.1f, u, true, u > 2.4f && u < 7 ? 0.6f : 0);
  else {
    flasche(g, 200, 214, 0.8f, u, false);
    maennlein(g, 166, 214, 1, -1, u, (u > 8.4f && u < 12) || (u > 18.4f && u < 24) ? 0.5f + 0.3f * sinf(u * 9) : 0,
              u > 8 && u < 12 ? 2 : 0.4f);
  }
  bool gibt = u > 12 && u < 14;
  anna(g, 100, 222, 0.8f, 1, u, (u > 14.4f && u < 18) || u > 24.4f ? 0.5f + 0.2f * sinf(u * 8) : 0.2f, gibt ? 1.4f : 0.4f, -1, false, 0.2f, u > 24.4f);
  if (gibt) g.ellipse(lerp(118, 160, seg(u, 12, 14)), 180, 5, 3, FARBE(0xC8A060));
  g.kameraAus();
  vers(g, {"Im Fläschchen mit dem Putzwasser", "saß ein winziges Männlein..."}, aufzu(u, 0.4f, 2.2f));
  blase(g, 190, 60, {"Gib mir auch zu essen", "und zu trinken! Mehr! Mehr!"}, aufzu(u, 2.4f, 12), 170, 160);
  blase(g, 110, 60, {"Ich heiße Anna Barbara."}, aufzu(u, 14.4f, 18), 100, 150);
  blase(g, 190, 60, {"Das ist ein schrecklich dummer", "Name. Ich werde dich", "Liebste nennen."}, aufzu(u, 18.4f, 24), 166, 160);
  vers(g, {"Da musste Anna lachen - und", "das Männlein schimpfte fürchterlich."}, aufzu(u, 24.4f, 27.8f));
}

// 8) Zweites Jahr: Silber - die Ratten rauben das Maennlein
static void szeneRatten(Gfx &g, float u) {
  halle(g, u, 0.8f);
  muenzen(g, 30, 214, 20, SILBER, WEISS);
  float rk = seg(u, 6, 16);
  bool gerettet = u > 15;
  float rx = lerp(150, 280, rk);
  for (int i = 0; i < 3; i++) ratte(g, rx + i * 18 - (gerettet ? (u - 15) * 60 : 0), 222, 1, 1, u + i * 0.3f);
  if (!gerettet && u > 6) {
    maennlein(g, rx + 4, 206, 0.7f, 1, u, 0.9f, 2.6f);
    g.ellipse(rx + 24, 214, 6, 3, FARBE(0xC8A060));
  }
  if (gerettet) maennlein(g, 140, 222, 0.9f, -1, u, u > 18.4f && u < 24 ? 0.5f : 0.2f);
  else if (u < 6) maennlein(g, 150, 222, 0.9f, -1, u, 0);
  bool wirft = u > 9 && u < 15;
  anna(g, 90, 222, 0.8f, 1, u, wirft ? 0.7f : 0.3f, wirft ? 2.2f + sinf(u * 10) * 0.6f : 0.4f, wirft ? fmodf(u * 1.4f, 1) : -1);
  if (wirft) for (int i = 0; i < 3; i++) {
    float k = fmodf(u * 1.6f + i * 0.33f, 1);
    g.ellipse(lerp(110, rx, k), 170 + k * 40 - huepf(k) * 30, 4, 2, SILBER);
  }
  g.kameraAus();
  vers(g, {"Im zweiten Jahr putzte Anna", "Silbergeld."}, aufzu(u, 0.4f, 5.6f));
  vers(g, {"Da schleppten die Ratten das", "Männlein und das letzte Brot fort!"}, aufzu(u, 6, 11));
  vers(g, {"Anna warf mit Silbermünzen nach", "ihnen - bis sie es losließen."}, aufzu(u, 11.4f, 18));
  blase(g, 170, 60, {"Du hast mich gerettet.", "Lass uns Frieden schließen."}, aufzu(u, 18.4f, 25.6f), 140, 160);
}

// 9) Drittes Jahr: der Goldkeller - welcher ist der goldene Taler?
static void szeneGold(Gfx &g, float u) {
  halle(g, u, 0.9f);
  for (int i = 0; i < 4; i++) {  // Tonnen voller Gold
    float x = 40 + i * 64;
    g.kontur(1.4f, OUT);
    g.rundRechteck(x - 18, 160, 36, 44, 6, HOLZ);
    g.konturAus();
    g.ellipse(x, 160, 18, 5, GOLD);
    for (int k = 0; k < 4; k++) g.kreis(x - 10 + k * 7, 158 - (k & 1) * 3, 3, FARBE(0xFFE070));
  }
  geiz(g, 236, 222, 0.85f, -1, u, (u > 0.4f && u < 6) ? 0.5f + 0.2f * sinf(u * 8) : 0, u < 6 ? 1.4f : 0.4f);
  bool krumen = u > 16;
  anna(g, 110, 222, 0.8f, 1, u, (u > 10.4f && u < 15) ? 0.4f : 0.2f, krumen && u < 19 ? 1.4f : 1 + sinf(u * 3) * 0.4f, -1, u > 10 && u < 16);
  maennlein(g, 156, 222, 0.9f, -1, u, (u > 6.4f && u < 10) ? 0.5f : u > 19 && u < 20.5f ? 0.9f : 0.1f);
  if (krumen && u < 19) for (int i = 0; i < 4; i++) g.kreis(lerp(126, 150, seg(u, 16, 19)) + i * 2, 186 + i, 1.2f, FARBE(0xC8A060));
  float st = seg(u, 20, 21.5f);
  if (st > 0) {
    float x = lerp(150, 134, st), y = lerp(196, 210, st) - huepf(st) * 16;
    if (u < 23) g.kreis(x, y, 4, FARBE(0xE8C84A));
    else taler(g, 134, 200, 10 * sanft(seg(u, 23, 24.4f)), u, 1, sanft(seg(u, 23, 24.4f)));
    puff(g, 134, 200, seg(u, 23, 24.4f));
  }
  g.kameraAus();
  blase(g, 180, 50, {"Wenn ihn dein Herz nicht", "erkennt, so hast du ihn auch", "nicht verdient."}, aufzu(u, 0.4f, 6), 236, 140);
  blase(g, 170, 60, {"Er hat ihn, und er", "hat ihn nicht."}, aufzu(u, 6.4f, 10), 156, 170);
  vers(g, {"Anna suchte und suchte - ihr Herz", "sprach bei keiner Münze."}, aufzu(u, 10.4f, 15.6f));
  vers(g, {"Da gab sie dem hungrigen Männlein", "ihre letzten Krümel. Es spuckte", "einen gelben Stein aus..."}, aufzu(u, 16, 23));
  vers(g, {"...und der wurde zum goldenen Taler!"}, aufzu(u, 23.4f, 27.8f));
}

// 10) Der rote Fleck - und die Traene
static void szeneFleck(Gfx &g, float u) {
  halle(g, u, 0.9f);
  float fleck = 1 - seg(u, 15, 17);
  bool putzt = u > 2 && u < 10;
  anna(g, 110, 222, 0.8f, 1, u, (u > 10.4f && u < 14.6f) ? 0.4f : u > 17 ? 0.6f : 0.2f, putzt ? 1.4f + sinf(u * 10) * 0.2f : 1.4f, -1,
       u > 12 && u < 17);
  maennlein(g, 176, 222, 0.9f, -1, u, 0.1f);
  float tx = 136, ty = 176;
  taler(g, tx, ty, 10, u, fleck, u > 17 ? 1 + 0.3f * sinf(u * 3) : 0.4f);
  if (putzt) g.ellipse(tx + sinf(u * 10) * 4, ty + 3, 7, 4, WEISS, 220);
  float tr = seg(u, 14, 15);
  if (tr > 0 && tr < 1) tropfen(g, 126, lerp(150, 176, tr), 2.4f, FARBE(0x7EC0EE));
  if (u > 17) glitzern(g, tx, ty, 26, u, 6);
  g.kameraAus();
  vers(g, {"Aber der Taler hatte einen roten", "Fleck. Selbst das schärfste", "Putzwasser half nicht."}, aufzu(u, 0.4f, 10));
  blase(g, 110, 60, {"Ich schaffe es nicht. Der Fleck", "lässt sich nicht wegreiben."}, aufzu(u, 10.4f, 14.6f), 110, 150);
  vers(g, {"Da fiel eine Träne auf den Taler -", "und er glänzte ohne jeden Fleck."}, aufzu(u, 15, 23.6f));
}

// 11) Der Kuss: das Maennlein wird ein junger Mann
static void szeneKuss(Gfx &g, float u) {
  halle(g, u, 0.9f);
  bool mann = u > 11;
  anna(g, 110, 222, 0.8f, 1, u, u > 4.4f && u < 10 ? 0.4f : 0.3f, 0.6f, -1, false, 0.2f, u > 9 && u < 11);
  taler(g, 96, 180, 6, u, 0, 0.6f);
  if (!mann) maennlein(g, 146, 196 - seg(u, 8, 10) * 20, 0.9f, -1, u, 0.1f);
  else {
    Pose p = posis(GESELL, u);
    p.mund = u > 13.4f ? 0.5f + 0.2f * sinf(u * 8) : 0.4f;
    p.armV = 1.2f;
    schneiderlein(g, 160, 222, 0.9f, -1, p);
    herzen(g, 136, 150, u, 4, 20);
  }
  puff(g, 150, 170, seg(u, 11, 12.6f));
  g.kameraAus();
  blase(g, 110, 60, {"Mein Geselle bist du ja doch,", "wenn du auch greulich", "anzusehen bist."}, aufzu(u, 0.4f, 5.6f), 110, 150);
  vers(g, {"Und sie gab dem Männlein", "einen Kuss..."}, aufzu(u, 6, 10.6f));
  blase(g, 180, 50, {"Du hast mich erlöst,", "Anna Barbara!"}, aufzu(u, 13.4f, 17.6f), 160, 130);
  vers(g, {"Auch er hatte einst um den Taler", "gedient - und war zur Strafe für", "seine Ungeduld in die Flasche gekommen."}, aufzu(u, 18, 23.8f));
}

// 12) Die Flucht: Putzwasser gegen Geiz und Hunde, Unverzagt hilft
static void szeneFlucht(Gfx &g, float u) {
  if (u < 18) {
    halle(g, u);
    bool nass = u > 9;
    float weicht = seg(u, 12, 14);
    hoellenhund(g, 220 + weicht * 30, 222, 0.9f, -1, u, 1 - weicht, weicht);
    geiz(g, 190 + (nass ? seg(u, 9, 11) * 20 : 0), 222, 0.85f, -1, u, (u > 4.4f && u < 8.6f) || nass ? 0.6f + 0.2f * sinf(u * 9) : 0,
         u > 4.4f && u < 9 ? 2.2f : 0.4f, -1, 1, nass);
    Pose p = posis(GESELL, u);
    p.armV = u > 8 && u < 10 ? 1.6f : 0.4f;
    schneiderlein(g, 140, 222, 0.85f, 1, p);
    if (u > 8 && u < 10) flasche(g, 156, 180, 0.4f, u, false);
    if (u > 8.6f && u < 10) for (int i = 0; i < 6; i++) tropfen(g, 160 + seg(u, 8.6f, 10) * 30 + i * 4, 166 + i * 2, 2, PUTZ);
    anna(g, 90, 222, 0.8f, 1, u, 0.3f, u < 4 ? 2.4f : 0.4f);
    if (u < 4) taler(g, 104, 160, 6, u, 0, 1);
    g.kameraAus();
    vers(g, {"Anna klopfte mit dem Taler", "an die Tür - und sie sprang auf."}, aufzu(u, 0.4f, 4.2f));
    blase(g, 190, 50, {"Halt! Der Bursche bleibt hier!"}, aufzu(u, 4.4f, 8.6f), 190, 140);
    vers(g, {"Da spritzte er ihm Putzwasser ins", "Gesicht - und auch die Hunde", "wichen vor dem Wasser zurück."}, aufzu(u, 9, 17.6f));
    return;
  }
  float v = u - 18;
  // im Schacht: der Schimmel laesst seinen Schweif hinab
  g.verlauf(0, 240, FARBE(0x3A2A1E), FARBE(0x2A1E16));
  g.rechteck(90, 0, 104, 240, FARBE(0x1E1612));
  himmel(g, 0.6f, u, 40);
  g.rechteck(0, 36, 90, 8, FARBE(0xF2F6FF));
  g.rechteck(194, 36, 90, 8, FARBE(0xF2F6FF));
  schimmel(g, 214, 40, 0.6f, -1, -1, 0.8f);
  float k = seg(v, 2, 10);
  g.linie(196, 26, 160, 40 + 140 * (1 - k * 0.6f), 2.4f, WEISS);  // das Pferdehaar
  anna(g, 150, 230 - k * 120, 0.7f, 1, u, 0.3f, 2.8f, -1, false, 2.8f);
  Pose p = posis(GESELL, u);
  p.armV = 2.8f; p.armH = 2.8f;
  schneiderlein(g, 168, 250 - k * 110, 0.7f, -1, p);
  g.kameraAus();
  vers(g, {"Im Schacht ließ der Schimmel", "Unverzagt ein langes Haar hinab", "und zog sie hinauf."}, aufzu(v, 0.4f, 11.6f));
}

// 13) Heim ins Haus der Grossmutter
static void szeneHeim(Gfx &g, float u) {
  himmel(g, 0, u, 200);
  sonne(g, 240, 34, 11, u);
  boden(g, 200);
  g.kontur(1.8f, OUT);
  g.rechteck(150, 110, 110, 92, FARBE(0xF0DCC0));
  float d[] = {140, 112, 205, 64, 270, 112};
  g.polygon(d, 3, FARBE(0xC0503A));
  g.konturAus();
  g.rundRechteck(196, 150, 26, 52, 8, HOLZ_D);
  laubbaum(g, 40, 206, 0.8f, FARBE(0x5DAE48));
  for (int i = 0; i < 5; i++) g.kreis(24 + i * 8, 140 + (i & 1) * 8, 3, FARBE(0xE8473D));
  schimmel(g, 66, 222, 0.6f, 1, -1, 0.5f + 0.3f * sinf(u));
  Pose p = posis(GESELL, u);
  p.armV = 1.2f;
  p.mund = 0.4f;
  schneiderlein(g, 160, 222, 0.88f, -1, p);
  anna(g, 124, 222, 0.82f, 1, u, 0.4f, 1.2f);
  taler(g, 142, 120, 9, u, 0, 1);
  herzen(g, 142, 160, u, 5, 26);
  g.kameraAus();
  vers(g, {"Sie gingen heim ins Haus der", "Großmutter und lebten glücklich -", "und der Schimmel blieb bei ihnen."}, aufzu(u, 0.4f, 10));
  vers(g, {"Denn wer den goldenen Taler ohne", "Fleck hat, der ist immer glücklich."}, aufzu(u, 10.4f, 18));
  endeBild(g, u, 20, 142, 170);
}

static void bild(Gfx &g, float t) {
  if (t < P2) szeneOma(g, t - P1);
  else if (t < P3) szeneAbschied(g, t - P2);
  else if (t < P4) szeneGeiz(g, t - P3);
  else if (t < P5) szeneFahrt(g, t - P4);
  else if (t < P6) szeneHalle(g, t - P5);
  else if (t < P7) szeneKupfer(g, t - P6);
  else if (t < P8) szeneMaennlein(g, t - P7);
  else if (t < P9) szeneRatten(g, t - P8);
  else if (t < P10) szeneGold(g, t - P9);
  else if (t < P11) szeneFleck(g, t - P10);
  else if (t < P12) szeneKuss(g, t - P11);
  else if (t < P13) szeneFlucht(g, t - P12);
  else szeneHeim(g, t - P13);
}

// =====================================================================
// Musik
// =====================================================================
// Talerlied (A-Dur, schlicht wie ein Volkslied)
static const Ton3 MEL_TALER[] = {
  {69, 1, 45}, {73, .5, 0}, {76, .5, 0}, {76, 1, 0}, {74, .5, 50}, {73, .5, 0}, {71, 1, 52}, {71, .5, 0}, {73, .5, 0},
  {74, 1, 50}, {76, 1, 0}, {73, 2, 45}, {76, 1, 45}, {78, .5, 0}, {76, .5, 0}, {74, 1, 50}, {73, .5, 0}, {71, .5, 0},
  {69, .5, 52}, {71, .5, 0}, {73, .5, 0}, {71, .5, 0}, {69, 2, 45}};
// Geizens Weise (a-Moll, knausrig getupft)
static const Ton3 MEL_GEIZ[] = {
  {69, .5, 45}, {0, .5, 0}, {68, .5, 0}, {0, .5, 0}, {69, .5, 0}, {72, .5, 0}, {71, 1, 0},
  {67, .5, 43}, {0, .5, 0}, {66, .5, 0}, {0, .5, 0}, {67, .5, 0}, {71, .5, 0}, {69, 1, 45}};
static void klimper(Synth &s, float x, int m) { s.note(x, m, 0.3f); s.ton(x, Synth::mtof(m + 24), 0.04f, 0.002f, 0.08f, SINUS, GERAEUSCH); }
static void knurr(Synth &s, float x) { s.rauschen(x, 0.6f, TIEFPASS, 300, 0.12f, 2, 0.05f); }
static void ton(Synth &s, float t0, float t1) {
  float a = P1;
  spieleMelodie(s, M(MEL_TALER), a + 0.5f, a + 23.6f, 0.4f, t0, t1, 0.6f);
  a = P2;
  spieleMelodie(s, M(MEL_WIEGE), a + 0.4f, a + 23.6f, 0.5f, t0, t1, 0.45f);
  a = P3;
  if (bei(a + 0.5f, t0, t1)) s.glocke(a + 0.5f, 62, 0.5f);
  if (bei(a + 3, t0, t1)) s.glocke(a + 3, 62, 0.4f);
  spieleMelodie(s, M(MEL_GEIZ), a + 4, a + 25.6f, 0.36f, t0, t1, 0.45f);
  a = P4;
  if (bei(a, t0, t1)) s.rauschen(a, 14, BANDPASS, 700, 0.08f, 0.8f, 1, 1500);
  spieleMelodie(s, M(MEL_GEIZ), a + 0.4f, a + 13.6f, 0.34f, t0, t1, 0.4f);
  if (bei(a + 19.5f, t0, t1)) s.ton(a + 19.5f, 500, 0.3f, 0.01f, 0.6f, SINUS, GERAEUSCH, 120);  // Sprung ins Loch
  spieleMelodie(s, M(MEL_SPUK), a + 14.4f, a + 23.6f, 0.42f, t0, t1, 0.35f);
  a = P5;
  for (int i = 0; i < 10; i++) if (bei(a + 0.4f + i * 2.5f, t0, t1)) knurr(s, a + 0.4f + i * 2.5f);
  if (bei(a, t0, t1)) s.rauschen(a, 26, BANDPASS, 900, 0.04f, 1, 1);  // Feuer
  spieleMelodie(s, M(MEL_GEIZ), a + 6.4f, a + 25.6f, 0.36f, t0, t1, 0.4f);
  a = P6;
  for (int i = 0; i < 30; i++) if (bei(a + 10.4f + i * 0.5f, t0, t1)) s.rauschen(a + 10.4f + i * 0.5f, 0.2f, BANDPASS, 2600, 0.03f);  // reiben
  spieleMelodie(s, M(MEL_TALER), a + 10.4f, a + 25.6f, 0.42f, t0, t1, 0.45f);
  a = P7;
  for (int i = 0; i < 8; i++) if (bei(a + 2.6f + i * 0.9f, t0, t1)) s.ton(a + 2.6f + i * 0.9f, 900, 0.12f, 0.005f, 0.12f, RECHTECK, GERAEUSCH, 700);  // Mehr!
  spieleMelodie(s, M(MEL_KEGEL), a + 12, a + 27.6f, 0.32f, t0, t1, 0.45f);
  a = P8;
  spieleMelodie(s, M(MEL_TALER), a + 0.4f, a + 5.6f, 0.4f, t0, t1, 0.45f);
  for (int i = 0; i < 20; i++) if (bei(a + 6 + i * 0.25f, t0, t1)) s.ton(a + 6 + i * 0.25f, 3000, 0.05f, 0.002f, 0.04f, SINUS, GERAEUSCH, 2600);  // piep
  spieleMelodie(s, M(MEL_JAGD), a + 6, a + 15, 0.2f, t0, t1, 0.5f);
  for (int i = 0; i < 12; i++) if (bei(a + 9 + i * 0.5f, t0, t1)) klimper(s, a + 9 + i * 0.5f, 84 + (i % 3) * 3);
  spieleMelodie(s, M(MEL_WIEGE), a + 18.4f, a + 25.6f, 0.42f, t0, t1, 0.45f);
  a = P9;
  spieleMelodie(s, M(MEL_GEIZ), a + 0.4f, a + 15.6f, 0.36f, t0, t1, 0.4f);
  for (int i = 0; i < 16; i++) if (bei(a + 10 + i * 0.35f, t0, t1)) klimper(s, a + 10 + i * 0.35f, 79 + (i % 5) * 2);
  if (bei(a + 23, t0, t1)) for (int k = 0; k < 6; k++) s.note(a + 23 + k * 0.08f, 81 + k * 2, 0.5f);
  spieleMelodie(s, M(MEL_TALER), a + 23.4f, a + 27.8f, 0.3f, t0, t1, 0.6f);
  a = P10;
  for (int i = 0; i < 16; i++) if (bei(a + 2 + i * 0.5f, t0, t1)) s.rauschen(a + 2 + i * 0.5f, 0.2f, BANDPASS, 2600, 0.04f);
  if (bei(a + 14.8f, t0, t1)) s.note(a + 14.8f, 88, 0.5f);
  if (bei(a + 15.2f, t0, t1)) for (int k = 0; k < 6; k++) s.glocke(a + 15.2f + k * 0.12f, 81 + k * 2, 0.4f);
  spieleMelodie(s, M(MEL_TALER), a + 17, a + 23.6f, 0.36f, t0, t1, 0.6f);
  a = P11;
  if (bei(a + 11, t0, t1)) for (int k = 0; k < 6; k++) s.note(a + 11 + k * 0.08f, 84 + k * 2, 0.5f);
  spieleMelodie(s, M(MEL_WIEGE), a + 0.4f, a + 10.6f, 0.42f, t0, t1, 0.45f);
  spieleMelodie(s, M(MEL_TALER), a + 12, a + 23.6f, 0.36f, t0, t1, 0.55f);
  a = P12;
  if (bei(a + 0.6f, t0, t1)) for (int k = 0; k < 3; k++) s.rauschen(a + 0.6f + k * 0.25f, 0.06f, BANDPASS, 1400, 0.25f);  // klopf
  if (bei(a + 9, t0, t1)) s.rauschen(a + 9, 0.6f, HOCHPASS, 2400, 0.2f);  // spritz
  for (int i = 0; i < 4; i++) if (bei(a + 10 + i * 0.8f, t0, t1)) s.ton(a + 10 + i * 0.8f, 600, 0.12f, 0.005f, 0.2f, SAEGE, GERAEUSCH, 900);  // jaul
  spieleMelodie(s, M(MEL_JAGD), a + 4, a + 17.6f, 0.22f, t0, t1, 0.5f);
  spieleMelodie(s, M(MEL_WANDER), a + 18.4f, a + 29.6f, 0.32f, t0, t1, 0.5f);
  a = P13;
  spieleMelodie(s, M(MEL_TALER), a + 0.4f, a + 19.6f, 0.36f, t0, t1, 0.7f);
  endeKlang(s, a + 20.4f, t0, t1);
}

// =====================================================================
// Startbild, Vorschau, Erkennungszeichen
// =====================================================================
static void titelSzene(Gfx &g, float t) {
  halle(g, t, 1);
  hoellenhund(g, 230, 226, 0.8f, -1, t, 0.6f);
  anna(g, 110, 226, 0.85f, 1, t, 0.4f, 1.4f);
  maennlein(g, 160, 226, 0.9f, -1, t, 0.3f);
  taler(g, 132, 150, 11, t, 0, 1);
}
static void titelbild(Gfx &g, float t) { titelSzene(g, t); titelRahmen(g, t, "Das Märchen vom goldenen Taler", 248, SCHRIFT_B); }
static void vorschau(Gfx &g, float t) { titelSzene(g, t); }
static void symbol(Gfx &g, float x, float y, float t) {
  taler(g, x, y - 34, 24, t, 0, 1);
  glitzern(g, x, y - 34, 40, t, 5);
}

const Maerchen TALER = {"Das Märchen vom goldenen Taler", PDAUER, FARBE(0xFFD54A), bild, ton, titelbild, vorschau, symbol,
                        LIED(MEL_TALER, 0.4f), "Hans Fallada"};

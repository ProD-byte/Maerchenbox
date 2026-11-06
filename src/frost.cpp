// Vaeterchen Frost - ein Spieluhr-Film fuer die Maerchenbox, nach dem russischen Volksmaerchen (Afanasjew).
// Die boese Stiefmutter, Marfuscha unter der grossen Tanne, die drei Fragen "Ist dir warm, Maedchen?",
// Pelz und Truhe, das Huendchen unter dem Tisch, die unhoefliche Stiefschwester - und Marfuschas Hochzeit.
// Harte Stellen nur angedeutet.
#include "erzaehlen.h"

// ---------- Ablauf (Sekunden) ----------
static const float P1 = 0, P2 = 24, P3 = 48, P4 = 72, P5 = 96, P6 = 122, P7 = 144, P8 = 172, P9 = 196, P10 = 218,
                   P11 = 248, P12 = 276, PDAUER = 300;

// ---------- Aussehen ----------
static Aussehen alterLook() {
  Aussehen a; a.weste = FARBE(0x8E5A3A); a.hemd = FARBE(0xE8D8C0); a.hose = FARBE(0x5A5A6E); a.haar = FARBE(0xD0D0D0);
  a.hut = FARBE(0x6E4A2E); a.hutD = FARBE(0x5A3A22); a.feder = FARBE(0x6E4A2E);
  return a;
}
static Aussehen braeutigamLook() {
  Aussehen a; a.weste = FARBE(0xC0392B); a.hemd = WEISS; a.hose = FARBE(0x2E3A5A); a.haar = FARBE(0xC98A3A);
  a.muetze = 1; a.halstuch = GOLD;
  return a;
}
static const Aussehen ALTER = alterLook(), BRAEUTIGAM = braeutigamLook();
static FrauLook marfLook(bool reich) {
  FrauLook l; l.kleid = reich ? FARBE(0x4E8EDC) : FARBE(0xC0503A); l.schuerze = reich ? 0 : FARBE(0xF2E8D0);
  l.haar = FARBE(0xF2C94E); l.frisur = 1; l.blume = reich ? FARBE(0xFFFFFF) : 0; l.glanz = reich; l.flicken = !reich;
  return l;
}
static FrauLook stiefLook() {
  FrauLook l; l.kleid = FARBE(0x5A4A6E); l.schuerze = FARBE(0x8E7AA8); l.haar = FARBE(0x5A4A3A); l.frisur = 3;
  l.tuch = FARBE(0x6E2A3A); l.wangen = false;
  return l;
}
static FrauLook schwesterLook(bool gefroren) {
  FrauLook l; l.kleid = FARBE(0x5E9A4E); l.schuerze = FARBE(0xE8D070); l.haar = FARBE(0x3A2A20); l.frisur = 0;
  if (gefroren) { l.haut = FARBE(0xC8DCF2); l.wangen = false; l.kleid = FARBE(0x9AB8D8); l.schuerze = FARBE(0xD8E8F8); }
  return l;
}
static const FrauLook MARF = marfLook(false), MARF_REICH = marfLook(true), STIEF = stiefLook(),
                      SCHWESTER = schwesterLook(false), SCHWESTER_EIS = schwesterLook(true);

static const Farbe SCHNEE = FARBE(0xF2F6FF), SCHNEE_D = FARBE(0xC8D4EC), EIS = FARBE(0x9FD0F8);

// =====================================================================
// Figuren
// =====================================================================
static void alter(Gfx &g, float x, float y, float s, float dir, float t, float mund, float armV = 0.4f, float lauf = -1,
                  bool weint = false) {
  Pose p = posis(ALTER, t);
  p.mund = mund;
  p.armV = armV;
  p.lauf = lauf;
  p.zu = weint || p.zu;
  schneiderlein(g, x, y, s, dir, p);
  // grauer Bart
  g.kontur(1 * s, OUT);
  g.ellipse(x + 3 * s * dir, y - 48 * s, 9 * s, 9 * s, FARBE(0xE8E8E8));
  g.konturAus();
  if (mund > 0.2f) g.ellipse(x + 6 * s * dir, y - 50 * s, 3 * s, mund * 2.4f * s, FARBE(0x7A2A2A));
  if (weint) for (int i = 0; i < 2; i++) {
    float k = fmodf(t * 1.2f + i * 0.5f, 1);
    tropfen(g, x + (i ? 10 : -1) * s * dir, y - 58 * s + k * 18 * s, 1.8f * s, FARBE(0x7EC0EE), (uint8_t)(255 * (1 - k)));
  }
}

static void marf(Gfx &g, float x, float y, float s, float dir, float t, float mund, float armV = 0.4f, float lauf = -1,
                 bool reich = false, bool weint = false, float armH = 0.2f, bool zittert = false) {
  FrauPose p = fposis(reich ? MARF_REICH : MARF, t);
  p.mund = mund;
  p.armV = armV;
  p.armH = armH;
  p.lauf = lauf;
  p.weint = weint;
  float z = zittert ? sinf(t * 40) * 1.2f : 0;
  frau(g, x + z, y, s, dir, p);
}

static void stief(Gfx &g, float x, float y, float s, float dir, float t, float mund, float armV = 0.4f, float boese = 0.8f,
                  float armH = 0.2f, bool weint = false) {
  FrauPose p = fposis(STIEF, t, 2);
  p.mund = mund;
  p.armV = armV;
  p.armH = armH;
  p.boese = boese;
  p.weint = weint;
  frau(g, x, y, s, dir, p);
}

static void schwester(Gfx &g, float x, float y, float s, float dir, float t, float mund, float armV = 0.4f, float boese = 0.6f,
                      bool eis = false, float lauf = -1) {
  FrauPose p = fposis(eis ? SCHWESTER_EIS : SCHWESTER, t, 1);
  p.mund = mund;
  p.armV = armV;
  p.boese = boese;
  p.lauf = lauf;
  p.zu = eis || p.zu;
  frau(g, x, y, s, dir, p);
  if (eis) for (int i = 0; i < 4; i++) {
    float ix = x - 10 * s + i * 6 * s;
    float e[] = {ix - 2 * s, y - 80 * s, ix + 2 * s, y - 80 * s, ix, y - 72 * s};
    g.polygon(e, 3, EIS);
  }
}

// Vaeterchen Frost: grosser Alter im blauweissen Pelz, langer weisser Bart, Stab mit Eisstern
static void frost(Gfx &g, float x, float y, float s, float dir, float t, float mund, float armV = 0.6f, float zorn = 0) {
  auto PX = [&](float d) { return x + d * s * dir; };
  auto PY = [&](float d) { return y + d * s; };
  const Farbe mantel = zorn > 0.5f ? FARBE(0x3A6AB8) : FARBE(0x6EA8E8), pelz = WEISS, haut = FARBE(0xFFE0D0);
  // Stab in der hinteren Hand
  g.linie(PX(-22), PY(-4), PX(-26), PY(-118), 3 * s, FARBE(0xC8E0F8));
  g.stern(PX(-26), PY(-122), 9 * s, 3 * s, 6, t, WEISS);
  g.stern(PX(-26), PY(-122), 5 * s, 2 * s, 6, -t * 1.4f, EIS);
  // Mantel
  g.kontur(1.6f * s, OUT);
  float m[] = {PX(-26), PY(0), PX(26), PY(0), PX(14), PY(-76), PX(-14), PY(-76)};
  g.polygon(m, 4, mantel);
  g.konturAus();
  g.rechteck(x - 26 * s, y - 8 * s, 52 * s, 8 * s, pelz);
  g.linie(PX(0), PY(-74), PX(0), PY(-4), 6 * s, pelz);
  for (int i = 0; i < 5; i++) g.stern(PX(-14 + (i % 3) * 13), PY(-60 + i * 11), 3 * s, 1.2f * s, 6, 0, WEISS, 200);
  // Arm vorn
  float ax = PX(10), ay = PY(-66);
  float hx = ax + sinf(armV) * 26 * s * dir, hy = ay + cosf(armV) * 26 * s;
  g.kontur(1.4f * s, OUT);
  g.linie(ax, ay, hx, hy, 10 * s, mantel);
  g.kreis(hx, hy, 6 * s, pelz);
  // Kopf
  g.kreis(PX(2), PY(-92), 15 * s, haut);
  g.konturAus();
  // Bart (lang, wehend)
  float w = sinf(t * 2) * 2;
  float b[] = {PX(-12), PY(-90), PX(16), PY(-90), PX(8 + w), PY(-40), PX(-2 + w), PY(-36), PX(-8), PY(-60)};
  g.kontur(1.2f * s, OUT);
  g.polygon(b, 5, pelz);
  g.konturAus();
  g.ellipse(PX(2), PY(-86), 13 * s, 5 * s, pelz);  // Schnurrbart
  g.kreis(PX(10), PY(-92), 4 * s, FARBE(0xF2707A));  // rote Nase
  if (mund > 0.05f) g.ellipse(PX(6), PY(-82), 4 * s, (1 + mund * 3) * s, FARBE(0x5A2A3A));
  // Augen und Brauen
  g.kreis(PX(-2), PY(-98), 2 * s, SCHWARZ);
  g.kreis(PX(10), PY(-98), 2 * s, SCHWARZ);
  g.linie(PX(-6), PY(-103 + zorn * 2), PX(2), PY(-104 - zorn * 1), 2.4f * s, pelz);
  g.linie(PX(6), PY(-104 - zorn * 1), PX(14), PY(-103 + zorn * 2), 2.4f * s, pelz);
  // Pelzmuetze
  g.kontur(1.4f * s, OUT);
  g.rundRechteck(x - 16 * s, y - 124 * s, 32 * s, 20 * s, 8 * s, mantel);
  g.rundRechteck(x - 18 * s, y - 110 * s, 36 * s, 9 * s, 4 * s, pelz);
  g.konturAus();
  for (int i = 0; i < 4; i++) {
    float k = fmodf(t * 0.6f + i * 0.25f, 1);
    g.stern(x + cosf(i * 1.6f + t) * 34 * s, y - 60 * s - k * 60 * s, 3 * s, 1.2f * s, 6, t + i, WEISS, (uint8_t)(220 * huepf(k)));
  }
}

static void hund(Gfx &g, float x, float y, float s, float dir, float t, float bellt = 0) {
  const Farbe fell = FARBE(0xB07A4A), hell = FARBE(0xE8C9A0), dunkel = FARBE(0x7A4E2A);
  auto PX = [&](float d) { return x + d * s * dir; };
  auto PY = [&](float d) { return y + d * s; };
  float hop = bellt > 0 ? fabsf(sinf(t * 14)) * 2 * bellt : 0;
  g.kontur(1.2f * s, OUT);
  for (int i = 0; i < 4; i++) {
    float bx = i < 2 ? -13 + i * 6 : 8 + (i - 2) * 6;
    g.linie(PX(bx), PY(-14 - hop), PX(bx), PY(-2), 4 * s, i & 1 ? fell : dunkel);
  }
  float wedel = sinf(t * 10) * 6;
  g.kurve(PX(-15), PY(-18 - hop), PX(-24), PY(-26 - hop + wedel * 0.3f), PX(-27), PY(-30 - hop + wedel), 3.5f * s, fell);
  g.ellipse(PX(0), PY(-18 - hop), 17 * s, 8 * s, fell);
  float kx = PX(17), ky = PY(-26 - hop * 2);
  g.kreis(kx, ky, 8 * s, fell);
  g.ellipse(kx + 8 * s * dir, ky + 2 * s, 6 * s, 4.5f * s, hell);
  g.konturAus();
  g.ellipse(kx - 3 * s * dir, ky + 1 * s, 3.5f * s, 7 * s, dunkel, 255, 0.3f * dir);
  g.kreis(kx + 13 * s * dir, ky + 1 * s, 1.8f * s, SCHWARZ);
  g.kreis(kx + 3 * s * dir, ky - 2 * s, 1.5f * s, SCHWARZ);
  if (bellt > 0.1f) g.ellipse(kx + 10 * s * dir, ky + 6 * s, 4 * s, (1 + 3 * fabsf(sinf(t * 14))) * s, FARBE(0x7A2A2A));
}

// =====================================================================
// Dinge und Orte
// =====================================================================
static void schneeFall(Gfx &g, float t, int n, float wind = 0, uint8_t a = 230) {
  for (int i = 0; i < n; i++) {
    float x = fmodf(zufall(i + 400) * 300 + t * (8 + wind * 40) + sinf(t + i) * 6, 300) - 8;
    float y = fmodf(zufall(i + 440) * 240 + t * (18 + zufall(i) * 14), 250) - 6;
    g.kreis(x, y, 1.3f + zufall(i + 7) * 1.2f, WEISS, a);
  }
}

static void schneeTanne(Gfx &g, float x, float y, float s, Farbe c = FARBE(0x2F7A4E)) {
  tanne(g, x, y, s, c);
  for (int i = 0; i < 3; i++) {
    float yy = y - 26 * s - i * 22 * s, w = (24 - i * 6) * s;
    float d[] = {x - w, yy + 4 * s, x, yy - 10 * s, x + w, yy + 4 * s, x, yy - 2 * s};
    g.polygon(d, 4, SCHNEE);
  }
}

static void schlitten(Gfx &g, float x, float y, float s, float dir, float t, float lauf, bool pferd = true) {
  if (pferd) einhorn(g, x + 62 * s * dir, y, 0.62f * s, lauf, dir, 0, 0, true);
  g.kontur(1.4f * s, OUT);
  g.rundRechteck(x - 30 * s, y - 34 * s, 60 * s, 20 * s, 5 * s, FARBE(0xC0392B));
  g.konturAus();
  g.linie(x - 30 * s, y - 24 * s, x + 30 * s, y - 24 * s, 2 * s, GOLD);
  g.linie(x - 34 * s, y - 4 * s, x + 34 * s, y - 4 * s, 2.6f * s, HOLZ_D);
  g.bogen(x + 34 * s * dir, y - 12 * s, 8 * s, dir > 0 ? -1.57f : 1.57f, dir > 0 ? 1.57f : 4.71f, 2.6f * s, HOLZ_D);
  for (int i = -1; i <= 1; i += 2) g.linie(x + i * 20 * s, y - 14 * s, x + i * 20 * s, y - 5 * s, 2 * s, HOLZ_D);
  if (pferd) g.linie(x + 30 * s * dir, y - 26 * s, x + 50 * s * dir, y - 36 * s, 1.6f * s, HOLZ_D);
}

static void truhe(Gfx &g, float x, float y, float s, float t, float auf = 0) {
  g.kontur(1.4f * s, OUT);
  g.rundRechteck(x - 22 * s, y - 26 * s, 44 * s, 26 * s, 3 * s, FARBE(0x8E4E2E));
  g.konturAus();
  g.rechteck(x - 22 * s, y - 18 * s, 44 * s, 3 * s, GOLD);
  g.rechteck(x - 3 * s, y - 22 * s, 6 * s, 8 * s, GOLD);
  if (auf > 0) {
    g.kreis(x, y - 28 * s, 30 * s, FARBE(0xFFF0A0), (uint8_t)(70 * auf));
    for (int i = 0; i < 5; i++) g.stern(x - 16 * s + i * 8 * s, y - 30 * s - (i & 1) * 6 * s, 4 * s, 1.6f * s, 4, t * 2 + i,
                                       i % 3 == 0 ? FARBE(0xE8283F) : i % 3 == 1 ? GOLD : FARBE(0x7EC0EE));
  }
  glitzern(g, x, y - 16 * s, 26 * s, t, 3);
}

static void pfannkuchen(Gfx &g, float x, float y, int n) {
  g.ellipse(x, y + 2, 20, 4, FARBE(0xE8E4DC));
  for (int i = 0; i < n; i++) g.ellipse(x, y - i * 3, 15, 3.4f, i & 1 ? FARBE(0xE8B060) : FARBE(0xD89A4A));
}

// Wald im Schnee; nacht 0..1; kalt = Frost klirrt (blauer, Eisglitzer)
static void winterwald(Gfx &g, float t, float nacht, float kalt = 0, float scroll = 0) {
  himmel(g, nacht, t, 200);
  if (nacht > 0.5f) {
    mond(g, 236, 36, 11);
    for (int i = 0; i < 20; i++) g.stern(zufall(i + 700) * 284, zufall(i + 720) * 100, 2, 0.8f, 4, 0, WEISS, 180);
  }
  for (int i = 0; i < 6; i++) schneeTanne(g, fmodf(i * 60 - scroll * 0.5f + 960, 360) - 40, 202, 0.8f + (i % 3) * 0.15f,
                                          mischeFarbe(FARBE(0x2F7A4E), FARBE(0x1F3B45), nacht));
  // die grosse Tanne
  schneeTanne(g, 200, 206, 1.5f, mischeFarbe(FARBE(0x2A6E44), FARBE(0x1A3440), nacht));
  g.rechteck(0, 200, 284, 40, mischeFarbe(SCHNEE, FARBE(0xA8B8D8), nacht * 0.7f));
  for (int i = 0; i < 6; i++) g.ellipse(zufall(i + 90) * 284, 204 + zufall(i + 95) * 30, 26, 4, SCHNEE_D, 160);
  if (kalt > 0) {
    g.rechteck(0, 0, 284, 240, FARBE(0x9FD0F8), (uint8_t)(50 * kalt));
    for (int i = 0; i < 10; i++)
      g.stern(zufall(i + 50) * 284, zufall(i + 60) * 200, 3, 1, 6, t + i, WEISS, (uint8_t)(200 * kalt * huepf(fmodf(t * 0.5f + i * 0.1f, 1))));
  }
}

// Bauernstube mit grossem weissem Ofen, Tisch und Fenster mit Eisblumen
static void stube(Gfx &g, float t, int kuchen = 0) {
  g.verlauf(0, 200, FARBE(0xC8A47A), FARBE(0xA8845A));
  for (int i = 0; i < 6; i++) g.linie(0, 30 + i * 32, 284, 30 + i * 32, 1.6f, FARBE(0x8E6A44));
  g.rechteck(0, 200, 284, 40, FARBE(0x7A5A3A));
  // Ofen
  g.kontur(2, OUT);
  g.rundRechteck(180, 70, 100, 132, 8, FARBE(0xF2EEE4));
  g.konturAus();
  g.rundRechteck(200, 140, 50, 40, 16, FARBE(0x3A2E26));
  for (int i = 0; i < 3; i++) {
    float h = 10 + 4 * sinf(t * 9 + i * 1.7f);
    float f[] = {212 + i * 13 - 6, 178, 212 + i * 13 + 6, 178, 212 + i * 13, 178 - h};
    g.polygon(f, 3, FARBE(0xFF8A2A));
  }
  g.linie(184, 100, 276, 100, 2, FARBE(0xC8C0B0));
  // Fenster mit Eisblumen
  g.kontur(1.6f, OUT);
  g.rundRechteck(30, 50, 60, 56, 4, HOLZ);
  g.konturAus();
  g.rundRechteck(34, 54, 52, 48, 2, FARBE(0xD8ECFF));
  for (int i = 0; i < 4; i++) g.stern(42 + i * 12, 64 + (i & 1) * 22, 6, 2, 6, i, WEISS, 200);
  g.rechteck(58, 54, 3, 48, HOLZ);
  // Tisch
  g.kontur(1.4f, OUT);
  g.rechteck(70, 160, 90, 7, HOLZ);
  g.konturAus();
  g.rechteck(76, 167, 6, 35, HOLZ_D);
  g.rechteck(148, 167, 6, 35, HOLZ_D);
  if (kuchen > 0) pfannkuchen(g, 118, 156, kuchen);
}

// Hoftor (fuer die Heimkehr mit dem Schlitten)
static void hof(Gfx &g, float t, float nacht = 0) {
  himmel(g, nacht, t, 200);
  sonne(g, 240, 34, 10, t);
  g.kontur(1.8f, OUT);
  g.rechteck(150, 100, 140, 102, FARBE(0x8E6A44));
  float d[] = {140, 102, 220, 56, 300, 102};
  g.polygon(d, 3, FARBE(0x6E4A2E));
  g.konturAus();
  for (int i = 0; i < 4; i++) g.linie(150, 112 + i * 22, 290, 112 + i * 22, 1.4f, FARBE(0x6E4A2E));
  float sd[] = {136, 104, 220, 62, 304, 104, 220, 74};
  g.polygon(sd, 4, SCHNEE);
  g.rundRechteck(176, 130, 30, 26, 3, FARBE(0xFFE8A0));
  g.rundRechteck(230, 150, 30, 52, 4, HOLZ_D);
  g.rechteck(0, 200, 284, 40, SCHNEE);
  for (int i = 0; i < 8; i++) {  // Zaun
    g.rechteck(6 + i * 16, 170, 5, 32, HOLZ);
    g.ellipse(8.5f + i * 16, 170, 4, 2, SCHNEE);
  }
  g.rechteck(0, 180, 130, 4, HOLZ_D);
}

// =====================================================================
// Szenen
// =====================================================================
// 1) Marfuscha und die boese Stiefmutter
static void szeneStube(Gfx &g, float u) {
  stube(g, u);
  // die Stiefschwester liegt faul auf dem Ofen
  schwester(g, 232, 70, 0.7f, -1, u, u > 14 ? 0.3f : 0, 0.4f, 0.2f);
  float k = fmodf(u * 0.12f, 1);
  marf(g, lerp(60, 140, huepf(k)), 222, 0.85f, k < 0.5f ? 1 : -1, u, 0, 1.2f + sinf(u * 5) * 0.3f, fmodf(u * 1.2f, 1));
  // Besen
  float bx = lerp(60, 140, huepf(k)) + 14;
  g.linie(bx, 176, bx + sinf(u * 5) * 10, 220, 2.4f, HOLZ_D);
  g.ellipse(bx + sinf(u * 5) * 10, 220, 8, 3, FARBE(0xE8C870));
  stief(g, 176, 222, 0.88f, -1, u, (u > 6.4f && u < 11) ? 0.5f + 0.3f * sinf(u * 9) : 0, u > 6.4f && u < 11 ? 1.8f : 0.4f);
  g.kameraAus();
  vers(g, {"Ein alter Mann hatte eine Tochter,", "die hieß Marfuscha. Die Stiefmutter", "konnte sie nicht leiden."}, aufzu(u, 0.4f, 6.2f));
  blase(g, 180, 50, {"Faules Ding! Nichts machst", "du recht!"}, aufzu(u, 6.4f, 11), 176, 140);
  vers(g, {"Dabei war Marfuscha fleißig und", "gut. Die eigene Tochter aber lag", "faul auf dem Ofen - und wurde gelobt."}, aufzu(u, 11.4f, 18.6f));
  vers(g, {"Marfuscha sagte kein böses Wort."}, aufzu(u, 19, 23.6f));
}

// 2) Die Stiefmutter befiehlt: in den Wald zu Vaeterchen Frost
static void szeneBefehl(Gfx &g, float u) {
  stube(g, u);
  stief(g, 190, 222, 0.88f, -1, u, (u > 0.4f && u < 9) ? 0.5f + 0.3f * sinf(u * 9) : 0, u < 9 ? 2.4f : 0.4f, 1);
  alter(g, 120, 222, 0.88f, 1, u, 0.3f, 0.4f, -1, u > 9.4f);
  marf(g, 64, 222, 0.82f, 1, u, 0, 0.4f, -1, false, u > 14);
  g.kameraAus();
  blase(g, 180, 50, {"Alter, bring sie fort in den", "Wald, zu Väterchen Frost -", "der soll sie freien!"}, aufzu(u, 0.4f, 9), 190, 140);
  vers(g, {"Der Alte weinte bitterlich, aber", "er wagte nicht zu widersprechen."}, aufzu(u, 9.4f, 16));
  vers(g, {"Marfuscha zog ihr dünnes Kleid", "an und ging mit dem Vater."}, aufzu(u, 16.4f, 23.6f));
}

// 3) Die Fahrt in den Wald - unter der grossen Tanne
static void szeneFahrt(Gfx &g, float u) {
  if (u < 10) {
    winterwald(g, u, 0.2f, 0, u * 60);
    schneeFall(g, u, 30, 0.5f);
    schlitten(g, 110, 222, 1, 1, u, fmodf(u * 1.6f, 1));
    alter(g, 116, 196, 0.6f, 1, u, 0, 1.4f);
    marf(g, 96, 194, 0.55f, 1, u, 0, 0.4f, -1, false, true);
    g.kameraAus();
    vers(g, {"Der Vater fuhr sie mit dem Schlitten", "tief in den verschneiten Wald."}, aufzu(u, 0.4f, 9.6f));
    return;
  }
  float v = u - 10;
  winterwald(g, u, 0.35f);
  schneeFall(g, u, 30, 0.5f);
  float k = seg(v, 6, 13);
  schlitten(g, lerp(80, -160, k), 222, 1, -1, u, k > 0 ? fmodf(u * 1.6f, 1) : -1);
  alter(g, v < 6 ? 120 : lerp(80, -160, k) - 4, v < 6 ? 222 : 196, v < 6 ? 0.85f : 0.6f, -1, u, v > 0.4f && v < 5 ? 0.4f : 0, 1.2f, -1, true);
  marf(g, 176, 222, 0.82f, -1, u, 0, v > 7 ? 1.6f + sinf(u * 4) * 0.3f : 0.4f, -1, false, v > 6, 0.2f, v > 8);
  g.kameraAus();
  blase(g, 120, 50, {"Leb wohl, mein liebes Kind!"}, aufzu(v, 0.4f, 5), 120, 140);
  vers(g, {"Unter einer großen Tanne setzte", "er sie in den Schnee und fuhr heim."}, aufzu(v, 5.4f, 13.8f));
}

// 4) Vaeterchen Frost kommt - die erste Frage
static void szeneErsteFrage(Gfx &g, float u) {
  winterwald(g, u, 1, 0.3f);
  schneeFall(g, u, 24);
  // Frost springt von Tanne zu Tanne und knackt
  float k = seg(u, 4, 10);
  float fx = lerp(330, 236, k), fy = 206 - huepf(fmodf(k * 3, 1)) * 40 * (k < 1);
  frost(g, fx - 14, fy, 0.78f, -1, u, u > 10.4f && u < 15 ? 0.5f + 0.3f * sinf(u * 9) : 0, 0.8f);
  marf(g, 120, 222, 0.82f, 1, u, u > 15.4f && u < 22 ? 0.4f : 0, 1.4f, -1, false, false, 1.4f, true);
  g.kameraAus();
  vers(g, {"Marfuscha saß unter der Tanne", "und zitterte. Da knackte es", "in den Zweigen..."}, aufzu(u, 0.4f, 9.6f));
  blase(g, 200, 50, {"Ist dir warm, Mädchen?"}, aufzu(u, 10.4f, 15), 230, 110);
  blase(g, 110, 60, {"Warm, Väterchen Frost,", "mir ist ganz warm."}, aufzu(u, 15.4f, 22), 120, 140);
}

// 5) Die zweite und dritte Frage
static void szeneFragen(Gfx &g, float u) {
  winterwald(g, u, 1, 0.6f + seg(u, 12, 14) * 0.3f);
  schneeFall(g, u, 30, 1);
  frost(g, u < 12 ? 222 : 196, 206, u < 12 ? 0.78f : 0.82f, -1, u, (u > 0.4f && u < 5) || (u > 12.4f && u < 17) ? 0.5f + 0.3f * sinf(u * 9) : 0,
        u < 12 ? 1 : 1.4f);
  marf(g, 112, 222, 0.82f, 1, u, (u > 5.4f && u < 11.6f) || u > 17.4f ? 0.25f : 0, 1.4f, -1, false, false, 1.4f, true);
  // ihre Wangen werden blass: Eiskristalle in der Luft um sie
  for (int i = 0; i < 6; i++) g.stern(112 + cosf(u + i) * 30, 170 + sinf(u * 1.3f + i) * 30, 2.4f, 1, 6, u, WEISS, 180);
  g.kameraAus();
  blase(g, 200, 50, {"Ist dir warm, Mädchen?", "Ist dir warm, du Schöne?"}, aufzu(u, 0.4f, 5), 230, 110);
  blase(g, 110, 60, {"Warm, Väterchen...", "mir ist warm."}, aufzu(u, 5.4f, 11.6f), 112, 140);
  blase(g, 190, 50, {"Ist dir warm, Mädchen?", "Ist dir warm, mein Täubchen?"}, aufzu(u, 12.4f, 17), 196, 110);
  blase(g, 110, 60, {"Warm, lieber Väterchen Frost..."}, aufzu(u, 17.4f, 22.6f), 112, 140);
  vers(g, {"Sie konnte kaum noch sprechen."}, aufzu(u, 22.8f, 25.8f));
}

// 6) Frost hat Mitleid: Pelze und eine Truhe
static void szeneGeschenke(Gfx &g, float u) {
  winterwald(g, u, 1, 0.2f * (1 - seg(u, 2, 6)));
  float pelz = seg(u, 4, 8);
  frost(g, 200, 206, 0.82f, -1, u, u > 0.4f && u < 4 ? 0.4f : 0.2f, 1.6f + sinf(u * 2) * 0.2f);
  marf(g, 112, 222, 0.82f, 1, u, pelz > 0.5f ? 0.5f : 0, pelz > 0.5f ? 0.8f : 1.4f, -1, pelz > 0.5f, false, 0.2f, pelz < 0.5f);
  if (pelz > 0 && pelz < 1) puff(g, 112, 170, pelz, WEISS);
  if (u > 8) truhe(g, 154, 222, 1, u, seg(u, 10, 12));
  g.kameraAus();
  vers(g, {"Da hatte Väterchen Frost Mitleid", "mit dem guten Mädchen."}, aufzu(u, 0.4f, 6));
  vers(g, {"Er hüllte sie in warme Pelze und", "schenkte ihr eine Truhe voller", "Schätze und ein schönes Kleid."}, aufzu(u, 8, 15.6f));
  blase(g, 110, 60, {"Danke, Väterchen Frost!"}, aufzu(u, 16, 21.6f), 112, 140);
}

// 7) Pfannkuchen und das Huendchen unter dem Tisch
static void szeneHund(Gfx &g, float u) {
  stube(g, u, 6);
  bool wirft = u > 12 && u < 14;
  stief(g, 196, 222, 0.88f, -1, u, (u > 0.4f && u < 6) || (u > 13.4f && u < 19) ? 0.5f + 0.3f * sinf(u * 9) : 0,
        wirft ? 2.4f : u < 6 ? 1.6f : 0.4f, 1);
  float bellt = (u > 6.4f && u < 12) || (u > 20 && u < 27) ? 1 : 0;
  hund(g, 110, 222, 0.9f, 1, u, bellt);
  float pk = seg(u, 12.4f, 13.6f);
  if (pk > 0 && pk < 1) g.ellipse(lerp(180, 134, pk), lerp(150, 200, pk) - huepf(pk) * 20, 8, 2.4f, FARBE(0xD89A4A));
  if (u > 19 && u < 20) g.ellipse(134, 202, 6, 2, FARBE(0xD89A4A));
  alter(g, 50, 222, 0.85f, 1, u, 0, 0.4f, u > 3 ? fmodf(u * 1.4f, 1) : -1);
  g.kameraAus();
  blase(g, 180, 50, {"Alter, hol sie heim. Ich backe", "Pfannkuchen für den Leichenschmaus!"}, aufzu(u, 0.4f, 6), 196, 140);
  blase(g, 120, 50, {"Wau, wau! Des Alten Tochter fährt", "mit Gold und Silber heim - die", "Tochter der Alten will keiner!"}, aufzu(u, 6.4f, 12), 126, 190);
  blase(g, 180, 50, {"Still! Da, nimm den Pfannkuchen", "und bell: Die Tochter der Alten", "wird bald gefreit!"}, aufzu(u, 13.4f, 19.6f), 196, 140);
  blase(g, 120, 50, {"Wau, wau! Des Alten Tochter fährt", "mit Gold und Silber heim - die", "Tochter der Alten will keiner!"}, aufzu(u, 20, 27.6f), 126, 190);
}

// 8) Marfuscha kommt reich beschenkt heim
static void szeneHeimkehr(Gfx &g, float u) {
  hof(g, u);
  float k = seg(u, 0, 6);
  schlitten(g, lerp(-80, 120, k), 222, 1, 1, u, k < 1 ? fmodf(u * 1.6f, 1) : -1);
  truhe(g, lerp(-80, 120, k) - 14, 196, 0.6f, u, k >= 1 ? 1 : 0);
  bool ausgestiegen = u > 8;
  if (!ausgestiegen) {
    alter(g, lerp(-80, 120, k) + 10, 196, 0.6f, 1, u, 0.5f, 1.4f);
    marf(g, lerp(-80, 120, k) + 18, 194, 0.55f, 1, u, 0.5f, 0.4f, -1, true);
  } else {
    alter(g, 90, 222, 0.85f, 1, u, 0.6f, 2.4f + sinf(u * 4) * 0.3f);
    marf(g, 140, 222, 0.85f, 1, u, 0.5f, 1.4f, -1, true);
    glitzern(g, 140, 170, 30, u, 5);
  }
  stief(g, 236, 222, 0.88f, -1, u, u > 13.4f ? 0.9f : 0.5f, u > 13 ? 2.8f : 0.4f, u > 13.4f ? 0.2f : 0.8f, u > 13 ? 2.8f : 0.2f);
  hund(g, 196, 222, 0.7f, -1, u, u > 8 && u < 12 ? 1 : 0);
  g.kameraAus();
  vers(g, {"Da kam der Schlitten heim - und", "Marfuscha saß darin, im Pelz,", "mit einer Truhe voller Schätze!"}, aufzu(u, 0.4f, 10));
  vers(g, {"Die Stiefmutter schlug die", "Hände über dem Kopf zusammen."}, aufzu(u, 13, 23.6f));
}

// 9) Nun soll die eigene Tochter auch zu Vaeterchen Frost
static void szeneNeid(Gfx &g, float u) {
  if (u < 12) {
    stube(g, u);
    stief(g, 196, 222, 0.88f, -1, u, u > 0.4f && u < 7 ? 0.5f + 0.3f * sinf(u * 9) : 0, 2.2f, 1);
    schwester(g, 140, 222, 0.85f, -1, u, u > 7.4f ? 0.4f : 0, 0.4f, 0.6f);
    alter(g, 60, 222, 0.85f, 1, u, 0, 0.4f);
    g.kameraAus();
    blase(g, 180, 50, {"Alter, spann an! Bring auch", "meine Tochter zu Väterchen Frost!"}, aufzu(u, 0.4f, 7), 196, 140);
    vers(g, {"Sie zog ihr die wärmsten Kleider", "an und gab ihr Kuchen mit."}, aufzu(u, 7.4f, 11.8f));
    return;
  }
  float v = u - 12;
  winterwald(g, u, 0.4f, 0, u * 40);
  schneeFall(g, u, 30, 0.5f);
  float k = seg(v, 4, 10);
  schlitten(g, lerp(80, -160, k), 222, 1, -1, u, fmodf(u * 1.6f, 1));
  alter(g, lerp(80, -160, k) - 4, 196, 0.6f, -1, u, 0, 1.2f);
  schwester(g, 176, 222, 0.82f, -1, u, 0.3f, 1.2f, 1);
  g.kameraAus();
  vers(g, {"Der Alte setzte sie unter dieselbe", "Tanne und fuhr heim."}, aufzu(v, 0.4f, 9.6f));
}

// 10) Die unhoefliche Tochter
static void szeneUnhoeflich(Gfx &g, float u) {
  float zorn = seg(u, 18, 21);
  winterwald(g, u, 1, 0.3f + zorn * 0.7f);
  schneeFall(g, u, 30 + (int)(zorn * 40), 1 + zorn * 3);
  frost(g, u < 14 ? 236 : 210, 206, 0.8f, -1, u, (u > 6.4f && u < 10) || (u > 14.4f && u < 18) ? 0.5f + 0.3f * sinf(u * 9) : 0,
        zorn > 0 ? 2.6f : 0.9f, zorn);
  schwester(g, 110, 222, 0.82f, 1, u, (u > 0.4f && u < 6) || (u > 10.4f && u < 14) || (u > 18.4f && u < 23) ? 0.6f + 0.3f * sinf(u * 9) : 0.2f,
            (u > 10.4f && u < 14) || u > 18.4f ? 2.4f : 1.4f, 1);
  g.kameraAus();
  blase(g, 110, 60, {"Brr! Wo bleibt denn mein", "Bräutigam? Mir ist so kalt!"}, aufzu(u, 0.4f, 6), 110, 140);
  blase(g, 200, 50, {"Ist dir warm, Mädchen?"}, aufzu(u, 6.4f, 10), 230, 110);
  blase(g, 110, 60, {"Bist du blind, alter Narr? Mir", "frieren Hände und Füße!"}, aufzu(u, 10.4f, 14), 110, 140);
  blase(g, 190, 50, {"Ist dir warm, du Schöne?"}, aufzu(u, 14.4f, 18), 206, 110);
  blase(g, 110, 60, {"Scher dich fort, du alter", "Griesgram!"}, aufzu(u, 18.4f, 23), 110, 140);
  vers(g, {"Da wurde Väterchen Frost zornig,", "und es wurde bitterkalt."}, aufzu(u, 23.4f, 29.6f));
}

// 11) Das Huendchen bellt wieder - der Schlitten kommt
static void szeneEnde(Gfx &g, float u) {
  if (u < 14) {
    stube(g, u, 6);
    stief(g, 196, 222, 0.88f, -1, u, u > 6.4f && u < 10 ? 0.6f : 0, u > 6 && u < 8 ? 2.4f : 0.4f, 1);
    hund(g, 110, 222, 0.9f, 1, u, u > 0.4f && u < 6 ? 1 : 0);
    marf(g, 50, 222, 0.82f, 1, u, 0, 0.4f, -1, true);
    g.kameraAus();
    blase(g, 120, 50, {"Wau, wau! Des Alten Tochter wird", "bald gefreit - die Tochter der", "Alten kommt steif gefroren heim!"}, aufzu(u, 0.4f, 6), 126, 190);
    blase(g, 180, 50, {"Still, du Köter!"}, aufzu(u, 6.4f, 10), 196, 140);
    vers(g, {"Da knarrte das Hoftor..."}, aufzu(u, 10.4f, 13.8f));
    return;
  }
  float v = u - 14;
  hof(g, u, 0.3f);
  float k = seg(v, 0, 5);
  schlitten(g, lerp(-80, 120, k), 222, 1, 1, u, k < 1 ? fmodf(u * 1.6f, 1) : -1);
  // unter der Decke im Schlitten: die Tochter, ganz steif und blau gefroren (nur angedeutet)
  g.kontur(1.2f, OUT);
  g.rundRechteck(lerp(-80, 120, k) - 26, 180, 40, 12, 5, FARBE(0x8E7AA8));
  g.konturAus();
  g.kreis(lerp(-80, 120, k) - 28, 182, 7, SCHWESTER_EIS.haut);
  for (int i = 0; i < 3; i++) g.stern(lerp(-80, 120, k) - 28 + i * 6, 176, 2.4f, 1, 6, 0, EIS);
  alter(g, lerp(-80, 120, k) + 10, 196, 0.6f, 1, u, 0, 1.4f, -1, true);
  stief(g, 220, 222, 0.88f, -1, u, v > 6 ? 0.6f : 0.3f, v > 6 ? 2.8f : 0.4f, 0, v > 6 ? 2.8f : 0.2f, v > 7);
  g.kameraAus();
  vers(g, {"Der Alte brachte ihre Tochter", "heim - steif gefroren lag sie", "unter der Decke."}, aufzu(v, 0.4f, 8));
  vers(g, {"Da weinte die Stiefmutter. Aber", "geschehen war geschehen."}, aufzu(v, 8.4f, 13.8f));
}

// 12) Marfuschas Hochzeit
static void szeneHochzeit(Gfx &g, float u) {
  hof(g, u);
  schneeFall(g, u, 14, 0, 180);
  Pose b = posis(BRAEUTIGAM, u);
  b.armV = 1.2f;
  b.mund = 0.5f;
  schneiderlein(g, 150, 222, 0.9f, -1, b);
  marf(g, 112, 222, 0.88f, 1, u, 0.5f, 1.2f, -1, true);
  alter(g, 60, 222, 0.8f, 1, u, 0.5f, 2.2f + sinf(u * 4) * 0.3f);
  hund(g, 200, 222, 0.7f, -1, u, 0);
  herzen(g, 130, 150, u, 6, 26);
  for (int i = 0; i < 3; i++) feuerwerk(g, 60 + i * 80, 50 + (i & 1) * 16, fmodf(u * 0.4f + i * 0.33f, 1), i == 1 ? GOLD : FARBE(0x9FD8FF));
  g.kameraAus();
  vers(g, {"Marfuscha aber heiratete einen", "braven Burschen und lebte", "glücklich und zufrieden."}, aufzu(u, 0.4f, 10));
  vers(g, {"Und der Alte erzählte noch den", "Enkeln von Väterchen Frost."}, aufzu(u, 10.4f, 17));
  endeBild(g, u, 19, 130, 170);
}

static void bild(Gfx &g, float t) {
  if (t < P2) szeneStube(g, t - P1);
  else if (t < P3) szeneBefehl(g, t - P2);
  else if (t < P4) szeneFahrt(g, t - P3);
  else if (t < P5) szeneErsteFrage(g, t - P4);
  else if (t < P6) szeneFragen(g, t - P5);
  else if (t < P7) szeneGeschenke(g, t - P6);
  else if (t < P8) szeneHund(g, t - P7);
  else if (t < P9) szeneHeimkehr(g, t - P8);
  else if (t < P10) szeneNeid(g, t - P9);
  else if (t < P11) szeneUnhoeflich(g, t - P10);
  else if (t < P12) szeneEnde(g, t - P11);
  else szeneHochzeit(g, t - P12);
}

// =====================================================================
// Musik
// =====================================================================
// russisch anmutende Weise (d-Moll, getragen)
static const Ton3 MEL_FROST[] = {
  {74, 1, 50}, {74, .5, 0}, {72, .5, 0}, {70, 1, 46}, {69, 1, 0}, {67, .5, 43}, {69, .5, 0}, {70, .5, 0}, {72, .5, 0},
  {69, 2, 45}, {74, 1, 50}, {77, .5, 0}, {76, .5, 0}, {74, 1, 46}, {72, 1, 0}, {70, .5, 43}, {69, .5, 0},
  {67, .5, 45}, {69, .5, 0}, {62, 2, 50}};
// Schellenschlitten (D-Dur, froh)
static const Ton3 MEL_SCHLITTEN[] = {
  {74, .5, 50}, {78, .5, 0}, {81, .5, 0}, {78, .5, 0}, {79, .5, 43}, {76, .5, 0}, {74, 1, 0},
  {73, .5, 45}, {76, .5, 0}, {79, .5, 0}, {76, .5, 0}, {78, .5, 50}, {74, .5, 0}, {74, 1, 0}};
static void schellen(Synth &s, float a, float b, float t0, float t1) {
  for (float x = a; x < b; x += 0.25f) if (bei(x, t0, t1)) s.ton(x, 3200 + ((int)(x * 4) & 1) * 400, 0.04f, 0.002f, 0.12f, SINUS, GERAEUSCH, 3000);
}
static void knack(Synth &s, float x) { s.rauschen(x, 0.06f, HOCHPASS, 3000, 0.25f); }
static void wuff(Synth &s, float x) { s.ton(x, 300, 0.22f, 0.005f, 0.1f, SAEGE, GERAEUSCH, 180); }
static void wind(Synth &s, float a, float d, float st, float t0, float t1) { if (bei(a, t0, t1)) s.rauschen(a, d, BANDPASS, 600, 0.06f * st, 0.8f, 1, 1400); }
static void ton(Synth &s, float t0, float t1) {
  float a = P1;
  spieleMelodie(s, M(MEL_FROST), a + 0.5f, a + 23.6f, 0.4f, t0, t1, 0.6f);
  a = P2;
  spieleMelodie(s, M(MEL_FROST), a + 0.4f, a + 23.6f, 0.46f, t0, t1, 0.45f);
  a = P3;
  schellen(s, a + 0.4f, a + 10, t0, t1);
  spieleMelodie(s, M(MEL_SCHLITTEN), a + 0.4f, a + 9.6f, 0.3f, t0, t1, 0.45f);
  wind(s, a + 10, 14, 1, t0, t1);
  spieleMelodie(s, M(MEL_N), a + 10.4f, a + 23.6f, 0.44f, t0, t1, 0.35f);
  a = P4;
  wind(s, a, 24, 1.2f, t0, t1);
  for (int i = 0; i < 8; i++) if (bei(a + 4 + i * 0.75f, t0, t1)) knack(s, a + 4 + i * 0.75f);
  spieleMelodie(s, M(MEL_FROST), a + 10.4f, a + 23.6f, 0.44f, t0, t1, 0.45f);
  a = P5;
  wind(s, a, 26, 1.6f, t0, t1);
  for (int i = 0; i < 12; i++) if (bei(a + 0.3f + i * 2.1f, t0, t1)) s.note(a + 0.3f + i * 2.1f, 88 + (i % 3) * 3, 0.3f);  // Eisklirren
  spieleMelodie(s, M(MEL_FROST), a + 0.4f, a + 25.6f, 0.48f, t0, t1, 0.4f);
  a = P6;
  for (int k = 0; k < 6; k++) if (bei(a + 4 + k * 0.08f, t0, t1)) s.note(a + 4 + k * 0.08f, 84 + k * 2, 0.45f);
  spieleMelodie(s, M(MEL_WIEGE), a + 6, a + 21.6f, 0.42f, t0, t1, 0.55f);
  a = P7;
  for (int i = 0; i < 8; i++) {
    if (bei(a + 6.6f + i * 0.6f, t0, t1)) wuff(s, a + 6.6f + i * 0.6f);
    if (bei(a + 20.2f + i * 0.7f, t0, t1)) wuff(s, a + 20.2f + i * 0.7f);
  }
  spieleMelodie(s, M(MEL_KEGEL), a + 0.4f, a + 6, 0.32f, t0, t1, 0.35f);
  spieleMelodie(s, M(MEL_KEGEL), a + 13.4f, a + 19.6f, 0.32f, t0, t1, 0.35f);
  a = P8;
  schellen(s, a, a + 6, t0, t1);
  spieleMelodie(s, M(MEL_SCHLITTEN), a + 0.4f, a + 23.6f, 0.3f, t0, t1, 0.6f);
  a = P9;
  spieleMelodie(s, M(MEL_SPUK), a + 0.4f, a + 11.8f, 0.42f, t0, t1, 0.35f);
  schellen(s, a + 12, a + 21.6f, t0, t1);
  wind(s, a + 12, 10, 0.8f, t0, t1);
  a = P10;
  wind(s, a, 30, 1.4f, t0, t1);
  wind(s, a + 18, 12, 3, t0, t1);
  spieleMelodie(s, M(MEL_SPUK), a + 0.4f, a + 18, 0.42f, t0, t1, 0.35f);
  if (bei(a + 18.5f, t0, t1)) for (int k = 0; k < 4; k++) s.note(a + 18.5f + k * 0.15f, 62 - k * 3, 0.6f);
  a = P11;
  for (int i = 0; i < 8; i++) if (bei(a + 0.6f + i * 0.6f, t0, t1)) wuff(s, a + 0.6f + i * 0.6f);
  if (bei(a + 10.4f, t0, t1)) s.ton(a + 10.4f, 220, 0.15f, 0.2f, 1.4f, SAEGE, GERAEUSCH, 160);  // Tor knarrt
  spieleMelodie(s, M(MEL_N), a + 14, a + 27.6f, 0.46f, t0, t1, 0.35f);
  a = P12;
  for (int i = 0; i < 4; i++) if (bei(a + 0.4f + i * 1.2f, t0, t1)) s.glocke(a + 0.4f + i * 1.2f, i & 1 ? 69 : 74, 0.6f);
  schellen(s, a + 0.4f, a + 6, t0, t1);
  spieleMelodie(s, M(MEL_SCHLITTEN), a + 0.8f, a + 18.6f, 0.3f, t0, t1, 0.7f);
  endeKlang(s, a + 19.4f, t0, t1);
}

// =====================================================================
// Startbild, Vorschau, Erkennungszeichen
// =====================================================================
static void titelSzene(Gfx &g, float t) {
  winterwald(g, t, 0.9f, 0.3f);
  schneeFall(g, t, 24);
  frost(g, 210, 210, 0.82f, -1, t, 0.3f, 1.4f);
  marf(g, 120, 226, 0.85f, 1, t, 0.4f, 0.8f, -1, true);
  truhe(g, 160, 226, 0.8f, t, 1);
}
static void titelbild(Gfx &g, float t) { titelSzene(g, t); titelRahmen(g, t, "Väterchen Frost", 196, SCHRIFT_M); }
static void vorschau(Gfx &g, float t) { titelSzene(g, t); }
static void symbol(Gfx &g, float x, float y, float t) {
  g.stern(x, y - 36, 30, 10, 6, t * 0.5f, WEISS);
  g.stern(x, y - 36, 18, 6, 6, -t * 0.7f, EIS);
  glitzern(g, x, y - 36, 44, t, 6);
}

const Maerchen FROST = {"Väterchen Frost", PDAUER, FARBE(0x9FD0F8), bild, ton, titelbild, vorschau, symbol,
                        LIED(MEL_FROST, 0.4f), "aus Russland"};

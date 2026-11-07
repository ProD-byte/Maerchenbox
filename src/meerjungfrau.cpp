// Die kleine Meerjungfrau - ein Spieluhr-Film fuer die Maerchenbox, nach Hans Christian Andersen (1837).
// Der Palast auf dem Meeresgrund, der fuenfzehnte Geburtstag, das Schiff und der Sturm, der Prinz am Tempel,
// die Meerhexe und der Preis (die Stimme), die Beine, das Findelkind, die Hochzeit mit dem Tempelmaedchen,
// das Messer der Schwestern (nur angedeutet), und die Toechter der Luft.
#include "erzaehlen.h"

// ---------- Ablauf (Sekunden) ----------
static const float P1 = 0, P2 = 24, P3 = 48, P4 = 72, P5 = 96, P6 = 120, P7 = 144, P8 = 168, P9 = 190, P10 = 216,
                   P11 = 240, P12 = 264, P13 = 290, P14 = 316, P15 = 340, PDAUER = 364;

static Aussehen prinzLook() {
  Aussehen a; a.weste = FARBE(0x2E5FAE); a.hemd = WEISS; a.hose = FARBE(0x2E3A5A); a.haar = FARBE(0x2A1E16);
  a.muetze = 1; a.halstuch = GOLD;
  return a;
}
static Aussehen matroseLook() {
  Aussehen a; a.weste = WEISS; a.hemd = FARBE(0x3E7FDC); a.hose = FARBE(0x2E3A5A); a.haar = FARBE(0x8E5A2A); a.muetze = 1;
  return a;
}
static const Aussehen PRINZ = prinzLook(), MATROSE = matroseLook();
static FrauLook nixeLook(int i) {
  FrauLook l;
  const Farbe h[] = {FARBE(0xE87A3A), FARBE(0xF2C94E), FARBE(0x8E5A2A), FARBE(0xC0392B), FARBE(0x3A2A20), FARBE(0xF2E8A0)};
  const Farbe k[] = {FARBE(0x5ED0A8), FARBE(0x8ECCF0), FARBE(0xB28DFF), FARBE(0xFF9AC0), FARBE(0x7EC0EE), FARBE(0xF2C94E)};
  l.haar = h[i]; l.kleid = k[i]; l.frisur = 0; l.blume = i == 0 ? FARBE(0xFFFFFF) : 0;
  return l;
}
static FrauLook menschLook() {  // die kleine Meerjungfrau als Menschenmaedchen
  FrauLook l = nixeLook(0); l.kleid = FARBE(0xE8F2FF); l.glanz = true; l.rockBreite = 1.15f; l.schuh = FARBE(0xE8E0F0);
  return l;
}
static FrauLook omaLook() {
  FrauLook l; l.haar = FARBE(0xE8E8F0); l.kleid = FARBE(0x6EA8C8); l.frisur = 2; l.krone = true; l.wangen = false;
  return l;
}
static FrauLook tempelLook() {
  FrauLook l; l.kleid = FARBE(0xFF8FC0); l.haar = FARBE(0x3A2A20); l.frisur = 0; l.glanz = true; l.rockBreite = 1.2f;
  return l;
}
static FrauLook hexeLook() {
  FrauLook l; l.kleid = FARBE(0x3E2A4E); l.haar = FARBE(0x5A6E5A); l.frisur = 0; l.haut = FARBE(0xA8C8A8); l.wangen = false;
  return l;
}
static const FrauLook NIXE[] = {nixeLook(0), nixeLook(1), nixeLook(2), nixeLook(3), nixeLook(4), nixeLook(5)};
static const FrauLook MENSCH = menschLook(), OMA = omaLook(), TEMPEL = tempelLook(), HEXE = hexeLook();
static const Farbe MEER_O = FARBE(0x3E8ED8), MEER_U = FARBE(0x1E3A7A);

// =====================================================================
// Figuren
// =====================================================================
// Meerjungfrau: Frauenfigur, deren Rock ein Fischschwanz ist; schwimm = Schwanzschlag
static void nixe(Gfx &g, const FrauLook &L, float x, float y, float s, float dir, float t, float mund, float armV = 0.6f,
                 float schwimm = 0, bool weint = false, float armH = 0.4f, uint8_t a = 255, int austern = 0) {
  auto PX = [&](float d) { return x + d * s * dir; };
  auto PY = [&](float d) { return y + d * s; };
  float w = sinf(t * 4 + x * 0.01f) * (3 + schwimm * 6);
  Farbe sch = mischeFarbe(L.kleid, FARBE(0x2E6E6E), 0.35f);
  FrauPose p = fposis(L, t);
  p.mund = mund;
  p.armV = armV;
  p.armH = armH;
  p.weint = weint;
  frau(g, x, y - 6 * s, s, dir, p);
  // der Schwanz deckt den Rock
  g.kontur(1.3f * s, OUT);
  float sw[] = {PX(-11), PY(-44), PX(11), PY(-44), PX(9 + w * 0.3f), PY(-20), PX(2 + w), PY(-6), PX(-4 + w), PY(-6), PX(-9 + w * 0.3f), PY(-22)};
  g.polygon(sw, 6, sch, a);
  float fl[] = {PX(-1 + w), PY(-8), PX(-16 + w * 1.4f), PY(4), PX(-4 + w * 1.2f), PY(0), PX(4 + w * 1.2f), PY(4), PX(14 + w * 1.4f), PY(0)};
  g.polygon(fl, 5, mischeFarbe(sch, WEISS, 0.2f), a);
  g.konturAus();
  for (int i = 0; i < 4; i++) g.bogen(PX(0 + w * i * 0.1f), PY(-38 + i * 7), 5 * s, 0.3f, 2.8f, 1 * s, mischeFarbe(sch, WEISS, 0.35f), a);
  for (int i = 0; i < austern; i++) g.ellipse(PX(-6 + (i % 4) * 4 + w * 0.3f), PY(-24 + (i / 4) * 5), 2.4f * s, 1.6f * s, WEISS, a);
}
static void prinz(Gfx &g, float x, float y, float s, float dir, float t, float mund, float armV = 0.4f, float lauf = -1,
                  bool zu = false, float armH = 0.2f) {
  Pose p = posis(PRINZ, t);
  p.mund = mund;
  p.armV = armV;
  p.armH = armH;
  p.lauf = lauf;
  p.zu = zu || p.zu;
  schneiderlein(g, x, y, s, dir, p);
}
static void maedchen(Gfx &g, const FrauLook &L, float x, float y, float s, float dir, float t, float mund, float armV = 0.4f,
                     float lauf = -1, bool weint = false, float armH = 0.2f, float drehen = 0) {
  FrauPose p = fposis(L, t, 1);
  p.mund = mund;
  p.armV = armV;
  p.armH = armH;
  p.lauf = lauf;
  p.weint = weint;
  p.drehen = drehen;
  frau(g, x, y, s, dir, p);
}
static void meerhexe(Gfx &g, float x, float y, float s, float dir, float t, float mund, float armV = 1.2f) {
  // Polypenarme
  for (int i = 0; i < 6; i++) {
    float a = 3.4f + i * 0.5f, w = sinf(t * 2 + i) * 0.3f;
    g.kurve(x, y - 20 * s, x + cosf(a + w) * 30 * s, y - 10 * s + sinf(a + w) * 10 * s, x + cosf(a + w * 2) * 50 * s,
            y + sinf(a) * 6 * s, 5 * s, FARBE(0x5A3A6E));
  }
  FrauPose p = fposis(HEXE, t, 3);
  p.mund = mund;
  p.armV = armV;
  p.boese = 0.8f;
  frau(g, x, y, s, dir, p);
  g.ellipse(x, y - 4 * s, 26 * s, 10 * s, FARBE(0x4E2A5E));
}
static void tochterDerLuft(Gfx &g, float x, float y, float s, float t, uint8_t a) {
  g.kreis(x, y - 40 * s, 30 * s, WEISS, (uint8_t)(a * 0.2f));
  float k[] = {x - 12 * s, y, x + 12 * s, y, x + 6 * s, y - 40 * s, x - 6 * s, y - 40 * s};
  g.polygon(k, 4, FARBE(0xF6F8FF), a);
  g.kreis(x, y - 50 * s, 9 * s, FARBE(0xFFE8D8), a);
  g.ellipse(x - 6 * s, y - 52 * s, 10 * s, 6 * s, NIXE[0].haar, a);
  for (int i = 0; i < 2; i++) g.ellipse(x + (i ? 14 : -14) * s, y - 32 * s, 12 * s, 5 * s, WEISS, (uint8_t)(a * 0.6f), (i ? 0.5f : -0.5f) + sinf(t * 3) * 0.2f);
}

// =====================================================================
// Orte und Dinge
// =====================================================================
static void meeresgrund(Gfx &g, float t, float dunkel = 0) {
  g.verlauf(0, 240, mischeFarbe(MEER_O, FARBE(0x1E2A4E), dunkel), mischeFarbe(MEER_U, FARBE(0x0E1428), dunkel));
  for (int i = 0; i < 5; i++) {  // Lichtstrahlen
    float x = 40 + i * 56 + sinf(t * 0.3f + i) * 10;
    float r[] = {x - 8, 0, x + 8, 0, x + 30, 220, x + 10, 220};
    g.polygon(r, 4, WEISS, (uint8_t)(24 * (1 - dunkel)));
  }
  g.rechteck(0, 210, 284, 30, mischeFarbe(FARBE(0xE8D8A8), FARBE(0x4A4A5A), dunkel));
  for (int i = 0; i < 8; i++) {  // Seegras
    float x = 10 + i * 38, w = sinf(t * 1.5f + i) * 8;
    g.kurve(x, 214, x + w, 180, x - w * 0.5f, 140 + (i % 3) * 14, 3, FARBE(0x3E8E5E));
  }
  for (int i = 0; i < 10; i++) {
    float k = fmodf(t * 0.3f + zufall(i) , 1);
    g.kreis(zufall(i + 30) * 284, 220 - k * 230, 1.6f + zufall(i) * 2, WEISS, (uint8_t)(140 * (1 - k)));
  }
}
static void palast(Gfx &g, float t) {
  g.kontur(1.6f, OUT);
  g.rechteck(150, 110, 120, 100, FARBE(0xF2B0A8));
  for (int i = 0; i < 3; i++) {
    float x = 160 + i * 50;
    g.rechteck(x, 80 - (i == 1 ? 20 : 0), 20, 40, FARBE(0xF2B0A8));
    float d[] = {x - 4, 82 - (i == 1 ? 20 : 0), x + 10, 56 - (i == 1 ? 20 : 0), x + 24, 82 - (i == 1 ? 20 : 0)};
    g.polygon(d, 3, FARBE(0x8ECCF0));
  }
  g.konturAus();
  for (int i = 0; i < 4; i++) g.rundRechteck(162 + i * 26, 140, 14, 22, 6, FARBE(0xFFE8A0));
  for (int i = 0; i < 8; i++) g.ellipse(156 + i * 14, 112, 5, 3, WEISS);  // Muscheln auf dem Dach
}
static void garten(Gfx &g, float t) {  // ihr runder Garten mit roten Blumen und der Marmorstatue
  for (int i = 0; i < 9; i++) {
    float a = i * 0.698f;
    g.kreis(80 + cosf(a) * 30, 214 + sinf(a) * 6, 6, FARBE(0xE8283F));
  }
  g.kreis(80, 214, 8, FARBE(0xFF5A4A));
  g.kontur(1, OUT);
  g.rechteck(100, 166, 14, 46, WEISS);
  g.kreis(107, 160, 8, WEISS);
  g.konturAus();
}
static void oberflaeche(Gfx &g, float t, float nacht, float sturm = 0) {
  himmel(g, nacht, t, 150);
  if (nacht > 0.5f) for (int i = 0; i < 16; i++) g.stern(zufall(i + 700) * 284, zufall(i + 720) * 100, 2, 0.8f, 4, 0, WEISS, 160);
  if (sturm > 0) {
    g.rechteck(0, 0, 284, 150, FARBE(0x1A1A2A), (uint8_t)(200 * sturm));
    if (fmodf(t, 2.7f) < 0.2f) { float b[] = {170, 0, 180, 0, 166, 60, 176, 60, 156, 130, 168, 70, 158, 70}; g.polygon(b, 7, FARBE(0xFFF6B0)); }
  }
  g.verlauf(150, 240, mischeFarbe(FARBE(0x3E8ED8), FARBE(0x1A2A4E), nacht), mischeFarbe(FARBE(0x1E5AA8), FARBE(0x0E1A30), nacht));
  for (int r = 0; r < 4; r++)
    for (int i = 0; i < 8; i++) {
      float x = fmodf(i * 40 + r * 17 + t * 12, 320) - 20, y = 156 + r * 22;
      float h = 3 + sturm * 16 * (0.6f + 0.4f * sinf(t * 2 + i + r));
      float wv[] = {x - 20, y, x, y - h, x + 20, y};
      g.polygon(wv, 3, mischeFarbe(FARBE(0x3E8ED8), WEISS, 0.25f), 160);
    }
}
static void schiff(Gfx &g, float x, float y, float t, float kipp = 0, float bruch = 0) {
  float c = cosf(kipp), sn = sinf(kipp);
  auto P = [&](float dx, float dy, float *o) { o[0] = x + dx * c - dy * sn; o[1] = y + dx * sn + dy * c; };
  float r[8];
  P(-70, -20, r); P(70, -20, r + 2); P(54, 10, r + 4); P(-54, 10, r + 6);
  g.kontur(1.6f, OUT);
  g.polygon(r, 4, HOLZ);
  g.konturAus();
  for (int i = 0; i < 6; i++) { float l[2]; P(-50 + i * 20, -12, l); g.kreis(l[0], l[1], 3, FARBE(0xFFD27A)); }
  float m0[2], m1[2];
  P(0, -20, m0); P(bruch * 30, -110 + bruch * 40, m1);
  g.linie(m0[0], m0[1], m1[0], m1[1], 4, HOLZ_D);
  if (bruch < 0.5f) {
    float s1[2], s2[2], s3[2];
    P(4, -100, s1); P(4, -30, s2); P(50, -40, s3);
    float se[] = {s1[0], s1[1], s2[0], s2[1], s3[0], s3[1]};
    g.polygon(se, 3, WEISS);
    for (int i = 0; i < 5; i++) { float l[2]; P(-60 + i * 12, -60 - i * 8, l); g.kreis(l[0], l[1], 2.4f, i & 1 ? FARBE(0xE8473D) : GOLD); }
  }
}
static void tempelUfer(Gfx &g, float t) {
  himmel(g, 0, t, 160);
  sonne(g, 240, 34, 11, t);
  g.rechteck(0, 150, 284, 90, FARBE(0x3E8ED8));
  float st[] = {0, 240, 0, 190, 150, 176, 220, 240};
  g.polygon(st, 4, FARBE(0xE8D8A8));
  // Tempel
  g.kontur(1.6f, OUT);
  g.rechteck(10, 120, 90, 60, WEISS);
  float d[] = {4, 122, 55, 96, 106, 122};
  g.polygon(d, 3, FARBE(0xF2EEE4));
  g.konturAus();
  for (int i = 0; i < 5; i++) g.rechteck(16 + i * 18, 126, 6, 54, FARBE(0xE8E0D0));
  // Felsen (Versteck)
  g.kontur(1.4f, OUT);
  g.ellipse(240, 196, 34, 22, STEIN);
  g.ellipse(214, 206, 24, 14, STEIN_D);
  g.konturAus();
}
static void schlossSaal(Gfx &g, float t) {
  g.verlauf(0, 200, FARBE(0xF6E2B4), FARBE(0xE8C483));
  for (int i = 0; i < 4; i++) {
    float x = 20 + i * 82;
    g.rechteck(x - 10, 30, 20, 160, FARBE(0xFFF3D6));
    g.rechteck(x - 14, 26, 28, 8, FARBE(0xE8D3A2));
  }
  g.rechteck(0, 192, 284, 48, FARBE(0xC9A46A));
  for (int r = 0; r < 3; r++)
    for (int c = 0; c < 12; c++)
      if ((r + c) & 1) g.rechteck(c * 26 - r * 8, 192 + r * 16, 26, 16, FARBE(0xA8823E));
}

// =====================================================================
// Szenen
// =====================================================================
static void szeneGrund(Gfx &g, float u) {
  meeresgrund(g, u);
  palast(g, u);
  garten(g, u);
  nixe(g, OMA, 210, 214, 0.8f, -1, u, u > 8.4f && u < 22 ? 0.4f + 0.2f * sinf(u * 8) : 0, 1);
  nixe(g, NIXE[0], 70, 196, 0.78f, 1, u, 0.2f, 0.8f, 0.3f);
  g.kameraAus();
  vers(g, {"Tief unten im Meer stand das", "Schloss des Meerkönigs. Er hatte", "sechs Töchter."}, aufzu(u, 0.4f, 8));
  vers(g, {"Die jüngste war still und nach-", "denklich. Ihr Garten war rund und", "rot wie die Sonne da oben."}, aufzu(u, 8.4f, 15));
  blase(g, 190, 50, {"Oben gibt es Schiffe, Städte,", "Wälder - und Blumen, die duften!"}, aufzu(u, 15.4f, 23.6f), 210, 150);
}

static void szeneSchwestern(Gfx &g, float u) {
  meeresgrund(g, u);
  palast(g, u);
  for (int i = 1; i < 6; i++) {
    float k = seg(u, i * 3, i * 3 + 4);
    nixe(g, NIXE[i], 60 + i * 32, lerp(214, -40, hinein(k)), 0.62f, 1, u, 0.3f, 2.4f, k > 0 && k < 1 ? 1 : 0, false, 0.4f, 255);
  }
  nixe(g, NIXE[0], 60, 214, 0.7f, 1, u, 0, 0.4f, 0, u > 18);
  g.kameraAus();
  vers(g, {"Mit fünfzehn Jahren durfte jede", "Schwester hinauf ans Licht."}, aufzu(u, 0.4f, 6.4f));
  vers(g, {"Sie erzählten von Lichtern und", "Glocken, von Schwänen, Delfinen", "und Eisbergen."}, aufzu(u, 6.8f, 15));
  vers(g, {"Die Jüngste musste warten.", "Wie sehnte sie sich hinauf!"}, aufzu(u, 15.4f, 23.6f));
}

static void szeneGeburtstag(Gfx &g, float u) {
  meeresgrund(g, u);
  palast(g, u);
  float auf = seg(u, 14, 20);
  nixe(g, OMA, 180, 214, 0.8f, -1, u, (u > 0.4f && u < 5) || (u > 9.4f && u < 13) ? 0.5f : 0, u < 5 ? 1.8f : 0.8f);
  nixe(g, NIXE[0], 120, lerp(214, -40, hinein(auf)), 0.78f, 1, u, u > 5.4f && u < 9 ? 0.7f : 0.3f, auf > 0 ? 2.6f : 0.8f, auf, false, 0.4f, 255,
       u > 4 ? 8 : 0);
  if (u > 3 && auf < 0.2f) for (int i = 0; i < 6; i++) g.kreis(108 + i * 5, 136 - (i & 1) * 3, 3, WEISS);  // Lilienkranz
  g.kameraAus();
  blase(g, 200, 50, {"Komm, lass dich schmücken!", "Acht Austern an den Schwanz -", "wie es sich für Prinzessinnen gehört."}, aufzu(u, 0.4f, 5), 180, 150);
  blase(g, 90, 50, {"Au, das tut weh!"}, aufzu(u, 5.4f, 9), 120, 140);
  blase(g, 200, 50, {"Ja - Eitelkeit muss leiden!"}, aufzu(u, 9.4f, 13), 180, 150);
  vers(g, {"Leicht wie eine Blase stieg sie", "hinauf durch das Wasser."}, aufzu(u, 14, 23.6f));
}

static void szeneSchiff(Gfx &g, float u) {
  oberflaeche(g, u, 0.8f);
  schiff(g, 170, 168, u, sinf(u) * 0.03f);
  prinz(g, 150, 148, 0.6f, -1, u, 0.5f, 1.2f);
  for (int i = 0; i < 3; i++) feuerwerk(g, 120 + i * 50, 40 + (i & 1) * 20, fmodf(u * 0.4f + i * 0.33f, 1), i == 0 ? GOLD : i == 1 ? FARBE(0xFF8FB0) : FARBE(0x9FD8FF));
  nixe(g, NIXE[0], 60, 196, 0.7f, 1, u, 0.4f, 1.6f, 0.3f);
  g.ausschnitt(0, 0, 284, 196);
  g.ausschnittAus();
  g.kameraAus();
  vers(g, {"Oben lag ein großes Schiff. Es", "wurde Geburtstag gefeiert - mit", "Musik und Feuerwerk."}, aufzu(u, 0.4f, 9));
  vers(g, {"Der Prinz war gerade sechzehn.", "Sie konnte die Augen nicht", "von ihm wenden."}, aufzu(u, 9.4f, 23.6f));
}

static void szeneSturm(Gfx &g, float u) {
  float st = seg(u, 0, 4);
  oberflaeche(g, u, 0.8f, st);
  float bruch = seg(u, 7, 10);
  if (bruch < 1) schiff(g, 170, 168 + bruch * 40, u, sinf(u * 2) * 0.2f * st + bruch * 0.6f, bruch);
  float pk = seg(u, 10, 13);
  if (u > 10) {
    float px = lerp(160, 130, pk), py = lerp(160, 196, pk);
    prinz(g, px, py, 0.6f, -1, u, 0, 2.6f, -1, true, 2.6f);
  }
  if (u > 12) nixe(g, NIXE[0], 116, 214, 0.7f, 1, u, 0.3f, 2.2f, 0.4f);
  g.kameraAus();
  vers(g, {"Da kam ein Sturm auf. Die Wellen", "türmten sich wie schwarze Berge."}, aufzu(u, 0.4f, 6.6f));
  vers(g, {"Das Schiff zerbrach, und der", "Prinz versank in der Tiefe..."}, aufzu(u, 7, 12));
  vers(g, {"Sie tauchte zu ihm und hielt", "seinen Kopf über das Wasser."}, aufzu(u, 13, 23.6f));
}

static void szeneTempel(Gfx &g, float u) {
  tempelUfer(g, u);
  prinz(g, 150, 196, 0.62f, -1, u, u > 16 ? 0.4f : 0, 0.4f, -1, u < 15);
  bool versteckt = u > 6;
  if (!versteckt) nixe(g, NIXE[0], 180, 214, 0.7f, -1, u, 0.3f, 1.4f, 0.2f);
  else nixe(g, NIXE[0], 250, 196, 0.6f, -1, u, 0, 0.4f, 0, u > 16);
  float k = seg(u, 9, 13);
  if (u > 9) maedchen(g, TEMPEL, lerp(-20, 116, k), 200, 0.66f, 1, u, u > 15 ? 0.5f : 0, u > 13 ? 1.6f : 0.4f, k < 1 ? fmodf(u * 1.4f, 1) : -1);
  if (versteckt) {  // Felsen vor ihr
    g.kontur(1.4f, OUT);
    g.ellipse(240, 206, 30, 16, STEIN);
    g.konturAus();
  }
  g.kameraAus();
  vers(g, {"Am Morgen legte sie ihn in den", "warmen Sand vor einem Tempel."}, aufzu(u, 0.4f, 6));
  vers(g, {"Sie versteckte sich hinter den", "Felsen. Ein junges Mädchen", "kam aus dem Tempel..."}, aufzu(u, 6.4f, 14.6f));
  vers(g, {"Der Prinz erwachte und lächelte", "sie an. Er wusste nicht, wer ihn", "in Wahrheit gerettet hatte."}, aufzu(u, 15, 23.6f));
}

static void szeneSeele(Gfx &g, float u) {
  meeresgrund(g, u, 0.2f);
  palast(g, u);
  nixe(g, OMA, 190, 214, 0.8f, -1, u, (u > 6.4f) ? 0.5f + 0.2f * sinf(u * 8) : 0, 1);
  nixe(g, NIXE[0], 100, 214, 0.78f, 1, u, u > 0.4f && u < 6 ? 0.5f : 0, 0.6f, 0, u > 18);
  g.kameraAus();
  blase(g, 110, 50, {"Großmutter, leben die Menschen", "für immer?"}, aufzu(u, 0.4f, 6), 100, 140);
  blase(g, 190, 50, {"Wir werden dreihundert Jahre alt", "und dann zu Schaum auf dem Meer.", "Die Menschen haben eine Seele."}, aufzu(u, 6.4f, 13), 190, 150);
  blase(g, 190, 50, {"Nur wenn ein Mensch dich lieber", "hat als Vater und Mutter, bekämst", "du auch eine Seele."}, aufzu(u, 13.4f, 23.6f), 190, 150);
}

static void szeneWeg(Gfx &g, float u) {
  meeresgrund(g, u, 0.7f);
  // Strudel und Polypen
  for (int i = 0; i < 3; i++) {
    float x = 60 + i * 80;
    for (int k = 0; k < 4; k++) g.bogen(x, 120, 8 + k * 8, u * 3 + k, u * 3 + k + 4, 2, WEISS, 120);
  }
  for (int i = 0; i < 7; i++) {
    float x = 20 + i * 42, w = sinf(u * 2 + i) * 12;
    g.kurve(x, 214, x + w, 170, x - w, 140, 4, FARBE(0x6E4A7A));
    g.kreis(x - w, 140, 5, FARBE(0x8E6A9A));
  }
  float k = seg(u, 0, 20);
  nixe(g, NIXE[0], lerp(-20, 300, k), 180 + sinf(u * 2) * 10, 0.7f, 1, u, 0, 2.4f, 1, false, 2.4f);
  g.kameraAus();
  vers(g, {"Heimlich schwamm sie zur Meerhexe -", "durch brausende Strudel und einen", "Wald aus Polypen."}, aufzu(u, 0.4f, 11));
  vers(g, {"Ihr Herz klopfte vor Angst. Aber", "sie dachte an den Prinzen."}, aufzu(u, 11.4f, 21.6f));
}

static void szeneHexe(Gfx &g, float u) {
  meeresgrund(g, u, 0.8f);
  g.kontur(1.4f, OUT);
  g.rundRechteck(150, 90, 120, 120, 20, FARBE(0xE8E0D0));  // Haus aus Knochen
  g.konturAus();
  for (int i = 0; i < 6; i++) g.linie(156 + i * 20, 96, 156 + i * 20, 206, 3, FARBE(0xC8C0B0));
  meerhexe(g, 200, 214, 0.9f, -1, u, (u > 0.4f && u < 20) ? 0.5f + 0.3f * sinf(u * 9) : 0, u > 20 ? 1.8f : 1.2f);
  nixe(g, NIXE[0], 90, 214, 0.78f, 1, u, 0, u > 21 ? 1.6f : 0.6f, 0, u > 12 && u < 20);
  if (u > 21) {
    g.kontur(1, OUT);
    g.rundRechteck(128, 160, 10, 16, 3, FARBE(0x8ECCF0), 200);
    g.konturAus();
    glitzern(g, 133, 168, 14, u, 4);
  }
  g.kameraAus();
  blase(g, 180, 40, {"Ich weiß schon, was du willst.", "Es ist dumm von dir - aber", "du sollst deinen Willen haben."}, aufzu(u, 0.4f, 6.6f), 196, 120);
  blase(g, 180, 40, {"Jeder Schritt wird sein, als", "trätest du auf scharfe Messer."}, aufzu(u, 7, 12), 196, 120);
  blase(g, 180, 40, {"Heiratet er eine andere, wirst", "du Schaum auf dem Meer. Und", "du musst mir deine Stimme geben."}, aufzu(u, 12.4f, 19.6f), 196, 120);
  vers(g, {"Die kleine Meerjungfrau gab ihre", "Stimme - und bekam den Trank."}, aufzu(u, 20.4f, 25.6f));
}

static void szeneBeine(Gfx &g, float u) {
  himmel(g, 0.2f, u, 160);
  sonne(g, 240, lerp(70, 40, seg(u, 0, 10)), 11, u);
  g.rechteck(0, 150, 284, 90, FARBE(0x3E8ED8));
  float st[] = {0, 240, 0, 186, 284, 176, 284, 240};
  g.polygon(st, 4, FARBE(0xF2EEE4));  // Marmortreppe
  for (int i = 0; i < 3; i++) g.linie(0, 196 + i * 14, 284, 186 + i * 14, 1.4f, FARBE(0xC8C0B0));
  bool beine = u > 5;
  if (!beine) nixe(g, NIXE[0], 120, 214, 0.78f, 1, u, 0, 2.2f);
  else maedchen(g, MENSCH, 120, 222, 0.78f, 1, u, 0, u > 14 ? 0.4f : 1.4f, -1, false, 0.2f);
  puff(g, 120, 180, seg(u, 4.4f, 6));
  float k = seg(u, 9, 13);
  if (u > 9) prinz(g, lerp(330, 180, k), 222, 0.85f, -1, u, u > 13.4f && u < 18 ? 0.5f : 0, u > 13 ? 1.4f : 0.4f, k < 1 ? fmodf(u * 1.4f, 1) : -1);
  g.kameraAus();
  vers(g, {"Vor dem Schloss des Prinzen trank", "sie den Trank. Statt des Schwanzes", "hatte sie nun zwei hübsche Beine."}, aufzu(u, 0.4f, 9));
  blase(g, 190, 50, {"Wer bist du? Woher", "kommst du denn?"}, aufzu(u, 13.4f, 18), 180, 140);
  vers(g, {"Sie konnte nicht antworten. Sie", "sah ihn nur sanft und traurig an."}, aufzu(u, 18.4f, 23.6f));
}

static void szeneFindelkind(Gfx &g, float u) {
  if (u < 14) {
    schlossSaal(g, u);
    float d = sinf(u * 2) * 0.6f;
    maedchen(g, MENSCH, 140, 222 - fabsf(sinf(u * 3)) * 4, 0.8f, sinf(u) > 0 ? 1 : -1, u, 0.2f, 2.2f + d, -1, false, 2.2f - d, sinf(u * 3) * 4);
    prinz(g, 70, 222, 0.85f, 1, u, u > 6.4f && u < 13 ? 0.5f : 0.3f, 1.2f);
    g.kameraAus();
    vers(g, {"Sie tanzte, wie noch niemand", "getanzt hatte - obwohl jeder", "Schritt wie Messer schmerzte."}, aufzu(u, 0.4f, 6));
    blase(g, 90, 50, {"Du bist mein liebes", "kleines Findelkind!"}, aufzu(u, 6.4f, 13), 70, 140);
    return;
  }
  float v = u - 14;
  oberflaeche(g, u, 1);
  float st[] = {0, 240, 0, 196, 284, 190, 284, 240};
  g.polygon(st, 4, FARBE(0xC8C0B0));
  maedchen(g, MENSCH, 80, 222, 0.75f, 1, u, 0, 0.6f, -1, v > 4);
  for (int i = 1; i < 6; i++) nixe(g, NIXE[i], 140 + i * 26, 190, 0.5f, -1, u, 0.3f, 1.8f, 0.2f, true);
  g.kameraAus();
  vers(g, {"Nachts kamen ihre Schwestern und", "sangen traurig über das Wasser."}, aufzu(v, 0.4f, 9.6f));
}

static void szeneBraut(Gfx &g, float u) {
  tempelUfer(g, u);
  prinz(g, 150, 222, 0.85f, -1, u, (u > 0.4f && u < 8) || (u > 14.4f && u < 20) ? 0.5f + 0.2f * sinf(u * 8) : 0.3f, u > 14 ? 2.2f : 1.2f);
  maedchen(g, MENSCH, 210, 222, 0.75f, -1, u, 0, 0.4f, -1, u > 20);
  float k = seg(u, 9, 13);
  if (u > 9) maedchen(g, TEMPEL, lerp(-20, 100, k), 222, 0.8f, 1, u, u > 13 ? 0.4f : 0, 0.8f, k < 1 ? fmodf(u * 1.4f, 1) : -1);
  if (u > 15) herzen(g, 124, 150, u, 4, 20);
  g.kameraAus();
  blase(g, 200, 50, {"Ich soll die Königstochter", "heiraten. Aber lieben könnte ich", "nur das Mädchen aus dem Tempel."}, aufzu(u, 0.4f, 8), 150, 140);
  vers(g, {"Da kam die Braut - und es war", "das Mädchen aus dem Tempel!"}, aufzu(u, 9, 14));
  blase(g, 200, 50, {"Du bist es! Du hast mich", "am Strand gerettet!"}, aufzu(u, 14.4f, 20), 150, 140);
  vers(g, {"Die kleine Meerjungfrau küsste", "seine Hand. Ihr Herz wollte brechen."}, aufzu(u, 20.4f, 25.6f));
}

static void szeneMesser(Gfx &g, float u) {
  oberflaeche(g, u, 1 - seg(u, 18, 26) * 0.5f);
  schiff(g, 150, 168, u);
  prinz(g, 110, 148, 0.5f, 1, u, 0, 0.4f, -1, true);
  maedchen(g, TEMPEL, 130, 148, 0.48f, -1, u, 0, 0.4f);
  maedchen(g, MENSCH, 196, 148, 0.55f, -1, u, 0, u > 14 && u < 17 ? 2.6f : 0.6f);
  for (int i = 1; i < 6; i++) {
    FrauLook kurz = NIXE[i];
    kurz.frisur = 4;
    nixe(g, kurz, 210 + i * 14, 210, 0.5f, -1, u, i == 3 && u > 4 && u < 13 ? 0.5f : 0.2f, 2, 0.2f, false, 0.4f);
  }
  float wurf = seg(u, 15, 17.4f);
  if (u > 4 && wurf <= 0) g.linie(232, 180, 240, 172, 2, STEIN);
  if (wurf > 0 && wurf < 1) { float x = lerp(200, 270, wurf), y = lerp(120, 200, wurf) - huepf(wurf) * 40; g.linie(x - 4, y + 4, x + 4, y - 4, 2, STEIN); }
  g.kameraAus();
  vers(g, {"In der Hochzeitsnacht kamen die", "Schwestern - ihr langes Haar", "hatten sie der Hexe gegeben."}, aufzu(u, 0.4f, 4));
  blase(g, 200, 70, {"Die Hexe gab uns dies Messer.", "Nur wenn der Prinz stirbt, wirst du", "wieder eine von uns!"}, aufzu(u, 4.4f, 12.6f), 252, 180);
  vers(g, {"Sie sah den schlafenden Prinzen", "an - und warf das Messer weit", "hinaus in die Wellen."}, aufzu(u, 13, 25.6f));
}

static void szeneLuft(Gfx &g, float u) {
  oberflaeche(g, u, 0.3f);
  sonne(g, 142, lerp(160, 60, seg(u, 0, 8)), 14, u);
  float schaum = seg(u, 2, 6);
  if (schaum < 1) maedchen(g, MENSCH, 142, lerp(150, 190, schaum), 0.6f, 1, u, 0, 2.6f, -1, false, 2.6f);
  if (schaum > 0) for (int i = 0; i < 10; i++) g.kreis(120 + i * 5, 186 + sinf(u * 3 + i) * 3, 4, WEISS, (uint8_t)(200 * (1 - seg(u, 8, 12))));
  float auf = seg(u, 8, 14);
  if (auf > 0) {
    tochterDerLuft(g, 142, lerp(190, 120, auf), 0.9f, u, (uint8_t)(220 * auf));
    for (int i = 0; i < 4; i++) tochterDerLuft(g, 60 + i * 56 + (i > 1 ? 30 : 0), 100 + sinf(u + i) * 10, 0.6f, u + i, (uint8_t)(150 * auf));
  }
  g.kameraAus();
  vers(g, {"Als die Sonne aufging, stürzte sie", "sich ins Meer und wurde Schaum..."}, aufzu(u, 0.4f, 7.6f));
  vers(g, {"Doch sie starb nicht. Sie stieg", "hinauf zu den Töchtern der Luft."}, aufzu(u, 8, 14));
  blase(g, 200, 50, {"Durch gute Taten bekommen wir", "eine Seele, die ewig lebt."}, aufzu(u, 14.4f, 21.6f), 200, 110);
}

static void szeneEnde(Gfx &g, float u) {
  oberflaeche(g, u, 0.1f);
  sonne(g, 240, 34, 12, u);
  schiff(g, 150, 168, u);
  prinz(g, 120, 148, 0.5f, 1, u, 0, 0.4f, -1, false);
  maedchen(g, TEMPEL, 142, 148, 0.48f, -1, u, 0, 0.4f);
  float auf = seg(u, 8, 18);
  tochterDerLuft(g, lerp(150, 200, auf), lerp(110, 30, auf), 0.7f, u, 220);
  herzen(g, 140, 120, u, 3, 20);
  g.kameraAus();
  vers(g, {"Unsichtbar küsste sie die Braut auf", "die Stirn und lächelte dem Prinzen zu."}, aufzu(u, 0.4f, 8));
  vers(g, {"Dann stieg sie mit den anderen", "Kindern der Luft zu den rosigen", "Wolken hinauf."}, aufzu(u, 8.4f, 18));
  endeBild(g, u, 19, 150, 130);
}

static void bild(Gfx &g, float t) {
  if (t < P2) szeneGrund(g, t - P1);
  else if (t < P3) szeneSchwestern(g, t - P2);
  else if (t < P4) szeneGeburtstag(g, t - P3);
  else if (t < P5) szeneSchiff(g, t - P4);
  else if (t < P6) szeneSturm(g, t - P5);
  else if (t < P7) szeneTempel(g, t - P6);
  else if (t < P8) szeneSeele(g, t - P7);
  else if (t < P9) szeneWeg(g, t - P8);
  else if (t < P10) szeneHexe(g, t - P9);
  else if (t < P11) szeneBeine(g, t - P10);
  else if (t < P12) szeneFindelkind(g, t - P11);
  else if (t < P13) szeneBraut(g, t - P12);
  else if (t < P14) szeneMesser(g, t - P13);
  else if (t < P15) szeneLuft(g, t - P14);
  else szeneEnde(g, t - P15);
}

// =====================================================================
// Musik
// =====================================================================
// Meereslied (a-Moll, wiegend im Dreiertakt)
static const Ton3 MEL_MEER[] = {
  {69, 1, 45}, {72, .5, 0}, {76, 1.5, 0}, {74, .5, 50}, {72, .5, 0}, {71, 1, 0}, {72, 1, 45}, {69, 2, 0},
  {67, 1, 43}, {71, .5, 0}, {74, 1.5, 0}, {72, .5, 48}, {71, .5, 0}, {69, 1, 0}, {68, 1, 52}, {69, 2, 45}};
static void blubb(Synth &s, float a, float d, float t0, float t1) {
  for (float x = a; x < a + d; x += 1.3f) if (bei(x, t0, t1)) s.ton(x, 400 + zufall((int)(x * 10)) * 300, 0.05f, 0.01f, 0.15f, SINUS, GERAEUSCH, 900);
}
static void ton(Synth &s, float t0, float t1) {
  float a = P1;
  blubb(s, a, 24, t0, t1);
  spieleMelodie(s, M(MEL_MEER), a + 0.5f, a + 23.6f, 0.4f, t0, t1, 0.6f);
  a = P2;
  blubb(s, a, 24, t0, t1);
  spieleMelodie(s, M(MEL_MEER), a + 0.4f, a + 23.6f, 0.42f, t0, t1, 0.5f);
  a = P3;
  spieleMelodie(s, M(MEL_KEGEL), a + 0.4f, a + 13, 0.34f, t0, t1, 0.4f);
  if (bei(a + 14, t0, t1)) for (int j = 0; j < 8; j++) s.note(a + 14 + j * 0.25f, 72 + j * 2, 0.4f);
  a = P4;
  for (int i = 0; i < 8; i++) if (bei(a + 1 + i * 2.6f, t0, t1)) s.rauschen(a + 1 + i * 2.6f, 0.6f, HOCHPASS, 2000, 0.12f, 1, 0.01f, 600);  // Feuerwerk
  spieleMelodie(s, M(MEL_F), a + 0.4f, a + 23.6f, 0.28f, t0, t1, 0.5f);
  a = P5;
  if (bei(a, t0, t1)) s.rauschen(a, 24, BANDPASS, 500, 0.1f, 0.7f, 2, 1400);
  for (int j = 0; j < 5; j++) if (bei(a + 2 + j * 4, t0, t1)) s.rauschen(a + 2 + j * 4, 1.4f, TIEFPASS, 260, 0.35f, 1, 0.01f);
  if (bei(a + 7.5f, t0, t1)) s.rauschen(a + 7.5f, 0.8f, BANDPASS, 1200, 0.3f);  // krach
  spieleMelodie(s, M(MEL_SPUK), a + 0.4f, a + 12, 0.36f, t0, t1, 0.35f);
  spieleMelodie(s, M(MEL_MEER), a + 13, a + 23.6f, 0.44f, t0, t1, 0.5f);
  a = P6;
  if (bei(a, t0, t1)) s.rauschen(a, 24, BANDPASS, 700, 0.03f, 0.6f, 1);
  spieleMelodie(s, M(MEL_WIEGE), a + 0.4f, a + 23.6f, 0.44f, t0, t1, 0.5f);
  a = P7;
  blubb(s, a, 24, t0, t1);
  spieleMelodie(s, M(MEL_MEER), a + 0.4f, a + 23.6f, 0.46f, t0, t1, 0.45f);
  a = P8;
  if (bei(a, t0, t1)) s.rauschen(a, 22, BANDPASS, 400, 0.08f, 0.7f, 1, 900);
  spieleMelodie(s, M(MEL_SPUK), a + 0.4f, a + 21.6f, 0.42f, t0, t1, 0.4f);
  a = P9;
  spieleMelodie(s, M(MEL_SPUK), a + 0.4f, a + 20, 0.44f, t0, t1, 0.4f);
  if (bei(a + 20.4f, t0, t1)) for (int j = 0; j < 6; j++) s.note(a + 20.4f + j * 0.1f, 76 - j * 2, 0.5f);
  a = P10;
  if (bei(a + 4.4f, t0, t1)) for (int j = 0; j < 6; j++) s.note(a + 4.4f + j * 0.07f, 84 + j * 2, 0.45f);
  spieleMelodie(s, M(MEL_WIEGE), a + 6, a + 23.6f, 0.42f, t0, t1, 0.5f);
  a = P11;
  spieleMelodie(s, M(MEL_F), a + 0.4f, a + 13.6f, 0.3f, t0, t1, 0.55f);
  spieleMelodie(s, M(MEL_MEER), a + 14, a + 23.6f, 0.46f, t0, t1, 0.4f);
  a = P12;
  spieleMelodie(s, M(MEL_F), a + 9, a + 20, 0.3f, t0, t1, 0.45f);
  spieleMelodie(s, M(MEL_MEER), a + 20.4f, a + 25.6f, 0.46f, t0, t1, 0.4f);
  a = P13;
  spieleMelodie(s, M(MEL_N), a + 0.4f, a + 25.6f, 0.44f, t0, t1, 0.4f);
  if (bei(a + 17.4f, t0, t1)) s.ton(a + 17.4f, 400, 0.15f, 0.002f, 0.3f, SINUS, GERAEUSCH, 160);  // plitsch
  a = P14;
  if (bei(a + 8, t0, t1)) for (int j = 0; j < 10; j++) s.glocke(a + 8 + j * 0.15f, 72 + j * 2, 0.35f);
  spieleMelodie(s, M(MEL_MEER), a + 8.4f, a + 23.6f, 0.4f, t0, t1, 0.55f);
  a = P15;
  spieleMelodie(s, M(MEL_WIEGE), a + 0.4f, a + 18.6f, 0.44f, t0, t1, 0.6f);
  endeKlang(s, a + 19.4f, t0, t1);
}

// =====================================================================
// Startbild, Vorschau, Erkennungszeichen
// =====================================================================
static void titelSzene(Gfx &g, float t) {
  meeresgrund(g, t);
  palast(g, t);
  garten(g, t);
  nixe(g, NIXE[0], 120, 200 + sinf(t) * 4, 0.85f, 1, t, 0.3f, 2, 0.4f);
}
static void titelbild(Gfx &g, float t) { titelSzene(g, t); titelRahmen(g, t, "Die kleine Meerjungfrau", 226, SCHRIFT_B); }
static void vorschau(Gfx &g, float t) { titelSzene(g, t); }
static void symbol(Gfx &g, float x, float y, float t) {
  g.kontur(1.4f, OUT);
  float fl[] = {x, y - 30, x - 30, y - 6, x - 6, y - 14, x + 6, y - 14, x + 30, y - 6};
  g.polygon(fl, 5, NIXE[0].kleid);
  g.konturAus();
  for (int i = 0; i < 4; i++) g.kreis(x - 20 + i * 14, y - 50 - (i & 1) * 8, 4, WEISS, 180);
  glitzern(g, x, y - 30, 40, t, 5);
}

const Maerchen MEERJUNGFRAU = {"Die kleine Meerjungfrau", PDAUER, FARBE(0x5ED0A8), bild, ton, titelbild, vorschau, symbol,
                               LIED(MEL_MEER, 0.4f), "H. C. Andersen"};

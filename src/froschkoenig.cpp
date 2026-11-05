// Der Froschkoenig oder der eiserne Heinrich - ein Spieluhr-Film fuer die Maerchenbox, nach den Bruedern Grimm.
// Die goldene Kugel im Brunnen, das Versprechen, plitsch platsch die Marmortreppe hinauf, "Was du versprochen hast,
// das musst du auch halten", an die Wand - ein Koenigssohn, und der treue Heinrich: "Heinrich, der Wagen bricht!"
#include "erzaehlen.h"

// ---------- Ablauf (Sekunden) ----------
static const float P1 = 0, P2 = 26, P3 = 70, P4 = 90, P5 = 138, P6 = 172, PDAUER = 226;

static FrauLook prinzessinLook() {
  FrauLook l; l.kleid = FARBE(0xF2C94E); l.haar = FARBE(0xF2E08A); l.frisur = 0; l.krone = true; l.glanz = true;
  return l;
}
static const FrauLook PRINZESSIN = prinzessinLook();
static Aussehen prinzLook() {
  Aussehen a; a.weste = FARBE(0x3E8E4E); a.hemd = WEISS; a.hose = FARBE(0x2E3A6E); a.haar = FARBE(0x5A3A26); a.muetze = 3;
  a.halstuch = GOLD;
  return a;
}
static Aussehen heinrichLook() {
  Aussehen a; a.weste = FARBE(0x8E2A2A); a.hemd = FARBE(0xF2E8D0); a.hose = FARBE(0x3A3436); a.haar = FARBE(0x9A9A9A);
  a.hut = FARBE(0x3A3436); a.hutD = FARBE(0x2A2426); a.feder = GOLD;
  return a;
}
static const Aussehen PRINZ = prinzLook(), HEINRICH = heinrichLook();

// ---------- Figuren und Dinge ----------
static void frosch(Gfx &g, float x, float y, float s, float dir, float t, float mund, float sprung = 0, bool krone = true) {
  float h = sprung > 0 ? huepf(sprung) * 24 * s : huepf(fmodf(t * 0.4f, 1)) * 1.5f;
  auto PX = [&](float d) { return x + d * s * dir; };
  g.kontur(1.2f * s, OUT);
  g.ellipse(PX(0), y - 8 * s - h, 14 * s, 9 * s, FARBE(0x5DAE48));
  g.kreis(PX(-6), y - 16 * s - h, 5 * s, FARBE(0x5DAE48));
  g.kreis(PX(7), y - 16 * s - h, 5 * s, FARBE(0x5DAE48));
  g.konturAus();
  g.ellipse(PX(0), y - 5 * s - h, 9 * s, 4 * s, FARBE(0xC9E89A));  // heller Bauch
  for (int i = 0; i < 2; i++) {
    float ex = PX(i ? 7 : -6);
    g.kreis(ex, y - 17 * s - h, 3 * s, WEISS);
    g.kreis(ex + 0.8f * s * dir, y - 17 * s - h, 1.5f * s, SCHWARZ);
  }
  if (mund > 0.05f) g.ellipse(PX(1), y - 7 * s - h, 6 * s, (1 + mund * 3) * s, FARBE(0x8A2A2A));
  else g.bogen(PX(1), y - 9 * s - h, 7 * s, 0.4f, 2.7f, 1.2f * s, OUT);
  float bein = sprung > 0 ? huepf(sprung) * 6 * s : 0;
  g.ellipse(PX(-12), y - 1 * s - bein, 6 * s, 3 * s, FARBE(0x4CA85A));
  g.ellipse(PX(12), y - 1 * s - bein, 6 * s, 3 * s, FARBE(0x4CA85A));
  if (krone) {
    float kx = PX(1), ky = y - 21 * s - h;
    float k[] = {kx - 5 * s, ky, kx - 5 * s, ky - 6 * s, kx - 2 * s, ky - 3 * s, kx, ky - 7 * s, kx + 2 * s, ky - 3 * s, kx + 5 * s, ky - 6 * s, kx + 5 * s, ky};
    g.polygon(k, 7, GOLD);
  }
}

static void kugel(Gfx &g, float x, float y, float r, float t) {
  g.kontur(1.2f, FARBE(0x9A6A1E));
  g.kreis(x, y, r, GOLD);
  g.konturAus();
  g.kreis(x - r * 0.35f, y - r * 0.35f, r * 0.3f, WEISS, 180);
  g.stern(x + r * 0.3f, y - r * 0.5f, r * 0.5f, r * 0.15f, 4, t, WEISS, (uint8_t)(150 + 100 * sinf(t * 3)));
}

static void linde(Gfx &g, float x, float y, float t) {
  g.kontur(1.4f, OUT);
  g.linie(x, y, x + 2, y - 90, 14, HOLZ_D);
  g.konturAus();
  for (int i = 0; i < 6; i++) {
    float a = i * 1.047f;
    g.kreis(x + cosf(a) * 34, y - 110 + sinf(a) * 22, 30, i & 1 ? FARBE(0x4CA85A) : FARBE(0x5DAE48));
  }
  g.kreis(x, y - 116, 30, FARBE(0x6EBF56));
}

static void brunnen(Gfx &g, float x, float y, float t) {
  g.kontur(1.6f, OUT);
  g.ellipse(x, y - 12, 50, 14, STEIN);
  g.konturAus();
  g.ellipse(x, y - 14, 42, 10, FARBE(0x3E6EA8));
  for (int i = 0; i < 3; i++) {
    float r = fmodf(t * 10 + i * 12, 36);
    g.bogen(x, y - 14, r, 3.4f, 6.0f, 1, FARBE(0xBFE6FF), (uint8_t)(200 * (1 - r / 36)));
  }
}

static void wald(Gfx &g, float t) {
  himmel(g, 0, t, 196);
  sonne(g, 248, 30, 11, t);
  schloss(g, 230, 196, t, 0);
  for (int i = 0; i < 3; i++) tanne(g, 10 + i * 40, 196, 0.9f, FARBE(0x2F8F4E));
  boden(g, 196);
  linde(g, 110, 210, t);
}

static void tafel(Gfx &g, float t) {
  g.verlauf(0, 190, FARBE(0xF6E2B4), FARBE(0xE8C483));
  for (int i = 0; i < 4; i++) {
    float x = 20 + i * 82;
    g.rechteck(x - 10, 30, 20, 160, FARBE(0xFFF3D6));
    g.rechteck(x - 14, 26, 28, 8, FARBE(0xE8D3A2));
  }
  g.rechteck(0, 190, 284, 50, FARBE(0xC9A46A));
  // Tafel
  g.kontur(1.6f, OUT);
  g.rundRechteck(60, 168, 170, 10, 3, WEISS);
  g.konturAus();
  g.rechteck(70, 178, 6, 34, HOLZ_D);
  g.rechteck(214, 178, 6, 34, HOLZ_D);
}

static void treppe(Gfx &g, float t) {
  himmel(g, 0.6f, t, 200);
  schloss(g, 142, 110, t, 0.4f);
  for (int i = 0; i < 7; i++) {
    float y = 110 + i * 18, w = 110 + i * 26;
    g.rechteck(142 - w / 2, y, w, 18, FARBE(0xF2EEE8));
    g.rechteck(142 - w / 2, y, w, 2.5f, WEISS);
    for (int k = 0; k < 4; k++) g.linie(142 - w / 2 + 10 + k * w / 4, y + 4, 142 - w / 2 + 24 + k * w / 4, y + 12, 0.8f, FARBE(0xD8D0E0));  // Marmor
  }
}

static void kammer(Gfx &g, float t) {
  g.verlauf(0, 200, FARBE(0xE8D8F0), FARBE(0xD0C0E0));
  g.rechteck(0, 200, 284, 40, FARBE(0x9E7656));
  g.kontur(1.6f, OUT);
  g.rundRechteck(170, 40, 60, 70, 26, HOLZ);
  g.konturAus();
  g.rundRechteck(176, 46, 48, 60, 22, FARBE(0x22285A));
  mond(g, 200, 66, 7);
  // Himmelbett
  g.rechteck(14, 70, 140, 8, FARBE(0xE8A0B8));
  g.kontur(1.4f, OUT);
  g.rundRechteck(14, 150, 140, 52, 8, FARBE(0xF2E0F0));
  g.konturAus();
  g.rundRechteck(20, 142, 34, 18, 8, WEISS);
  g.rechteck(14, 70, 6, 132, HOLZ_D);
  g.rechteck(148, 70, 6, 132, HOLZ_D);
}

static void kutsche(Gfx &g, float x, float y, float t, float lauf, float zerbirst) {
  float d = lauf >= 0 ? lauf * 6.28f : 0;
  g.kontur(1.6f, OUT);
  g.rundRechteck(x - 40, y - 70, 80, 50, 14, FARBE(0xF2F0E8));
  float dach[] = {x - 44, y - 70, x, y - 90, x + 44, y - 70};
  g.polygon(dach, 3, GOLD);
  g.konturAus();
  g.rundRechteck(x - 26, y - 62, 22, 22, 6, FARBE(0x8FD3FF));
  g.rundRechteck(x + 4, y - 62, 22, 22, 6, FARBE(0x8FD3FF));
  for (int i = 0; i < 3; i++) g.stern(x - 30 + i * 30, y - 92, 4, 1.6f, 5, -1.5708f, WEISS);  // weisse Federbuesche
  for (int i = 0; i < 2; i++) {
    float wx = x + (i ? 28 : -28);
    g.kontur(1.3f, OUT);
    g.kreis(wx, y - 14, 14, GOLD);
    g.konturAus();
    for (int k = 0; k < 4; k++) { float a = d + k * 0.785f; g.linie(wx - cosf(a) * 12, y - 14 - sinf(a) * 12, wx + cosf(a) * 12, y - 14 + sinf(a) * 12, 1.4f, FARBE(0x9A6A1E)); }
  }
  (void)zerbirst;
}

// der treue Heinrich mit den drei eisernen Baendern ums Herz
static void heinrich(Gfx &g, float x, float y, float s, float dir, float t, int baender, float mund) {
  Pose p = posis(HEINRICH, t);
  p.mund = mund;
  p.armV = 1.4f;
  schneiderlein(g, x, y, s, dir, p);
  for (int i = 0; i < baender; i++)
    g.rundRechteck(x - 12 * s, y - (36 - i * 5) * s, 24 * s, 2.6f * s, 1 * s, FARBE(0x6E7484));
}

// =====================================================================
// Szenen
// =====================================================================
// 1) Die goldene Kugel faellt in den Brunnen
static void szeneKugel(Gfx &g, float u) {
  wald(g, u);
  brunnen(g, 160, 214, u);
  FrauPose p = fposis(PRINZESSIN, u);
  float wurf = fmodf(u, 3) / 3;
  float faellt = seg(u, 10, 12);
  p.armV = u < 10 ? 1.6f + huepf(wurf) * 1.2f : (u < 14 ? 2.4f : 2.2f);
  p.mund = u < 10 ? 0.4f : 0.6f;
  p.weint = u > 14;
  float hx, hy;
  frau(g, 80, 214, 0.9f, 1, p, &hx, &hy);
  if (u < 10) kugel(g, hx + 4, hy - huepf(wurf) * 50, 6, u);
  else if (faellt < 1) kugel(g, lerp(hx + 4, 160, faellt), lerp(hy - 50, 200, hinein(faellt)) - huepf(faellt) * 20, 6, u);
  if (faellt >= 1 && u < 14) for (int i = 0; i < 8; i++) g.kreis(160 + cosf(i * 0.785f) * seg(u, 12, 14) * 30, 200 - fabsf(sinf(i * 0.785f)) * 14 * seg(u, 12, 14), 2.4f, FARBE(0xBFE6FF), (uint8_t)(255 * (1 - seg(u, 12, 14))));
  g.kameraAus();
  vers(g, {"In alten Zeiten, wo das Wünschen", "noch geholfen hat, lebte ein König,", "dessen jüngste Tochter war so schön."}, aufzu(u, 0.4f, 9.6f));
  vers(g, {"Da rollte die Kugel geradezu", "ins Wasser hinein."}, aufzu(u, 11, 18));
  vers(g, {"Da fing sie an zu weinen und", "weinte immer lauter."}, aufzu(u, 18.4f, 25));
  blende(g, u, 26, 140, 160);
}

// 2) Der Frosch: "Was gibst du mir?" - das Versprechen
static void szeneVersprechen(Gfx &g, float u) {
  wald(g, u);
  brunnen(g, 160, 214, u);
  FrauPose p = fposis(PRINZESSIN, u);
  p.weint = u < 6;
  p.armV = 1.3f;
  bool spricht = (u > 6 && u < 10) || (u > 16 && u < 23) || (u > 33 && u < 37.6f);
  p.mund = spricht ? 0.5f : 0.1f;
  frau(g, 80, 214, 0.9f, 1, p);
  frosch(g, 160, 202, 1.2f, -1, u, (u > 1 && u < 6) || (u > 10 && u < 16) || (u > 23 && u < 33) ? 0.3f + 0.3f * fabsf(sinf(u * 8)) : 0, 0, false);
  g.kameraAus();
  blase(g, 190, 46, {"Was hast du vor, Königstochter?", "Du schreist ja, dass sich", "ein Stein erbarmen möchte."}, aufzu(u, 1, 6), 160, 186);
  blase(g, 90, 46, {"Ich weine über meine goldne", "Kugel, die mir in den Brunnen", "hinabgefallen ist."}, aufzu(u, 6.2f, 10), 92, 120);
  blase(g, 190, 46, {"Sei still und weine nicht.", "Aber was gibst du mir, wenn ich", "dein Spielwerk heraufhole?"}, aufzu(u, 10.2f, 16), 160, 186);
  blase(g, 90, 46, {"Was du haben willst, lieber", "Frosch: meine Kleider, meine", "Perlen, auch die goldne Krone!"}, aufzu(u, 16.2f, 23), 92, 120);
  blase(g, 190, 40, {"Wenn du mich lieb haben willst,", "ich an deinem Tischlein sitzen,", "von deinem Tellerlein essen,", "in deinem Bettlein schlafen darf ..."}, aufzu(u, 23.2f, 33), 160, 186);
  blase(g, 90, 60, {"Ach ja, ich verspreche dir alles!"}, aufzu(u, 33.2f, 37.6f), 92, 120);
  vers(g, {"Sie dachte aber: Was der einfältige", "Frosch schwätzt!"}, aufzu(u, 38, 43.6f));
  blende(g, u, 44, 140, 160);
}

// 3) Der Frosch holt die Kugel; sie laeuft davon - "Warte, warte, nimm mich mit!"
static void szeneWarte(Gfx &g, float u) {
  wald(g, u);
  brunnen(g, 160, 214, u);
  float rauf = seg(u, 2, 4);
  if (u < 2) for (int i = 0; i < 6; i++) g.kreis(160 + cosf(i) * 20 * u, 200, 2, FARBE(0xBFE6FF), (uint8_t)(255 * (1 - u / 2)));
  float fx = 160, fy = 202;
  if (u > 2) frosch(g, fx, fy, 1.2f, -1, u, u > 9 ? 0.6f + 0.3f * fabsf(sinf(u * 9)) : 0, u > 9 ? fmodf(u * 1.4f, 1) : 0, false);
  if (u > 2 && u < 6) kugel(g, fx - 14, fy - 20 + (1 - rauf) * 20, 6, u);
  FrauPose p = fposis(PRINZESSIN, u);
  float hx, hy;
  if (u < 6) { p.armV = 1.6f; p.mund = 0.6f; frau(g, 80, 214, 0.9f, 1, p, &hx, &hy); }
  else {
    p.lauf = fmodf(u * 2, 1);
    p.armV = 1.6f;
    p.mund = 0.3f;
    frau(g, lerp(80, -40, seg(u, 6, 10)), 214, 0.9f, -1, p, &hx, &hy);
    kugel(g, hx - 4, hy, 6, u);
  }
  g.kameraAus();
  blase(g, 190, 70, {"Warte, warte! Nimm mich mit,", "ich kann nicht so laufen wie du!"}, aufzu(u, 9.4f, 16), 160, 186);
  vers(g, {"Aber was half ihm das!"}, aufzu(u, 16.2f, 19.8f));
  blende(g, u, 20, 140, 160);
}

// 4) Am andern Tag: plitsch platsch die Marmortreppe hinauf; "Was du versprochen hast ..."; er isst von ihrem Tellerlein
static void szeneTafel(Gfx &g, float u) {
  if (u < 10) {
    treppe(g, u);
    float k = seg(u, 0, 9);
    float sx = 142 + sinf(k * 30) * 4, sy = lerp(236, 116, k);
    frosch(g, sx, sy, 0.9f, 1, u, 0, fmodf(u * 1.4f, 1), false);
    g.kameraAus();
    vers(g, {"Da kam etwas plitsch platsch,", "plitsch platsch die Marmortreppe", "heraufgekrochen."}, aufzu(u, 0.4f, 9.6f));
    return;
  }
  float v = u - 10;
  tafel(g, u);
  koenig(g, 210, 214, 0.95f, -1, v > 10 && v < 16 ? 0.6f : 0);
  FrauPose p = fposis(PRINZESSIN, u);
  p.mund = v > 2 && v < 7 ? 0.5f : 0.1f;
  p.boese = v > 18 ? 0.6f : 0;
  p.armV = 1.5f;
  frau(g, 110, 214, 0.9f, 1, p);
  // Tellerlein, Becherlein
  g.ellipse(146, 166, 14, 3.5f, WEISS);
  g.ellipse(146, 165, 10, 2, FARBE(0xE0A050));
  g.rechteck(170, 156, 6, 10, GOLD);
  if (v > 2 && v < 16) for (int i = 0; i < 3; i++) g.stern(18, 150 - i * 6, 3, 1.2f, 4, u * 6 + i, OUT);  // klopf, klopf an der Tuer
  if (v > 18) frosch(g, 146, 164, 0.6f, -1, u, v > 20 ? 0.4f * fabsf(sinf(u * 6)) : 0, 0, false);  // isst vom Tellerlein
  else if (v > 16) frosch(g, lerp(20, 146, seg(v, 16, 18)), lerp(214, 164, seg(v, 16, 18)), 0.8f, 1, u, 0, fmodf(u * 2, 1), false);
  g.kameraAus();
  blase(g, 80, 50, {"Königstochter, jüngste,", "mach mir auf!"}, aufzu(v, 2, 6), 18, 160);
  blase(g, 120, 50, {"Ach, lieber Vater, da draußen", "ist ein garstiger Frosch ..."}, aufzu(v, 6.2f, 10), 112, 140);
  blase(g, 190, 50, {"Was du versprochen hast,", "das musst du auch halten.", "Geh nur und mach ihm auf!"}, aufzu(v, 10.2f, 16), 200, 140);
  vers(g, {"Der Frosch ließ sich's gut", "schmecken, ihr aber blieb fast", "jedes Bisslein im Halse stecken."}, aufzu(v, 20, 31));
  blende(g, u, 48, 140, 160);
}

// 5) In der Kammer: "Heb mich herauf, oder ich sag's deinem Vater" - an die Wand - ein Koenigssohn
static void szeneWand(Gfx &g, float u) {
  kammer(g, u);
  float wurf = seg(u, 13, 14);
  bool prinz = u > 15;
  FrauPose p = fposis(PRINZESSIN, u);
  p.boese = u > 11 && u < 15 ? 1 : 0;
  p.mund = u > 11 && u < 15 ? 0.6f : 0.2f;
  p.armV = u < 13 ? 1.4f : (u < 14 ? 2.8f : 1.6f);
  if (prinz) { p.mund = 0.5f; p.boese = 0; p.armV = 2.0f; }
  float hx, hy;
  frau(g, 110, 214, 0.9f, 1, p, &hx, &hy);
  if (u < 13) frosch(g, hx + 4, hy + 4, 0.55f, 1, u, u > 3 && u < 9 ? 0.4f * fabsf(sinf(u * 8)) : 0, 0, false);
  else if (u < 14.4f) frosch(g, lerp(hx, 250, wurf), lerp(hy, 150, wurf), 0.55f, 1, u, 0.6f, 0, false);
  if (u > 14 && u < 15.6f) puff(g, 240, 160, seg(u, 14, 15.6f));
  if (prinz) {
    Pose k = posis(PRINZ, u);
    k.mund = u > 16 && u < 24 ? 0.5f : 0.3f;
    k.armV = 1.6f;
    schneiderlein(g, lerp(240, 190, seg(u, 15, 17)), 214, 0.95f, -1, k);
    herzen(g, 150, 150, u, 4, 30);
  }
  if (u > 14 && u < 14.4f) g.wackeln(4, 2);
  g.kameraAus();
  blase(g, 160, 60, {"Ich bin müde, ich will schlafen", "so gut wie du. Heb mich herauf,", "oder ich sag's deinem Vater!"}, aufzu(u, 3, 10.6f), hx + 6, hy - 10);
  blase(g, 110, 50, {"Nun wirst du Ruhe haben,", "du garstiger Frosch!"}, aufzu(u, 11, 14.6f), 110, 130);
  vers(g, {"Als er herabfiel, war er kein", "Frosch, sondern ein Königssohn", "mit schönen freundlichen Augen."}, aufzu(u, 15.6f, 24));
  vers(g, {"Eine böse Hexe hatte ihn", "verwünscht, und niemand hatte", "ihn erlösen können als sie."}, aufzu(u, 24.2f, 33.6f));
  blende(g, u, 34, 140, 160);
}

// 6) Der treue Heinrich: "Heinrich, der Wagen bricht!"
static void szeneHeinrich(Gfx &g, float u) {
  float scroll = u * 40;
  himmel(g, 0, u, 190);
  sonne(g, 248, 30, 11, u);
  huegel(g, scroll * 0.3f, 0.4f, 190, 28, 100, FARBE(0x8ACB6A));
  boden(g, 190);
  g.rechteck(0, 206, 284, 18, FARBE(0xD8C49A));
  for (int i = 0; i < 4; i++) laubbaum(g, fmodf(i * 90 - scroll + 900, 360) - 40, 200, 0.7f, FARBE(0x4CB85A));
  float lauf = fmodf(u * 1.6f, 1);
  kutsche(g, 110, 222 - fabsf(sinf(u * 10)) * 2, u, lauf, 0);
  einhorn(g, 186, 222, 0.75f, lauf, 1, 0, 0, true);
  einhorn(g, 232, 222, 0.75f, fmodf(lauf + 0.5f, 1), 1, 0, 0, true);
  // im Fenster: Prinz und Prinzessin
  g.ausschnitt(84, 160, 22, 22);
  Pose k = posis(PRINZ, u);
  k.mund = (u > 12 && u < 14) || (u > 24 && u < 26) ? 0.6f : 0.2f;
  schneiderlein(g, 95, 212, 0.55f, 1, k);
  g.ausschnittAus();
  g.ausschnitt(114, 160, 22, 22);
  FrauPose p = fposis(PRINZESSIN, u);
  frau(g, 125, 212, 0.55f, -1, p);
  g.ausschnittAus();
  // Heinrich hinten auf dem Wagen; drei Baender springen nacheinander
  int baender = u < 11 ? 3 : u < 23 ? 2 : u < 34 ? 1 : 0;
  heinrich(g, 66, 160, 0.7f, 1, u, baender, (u > 14 && u < 22) || (u > 26 && u < 32) ? 0.5f : 0.1f);
  for (float krach : {11.0f, 23.0f, 34.0f})
    if (u > krach && u < krach + 1) for (int i = 0; i < 4; i++) g.stern(66 + cosf(i * 1.57f) * seg(u, krach, krach + 1) * 20, 130 + sinf(i * 1.57f) * seg(u, krach, krach + 1) * 14, 3, 1.2f, 4, u * 6, FARBE(0x8E94A6));
  if (u > 34) herzen(g, 110, 150, u, 4, 40);
  g.kameraAus();
  vers(g, {"Am Morgen kam ein Wagen mit acht", "weißen Pferden, hinten stand der", "treue Heinrich."}, aufzu(u, 0.4f, 9));
  blase(g, 120, 46, {"Heinrich, der Wagen bricht!"}, aufzu(u, 11.4f, 14), 95, 160);
  blase(g, 140, 36, {"Nein, Herr, der Wagen nicht,", "es ist ein Band von meinem Herzen,", "das da lag in großen Schmerzen,", "als Ihr in dem Brunnen saßt."}, aufzu(u, 14.2f, 22.6f), 66, 110);
  blase(g, 120, 46, {"Heinrich, der Wagen bricht!"}, aufzu(u, 23.4f, 26), 95, 160);
  vers(g, {"Es waren die Bänder, die vom", "Herzen des treuen Heinrich", "absprangen, weil sein Herr erlöst war."}, aufzu(u, 34.4f, 44));
  endeBild(g, u, 48);
}

static void bild(Gfx &g, float t) {
  if (t < P2) szeneKugel(g, t - P1);
  else if (t < P3) szeneVersprechen(g, t - P2);
  else if (t < P4) szeneWarte(g, t - P3);
  else if (t < P5) szeneTafel(g, t - P4);
  else if (t < P6) szeneWand(g, t - P5);
  else szeneHeinrich(g, t - P6);
}

// =====================================================================
// Musik
// =====================================================================
// Froschkoenig (F-Dur, mit hopsenden Sprungtoenen)
static const Ton3 MEL_FROSCH[] = {
  {65, .5, 41}, {69, .5, 0}, {72, .5, 0}, {77, .5, 0}, {76, .5, 48}, {72, .5, 0}, {74, 1, 0},
  {70, .5, 46}, {74, .5, 0}, {77, .5, 0}, {74, .5, 0}, {72, .5, 41}, {69, .5, 0}, {72, 1, 0},
  {65, .5, 41}, {69, .5, 0}, {72, .5, 0}, {77, .5, 0}, {81, .5, 46}, {79, .5, 0}, {77, .5, 0}, {74, .5, 0},
  {72, .5, 48}, {76, .5, 0}, {77, 1, 41}};

static void quak(Synth &s, float x) { for (int i = 0; i < 2; i++) s.ton(x + i * 0.22f, 180, 0.09f, 0.01f, 0.14f, RECHTECK, GERAEUSCH, 130); }
static void plitsch(Synth &s, float x) { s.rauschen(x, 0.12f, BANDPASS, 1300, 0.08f, 1.2f, 0.005f, 700); }
static void krach(Synth &s, float x) {
  s.ton(x, 900, 0.15f, 0.002f, 0.2f, SAEGE, GERAEUSCH, 300);
  s.rauschen(x, 0.3f, HOCHPASS, 2500, 0.15f);
}

static void ton(Synth &s, float t0, float t1) {
  float a = P1;
  spieleMelodie(s, M(MEL_FROSCH), a + 0.5f, a + 10, 0.3f, t0, t1, 0.8f);
  for (int i = 0; i < 3; i++) if (bei(a + 1.5f + i * 3, t0, t1)) s.glocke(a + 1.5f + i * 3, 88, 0.3f);  // die Kugel fliegt
  if (bei(a + 12, t0, t1)) { s.ton(a + 12, 300, 0.2f, 0.002f, 0.3f, SINUS, GERAEUSCH, 120); s.rauschen(a + 12, 0.5f, BANDPASS, 900, 0.2f); }  // plumps
  spieleMelodie(s, M(MEL_WIEGE), a + 14, a + 25.5f, 0.45f, t0, t1, 0.6f);
  a = P2;
  if (bei(a + 0.6f, t0, t1)) quak(s, a + 0.6f);
  spieleMelodie(s, M(MEL_FROSCH), a + 1.4f, a + 43.5f, 0.36f, t0, t1, 0.65f);
  a = P3;
  if (bei(a + 0.2f, t0, t1)) s.rauschen(a + 0.2f, 0.5f, BANDPASS, 900, 0.2f);
  if (bei(a + 2.4f, t0, t1)) for (int k = 0; k < 6; k++) s.glocke(a + 2.4f + k * 0.12f, 79 + k * 2, 0.4f);  // die Kugel ist wieder da
  spieleMelodie(s, M(MEL_JAGD), a + 6, a + 10, 0.16f, t0, t1, 0.8f);
  for (float x = ceilf(t0 / 0.7f) * 0.7f; x < t1; x += 0.7f)
    if (x > a + 9 && x < a + 18) plitsch(s, x);
  a = P4;
  for (float x = ceilf(t0 / 0.7f) * 0.7f; x < t1; x += 0.7f)
    if (x > a && x < a + 10) plitsch(s, x);
  for (int i = 0; i < 2; i++) if (bei(a + 12 + i * 1.2f, t0, t1)) for (int k = 0; k < 3; k++) s.ton(a + 12 + i * 1.2f + k * 0.25f, 160, 0.3f, 0.002f, 0.12f, SINUS, GERAEUSCH, 90);  // klopf
  spieleMelodie(s, M(MEL_F), a + 20, a + 26, 0.28f, t0, t1, 0.7f);
  spieleMelodie(s, M(MEL_FROSCH), a + 28, a + 47.5f, 0.34f, t0, t1, 0.7f);
  a = P5;
  spieleMelodie(s, M(MEL_WIEGE), a + 0.4f, a + 11, 0.45f, t0, t1, 0.55f);
  if (bei(a + 14, t0, t1)) { s.ton(a + 14, 110, 0.35f, 0.002f, 0.2f, SINUS, GERAEUSCH, 60); }  // an die Wand
  if (bei(a + 14.2f, t0, t1)) for (int k = 0; k < 14; k++) s.glocke(a + 14.2f + k * 0.12f, 72 + (k % 7) * 2, 0.5f);  // Verwandlung
  spieleMelodie(s, M(MEL_F), a + 16.5f, a + 33.5f, 0.28f, t0, t1, 0.8f);
  a = P6;
  spieleMelodie(s, M(MEL_FROSCH), a + 0.4f, a + 46, 0.26f, t0, t1, 0.8f);
  for (float x = ceilf(t0 / 0.227f) * 0.227f; x < t1; x += 0.227f)
    if (x > a && x < a + 46) s.note(x, 79, 0.12f);  // Hufe
  for (float k : {11.0f, 23.0f, 34.0f}) if (bei(a + k, t0, t1)) krach(s, a + k);
  endeKlang(s, a + 50.2f, t0, t1);
}

// =====================================================================
// Startbild, Vorschau, Erkennungszeichen
// =====================================================================
static void titelSzene(Gfx &g, float t) {
  wald(g, t);
  brunnen(g, 168, 222, t);
  FrauPose p = fposis(PRINZESSIN, t);
  p.armV = 1.8f;
  p.mund = 0.3f;
  float hx, hy;
  frau(g, 90, 222, 0.95f, 1, p, &hx, &hy);
  kugel(g, hx + 4, hy - 4, 6, t);
  frosch(g, 176, 210, 1.2f, -1, t, 0, 0, true);
}
static void titelbild(Gfx &g, float t) { titelSzene(g, t); titelRahmen(g, t, "Der Froschkönig", 196, SCHRIFT_M); }
static void vorschau(Gfx &g, float t) { titelSzene(g, t); }
static void symbol(Gfx &g, float x, float y, float t) {
  frosch(g, x - 14, y, 2.2f, 1, t, 0, 0, true);
  kugel(g, x + 40, y - 14, 12, t);
}

const Maerchen FROSCHKOENIG = {"Der Froschkönig", PDAUER, FARBE(0x5DAE48), bild, ton, titelbild, vorschau, symbol,
                               LIED(MEL_FROSCH, 0.32f), nullptr};

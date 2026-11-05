// Dornroeschen - ein Spieluhr-Film fuer die Maerchenbox, nach dem Maerchen der Brueder Grimm.
// Der Frosch im Bad, die zwoelf weisen Frauen und die dreizehnte, die Spindel im Turm, das ganze Schloss schlaeft,
// die Dornenhecke, nach hundert Jahren der Koenigssohn, der Kuss - und der Koch gibt endlich die Ohrfeige.
#include "erzaehlen.h"

// ---------- Ablauf (Sekunden) ----------
static const float P1 = 0, P2 = 24, P3 = 66, P4 = 80, P5 = 122, P6 = 156, P7 = 190, P8 = 234, PDAUER = 272;

// ---------- Aussehen ----------
static FrauLook roeschenLook() {
  FrauLook l; l.kleid = FARBE(0xF2A0B8); l.haar = FARBE(0xF2C94E); l.frisur = 0; l.blume = FARBE(0xE8283F);
  return l;
}
static FrauLook kindLook() { FrauLook l = roeschenLook(); l.frisur = 1; return l; }
static FrauLook braut() { FrauLook l = roeschenLook(); l.kleid = WEISS; l.glanz = true; l.krone = true; l.rockBreite = 1.4f; l.frisur = 2; return l; }
static FrauLook koeniginLook() {
  FrauLook l; l.kleid = FARBE(0x5B7FC7); l.haar = FARBE(0x8A5A2E); l.frisur = 2; l.krone = true; l.rockBreite = 1.3f;
  return l;
}
static FrauLook alteLook() {
  FrauLook l; l.kleid = FARBE(0x8E7A6A); l.schuerze = FARBE(0xC9B8A0); l.haar = FARBE(0xE8E8E8); l.frisur = 3; l.tuch = FARBE(0x6E5A4A);
  l.wangen = false;
  return l;
}
static FrauLook weiseLook(int i) {
  static const uint32_t K[6] = {0xB28DFF, 0x5BB8E8, 0x7CCB5E, 0xF2A0C8, 0xF2C94E, 0xE8A060};
  FrauLook l; l.kleid = FARBE(K[i % 6]); l.haar = i & 1 ? FARBE(0xE8E8E8) : FARBE(0xC98A3A); l.frisur = 2; l.glanz = true;
  return l;
}
static FrauLook dreizehnteLook() {
  FrauLook l; l.kleid = FARBE(0x2E2440); l.haar = FARBE(0x5E5E66); l.frisur = 3; l.tuch = FARBE(0x1E1830); l.wangen = false;
  l.haut = FARBE(0xD8D8C8);
  return l;
}
static const FrauLook ROESCHEN = roeschenLook(), KIND = kindLook(), BRAUT = braut(), KOENIGIN = koeniginLook(),
                      ALTE = alteLook(), DREIZEHNTE = dreizehnteLook();
static const FrauLook WEISE[6] = {weiseLook(0), weiseLook(1), weiseLook(2), weiseLook(3), weiseLook(4), weiseLook(5)};

static Aussehen prinzLook() {
  Aussehen a; a.weste = FARBE(0x2E6EA8); a.hemd = WEISS; a.hose = FARBE(0x3A2E5A); a.haar = FARBE(0x3A2A20); a.muetze = 3;
  a.halstuch = GOLD;
  return a;
}
static Aussehen kochLook() {
  Aussehen a; a.weste = WEISS; a.hemd = WEISS; a.hose = FARBE(0x6E6A70); a.haar = FARBE(0x8A5A2E); a.muetze = 2; a.hut = WEISS;
  a.hutD = FARBE(0xE0E0E0); a.feder = WEISS;
  return a;
}
static Aussehen jungeLook() { Aussehen a; a.weste = FARBE(0x8EA6C8); a.hose = FARBE(0x4A4A5A); a.haar = FARBE(0xC98A3A); a.muetze = 1; return a; }
static const Aussehen PRINZ = prinzLook(), KOCH = kochLook(), JUNGE = jungeLook();

// ---------- Dinge und Tiere ----------
static void frosch(Gfx &g, float x, float y, float s, float t, float mund) {
  float h = huepf(fmodf(t * 0.4f, 1)) * 2;
  g.kontur(1.2f * s, OUT);
  g.ellipse(x, y - 8 * s - h, 14 * s, 9 * s, FARBE(0x5DAE48));
  g.kreis(x - 7 * s, y - 16 * s - h, 5 * s, FARBE(0x5DAE48));
  g.kreis(x + 7 * s, y - 16 * s - h, 5 * s, FARBE(0x5DAE48));
  g.konturAus();
  for (int i = 0; i < 2; i++) {
    float ex = x + (i ? 7 : -7) * s;
    g.kreis(ex, y - 17 * s - h, 3 * s, WEISS);
    g.kreis(ex + 0.5f * s, y - 17 * s - h, 1.5f * s, SCHWARZ);
  }
  if (mund > 0.05f) g.ellipse(x, y - 5 * s - h, 6 * s, (1 + mund * 3) * s, FARBE(0x8A2A2A));
  else g.bogen(x, y - 8 * s - h, 7 * s, 0.4f, 2.7f, 1.2f * s, OUT);
  g.ellipse(x - 12 * s, y - 1 * s, 6 * s, 3 * s, FARBE(0x4CA85A));
  g.ellipse(x + 12 * s, y - 1 * s, 6 * s, 3 * s, FARBE(0x4CA85A));
}

static void spindel(Gfx &g, float x, float y, float s, float dreh) {
  g.linie(x, y - 14 * s, x, y + 14 * s, 1.4f * s, HOLZ_D);
  g.ellipse(x, y, 4 * s * (0.6f + 0.4f * fabsf(cosf(dreh))), 6 * s, FARBE(0xE8D8B0));
  g.kreis(x, y + 14 * s, 1.4f * s, FARBE(0xC0C8D4));
}

static void rosenhecke(Gfx &g, float wachs, float bluete, float t, float offen = 0) {
  if (wachs <= 0) return;
  const Farbe blatt = FARBE(0x2E5A30), blatt2 = FARBE(0x3E7A40);
  for (int i = 0; i < 16; i++) {
    float x = (i % 8) * 40 + (i / 8) * 20 - 10;
    float seite = x < 142 ? -1 : 1;
    x += seite * offen * 120;
    float h = (60 + zufall(i + 10) * 70) * wachs;
    float y = 214 - (i / 8) * 20;
    g.kontur(1.2f, OUT);
    g.ellipse(x, y - h * 0.5f, 26, h * 0.55f + 6, i & 1 ? blatt : blatt2);
    g.konturAus();
    for (int k = 0; k < 6; k++) {  // Dornen
      float a = k * 1.05f + i;
      float dx = x + cosf(a) * 24, dy = y - h * 0.5f + sinf(a) * (h * 0.5f);
      g.linie(dx, dy, dx + cosf(a) * 5, dy + sinf(a) * 5, 1.2f, FARBE(0x5A3A26));
    }
    if (bluete > 0)
      for (int k = 0; k < 4; k++) {
        float rx = x - 14 + zufall(i * 7 + k) * 28, ry = y - zufall(i * 5 + k + 3) * h;
        float r = 4.5f * bluete;
        g.kreis(rx, ry, r, FARBE(0xE8283F));
        g.kreis(rx - 1, ry - 1, r * 0.45f, FARBE(0xFF8AA0));
      }
  }
}

static void weiseFrau(Gfx &g, int i, float x, float y, float s, float dir, float t, float mund, float stab) {
  FrauPose p = fposis(WEISE[i % 6], t, i);
  p.mund = mund;
  p.armV = 1.4f + stab;
  float hx, hy;
  frau(g, x, y, s, dir, p, &hx, &hy);
  g.linie(hx, hy, hx + 4 * dir, hy - 16, 1.6f, FARBE(0xF2F2F2));
  g.stern(hx + 4 * dir, hy - 18, 4 + stab * 2, 1.6f, 5, t * 2 + i, GOLD);
}

static void fliegeWand(Gfx &g, float x, float y, float t, bool schlaeft) {
  float dx = schlaeft ? 0 : sinf(t * 3 + x) * 4;
  g.ellipse(x + dx, y, 2.6f, 1.6f, SCHWARZ);
  g.ellipse(x + dx - 1.6f, y - 1.6f, 2, 1.2f, FARBE(0xC9E0FF), 180, -0.5f);
  g.ellipse(x + dx + 1.6f, y - 1.6f, 2, 1.2f, FARBE(0xC9E0FF), 180, 0.5f);
}

// ---------- Kulissen ----------
static void saal(Gfx &g, float t, float schlaf = 0) {
  Farbe w = mischeFarbe(FARBE(0xF6E2B4), FARBE(0xB8A8C8), schlaf * 0.5f);
  g.verlauf(0, 180, w, mischeFarbe(w, OUT, 0.15f));
  for (int i = 0; i < 4; i++) {
    float x = 20 + i * 82;
    g.rechteck(x - 10, 30, 20, 160, FARBE(0xFFF3D6));
    g.rechteck(x - 14, 26, 28, 8, FARBE(0xE8D3A2));
  }
  g.rechteck(0, 190, 284, 50, FARBE(0xC9A46A));
  for (int r = 0; r < 3; r++) for (int c = 0; c < 12; c++) if ((r + c) & 1) g.rechteck(c * 26 - r * 8, 190 + r * 17, 26, 17, FARBE(0xA8823E));
  for (int i = 0; i < 2; i++) {  // Fahnen
    g.rechteck(60 + i * 150, 40, 24, 40, FARBE(0xC0392B));
    float sp[] = {60 + i * 150.0f, 80, 72 + i * 150.0f, 72, 84 + i * 150.0f, 80};
    g.polygon(sp, 3, FARBE(0xC0392B));
    g.stern(72 + i * 150, 56, 6, 2.4f, 5, -1.5708f, GOLD);
  }
}

static void bad(Gfx &g, float t) {
  himmel(g, 0, t, 196);
  sonne(g, 248, 30, 11, t);
  schloss(g, 70, 196, t, 0);
  boden(g, 196);
  g.ellipse(180, 214, 90, 18, FARBE(0x6EB8E8));
  g.ellipse(180, 212, 82, 13, FARBE(0x8ECCF0));
  for (int i = 0; i < 4; i++) g.ellipse(120 + i * 40, 210 + (i & 1) * 4, 9, 3, FARBE(0x4CA85A));  // Seerosenblaetter
  g.kreis(160, 208, 3, FARBE(0xF2A0C8));
}

static void turmstube(Gfx &g, float t) {
  g.verlauf(0, 200, FARBE(0xD9CCB8), FARBE(0xC0B19A));
  for (int r = 0; r < 6; r++) for (int c = 0; c < 6; c++) g.rundRechteck(c * 52 - (r & 1) * 26 + 2, r * 32 + 2, 48, 28, 3, FARBE(0xCFC0A8));
  g.rechteck(0, 200, 284, 40, FARBE(0x8E7656));
  g.kontur(1.8f, OUT);
  g.rundRechteck(30, 40, 50, 80, 24, FARBE(0x8E8676));
  g.konturAus();
  g.rundRechteck(36, 46, 38, 68, 18, FARBE(0x9FD8FF));
  // die kleine Tuer mit dem rostigen Schluessel
  g.rundRechteck(230, 110, 40, 90, 18, HOLZ);
  g.kreis(240, 158, 3, FARBE(0xA8703E));
}

static void kueche(Gfx &g, float t, bool schlaf) {
  g.verlauf(0, 200, FARBE(0xE8D2B0), FARBE(0xD6B88E));
  g.rechteck(0, 200, 284, 40, FARBE(0x9E7656));
  g.kontur(1.8f, OUT);
  g.rundRechteck(170, 120, 100, 82, 6, FARBE(0xA89888));
  g.konturAus();
  g.rundRechteck(186, 150, 68, 50, 18, FARBE(0x2E2620));
  if (!schlaf)
    for (int i = 0; i < 3; i++) {
      float fx = 200 + i * 20, h = 16 + 6 * sinf(t * 9 + i * 1.7f);
      float f[] = {fx - 7, 196, fx + 7, 196, fx + sinf(t * 7 + i) * 2, 196 - h};
      g.polygon(f, 3, FARBE(0xFF8A2A));
    }
  else for (int i = 0; i < 3; i++) g.ellipse(200 + i * 20, 194, 6, 2, FARBE(0xC0603A));  // das Feuer steht still
  // Braten am Spiess
  g.linie(176, 140, 266, 140, 2, FARBE(0x8E94A6));
  g.ellipse(220, 140, 18, 9, FARBE(0xC9702E));
  g.rechteck(20, 70, 90, 5, HOLZ_D);
  for (int i = 0; i < 4; i++) {
    g.kontur(1.2f, OUT);
    g.rundRechteck(26 + i * 22, 52, 16, 18, 4, i & 1 ? FARBE(0xC0632E) : FARBE(0x9AA6B8));
    g.konturAus();
  }
}

// =====================================================================
// Szenen
// =====================================================================
// 1) Die Koenigin im Bad, der Frosch prophezeit
static void szeneFrosch(Gfx &g, float u) {
  bad(g, u);
  FrauPose k = fposis(KOENIGIN, u);
  k.armV = 1.2f;
  k.mund = u > 4 && u < 8 ? 0.5f : 0.1f;
  if (u > 16) { k.mund = 0.6f; k.armV = 2.2f; }
  frau(g, 100, 214, 1, 1, k);
  frosch(g, 196, 212, 1.2f, u, u > 9 && u < 16 ? 0.4f + 0.3f * fabsf(sinf(u * 8)) : 0);
  if (u > 16) glitzern(g, 100, 150, 30, u, 6, (uint8_t)(255 * (1 - seg(u, 20, 24))));
  g.kameraAus();
  vers(g, {"Vor Zeiten war ein König und", "eine Königin, die sprachen", "jeden Tag:"}, aufzu(u, 0.4f, 4));
  blase(g, 90, 60, {"Ach, wenn wir doch ein Kind hätten!"}, aufzu(u, 4.2f, 8.6f), 104, 130);
  blase(g, 190, 70, {"Dein Wunsch wird erfüllt werden.", "Ehe ein Jahr vergeht, wirst du", "eine Tochter zur Welt bringen."}, aufzu(u, 9, 16), 196, 186);
  blende(g, u, 24, 160, 160);
}

// 2) Das Fest: die zwoelf weisen Frauen und die dreizehnte
static void szeneFest(Gfx &g, float u) {
  saal(g, u);
  koenig(g, 40, 214, 0.95f, 1, u > 30 ? 0 : 0.3f);
  FrauPose k = fposis(KOENIGIN, u);
  k.armV = 1.6f;
  float khx, khy;
  frau(g, 76, 214, 0.95f, 1, k, &khx, &khy);
  // die Wiege mit dem Kind
  g.kontur(1.2f, OUT);
  float w[] = {104, 196, 140, 196, 136, 212, 108, 212};
  g.polygon(w, 4, GOLD);
  g.konturAus();
  g.kreis(114, 192, 6, HAUT);
  g.ellipse(126, 194, 10, 5, FARBE(0xF6B8D2));
  // die weisen Frauen beschenken das Kind, eine nach der anderen
  for (int i = 0; i < 5; i++) {
    float auf = seg(u, 2 + i * 2.2f, 3.2f + i * 2.2f);
    if (auf <= 0) continue;
    float wx = 156 + i * 26;
    float stab = (u > 2 + i * 2.2f && u < 4.4f + i * 2.2f) ? 1 : 0;
    weiseFrau(g, i, wx, 214, 0.78f, -1, u, stab > 0 ? 0.4f : 0, stab);
    if (stab > 0) glitzern(g, 124, 186, 16, u + i, 5);
  }
  // die dreizehnte
  if (u > 14 && u < 26) {
    FrauPose d = fposis(DREIZEHNTE, u);
    d.boese = 1;
    d.mund = u > 16 && u < 23 ? 0.7f : 0;
    d.armV = 2.4f;
    float k2 = sanft(seg(u, 14, 15.5f));
    frau(g, lerp(330, 196, k2) * (u < 24 ? 1 : 1) + (u > 24 ? (u - 24) * 80 : 0), 214, 0.9f, -1, d);
    puff(g, 196, 150, seg(u, 13.6f, 15), FARBE(0x6E4E8E));
    if (u > 16 && u < 23) for (int i = 0; i < 4; i++) g.stern(150 + i * 10, 120 + sinf(u * 6 + i) * 6, 3, 1.2f, 4, u * 4, FARBE(0x6E4E8E));
  }
  // die zwoelfte mildert den Fluch
  if (u > 26) {
    weiseFrau(g, 5, 200, 214, 0.85f, -1, u, u > 27 && u < 36 ? 0.5f : 0, u > 27 && u < 36 ? 1 : 0);
    if (u > 27 && u < 36) glitzern(g, 124, 186, 20, u, 6);
  }
  g.kameraAus();
  vers(g, {"Der König lud die weisen Frauen ein.", "Es waren dreizehn, er hatte", "aber nur zwölf goldene Teller."}, aufzu(u, 0.2f, 2.9f));
  vers(g, {"Die eine schenkte Tugend,", "die andere Schönheit,", "die dritte Reichtum ..."}, aufzu(u, 3, 13.6f));
  blase(g, 140, 46, {"Die Königstochter soll sich in", "ihrem fünfzehnten Jahr an einer", "Spindel stechen und hinfallen!"}, aufzu(u, 16, 23), 190, 140);
  blase(g, 150, 46, {"Es soll aber kein Tod sein,", "sondern ein hundertjähriger", "tiefer Schlaf."}, aufzu(u, 27.4f, 36), 196, 140);
  blende(g, u, 42, 120, 160);
}

// 3) Der Koenig laesst alle Spindeln im ganzen Koenigreiche verbrennen
static void szeneSpindeln(Gfx &g, float u) {
  himmel(g, 0.2f, u, 196);
  schloss(g, 200, 196, u, 0);
  boden(g, 196);
  for (int i = 0; i < 3; i++) g.linie(80 + i * 10, 214, 70 + i * 14, 196, 3, HOLZ_D);
  for (int i = 0; i < 4; i++) {
    float h = 30 + 10 * sinf(u * 8 + i * 1.7f);
    float f[] = {66 + i * 12, 212, 78 + i * 12, 212, 72 + i * 12 + sinf(u * 6 + i) * 3, 212 - h};
    g.polygon(f, 3, i & 1 ? FARBE(0xFFD54A) : FARBE(0xFF8A2A));
  }
  for (int i = 0; i < 6; i++) {
    float k = fmodf(u * 0.6f + i / 6.0f, 1);
    spindel(g, lerp(150 + i * 6, 84, k), lerp(150 - i * 8, 196, k) - huepf(k) * 30, 0.8f, u * 4 + i);
  }
  koenig(g, 170, 214, 0.95f, -1, 0.4f);
  g.kameraAus();
  vers(g, {"Der König ließ befehlen, dass", "alle Spindeln im ganzen Reiche", "verbrannt werden sollten."}, aufzu(u, 0.4f, 13.4f));
  blende(g, u, 14, 120, 160);
}

// 4) Der fuenfzehnte Geburtstag: die Alte im Turm, die Spindel - sie sticht sich und schlaeft ein
static void szeneTurm(Gfx &g, float u) {
  if (u < 8) {
    himmel(g, 0, u, 196);
    sonne(g, 248, 30, 11, u);
    schloss(g, 142, 196, u, 0);
    boden(g, 196);
    FrauPose r = fposis(ROESCHEN, u, 1);
    r.lauf = fmodf(u * 1.4f, 1);
    frau(g, lerp(40, 200, seg(u, 0, 7)), 214, 0.85f, 1, r);
    g.kameraAus();
    vers(g, {"An dem Tage, wo sie gerade", "fünfzehn Jahre alt ward, kam", "sie an einen alten Turm ..."}, aufzu(u, 0.4f, 7.8f));
    return;
  }
  float v = u - 8;
  turmstube(g, u);
  FrauPose a = fposis(ALTE, u);
  a.armV = 1.3f + sinf(u * 4) * 0.2f;
  a.mund = (v > 4 && v < 6) || (v > 10 && v < 14) ? 0.4f : 0.1f;
  float ahx, ahy;
  frau(g, 160, 214, 0.9f, -1, a, &ahx, &ahy);
  // Spinnrad
  g.kontur(1.2f, OUT);
  g.kreis(110, 180, 20, HOLZ);
  g.konturAus();
  g.kreis(110, 180, 16, FARBE(0xE8D2B0));
  for (int i = 0; i < 6; i++) { float w = u * 3 + i * 1.047f; g.linie(110, 180, 110 + cosf(w) * 16, 180 + sinf(w) * 16, 1.6f, HOLZ_D); }
  g.linie(100, 214, 110, 180, 2, HOLZ_D);
  g.linie(120, 214, 110, 180, 2, HOLZ_D);
  float stich = seg(v, 18, 19);
  FrauPose r = fposis(ROESCHEN, u, 1);
  r.mund = (v > 1 && v < 4) || (v > 6 && v < 10) ? 0.5f : 0.15f;
  r.armV = v > 15 ? 1.6f : 0.9f;
  float rx = v < 2 ? lerp(300, 230, v / 2) : 230;
  if (v > 14) rx = lerp(230, 140, seg(v, 14, 17));
  float rhx, rhy;
  if (stich < 1 || v < 22) {
    if (v > 19) { r.zu = true; r.armV = 0.3f; }
    float sink = seg(v, 19.5f, 22);
    frau(g, rx, 214 + sink * 4, 0.85f, -1, r, &rhx, &rhy);
    if (v > 15 && v < 19) spindel(g, rhx - 6, rhy - 4, 0.9f, u * 6);
  }
  if (v >= 22) {  // sie liegt auf dem Bett und schlaeft
    g.kontur(1.4f, OUT);
    g.rundRechteck(150, 186, 110, 20, 6, FARBE(0xF2EEFF));
    g.konturAus();
    g.ellipse(214, 182, 40, 10, ROESCHEN.kleid);
    g.ellipse(166, 176, 14, 9, ROESCHEN.haar);
    g.kreis(170, 178, 9, HAUT);
    g.bogen(172, 179, 2.2f, 0.3f, 2.8f, 1.2f, OUT);
    zzz(g, 176, 160, u);
  }
  if (v > 18 && v < 19.5f) glitzern(g, rhx, rhy, 16, u, 6);
  g.kameraAus();
  blase(g, 200, 60, {"Guten Tag, du altes Mütterchen,", "was machst du da?"}, aufzu(v, 1, 4), 220, 140);
  blase(g, 150, 60, {"Ich spinne."}, aufzu(v, 4.2f, 6), 150, 140);
  blase(g, 200, 60, {"Was ist das für ein Ding,", "das so lustig herumspringt?"}, aufzu(v, 6.2f, 10), 220, 140);
  vers(g, {"Kaum hatte sie die Spindel", "angerührt, so stach sie sich", "damit in den Finger ..."}, aufzu(v, 18, 24));
  vers(g, {"... und fiel in einen tiefen Schlaf."}, aufzu(v, 24.2f, 33.6f));
  blende(g, u, 42, 170, 170);
}

// 5) Das ganze Schloss schlaeft ein
static void szeneSchlaf(Gfx &g, float u) {
  if (u < 18) {
    // die Kueche: der Koch will den Kuechenjungen an den Haaren ziehen - und schlaeft ein
    bool schlaf = u > 4;
    kueche(g, u, schlaf);
    Pose k = posis(KOCH, u);
    k.armV = 2.2f;
    k.zu = schlaf;
    k.boese = schlaf ? 0 : 1;
    k.mund = schlaf ? 0 : 0.6f;
    schneiderlein(g, 100, 214, 1, 1, k);
    Pose j = posis(JUNGE, u);
    j.zu = schlaf;
    j.mund = schlaf ? 0 : 0.6f;
    j.armV = 0.4f;
    schneiderlein(g, 146, 214, 0.75f, -1, j);
    for (int i = 0; i < 4; i++) fliegeWand(g, 40 + i * 30, 110 + (i & 1) * 20, u, schlaf);
    if (schlaf) zzz(g, 110, 120, u);
    g.kameraAus();
    vers(g, {"Der König und die Königin", "schliefen ein, und der ganze", "Hofstaat mit ihnen."}, aufzu(u, 0.4f, 6.4f));
    vers(g, {"Die Fliegen an der Wand,", "das Feuer auf dem Herd,", "auch der Koch ..."}, aufzu(u, 6.6f, 17.6f));
    return;
  }
  float v = u - 18;
  // draussen: Pferde im Stall, die Hunde im Hof, die Tauben auf dem Dache - alles schlaeft
  himmel(g, 0.25f, u, 196);
  schloss(g, 160, 196, u, 0.2f);
  boden(g, 196);
  einhorn(g, 50, 214, 0.8f, -1, 1, 0.5f, 0, true);
  for (int i = 0; i < 3; i++) taube(g, 130 + i * 20, 104, 0.6f, 1, -1, 0);
  for (int i = 0; i < 3; i++) zzz(g, 60 + i * 80, 150 - i * 20, u + i);
  for (int i = 0; i < 6; i++) {
    float x = 20 + i * 48, y = 230;
    g.linie(x, y, x + 4, y - 10, 1, GRAS_D);  // stilles Gras
  }
  g.kameraAus();
  vers(g, {"Die Pferde schliefen im Stall,", "die Tauben auf dem Dache,", "und der Wind legte sich."}, aufzu(v, 0.6f, 15));
  blende(g, u, 34, 160, 150);
}

// 6) Die Dornenhecke waechst; hundert Jahre
static void szeneHecke(Gfx &g, float u) {
  float jahre = seg(u, 0, 26);
  float jahreszeit = fmodf(u * 0.35f, 1);
  himmel(g, 0.2f + 0.3f * sinf(u * 0.8f) * 0.5f, u, 196);
  schloss(g, 142, 196, u, 0.3f);
  boden(g, 196, mischeFarbe(GRAS, WEISS, jahreszeit > 0.75f ? 0.6f : 0));
  rosenhecke(g, sanft(seg(u, 0, 14)), 0, u);
  if (jahreszeit > 0.75f) for (int i = 0; i < 20; i++) g.kreis(zufall(i + 30) * 284, fmodf(zufall(i + 50) * 240 + u * 30, 240), 1.6f, WEISS);
  // Koenigssoehne bleiben in den Dornen haengen (nur angedeutet: sie kommen nicht hindurch und kehren um)
  if (u > 14 && u < 24) {
    Pose p = posis(PRINZ, u);
    float k = seg(u, 14, 24);
    p.lauf = fmodf(u * 1.4f, 1);
    p.armV = 2.2f;
    p.mund = 0.5f;
    float px = k < 0.5f ? lerp(-30, 60, k * 2) : lerp(60, -40, (k - 0.5f) * 2);
    schneiderlein(g, px, 214, 0.85f, k < 0.5f ? 1 : -1, p);
  }
  g.kameraAus();
  char jahr[24];
  snprintf(jahr, sizeof jahr, "%d Jahre", (int)(jahre * 100));
  g.text(SCHRIFT_M, jahr, 142, 228, WEISS, 1, (uint8_t)(255 * aufzu(u, 2, 32)));
  vers(g, {"Rings um das Schloss begann eine", "Dornenhecke zu wachsen."}, aufzu(u, 0.4f, 8));
  vers(g, {"Königssöhne kamen und wollten", "hindurch, aber sie konnten nicht."}, aufzu(u, 14, 24));
  blende(g, u, 34, 142, 150);
}

// 7) Nach hundert Jahren: die Hecke blueht und tut sich auf; der Kuss
static void szeneKuss(Gfx &g, float u) {
  if (u < 18) {
    himmel(g, 0, u, 196);
    sonne(g, 248, 30, 11, u);
    schloss(g, 142, 196, u, 0);
    boden(g, 196);
    float offen = sanft(seg(u, 6, 12));
    rosenhecke(g, 1, sanft(seg(u, 2, 6)), u, offen);
    Pose p = posis(PRINZ, u);
    float px = lerp(-30, 142, sanft(seg(u, 3, 15)));
    p.lauf = u > 3 && u < 15 ? fmodf(u * 1.4f, 1) : -1;
    schneiderlein(g, px, 214, 0.9f, 1, p);
    g.kameraAus();
    vers(g, {"Als der Königssohn sich der", "Hecke näherte, waren es lauter", "große schöne Blumen."}, aufzu(u, 2, 12));
    if (u > 17) g.rechteck(0, 0, 284, 240, 0, (uint8_t)(255 * seg(u, 17, 18)));
    return;
  }
  float v = u - 18;
  turmstube(g, u);
  g.kontur(1.4f, OUT);
  g.rundRechteck(150, 186, 110, 20, 6, FARBE(0xF2EEFF));
  g.konturAus();
  bool wach = v > 12;
  if (!wach) {
    g.ellipse(214, 182, 40, 10, ROESCHEN.kleid);
    g.ellipse(166, 176, 14, 9, ROESCHEN.haar);
    g.kreis(170, 178, 9, HAUT);
    g.bogen(172, 179, 2.2f, 0.3f, 2.8f, 1.2f, OUT);
  } else {
    FrauPose r = fposis(ROESCHEN, u, 1);
    r.mund = 0.4f;
    r.armV = 1.6f;
    frau(g, 190, 214, 0.9f, -1, r);
  }
  Pose p = posis(PRINZ, u);
  float px = lerp(60, 140, sanft(seg(v, 0, 5)));
  p.lauf = v < 5 ? fmodf(u * 1.4f, 1) : -1;
  p.zu = v > 8 && v < 11;
  p.armV = v > 7 ? 1.6f : 0.6f;
  schneiderlein(g, px, 214, 0.95f, 1, p);
  if (v > 8 && v < 11) herz(g, 160, 150, 7, FARBE(0xFF5A7A), (uint8_t)(255 * huepf(seg(v, 8, 11))));
  if (v > 11) glitzern(g, 190, 140, 34, u, 8, (uint8_t)(255 * (1 - seg(v, 15, 18))));
  if (v > 14) herzen(g, 170, 150, u, 4, 24);
  g.kameraAus();
  if (v < 1) g.rechteck(0, 0, 284, 240, 0, (uint8_t)(255 * (1 - v)));
  vers(g, {"Da lag sie und war so schön,", "dass er die Augen nicht", "abwenden konnte."}, aufzu(v, 1, 7.6f));
  vers(g, {"Er bückte sich und gab ihr", "einen Kuss. Da schlug", "Dornröschen die Augen auf."}, aufzu(v, 8, 16));
  blende(g, u, 44, 170, 150);
}

// 8) Alles erwacht: das Feuer flackert, die Fliegen krabbeln, der Koch gibt die Ohrfeige; Hochzeit
static void szeneErwachen(Gfx &g, float u) {
  if (u < 14) {
    kueche(g, u, false);
    float klatsch = seg(u, 4, 4.6f);
    Pose k = posis(KOCH, u);
    k.armV = u > 4 && u < 4.6f ? lerp(2.4f, 1.2f, klatsch) : 2.2f;
    k.mund = 0.6f;
    k.boese = 1;
    schneiderlein(g, 100, 214, 1, 1, k);
    Pose j = posis(JUNGE, u);
    j.mund = u > 4.6f ? 0.9f : 0.3f;
    j.zu = u > 4.6f && u < 6;
    schneiderlein(g, 146 + (u > 4.6f && u < 5.2f ? 6 : 0), 214, 0.75f, -1, j);
    if (u > 4.6f && u < 6) for (int i = 0; i < 4; i++) g.stern(150, 140 - i * 6, 3, 1.2f, 4, u * 6 + i, GOLD);
    for (int i = 0; i < 4; i++) fliegeWand(g, 40 + i * 30, 110 + (i & 1) * 20, u, false);
    g.kameraAus();
    vers(g, {"Da erwachte der ganze Hof.", "Das Feuer flackerte, die", "Fliegen krochen weiter."}, aufzu(u, 0.4f, 4));
    vers(g, {"Und der Koch gab dem Jungen eine", "Ohrfeige, dass er schrie."}, aufzu(u, 4.8f, 12));
    if (u > 13) g.rechteck(0, 0, 284, 240, 0, (uint8_t)(255 * seg(u, 13, 14)));
    return;
  }
  float v = u - 14;
  himmel(g, 0, u, 196);
  sonne(g, 244, 34, 12, u);
  schloss(g, 142, 196, u, 0);
  boden(g, 196);
  rosenhecke(g, 0.35f, 1, u, 1);
  FrauPose b = fposis(BRAUT, u, 1);
  b.mund = 0.4f;
  frau(g, 124, 214, 0.92f, 1, b);
  Pose p = posis(PRINZ, u);
  p.mund = 0.4f;
  p.armV = 2.5f + sinf(u * 8) * 0.3f;
  schneiderlein(g, 160, 214, 0.95f, -1, p);
  koenig(g, 60, 214, 0.85f, 1, 0.5f + 0.5f * sinf(u * 4));
  FrauPose k = fposis(KOENIGIN, u);
  k.armV = 2.4f;
  frau(g, 220, 214, 0.85f, -1, k);
  herzen(g, 142, 150, u, 6, 40);
  g.kameraAus();
  if (v < 1) g.rechteck(0, 0, 284, 240, 0, (uint8_t)(255 * (1 - v)));
  vers(g, {"Da ward die Hochzeit mit aller", "Pracht gefeiert, und sie lebten", "vergnügt bis an ihr Ende."}, aufzu(v, 1, 12));
  endeBild(g, v, 18);
}

static void bild(Gfx &g, float t) {
  if (t < P2) szeneFrosch(g, t - P1);
  else if (t < P3) szeneFest(g, t - P2);
  else if (t < P4) szeneSpindeln(g, t - P3);
  else if (t < P5) szeneTurm(g, t - P4);
  else if (t < P6) szeneSchlaf(g, t - P5);
  else if (t < P7) szeneHecke(g, t - P6);
  else if (t < P8) szeneKuss(g, t - P7);
  else szeneErwachen(g, t - P8);
}

// =====================================================================
// Musik
// =====================================================================
// Dornroeschens Walzer (D-Dur, traeumerisch)
static const Ton3 MEL_DORN[] = {
  {74, 1, 50}, {78, 1, 0}, {81, 1, 0}, {83, 2, 43}, {81, 1, 0}, {78, 2, 45}, {76, 1, 0}, {74, 3, 50},
  {76, 1, 45}, {78, 1, 0}, {79, 1, 0}, {81, 2, 50}, {78, 1, 0}, {76, 2, 45}, {73, 1, 0}, {74, 3, 50}};

static void ton(Synth &s, float t0, float t1) {
  float a = P1;
  spieleMelodie(s, M(MEL_WIEGE), a + 0.5f, a + 8.6f, 0.45f, t0, t1, 0.7f);
  for (int i = 0; i < 4; i++) if (bei(a + 9 + i * 1.6f, t0, t1)) s.ton(a + 9 + i * 1.6f, 160, 0.08f, 0.01f, 0.12f, RECHTECK, GERAEUSCH, 120);  // quak
  if (bei(a + 16.4f, t0, t1)) for (int k = 0; k < 10; k++) s.glocke(a + 16.4f + k * 0.15f, 76 + (k % 5) * 3, 0.45f);
  a = P2;
  spieleMelodie(s, M(MEL_F), a + 0.4f, a + 13.5f, 0.26f, t0, t1, 0.8f);
  for (int i = 0; i < 5; i++) if (bei(a + 2.2f + i * 2.2f, t0, t1)) for (int k = 0; k < 5; k++) s.glocke(a + 2.2f + i * 2.2f + k * 0.1f, 84 + k * 2, 0.35f);
  if (bei(a + 13.6f, t0, t1)) for (int k = 0; k < 8; k++) s.note(a + 13.6f + k * 0.06f, 50 + k * 3, 0.8f);  // die dreizehnte
  spieleMelodie(s, M(MEL_SPUK), a + 15.4f, a + 26, 0.34f, t0, t1, 0.8f);
  spieleMelodie(s, M(MEL_DORN), a + 27, a + 41.5f, 0.32f, t0, t1, 0.75f);
  if (bei(a + 27.4f, t0, t1)) for (int k = 0; k < 8; k++) s.glocke(a + 27.4f + k * 0.12f, 79 + k * 2, 0.4f);
  a = P3;
  for (float x = ceilf(t0 / 0.25f) * 0.25f; x < t1; x += 0.25f)
    if (x > a && x < a + 14) s.rauschen(x, 0.12f, HOCHPASS, 2500, 0.03f);  // Feuer knistert
  spieleMelodie(s, M(MEL_SPUK), a + 0.4f, a + 13.5f, 0.4f, t0, t1, 0.55f);
  a = P4;
  spieleMelodie(s, M(MEL_DORN), a + 0.4f, a + 8, 0.3f, t0, t1, 0.8f);
  for (float x = ceilf(t0 / 0.5f) * 0.5f; x < t1; x += 0.5f)
    if (x > a + 8 && x < a + 26) s.rauschen(x, 0.3f, BANDPASS, 700, 0.03f);  // Spinnrad
  spieleMelodie(s, M(MEL_SPUK), a + 9, a + 26, 0.4f, t0, t1, 0.55f);
  if (bei(a + 26, t0, t1)) s.glocke(a + 26, 96, 0.6f);  // pieks
  spieleMelodie(s, M(MEL_WIEGE), a + 27, a + 41.5f, 0.55f, t0, t1, 0.6f);
  a = P5;
  for (int i = 0; i < 12; i++) if (bei(a + 0.4f + i * 0.4f, t0, t1)) s.glocke(a + 0.4f + i * 0.4f, 84 - i * 2, 0.35f);  // alles schlaeft ein
  spieleMelodie(s, M(MEL_WIEGE), a + 5, a + 33.5f, 0.6f, t0, t1, 0.55f);
  a = P6;
  spieleMelodie(s, M(MEL_SPUK), a + 0.4f, a + 33.5f, 0.5f, t0, t1, 0.5f);
  for (float x = ceilf(t0 / 0.7f) * 0.7f; x < t1; x += 0.7f)
    if (x > a && x < a + 14) s.rauschen(x, 0.2f, BANDPASS, 900, 0.03f);  // die Hecke waechst
  a = P7;
  spieleMelodie(s, M(MEL_DORN), a + 0.4f, a + 17, 0.32f, t0, t1, 0.8f);
  for (int i = 0; i < 16; i++) if (bei(a + 2 + i * 0.25f, t0, t1)) s.glocke(a + 2 + i * 0.25f, 76 + (i * 5) % 14, 0.25f);  // Blueten
  spieleMelodie(s, M(MEL_WIEGE), a + 18.5f, a + 26, 0.5f, t0, t1, 0.6f);
  if (bei(a + 30, t0, t1)) for (int k = 0; k < 14; k++) s.glocke(a + 30 + k * 0.14f, 72 + (k % 7) * 2, 0.5f);  // erwacht
  spieleMelodie(s, M(MEL_DORN), a + 32.5f, a + 43.5f, 0.3f, t0, t1, 0.8f);
  a = P8;
  if (bei(a + 4.3f, t0, t1)) { s.ton(a + 4.3f, 1200, 0.15f, 0.002f, 0.08f, SINUS, GERAEUSCH, 600); s.rauschen(a + 4.3f, 0.06f, HOCHPASS, 3000, 0.15f); }  // klatsch
  if (bei(a + 4.6f, t0, t1)) for (int k = 0; k < 4; k++) s.note(a + 4.6f + k * 0.08f, 88 - k * 2, 0.6f);  // der Junge schreit
  spieleMelodie(s, M(MEL_F), a + 14.4f, a + 35, 0.26f, t0, t1, 0.9f);
  for (int i = 0; i < 8; i++) if (bei(a + 22 + i * 0.6f, t0, t1)) s.glocke(a + 22 + i * 0.6f, i & 1 ? 79 : 72, 0.7f);
  endeKlang(s, a + 34.2f, t0, t1);
}

// =====================================================================
// Startbild, Vorschau, Erkennungszeichen
// =====================================================================
static void titelSzene(Gfx &g, float t) {
  himmel(g, 0, t, 196);
  sonne(g, 248, 30, 11, t);
  schloss(g, 142, 196, t, 0);
  boden(g, 196);
  rosenhecke(g, 0.6f, 1, t, 0.4f);
  FrauPose r = fposis(ROESCHEN, t, 1);
  r.zu = true;
  r.armV = 1.2f;
  frau(g, 142, 222, 0.9f, 1, r);
  zzz(g, 160, 120, t);
}
static void titelbild(Gfx &g, float t) { titelSzene(g, t); titelRahmen(g, t, "Dornröschen", 180); }
static void vorschau(Gfx &g, float t) { titelSzene(g, t); }
static void symbol(Gfx &g, float x, float y, float t) {
  // eine grosse Rose mit Dornenranke und die Spindel
  g.kontur(1.4f, OUT);
  g.kurve(x - 46, y - 4, x - 10, y - 40, x + 30, y - 10, 3, FARBE(0x3E7A40));
  g.konturAus();
  for (int i = 0; i < 6; i++) { float k = i / 6.0f; float dx = lerp(x - 40, x + 24, k), dy = y - 10 - sinf(k * 3.14f) * 22; g.linie(dx, dy, dx + 3, dy - 5, 1.2f, FARBE(0x5A3A26)); }
  float b = 1 + sinf(t * 2) * 0.05f;
  for (int i = 0; i < 6; i++) {
    float a = i * 1.047f + t * 0.2f;
    g.kreis(x + cosf(a) * 9 * b, y - 50 + sinf(a) * 9 * b, 10 * b, FARBE(0xE8283F));
  }
  g.kreis(x, y - 50, 9 * b, FARBE(0xC01830));
  g.bogen(x, y - 50, 5, 0, 5, 2, FARBE(0xFF8AA0));
  spindel(g, x + 40, y - 30, 1.6f, t * 2);
  zzz(g, x - 40, y - 60, t, 200);
}

const Maerchen DORNROESCHEN = {"Dornröschen", PDAUER, FARBE(0xE8506A), bild, ton, titelbild, vorschau, symbol,
                               LIED(MEL_DORN, 0.34f), nullptr};

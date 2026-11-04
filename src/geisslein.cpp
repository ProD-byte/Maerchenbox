// Der Wolf und die sieben jungen Geisslein - ein Spieluhr-Film fuer die Maerchenbox, nach den Bruedern Grimm.
// Rauhe Stimme und schwarze Pfote, Kreide, Teig und Mehl, die sieben Verstecke, der Uhrkasten,
// Bauch auf, Wackersteine hinein, und am Brunnen: "Was rumpelt und pumpelt in meinem Bauch herum?"
#include "maerchen.h"
#include "schriften.h"
#include "figuren.h"
#include "melodien.h"
#include <stdio.h>
#include <initializer_list>

// ---------- Ablauf (Sekunden) ----------
static const float G1 = 0, G2 = 30, G3 = 52, G4 = 74, G5 = 100, G6 = 150, G7 = 184, G8 = 236, GDAUER = 300;

// =====================================================================
// Hilfen
// =====================================================================
static float aufzu(float u, float a, float b) { return federnd(seg(u, a, a + 0.4f)) * (1 - seg(u, b - 0.3f, b)); }

static void blase(Gfx &g, float x, float y, std::initializer_list<const char *> zeilen, float k, float sx, float sy,
                  Farbe f = WEISS) {
  if (k <= 0.05f) return;
  int w = 0, n = 0;
  for (const char *z : zeilen) { int b = g.textBreite(SCHRIFT_K, z); if (b > w) w = b; n++; }
  float bw = (w + 18) * k, bh = (8 + n * 18) * k;
  if (x - bw / 2 < 3) x = 3 + bw / 2;
  if (x + bw / 2 > 281) x = 281 - bw / 2;
  float sp[] = {x - 6, y + bh / 2 - 2, x + 6, y + bh / 2 - 2, sx, sy};
  g.kontur(1.4f, OUT);
  g.rundRechteck(x - bw / 2, y - bh / 2, bw, bh, 9 * k, f);
  g.konturAus();
  g.polygon(sp, 3, f);
  if (k > 0.85f) {
    int i = 0;
    for (const char *z : zeilen) g.text(SCHRIFT_K, z, x, y - bh / 2 + 18 + i++ * 18, OUT, 1);
  }
}

static void puff(Gfx &g, float x, float y, float k, Farbe c = WEISS) {
  if (k <= 0 || k >= 1) return;
  for (int i = 0; i < 8; i++) {
    float a = i * 0.785f;
    g.stern(x + cosf(a) * k * 36, y + sinf(a) * k * 30, 5 * (1 - k) + 2, 2, 5, a + k * 4, i & 1 ? c : GOLD, (uint8_t)(255 * (1 - k)));
  }
}

// =====================================================================
// Figuren
// =====================================================================
struct GeissPose {
  float lauf = -1, mund = 0, huepf = 0;
  bool zu = false, weint = false;
  bool korb = false;      // Mutter mit Korb (geht in den Wald)
};

// Geiss auf vier Beinen; mutter = gross mit Hoernern, Bart und Halstuch
static void geiss(Gfx &g, float x, float y, float s, float dir, const GeissPose &p, float t, Farbe fell, Farbe fleck,
                  bool mutter) {
  auto PX = [&](float dx) { return x + dx * s * dir; };
  float h = p.huepf * fabsf(sinf(t * 6 + x * 0.1f)) * 12 * s;
  auto PY = [&](float dy) { return y + dy * s - h; };
  const Farbe huf = FARBE(0x5A4636), schn = mischeFarbe(fell, FARBE(0xFFD9C0), 0.5f);
  g.kontur(1.3f * s, OUT);
  for (int i = 0; i < 4; i++) {
    float bx = i < 2 ? -20 + i * 8 : 14 + (i - 2) * 8;
    float sw = p.lauf >= 0 ? sinf(p.lauf * 6.2831853f + i * 1.6f) * 7 : 0;
    g.linie(PX(bx), PY(-26), PX(bx + sw), PY(-4), 4.5f * s, fell);
    g.ellipse(PX(bx + sw), PY(-2.5f), 3.5f * s, 2.5f * s, huf);
  }
  float sch[] = {PX(-26), PY(-38), PX(-34), PY(-48), PX(-28), PY(-36)};
  g.polygon(sch, 3, fell);
  g.ellipse(PX(0), PY(-34), 28 * s, 15 * s, fell);
  // Hals und Kopf
  float kx = PX(30), ky = PY(-56);
  g.linie(PX(18), PY(-38), kx - 4 * s * dir, ky + 6 * s, 10 * s, fell);
  g.ellipse(kx - 10 * s * dir, ky - 1 * s, 6 * s, 3 * s, fell, 255, 0.5f * dir);  // Ohr
  g.ellipse(kx, ky, 11 * s, 9 * s, fell, 255, 0.25f * dir);
  g.konturAus();
  // Hoerner
  float hl = mutter ? 1 : 0.35f;
  g.kurve(kx - 2 * s * dir, ky - 7 * s, kx - 6 * s * dir, ky - 18 * s * hl, kx - 14 * s * dir, ky - 20 * s * hl, (mutter ? 4 : 3) * s, FARBE(0xD9C9A8));
  g.ellipse(kx + 6 * s * dir, ky + 3 * s, 6 * s, 5 * s, schn);
  g.kreis(kx + 10 * s * dir, ky + 2 * s, 1.2f * s, OUT);
  if (p.zu) g.bogen(kx + 1 * s * dir, ky - 2 * s, 2.6f * s, 3.5f, 5.9f, 1.4f * s, OUT);
  else {
    g.ellipse(kx + 1 * s * dir, ky - 2 * s, 2.8f * s, 3.2f * s, SCHWARZ);
    g.kreis(kx + 2 * s * dir, ky - 3 * s, 1 * s, WEISS);
  }
  if (p.weint) {
    float k = fmodf(t * 1.3f, 1);
    g.kreis(kx + 1 * s * dir, ky + (2 + k * 10) * s, 1.4f * s, FARBE(0x8FD8FF), (uint8_t)(255 * (1 - k)));
  }
  if (p.mund > 0.05f) g.ellipse(kx + 7 * s * dir, ky + 7 * s, 2.4f * s, (1 + p.mund * 2.5f) * s, FARBE(0x8A2A2A));
  if (fleck) {
    g.kreis(PX(-8), PY(-38), 6 * s, fleck);
    g.kreis(PX(10), PY(-30), 5 * s, fleck);
  }
  if (mutter) {
    float b[] = {kx + 2 * s * dir, ky + 8 * s, kx + 6 * s * dir, ky + 8 * s, kx + 3 * s * dir, ky + 16 * s};
    g.polygon(b, 3, FARBE(0xE8E0D0));  // Bart
    float tuch[] = {PX(16), PY(-46), PX(26), PY(-48), PX(22), PY(-36)};
    g.polygon(tuch, 3, FARBE(0xE0473A));
    if (p.korb) {
      g.kontur(1.2f, OUT);
      g.rundRechteck(PX(12) - 9 * s, PY(-30), 18 * s, 12 * s, 3 * s, FARBE(0xC9A46A));
      g.konturAus();
      g.bogen(PX(12), PY(-30), 8 * s, 3.4f, 6.0f, 1.6f * s, FARBE(0xA8823E));
    }
  } else {
    g.kreis(kx - 4 * s * dir, ky + 8 * s, 2 * s, GOLD);  // Gloeckchen
  }
}

// die sieben Geisslein: Fellfarben und Flecken
static const Farbe KIND_FELL[7] = {FARBE(0xFFFFFF), FARBE(0xF4E6CC), FARBE(0xD9C2A0), FARBE(0xE8E8E8), FARBE(0xFFF6E8),
                                   FARBE(0xC9B08A), FARBE(0xFFFFFF)};
static const Farbe KIND_FLECK[7] = {0, FARBE(0xB0703A), 0, FARBE(0x8E8E9A), FARBE(0xD9A060), 0, FARBE(0xE8C8A0)};
static const Farbe MUTTER_FELL = FARBE(0xF2EADA);

static void kind(Gfx &g, int i, float x, float y, float dir, float t, float lauf = -1, float huepf = 0, float mund = 0,
                 float s = 0.5f) {
  GeissPose p;
  p.lauf = lauf;
  p.huepf = huepf;
  p.mund = mund;
  p.zu = fmodf(t + i * 0.7f, 3.3f) < 0.12f;
  geiss(g, x, y, s, dir, p, t + i, KIND_FELL[i], KIND_FLECK[i], false);
}

struct WolfPose {
  float lauf = -1;
  bool aufrecht = false, liegt = false, zu = false, naht = false;
  float mund = 0;         // 0..1 Maul auf
  float dick = 0;         // 0..1 voller Bauch
  bool weiss = false;     // Pfoten mit Mehl bestaeubt
  float pfote = 0;        // aufrecht: vordere Pfote hoch (zum Fenster)
  float buecken = 0;      // aufrecht: beugt sich vor (am Brunnen)
};

static void wolf(Gfx &g, float x, float y, float s, float dir, const WolfPose &p, float t, float *maulX = nullptr,
                 float *maulY = nullptr) {
  const Farbe fell = FARBE(0x7E8696), hell = FARBE(0xC9CDD6), dunkel = FARBE(0x5E6474);
  const Farbe pf = p.weiss ? WEISS : FARBE(0x2A2630);
  auto PX = [&](float dx) { return x + dx * s * dir; };
  auto PY = [&](float dy) { return y + dy * s; };
  float kx, ky, kw = 0;  // Kopf
  g.kontur(1.5f * s, OUT);
  if (p.liegt) {
    // liegt auf dem Ruecken und schlaeft, Beine in die Luft
    float atm = sinf(t * 1.6f) * 2 * s;
    for (int i = 0; i < 4; i++) {
      float bx = -24 + i * 14;
      g.linie(PX(bx), PY(-20), PX(bx + 3), PY(-40 - (i & 1) * 4), 6 * s, fell);
      g.kreis(PX(bx + 3), PY(-42 - (i & 1) * 4), 4 * s, pf);
    }
    g.kurve(PX(-36), PY(-14), PX(-56), PY(-14), PX(-62), PY(-26), 10 * s, fell);
    g.ellipse(PX(0), PY(-16), 38 * s, 14 * s, fell);
    g.ellipse(PX(2), PY(-26) - atm, (18 + p.dick * 14) * s, (8 + p.dick * 12) * s + atm, hell);
    if (p.naht)
      for (int i = 0; i < 6; i++) {
        float nx = PX(-12 + i * 5), ny = PY(-28 - p.dick * 12) - atm;
        g.linie(nx - 2 * s, ny - 3 * s, nx + 2 * s, ny + 3 * s, 1.2f * s, OUT);
      }
    kx = PX(40); ky = PY(-18); kw = 0.4f;
  } else if (!p.aufrecht) {
    for (int i = 0; i < 4; i++) {
      float bx = i < 2 ? -24 + i * 10 : 18 + (i - 2) * 10;
      float sw = p.lauf >= 0 ? sinf(p.lauf * 6.2831853f + i * 1.6f) * 9 : 0;
      g.linie(PX(bx), PY(-28), PX(bx + sw), PY(-5), 6 * s, i & 1 ? fell : dunkel);
      g.ellipse(PX(bx + sw + 1), PY(-3), 5 * s, 3 * s, pf);
    }
    g.kurve(PX(-30), PY(-36), PX(-50), PY(-34), PX(-54), PY(-20), 10 * s, fell);
    g.ellipse(PX(0), PY(-36), 34 * s, 15 * s, fell);
    if (p.dick > 0) g.ellipse(PX(2), PY(-28), (16 + p.dick * 10) * s, (8 + p.dick * 10) * s, hell);
    kx = PX(36); ky = PY(-50);
  } else {
    // aufrecht auf den Hinterbeinen
    float b = p.buecken;
    g.linie(PX(-8), PY(-34), PX(-10), PY(-4), 8 * s, dunkel);
    g.linie(PX(8), PY(-34), PX(10), PY(-4), 8 * s, fell);
    g.ellipse(PX(-10), PY(-3), 7 * s, 3.5f * s, pf);
    g.ellipse(PX(12), PY(-3), 7 * s, 3.5f * s, pf);
    g.kurve(PX(-14), PY(-30), PX(-34), PY(-30), PX(-38), PY(-14), 10 * s, fell);
    float ox = 0 + b * 18, oy = b * 10;
    g.ellipse(PX(ox * 0.5f), PY(-50 + oy * 0.5f), 18 * s, 26 * s, fell);
    g.ellipse(PX(4 + ox * 0.6f), PY(-46 + oy * 0.6f), (11 + p.dick * 9) * s, (14 + p.dick * 8) * s, hell);
    if (p.naht)
      for (int i = 0; i < 5; i++) {
        float nx = PX(8 + ox * 0.6f + p.dick * 6), ny = PY(-56 + i * 5 + oy * 0.6f);
        g.linie(nx - 3 * s, ny, nx + 3 * s, ny, 1.2f * s, OUT);
      }
    // Arme
    float w = 0.5f + p.pfote * 2.2f + b * 0.8f;
    float sx = PX(10 + ox), sy = PY(-66 + oy);
    float hx = sx + sinf(w) * 22 * s * dir, hy = sy + cosf(w) * 22 * s;
    g.linie(PX(-8 + ox), PY(-66 + oy), PX(-12 + ox), PY(-44 + oy), 6 * s, dunkel);
    g.linie(sx, sy, hx, hy, 6.5f * s, fell);
    g.kreis(hx, hy, 5 * s, pf);
    kx = PX(8 + ox * 1.2f); ky = PY(-84 + oy * 1.4f); kw = b * 0.6f;
  }
  // Kopf mit langer Schnauze
  float c = cosf(kw), sn = sinf(kw);
  auto K = [&](float dx, float dy, float *o) { o[0] = kx + (dx * c - dy * sn) * s * dir; o[1] = ky + (dx * sn * dir * dir + dy * c) * s; };
  float a[2], b2[2];
  K(-8, -12, a); K(4, -12, b2);
  float o1[] = {a[0] - 4 * s, a[1] + 4 * s, a[0], a[1] - 10 * s, a[0] + 5 * s, a[1] + 4 * s};
  float o2[] = {b2[0] - 4 * s, b2[1] + 4 * s, b2[0], b2[1] - 10 * s, b2[0] + 5 * s, b2[1] + 4 * s};
  g.polygon(o1, 3, fell);
  g.polygon(o2, 3, fell);
  g.kreis(kx, ky, 14 * s, fell);
  float m = p.mund;
  float sch[10];
  K(6, -4, sch); K(28, 0, sch + 2); K(28, 4 + m * 6, sch + 4); K(6, 8, sch + 6);
  g.polygon(sch, 4, fell);
  g.konturAus();
  if (m > 0.05f) {  // offenes Maul mit Zaehnen
    float u2[8];
    K(8, 6, u2); K(26, 6 + m * 8, u2 + 2); K(26, 10 + m * 10, u2 + 4); K(8, 10, u2 + 6);
    g.polygon(u2, 4, FARBE(0x7A2A2A));
    for (int i = 0; i < 3; i++) { float z[2]; K(12 + i * 5, 6 + m * 2, z); g.stern(z[0], z[1] + 1 * s, 2 * s, 0.8f * s, 2, 1.57f, WEISS); }
  }
  K(27, 1, a);
  g.kreis(a[0], a[1], 3 * s, SCHWARZ);
  K(4, -2, a);
  if (p.zu) g.bogen(a[0], a[1], 3 * s, 3.5f, 5.9f, 1.6f * s, OUT);
  else {
    g.kreis(a[0], a[1], 3 * s, FARBE(0xF2D24A));
    g.kreis(a[0] + 0.8f * s * dir, a[1], 1.4f * s, SCHWARZ);
    g.linie(a[0] - 4 * s, a[1] - 5 * s, a[0] + 4 * s, a[1] - 3 * s, 1.6f * s, OUT);  // listige Braue
  }
  if (maulX) { K(28, 6, a); *maulX = a[0]; *maulY = a[1]; }
}

static Aussehen kraemerLook() {
  Aussehen a;
  a.weste = FARBE(0x8E6E4E); a.hemd = FARBE(0xF4EEDC); a.hose = FARBE(0x4A3B2A); a.haar = FARBE(0x5A3A26);
  a.muetze = 1; a.halstuch = FARBE(0x3E7FDC);
  return a;
}
static Aussehen baeckerLook() {
  Aussehen a;
  a.weste = WEISS; a.hemd = FARBE(0xF4EEDC); a.hose = FARBE(0x6E6A70); a.haar = FARBE(0x8A6040);
  a.muetze = 2; a.hut = WEISS;
  return a;
}
static Aussehen muellerLook() {
  Aussehen a;
  a.weste = FARBE(0xE8E2D6); a.hemd = WEISS; a.hose = FARBE(0xCFC8BA); a.haar = FARBE(0xE8E8E8);
  a.hut = FARBE(0xF4F0E8); a.hutD = FARBE(0xD9D2C4); a.feder = FARBE(0xD9D2C4);
  return a;
}
static const Aussehen KRAEMER = kraemerLook(), BAECKER = baeckerLook(), MUELLER = muellerLook();

static void schere(Gfx &g, float x, float y, float s, float auf) {
  g.kreis(x - 4 * s, y + 4 * s, 3 * s, FARBE(0xE0473A));
  g.kreis(x + 4 * s, y + 4 * s, 3 * s, FARBE(0xE0473A));
  g.linie(x - 2 * s, y + 2 * s, x + (4 + auf * 4) * s, y - 12 * s, 2 * s, FARBE(0xC7D0DC));
  g.linie(x + 2 * s, y + 2 * s, x - (4 + auf * 4) * s + 8 * s, y - 12 * s, 2 * s, FARBE(0xC7D0DC));
}

// =====================================================================
// Kulissen
// =====================================================================
// das Haeuschen der Geissen von aussen; tuer 0 zu .. 1 offen; pfote: Wolfspfote auf dem Fensterbrett
static void haus(Gfx &g, float t, float tuer, int pfote = 0) {
  himmel(g, 0, t, 196);
  sonne(g, 244, 34, 12, t);
  huegel(g, 0, 0, 186, 30, 140, FARBE(0xA7DE85));
  laubbaum(g, 30, 200, 0.9f);
  boden(g, 196);
  blumen(g, 0, 200, 8, 91);
  g.kontur(1.8f, OUT);
  g.rechteck(110, 100, 130, 96, FARBE(0xF1DDB0));
  float d[] = {98, 102, 175, 56, 252, 102};
  g.polygon(d, 3, FARBE(0xC0632E));
  g.konturAus();
  g.rundRechteck(146, 136, 34, 60, 14, FARBE(0x3A2A20));
  if (tuer < 1) {
    g.kontur(1.4f, OUT);
    g.rundRechteck(146, 136, 34 * (1 - tuer * 0.85f), 60, 14 * (1 - tuer * 0.8f), HOLZ);
    g.konturAus();
  }
  g.rundRechteck(196, 120, 30, 26, 3, FARBE(0x8FD3FF));
  g.rechteck(210, 120, 2, 26, HOLZ);
  g.rechteck(192, 146, 38, 4, HOLZ_D);
  if (pfote) {  // die Pfote liegt im Fenster
    g.kontur(1.2f, OUT);
    g.ellipse(212, 145, 9, 5, pfote == 1 ? FARBE(0x2A2630) : WEISS);
    g.konturAus();
  }
}

// die Stube mit den sieben Verstecken; unordnung = 0..1 (nach dem Wolf)
static const float V_TISCH = 132, V_BETT = 52, V_OFEN = 122, V_KUECHE = 16, V_SCHRANK = 190, V_SCHUESSEL = 264, V_UHR = 228;
static void stube(Gfx &g, float t, float unordnung) {
  g.verlauf(0, 196, FARBE(0xF0D9B5), FARBE(0xE0BE8E));
  g.rechteck(0, 196, 284, 44, FARBE(0xB67A45));
  for (int i = 0; i < 3; i++) g.rechteck(0, 206 + i * 12, 284, 1.5f, FARBE(0x9C6436));
  // Kuechentuer links
  g.rundRechteck(2, 110, 30, 86, 12, FARBE(0x6E4426));
  // Bett
  g.kontur(1.4f, OUT);
  g.rundRechteck(30, 150, 8, 46, 2, HOLZ_D);
  g.rundRechteck(34, 172, 62, 16, 3, HOLZ);
  g.konturAus();
  if (unordnung < 0.5f) {
    g.ellipse(46, 168, 10, 5, WEISS);
    g.rundRechteck(40, 164, 56, 10, 4, FARBE(0x8EC1F0));
  }
  // Ofen
  g.kontur(1.6f, OUT);
  g.rundRechteck(100, 96, 44, 100, 8, FARBE(0x6EA37A));
  g.konturAus();
  for (int i = 0; i < 4; i++) g.rechteck(102, 104 + i * 22, 40, 2, FARBE(0x4E8360));
  g.rundRechteck(110, 150, 24, 20, 3, FARBE(0x2E2620));
  // Schrank
  g.kontur(1.6f, OUT);
  g.rundRechteck(168, 86, 44, 110, 4, HOLZ);
  g.konturAus();
  g.rechteck(189, 90, 2, 102, HOLZ_D);
  g.kreis(185, 140, 2, GOLD); g.kreis(195, 140, 2, GOLD);
  // Wanduhr mit Kasten
  g.kontur(1.6f, OUT);
  g.rundRechteck(216, 60, 24, 136, 6, FARBE(0x7E4620));
  g.kreis(228, 78, 10, FARBE(0xFFF3D6));
  g.konturAus();
  float zw = t * 0.5f;
  g.linie(228, 78, 228 + cosf(zw) * 7, 78 + sinf(zw) * 7, 1.5f, OUT);
  g.linie(228, 78, 228 + cosf(zw * 12) * 5, 78 + sinf(zw * 12) * 5, 1.2f, OUT);
  g.rundRechteck(220, 104, 16, 84, 4, FARBE(0x5E3416));
  float pend = sinf(t * 4) * 0.3f;
  g.linie(228, 108, 228 + sinf(pend) * 30, 108 + cosf(pend) * 30, 1.4f, GOLD);
  g.kreis(228 + sinf(pend) * 30, 108 + cosf(pend) * 30, 3.5f, GOLD);
  // Waschschuessel auf dem Schemel
  g.rechteck(252, 176, 24, 20, HOLZ_D);
  if (unordnung < 0.5f) {
    g.kontur(1.2f, OUT);
    g.ellipse(264, 172, 16, 6, FARBE(0xE8F0FF));
    g.konturAus();
  } else {  // Scherben
    for (int i = 0; i < 5; i++) {
      float q[] = {250 + i * 6.0f, 212, 256 + i * 6.0f, 210, 254 + i * 6.0f, 216};
      g.polygon(q, 3, FARBE(0xE8F0FF));
    }
  }
  // Tisch mit Stuehlen (vorn); umgeworfen bei Unordnung
  if (unordnung < 0.5f) {
    g.kontur(1.6f, OUT);
    g.rundRechteck(92, 170, 80, 8, 3, HOLZ);
    g.konturAus();
    g.rechteck(98, 178, 6, 40, HOLZ_D);
    g.rechteck(160, 178, 6, 40, HOLZ_D);
  } else {
    g.kontur(1.6f, OUT);
    float q[] = {96, 214, 104, 206, 176, 214, 168, 222};
    g.polygon(q, 4, HOLZ);
    g.konturAus();
    g.linie(110, 208, 120, 186, 5, HOLZ_D);
    g.linie(150, 210, 160, 190, 5, HOLZ_D);
    g.rundRechteck(30, 212, 60, 10, 4, FARBE(0x8EC1F0));  // Bettdecke am Boden
    g.ellipse(80, 220, 10, 5, WEISS);                    // Kissen
  }
}

static void brunnen(Gfx &g, float x, float y, float platsch) {
  g.kontur(1.6f, OUT);
  g.rundRechteck(x - 34, y - 34, 68, 34, 6, STEIN);
  g.konturAus();
  for (int i = 0; i < 3; i++) g.rechteck(x - 34, y - 30 + i * 10, 68, 1.5f, STEIN_D);
  g.ellipse(x, y - 34, 32, 6, FARBE(0x2A3A5A));
  g.rechteck(x - 30, y - 92, 6, 58, HOLZ_D);
  g.rechteck(x + 24, y - 92, 6, 58, HOLZ_D);
  float d[] = {x - 42, y - 88, x, y - 110, x + 42, y - 88};
  g.kontur(1.4f, OUT);
  g.polygon(d, 3, FARBE(0xC0632E));
  g.konturAus();
  g.linie(x, y - 88, x, y - 60, 1.2f, FARBE(0xC9A46A));
  g.rundRechteck(x - 6, y - 62, 12, 10, 2, FARBE(0x8E5426));
  if (platsch > 0 && platsch < 1)
    for (int i = 0; i < 8; i++) {
      float a = 3.4f + i * 0.35f;
      g.kreis(x + cosf(a) * platsch * 40, y - 34 + sinf(a) * platsch * 50, 3 * (1 - platsch) + 1, FARBE(0x8FD8FF));
    }
}

static void wiese(Gfx &g, float t) {
  himmel(g, 0, t, 196);
  sonne(g, 244, 34, 12, t);
  wolke(g, 40 + t * 2, 40, 0.9f);
  huegel(g, 0, 0, 186, 30, 140, FARBE(0xA7DE85));
  boden(g, 196);
  blumen(g, 0, 200, 14, 57);
}

// =====================================================================
// Szenen
// =====================================================================
// 1) Die Mutter geht in den Wald und warnt vor dem Wolf
static void szeneAbschied(Gfx &g, float u) {
  haus(g, u, 1);
  GeissPose m;
  m.korb = true;
  m.mund = u > 2 && u < 14 && fmodf(u, 0.4f) < 0.2f ? 0.5f : 0;
  m.zu = fmodf(u, 3.9f) < 0.13f;
  float mx = 110;
  if (u > 21) { mx = lerp(110, -80, seg(u, 21, 28)); m.lauf = fmodf(u * 1.4f, 1); }
  geiss(g, mx, 214, 1, u > 21 ? -1 : 1, m, u, MUTTER_FELL, 0, true);
  for (int i = 0; i < 7; i++) {
    float x = 156 + (i % 4) * 26, y = 214 + (i / 4) * 12 - 6;
    kind(g, i, x, y, -1, u, -1, u > 16 && u < 20 ? 0.4f : 0, u > 14.5f && u < 18 ? 0.4f : 0);
  }
  blase(g, 120, 44, {"Liebe Kinder, ich will in den Wald.", "Seid auf eurer Hut vor dem Wolf!", "An seiner rauen Stimme und an", "seinen schwarzen Füßen erkennt ihr ihn."}, aufzu(u, 2, 13.6f), 130, 150);
  blase(g, 196, 60, {"Liebe Mutter, wir wollen", "uns schon in Acht nehmen."}, aufzu(u, 14.4f, 19.4f), 196, 160);
  if (u > 22) for (int i = 0; i < 3; i++) {
      float ph = fmodf(u * 0.4f + i / 3.0f, 1);
      notenzeichen(g, 180 + i * 30, 120 - ph * 50, i & 1 ? FARBE(0x3E7FDC) : FARBE(0xE8473D), (uint8_t)(220 * huepf(ph)));
    }
  blende(g, u, 30, 140, 150);
}

// Wolf an der Tuer (aufrecht), Geisslein sieht man im Fenster
static void tuerWolf(Gfx &g, float u, float x, const WolfPose &p) {
  wolf(g, x, 214, 0.95f, -1, p, u);
}

// 2) Der Wolf klopft: "Macht auf ..." - "Du hast eine raue Stimme, du bist der Wolf!"
static void szeneWolf1(Gfx &g, float u) {
  haus(g, u, 0);
  WolfPose w;
  w.aufrecht = true;
  float wx = lerp(-60, 100, sanft(seg(u, 0, 4)));
  if (u < 4) { w.aufrecht = false; w.lauf = fmodf(u * 1.6f, 1); }
  if (u > 4 && u < 5.5f) w.pfote = 0.6f + fabsf(sinf(u * 14)) * 0.3f;  // klopft
  w.mund = u > 5.5f && u < 10 && fmodf(u, 0.4f) < 0.2f ? 0.6f : 0;
  if (u > 15) { w.mund = 0.4f; }
  if (u > 17) { wx = lerp(100, -80, seg(u, 17, 21)); w.aufrecht = false; w.lauf = fmodf(u * 1.6f, 1); }
  wolf(g, wx, 214, 0.95f, u > 17 ? -1 : 1, w, u);
  // Geisslein schauen hinter dem Fenster
  g.ausschnitt(196, 120, 30, 26);
  for (int i = 0; i < 3; i++) kind(g, i, 200 + i * 10, 154, -1, u, -1, 0, u > 10 && u < 15 ? 0.5f : 0, 0.42f);
  g.ausschnittAus();
  blase(g, 110, 46, {"Macht auf, ihr lieben Kinder,", "eure Mutter ist da und hat jedem", "von euch etwas mitgebracht."}, aufzu(u, 5.4f, 10), 120, 110);
  blase(g, 170, 40, {"Wir machen nicht auf!", "Deine Stimme ist rau,", "du bist der Wolf!"}, aufzu(u, 10.4f, 15.6f), 210, 120);
  blende(g, u, 22, 140, 150);
}

// 3) Der Wolf kauft Kreide beim Kraemer; zweites Mal: die schwarze Pfote im Fenster
static void szeneKreide(Gfx &g, float u) {
  if (u < 9) {
    // beim Kraemer
    g.verlauf(0, 196, FARBE(0xF2DFC0), FARBE(0xE2C29A));
    g.rechteck(0, 196, 284, 44, FARBE(0x9E7656));
    g.rechteck(20, 70, 244, 5, HOLZ_D);
    for (int i = 0; i < 8; i++) {
      g.kontur(1, OUT);
      g.rundRechteck(26 + i * 30, 50, 22, 20, 4, i & 1 ? FARBE(0xD9A060) : FARBE(0x9AA6B8));
      g.konturAus();
    }
    g.kontur(1.6f, OUT);
    g.rundRechteck(150, 150, 120, 46, 4, HOLZ);
    g.konturAus();
    Pose k;
    k.aussehen = &KRAEMER;
    k.armV = u > 3 && u < 5 ? 1.6f : 0.6f;
    float khx, khy;
    schneiderlein(g, 220, 176, 0.95f, -1, k, &khx, &khy);
    WolfPose w;
    w.aufrecht = true;
    w.pfote = u > 4.6f && u < 6 ? 1.2f : 0.3f;
    w.mund = u > 6 && u < 8 ? 0.8f : 0;
    float mx, my;
    wolf(g, 100, 214, 0.95f, 1, w, u, &mx, &my);
    if (u > 3 && u < 4.6f) { g.kontur(1, OUT); g.rundRechteck(khx - 8, khy - 6, 16, 10, 2, WEISS); g.konturAus(); }  // ein grosses Stueck Kreide
    if (u > 4.6f && u < 6.4f) { g.kontur(1, OUT); g.rundRechteck(lerp(khx, mx, seg(u, 4.6f, 6)) - 8, lerp(khy, my, seg(u, 4.6f, 6)) - 6, 16, 10, 2, WEISS); g.konturAus(); }
    if (u > 6.4f && u < 8) for (int i = 0; i < 4; i++) g.kreis(mx + (i - 1.5f) * 4, my + sinf(u * 12 + i) * 3, 1.6f, WEISS);  // knirsch
    blase(g, 100, 46, {"Ein großes Stück Kreide!"}, aufzu(u, 0.6f, 3.4f), 120, 120);
    blende(g, u, 22, 140, 150);
    return;
  }
  float v = u - 9;
  haus(g, u, 0, v > 3 && v < 11 ? 1 : 0);
  WolfPose w;
  w.aufrecht = true;
  w.pfote = v > 3 && v < 11 ? 1.6f : 0.3f;
  w.mund = v > 0.6f && v < 3 && fmodf(v, 0.4f) < 0.2f ? 0.5f : 0;
  float wx = 188;
  if (v > 11) { wx = lerp(188, -80, seg(v, 11, 13)); w.aufrecht = false; w.lauf = fmodf(u * 1.6f, 1); }
  wolf(g, wx, 214, 0.95f, v > 11 ? -1 : 1, w, u);
  blase(g, 110, 46, {"Macht auf, ihr lieben Kinder!"}, aufzu(v, 0.6f, 3.2f), 140, 110);
  blase(g, 150, 40, {"Unsere Mutter hat keinen", "schwarzen Fuß wie du:", "du bist der Wolf!"}, aufzu(v, 5, 10.4f), 206, 118);
  blende(g, u, 22, 140, 150);
}

// 4) Beim Baecker (Teig auf die Pfote) und beim Mueller (Mehl darauf)
static void szeneMehl(Gfx &g, float u) {
  bool baecker = u < 11;
  float v = baecker ? u : u - 11;
  g.verlauf(0, 196, baecker ? FARBE(0xF6E0BE) : FARBE(0xECE6DC), baecker ? FARBE(0xE6C496) : FARBE(0xD6CEC0));
  g.rechteck(0, 196, 284, 44, baecker ? FARBE(0xA8805A) : FARBE(0xB8AE9C));
  if (baecker) {  // Backofen und Brote
    g.kontur(1.6f, OUT);
    g.rundRechteck(170, 90, 100, 106, 20, FARBE(0xC9A47A));
    g.konturAus();
    g.rundRechteck(192, 130, 56, 40, 20, FARBE(0x2E2620));
    for (int i = 0; i < 3; i++) g.ellipse(200 + i * 20, 158, 8, 5, FARBE(0xD9A060));
    for (int i = 0; i < 4; i++) { g.kontur(1, OUT); g.ellipse(30 + i * 24, 70, 10, 6, FARBE(0xC98A45)); g.konturAus(); }
  } else {  // Muehle mit Mehlsaecken
    for (int i = 0; i < 3; i++) { g.kontur(1.2f, OUT); g.rundRechteck(190 + i * 26, 160, 22, 36, 8, FARBE(0xF4F0E8)); g.konturAus(); }
    float r = u * 1.5f;
    for (int i = 0; i < 4; i++) g.linie(60, 80, 60 + cosf(r + i * 1.57f) * 46, 80 + sinf(r + i * 1.57f) * 46, 8, HOLZ);  // Muehlrad
    g.kreis(60, 80, 8, HOLZ_D);
    for (int i = 0; i < 10; i++) g.kreis(fmodf(zufall(i + 50) * 284 + u * 6, 284), fmodf(zufall(i + 60) * 200 + u * 10, 200), 1.6f, WEISS, 150);
  }
  Pose p;
  p.aussehen = baecker ? &BAECKER : &MUELLER;
  p.armV = v > 4 && v < 7 ? 1.5f + sinf(v * 10) * 0.2f : 0.5f;
  if (!baecker && v > 2.4f && v < 4) { p.boese = 0.4f; p.armH = 0.2f; p.armV = 0.3f; }  // weigert sich erst
  if (!baecker && v > 4) { p.mund = 0.4f; }
  float hx, hy;
  schneiderlein(g, 160, 214, 0.95f, -1, p, &hx, &hy);
  WolfPose w;
  w.aufrecht = true;
  w.pfote = 1.1f;
  w.weiss = !baecker && v > 6.4f;
  w.mund = (v > 0.6f && v < 2.4f) || (!baecker && v > 3 && v < 4.4f) ? 0.6f : 0;
  float wx = 90;
  if (v > 8.5f) { wx = lerp(90, -80, seg(v, 8.5f, 10.6f)); w.aufrecht = false; w.lauf = fmodf(u * 1.6f, 1); }
  wolf(g, wx, 214, 0.95f, v > 8.5f ? -1 : 1, w, u);
  if (v > 4 && v < 7) {
    if (baecker) g.ellipse(hx - 10, hy + 2, 7, 4, FARBE(0xF4DEB0));  // Teig
    else for (int i = 0; i < 8; i++) {
        float k = fmodf(v * 2 + i * 0.125f, 1);
        g.kreis(hx - 6 - k * 10, hy + k * 14, 1.8f, WEISS, (uint8_t)(255 * (1 - k)));  // Mehl
      }
  }
  if (baecker) {
    blase(g, 120, 46, {"Ich habe mich an den Fuß gestoßen,", "streich mir Teig darüber!"}, aufzu(v, 0.6f, 4), 110, 110);
  } else {
    blase(g, 110, 46, {"Streu mir weißes Mehl", "auf meine Pfote!"}, aufzu(v, 0.4f, 2.4f), 110, 110);
    blase(g, 110, 46, {"Wenn du es nicht tust,", "so fresse ich dich!"}, aufzu(v, 2.6f, 4.4f), 110, 110);
    blase(g, 180, 50, {"So sind die Menschen."}, aufzu(v, 7, 9.6f), 168, 120);
  }
  blende(g, u, 26, 140, 150);
}

// Verstecke: wo sich jedes Geisslein versteckt (x, y), und wann der Wolf es findet
static const float VERSTECK[7][2] = {{V_TISCH, 214}, {V_BETT + 22, 184}, {V_OFEN, 168}, {V_KUECHE, 196},
                                     {V_SCHRANK, 150}, {V_SCHUESSEL, 196}, {V_UHR, 186}};
// 5) Der Wolf kommt herein; die Geisslein verstecken sich; er findet sechs - nicht das juengste im Uhrkasten
static void szeneFressen(Gfx &g, float u) {
  if (u < 13) {
    // drittes Mal: feine Stimme (Kreide) und weisse Pfote - sie machen auf
    haus(g, u, seg(u, 9.4f, 10.4f), u > 3 && u < 9 ? 2 : 0);
    WolfPose w;
    w.aufrecht = true;
    w.weiss = true;
    w.pfote = u > 3 && u < 9 ? 1.6f : 0.3f;
    w.mund = u > 0.6f && u < 3 && fmodf(u, 0.4f) < 0.2f ? 0.4f : u > 10 ? 1 : 0;
    float wx = u < 10 ? 188 : lerp(188, 164, seg(u, 10, 11.5f));
    wolf(g, wx, 214, 0.95f, u < 10 ? 1 : -1, w, u);
    blase(g, 100, 46, {"Macht auf, Kinder!", "Ich bin eure Mutter."}, aufzu(u, 0.6f, 3.2f), 140, 110);
    blase(g, 150, 40, {"Zeig uns erst deine Pfote!"}, aufzu(u, 3.4f, 5.6f), 206, 118);
    blase(g, 150, 40, {"Weiß ist sie - das ist die Mutter!"}, aufzu(u, 6, 8.8f), 206, 118);
    if (u > 11) g.rechteck(0, 0, 284, 240, 0, (uint8_t)(255 * seg(u, 12, 13)));
    if (u < 1.2f) { g.kameraAus(); g.iris(150, 150, lerp(0, 330, sanft(u / 1.2f))); }
    return;
  }
  float v = u - 13;
  stube(g, u, 0);
  // Wolf springt herein und sucht (Reihenfolge wie im Maerchen)
  const float fund[6] = {7, 11, 15, 19, 23, 27};  // Tisch, Bett, Ofen, Kueche, Schrank, Waschschuessel
  int gefressen = 0;
  for (int i = 0; i < 6; i++) if (v > fund[i] + 1.2f) gefressen++;
  // der Wolf geht von Versteck zu Versteck (aus der Zeit berechnet, damit alle Bildstreifen gleich sind)
  float wy = 214;
  float wxg = v < 3 ? lerp(320, 240, sanft(seg(v, 0, 2))) : 240;
  for (int i = 0; i < 6; i++) {
    float von = i == 0 ? 240 : VERSTECK[i - 1][0] + 34, nach = VERSTECK[i][0] + 34;
    if (v > fund[i] - 3) wxg = lerp(von, nach, sanft(seg(v, fund[i] - 3, fund[i] - 0.4f)));
  }
  WolfPose w;
  w.weiss = true;
  w.dick = gefressen / 6.0f;
  w.lauf = fmodf(u * 1.5f, 1);
  bool schnapp = false;
  for (int i = 0; i < 6; i++) if (v > fund[i] && v < fund[i] + 1.4f) schnapp = true;
  w.mund = schnapp ? 1 : 0.3f;
  if (v > 31) wxg = lerp(VERSTECK[5][0] + 34, 330, seg(v, 31, 35));
  float mx, my;
  wolf(g, wxg, wy, 0.9f, v > 31 ? 1 : -1, w, u, &mx, &my);
  // Geisslein: laufen zu ihren Verstecken (0-3), schauen hervor, werden verschluckt
  for (int i = 0; i < 7; i++) {
    float sx = 150 + (i % 4) * 24, sy = 214;
    float zx = VERSTECK[i][0], zy = VERSTECK[i][1];
    float k = sanft(seg(v, 0.2f + i * 0.2f, 2.2f + i * 0.2f));
    float x = lerp(sx, zx, k), y = lerp(sy, zy, k) - sinf(k * 3.14159f) * 20;
    if (i < 6 && v > fund[i]) {
      float s2 = seg(v, fund[i], fund[i] + 1.2f);
      if (s2 >= 1) continue;
      x = lerp(zx, mx, s2); y = lerp(zy, my, s2) - sinf(s2 * 3.14159f) * 16;
      kind(g, i, x, y, 1, u, -1, 0, 0.8f, 0.5f * (1 - s2 * 0.7f));
      continue;
    }
    if (k >= 1 && i == 6) {  // das juengste guckt aus dem Uhrkasten
      g.ausschnitt(220, 104, 16, 84);
      kind(g, i, 228, 188, -1, u, -1, 0, 0, 0.42f);
      g.ausschnittAus();
      continue;
    }
    if (k >= 1 && (i == 2 || i == 4 || i == 3)) {  // im Ofen, Schrank, in der Kueche: nur ein Stueck sichtbar
      if (i == 2) { g.ausschnitt(110, 150, 24, 20); kind(g, i, zx, 172, -1, u, -1, 0, 0, 0.42f); g.ausschnittAus(); }
      if (i == 3) { g.ausschnitt(2, 110, 30, 86); kind(g, i, 14, 196, 1, u, -1, 0, 0, 0.45f); g.ausschnittAus(); }
      if (i == 4) { g.kreis(190, 140, 3, KIND_FELL[4]); }
      continue;
    }
    kind(g, i, x, y, -1, u, k < 1 ? fmodf(u * 2, 1) : -1, 0, k < 1 ? 0.7f : 0, k >= 1 && i == 5 ? 0.42f : 0.5f);
  }
  // das Bett und die Waschschuessel noch einmal darueber (Versteck)
  if (v > 2 && v < fund[1] + 0.2f) { g.rundRechteck(40, 164, 56, 10, 4, FARBE(0x8EC1F0)); }
  if (v > 2 && v < fund[5] + 0.2f) { g.kontur(1.2f, OUT); g.ellipse(264, 190, 14, 9, FARBE(0xE8F0FF)); g.konturAus(); }
  // Tisch vorn drueber
  if (v < fund[0] + 0.2f) {
    g.kontur(1.6f, OUT);
    g.rundRechteck(92, 170, 80, 8, 3, HOLZ);
    g.konturAus();
  }
  for (int i = 0; i < 6; i++) if (v > fund[i] + 1 && v < fund[i] + 1.6f) {
      g.text(SCHRIFT_M, "Happs!", mx, my - 30, OUT, 1);
    }
  blende(g, u, 50, 150, 150);
}

// 6) Die Mutter kommt heim: alles durcheinander, sie ruft - das juengste steckt im Uhrkasten
static void szeneHeim(Gfx &g, float u) {
  stube(g, u, 1);
  GeissPose m;
  m.korb = u < 26;
  m.mund = (u > 6 && u < 12) ? 0.6f : u > 24 ? 0.3f : 0;
  m.weint = u > 22;
  m.zu = fmodf(u, 3.9f) < 0.13f;
  float mx = lerp(320, 170, sanft(seg(u, 0.5f, 4)));
  if (u > 12) mx = lerp(170, 200, seg(u, 12, 14));
  m.lauf = (u < 4) || (u > 12 && u < 14) ? fmodf(u * 1.4f, 1) : -1;
  geiss(g, mx, 214, 1, u < 12 ? -1 : 1, m, u, MUTTER_FELL, 0, true);
  // das juengste springt aus dem Uhrkasten
  if (u > 14.5f) {
    float k = sanft(seg(u, 14.5f, 16));
    kind(g, 6, lerp(228, 250, k), lerp(186, 214, k) - sinf(k * 3.14159f) * 20, -1, u, -1, 0, u > 17 && u < 22 ? 0.6f : 0);
  } else if (u > 12) {
    g.rundRechteck(220, 104, 16 * (1 - seg(u, 13.6f, 14.4f)), 84, 4, FARBE(0x5E3416));
  }
  blase(g, 130, 40, {"Kinder! Wo seid ihr?"}, aufzu(u, 5, 8.4f), 160, 150);
  blase(g, 120, 40, {"Liebe Mutter,", "ich stecke im Uhrkasten!"}, aufzu(u, 9, 12.6f), 220, 110);
  blase(g, 150, 40, {"Der Wolf ist gekommen", "und hat die anderen gefressen!"}, aufzu(u, 17, 22), 240, 160);
  blende(g, u, 34, 150, 150);
}

// 7) Auf der Wiese: der Wolf schlaeft, im Bauch zappelt es; Schere, Nadel, Faden; Wackersteine
static void szeneBauch(Gfx &g, float u) {
  wiese(g, u);
  laubbaum(g, 150, 204, 1.3f);
  WolfPose w;
  w.liegt = true;
  w.zu = true;
  w.weiss = true;
  w.dick = 1;
  w.naht = u > 40;
  wolf(g, 150, 214, 0.95f, 1, w, u);
  // im Bauch zappelt es
  if (u < 22) for (int i = 0; i < 3; i++) {
      float k = fmodf(u * 0.8f + i * 0.33f, 1);
      if (k < 0.3f) g.kreis(140 + i * 10, 186 - k * 10, 3, FARBE(0xC9CDD6));
    }
  if (u < 40) zzz(g, 190, 150, u);
  GeissPose m;
  m.mund = (u > 3 && u < 8) || (u > 30 && u < 34) ? 0.5f : 0;
  m.zu = fmodf(u, 3.9f) < 0.13f;
  geiss(g, 80, 214, 1, 1, m, u, MUTTER_FELL, 0, true);
  // das juengste holt Schere, Nadel und Faden
  float kx = 50;
  if (u > 9 && u < 16) { kx = u < 12.5f ? lerp(50, -40, seg(u, 9, 11)) : lerp(-40, 108, seg(u, 13, 15.5f)); }
  if (u >= 16) kx = 108;
  kind(g, 6, kx, 214, u > 9 && u < 11.5f ? -1 : 1, u, (u > 9 && u < 15.5f) ? fmodf(u * 2, 1) : -1);
  if (u > 13 && u < 22) schere(g, kx + 10, 190, 1, u > 17 ? fabsf(sinf(u * 8)) : 0);
  // aufschneiden: die sechs springen heraus, gesund und munter
  for (int i = 0; i < 6; i++) {
    float an = 18 + i * 0.6f;
    if (u < an) continue;
    float k = sanft(seg(u, an, an + 1.2f));
    float zx = 30 + i * 40;
    float x = lerp(146, zx, k), y = lerp(184, 214, k) - sinf(k * 3.14159f) * 40;
    bool traegt = u > 30 && u < 40;
    if (traegt) {  // Wackersteine holen und in den Bauch stecken
      float w2 = fmodf((u - 30) / 5 + i * 0.15f, 1);
      x = lerp(zx, 150, huepf(w2)); y = 214;
    }
    kind(g, i, x, y, x < 150 ? 1 : -1, u, traegt ? fmodf(u * 2, 1) : -1, u > 24 && u < 30 ? 0.6f : 0, u < 30 ? 0.6f : 0);
    if (traegt) steinbrocken(g, x, 190, 5);
  }
  if (u > 40 && u < 46) {  // zunaehen
    float nx = 140 + fmodf(u * 6, 20);
    g.linie(nx, 170, nx + 6, 160, 1.4f, FARBE(0xC7D0DC));
    g.kurve(nx + 6, 160, nx + 20, 150, 108, 186, 0.8f, OUT);
  }
  blase(g, 120, 50, {"Ach Gott, sollten meine Kinder", "noch am Leben sein?"}, aufzu(u, 3, 8.4f), 92, 160);
  blase(g, 140, 46, {"Jetzt geht und sucht Wackersteine!"}, aufzu(u, 26.4f, 30), 92, 160);
  if (u > 46 && u < 50)
    for (int i = 0; i < 6; i++) g.stern(40 + i * 40, 150, 4, 1.6f, 5, u * 3 + i, GOLD);
  blende(g, u, 52, 150, 150);
}

// 8) Am Brunnen: "Was rumpelt und pumpelt in meinem Bauch herum?" - Platsch - "Der Wolf ist tot!"
static void szeneBrunnen(Gfx &g, float u) {
  wiese(g, u);
  brunnen(g, 196, 214, seg(u, 22.6f, 25));
  // Geisslein verstecken sich hinter dem Busch, dann tanzen sie
  bool tanz = u > 27;
  if (!tanz) {
    g.kontur(1.4f, OUT);
    g.kreis(40, 196, 26, FARBE(0x4CB85A));
    g.kreis(70, 200, 22, FARBE(0x4CB85A));
    g.konturAus();
    for (int i = 0; i < 3; i++) kind(g, i, 30 + i * 18, 182, 1, u, -1, 0, 0, 0.38f);
  }
  // Wolf erwacht, hat Durst, geht zum Brunnen (die Steine klappern), beugt sich hinunter und faellt hinein
  if (u < 22.6f) {
    WolfPose w;
    w.aufrecht = true;
    w.weiss = true;
    w.dick = 1;
    w.naht = true;
    w.mund = u > 6 && u < 16 && fmodf(u, 0.4f) < 0.2f ? 0.5f : 0;
    w.zu = u < 2;
    float wx = lerp(80, 152, seg(u, 2, 6));
    float wy = 214 - (u > 2 && u < 6 ? fabsf(sinf(u * 6)) * 3 : 0);
    w.buecken = sanft(seg(u, 18, 21));
    float fall = seg(u, 21, 22.6f);
    wolf(g, wx + fall * 30, wy + hinein(fall) * 40, 0.95f, 1, w, u);
    if (fall > 0) g.ellipse(196, 180, 34, 6, FARBE(0x2A3A5A));  // Brunnenrand vorn
  }
  if (u > 22 && u < 24) g.wackeln(sinf(u * 90) * 3, cosf(u * 80) * 2);
  if (tanz) {
    GeissPose m;
    m.mund = 0.6f;
    m.huepf = 0.6f;
    for (int i = 0; i < 8; i++) {
      float a = (u - 27) * 1.3f + i * 0.785f;
      float x = 196 + cosf(a) * 80, y = 218 + sinf(a) * 10;
      if (i == 7) geiss(g, x, y, 0.85f, sinf(a) > 0 ? -1 : 1, m, u, MUTTER_FELL, 0, true);
      else kind(g, i, x, y, sinf(a) > 0 ? -1 : 1, u, -1, 0.8f, 0.6f);
    }
  }
  blase(g, 110, 42, {"Was rumpelt und pumpelt", "in meinem Bauch herum?", "Ich meinte, es wären sechs Geißlein,", "so sind's lauter Wackerstein!"}, aufzu(u, 6, 16.4f), 140, 120);
  blase(g, 142, 50, {"Der Wolf ist tot!", "Der Wolf ist tot!"}, aufzu(u, 27.6f, 36), 196, 140);
  g.kameraAus();
  if (u < 1.2f) g.iris(150, 150, lerp(0, 330, sanft(u / 1.2f)));
  if (u > 56) g.iris(196, 170, lerp(330, 0, sanft(seg(u, 56, 58.5f))));
  if (u > 58.5f) {
    uint8_t a = (uint8_t)(255 * sanft(seg(u, 58.5f, 59.5f)));
    g.text(SCHRIFT_G, "Ende", 142, 122, GOLD, 1, a);
    for (int i = 0; i < 6; i++) {
      float an = u * 1.2f + i * 1.047f;
      g.stern(142 + cosf(an) * 60, 112 + sinf(an) * 30, 5, 2, 5, an, GOLD, a);
    }
  }
}

static void bild(Gfx &g, float t) {
  if (t < G2) szeneAbschied(g, t - G1);
  else if (t < G3) szeneWolf1(g, t - G2);
  else if (t < G4) szeneKreide(g, t - G3);
  else if (t < G5) szeneMehl(g, t - G4);
  else if (t < G6) szeneFressen(g, t - G5);
  else if (t < G7) szeneHeim(g, t - G6);
  else if (t < G8) szeneBauch(g, t - G7);
  else szeneBrunnen(g, t - G8);
}

// =====================================================================
// Musik
// =====================================================================
// Geisslein-Lied (C-Dur, huepfend)
static const Ton3 MEL_GEISS[] = {
  {72, .5, 48}, {72, .5, 0}, {79, .5, 0}, {79, .5, 0}, {81, .5, 53}, {81, .5, 0}, {79, 1, 0},
  {77, .5, 53}, {77, .5, 0}, {76, .5, 0}, {76, .5, 0}, {74, .5, 55}, {74, .5, 0}, {72, 1, 48}};
// der Wolf schleicht (d-Moll, tief)
static const Ton3 MEL_WOLF[] = {
  {50, 1, 38}, {0, .5, 0}, {53, .5, 0}, {57, 1, 0}, {56, 1, 0}, {55, 1, 36}, {0, .5, 0}, {52, .5, 0}, {50, 2, 0},
  {50, .5, 38}, {53, .5, 0}, {57, .5, 0}, {62, .5, 0}, {61, 1, 0}, {57, 1, 0}, {55, 1, 36}, {52, 1, 0}, {50, 2, 38}};

static void klopf(Synth &s, float x) { for (int i = 0; i < 3; i++) s.ton(x + i * 0.3f, 160, 0.35f, 0.002f, 0.12f, SINUS, GERAEUSCH, 90); }
static void meck(Synth &s, float x, float h = 1) {
  for (int k = 0; k < 4; k++) s.ton(x + k * 0.07f, 700 * h, 0.06f, 0.01f, 0.06f, DREIECK, GERAEUSCH, 650 * h);
}
static void rauh(Synth &s, float x, float d) {  // raue Wolfsstimme
  for (float k = 0; k < d; k += 0.4f) s.ton(x + k, 95, 0.1f, 0.05f, 0.3f, SAEGE, GERAEUSCH, 80);
}

static void ton(Synth &s, float t0, float t1) {
  float a = G1;
  spieleMelodie(s, M(MEL_GEISS), a + 0.5f, a + 29.5f, 0.28f, t0, t1, 0.85f);
  if (bei(a + 14.6f, t0, t1)) for (int i = 0; i < 4; i++) meck(s, a + 14.6f + i * 0.4f, 1.2f + i * 0.05f);
  a = G2;
  spieleMelodie(s, M(MEL_WOLF), a + 0.5f, a + 4.5f, 0.3f, t0, t1, 0.9f);
  if (bei(a + 4.2f, t0, t1)) klopf(s, a + 4.2f);
  if (bei(a + 5.6f, t0, t1)) rauh(s, a + 5.6f, 4);
  if (bei(a + 10.6f, t0, t1)) for (int i = 0; i < 6; i++) meck(s, a + 10.6f + i * 0.5f, 1.2f);
  spieleMelodie(s, M(MEL_WOLF), a + 16.5f, a + 21.5f, 0.24f, t0, t1, 0.8f);
  a = G3;
  spieleMelodie(s, M(MEL_WOLF), a + 0.5f, a + 6, 0.3f, t0, t1, 0.7f);
  for (float x = ceilf(t0 / 0.2f) * 0.2f; x < t1; x += 0.2f)
    if (x > a + 6.4f && x < a + 8) s.rauschen(x, 0.04f, HOCHPASS, 2500, 0.08f);  // knirsch
  if (bei(a + 9.4f, t0, t1)) klopf(s, a + 9.4f);
  if (bei(a + 9.8f, t0, t1)) for (int k = 0; k < 5; k++) s.note(a + 9.8f + k * 0.3f, 76 + (k & 1) * 3, 0.5f);  // feine Stimme
  if (bei(a + 14, t0, t1)) for (int i = 0; i < 6; i++) meck(s, a + 14 + i * 0.5f, 1.2f);
  a = G4;
  spieleMelodie(s, M(MEL_WOLF), a + 0.4f, a + 25.5f, 0.28f, t0, t1, 0.7f);
  if (bei(a + 13.6f, t0, t1)) rauh(s, a + 13.6f, 1.6f);
  for (float x = ceilf(t0 / 0.15f) * 0.15f; x < t1; x += 0.15f)
    if (x > a + 15 && x < a + 18) s.ton(x, 3400 + fmodf(x * 37, 3) * 300, 0.02f, 0.002f, 0.04f);  // Mehl rieselt
  a = G5;
  if (bei(a + 0.6f, t0, t1)) klopf(s, a + 0.6f);
  if (bei(a + 1, t0, t1)) for (int k = 0; k < 5; k++) s.note(a + 1 + k * 0.3f, 76 + (k & 1) * 3, 0.5f);
  if (bei(a + 9.4f, t0, t1)) for (int k = 0; k < 4; k++) s.note(a + 9.4f + k * 0.1f, 72 + k * 4, 0.6f);  // Tuer auf
  if (bei(a + 10.4f, t0, t1)) for (int k = 0; k < 8; k++) s.note(a + 10.4f + k * 0.06f, 50 + k * 3, 0.8f);  // da ist der Wolf!
  spieleMelodie(s, M(MEL_JAGD), a + 13, a + 17, 0.15f, t0, t1, 0.9f);
  for (int i = 0; i < 7; i++) if (bei(a + 13.2f + i * 0.2f, t0, t1)) meck(s, a + 13.2f + i * 0.2f, 1.3f);
  spieleMelodie(s, M(MEL_WOLF), a + 17, a + 48, 0.26f, t0, t1, 0.8f);
  const float fund[6] = {7, 11, 15, 19, 23, 27};
  for (float f : fund) {
    float x = a + 13 + f;
    if (bei(x, t0, t1)) { meck(s, x, 1.4f); s.ton(x + 0.9f, 120, 0.3f, 0.01f, 0.25f, SINUS, GERAEUSCH, 60); s.note(x + 0.95f, 45, 0.8f); }  // Happs
  }
  a = G6;
  spieleMelodie(s, M(MEL_GEISS), a + 0.4f, a + 4.5f, 0.3f, t0, t1, 0.6f);
  if (bei(a + 4.6f, t0, t1)) for (int k = 0; k < 6; k++) s.note(a + 4.6f + k * 0.2f, 64 - k, 0.6f);
  for (float x : {a + 5.2f, a + 6.4f, a + 7.6f}) if (bei(x, t0, t1)) meck(s, x, 0.9f);
  if (bei(a + 9.2f, t0, t1)) meck(s, a + 9.2f, 1.5f);
  spieleMelodie(s, M(MEL_WIEGE), a + 17, a + 33, 0.42f, t0, t1, 0.7f);
  a = G7;
  for (float x = ceilf(t0 / 2.4f) * 2.4f; x < t1; x += 2.4f)
    if (x > a && x < a + 40) s.ton(x, 75, 0.14f, 0.6f, 0.8f, DREIECK, GERAEUSCH, 95);  // Schnarchen
  spieleMelodie(s, M(MEL_N), a + 0.5f, a + 17, 0.42f, t0, t1, 0.7f);
  for (int i = 0; i < 3; i++) if (bei(a + 17.4f + i * 0.4f, t0, t1)) { s.ton(a + 17.4f + i * 0.4f, 3200, 0.08f, 0.002f, 0.05f); }  // schnipp
  for (int i = 0; i < 6; i++) if (bei(a + 18 + i * 0.6f, t0, t1)) { meck(s, a + 18 + i * 0.6f, 1.3f); s.note(a + 18 + i * 0.6f, 72 + i * 2, 0.6f); }
  spieleMelodie(s, M(MEL_GEISS), a + 22, a + 30, 0.24f, t0, t1, 0.9f);
  for (float x = ceilf(t0 / 0.8f) * 0.8f; x < t1; x += 0.8f)
    if (x > a + 30 && x < a + 40) s.ton(x, 140, 0.15f, 0.002f, 0.12f, SINUS, GERAEUSCH, 80);  // Steine plumpsen
  for (float x = ceilf(t0 / 0.6f) * 0.6f; x < t1; x += 0.6f)
    if (x > a + 40 && x < a + 46) s.ton(x, 2400, 0.04f, 0.002f, 0.1f);  // naehen
  a = G8;
  for (float x = ceilf(t0 / 0.35f) * 0.35f; x < t1; x += 0.35f)
    if (x > a + 2 && x < a + 18) { s.ton(x, 150 + fmodf(x * 17, 3) * 40, 0.12f, 0.002f, 0.08f, SINUS, GERAEUSCH, 90); }  // Steine rumpeln
  spieleMelodie(s, M(MEL_WOLF), a + 6, a + 18, 0.3f, t0, t1, 0.6f);
  if (bei(a + 21, t0, t1)) for (int k = 0; k < 10; k++) s.note(a + 21 + k * 0.12f, 72 - k * 2, 0.6f);  // faellt
  if (bei(a + 22.6f, t0, t1)) { s.rauschen(a + 22.6f, 0.8f, TIEFPASS, 3000, 0.4f, 0.7f, 0.005f, 300); s.ton(a + 22.6f, 70, 0.4f, 0.002f, 0.5f, SINUS, GERAEUSCH, 40); }  // Platsch
  spieleMelodie(s, M(MEL_GEISS), a + 27, a + 55.5f, 0.24f, t0, t1, 1);
  for (float x = ceilf(t0 / 1.6f) * 1.6f; x < t1; x += 1.6f)
    if (x > a + 27.5f && x < a + 36) meck(s, x, 1.3f);
  if (bei(a + 58.5f, t0, t1)) { s.glocke(a + 58.5f, 72); s.glocke(a + 58.8f, 76); s.glocke(a + 59.1f, 79); s.glocke(a + 59.5f, 84); }
}

// =====================================================================
// Startbild und Vorschau
// =====================================================================
static void titelSzene(Gfx &g, float t) {
  haus(g, t, 1);
  GeissPose m;
  m.zu = fmodf(t, 3.9f) < 0.13f;
  geiss(g, 76, 222, 1, 1, m, t, MUTTER_FELL, 0, true);
  for (int i = 0; i < 7; i++) kind(g, i, 130 + (i % 4) * 24, 224 + (i / 4) * 10 - 8, -1, t, -1, 0.4f, 0, 0.45f);
  WolfPose w;
  w.zu = fmodf(t, 4.2f) < 0.15f;
  g.ausschnitt(0, 0, 284, 240);
  wolf(g, 262, 222, 0.7f, -1, w, t);
  g.ausschnittAus();
}

static void titelbild(Gfx &g, float t) {
  titelSzene(g, t);
  g.rundRechteck(14, 14, 256, 54, 14, FARBE(0x1E1A3A), 160);
  g.text(SCHRIFT_K, "Der Wolf und die", 142, 34, WEISS, 1);
  g.text(SCHRIFT_M, "sieben jungen Geißlein", 142, 58, GOLD, 1);
  float b = 1 + sinf(t * 4) * 0.08f;
  g.kontur(2, OUT);
  g.kreis(142, 92, 14 * b, FARBE(0xE8473D));
  g.konturAus();
  float tri[] = {137, 84, 137, 100, 150, 92};
  g.polygon(tri, 3, WEISS);
}

static void vorschau(Gfx &g, float t) { titelSzene(g, t); }

const Maerchen GEISSLEIN = {"Der Wolf und die sieben jungen Geißlein", GDAUER, FARBE(0x8E8676), bild, ton, titelbild, vorschau};

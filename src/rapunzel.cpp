// Rapunzel - ein Spieluhr-Film fuer die Maerchenbox, nach dem Maerchen der Brueder Grimm.
// Die Rapunzeln im Garten der Zauberin, der Turm ohne Tuer, "Rapunzel, Rapunzel, lass dir dein Haar herunter!",
// der Koenigssohn, die abgeschnittenen Zoepfe, die Dornen, die Wuestenei und die zwei Traenen.
#include "maerchen.h"
#include "schriften.h"
#include "figuren.h"
#include "melodien.h"
#include <stdio.h>
#include <initializer_list>

// ---------- Ablauf (Sekunden) ----------
static const float P1 = 0, P2 = 40, P3 = 62, P4 = 96, P5 = 140, P6 = 176, P7 = 216, P8 = 252, PDAUER = 300;

// Turm: Fenster oben, von dort haengt der Zopf
static const float TURM_X = 196, FENSTER_Y = 78, BODEN = 212;

// ---------- Aussehen ----------
static FrauLook rapunzelLook() {
  FrauLook l; l.kleid = FARBE(0xB9A0E8); l.haar = FARBE(0xF2C94E); l.frisur = 1; l.blume = FARBE(0xFF7FA8);
  return l;
}
static FrauLook rapunzelKurz() {
  FrauLook l = rapunzelLook(); l.frisur = 4; l.blume = 0; l.kleid = FARBE(0x9E8EB8); l.flicken = true;
  return l;
}
static FrauLook rapunzelKind() { FrauLook l = rapunzelLook(); l.frisur = 0; return l; }
static FrauLook gothelLook() {
  FrauLook l; l.kleid = FARBE(0x3E4A3A); l.haar = FARBE(0x8E8E8E); l.frisur = 3; l.tuch = FARBE(0x4A2E5A);
  l.haut = FARBE(0xE2DCC0); l.wangen = false; l.schuerze = FARBE(0x2E2A3E);
  return l;
}
static FrauLook frauLook() {
  FrauLook l; l.kleid = FARBE(0xD9A060); l.haar = FARBE(0x7A4524); l.frisur = 3; l.tuch = FARBE(0xE8D6B0);
  l.schuerze = FARBE(0xF4EEDC);
  return l;
}
static FrauLook tochterLook() { FrauLook l; l.kleid = FARBE(0xF28AB8); l.haar = FARBE(0xF2C94E); l.frisur = 1; return l; }
static FrauLook brautLook() { FrauLook l = rapunzelLook(); l.kleid = WEISS; l.glanz = true; l.krone = true; l.rockBreite = 1.4f; l.frisur = 4; return l; }
static const FrauLook RAPUNZEL = rapunzelLook(), RAPUNZEL_KURZ = rapunzelKurz(), RAPUNZEL_KIND = rapunzelKind(),
                      GOTHEL = gothelLook(), FRAU = frauLook(), TOCHTER = tochterLook(), BRAUT = brautLook();

static Aussehen mannLook() {
  Aussehen a; a.weste = FARBE(0x6E8E4E); a.hose = FARBE(0x5A4636); a.haar = FARBE(0x5A3A26); a.muetze = 1;
  return a;
}
static Aussehen prinzLook() {
  Aussehen a; a.weste = FARBE(0xC0392B); a.hemd = FARBE(0xFFF6E0); a.hose = FARBE(0x2E3A6E); a.haar = FARBE(0x5A3A26);
  a.muetze = 3; a.halstuch = GOLD;
  return a;
}
static Aussehen sohnLook() { Aussehen a; a.weste = FARBE(0x3E7FDC); a.haar = FARBE(0xF2C94E); a.muetze = 1; return a; }
static Aussehen armerPrinz() { Aussehen a = prinzLook(); a.weste = FARBE(0x8E5A4A); a.hemd = FARBE(0xC9B8A0); a.muetze = 1; return a; }
static const Aussehen MANN = mannLook(), PRINZ = prinzLook(), SOHN = sohnLook(), ARMER_PRINZ = armerPrinz();

static FrauPose fp(const FrauLook &l, float t) {
  FrauPose p;
  p.look = &l;
  p.zeit = t;
  p.zu = fmodf(t + l.kleid * 0.0007f, 3.7f) < 0.13f;
  return p;
}
static Pose mp(const Aussehen &a, float t) {
  Pose p;
  p.aussehen = &a;
  p.zu = fmodf(t + 0.4f, 4.1f) < 0.13f;
  return p;
}

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
  if (y - bh / 2 < 3) y = 3 + bh / 2;
  inKreis(x, y, bw, bh);  // rundes Board: ganz in den sichtbaren Kreis
  float sp[] = {x - 6, y + bh / 2 - 2, x + 6, y + bh / 2 - 2, sx, sy};
  g.kontur(1.4f, OUT);
  g.rundRechteck(x - bw / 2, y - bh / 2, bw, bh, 9 * k, f);
  g.konturAus();
  g.polygon(sp, 3, f);
  if (k > 0.85f) {
    blaseLesbar = 1;
    int i = 0;
    for (const char *z : zeilen) g.text(SCHRIFT_K, z, x, y - bh / 2 + 18 + i++ * 18, OUT, 1);
  }
}

static void glitzern(Gfx &g, float x, float y, float r, float t, int n, uint8_t a = 255) {
  for (int i = 0; i < n; i++) {
    float an = t * 2 + i * 6.2831853f / n;
    float rr = r * (0.6f + 0.4f * sinf(t * 3 + i));
    g.stern(x + cosf(an) * rr, y + sinf(an) * rr * 0.8f, 4, 1.6f, 4, an, i & 1 ? GOLD : WEISS, a);
  }
}

static void singen(Gfx &g, float x, float y, float t) {
  for (int i = 0; i < 3; i++) {
    float ph = fmodf(t * 0.45f + i / 3.0f, 1);
    notenzeichen(g, x + ph * 26 + sinf(ph * 8) * 4, y - ph * 46, i & 1 ? FARBE(0xB28DFF) : FARBE(0xE8473D), (uint8_t)(230 * huepf(ph)));
  }
}

// =====================================================================
// Kulissen und Dinge
// =====================================================================
// Rapunzel-Pflanzen (Glockenblumen)
static void rapunzeln(Gfx &g, float x, float y, float s) {
  for (int i = 0; i < 4; i++) {
    float a = -0.9f + i * 0.6f;
    g.ellipse(x + sinf(a) * 8 * s, y - 4 * s, 3 * s, 8 * s, FARBE(0x4CA85A), 255, a);
  }
  g.linie(x, y - 4 * s, x + 2 * s, y - 20 * s, 1.4f * s, FARBE(0x3E8A4B));
  for (int i = 0; i < 3; i++) g.ellipse(x + 2 * s + (i - 1) * 4 * s, y - 18 * s - i * 3 * s, 2.4f * s, 3.4f * s, FARBE(0x7E8AE8));
}

// Garten der Zauberin hinter der hohen Mauer (vom Fenster des Hauses aus gesehen)
static void garten(Gfx &g, float t, float abend, float mauerAus = 0) {
  himmel(g, abend, t, 196);
  if (abend < 0.5f) sonne(g, 244, 34, 12, t);
  else mond(g, 244, 36, 11);
  boden(g, 196, mischeFarbe(FARBE(0x6EBF56), FARBE(0x2E5A4A), abend));
  for (int i = 0; i < 9; i++) rapunzeln(g, 130 + (i % 5) * 30, 214 - (i / 5) * 14, 1.1f);
  for (int i = 0; i < 4; i++) {  // Blumenbeete der Zauberin
    g.kreis(150 + i * 36, 186, 6, mischeFarbe(FARBE(0xE8473D), FARBE(0x5A2A4A), abend));
    g.kreis(156 + i * 36, 180, 5, mischeFarbe(GOLD, FARBE(0x6A5A3A), abend));
  }
  laubbaum(g, 260, 200, 1, mischeFarbe(FARBE(0x4CB85A), FARBE(0x2B5A4A), abend));
  // die hohe Mauer
  if (mauerAus < 1) {
    Farbe m = mischeFarbe(FARBE(0xC9B8A0), FARBE(0x5E5470), abend);
    g.rechteck(104, 120, 14, 100, m);
    for (int i = 0; i < 6; i++) g.rechteck(104, 124 + i * 16, 14, 1.5f, mischeFarbe(m, OUT, 0.3f));
  }
}

// Turm ohne Tuer und Treppe, oben ein kleines Fenster
static void turm(Gfx &g, float x, float t, float nacht) {
  Farbe st = mischeFarbe(FARBE(0xC9BFAE), FARBE(0x6E6680), nacht);
  g.kontur(1.8f, OUT);
  g.rechteck(x - 26, FENSTER_Y - 30, 52, BODEN - FENSTER_Y + 30, st);
  float dach[] = {x - 34, FENSTER_Y - 28, x, FENSTER_Y - 86, x + 34, FENSTER_Y - 28};
  g.polygon(dach, 3, mischeFarbe(FARBE(0x5B7FC7), FARBE(0x3A3466), nacht));
  g.konturAus();
  for (int i = 0; i < 9; i++) {  // Steine
    float y = FENSTER_Y - 18 + i * 15;
    g.rechteck(x - 26, y, 52, 1.2f, mischeFarbe(st, OUT, 0.25f));
    g.rechteck(x - 12 + (i & 1) * 14, y, 1.2f, 15, mischeFarbe(st, OUT, 0.25f));
  }
  for (int i = 0; i < 5; i++) g.kreis(x - 22 + (i & 1) * 8, BODEN - 8 - i * 14, 6, mischeFarbe(FARBE(0x4CA85A), FARBE(0x2B5A4A), nacht));  // Efeu
  g.kontur(1.4f, OUT);
  g.rundRechteck(x - 12, FENSTER_Y - 14, 24, 26, 11, FARBE(0x2A2234));
  g.konturAus();
  g.linie(x + 12, FENSTER_Y + 10, x + 16, FENSTER_Y + 10, 2, FARBE(0x8E94A6));  // Fensterhaken
}

// Rapunzel schaut aus dem Turmfenster
static void imFenster(Gfx &g, const FrauLook &l, float t, float mund = 0) {
  g.ausschnitt(TURM_X - 12, FENSTER_Y - 14, 24, 26);
  FrauPose p = fp(l, t);
  p.mund = mund;
  frau(g, TURM_X, FENSTER_Y + 46, 0.62f, -1, p);
  g.ausschnittAus();
}

// der lange goldene Zopf vom Fensterhaken bis (laenge 0..1) zum Boden
static void zopf(Gfx &g, float laenge, float t, Farbe c = FARBE(0xF2C94E)) {
  if (laenge <= 0) return;
  float x0 = TURM_X + 14, y0 = FENSTER_Y + 10;
  float y1 = lerp(y0, BODEN - 4, laenge), x1 = x0 + 10 + sinf(t * 1.5f) * 3 * laenge;
  g.kontur(1.4f, OUT);
  g.kurve(x0, y0, x0 + 16, (y0 + y1) / 2, x1, y1, 8, c);
  g.konturAus();
  int n = (int)(laenge * 14);
  for (int i = 0; i < n; i++) {  // geflochten
    float u = (i + 0.5f) / 14 / laenge;
    if (u > 1) break;
    float v = 1 - u;
    float x = v * v * x0 + 2 * v * u * (x0 + 16) + u * u * x1, y = v * v * y0 + 2 * v * u * (y0 + y1) / 2 + u * u * y1;
    g.linie(x - 3, y - 2, x + 3, y + 2, 1.2f, FARBE(0xC99A2A));
  }
  g.kreis(x1, y1 + 3, 4, FARBE(0xE8473D));  // Schleife
}

// Position am Zopf (0 = unten, 1 = am Fenster) fuer eine kletternde Figur
static void amZopf(float k, float &x, float &y) {
  float x0 = TURM_X + 14, y0 = FENSTER_Y + 10, x1 = x0 + 10, y1 = BODEN - 4;
  float u = 1 - k, v = 1 - u;
  x = v * v * x0 + 2 * v * u * (x0 + 16) + u * u * x1 - 4;
  y = v * v * y0 + 2 * v * u * (y0 + y1) / 2 + u * u * y1 + 76;  // die Haende halten den Zopf
  if (y > 214) y = 214;
}

static void wald(Gfx &g, float t, float nacht, float herbst = 0) {
  himmel(g, nacht, t, 200);
  if (nacht > 0.5f) mond(g, 40, 36, 11);
  else sonne(g, 40, 36, 12, t);
  Farbe tan = mischeFarbe(mischeFarbe(FARBE(0x2F8F4E), FARBE(0xC0802E), herbst), FARBE(0x1F3B45), nacht);
  for (int i = 0; i < 5; i++) tanne(g, i * 64 + 12, 200, 0.9f + (i & 1) * 0.2f, tan);
  boden(g, 200, mischeFarbe(mischeFarbe(GRAS, FARBE(0xB89A5A), herbst), FARBE(0x2F4A40), nacht));
}

static void dornen(Gfx &g, float x, float y, float s) {
  g.kontur(1.4f, OUT);
  g.ellipse(x, y - 14 * s, 40 * s, 18 * s, FARBE(0x3E5A30));
  g.konturAus();
  for (int i = 0; i < 16; i++) {
    float a = 3.3f + i * 0.18f;
    float bx = x + cosf(a) * 40 * s, by = y - 14 * s + sinf(a) * 18 * s;
    g.linie(bx, by, bx + cosf(a) * 6 * s, by + sinf(a) * 6 * s, 1.4f, FARBE(0x5A3A26));
  }
  for (int i = 0; i < 5; i++) g.kreis(x - 24 * s + i * 12 * s, y - 20 * s - (i & 1) * 6 * s, 2.5f * s, FARBE(0xC0392B));  // Beeren
}

static void wueste(Gfx &g, float t) {
  g.verlauf(0, 180, FARBE(0xF2C98A), FARBE(0xFBE6C0));
  sonne(g, 240, 40, 14, t);
  g.ellipse(60, 200, 160, 40, FARBE(0xE0B678));
  g.ellipse(230, 206, 150, 40, FARBE(0xD9A868));
  g.rechteck(0, 200, 284, 40, FARBE(0xE6C08A));
  for (int i = 0; i < 4; i++) {  // duerre Buesche
    float x = 30 + i * 70;
    for (int k = 0; k < 4; k++) g.linie(x, 210, x + (k - 1.5f) * 6, 196 - k * 2, 1.4f, FARBE(0x8E6E4E));
  }
  // armselige Huette aus Schilf
  g.kontur(1.4f, OUT);
  float h[] = {20, 210, 20, 168, 50, 148, 80, 168, 80, 210};
  g.polygon(h, 5, FARBE(0xC9A46A));
  g.konturAus();
  for (int i = 0; i < 6; i++) g.linie(24 + i * 10, 170, 24 + i * 10, 208, 1.2f, FARBE(0xA8823E));
  g.rundRechteck(40, 184, 18, 26, 6, FARBE(0x5A3A26));
}

static void stube(Gfx &g, float t, float nacht) {
  g.verlauf(0, 196, mischeFarbe(FARBE(0xE8D2B0), FARBE(0x6E5E7E), nacht), mischeFarbe(FARBE(0xD6B88E), FARBE(0x4E4060), nacht));
  g.rechteck(0, 196, 284, 44, FARBE(0x9E7656));
  // Fenster nach hinten (zum Garten der Zauberin)
  g.kontur(1.8f, OUT);
  g.rundRechteck(150, 40, 100, 80, 6, HOLZ);
  g.konturAus();
  g.ausschnitt(156, 46, 88, 68);
  garten(g, t, nacht);
  g.ausschnittAus();
  g.rechteck(198, 46, 4, 68, HOLZ);
  g.rechteck(146, 118, 108, 6, HOLZ_D);
}

static void turmstube(Gfx &g, float t, float nacht) {
  g.verlauf(0, 200, mischeFarbe(FARBE(0xD9CCB8), FARBE(0x5E5670), nacht), mischeFarbe(FARBE(0xC0B19A), FARBE(0x3E3650), nacht));
  for (int r = 0; r < 6; r++) for (int c = 0; c < 6; c++) {
      Farbe m = mischeFarbe(mischeFarbe(FARBE(0xD9CCB8), FARBE(0x5E5670), nacht), OUT, 0.08f);
      g.rundRechteck(c * 52 - (r & 1) * 26 + 2, r * 32 + 2, 48, 28, 3, m);
    }
  g.rechteck(0, 200, 284, 40, FARBE(0x8E7656));
  // das Fenster mit dem Haken
  g.kontur(1.8f, OUT);
  g.rundRechteck(180, 40, 70, 90, 34, FARBE(0x8E8676));
  g.konturAus();
  g.rundRechteck(188, 48, 54, 78, 26, mischeFarbe(FARBE(0x9FD8FF), FARBE(0x22285A), nacht));
  if (nacht > 0.5f) mond(g, 226, 74, 8);
  g.linie(242, 118, 252, 118, 3, FARBE(0x8E94A6));
  // Spinnrad / Seidenstraenge in der Ecke
  g.rundRechteck(20, 168, 60, 32, 4, HOLZ);
  for (int i = 0; i < 4; i++) g.linie(26 + i * 12, 168, 30 + i * 12, 150, 2, FARBE(0xE8D8F0));
}

static void wiege(Gfx &g, float x, float y, float t, bool kind = true) {
  float sch = sinf(t * 2) * 0.12f;
  g.kontur(1.4f, OUT);
  float w[] = {x - 22, y - 20, x + 22, y - 20 + sch * 20, x + 16, y - 2 + sch * 10, x - 16, y - 2 - sch * 10};
  g.polygon(w, 4, HOLZ);
  g.konturAus();
  g.bogen(x, y + 2, 22, 3.6f, 5.8f, 3, HOLZ_D);
  if (!kind) return;
  g.kreis(x - 6, y - 24, 7, HAUT);
  g.ellipse(x + 4, y - 22, 12, 6, FARBE(0xF6B8D2));
  g.bogen(x - 7, y - 25, 2, 3.5f, 5.9f, 1, OUT);
}

static void schere(Gfx &g, float x, float y, float s, float auf) {
  g.kreis(x - 4 * s, y + 4 * s, 3 * s, FARBE(0x5E6474));
  g.kreis(x + 4 * s, y + 4 * s, 3 * s, FARBE(0x5E6474));
  g.linie(x - 2 * s, y + 2 * s, x + (4 + auf * 4) * s, y - 12 * s, 2 * s, FARBE(0xC7D0DC));
  g.linie(x + 2 * s, y + 2 * s, x - (4 + auf * 4) * s + 8 * s, y - 12 * s, 2 * s, FARBE(0xC7D0DC));
}

// =====================================================================
// Szenen
// =====================================================================
// 1) Die Frau will Rapunzeln; der Mann steigt ueber die Mauer; die Zauberin erwischt ihn
static void szeneGarten(Gfx &g, float u) {
  if (u < 13) {
    stube(g, u, seg(u, 9, 12));
    FrauPose f = fp(FRAU, u);
    f.armV = 1.8f;
    f.mund = u > 3 && u < 8 ? 0.5f : 0;
    f.weint = u > 4;
    frau(g, 120, 214, 0.95f, 1, f);
    Pose m = mp(MANN, u);
    m.armV = 1.2f;
    m.boese = u > 8 ? 0.3f : 0;
    schneiderlein(g, 60, 214, 0.95f, 1, m);
    blase(g, 110, 46, {"Ach, wenn ich keine Rapunzeln", "aus dem Garten hinter unserm Haus", "zu essen kriege, so sterbe ich!"}, aufzu(u, 2.6f, 8.6f), 120, 120);
    blase(g, 70, 60, {"Eh du sterben lässt,", "holst du dir Rapunzeln!"}, aufzu(u, 8.8f, 12.6f), 62, 120);
  } else {
    // in der Daemmerung: ueber die Mauer, Rapunzeln holen - beim zweiten Mal steht die Zauberin da
    float abend = 0.8f;
    garten(g, u, abend);
    Pose m = mp(MANN, u);
    float mx = 80, my = 214;
    if (u < 17) { float k = seg(u, 13, 16); mx = lerp(60, 140, k); my = 214 - sinf(k * 3.14159f) * 100; }
    else { mx = 150; m.armV = u < 20 ? 0.2f + fabsf(sinf(u * 6)) * 0.8f : 1.4f; }  // rupft
    if (u > 20.5f) { m.mund = 0.8f; m.armV = 2.6f; m.armH = 2.6f; }  // erschrickt
    if (u > 27) { m.armV = 2.0f; m.armH = 2.0f; m.mund = 0.4f; }   // fleht
    float hx, hy;
    schneiderlein(g, mx, my, 0.95f, 1, m, &hx, &hy);
    if (u > 18 && u < 21) rapunzeln(g, hx, hy + 8, 0.8f);
    if (u > 20) {
      FrauPose z = fp(GOTHEL, u);
      z.boese = 1;
      z.armV = u < 27 ? 2.4f : 1.4f;
      z.mund = (u > 21 && u < 26) || (u > 31 && u < 39) ? 0.6f : 0;
      float zx = lerp(320, 210, sanft(seg(u, 20, 21.5f)));
      frau(g, zx, 214, 1, -1, z);
      blase(g, 190, 40, {"Wie kannst du es wagen, in meinen", "Garten zu steigen und mir meine", "Rapunzeln zu stehlen?"}, aufzu(u, 21.6f, 26.8f), 214, 120);
      blase(g, 100, 50, {"Meine Frau hat Eure Rapunzeln", "erblickt, sie stirbt sonst!"}, aufzu(u, 27, 31), 150, 130);
      blase(g, 160, 40, {"Nimm so viel Rapunzeln du willst,", "aber du musst mir das Kind geben,", "das deine Frau zur Welt bringen wird."}, aufzu(u, 31.2f, 38.6f), 214, 120);
    }
  }
  blende(g, u, 40, 140, 150);
}

// 2) Das Kind wird geboren, die Zauberin nennt es Rapunzel und nimmt es mit; mit zwoelf Jahren in den Turm
static void szeneKind(Gfx &g, float u) {
  if (u < 11) {
    stube(g, u, 0);
    wiege(g, 110, 214, u, u < 6);
    FrauPose f = fp(FRAU, u);
    f.weint = u > 5;
    frau(g, 60, 214, 0.95f, 1, f);
    FrauPose z = fp(GOTHEL, u);
    z.armV = u > 5 ? 1.6f : 0.6f;
    float zx = lerp(320, 160, sanft(seg(u, 1, 4)));
    if (u > 7) zx = lerp(160, 330, seg(u, 7, 10.5f));
    float hx, hy;
    frau(g, zx, 214, 1, u > 7 ? 1 : -1, z, &hx, &hy);
    if (u > 6) { g.kreis(hx + 2, hy - 2, 6, HAUT); g.ellipse(hx + 8, hy, 9, 5, FARBE(0xF6B8D2)); }
    blase(g, 170, 50, {"Sie soll Rapunzel heißen."}, aufzu(u, 4.6f, 7.6f), 170, 120);
  } else {
    // zwoelf Jahre spaeter: das schoenste Kind unter der Sonne, in den Turm gesperrt
    float v = u - 11;
    wald(g, u, 0);
    turm(g, TURM_X, u, 0);
    FrauPose r = fp(RAPUNZEL_KIND, u), z = fp(GOTHEL, u);
    float k = seg(v, 0, 4);
    float rx = lerp(-30, 140, k), zx = rx - 34;
    r.lauf = z.lauf = k < 1 ? fmodf(u * 1.6f, 1) : -1;
    if (v < 6) {
      frau(g, zx, 214, 1, 1, z);
      frau(g, rx, 214, 0.85f, 1, r);
    } else {
      imFenster(g, RAPUNZEL, u);
      frau(g, 140, 214, 1, 1, z);
      if (v < 7.4f) for (int i = 0; i < 8; i++) {
          float a = i * 0.785f, kk = seg(v, 6, 7.4f);
          g.stern(TURM_X + cosf(a) * kk * 40, FENSTER_Y + sinf(a) * kk * 40, 5, 2, 5, a, GOLD, (uint8_t)(255 * (1 - kk)));
        }
    }
    g.kameraAus();
    g.text(SCHRIFT_K, "zwölf Jahre später", 10, 20, OUT);
  }
  blende(g, u, 22, 140, 150);
}

// 3) "Rapunzel, Rapunzel, lass dir dein Haar herunter!" - die Zauberin steigt hinauf
static void szeneHaar(Gfx &g, float u) {
  wald(g, u, 0);
  turm(g, TURM_X, u, 0);
  bool oben = u > 20;
  imFenster(g, RAPUNZEL, u, u < 4 ? 0.4f : 0);
  if (u < 4) singen(g, TURM_X + 14, FENSTER_Y - 10, u);
  float laenge = sanft(seg(u, 9, 12)) * (1 - seg(u, 26, 29));
  zopf(g, laenge, u);
  FrauPose z = fp(GOTHEL, u);
  z.mund = u > 4 && u < 9 ? 0.6f : 0;
  z.armV = u > 4 && u < 9 ? 2.4f : 0.6f;
  if (u < 13) frau(g, lerp(-30, 186, sanft(seg(u, 0, 3.5f))), 214, 1, 1, z);
  else if (!oben) {
    float k = seg(u, 13, 20);
    float x, y;
    amZopf(k, x, y);
    z.armV = 2.9f; z.armH = 2.9f;
    frau(g, x - 6, y, 0.9f, 1, z);
  }
  blase(g, 110, 40, {"Rapunzel, Rapunzel,", "lass mir dein Haar herunter!"}, aufzu(u, 4.2f, 9), 160, 140);
  g.kameraAus();
  g.text(SCHRIFT_K, "Die Zöpfe waren zwanzig Ellen lang.", 142, 230, OUT, 1, (uint8_t)(255 * aufzu(u, 12, 18)));
  blende(g, u, 34, 160, 120);
}

// 4) Der Koenigssohn hoert den Gesang, ruft den Spruch, steigt hinauf; Rapunzel sagt Ja
static void szenePrinz(Gfx &g, float u) {
  if (u < 26) {
    float nacht = seg(u, 10, 13) * 0.6f;
    wald(g, u, nacht);
    turm(g, TURM_X, u, nacht);
    imFenster(g, RAPUNZEL, u, u < 9 ? 0.4f : 0);
    if (u < 9) singen(g, TURM_X + 14, FENSTER_Y - 10, u);
    // er reitet heran und haelt still
    float px = lerp(-80, 90, sanft(seg(u, 0, 5)));
    Pose p = mp(PRINZ, u);
    p.armV = 1.4f;
    p.mund = u > 5 && u < 9 ? 0.4f : 0;
    if (u < 10) {
      schneiderlein(g, px + 6, 214 - 44, 0.72f, 1, p);
      einhorn(g, px, 214, 0.95f, u < 5 ? fmodf(u * 1.6f, 1) : -1, 1, 0, 0, true);
    } else {
      // am Abend: er ruft und klettert hinauf
      float laenge = sanft(seg(u, 16, 18));
      zopf(g, laenge, u);
      p.mund = u > 12 && u < 16 ? 0.6f : 0;
      if (u < 19) { p.armV = u > 12 && u < 16 ? 2.4f : 0.6f; schneiderlein(g, 180, 214, 0.95f, 1, p); }
      else {
        float x, y;
        amZopf(seg(u, 19, 25), x, y);
        p.armV = 2.9f; p.armH = 2.9f;
        schneiderlein(g, x - 6, y, 0.85f, 1, p);
      }
      einhorn(g, 60, 214, 0.9f, -1, 1, 0, 0, true);
    }
    blase(g, 90, 60, {"Was für eine liebliche Stimme!"}, aufzu(u, 5, 9), 96, 150);
    blase(g, 110, 40, {"Rapunzel, Rapunzel,", "lass mir dein Haar herunter!"}, aufzu(u, 12, 16.2f), 160, 140);
  } else {
    // in der Turmstube
    float v = u - 26;
    turmstube(g, u, 0.7f);
    FrauPose r = fp(RAPUNZEL, u);
    r.mund = (v > 0.4f && v < 2.6f) || (v > 8.6f && v < 12) ? 0.6f : 0;
    r.armV = v < 3 ? 2.6f : 1.4f;
    frau(g, 90, 214, 0.95f, 1, r);
    Pose p = mp(PRINZ, u);
    p.mund = v > 3 && v < 8.4f ? 0.5f : 0.3f;
    p.armV = v > 3 && v < 8.4f ? 1.6f : 0.8f;
    schneiderlein(g, 190, 214, 1, -1, p);
    if (v > 12) for (int i = 0; i < 5; i++) {
        float k = fmodf((v - 12) * 0.5f + i / 5.0f, 1);
        herz(g, 140 + sinf(k * 7 + i) * 24, 150 - k * 100, 5 + (i % 3), FARBE(0xFF5A7A), (uint8_t)(255 * huepf(k)));
      }
    blase(g, 100, 50, {"Oh!"}, aufzu(v, 0.4f, 2.6f), 92, 120);
    blase(g, 180, 40, {"Fürchte dich nicht!", "Willst du mich zum Mann nehmen?"}, aufzu(v, 3, 8.4f), 196, 110);
    blase(g, 110, 40, {"Ja, ich will gern mit dir gehen.", "Bring jedes Mal einen Strang Seide mit,", "daraus flecht ich eine Leiter."}, aufzu(v, 8.6f, 15.6f), 96, 120);
  }
  blende(g, u, 44, 150, 150);
}

// 5) Rapunzel verraet sich; "Ach du gottloses Kind!" - die Zoepfe werden abgeschnitten, in die Wuestenei
static void szeneVerrat(Gfx &g, float u) {
  if (u < 22) {
    turmstube(g, u, 0);
    bool kurz = u > 12.6f;
    FrauPose r = fp(kurz ? RAPUNZEL_KURZ : RAPUNZEL, u);
    r.mund = u > 1 && u < 7 ? 0.5f : u > 12 ? 0.6f : 0;
    r.weint = u > 13;
    frau(g, 100, 214, 0.95f, 1, r);
    FrauPose z = fp(GOTHEL, u);
    z.boese = u > 7 ? 1 : 0.2f;
    z.mund = u > 7 && u < 11 ? 0.7f : 0;
    z.armV = u > 10 && u < 13 ? 1.8f + fabsf(sinf(u * 10)) * 0.4f : 0.7f;
    float zx = 170, zhx, zhy;
    if (u > 15) zx = lerp(170, 200, seg(u, 15, 17));
    frau(g, zx, 214, 1, -1, z, &zhx, &zhy);
    if (u > 10 && u < 13.4f) schere(g, zhx - 4, zhy, 1.2f, fabsf(sinf(u * 10)));
    // die abgeschnittenen Zoepfe liegen auf der Erde
    if (u > 12.6f) {
      g.kontur(1.2f, OUT);
      g.kurve(60, 214, 90, 206, 130, 214, 8, FARBE(0xF2C94E));
      g.konturAus();
    }
    blase(g, 120, 40, {"Frau Gothel, wie kommt es nur?", "Ihr werdet mir viel schwerer", "heraufzuziehen als der", "junge Königssohn!"}, aufzu(u, 1, 7), 104, 120);
    blase(g, 180, 40, {"Ach du gottloses Kind!", "Ich dachte, ich hätte dich von", "aller Welt geschieden!"}, aufzu(u, 7.2f, 11), 176, 120);
    g.kameraAus();
    if (u > 11 && u < 13) g.text(SCHRIFT_G, "ritsch, ratsch!", 142, 60, OUT, 1, (uint8_t)(255 * aufzu(u, 11, 13)));
    if (u > 20) g.rechteck(0, 0, 284, 240, 0, (uint8_t)(255 * seg(u, 20, 22)));
  } else {
    // in der Wuestenei zurueckgelassen
    float v = u - 22;
    wueste(g, u);
    FrauPose r = fp(RAPUNZEL_KURZ, u);
    r.weint = true;
    r.armV = 2.4f;
    frau(g, 140, 214, 0.95f, 1, r);
    FrauPose z = fp(GOTHEL, u);
    z.boese = 1;
    float zx = lerp(176, 330, seg(v, 2, 7));
    z.lauf = v > 2 ? fmodf(u * 1.6f, 1) : -1;
    frau(g, zx, 214, 1, 1, z);
    g.kameraAus();
    if (v < 1.2f) g.iris(140, 150, lerp(0, 330, sanft(v / 1.2f)));
    if (v > 12.8f) g.iris(140, 150, lerp(330, 0, sanft(seg(v, 12.8f, 14))));
    return;
  }
  if (u < 1.2f) g.iris(140, 150, lerp(0, 330, sanft(u / 1.2f)));
}

// 6) Der Koenigssohn steigt hinauf und findet die Zauberin; er springt hinab in die Dornen
static void szeneDornen(Gfx &g, float u) {
  float nacht = 0.7f;
  wald(g, u, nacht);
  dornen(g, 96, 214, 1.1f);
  turm(g, TURM_X, u, nacht);
  // die abgeschnittenen Zoepfe am Fensterhaken
  float laenge = sanft(seg(u, 5, 7)) * (1 - seg(u, 18, 18.6f));
  zopf(g, laenge, u);
  Pose p = mp(PRINZ, u);
  if (u < 7) {
    p.mund = u > 1 && u < 5 ? 0.6f : 0;
    p.armV = u > 1 && u < 5 ? 2.4f : 0.6f;
    schneiderlein(g, 176, 214, 0.95f, 1, p);
  } else if (u < 13) {
    float x, y;
    amZopf(seg(u, 7, 12.6f), x, y);
    p.armV = 2.9f; p.armH = 2.9f;
    schneiderlein(g, x - 6, y, 0.85f, 1, p);
  }
  // die Zauberin oben im Fenster mit boesen Blicken
  if (u > 12) {
    g.ausschnitt(TURM_X - 12, FENSTER_Y - 14, 24, 26);
    FrauPose z = fp(GOTHEL, u);
    z.boese = 1;
    z.mund = u > 13 && u < 19 ? 0.6f : 0;
    frau(g, TURM_X, FENSTER_Y + 46, 0.62f, -1, z);
    g.ausschnittAus();
  }
  // er springt hinab, faellt in die Dornen; die Dornen stechen ihm in die Augen
  if (u > 13 && u < 19) {  // haengt am Fenster
    p.armV = 2.9f; p.armH = 2.9f; p.mund = 0.8f;
    schneiderlein(g, TURM_X + 22, FENSTER_Y + 108, 0.85f, -1, p);
  }
  if (u > 19 && u < 21) {
    float k = seg(u, 19, 21);
    p.armV = 2.9f; p.armH = 2.9f; p.mund = 0.9f;
    schneiderlein(g, lerp(TURM_X + 22, 110, k), lerp(FENSTER_Y + 108, 214, hinein(k)), 0.85f, -1, p);
  }
  if (u > 21) {
    p.zu = true; p.mund = 0.5f; p.armV = 2.6f; p.armH = 2.6f;
    schneiderlein(g, 104, 208, 0.85f, 1, p);
    for (int i = 0; i < 4; i++) {
      float a = u * 4 + i * 1.57f;
      g.stern(110 + cosf(a) * 14, 140 + sinf(a) * 5, 3, 1.3f, 5, a, WEISS);
    }
    dornen(g, 96, 218, 0.9f);  // die Dornen vorn
  }
  if (u > 20.8f && u < 21.3f) g.wackeln(sinf(u * 90) * 4, cosf(u * 80) * 3);
  g.kameraAus();
  blase(g, 110, 40, {"Rapunzel, Rapunzel,", "lass mir dein Haar herunter!"}, aufzu(u, 1, 5), 176, 140);
  blase(g, 110, 40, {"Aha, du willst die Frau Liebste holen,", "aber der schöne Vogel sitzt nicht", "mehr im Nest und singt nicht mehr!",
                     "Rapunzel ist für dich verloren."}, aufzu(u, 13.2f, 19.2f), 190, 80);
  blende(g, u, 40, 120, 160);
}

// 7) Blind irrt er durch den Wald, isst Wurzeln und Beeren und weint - jahrelang
static void szeneBlind(Gfx &g, float u) {
  float herbst = 0.5f + 0.5f * sinf(u * 0.35f);
  float nacht = 0.5f - 0.5f * cosf(u * 0.5f);
  wald(g, u, nacht * 0.7f, herbst);
  if (herbst > 0.6f)
    for (int i = 0; i < 12; i++) {  // fallende Blaetter
      float x = fmodf(zufall(i + 40) * 300 + u * 8, 300) - 10, y = fmodf(zufall(i + 60) * 240 + u * 20, 240);
      g.ellipse(x, y, 3, 2, i & 1 ? FARBE(0xE07A2A) : FARBE(0xC9A43A), 220, u + i);
    }
  dornen(g, 230, 214, 0.8f);
  Pose p = mp(ARMER_PRINZ, u);
  p.zu = true;
  p.lauf = fmodf(u * 1.1f, 1);
  p.armV = 1.7f;  // tastet mit dem Stock
  float px = fmodf(u * 14, 340) - 40;
  float hx, hy;
  schneiderlein(g, px, 214, 0.9f, 1, p, &hx, &hy);
  g.linie(hx, hy, hx + 14, 214, 2.5f, HOLZ);
  if (fmodf(u, 9) > 4 && fmodf(u, 9) < 6) g.kreis(hx + 2, hy - 8, 3, FARBE(0xC0392B));  // isst Beeren
  for (int i = 0; i < 2; i++) {
    float k = fmodf(u * 1.3f + i * 0.5f, 1);
    g.kreis(px + 3 + i * 8, 156 + k * 14, 1.6f, FARBE(0x8FD8FF), (uint8_t)(255 * (1 - k)));  // weint
  }
  g.kameraAus();
  g.text(SCHRIFT_K, "So ging er einige Jahre im Elend umher.", 142, 230, OUT, 1, (uint8_t)(255 * aufzu(u, 4, 14)));
  blende(g, u, 36, 140, 150);
}

// 8) In der Wuestenei: ihre Stimme; zwei Traenen machen ihn sehend; mit den Zwillingen heim in sein Reich
static void szeneWiedersehen(Gfx &g, float u) {
  if (u < 34) {
    wueste(g, u);
    FrauPose r = fp(RAPUNZEL_KURZ, u);
    bool erkannt = u > 9;
    r.mund = u < 8 ? 0.4f : u > 9 && u < 12 ? 0.8f : 0;
    r.weint = u > 12 && u < 22;
    r.armV = u > 11 ? 2.0f : 0.6f;
    float rx = 150;
    if (erkannt) rx = lerp(150, 174, seg(u, 10, 12));
    frau(g, rx, 214, 0.95f, 1, r);
    if (u < 8) singen(g, rx + 10, 120, u);
    // die Zwillinge
    Pose b = mp(SOHN, u);
    FrauPose t = fp(TOCHTER, u + 1);
    b.mund = t.mund = u > 22 ? 0.5f : 0;
    schneiderlein(g, 102, 214, 0.5f, 1, b);
    frau(g, 122, 214, 0.5f, 1, t);
    // der Koenigssohn kommt blind heran
    Pose p = mp(ARMER_PRINZ, u);
    bool sieht = u > 16.4f;
    p.zu = !sieht;
    float px = lerp(330, 214, sanft(seg(u, 0, 9)));
    p.lauf = u < 9 ? fmodf(u * 1.1f, 1) : -1;
    p.armV = u < 12 ? 1.7f : 1.6f;
    p.mund = sieht ? 0.6f : 0;
    float hx, hy;
    schneiderlein(g, px, 214, 0.95f, -1, p, &hx, &hy);
    if (u < 11) g.linie(hx, hy, hx - 14, 214, 2.5f, HOLZ);
    // zwei Traenen fallen in seine Augen
    if (u > 13 && u < 16.4f)
      for (int i = 0; i < 2; i++) {
        float k = seg(u, 13 + i * 0.8f, 15 + i * 0.8f);
        tropfen(g, lerp(rx + 6, px - 2 + i * 8, k), lerp(150, 154, k), 2.2f, FARBE(0x8FD8FF));
      }
    if (u > 16.4f && u < 20) glitzern(g, px, 150, 20, u, 6, (uint8_t)(255 * (1 - seg(u, 18, 20))));
    if (u > 20) for (int i = 0; i < 5; i++) {
        float k = fmodf((u - 20) * 0.5f + i / 5.0f, 1);
        herz(g, 194 + sinf(k * 7 + i) * 20, 150 - k * 100, 5 + (i % 3), FARBE(0xFF5A7A), (uint8_t)(255 * huepf(k)));
      }
    blase(g, 140, 40, {"Diese Stimme kenne ich!"}, aufzu(u, 6, 9), 214, 130);
    blase(g, 110, 40, {"Du bist es!"}, aufzu(u, 9.4f, 12), 168, 130);
    blase(g, 150, 40, {"Ich kann wieder sehen!"}, aufzu(u, 16.8f, 20.6f), 214, 130);
    if (u > 32) g.rechteck(0, 0, 284, 240, 0, (uint8_t)(255 * seg(u, 32, 34)));
    if (u < 1.2f) { g.kameraAus(); g.iris(180, 150, lerp(0, 330, sanft(u / 1.2f))); }
    return;
  }
  // heim in sein Reich: Freude, Glocken
  float v = u - 34;
  himmel(g, 0, u, 196);
  sonne(g, 244, 34, 12, u);
  schloss(g, 142, 196, u, 0);
  boden(g, 196);
  FrauPose r = fp(BRAUT, u);
  r.mund = 0.4f;
  frau(g, 124, 214, 0.92f, 1, r);
  Pose p = mp(PRINZ, u);
  p.mund = 0.4f;
  p.armV = 2.5f + sinf(u * 8) * 0.3f;
  schneiderlein(g, 160, 214, 0.95f, -1, p);
  Pose b = mp(SOHN, u);
  FrauPose t = fp(TOCHTER, u + 1);
  b.armV = 2.6f; t.armV = 2.6f;
  schneiderlein(g, 90, 214 - fabsf(sinf(u * 6)) * 5, 0.5f, 1, b);
  frau(g, 194, 214 - fabsf(sinf(u * 6 + 1)) * 5, 0.5f, -1, t);
  for (int i = 0; i < 6; i++) {
    float k = fmodf(v * 0.45f + i / 6.0f, 1);
    herz(g, 142 + sinf(k * 7 + i) * 40, 150 - k * 110, 5 + (i % 3), FARBE(0xFF5A7A), (uint8_t)(255 * huepf(k)));
  }
  g.kameraAus();
  if (v < 1) g.rechteck(0, 0, 284, 240, 0, (uint8_t)(255 * (1 - v)));
  g.text(SCHRIFT_K, "Sie lebten glücklich und vergnügt.", 142, 230, OUT, 1, (uint8_t)(255 * aufzu(v, 2, 9)));
  if (v > 9.5f) g.iris(142, 170, lerp(330, 0, sanft(seg(v, 9.5f, 11.5f))));
  if (v > 11.5f) {
    uint8_t a = (uint8_t)(255 * sanft(seg(v, 11.5f, 12.5f)));
    g.text(SCHRIFT_G, "Ende", 142, 122, GOLD, 1, a);
    for (int i = 0; i < 6; i++) {
      float an = v * 1.2f + i * 1.047f;
      g.stern(142 + cosf(an) * 60, 112 + sinf(an) * 30, 5, 2, 5, an, GOLD, a);
    }
  }
}

static void bild(Gfx &g, float t) {
  if (t < P2) szeneGarten(g, t - P1);
  else if (t < P3) szeneKind(g, t - P2);
  else if (t < P4) szeneHaar(g, t - P3);
  else if (t < P5) szenePrinz(g, t - P4);
  else if (t < P6) szeneVerrat(g, t - P5);
  else if (t < P7) szeneDornen(g, t - P6);
  else if (t < P8) szeneBlind(g, t - P7);
  else szeneWiedersehen(g, t - P8);
}

// =====================================================================
// Musik
// =====================================================================
// Rapunzels Lied (E-Dur, sehnsuechtig)
static const Ton3 MEL_RAPUNZEL[] = {
  {76, 1, 52}, {80, .5, 0}, {83, .5, 0}, {81, 1, 0}, {80, 1, 0}, {78, 1, 47}, {76, .5, 0}, {78, .5, 0}, {80, 2, 0},
  {81, 1, 45}, {80, .5, 0}, {78, .5, 0}, {76, 1, 0}, {73, 1, 0}, {75, 1, 47}, {78, 1, 0}, {76, 2, 52}};
// Klagelied (h-Moll)
static const Ton3 MEL_KLAGE[] = {
  {71, 1.5, 47}, {69, .5, 0}, {67, 1, 0}, {66, 1, 0}, {64, 1, 43}, {66, 1, 0}, {67, 2, 0},
  {69, 1, 45}, {67, .5, 0}, {66, .5, 0}, {64, 1, 0}, {62, 1, 0}, {61, 1, 42}, {62, 1, 0}, {59, 2, 47}};

static void ruf(Synth &s, float x) {  // "Rapunzel, Rapunzel ..." gesungen
  const int t[8] = {67, 64, 67, 64, 69, 67, 64, 60};
  for (int i = 0; i < 8; i++) s.note(x + i * 0.45f, t[i], 0.7f);
}

static void ton(Synth &s, float t0, float t1) {
  float a = P1;
  spieleMelodie(s, M(MEL_KLAGE), a + 0.5f, a + 13, 0.4f, t0, t1, 0.7f);
  spieleMelodie(s, M(MEL_SPUK), a + 13.5f, a + 20, 0.32f, t0, t1, 0.7f);
  for (float x = ceilf(t0 / 0.3f) * 0.3f; x < t1; x += 0.3f)
    if (x > a + 17 && x < a + 20) s.rauschen(x, 0.06f, HOCHPASS, 2500, 0.05f);  // rupfen
  if (bei(a + 20.4f, t0, t1)) for (int k = 0; k < 8; k++) s.note(a + 20.4f + k * 0.06f, 52 + k * 3, 0.8f);  // die Zauberin!
  spieleMelodie(s, M(MEL_SPUK), a + 21, a + 39.5f, 0.3f, t0, t1, 0.85f);
  a = P2;
  spieleMelodie(s, M(MEL_WIEGE), a + 0.5f, a + 10.5f, 0.42f, t0, t1, 0.7f);
  spieleMelodie(s, M(MEL_RAPUNZEL), a + 11.5f, a + 21.5f, 0.3f, t0, t1, 0.8f);
  if (bei(a + 17, t0, t1)) for (int k = 0; k < 8; k++) s.glocke(a + 17 + k * 0.15f, 76 + (k % 4) * 3, 0.4f);
  a = P3;
  spieleMelodie(s, M(MEL_RAPUNZEL), a + 0.4f, a + 4, 0.3f, t0, t1, 0.8f);
  if (bei(a + 4.4f, t0, t1)) ruf(s, a + 4.4f);
  if (bei(a + 9, t0, t1)) for (int k = 0; k < 14; k++) s.note(a + 9 + k * 0.2f, 96 - k * 2, 0.4f);  // der Zopf faellt
  for (float x = ceilf(t0 / 0.5f) * 0.5f; x < t1; x += 0.5f)
    if (x > a + 13 && x < a + 20) s.note(x, 60 + fmodf(x * 2, 6), 0.3f);  // klettern
  spieleMelodie(s, M(MEL_RAPUNZEL), a + 21, a + 33.5f, 0.3f, t0, t1, 0.8f);
  a = P4;
  spieleMelodie(s, M(MEL_RAPUNZEL), a + 0.4f, a + 9, 0.3f, t0, t1, 0.8f);
  for (float x = ceilf(t0 / 0.227f) * 0.227f; x < t1; x += 0.227f)
    if (x > a && x < a + 5) s.note(x, 79, 0.18f);  // Hufe
  if (bei(a + 12.2f, t0, t1)) ruf(s, a + 12.2f);
  if (bei(a + 16, t0, t1)) for (int k = 0; k < 14; k++) s.note(a + 16 + k * 0.14f, 96 - k * 2, 0.4f);
  for (float x = ceilf(t0 / 0.5f) * 0.5f; x < t1; x += 0.5f)
    if (x > a + 19 && x < a + 25) s.note(x, 60 + fmodf(x * 2, 6), 0.3f);
  if (bei(a + 26.4f, t0, t1)) for (int k = 0; k < 4; k++) s.note(a + 26.4f + k * 0.08f, 84 + k * 2, 0.6f);  // Oh!
  spieleMelodie(s, M(MEL_F), a + 29, a + 43.5f, 0.26f, t0, t1, 0.85f);
  a = P5;
  spieleMelodie(s, M(MEL_RAPUNZEL), a + 0.4f, a + 7, 0.3f, t0, t1, 0.7f);
  if (bei(a + 7.2f, t0, t1)) for (int k = 0; k < 8; k++) s.note(a + 7.2f + k * 0.06f, 50 + k * 3, 0.8f);
  for (float x : {a + 11.2f, a + 11.6f, a + 12.0f, a + 12.4f}) if (bei(x, t0, t1)) { s.rauschen(x, 0.08f, HOCHPASS, 3000, 0.15f); s.ton(x, 3000, 0.08f, 0.002f, 0.05f); }  // ritsch, ratsch
  spieleMelodie(s, M(MEL_KLAGE), a + 13, a + 35.5f, 0.42f, t0, t1, 0.8f);
  a = P6;
  if (bei(a + 1.2f, t0, t1)) ruf(s, a + 1.2f);
  for (float x = ceilf(t0 / 0.5f) * 0.5f; x < t1; x += 0.5f)
    if (x > a + 7 && x < a + 12.6f) s.note(x, 60 + fmodf(x * 2, 6), 0.3f);
  spieleMelodie(s, M(MEL_SPUK), a + 13, a + 19, 0.3f, t0, t1, 0.9f);
  if (bei(a + 19, t0, t1)) for (int k = 0; k < 12; k++) s.note(a + 19 + k * 0.15f, 84 - k * 3, 0.6f);  // er springt
  if (bei(a + 21, t0, t1)) { s.ton(a + 21, 110, 0.35f, 0.002f, 0.3f, SINUS, GERAEUSCH, 55); s.rauschen(a + 21, 0.4f, BANDPASS, 1400, 0.25f); }
  spieleMelodie(s, M(MEL_KLAGE), a + 23, a + 39.5f, 0.42f, t0, t1, 0.7f);
  a = P7;
  spieleMelodie(s, M(MEL_KLAGE), a + 0.4f, a + 35.5f, 0.5f, t0, t1, 0.75f);
  for (float x = ceilf(t0 / 0.9f) * 0.9f; x < t1; x += 0.9f)
    if (x > a && x < a + 35) s.rauschen(x, 0.04f, TIEFPASS, 500, 0.05f);  // Schritte
  a = P8;
  spieleMelodie(s, M(MEL_RAPUNZEL), a + 0.4f, a + 8.5f, 0.3f, t0, t1, 0.8f);
  for (int i = 0; i < 2; i++) if (bei(a + 13 + i * 0.8f, t0, t1)) s.glocke(a + 13 + i * 0.8f, 88 + i * 4, 0.6f);  // zwei Traenen
  if (bei(a + 16.4f, t0, t1)) for (int k = 0; k < 12; k++) s.glocke(a + 16.4f + k * 0.15f, 72 + (k % 6) * 2, 0.5f);  // er sieht wieder
  spieleMelodie(s, M(MEL_F), a + 20, a + 45, 0.26f, t0, t1, 0.9f);
  for (int i = 0; i < 10; i++) if (bei(a + 35 + i * 0.6f, t0, t1)) s.glocke(a + 35 + i * 0.6f, i & 1 ? 79 : 72 + (i % 4 == 2) * 4, 0.8f);
  if (bei(a + 45.5f, t0, t1)) { s.glocke(a + 45.5f, 72); s.glocke(a + 45.8f, 76); s.glocke(a + 46.1f, 79); s.glocke(a + 46.5f, 84); }
}

// =====================================================================
// Startbild und Vorschau
// =====================================================================
static void titelSzene(Gfx &g, float t) {
  wald(g, t, 0);
  turm(g, TURM_X, t, 0);
  imFenster(g, RAPUNZEL, t, 0.3f);
  zopf(g, 1, t);
  singen(g, TURM_X + 14, FENSTER_Y - 10, t);
  Pose p = mp(PRINZ, t);
  p.armV = 2.4f + sinf(t * 2) * 0.2f;
  schneiderlein(g, 70, 222, 1, 1, p);
}

static void titelbild(Gfx &g, float t) {
  titelSzene(g, t);
  g.rundRechteck(8, 20, 130, 50, 14, FARBE(0x1E1A3A), 160);
  g.text(SCHRIFT_G, "Rapunzel", 73, 55, GOLD, 1);
  float b = 1 + sinf(t * 4) * 0.08f;
  g.kontur(2, OUT);
  g.kreis(73, 96, 14 * b, FARBE(0xE8473D));
  g.konturAus();
  float tri[] = {68, 88, 68, 104, 81, 96};
  g.polygon(tri, 3, WEISS);
}

static void vorschau(Gfx &g, float t) { titelSzene(g, t); }

static void symbol(Gfx &g, float x, float y, float t) {
  // Turmspitze mit Fenster, Rapunzel und dem langen Zopf ganz nah
  const float z = 0.62f;
  g.ansicht(x - 204 * z, y - 80 - (FENSTER_Y - 90) * z, z);
  turm(g, TURM_X, t, 0);
  imFenster(g, RAPUNZEL, t, 0.3f);
  zopf(g, 0.5f, t);
  singen(g, TURM_X + 14, FENSTER_Y - 10, t);
  g.kameraAus();
}

const Maerchen RAPUNZEL_M = {"Rapunzel", PDAUER, FARBE(0xF2C94E), bild, ton, titelbild, vorschau, symbol, LIED(MEL_RAPUNZEL, 0.4f)};

// Schneewittchen - ein Spieluhr-Film fuer die Maerchenbox, nach dem Maerchen der Brueder Grimm.
// Drei Tropfen Blut im Schnee, "Spieglein, Spieglein an der Wand", der Jaeger, die sieben Zwerge,
// Schnuerriemen, Kamm und Apfel, der glaeserne Sarg und der Koenigssohn.
#include "maerchen.h"
#include "schriften.h"
#include "figuren.h"
#include "melodien.h"
#include <stdio.h>
#include <initializer_list>

// ---------- Ablauf (Sekunden) ----------
static const float P1 = 0, P2 = 30, P3 = 56, P4 = 86, P5 = 140, P6 = 180, P7 = 220, P8 = 266, PDAUER = 300;

// ---------- Aussehen ----------
static const Farbe EBENHOLZ = FARBE(0x2A1E1E);
static FrauLook schneeLook() {
  FrauLook l; l.kleid = FARBE(0xD9343F); l.schuerze = FARBE(0xFFF6EE); l.haar = EBENHOLZ; l.frisur = 0;
  l.haut = FARBE(0xFFF0EA); l.blume = WEISS;
  return l;
}
static FrauLook kindLook() { FrauLook l = schneeLook(); l.schuerze = 0; l.frisur = 1; return l; }
static FrauLook muetterLook() {
  FrauLook l; l.kleid = FARBE(0x4A6ED0); l.haar = EBENHOLZ; l.frisur = 2; l.krone = true; l.glanz = true;
  return l;
}
static FrauLook koeniginLook() {
  FrauLook l; l.kleid = FARBE(0x5A2A6E); l.haar = FARBE(0x6E2E2E); l.frisur = 2; l.krone = true; l.wangen = false;
  l.rockBreite = 1.25f; l.haut = FARBE(0xF6E0D0);
  return l;
}
static FrauLook kraemerinLook() {
  FrauLook l; l.kleid = FARBE(0x7A6A5A); l.haar = FARBE(0xBDBDBD); l.frisur = 3; l.tuch = FARBE(0x5E4A3A);
  l.schuerze = FARBE(0xA89A80); l.wangen = false; l.haut = FARBE(0xE6D2B8); l.flicken = true;
  return l;
}
static FrauLook bauersfrauLook() {
  FrauLook l = kraemerinLook(); l.kleid = FARBE(0x6E7E5A); l.tuch = FARBE(0xC9B07A); l.flicken = false; l.schuerze = FARBE(0xE8DCC0);
  return l;
}
static FrauLook brautLook() { FrauLook l = schneeLook(); l.kleid = WEISS; l.schuerze = 0; l.glanz = true; l.krone = true; l.rockBreite = 1.4f; l.frisur = 2; return l; }
static const FrauLook SCHNEE = schneeLook(), SCHNEE_KIND = kindLook(), MUTTER = muetterLook(), KOENIGIN = koeniginLook(),
                      KRAEMERIN = kraemerinLook(), BAUERSFRAU = bauersfrauLook(), BRAUT = brautLook();

static Aussehen jaegerLook() {
  Aussehen a; a.weste = FARBE(0x4E7A3A); a.hemd = FARBE(0xE8DCC0); a.hose = FARBE(0x5A4636); a.haar = FARBE(0x6E4A2A);
  a.hut = FARBE(0x3A5A2A); a.hutD = FARBE(0x2A4220); a.feder = FARBE(0xC9A43A);
  return a;
}
static Aussehen prinzLook() {
  Aussehen a; a.weste = FARBE(0x2E5AA8); a.hemd = FARBE(0xFFF6E0); a.hose = FARBE(0x3A2E5A); a.haar = FARBE(0xC98A3A);
  a.muetze = 3; a.halstuch = GOLD;
  return a;
}
static Aussehen dienerLook() { Aussehen a; a.weste = FARBE(0x8E2A2A); a.hose = FARBE(0x3A2A2A); a.muetze = 1; a.haar = FARBE(0x5A3A26); return a; }
static Aussehen zwergLook(int i) {
  static const uint32_t HUT[7] = {0xC0392B, 0x3E7FDC, 0x3DAE5B, 0xE0A23C, 0x9A5AC8, 0xE86FA8, 0x2E9A9A};
  Aussehen a;
  a.weste = FARBE(0x7A5A3A); a.hemd = FARBE(0x9A7A5A); a.hose = FARBE(0x4A3B2A); a.haar = FARBE(0xC9C9C9);
  a.hut = FARBE(HUT[i]); a.hutD = mischeFarbe(a.hut, OUT, 0.3f); a.feder = a.hut; a.haut = FARBE(0xF2C9A8);
  return a;
}
static const Aussehen JAEGER = jaegerLook(), PRINZ = prinzLook(), DIENER = dienerLook();
static const Aussehen ZW[7] = {zwergLook(0), zwergLook(1), zwergLook(2), zwergLook(3), zwergLook(4), zwergLook(5), zwergLook(6)};

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
  p.zu = fmodf(t + 0.4f + a.hut * 0.0003f, 4.1f) < 0.13f;
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

static void puff(Gfx &g, float x, float y, float k, Farbe c = FARBE(0xB28DFF)) {
  if (k <= 0 || k >= 1) return;
  for (int i = 0; i < 10; i++) {
    float a = i * 0.628f;
    g.stern(x + cosf(a) * k * 50, y + sinf(a) * k * 44, 6 * (1 - k) + 2, 2, 5, a + k * 4, i & 1 ? c : WEISS,
            (uint8_t)(255 * (1 - k)));
  }
  g.kreis(x, y, 34 * (1 - k), WEISS, (uint8_t)(180 * (1 - k)));
}

static void schnee(Gfx &g, float t, int n, uint8_t a = 230) {
  for (int i = 0; i < n; i++) {
    float x = fmodf(zufall(i + 10) * 300 + sinf(t + i) * 8, 300) - 8;
    float y = fmodf(zufall(i + 30) * 240 + t * (14 + zufall(i + 50) * 10), 250) - 6;
    g.kreis(x, y, 1.4f + zufall(i + 70) * 1.4f, WEISS, a);
  }
}

// Zwerg mit langem weissem Bart
static void zwerg(Gfx &g, int nr, float x, float y, float s, float dir, const Pose &pIn, float t) {
  Pose p = pIn;
  p.aussehen = &ZW[nr];
  schneiderlein(g, x, y, s, dir, p);
  float cx = x + 5 * s * dir, cy = y - 50 * s;
  float ex = cx + sinf(t * 2 + nr) * 2 * s, ey = cy + 30 * s;
  float mx = (cx + ex) / 2 + 3 * s * dir, my = (cy + ey) / 2;
  float pts[2 * 14];
  int n = 0;
  for (int i = 0; i <= 6; i++) {
    float u = i / 6.0f, v = 1 - u;
    pts[2 * n] = v * v * cx + 2 * v * u * mx + u * u * ex - lerp(10, 2.5f, u) * s;
    pts[2 * n + 1] = v * v * cy + 2 * v * u * my + u * u * ey; n++;
  }
  for (int i = 6; i >= 0; i--) {
    float u = i / 6.0f, v = 1 - u;
    pts[2 * n] = v * v * cx + 2 * v * u * mx + u * u * ex + lerp(10, 2.5f, u) * s;
    pts[2 * n + 1] = v * v * cy + 2 * v * u * my + u * u * ey; n++;
  }
  g.polygon(pts, n, FARBE(0xEDEDED));
  g.ellipse(cx, cy - 2 * s, 8 * s, 3.5f * s, FARBE(0xEDEDED));
}

// liegende Figur (Kopf links), Augen zu
static void liegt(Gfx &g, float x, float y, float s, const FrauLook &l) {
  g.kontur(1.3f * s, OUT);
  g.ellipse(x - 40 * s, y - 13 * s, 15 * s, 12 * s, l.haar);
  g.ellipse(x + 8 * s, y - 9 * s, 36 * s, 10 * s, l.kleid);
  g.kreis(x - 34 * s, y - 14 * s, 12 * s, l.haut);
  g.konturAus();
  g.bogen(x - 37 * s, y - 15 * s, 2.4f * s, 0.3f, 2.8f, 1.4f * s, OUT);
  g.bogen(x - 29 * s, y - 15 * s, 2.4f * s, 0.3f, 2.8f, 1.4f * s, OUT);
  g.ellipse(x - 33 * s, y - 8 * s, 2.4f * s, 1.4f * s, FARBE(0xD9343F));
  g.kreis(x - 4 * s, y - 18 * s, 4 * s, l.haut);  // gefaltete Haende
  if (l.krone) g.rechteck(x - 50 * s, y - 20 * s, 4 * s, 12 * s, GOLD);
}

// Zauberspiegel; gesicht 0..1 erscheint im Glas, mund 0..1 spricht
static void spiegel(Gfx &g, float x, float y, float s, float t, float gesicht, float mund) {
  g.kontur(2 * s, OUT);
  g.ellipse(x, y, 40 * s, 52 * s, GOLD);
  g.konturAus();
  for (int i = 0; i < 10; i++) {
    float a = i * 0.628f;
    g.kreis(x + cosf(a) * 38 * s, y + sinf(a) * 50 * s, 3.5f * s, FARBE(0xE0A23C));
  }
  g.ellipse(x, y, 32 * s, 44 * s, mischeFarbe(FARBE(0xBFD8EE), FARBE(0x5A6E9A), gesicht * 0.6f));
  g.linie(x - 16 * s, y - 28 * s, x - 6 * s, y - 36 * s, 3 * s, WEISS, 160);
  if (gesicht > 0.02f) {
    uint8_t a = (uint8_t)(200 * gesicht);
    g.ellipse(x, y + 2 * s, 18 * s, 24 * s, FARBE(0xE6EEF8), a);
    g.ellipse(x - 7 * s, y - 4 * s, 3 * s, 2 * s, FARBE(0x3A4A7A), a);
    g.ellipse(x + 7 * s, y - 4 * s, 3 * s, 2 * s, FARBE(0x3A4A7A), a);
    g.ellipse(x, y + 12 * s, 5 * s, (1 + mund * 4) * s, FARBE(0x3A4A7A), a);
  }
  float sch = 0.5f + 0.5f * sinf(t * 1.3f);
  g.stern(x + 18 * s, y - 30 * s, 4 * s, 1.5f * s, 4, t, WEISS, (uint8_t)(200 * sch));
}

static void spiegelSaal(Gfx &g, float t) {
  g.verlauf(0, 200, FARBE(0x4A3A6E), FARBE(0x2E2448));
  for (int i = 0; i < 3; i++) {
    float x = 24 + i * 118;
    g.rechteck(x - 8, 0, 16, 200, FARBE(0x5E4A80));
  }
  g.rechteck(0, 200, 284, 40, FARBE(0x5A3A3A));
  for (int i = 0; i < 6; i++) g.rechteck(i * 50, 206 + (i & 1) * 12, 40, 3, FARBE(0x7A4A4A));
  // Kerzen
  for (int i = 0; i < 2; i++) {
    float x = 30 + i * 224;
    g.rechteck(x - 3, 120, 6, 24, WEISS);
    g.ellipse(x + sinf(t * 9 + i) * 0.8f, 114, 2.4f, 5, FARBE(0xFFB83D));
    g.kreis(x, 116, 22, FARBE(0xFFE0A0), 40);
  }
}

static void apfel(Gfx &g, float x, float y, float r, float halb = 0, float biss = 0) {
  g.kontur(1.2f, OUT);
  g.kreis(x, y, r, FARBE(0xD9343F));
  g.konturAus();
  g.ellipse(x + r * 0.45f, y, r * 0.5f, r * 0.9f, FARBE(0xF6EED6));  // weisse Haelfte
  if (halb > 0) g.linie(x, y - r, x, y + r, 1, OUT, (uint8_t)(255 * halb));
  if (biss > 0) g.kreis(x - r * 0.9f, y - r * 0.2f, r * 0.45f * biss, FARBE(0xFFF0DC));
  g.linie(x, y - r, x + 2, y - r - 5, 1.5f, HOLZ_D);
  g.ellipse(x + 5, y - r - 4, 4, 2, FARBE(0x4CA85A), 255, -0.5f);
  g.kreis(x - r * 0.4f, y - r * 0.4f, r * 0.2f, WEISS, 170);
}

// ---------- Kulissen ----------
static void fensterWinter(Gfx &g, float t) {
  g.verlauf(0, 240, FARBE(0x6E5A4E), FARBE(0x4E3E36));
  // das Fenster mit dem Rahmen aus Ebenholz, draussen fallen die Flocken
  g.kontur(2, OUT);
  g.rundRechteck(120, 20, 150, 150, 6, EBENHOLZ);
  g.konturAus();
  g.ausschnitt(128, 28, 134, 134);
  g.verlauf(28, 162, FARBE(0x8EA6C8), FARBE(0xD8E6F2));
  g.ellipse(195, 170, 110, 30, WEISS);
  for (int i = 0; i < 3; i++) tanne(g, 150 + i * 44, 156, 0.6f, FARBE(0x3E6A5A));
  schnee(g, t, 22);
  g.ausschnittAus();
  g.rechteck(193, 28, 4, 134, EBENHOLZ);
  g.rechteck(128, 93, 134, 4, EBENHOLZ);
  g.rechteck(112, 166, 166, 10, EBENHOLZ);  // Fensterbrett
  // Schnee auf dem Fensterbrett (draussen)
  g.ellipse(195, 164, 66, 4, WEISS);
}

static void wald(Gfx &g, float t, float nacht) {
  himmel(g, nacht, t, 200);
  if (nacht > 0.5f) mond(g, 240, 36, 11);
  Farbe tan = mischeFarbe(FARBE(0x2F8F4E), FARBE(0x1F3B45), nacht);
  for (int i = 0; i < 6; i++) tanne(g, i * 54 + 6, 200, 0.85f + (i % 3) * 0.15f, tan);
  boden(g, 200, mischeFarbe(GRAS, FARBE(0x2F4A40), nacht));
}

static void zwergHausAussen(Gfx &g, float t, float nacht) {
  wald(g, t, nacht);
  g.kontur(1.8f, OUT);
  g.rechteck(150, 130, 100, 70, FARBE(0xE8D0A0));
  float d[] = {140, 132, 200, 92, 260, 132};
  g.polygon(d, 3, FARBE(0xB05A3A));
  g.konturAus();
  g.rundRechteck(190, 160, 22, 40, 10, HOLZ);
  for (int i = 0; i < 2; i++) {
    g.rundRechteck(160 + i * 64, 144, 18, 16, 2, nacht > 0.3f ? FARBE(0xFFD27A) : FARBE(0x8FD3FF));
    if (nacht > 0.3f) g.kreis(169 + i * 64, 152, 20, FARBE(0xFFD27A), (uint8_t)(50 * nacht));
  }
  g.rechteck(226, 96, 12, 20, FARBE(0x8E6E5A));  // Schornstein
}

// Stube der Zwerge: sieben Tellerlein am Tisch, sieben Bettlein an der Wand
static void zwergStube(Gfx &g, float t, float nacht, int gegessen) {
  g.verlauf(0, 200, mischeFarbe(FARBE(0xE8D0A0), FARBE(0x5E4E5A), nacht), mischeFarbe(FARBE(0xD6B88E), FARBE(0x4E3E4A), nacht));
  g.rechteck(0, 200, 284, 40, FARBE(0x9E7656));
  for (int i = 0; i < 7; i++) {  // Bettlein
    float x = 8 + i * 40;
    g.kontur(1.2f, OUT);
    g.rundRechteck(x, 92, 34, 40, 4, HOLZ);
    g.konturAus();
    g.rundRechteck(x + 3, 98, 28, 30, 3, WEISS);
    g.rundRechteck(x + 3, 110, 28, 18, 3, ZW[i].hut);
  }
  // Tisch mit weissem Tuch
  g.kontur(1.6f, OUT);
  g.rundRechteck(30, 168, 224, 14, 4, WEISS);
  g.konturAus();
  g.rechteck(40, 182, 8, 30, HOLZ_D);
  g.rechteck(236, 182, 8, 30, HOLZ_D);
  for (int i = 0; i < 7; i++) {
    float x = 46 + i * 32;
    g.ellipse(x, 168, 10, 3, FARBE(0xE8E2D8));
    if (i >= gegessen) g.kreis(x - 2, 166, 3, FARBE(0xC9A46A));  // Brot
    g.rechteck(x + 8, 156, 5, 10, FARBE(0xC0C8D4));          // Becherlein
    g.linie(x - 10, 167, x - 10, 158, 1.2f, FARBE(0xC0C8D4));  // Loeffelein
  }
  if (nacht > 0.4f) {
    g.rechteck(140, 150, 5, 14, WEISS);
    g.ellipse(142.5f + sinf(t * 9) * 0.6f, 146, 2, 4, FARBE(0xFFB83D));
    g.kreis(142, 148, 40, FARBE(0xFFE0A0), (uint8_t)(50 * nacht));
  }
}

static void berg(Gfx &g, float t, float nacht) {
  himmel(g, nacht, t, 200);
  if (nacht > 0.5f) mond(g, 240, 36, 11); else sonne(g, 244, 34, 12, t);
  float b[] = {-10, 200, 70, 90, 150, 140, 220, 80, 300, 200};
  g.polygon(b, 5, mischeFarbe(FARBE(0x8EA6B8), FARBE(0x3E4A6A), nacht));
  float sb[] = {54, 110, 70, 90, 86, 110, 70, 104};
  g.polygon(sb, 4, WEISS);
  boden(g, 200, mischeFarbe(GRAS, FARBE(0x2F4A40), nacht));
}

static void sarg(Gfx &g, float x, float y, const FrauLook &l, float deckel) {
  g.rechteck(x - 56, y - 6, 112, 8, GOLD);
  liegt(g, x + 4, y - 6, 0.85f, l);
  // Glas
  float dy = -deckel * 30;
  g.kontur(1.4f, FARBE(0x7AA6C8));
  g.rundRechteck(x - 58, y - 34 + dy, 116, 30, 6, FARBE(0xD8EEFF), 90);
  g.konturAus();
  g.linie(x - 46, y - 28 + dy, x - 30, y - 28 + dy, 2, WEISS, 200);
  g.text(SCHRIFT_K, "Königstochter", x, y - 14 + dy, FARBE(0xA8761B), 1);
}

static void eule(Gfx &g, float x, float y, float t) {
  g.kontur(1.2f, OUT);
  g.ellipse(x, y, 9, 12, FARBE(0x8E6E4E));
  g.konturAus();
  for (int i = 0; i < 2; i++) {
    g.kreis(x - 4 + i * 8, y - 4, 3.6f, WEISS);
    g.kreis(x - 4 + i * 8, y - 4, 1.8f, OUT);
  }
  float b[] = {x - 2, y, x + 2, y, x, y + 3};
  g.polygon(b, 3, GOLD);
}
static void rabe(Gfx &g, float x, float y, float t) {
  g.kontur(1, OUT);
  g.ellipse(x, y, 11, 7, SCHWARZ);
  g.konturAus();
  g.kreis(x + 9, y - 5, 5, SCHWARZ);
  float b[] = {x + 13, y - 6, x + 19, y - 4, x + 13, y - 3};
  g.polygon(b, 3, FARBE(0x5E5A5A));
  g.kreis(x + 10, y - 6, 1.2f, WEISS);
}

static void zwergeReihe(Gfx &g, float x0, float abstand, float y, float s, float dir, float t, bool weinen) {
  for (int i = 0; i < 7; i++) {
    Pose z = mp(ZW[i], t);
    if (weinen) { z.zu = true; z.armV = 2.2f; }
    zwerg(g, i, x0 + i * abstand, y, s, dir, z, t);
    if (weinen) {
      float k = fmodf(t * 1.3f + i * 0.3f, 1);
      g.kreis(x0 + i * abstand + 6 * s * dir, y - 56 * s + k * 12, 1.4f, FARBE(0x8FD8FF), (uint8_t)(255 * (1 - k)));
    }
  }
}

// =====================================================================
// Szenen
// =====================================================================
// 1) Die Koenigin naeht am Fenster; drei Tropfen Blut im Schnee
static void szeneFenster(Gfx &g, float u) {
  if (u < 20) {
    fensterWinter(g, u);
    FrauPose k = fp(MUTTER, u);
    k.armV = 1.5f + sinf(u * 5) * (u < 6 ? 0.3f : 0);
    k.mund = u > 9 && u < 16 ? 0.4f : 0;
    float hx, hy;
    frau(g, 80, 214, 1.05f, 1, k, &hx, &hy);
    g.rundRechteck(hx - 2, hy - 2, 26, 10, 3, WEISS);  // Naeharbeit
    g.linie(hx + 10, hy, hx + 22, hy - 10, 1, FARBE(0xC0C8D4));
    if (u > 5.6f) {
      // drei Tropfen fallen in den Schnee auf dem Fensterbrett
      for (int i = 0; i < 3; i++) {
        float k0 = 6 + i * 0.8f;
        float k = seg(u, k0, k0 + 0.8f);
        if (k < 1) tropfen(g, hx + 22, lerp(hy, 160, hinein(k)), 2, FARBE(0xC01830));
        else g.ellipse(180 + i * 12, 163, 3, 1.5f, FARBE(0xC01830));
      }
    }
    if (u > 5.4f && u < 6.2f) g.text(SCHRIFT_K, "au!", hx + 6, hy - 22, OUT, 1);
    blase(g, 150, 42, {"Hätt ich ein Kind so weiß wie Schnee,", "so rot wie Blut und so schwarz", "wie das Holz an dem Rahmen!"},
          aufzu(u, 9.2f, 16.6f), 96, 120);
    g.kameraAus();
    if (u > 18) g.rechteck(0, 0, 284, 240, 0, (uint8_t)(255 * seg(u, 18, 20)));
  } else {
    float v = u - 20;
    g.verlauf(0, 240, FARBE(0x6E5A4E), FARBE(0x4E3E36));
    // die Wiege mit dem Kind
    g.kontur(1.4f, OUT);
    float w[] = {104, 180, 180, 180, 170, 206, 114, 206};
    g.polygon(w, 4, HOLZ);
    g.konturAus();
    g.kreis(126, 176, 10, FARBE(0xFFF0EA));
    g.ellipse(120, 170, 9, 5, EBENHOLZ);
    g.ellipse(150, 180, 22, 8, FARBE(0xF6B8D2));
    g.kreis(122, 180, 1.6f, WANGE);
    g.bogen(124, 176, 2, 3.5f, 5.9f, 1, OUT);
    glitzern(g, 140, 170, 34, u, 6);
    g.kameraAus();
    g.text(SCHRIFT_B, "Schneewittchen", 142, 60, WEISS, 1, (uint8_t)(255 * aufzu(v, 0.6f, 9.6f)));
    g.text(SCHRIFT_K, "weiß wie Schnee, rot wie Blut,", 142, 90, WEISS, 1, (uint8_t)(255 * aufzu(v, 1.4f, 9.6f)));
    g.text(SCHRIFT_K, "schwarzhaarig wie Ebenholz", 142, 110, WEISS, 1, (uint8_t)(255 * aufzu(v, 1.4f, 9.6f)));
    if (v < 1) g.rechteck(0, 0, 284, 240, 0, (uint8_t)(255 * (1 - v)));
  }
  blende(g, u, 30, 142, 140);
}

// 2) Die neue Koenigin und ihr Spiegel; sieben Jahre spaeter
static void szeneSpiegel(Gfx &g, float u) {
  spiegelSaal(g, u);
  bool zweites = u > 13;
  float sprich = zweites ? seg(u, 16, 17) * (1 - seg(u, 23, 24)) : seg(u, 6, 7) * (1 - seg(u, 11.6f, 12.6f));
  spiegel(g, 200, 110, 1.1f, u, sprich, sprich > 0.5f ? fabsf(sinf(u * 9)) : 0);
  FrauPose k = fp(KOENIGIN, u);
  k.armV = u < 6 || (u > 13.5f && u < 16) ? 1.8f : 0.6f;
  k.mund = (u > 1 && u < 5.8f) || (u > 13.4f && u < 15.8f) ? 0.5f : 0;
  k.boese = u > 22 ? 1 : 0.3f;
  if (u > 22) k.armV = 2.4f + sinf(u * 8) * 0.2f;
  frau(g, 90, 214, 1.05f, 1, k);
  blase(g, 90, 40, {"Spieglein, Spieglein an der Wand,", "wer ist die Schönste im ganzen Land?"}, aufzu(u, 1, 5.8f), 100, 120);
  blase(g, 150, 30, {"Frau Königin, Ihr seid", "die Schönste im Land."}, aufzu(u, 7, 12.2f), 186, 80);
  blase(g, 90, 40, {"Spieglein, Spieglein an der Wand,", "wer ist die Schönste im ganzen Land?"}, aufzu(u, 13.4f, 15.8f), 100, 120);
  blase(g, 142, 30, {"Frau Königin, Ihr seid die", "Schönste hier, aber Schneewittchen", "ist tausendmal schöner als Ihr."},
        aufzu(u, 16.4f, 23.6f), 186, 80);
  g.kameraAus();
  if (u > 12.4f && u < 13.6f) {
    g.rechteck(0, 0, 284, 240, 0, (uint8_t)(255 * huepf(seg(u, 12.4f, 13.6f))));
  }
  if (u > 12.8f) g.text(SCHRIFT_K, "sieben Jahre später", 10, 232, WEISS, 0, (uint8_t)(255 * aufzu(u, 12.8f, 16)));
  blende(g, u, 26, 120, 130);
}

// 3) Der Jaeger: "Ach, lieber Jaeger, lass mir mein Leben!" - sie laeuft durch den wilden Wald
static void szeneJaeger(Gfx &g, float u) {
  if (u < 17) {
    wald(g, u, 0);
    for (int i = 0; i < 5; i++) g.kreis(30 + i * 50, 210 - (i & 1) * 4, 4, i & 1 ? WEISS : FARBE(0xFF7FA8));
    FrauPose s = fp(SCHNEE, u);
    s.armV = u > 4 ? 2.2f : 1.0f;
    s.mund = u > 4 && u < 9 ? 0.6f : 0;
    s.weint = u > 4 && u < 11;
    float sx = u > 13 ? lerp(110, 330, seg(u, 13, 16.6f)) : 110;
    s.lauf = u > 13 ? fmodf(u * 1.8f, 1) : -1;
    float hx, hy;
    if (u < 1.5f || u > 2.8f) { float b = u < 1.5f ? 0 : 1; s.armV = lerp(1.6f, s.armV, b); }
    frau(g, sx, 214, 0.95f, u > 13 ? 1 : -1, s, &hx, &hy);
    if (u < 2.6f) g.kreis(hx + 4, hy - 4, 3, FARBE(0xFF7FA8));  // pflueckt Blumen
    Pose j = mp(JAEGER, u);
    j.armV = u > 2.6f && u < 4 ? 2.4f : u > 9 ? 1.6f : 0.8f;
    j.mund = u > 9.4f && u < 13 ? 0.5f : 0;
    float jhx, jhy;
    schneiderlein(g, 190, 214, 1, -1, j, &jhx, &jhy);
    if (u > 2.6f && u < 9) g.linie(jhx, jhy, jhx - 10, jhy - 10, 2.2f, FARBE(0xC0C8D4));  // Hirschfaenger
    blase(g, 90, 50, {"Ach, lieber Jäger,", "lass mir mein Leben!"}, aufzu(u, 4.2f, 9), 104, 120);
    blase(g, 190, 46, {"So lauf hin, du armes Kind!"}, aufzu(u, 9.4f, 13), 186, 120);
  } else {
    // allein im wilden Wald, abends das kleine Haeuschen
    float v = u - 17;
    float nacht = seg(v, 4, 9) * 0.6f;
    if (v < 8) {
      wald(g, u, 0.5f);
      for (int i = 0; i < 4; i++) {  // Augen im Dunkel
        float a = huepf(fmodf(u * 0.6f + i * 0.27f, 1));
        g.kreis(30 + i * 70, 150 + (i & 1) * 20, 2.6f, GOLD, (uint8_t)(255 * a));
        g.kreis(38 + i * 70, 150 + (i & 1) * 20, 2.6f, GOLD, (uint8_t)(255 * a));
      }
      FrauPose s = fp(SCHNEE, u);
      s.lauf = fmodf(u * 1.8f, 1);
      s.weint = true;
      s.armV = 1.2f;
      frau(g, lerp(-30, 320, v / 8), 214, 0.95f, 1, s);
    } else {
      zwergHausAussen(g, u, nacht);
      FrauPose s = fp(SCHNEE, u);
      s.lauf = v < 12 ? fmodf(u * 1.4f, 1) : -1;
      s.armV = v > 11 ? 1.6f : 0.6f;
      float sx = lerp(-30, 180, seg(v, 8, 12));
      if (v > 12.4f) sx = 330;  // hinein
      frau(g, sx, 214, 0.8f, 1, s);
    }
  }
  blende(g, u, 30, 142, 150);
}

// 4) Im Zwergenhaus: von jedem Tellerlein ein wenig; die Zwerge kommen heim
static void szeneZwerge(Gfx &g, float u) {
  float nacht = u > 14 ? 0.7f : 0.3f;
  int gegessen = (int)(seg(u, 1, 7) * 7.99f);
  if (u < 14) {
    zwergStube(g, u, nacht, gegessen);
    FrauPose s = fp(SCHNEE, u);
    if (u < 8) {
      float k = seg(u, 1, 7);
      s.armV = 1.6f + sinf(u * 6) * 0.2f;
      frau(g, 40 + k * 196, 214, 0.8f, 1, s);
    } else {
      // muede: probiert die Bettlein, schlaeft im siebenten ein
      if (u < 12) {
        s.lauf = fmodf(u * 1.4f, 1);
        frau(g, lerp(40, 250, seg(u, 8, 12)), 214, 0.8f, 1, s);
      } else {
        g.ellipse(256, 108, 8, 6, EBENHOLZ);
        g.kreis(258, 110, 6, SCHNEE.haut);
        zzz(g, 260, 92, u);
      }
    }
    g.kameraAus();
    g.text(SCHRIFT_K, "Alles war klein, aber zierlich und reinlich.", 142, 232, OUT, 1, (uint8_t)(255 * aufzu(u, 1, 7)));
  } else if (u < 34) {
    float v = u - 14;
    zwergStube(g, u, nacht, 7);
    g.ellipse(256, 108, 8, 6, EBENHOLZ);
    g.kreis(258, 110, 6, SCHNEE.haut);
    zzz(g, 260, 92, u, (uint8_t)(255 * (1 - seg(v, 17, 19))));
    // die sieben Zwerge kommen mit ihren Lichtlein aus den Bergen
    for (int i = 0; i < 7; i++) {
      Pose z = mp(ZW[i], u);
      float ankunft = seg(v, i * 0.4f, 3 + i * 0.4f);
      float x = lerp(-40 - i * 30, 36 + i * 32, sanft(ankunft));
      z.lauf = ankunft < 1 ? fmodf(u * 1.8f + i * 0.3f, 1) : -1;
      bool spricht = v > 4 + i * 1.6f && v < 5.4f + i * 1.6f;
      z.mund = spricht ? 0.6f : 0;
      if (v > 15) { z.mund = 0.7f; z.armV = 2.6f; z.blick = 1; }
      zwerg(g, i, x, 214, 0.55f, v > 15 ? 1 : 1, z, u);
      if (ankunft < 1) {
        g.kreis(x + 14, 170, 3, FARBE(0xFFD27A));
        g.kreis(x + 14, 170, 9, FARBE(0xFFD27A), 70);
      }
    }
    static const char *FR1[7] = {"Wer hat auf meinem", "Wer hat von meinem", "Wer hat von meinem", "Wer hat von meinem",
                                 "Wer hat mit meinem", "Wer hat mit meinem", "Wer hat aus meinem"};
    static const char *FR2[7] = {"Stühlchen gesessen?", "Tellerchen gegessen?", "Brötchen genommen?", "Gemüschen gegessen?",
                                 "Gäbelchen gestochen?", "Messerchen geschnitten?", "Becherlein getrunken?"};
    for (int i = 0; i < 7; i++) {
      float a = 4 + i * 1.6f;
      if (v > a && v < a + 1.6f) blase(g, 142, 40, {FR1[i], FR2[i]}, aufzu(v, a, a + 1.6f), 36 + i * 32, 160);
    }
    blase(g, 142, 36, {"Ei, du mein Gott!", "Was ist das Kind so schön!"}, aufzu(v, 15.4f, 19.6f), 228, 160);
  } else {
    // am Morgen: "Willst du unsern Haushalt versehen ..."
    float v = u - 34;
    zwergStube(g, u, 0.1f, 7);
    FrauPose s = fp(SCHNEE, u);
    s.mund = (v > 0.2f && v < 3.4f) || (v > 9 && v < 12) ? 0.6f : 0;
    s.armV = v > 9 ? 2.2f : 1.0f;
    frau(g, 210, 214, 0.95f, -1, s);
    for (int i = 0; i < 7; i++) {
      Pose z = mp(ZW[i], u);
      z.mund = v > 3.6f && v < 8.8f && i == 0 ? 0.6f : v > 12 ? 0.4f : 0;
      if (v > 12) z.armV = 2.4f + sinf(u * 8 + i) * 0.3f;
      zwerg(g, i, 16 + i * 28, 214 - (v > 12 ? fabsf(sinf(u * 6 + i)) * 4 : 0), 0.55f, 1, z, u);
    }
    blase(g, 200, 40, {"Ich heiße Schneewittchen."}, aufzu(v, 0.2f, 3.4f), 214, 120);
    blase(g, 110, 40, {"Willst du unsern Haushalt", "versehen, kochen, betten, waschen,", "so kannst du bei uns bleiben."},
          aufzu(v, 3.6f, 8.8f), 30, 160);
    blase(g, 200, 50, {"Ja, von Herzen gern!"}, aufzu(v, 9, 12), 214, 120);
    blase(g, 110, 40, {"Hüte dich vor deiner Stiefmutter", "und lass niemand herein!"}, aufzu(v, 13, 19.4f), 30, 160);
  }
  blende(g, u, 54, 142, 150);
}

// 5) "Schoene Ware feil!" - der Schnuerriemen und der giftige Kamm
static void szeneKraemerin(Gfx &g, float u) {
  if (u < 10) {
    spiegelSaal(g, u);
    float sprich = seg(u, 0.4f, 1) * (1 - seg(u, 5.4f, 6));
    spiegel(g, 200, 110, 1.1f, u, sprich, sprich > 0.5f ? fabsf(sinf(u * 9)) : 0);
    bool verwandelt = u > 7.6f;
    FrauPose k = fp(verwandelt ? KRAEMERIN : KOENIGIN, u);
    k.boese = 1;
    k.armV = u > 6 && u < 7.6f ? 2.6f : 0.6f;
    frau(g, 90, 214, verwandelt ? 0.95f : 1.05f, 1, k);
    puff(g, 90, 150, seg(u, 7.2f, 8.4f));
    blase(g, 142, 30, {"Schneewittchen über den Bergen", "bei den sieben Zwergen ist noch", "tausendmal schöner als Ihr."},
          aufzu(u, 0.6f, 5.8f), 186, 80);
  } else {
    float v = u - 10;
    bool kamm = v > 15;
    float w = kamm ? v - 15 : v;
    zwergHausAussen(g, u, 0);
    FrauPose k = fp(KRAEMERIN, u);
    k.boese = 0.6f;
    k.mund = w < 3 ? 0.6f : 0;
    k.armV = w > 4 && w < 8 ? 1.7f : 1.0f;
    float kx = lerp(-30, 100, sanft(seg(w, 0, 2.6f)));
    if (w > 9) { kx = lerp(100, -40, seg(w, 9, 12)); k.lauf = fmodf(u * 1.6f, 1); }
    else if (w < 2.6f) k.lauf = fmodf(u * 1.6f, 1);
    float hx, hy;
    frau(g, kx, 214, 0.95f, w > 9 ? -1 : 1, k, &hx, &hy);
    if (w < 9) {
      if (!kamm) g.kurve(hx, hy, hx + 8, hy + 12, hx + 4, hy + 24, 2.5f, FARBE(0xE86FA8));  // bunter Schnuerriemen
      else { g.rundRechteck(hx - 2, hy - 4, 14, 6, 2, FARBE(0x5A3A26)); for (int i = 0; i < 5; i++) g.linie(hx + i * 3, hy + 2, hx + i * 3, hy + 6, 1, FARBE(0x5A3A26)); }
    }
    // Schneewittchen am Fenster / an der Tuer
    FrauPose s = fp(SCHNEE, u);
    float faellt = seg(w, 6.6f, 7.4f);
    s.armV = w > 3 && w < 6.6f ? 1.6f : 0.6f;
    s.mund = faellt > 0 ? 0.6f : 0;
    if (faellt < 1) frau(g, 160, 214, 0.9f, -1, s);
    else liegt(g, 166, 214, 0.8f, SCHNEE);
    if (kamm && w > 5.6f && faellt < 1) g.rundRechteck(150, 132, 12, 5, 2, FARBE(0x5A3A26));
    // die Zwerge kommen und helfen
    if (w > 10.4f) {
      for (int i = 0; i < 3; i++) {
        Pose z = mp(ZW[i], u);
        z.armV = 1.8f;
        zwerg(g, i, lerp(330 + i * 26, 214 + i * 22, sanft(seg(w, 10.4f, 12))), 214, 0.55f, -1, z, u);
      }
    }
    if (w > 12.6f && w < 15) glitzern(g, 160, 196, 24, u, 6, (uint8_t)(255 * (1 - seg(w, 14, 15))));
    if (!kamm) blase(g, 90, 40, {"Schöne Ware feil! feil!"}, aufzu(w, 0.4f, 3), kx + 10, 120);
    else blase(g, 90, 40, {"Gute Ware feil! feil!"}, aufzu(w, 0.4f, 3), kx + 10, 120);
    blase(g, 110, 50, {"Kind, wie du aussiehst!", "Komm, ich will dich", "einmal ordentlich schnüren."}, kamm ? 0 : aufzu(w, 3.2f, 6.6f), kx + 10, 120);
    blase(g, 110, 50, {"Nun will ich dich einmal", "ordentlich kämmen."}, kamm ? aufzu(w, 3.2f, 6.6f) : 0, kx + 10, 120);
    g.kameraAus();
    if (w > 6.6f && w < 9) g.text(SCHRIFT_K, kamm ? "Das Gift im Kamm wirkte." : "Sie schnürte so fest ...", 142, 230, OUT, 1, (uint8_t)(255 * aufzu(w, 6.6f, 9)));
  }
  blende(g, u, 40, 142, 150);
}

// 6) Der giftige Apfel: "Den roten Backen iss du, den weissen will ich essen."
static void szeneApfel(Gfx &g, float u) {
  if (u < 7) {
    spiegelSaal(g, u);
    // die Koenigin in ihrer heimlichen Kammer mit dem Apfel, dann verkleidet als Bauersfrau
    bool verwandelt = u > 4.6f;
    FrauPose k = fp(verwandelt ? BAUERSFRAU : KOENIGIN, u);
    k.boese = 1;
    k.armV = 1.7f;
    float hx, hy;
    frau(g, 142, 214, 1, 1, k, &hx, &hy);
    apfel(g, hx + 6, hy - 4, 8);
    g.kreis(hx + 6, hy - 4, 14, FARBE(0x6EE86E), (uint8_t)(80 * (1 - seg(u, 3, 4.6f))));
    puff(g, 142, 150, seg(u, 4.2f, 5.4f));
    g.kameraAus();
    g.text(SCHRIFT_K, "Sie machte einen giftigen Apfel.", 142, 230, WEISS, 1, (uint8_t)(255 * aufzu(u, 0.6f, 4.4f)));
  } else if (u < 30) {
    float v = u - 7;
    zwergHausAussen(g, u, 0);
    FrauPose b = fp(BAUERSFRAU, u);
    b.armV = v > 3 && v < 9 ? 1.8f : 1.2f;
    b.mund = v > 3.4f && v < 9 ? 0.5f : v > 14.4f && v < 20 ? 0.6f : 0;
    b.boese = v > 13 ? 1 : 0.2f;
    float bx = lerp(-30, 110, sanft(seg(v, 0, 3)));
    if (v > 0 && v < 3) b.lauf = fmodf(u * 1.6f, 1);
    float hx, hy;
    frau(g, bx, 214, 0.95f, 1, b, &hx, &hy);
    if (v < 6) apfel(g, hx + 6, hy - 4, 8);
    else if (v < 13) apfel(g, hx + 6, hy - 4, 8, 1, 1);  // isst die weisse Haelfte
    FrauPose s = fp(SCHNEE, u);
    float faellt = seg(v, 10.6f, 11.4f);
    s.armV = v > 6 ? 1.6f : 0.6f;
    s.mund = v > 9.4f && v < 10.6f ? 0.6f : 0;
    float shx, shy;
    if (faellt < 1) {
      frau(g, 170, 214, 0.9f, -1, s, &shx, &shy);
      if (v > 6) apfel(g, shx - 4, shy - 4, 7, 1, v > 9.4f ? 1 : 0);
    } else {
      liegt(g, 176, 214, 0.8f, SCHNEE);
      apfel(g, 120, 210, 5, 1, 1);
    }
    blase(g, 110, 40, {"Fürchtest du dich vor Gift?", "Den roten Backen iss du,", "den weißen will ich essen."},
          aufzu(v, 3.4f, 9.2f), bx + 10, 120);
    blase(g, 120, 40, {"Weiß wie Schnee, rot wie Blut,", "schwarz wie Ebenholz! Diesmal", "können dich die Zwerge", "nicht wieder erwecken."},
          aufzu(v, 14.4f, 21), bx + 10, 120);
  } else {
    float v = u - 30;
    spiegelSaal(g, u);
    float sprich = seg(v, 3, 4) * (1 - seg(v, 8, 9));
    spiegel(g, 200, 110, 1.1f, u, sprich, sprich > 0.5f ? fabsf(sinf(u * 9)) : 0);
    FrauPose k = fp(KOENIGIN, u);
    k.boese = 0.6f;
    k.mund = v > 0.4f && v < 2.8f ? 0.5f : 0;
    k.armV = v > 4 ? 2.2f : 1.6f;
    frau(g, 90, 214, 1.05f, 1, k);
    blase(g, 90, 40, {"Spieglein, Spieglein an der Wand,", "wer ist die Schönste im ganzen Land?"}, aufzu(v, 0.4f, 2.8f), 100, 120);
    blase(g, 150, 30, {"Frau Königin, Ihr seid", "die Schönste im Land."}, aufzu(v, 3.4f, 8.6f), 186, 80);
  }
  blende(g, u, 40, 150, 150);
}

// 7) Der glaeserne Sarg; der Koenigssohn; das giftige Apfelstueck faehrt aus dem Hals
static void szeneSarg(Gfx &g, float u) {
  if (u < 30) {
    berg(g, u, u < 10 ? 0.3f : 0);
    float sx = 150;
    float hop = 0, dx = 0;
    if (u > 22) {  // die Diener tragen ihn fort und stolpern
      float k = seg(u, 22, 27);
      dx = k * 80;
      hop = u > 24.6f && u < 25.4f ? sinf(seg(u, 24.6f, 25.4f) * 3.14159f) * 10 : 0;
      for (int i = 0; i < 2; i++) {
        Pose d = mp(DIENER, u);
        d.armV = 2.6f; d.lauf = fmodf(u * 1.6f + i * 0.5f, 1);
        schneiderlein(g, sx - 60 + i * 120 + dx, 214, 0.8f, 1, d);
      }
      laubbaum(g, 260, 214, 0.5f, FARBE(0x5DAE48));  // der Strauch
    }
    sarg(g, sx + dx, 196 - hop - (u > 22 ? 18 : 0), SCHNEE, 0);
    if (u > 25 && u < 26.4f) {
      float k = seg(u, 25, 26.4f);
      apfel(g, sx + dx - 30 + k * 30, 160 - huepf(k) * 40, 3, 1, 1);
    }
    if (u < 22) {
      // die Voegel beweinen sie: Eule, Rabe, Taeubchen
      eule(g, 70, 140, u);
      rabe(g, 230, 150, u);
      taube(g, 200, 120 + sinf(u * 2) * 4, 0.7f, -1, fmodf(u * 1.5f, 1));
      for (int i = 0; i < 3; i++) {
        Pose z = mp(ZW[i * 3], u);
        z.zu = true;
        zwerg(g, i * 3, 40 + i * 22, 214, 0.5f, 1, z, u);
      }
      // der Koenigssohn kommt
      if (u > 8) {
        Pose p = mp(PRINZ, u);
        float px = lerp(330, 246, sanft(seg(u, 8, 12)));
        p.lauf = u < 12 ? fmodf(u * 1.4f, 1) : -1;
        p.mund = (u > 12.4f && u < 15.6f) || (u > 18 && u < 21.6f) ? 0.5f : 0;
        p.armV = u > 18 ? 2.0f : 1.2f;
        schneiderlein(g, px, 214, 0.95f, -1, p);
      }
      blase(g, 200, 40, {"Lasst mir den Sarg, ich will euch", "geben, was ihr haben wollt."}, aufzu(u, 12.4f, 15.6f), 240, 130);
      blase(g, 90, 40, {"Wir geben ihn nicht", "um alles Gold in der Welt!"}, aufzu(u, 15.8f, 18), 60, 160);
      blase(g, 190, 40, {"So schenkt ihn mir, denn ich", "kann nicht leben, ohne", "Schneewittchen zu sehen."}, aufzu(u, 18.2f, 21.8f), 240, 130);
      g.kameraAus();
      g.text(SCHRIFT_K, "Sie sah aus, als wenn sie schliefe.", 142, 232, OUT, 1, (uint8_t)(255 * aufzu(u, 1, 7.6f)));
    }
    if (u > 25.2f && u < 25.6f) g.wackeln(sinf(u * 90) * 3, cosf(u * 80) * 2);
    if (u > 28) { g.kameraAus(); g.rechteck(0, 0, 284, 240, WEISS, (uint8_t)(255 * seg(u, 28, 30))); }
  } else {
    float v = u - 30;
    berg(g, u, 0);
    // der Deckel geht auf, sie richtet sich auf und lebt
    g.rechteck(84, 190, 112, 8, GOLD);
    FrauPose s = fp(SCHNEE, u);
    s.mund = v > 1.6f && v < 4.6f ? 0.6f : 0.2f;
    s.armV = 1.4f;
    frau(g, 120, 214, 0.95f, 1, s);
    Pose p = mp(PRINZ, u);
    p.mund = v > 4.8f && v < 11 ? 0.5f : 0.3f;
    p.armV = 1.6f;
    schneiderlein(g, 178, 214, 1, -1, p);
    if (v > 10) for (int i = 0; i < 5; i++) {
        float k = fmodf((v - 10) * 0.5f + i / 5.0f, 1);
        herz(g, 150 + sinf(k * 7 + i) * 24, 150 - k * 100, 5 + (i % 3), FARBE(0xFF5A7A), (uint8_t)(255 * huepf(k)));
      }
    blase(g, 100, 40, {"Ach Gott, wo bin ich?"}, aufzu(v, 1.6f, 4.6f), 118, 120);
    blase(g, 170, 36, {"Du bist bei mir.", "Ich habe dich lieber als", "alles auf der Welt!"}, aufzu(v, 4.8f, 11), 186, 120);
    g.kameraAus();
    if (v < 1) g.rechteck(0, 0, 284, 240, WEISS, (uint8_t)(255 * (1 - v)));
  }
  blende(g, u, 46, 142, 150);
}

// 8) Hochzeit; die boese Koenigin muss in den gluehenden Schuhen davontanzen
static void szeneHochzeit(Gfx &g, float u) {
  himmel(g, 0, u, 196);
  sonne(g, 244, 34, 12, u);
  schloss(g, 142, 196, u, 0);
  boden(g, 196);
  FrauPose b = fp(BRAUT, u);
  b.mund = 0.4f;
  frau(g, 124, 214, 0.92f, 1, b);
  Pose p = mp(PRINZ, u);
  p.mund = 0.4f;
  p.armV = 2.5f + sinf(u * 8) * 0.3f;
  schneiderlein(g, 160, 214, 0.95f, -1, p);
  for (int i = 0; i < 7; i++) {
    Pose z = mp(ZW[i], u);
    z.armV = 2.6f + sinf(u * 6 + i) * 0.3f;
    z.mund = 0.5f;
    float x = i < 4 ? 12 + i * 22 : 196 + (i - 4) * 26;
    zwerg(g, i, x, 220 - fabsf(sinf(u * 6 + i)) * 5, 0.5f, i < 4 ? 1 : -1, z, u);
  }
  // die boese Koenigin tanzt in den gluehenden Schuhen davon
  if (u > 8 && u < 20) {
    FrauPose k = fp(KOENIGIN, u);
    k.boese = 1;
    k.mund = 0.7f;
    k.armV = 2.4f + sinf(u * 12) * 0.5f;
    k.armH = 2.4f - sinf(u * 12) * 0.5f;
    k.drehen = sinf(u * 10) * 5;
    float kx = lerp(-40, 330, seg(u, 8, 20));
    float hop = fabsf(sinf(u * 10)) * 8;
    frau(g, kx, 238 - hop, 0.6f, 1, k);
    g.kreis(kx - 3, 236 - hop, 4, FARBE(0xFF5A2A));
    g.kreis(kx + 5, 236 - hop, 4, FARBE(0xFF5A2A));
  }
  for (int i = 0; i < 6; i++) {
    float k = fmodf(u * 0.45f + i / 6.0f, 1);
    herz(g, 142 + sinf(k * 7 + i) * 40, 150 - k * 110, 5 + (i % 3), FARBE(0xFF5A7A), (uint8_t)(255 * huepf(k)));
  }
  g.kameraAus();
  g.text(SCHRIFT_K, "Die Hochzeit war voller Pracht.", 142, 20, OUT, 1, (uint8_t)(255 * aufzu(u, 1, 7.6f)));
  g.text(SCHRIFT_K, "Die böse Königin musste tanzen ...", 142, 20, OUT, 1, (uint8_t)(255 * aufzu(u, 9, 18)));
  if (u > 28.5f) g.iris(142, 170, lerp(330, 0, sanft(seg(u, 28.5f, 30.5f))));
  if (u > 30.5f) {
    uint8_t a = (uint8_t)(255 * sanft(seg(u, 30.5f, 31.5f)));
    g.text(SCHRIFT_G, "Ende", 142, 122, GOLD, 1, a);
    for (int i = 0; i < 6; i++) {
      float an = u * 1.2f + i * 1.047f;
      g.stern(142 + cosf(an) * 60, 112 + sinf(an) * 30, 5, 2, 5, an, GOLD, a);
    }
  }
  if (u < 1.2f) g.iris(142, 150, lerp(0, 330, sanft(u / 1.2f)));
}

static void bild(Gfx &g, float t) {
  if (t < P2) szeneFenster(g, t - P1);
  else if (t < P3) szeneSpiegel(g, t - P2);
  else if (t < P4) szeneJaeger(g, t - P3);
  else if (t < P5) szeneZwerge(g, t - P4);
  else if (t < P6) szeneKraemerin(g, t - P5);
  else if (t < P7) szeneApfel(g, t - P6);
  else if (t < P8) szeneSarg(g, t - P7);
  else szeneHochzeit(g, t - P8);
}

// =====================================================================
// Musik
// =====================================================================
// Schneewittchens Lied (F-Dur, zart)
static const Ton3 MEL_SCHNEE[] = {
  {72, 1, 53}, {77, 1, 0}, {76, .5, 0}, {74, .5, 0}, {72, 1, 0}, {69, 1, 50}, {70, .5, 0}, {72, .5, 0}, {74, 2, 0},
  {72, 1, 46}, {74, .5, 0}, {76, .5, 0}, {77, 1, 0}, {79, 1, 0}, {81, 1, 48}, {79, .5, 0}, {76, .5, 0}, {77, 2, 53}};
// Zwergenmarsch (munter, G-Dur)
static const Ton3 MEL_ZWERGE[] = {
  {67, .5, 43}, {71, .5, 0}, {74, .5, 0}, {71, .5, 0}, {72, .5, 45}, {76, .5, 0}, {74, 1, 0},
  {71, .5, 43}, {74, .5, 0}, {79, .5, 0}, {78, .5, 0}, {76, .5, 45}, {74, .5, 0}, {67, 1, 43}};
// Spiegelzauber (d-Moll, geheimnisvoll)
static const Ton3 MEL_ZAUBER[] = {
  {62, 1, 38}, {65, 1, 0}, {69, 1, 0}, {68, 1, 0}, {69, 2, 41}, {74, 1, 0}, {73, 1, 0},
  {70, 1, 39}, {69, 1, 0}, {65, 1, 0}, {64, 1, 0}, {62, 2, 38}};

static void glas(Synth &s, float x) { for (int k = 0; k < 6; k++) s.glocke(x + k * 0.09f, 88 + k * 2, 0.35f); }

static void ton(Synth &s, float t0, float t1) {
  float a = P1;
  spieleMelodie(s, M(MEL_SCHNEE), a + 0.5f, a + 18, 0.42f, t0, t1, 0.75f);
  for (int i = 0; i < 3; i++) if (bei(a + 6.8f + i * 0.8f, t0, t1)) s.glocke(a + 6.8f + i * 0.8f, 84 - i * 3, 0.5f);  // Tropfen
  spieleMelodie(s, M(MEL_WIEGE), a + 20.5f, a + 29.5f, 0.42f, t0, t1, 0.7f);
  a = P2;
  spieleMelodie(s, M(MEL_ZAUBER), a + 0.5f, a + 25.5f, 0.42f, t0, t1, 0.8f);
  if (bei(a + 6, t0, t1)) glas(s, a + 6);
  if (bei(a + 16, t0, t1)) glas(s, a + 16);
  if (bei(a + 22.4f, t0, t1)) for (int k = 0; k < 8; k++) s.note(a + 22.4f + k * 0.06f, 52 + k * 3, 0.8f);  // Wut
  a = P3;
  spieleMelodie(s, M(MEL_SCHNEE), a + 0.4f, a + 4, 0.36f, t0, t1, 0.75f);
  spieleMelodie(s, M(MEL_SPUK), a + 4.2f, a + 13, 0.36f, t0, t1, 0.8f);
  spieleMelodie(s, M(MEL_JAGD), a + 13, a + 25, 0.2f, t0, t1, 0.8f);
  spieleMelodie(s, M(MEL_SCHNEE), a + 25, a + 29.5f, 0.36f, t0, t1, 0.75f);
  a = P4;
  for (int i = 0; i < 7; i++) if (bei(a + 1 + i * 0.86f, t0, t1)) s.note(a + 1 + i * 0.86f, 72 + i * 2, 0.4f);  // je ein Bissen
  if (bei(a + 12, t0, t1)) spieleMelodie(s, M(MEL_WIEGE), a + 12, a + 14, 0.4f, t0, t1, 0.6f);
  spieleMelodie(s, M(MEL_ZWERGE), a + 14, a + 18, 0.22f, t0, t1, 0.85f);
  for (int i = 0; i < 7; i++) if (bei(a + 18 + i * 1.6f, t0, t1)) for (int k = 0; k < 3; k++) s.note(a + 18 + i * 1.6f + k * 0.12f, 76 + i + k * 2, 0.5f);
  if (bei(a + 29.4f, t0, t1)) for (int k = 0; k < 10; k++) s.glocke(a + 29.4f + k * 0.15f, 76 + (k % 5) * 2, 0.4f);
  spieleMelodie(s, M(MEL_ZWERGE), a + 34, a + 53.5f, 0.22f, t0, t1, 0.8f);
  a = P5;
  spieleMelodie(s, M(MEL_ZAUBER), a + 0.4f, a + 7, 0.4f, t0, t1, 0.8f);
  if (bei(a + 7.4f, t0, t1)) for (int k = 0; k < 10; k++) s.glocke(a + 7.4f + k * 0.07f, 60 + k * 3, 0.5f);  // Verwandlung
  for (int r = 0; r < 2; r++) {
    float b = a + 10 + r * 15;
    spieleMelodie(s, M(MEL_SPUK), b + 0.2f, b + 6.6f, 0.3f, t0, t1, 0.75f);
    if (bei(b + 6.8f, t0, t1)) for (int k = 0; k < 8; k++) s.note(b + 6.8f + k * 0.1f, 72 - k * 3, 0.6f);  // sie faellt
    if (bei(b + 12.6f, t0, t1)) for (int k = 0; k < 8; k++) s.glocke(b + 12.6f + k * 0.12f, 72 + k * 2, 0.5f);  // sie lebt
  }
  a = P6;
  spieleMelodie(s, M(MEL_ZAUBER), a + 0.4f, a + 6.6f, 0.36f, t0, t1, 0.8f);
  if (bei(a + 4.6f, t0, t1)) for (int k = 0; k < 10; k++) s.glocke(a + 4.6f + k * 0.07f, 60 + k * 3, 0.5f);
  spieleMelodie(s, M(MEL_SPUK), a + 7.4f, a + 17.6f, 0.32f, t0, t1, 0.8f);
  if (bei(a + 16.4f, t0, t1)) s.ton(a + 16.4f, 900, 0.06f, 0.002f, 0.05f);  // Biss
  if (bei(a + 17.6f, t0, t1)) for (int k = 0; k < 10; k++) s.note(a + 17.6f + k * 0.12f, 76 - k * 3, 0.7f);
  spieleMelodie(s, M(MEL_ZAUBER), a + 21, a + 39.5f, 0.42f, t0, t1, 0.8f);
  if (bei(a + 33.4f, t0, t1)) glas(s, a + 33.4f);
  a = P7;
  spieleMelodie(s, M(MEL_WIEGE), a + 0.4f, a + 8, 0.5f, t0, t1, 0.7f);
  spieleMelodie(s, M(MEL_SCHNEE), a + 8.4f, a + 22, 0.36f, t0, t1, 0.8f);
  for (float x = ceilf(t0 / 0.4f) * 0.4f; x < t1; x += 0.4f)
    if (x > a + 22 && x < a + 24.6f) s.rauschen(x, 0.04f, TIEFPASS, 500, 0.06f);  // Schritte
  if (bei(a + 25, t0, t1)) { s.ton(a + 25, 110, 0.3f, 0.002f, 0.25f, SINUS, GERAEUSCH, 55); s.note(a + 25.3f, 84, 0.6f); }
  if (bei(a + 30.2f, t0, t1)) for (int k = 0; k < 12; k++) s.glocke(a + 30.2f + k * 0.15f, 72 + (k % 6) * 2, 0.5f);
  spieleMelodie(s, M(MEL_F), a + 32, a + 45.5f, 0.28f, t0, t1, 0.85f);
  a = P8;
  spieleMelodie(s, M(MEL_F), a + 0.4f, a + 8, 0.26f, t0, t1, 0.9f);
  spieleMelodie(s, M(MEL_ZWERGE), a + 8, a + 20, 0.16f, t0, t1, 0.8f);
  spieleMelodie(s, M(MEL_F), a + 20, a + 30.5f, 0.26f, t0, t1, 0.9f);
  if (bei(a + 30.6f, t0, t1)) { s.glocke(a + 30.6f, 72); s.glocke(a + 30.9f, 76); s.glocke(a + 31.2f, 79); s.glocke(a + 31.6f, 84); }
}

// =====================================================================
// Startbild und Vorschau
// =====================================================================
static void titelSzene(Gfx &g, float t) {
  wald(g, t, 0);
  g.kontur(1.8f, OUT);
  g.rechteck(176, 140, 90, 60, FARBE(0xE8D0A0));
  float d[] = {166, 142, 221, 104, 276, 142};
  g.polygon(d, 3, FARBE(0xB05A3A));
  g.konturAus();
  g.rundRechteck(212, 166, 20, 34, 9, HOLZ);
  FrauPose s = fp(SCHNEE, t);
  s.armV = 1.6f;
  float hx, hy;
  frau(g, 150, 222, 1, 1, s, &hx, &hy);
  apfel(g, hx + 4, hy - 4, 7);
  for (int i = 0; i < 4; i++) {
    Pose z = mp(ZW[i], t);
    z.armV = 2.4f + sinf(t * 3 + i) * 0.3f;
    zwerg(g, i, 188 + i * 24, 226 - fabsf(sinf(t * 4 + i)) * 3, 0.5f, -1, z, t);
  }
}

static void titelbild(Gfx &g, float t) {
  titelSzene(g, t);
  g.rundRechteck(8, 20, 212, 50, 14, FARBE(0x1E1A3A), 160);
  g.text(SCHRIFT_G, "Schneewittchen", 114, 55, GOLD, 1);
  float b = 1 + sinf(t * 4) * 0.08f;
  g.kontur(2, OUT);
  g.kreis(88, 96, 14 * b, FARBE(0xE8473D));
  g.konturAus();
  float tri[] = {83, 88, 83, 104, 96, 96};
  g.polygon(tri, 3, WEISS);
}

static void vorschau(Gfx &g, float t) { titelSzene(g, t); }

static void symbol(Gfx &g, float x, float y, float t) {
  apfel(g, x, y - 34 - fabsf(sinf(t * 2)) * 4, 22);
  glitzern(g, x, y - 34, 30, t, 4, 200);
  for (int i = 0; i < 2; i++) {
    Pose z = mp(ZW[i * 3], t);
    z.armV = 2.4f + sinf(t * 3 + i) * 0.3f;
    zwerg(g, i * 3, x + (i ? 40 : -40), y, 0.45f, i ? -1 : 1, z, t);
  }
}

const Maerchen SCHNEEWITTCHEN = {"Schneewittchen", PDAUER, FARBE(0xD9343F), bild, ton, titelbild, vorschau, symbol, LIED(MEL_SCHNEE, 0.4f)};

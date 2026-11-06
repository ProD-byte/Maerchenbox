// Daeumelinchen - ein Spieluhr-Film fuer die Maerchenbox, nach dem Maerchen von Hans Christian Andersen.
// Ein Maedchen, nicht groesser als ein Daumen, aus einer Tulpe. Die Kroete holt sie, die Fische befreien sie,
// den Winter verbringt sie bei der Feldmaus, rettet die Schwalbe - und die traegt sie ins Land der Blumen.
#include "erzaehlen.h"

// ---------- Ablauf (Sekunden) ----------
static const float P1 = 0, P2 = 28, P3 = 52, P4 = 80, P5 = 106, P6 = 134, P7 = 160, PDAUER = 190;

static FrauLook daumLook(bool fluegel) {
  FrauLook l; l.kleid = FARBE(0xFF9AC0); l.haar = FARBE(0xF6D04A); l.blume = FARBE(0xFFFFFF); l.glanz = fluegel; l.krone = fluegel;
  return l;
}
static FrauLook mutterLook() { FrauLook l; l.kleid = FARBE(0x7A8EC8); l.schuerze = WEISS; l.haar = FARBE(0x7A4524); l.frisur = 2; return l; }
static FrauLook hexeLook() { FrauLook l; l.kleid = FARBE(0x5A3A6E); l.haar = FARBE(0xC8C8C8); l.frisur = 3; l.tuch = FARBE(0x3A2A4A); return l; }
static const FrauLook DAUM = daumLook(false), FEE = daumLook(true), MUTTER = mutterLook(), HEXE = hexeLook();
static Aussehen prinzLook() { Aussehen a; a.weste = WEISS; a.hemd = FARBE(0xBFE6FF); a.hose = FARBE(0xE8E4F4); a.haar = FARBE(0xE0A23C); a.muetze = 3; return a; }
static const Aussehen BLUMENPRINZ = prinzLook();

// ---------- Figuren ----------
static void fluegel(Gfx &g, float x, float y, float s, float t) {
  float w = sinf(t * 10) * 0.3f;
  for (int i = -1; i <= 1; i += 2) {
    g.ellipse(x + i * 10 * s, y - 6 * s, 10 * s, 18 * s, FARBE(0xE6F4FF), 170, i * (0.5f + w));
    g.ellipse(x + i * 8 * s, y + 8 * s, 6 * s, 10 * s, FARBE(0xE6F4FF), 150, i * (1.2f + w));
  }
}

static void tulpe(Gfx &g, float x, float y, float s, float auf, Farbe c = FARBE(0xE8473D)) {
  g.linie(x, y, x, y - 70 * s, 4 * s, FARBE(0x3E9A4E));
  g.ellipse(x - 14 * s, y - 30 * s, 16 * s, 5 * s, FARBE(0x4CB85A), 255, -0.7f);
  float o = auf * 0.7f;
  g.kontur(1.2f, OUT);
  for (int i = -1; i <= 1; i++) g.ellipse(x + i * (8 + o * 12) * s, y - 86 * s, 10 * s, 20 * s, i ? c : mischeFarbe(c, WEISS, 0.15f), 255, i * (0.25f + o));
  g.konturAus();
  if (auf > 0.3f) g.ellipse(x, y - 76 * s, 12 * s * auf, 4 * s, FARBE(0xFFE070));
}

static void walnuss(Gfx &g, float x, float y, float s, bool schlaeft, float t) {
  g.kontur(1.2f, OUT);
  g.ellipse(x, y - 6 * s, 22 * s, 10 * s, FARBE(0xA8702E));
  g.konturAus();
  g.ellipse(x, y - 12 * s, 18 * s, 4 * s, FARBE(0x6E4A2A));
  if (schlaeft) {
    g.rundRechteck(x - 10 * s, y - 16 * s, 22 * s, 6 * s, 3 * s, FARBE(0xFF8FC0));  // Rosenblatt als Decke
    g.kreis(x - 12 * s, y - 15 * s, 4 * s, HAUT);
    g.ellipse(x - 14 * s, y - 17 * s, 4 * s, 3 * s, DAUM.haar);
    zzz(g, x - 6 * s, y - 30 * s, t);
  }
}

static void kroete(Gfx &g, float x, float y, float s, float dir, float t, float mund = 0, float sprung = 0) {
  float h = sprung > 0 ? huepf(sprung) * 30 * s : 0;
  auto PX = [&](float d) { return x + d * s * dir; };
  const Farbe f = FARBE(0x8E8A4A), d = FARBE(0x6E6A3A);
  g.kontur(1.2f * s, OUT);
  g.ellipse(PX(0), y - 12 * s - h, 22 * s, 13 * s, f);
  g.kreis(PX(-9), y - 24 * s - h, 6 * s, f);
  g.kreis(PX(10), y - 24 * s - h, 6 * s, f);
  g.konturAus();
  for (int i = 0; i < 6; i++) g.kreis(PX(-14 + i * 6), y - 14 * s - h + (i & 1) * 4 * s, 2 * s, d);  // Warzen
  for (int i = 0; i < 2; i++) { float ex = PX(i ? 10 : -9); g.kreis(ex, y - 25 * s - h, 3.4f * s, FARBE(0xF2C230)); g.ellipse(ex, y - 25 * s - h, 2.4f * s, 1 * s, SCHWARZ); }
  if (mund > 0.05f) g.ellipse(PX(2), y - 12 * s - h, 9 * s, (1 + mund * 3) * s, FARBE(0x8A2A2A));
  else g.bogen(PX(2), y - 15 * s - h, 11 * s, 0.4f, 2.7f, 1.4f * s, OUT);
  g.ellipse(PX(-18), y - 2 * s - h, 8 * s, 3 * s, d);
  g.ellipse(PX(18), y - 2 * s - h, 8 * s, 3 * s, d);
}

static void seerose(Gfx &g, float x, float y, float s) {
  g.ellipse(x, y, 34 * s, 9 * s, FARBE(0x4CA85A));
  g.ellipse(x + 6 * s, y - 1 * s, 26 * s, 6 * s, FARBE(0x6EC06A));
  float sp[] = {x - 34 * s, y, x - 20 * s, y - 2 * s, x - 22 * s, y + 4 * s};
  g.polygon(sp, 3, FARBE(0x5BA8E0));
}

static void schmetterling(Gfx &g, float x, float y, float s, float t) {
  float w = fabsf(sinf(t * 9));
  for (int i = -1; i <= 1; i += 2) {
    g.ellipse(x + i * 7 * s * w, y - 4 * s, 7 * s * w + 1, 8 * s, FARBE(0xFFFFFF));
    g.ellipse(x + i * 5 * s * w, y + 5 * s, 5 * s * w + 1, 5 * s, FARBE(0xF6F2FA));
  }
  g.ellipse(x, y, 1.6f * s, 7 * s, OUT);
}

static void feldmaus(Gfx &g, float x, float y, float s, float dir, float t, float mund = 0, float armV = 0.6f) {
  auto PX = [&](float d) { return x + d * s * dir; };
  auto PY = [&](float d) { return y + d * s; };
  const Farbe fell = FARBE(0xA89A8A);
  g.kurve(PX(-12), PY(-10), PX(-26), PY(-8), PX(-30), PY(-20), 2 * s, FARBE(0xC9A0A0));
  g.kontur(1.2f * s, OUT);
  g.ellipse(PX(0), PY(-22), 14 * s, 20 * s, fell);
  g.konturAus();
  g.ellipse(PX(1), PY(-16), 10 * s, 12 * s, WEISS);  // Schuerze
  g.linie(PX(8), PY(-30), PX(8 + sinf(armV) * 12), PY(-30 + cosf(armV) * 12), 4 * s, fell);
  g.kontur(1.2f * s, OUT);
  g.kreis(PX(-4), PY(-54), 7 * s, fell);
  g.kreis(PX(8), PY(-54), 7 * s, fell);
  g.kreis(PX(2), PY(-44), 11 * s, fell);
  g.konturAus();
  g.kreis(PX(-4), PY(-54), 4 * s, FARBE(0xE8C0C0));
  g.kreis(PX(8), PY(-54), 4 * s, FARBE(0xE8C0C0));
  g.kreis(PX(12), PY(-42), 2 * s, FARBE(0xFF8CA0));
  g.kreis(PX(5), PY(-46), 1.8f * s, SCHWARZ);
  if (mund > 0.05f) g.ellipse(PX(9), PY(-38), 2 * s, mund * 2.4f * s, FARBE(0x8A2A2A));
  g.ellipse(PX(2), PY(-56), 9 * s, 4 * s, FARBE(0xE8473D));  // Haeubchen
}

static void maulwurf(Gfx &g, float x, float y, float s, float dir, float t, float mund = 0) {
  auto PX = [&](float d) { return x + d * s * dir; };
  auto PY = [&](float d) { return y + d * s; };
  const Farbe pelz = FARBE(0x2E2A36);
  g.kontur(1.2f * s, OUT);
  g.ellipse(PX(0), PY(-24), 20 * s, 24 * s, pelz);
  g.kreis(PX(8), PY(-48), 13 * s, pelz);
  g.konturAus();
  g.ellipse(PX(20), PY(-44), 6 * s, 4 * s, FARBE(0xF2A6B0));
  for (int i = 0; i < 2; i++) g.ellipse(PX(14 + i * 4), PY(-12 + i * 2), 6 * s, 3 * s, FARBE(0xF2A6B0));  // Schaufelhaende
  // Brille und schwarzer Samtrock mit Knoepfen
  g.bogen(PX(10), PY(-50), 4 * s, 0, 6.28f, 1.2f * s, GOLD);
  for (int i = 0; i < 3; i++) g.kreis(PX(6), PY(-34 + i * 8), 1.6f * s, GOLD);
  if (mund > 0.05f) g.ellipse(PX(16), PY(-38), 2 * s, mund * 2 * s, FARBE(0x8A2A2A));
}

static void schwalbe(Gfx &g, float x, float y, float s, float dir, float t, float flug = -1, bool liegt = false) {
  auto PX = [&](float d) { return x + d * s * dir; };
  auto PY = [&](float d) { return y + d * s; };
  const Farbe blau = FARBE(0x2E3A6E);
  float w = flug >= 0 ? sinf(flug * 6.28f) : 0;
  if (liegt) {
    g.ellipse(PX(0), PY(-6), 22 * s, 7 * s, blau);
    g.ellipse(PX(4), PY(-4), 12 * s, 4 * s, WEISS);
    g.kreis(PX(20), PY(-6), 6 * s, blau);
    g.bogen(PX(22), PY(-7), 2 * s, 3.5f, 5.9f, 1 * s, WEISS);
    return;
  }
  float sw[] = {PX(-16), PY(-2), PX(-34), PY(-8), PX(-24), PY(2), PX(-34), PY(6)};
  g.polygon(sw, 4, blau);
  g.kontur(1.1f * s, OUT);
  g.ellipse(PX(0), PY(0), 20 * s, 8 * s, blau);
  g.konturAus();
  g.ellipse(PX(4), PY(3), 12 * s, 4 * s, WEISS);
  float fl[] = {PX(-4), PY(-4), PX(10), PY(-6), PX(-14), PY(-30 * w - 4)};
  g.polygon(fl, 3, FARBE(0x3E4A8E));
  g.kreis(PX(18), PY(-3), 7 * s, blau);
  g.kreis(PX(20), PY(0), 3 * s, FARBE(0xC0392B));
  float sn[] = {PX(24), PY(-4), PX(30), PY(-2), PX(24), PY(-1)};
  g.polygon(sn, 3, OUT);
  g.kreis(PX(20), PY(-5), 1.4f * s, WEISS);
  g.kreis(PX(20.5f), PY(-5), 0.8f * s, SCHWARZ);
}

// Daeumelinchen; s = 1 ist in Nahaufnahme etwa 50 px gross
static void daum(Gfx &g, float x, float y, float s, float dir, float t, float mund = 0, float armV = 0.4f, bool weint = false,
                 bool fee = false, float lauf = -1) {
  if (fee) fluegel(g, x - 2 * s * dir, y - 34 * s, s, t);
  FrauPose p = fposis(fee ? FEE : DAUM, t);
  p.mund = mund; p.armV = armV; p.weint = weint; p.zeit = t; p.lauf = lauf;
  frau(g, x, y, 0.52f * s, dir, p);
}

static void wiese(Gfx &g, float t, float winter = 0) {
  himmel(g, winter * 0.3f, t, 200);
  if (winter < 0.5f) sonne(g, 250, 30, 11, t);
  // riesige Graeser und Blumen (alles ist gross neben Daeumelinchen)
  for (int i = 0; i < 6; i++) {
    float x = 20 + i * 52;
    g.kurve(x, 240, x + 8, 150, x + 20 + sinf(t + i) * 4, 70, 5, mischeFarbe(FARBE(0x4CB85A), FARBE(0xB0A890), winter));
  }
  g.rechteck(0, 214, 284, 26, mischeFarbe(GRAS, WEISS, winter));
  if (winter < 0.5f) for (int i = 0; i < 4; i++) {
    float x = 40 + i * 70;
    g.linie(x, 214, x, 150, 3, FARBE(0x3E9A4E));
    for (int k = 0; k < 6; k++) g.ellipse(x + cosf(k * 1.047f) * 10, 140 + sinf(k * 1.047f) * 10, 7, 4, WEISS, 255, k * 1.047f);
    g.kreis(x, 140, 5, FARBE(0xFFD54A));
  }
}

// =====================================================================
// Szenen
// =====================================================================
// 1) Das Gerstenkorn, die Tulpe - und darin ein winziges Maedchen
static void szeneTulpe(Gfx &g, float u) {
  if (u < 10) {
    g.verlauf(0, 200, FARBE(0xF2E2C8), FARBE(0xE0C8A8));
    g.rechteck(0, 200, 284, 40, FARBE(0xA8805E));
    FrauPose m = fposis(MUTTER, u);
    m.armV = u > 5 ? 1.6f : 0.6f;
    m.mund = u > 1 && u < 4.4f ? 0.5f : 0;
    frau(g, 90, 214, 0.95f, 1, m);
    FrauPose h = fposis(HEXE, u);
    h.armV = u > 5 ? 1.6f : 0.4f;
    h.mund = u > 5 && u < 9 ? 0.4f : 0;
    frau(g, 196, 214, 0.9f, -1, h);
    if (u > 5) { g.ellipse(146, 148, 4, 2.5f, FARBE(0xE8C860)); glitzern(g, 146, 148, 12, u, 4); }
    g.kameraAus();
    blase(g, 100, 60, {"Ach, hätte ich doch", "ein ganz kleines Kind!"}, aufzu(u, 0.6f, 4.6f), 96, 130);
    blase(g, 190, 60, {"Hier hast du ein Gerstenkorn.", "Leg es in einen Blumentopf."}, aufzu(u, 5, 9.6f), 196, 130);
    return;
  }
  float v = u - 10;
  g.verlauf(0, 240, FARBE(0xF2E2C8), FARBE(0xE0C8A8));
  // Nahaufnahme: Blumentopf auf dem Fensterbrett
  g.rechteck(0, 210, 284, 30, FARBE(0xC9A46A));
  g.kontur(1.4f, OUT);
  float tp[] = {100, 170, 184, 170, 174, 214, 110, 214};
  g.polygon(tp, 4, FARBE(0xC0632E));
  g.konturAus();
  float wachs = seg(v, 0, 3);
  float auf = seg(v, 4, 6);
  g.ansicht(142 - 142 * (0.5f + wachs * 0.5f), 170 - 170 * (0.5f + wachs * 0.5f), 0.5f + wachs * 0.5f);
  tulpe(g, 142, 170, 0.85f, auf);
  g.kameraAus();
  if (auf > 0.5f && v < 8.4f) {
    daum(g, 142, 106, 0.6f, 1, u, 0.4f, 1.4f + sinf(u * 2) * 0.4f);
    glitzern(g, 142, 86, 30, u, 5, (uint8_t)(255 * seg(v, 5, 6)));
  }
  if (v > 8) walnuss(g, 230, 214, 1.4f, true, u);
  g.kameraAus();
  vers(g, {"Daraus wuchs eine Tulpe.", "Die Mutter küsste sie - da ging", "sie auf, und darin saß ein Mädchen."}, aufzu(v, 0.4f, 8));
  vers(g, {"Nicht größer als ein Daumen -", "darum hieß sie Däumelinchen.", "Ihr Bett war eine Walnussschale."}, aufzu(v, 8.4f, 17.6f));
  blende(g, u, 28, 142, 120);
}

// 2) Die Kroete holt sie und setzt sie auf ein Seerosenblatt
static void szeneKroete(Gfx &g, float u) {
  if (u < 9) {
    g.verlauf(0, 240, FARBE(0x2A2E5A), FARBE(0x1E2240));
    g.rechteck(0, 210, 284, 30, FARBE(0x5A4A3A));
    g.rundRechteck(170, 40, 90, 100, 6, FARBE(0x101830));
    mond(g, 220, 76, 12);
    if (u < 5) {
      walnuss(g, 110, 214, 1.4f, true, u);
      float k = seg(u, 1, 3.6f);
      kroete(g, lerp(220, 150, k), lerp(140, 214, k) - (k < 1 ? huepf(k) * 40 : 0), 1.4f, -1, u, 0.3f);
    } else {  // mit der Nuss davon
      float w = seg(u, 5, 8.6f);
      float kx = lerp(140, 300, w), ky = 214 - huepf(fmodf(w * 3, 1)) * 20;
      kroete(g, kx, ky, 1.4f, 1, u, 0.2f);
      walnuss(g, kx - 4, ky - 30, 1.1f, true, u);
    }
    g.kameraAus();
    blase(g, 150, 40, {"Das wäre eine schöne", "Frau für meinen Sohn!"}, aufzu(u, 3.6f, 8), 140, 150);
    return;
  }
  float v = u - 9;
  himmel(g, 0, u, 160);
  sonne(g, 250, 30, 11, u);
  g.rechteck(0, 150, 284, 90, FARBE(0x5BA8E0));
  for (int i = 0; i < 5; i++) g.linie(fmodf(i * 70 + u * 10, 320) - 20, 170 + i * 14, fmodf(i * 70 + u * 10, 320) + 10, 170 + i * 14, 1.4f, FARBE(0x8ECCF0));
  for (int i = 0; i < 3; i++) g.kurve(10 + i * 30, 240, 14 + i * 30, 160, 20 + i * 30, 100, 4, FARBE(0x4CA85A));  // Schilf
  seerose(g, 160, 196, 1.3f);
  daum(g, 166, 194, 1.2f, -1, u, 0, 0.4f, v > 4);
  kroete(g, 76, 210, 1.2f, 1, u, v > 4 && v < 10 ? 0.5f : 0.2f);
  kroete(g, 112, 214, 1.0f, 1, u, v > 10 && v < 13 ? 0.7f : 0.1f);  // der Sohn
  g.kameraAus();
  vers(g, {"Sie setzte sie auf ein Seerosen-", "blatt mitten im Bach."}, aufzu(v, 0.4f, 4));
  blase(g, 80, 60, {"Hier wohnst du nun mit", "meinem Sohn im Schlamm!"}, aufzu(v, 4.4f, 10), 82, 180);
  blase(g, 120, 90, {"Koax, koax, brekekekex!"}, aufzu(v, 10.4f, 13.6f), 116, 190);
  blende(g, u, 24, 160, 180);
}

// 3) Die Fische nagen den Stiel durch, der Schmetterling zieht sie fort
static void szeneFische(Gfx &g, float u) {
  himmel(g, 0, u, 150);
  sonne(g, 250, 30, 11, u);
  g.rechteck(0, 140, 284, 100, FARBE(0x5BA8E0));
  float fort = seg(u, 9, 26);
  float bx = lerp(150, 230, sanft(fort));
  float sc = fort * 400;
  for (int i = 0; i < 4; i++) laubbaum(g, fmodf(i * 90 - sc + 3600, 360) - 40, 140, 0.5f);
  for (int i = 0; i < 6; i++) g.linie(fmodf(i * 60 + u * 10 + sc, 320) - 20, 160 + i * 12, fmodf(i * 60 + u * 10 + sc, 320) + 10, 160 + i * 12, 1.4f, FARBE(0x8ECCF0));
  // Stiel und Fische
  if (u < 9) {
    float st = seg(u, 4, 8.6f);
    g.linie(150, 196, 150, 240, 3, FARBE(0x3E8A4E));
    if (st > 0.9f) g.rechteck(146, 214, 8, 6, FARBE(0x5BA8E0));
    for (int i = 0; i < 3; i++) fisch(g, 150 + cosf(u * 2 + i * 2.1f) * 18, 220 + sinf(u * 3 + i) * 6, 1.1f, u * 2 + i * 2.1f);
  }
  seerose(g, bx, 196, 1.3f);
  daum(g, bx + 6, 194, 1.2f, 1, u, u > 10 ? 0.4f : 0, u > 12 ? 2.4f : 0.4f, u < 4);
  if (u > 10) {
    float sx = bx + 70, sy = 120 + sinf(u * 2) * 8;
    schmetterling(g, sx, sy, 1.4f, u);
    g.linie(bx + 12, 168, sx - 4, sy + 4, 1, FARBE(0xFF8FC0));  // ihr Guertelband
  }
  g.kameraAus();
  vers(g, {"Die kleinen Fische sahen sie", "weinen und nagten den Stiel durch."}, aufzu(u, 0.4f, 9));
  vers(g, {"Ein weißer Schmetterling zog sie", "an ihrem Gürtelband weit fort."}, aufzu(u, 11, 20));
  vers(g, {"Den ganzen Sommer lebte sie", "allein im großen Wald."}, aufzu(u, 20.4f, 27.6f));
  blende(g, u, 28, 180, 160);
}

// 4) Der Winter - die Feldmaus nimmt sie auf, der Maulwurf kommt zu Besuch
static void szeneWinter(Gfx &g, float u) {
  if (u < 10) {
    wiese(g, u, 1);
    for (int i = 0; i < 30; i++) {
      float x = fmodf(zufall(i) * 284 + sinf(u + i) * 10, 284), y = fmodf(zufall(i + 40) * 240 + u * 30, 240);
      g.kreis(x, y, 2 + (i % 3), WEISS);
    }
    daum(g, lerp(30, 180, seg(u, 0, 8)), 222, 1.4f, 1, u, 0, 2.6f, false, false, fmodf(u * 0.8f, 1));
    // Tuer der Feldmaus im Boden
    g.kontur(1.2f, OUT);
    g.rundRechteck(210, 196, 34, 26, 12, HOLZ);
    g.konturAus();
    g.kameraAus();
    vers(g, {"Dann kam der Winter. Sie fror", "so sehr und hatte Hunger."}, aufzu(u, 0.4f, 9.6f));
    return;
  }
  float v = u - 10;
  g.verlauf(0, 240, FARBE(0xC9A46A), FARBE(0x9E7656));
  g.rechteck(0, 214, 284, 26, FARBE(0x7E5A3A));
  for (int i = 0; i < 6; i++) g.ellipse(40 + i * 40, 60 + (i & 1) * 10, 10, 6, FARBE(0xE8C860));  // Kornvorrat
  feldmaus(g, 80, 222, 1.5f, 1, u, v < 6 ? 0.5f : 0.1f, 1.6f);
  daum(g, 150, 222, 1.4f, -1, u, 0, 0.6f, v > 12);
  if (v > 6) maulwurf(g, lerp(320, 220, seg(v, 6, 9)), 222, 1.4f, -1, u, v > 9 && v < 13 ? 0.6f : 0);
  g.kameraAus();
  blase(g, 100, 50, {"Du armes Ding! Komm herein", "und bleib den Winter bei mir."}, aufzu(v, 0.4f, 6), 96, 140);
  vers(g, {"Der reiche Nachbar Maulwurf", "kam zu Besuch. Er wollte", "Däumelinchen heiraten."}, aufzu(v, 7, 13.6f));
  vers(g, {"Doch er mochte keine Sonne", "und keine Blumen."}, aufzu(v, 14, 15.8f));
}

// 5) Die Schwalbe im Gang - Daeumelinchen waermt sie, sie wird gesund
static void szeneSchwalbe(Gfx &g, float u) {
  g.verlauf(0, 240, FARBE(0x5A4030), FARBE(0x3A2A20));
  g.ellipse(142, 150, 170, 70, FARBE(0x7E5A3A));
  bool lebt = u > 12;
  float hoch = seg(u, 20, 22);
  if (!lebt) schwalbe(g, 150, 196, 1.6f, 1, u, -1, true);
  else schwalbe(g, lerp(150, 330, hinein(seg(u, 22, 26))), 190 - hoch * 60, 1.6f, 1, u, hoch > 0 ? fmodf(u * 3, 1) : -1);
  if (u > 4 && u < 20) {
    g.rundRechteck(126, 182, 46, 12, 5, FARBE(0xE8D8B8), (uint8_t)(255 * seg(u, 4, 5)));  // Heudecke
  }
  if (u > 20) { g.rechteck(120, 70, 50, 30, FARBE(0x9FD8FF)); sonne(g, 145, 80, 8, u); }  // Loch zur Sonne
  daum(g, 110, 204, 1.4f, 1, u, lebt ? 0.5f : 0.1f, u > 4 && u < 9 ? 1.8f : 0.6f, u < 4);
  if (u > 7 && u < 12) {  // sie lauscht: das Herz klopft
    float k = fmodf(u, 0.8f) < 0.3f;
    herz(g, 158, 170, 5 + k * 2, FARBE(0xFF5A7A), 220);
  }
  g.kameraAus();
  vers(g, {"Im Gang lag eine Schwalbe,", "wie erfroren."}, aufzu(u, 0.4f, 4));
  vers(g, {"Däumelinchen deckte sie zu", "und legte ihr Köpfchen auf", "ihre Brust - das Herz klopfte!"}, aufzu(u, 4.4f, 12));
  blase(g, 200, 60, {"Danke, liebes Kind!", "Komm mit mir in die Sonne!"}, aufzu(u, 13, 18.6f), 168, 180);
  vers(g, {"Aber sie blieb bei der Feldmaus.", "Im Frühling flog die Schwalbe fort."}, aufzu(u, 19, 27.6f));
  blende(g, u, 28, 142, 160);
}

// 6) Die Hochzeit mit dem Maulwurf droht - die Schwalbe kommt zurueck und traegt sie fort
static void szeneFlug(Gfx &g, float u) {
  if (u < 12) {
    wiese(g, u, 0);
    g.rechteck(0, 214, 284, 26, FARBE(0xC9A040));  // Stoppelfeld im Herbst
    daum(g, 120, 222, 1.4f, 1, u, 0.3f, 2.4f, u < 6);
    if (u > 5) schwalbe(g, lerp(330, 180, seg(u, 5, 8)), lerp(60, 160, seg(u, 5, 8)), 1.6f, -1, u, u < 8 ? fmodf(u * 3, 1) : -1);
    g.kameraAus();
    vers(g, {"Im Herbst sollte Hochzeit sein.", "Leb wohl, liebe Sonne, sagte sie."}, aufzu(u, 0.4f, 5.6f));
    blase(g, 190, 60, {"Komm mit mir in die warmen", "Länder - setz dich auf meinen Rücken!"}, aufzu(u, 6, 11.6f), 196, 150);
    return;
  }
  float v = u - 12;
  himmel(g, 0, u, 240);
  sonne(g, 250, 30, 11, u);
  for (int i = 0; i < 3; i++) wolke(g, fmodf(i * 120 - v * 60 + 900, 400) - 60, 60 + i * 30, 0.9f);
  // Berge mit Schnee, unten
  for (int i = 0; i < 4; i++) {
    float x = fmodf(i * 100 - v * 30 + 800, 400) - 60;
    float b[] = {x, 240, x + 50, 170, x + 100, 240};
    g.polygon(b, 3, FARBE(0x8E86B8));
    float sn[] = {x + 36, 186, x + 50, 170, x + 64, 186};
    g.polygon(sn, 3, WEISS);
  }
  float sy = 130 + sinf(v * 1.5f) * 10;
  schwalbe(g, 142, sy, 2.4f, 1, u, fmodf(u * 2, 1));
  daum(g, 136, sy - 10, 0.9f, 1, u, 0.6f, 2.6f);
  g.kameraAus();
  vers(g, {"Sie flogen über Wälder und Seen", "und hohe Berge mit Schnee."}, aufzu(v, 0.4f, 12));
  blende(g, u, 26, 142, 120);
}

// 7) Das Land der Blumen: der kleine Blumenprinz, Fluegel - und ein neuer Name
static void szeneBlumen(Gfx &g, float u) {
  himmel(g, 0, u, 200);
  sonne(g, 250, 30, 12, u);
  g.rechteck(0, 200, 284, 40, FARBE(0x7CCB5E));
  for (int i = 0; i < 5; i++) {  // grosse weisse Blumen
    float x = 30 + i * 56;
    g.linie(x, 240, x, 150, 4, FARBE(0x3E9A4E));
    g.kontur(1, OUT);
    for (int k = 0; k < 6; k++) g.ellipse(x + cosf(k * 1.047f) * 18, 140 + sinf(k * 1.047f) * 12, 12, 7, WEISS, 255, k * 1.047f);
    g.konturAus();
    g.kreis(x, 140, 8, FARBE(0xFFD54A));
  }
  schwalbe(g, 240, 70 + sinf(u * 2) * 6, 1.4f, -1, u, fmodf(u * 2, 1));
  bool fee = u > 10;
  Pose p = posis(BLUMENPRINZ, u);
  p.armV = u > 4 && u < 10 ? 1.6f : 0.6f;
  p.mund = u > 4 && u < 10 ? 0.5f : 0.2f;
  fluegel(g, 162, 186, 1, u);
  schneiderlein(g, 168, 220, 0.6f, -1, p);
  daum(g, 112, 220, 1.3f, 1, u, 0.4f, 1.2f, false, fee);
  if (u > 10 && u < 12) puff(g, 112, 190, seg(u, 10, 12), FARBE(0xFFB0D0));
  if (u > 12) herzen(g, 140, 170, u, 5, 30);
  g.kameraAus();
  vers(g, {"In einer Blume wohnte ein Prinz,", "nicht größer als sie selbst."}, aufzu(u, 0.4f, 4));
  blase(g, 200, 60, {"Willst du meine Frau und", "Königin der Blumen werden?"}, aufzu(u, 4.4f, 10), 170, 160);
  vers(g, {"Sie bekam Flügel und einen", "neuen Namen: Maja.", "Und die Schwalbe sang dazu."}, aufzu(u, 11, 22));
  endeBild(g, u, 26, 140, 170);
}

static void bild(Gfx &g, float t) {
  if (t < P2) szeneTulpe(g, t - P1);
  else if (t < P3) szeneKroete(g, t - P2);
  else if (t < P4) szeneFische(g, t - P3);
  else if (t < P5) szeneWinter(g, t - P4);
  else if (t < P6) szeneSchwalbe(g, t - P5);
  else if (t < P7) szeneFlug(g, t - P6);
  else szeneBlumen(g, t - P7);
}

// =====================================================================
// Musik
// =====================================================================
// Daeumelinchens Lied (E-Dur, zart und hoch)
static const Ton3 MEL_DAUM[] = {
  {76, 1, 52}, {80, .5, 0}, {83, .5, 0}, {81, 1, 0}, {80, 1, 0}, {78, .5, 47}, {80, .5, 0}, {81, .5, 0}, {83, .5, 0}, {80, 2, 52},
  {85, 1, 57}, {83, .5, 0}, {81, .5, 0}, {80, 1, 52}, {78, 1, 0}, {76, .5, 47}, {78, .5, 0}, {80, .5, 0}, {78, .5, 0}, {76, 2, 52}};
static void zwitscher(Synth &s, float x) { for (int i = 0; i < 4; i++) s.ton(x + i * 0.09f, 2600 + i * 300, 0.08f, 0.005f, 0.06f, SINUS, GERAEUSCH, 3400); }
static void ton(Synth &s, float t0, float t1) {
  float a = P1;
  spieleMelodie(s, M(MEL_WIEGE), a + 0.5f, a + 9.6f, 0.42f, t0, t1, 0.5f);
  if (bei(a + 14, t0, t1)) for (int k = 0; k < 8; k++) s.note(a + 14 + k * 0.08f, 84 + k * 2, 0.5f);
  spieleMelodie(s, M(MEL_DAUM), a + 15, a + 27.5f, 0.3f, t0, t1, 0.75f);
  a = P2;
  spieleMelodie(s, M(MEL_SPUK), a + 0.4f, a + 8.6f, 0.42f, t0, t1, 0.4f);
  for (int i = 0; i < 4; i++) if (bei(a + 1.4f + i * 0.5f, t0, t1)) s.ton(a + 1.4f + i * 0.5f, 130, 0.3f, 0.01f, 0.2f, SAEGE, GERAEUSCH, 90);  // plumps
  if (bei(a + 19.4f, t0, t1)) for (int i = 0; i < 3; i++) s.ton(a + 19.4f + i * 0.35f, 160, 0.25f, 0.01f, 0.25f, SAEGE, GERAEUSCH, 110);  // koax
  spieleMelodie(s, M(MEL_WIEGE), a + 13, a + 23.5f, 0.42f, t0, t1, 0.45f);
  a = P3;
  for (int i = 0; i < 6; i++) if (bei(a + 4 + i * 0.8f, t0, t1)) s.note(a + 4 + i * 0.8f, 72 + (i % 2) * 4, 0.3f);  // blubb
  spieleMelodie(s, M(MEL_DAUM), a + 10, a + 27.5f, 0.32f, t0, t1, 0.7f);
  a = P4;
  if (bei(a + 0.4f, t0, t1)) s.rauschen(a + 0.4f, 9, BANDPASS, 500, 0.06f, 1, 1.5f);  // Wind
  spieleMelodie(s, M(MEL_WIEGE), a + 0.4f, a + 9.6f, 0.46f, t0, t1, 0.4f);
  spieleMelodie(s, M(MEL_KEGEL), a + 10.4f, a + 25.5f, 0.34f, t0, t1, 0.4f);
  a = P5;
  spieleMelodie(s, M(MEL_WIEGE), a + 0.4f, a + 12, 0.46f, t0, t1, 0.45f);
  for (int i = 0; i < 6; i++) if (bei(a + 7 + i * 0.8f, t0, t1)) s.ton(a + 7 + i * 0.8f, 70, 0.3f, 0.005f, 0.12f);  // poch poch
  if (bei(a + 12.6f, t0, t1)) zwitscher(s, a + 12.6f);
  spieleMelodie(s, M(MEL_DAUM), a + 13, a + 27.5f, 0.32f, t0, t1, 0.6f);
  if (bei(a + 22, t0, t1)) zwitscher(s, a + 22);
  a = P6;
  spieleMelodie(s, M(MEL_WIEGE), a + 0.4f, a + 5.6f, 0.46f, t0, t1, 0.4f);
  if (bei(a + 5.4f, t0, t1)) zwitscher(s, a + 5.4f);
  spieleMelodie(s, M(MEL_DAUM), a + 6, a + 25.5f, 0.26f, t0, t1, 0.85f);
  a = P7;
  spieleMelodie(s, M(MEL_DAUM), a + 0.4f, a + 25.5f, 0.3f, t0, t1, 0.8f);
  if (bei(a + 10, t0, t1)) for (int k = 0; k < 8; k++) s.note(a + 10 + k * 0.08f, 84 + k * 2, 0.5f);
  for (int i = 0; i < 3; i++) if (bei(a + 14 + i * 3, t0, t1)) zwitscher(s, a + 14 + i * 3);
  endeKlang(s, a + 26.2f, t0, t1);
}

// =====================================================================
// Startbild, Vorschau, Erkennungszeichen
// =====================================================================
static void titelSzene(Gfx &g, float t) {
  wiese(g, t, 0);
  tulpe(g, 160, 240, 1.7f, 1);
  daum(g, 160, 100, 0.9f, -1, t, 0.4f, 1.6f + sinf(t * 2) * 0.3f);
  schwalbe(g, 60, 70 + sinf(t * 2) * 6, 1.4f, 1, t, fmodf(t * 2, 1));
}
static void titelbild(Gfx &g, float t) { titelSzene(g, t); titelRahmen(g, t, "Däumelinchen", 170); }
static void vorschau(Gfx &g, float t) { titelSzene(g, t); }
static void symbol(Gfx &g, float x, float y, float t) {
  tulpe(g, x, y + 12, 0.9f, 1);
  daum(g, x, y - 56, 0.45f, 1, t, 0.3f, 1.8f);
}

const Maerchen DAEUMELINCHEN = {"Däumelinchen", PDAUER, FARBE(0xFF9AC0), bild, ton, titelbild, vorschau, symbol,
                                LIED(MEL_DAUM, 0.3f), "H. C. Andersen"};

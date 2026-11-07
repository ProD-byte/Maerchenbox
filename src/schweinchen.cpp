// Die drei kleinen Schweinchen - ein Spieluhr-Film fuer die Maerchenbox, nach dem englischen Volksmaerchen.
// Stroh, Holz und Stein; "Kleines Schweinchen, lass mich herein!" - "Nein, nein, beim Barthaar an meinem Kinn!" -
// "Dann huste und puste ich dein Haus zusammen!" In der kinderfreundlichen Fassung: niemand wird gefressen.
#include "erzaehlen.h"

// ---------- Ablauf (Sekunden) ----------
static const float P1 = 0, P2 = 22, P3 = 48, P4 = 82, P5 = 112, P6 = 146, P7 = 174, PDAUER = 200;

static const Farbe ROSA = FARBE(0xF6B0B8), ROSA_D = FARBE(0xE08A98);

// ---------- Figuren ----------
// ein Schweinchen; i = 0 Stroh (gelbes Huetchen), 1 Holz (gruenes Tuch), 2 Stein (blaue Latzhose)
static void schwein(Gfx &g, int i, float x, float y, float s, float dir, float t, float lauf = -1, float mund = 0, bool angst = false,
                    float armV = 0.6f) {
  auto PX = [&](float d) { return x + d * s * dir; };
  auto PY = [&](float d) { return y + d * s; };
  float hop = lauf >= 0 ? fabsf(sinf(lauf * 6.28f)) * 3 * s : 0;
  // Beinchen
  for (int k = 0; k < 2; k++) {
    float sw = lauf >= 0 ? sinf(lauf * 6.28f + k * 3.14f) * 4 : 0;
    g.linie(PX(-5 + k * 10), PY(-10) - hop, PX(-5 + k * 10 + sw), PY(-1), 4 * s, ROSA_D);
  }
  // Ringelschwaenzchen
  g.bogen(PX(-15), PY(-22) - hop, 3.5f * s, 0, 5.5f, 1.4f * s, ROSA_D);
  g.kontur(1.2f * s, OUT);
  g.ellipse(PX(0), PY(-22) - hop, 14 * s, 13 * s, ROSA);  // Bauch
  if (i == 2) {  // Latzhose
    g.rundRechteck(PX(-10) < PX(10) ? PX(-10) : PX(10), PY(-24) - hop, 20 * s, 14 * s, 4 * s, FARBE(0x3E6EA8));
  }
  // Arm
  float ax = PX(8), ay = PY(-26) - hop;
  g.linie(ax, ay, ax + sinf(armV) * 10 * s * dir, ay + cosf(armV) * 10 * s, 3.4f * s, ROSA);
  // Kopf
  float kx = PX(4), ky = PY(-42) - hop;
  g.kreis(kx, ky, 11 * s, ROSA);
  float o1[] = {kx - 9 * s * dir, ky - 6 * s, kx - 6 * s * dir, ky - 16 * s, kx - 1 * s * dir, ky - 9 * s};
  float o2[] = {kx + 1 * s * dir, ky - 9 * s, kx + 6 * s * dir, ky - 16 * s, kx + 8 * s * dir, ky - 6 * s};
  g.polygon(o1, 3, ROSA);
  g.polygon(o2, 3, ROSA);
  g.ellipse(kx + 8 * s * dir, ky + 2 * s, 5 * s, 4 * s, ROSA_D);  // Ruessel
  g.konturAus();
  g.kreis(kx + 7 * s * dir, ky + 2 * s, 0.9f * s, OUT);
  g.kreis(kx + 10 * s * dir, ky + 2 * s, 0.9f * s, OUT);
  for (int k = 0; k < 2; k++) {
    float ex = kx + (k ? 4 : -2) * s * dir, ey = ky - 3 * s;
    if (angst) { g.kreis(ex, ey, 2.4f * s, WEISS); g.kreis(ex, ey, 1.2f * s, SCHWARZ); }
    else if (fmodf(t + i * 1.3f, 3.7f) < 0.13f) g.bogen(ex, ey + 0.5f * s, 1.5f * s, 3.5f, 5.9f, 1 * s, OUT);
    else g.kreis(ex, ey, 1.3f * s, SCHWARZ);
  }
  if (mund > 0.05f) g.ellipse(kx + 4 * s * dir, ky + 7 * s, 2 * s, (0.8f + mund * 2) * s, FARBE(0x8A2A2A));
  else g.bogen(kx + 4 * s * dir, ky + 5 * s, 3 * s, 0.4f, 2.7f, 1 * s, OUT);
  g.kreis(kx - 3 * s * dir, ky + 4 * s, 2 * s, WANGE, 120);
  if (i == 0) {  // Strohhuetchen
    g.ellipse(kx, ky - 10 * s, 12 * s, 3 * s, FARBE(0xE8C860));
    g.ellipse(kx, ky - 14 * s, 7 * s, 5 * s, FARBE(0xE8C860));
    g.rechteck(kx - 7 * s, ky - 12 * s, 14 * s, 2 * s, FARBE(0xC0392B));
  } else if (i == 1) {  // gruenes Halstuch
    float h[] = {kx - 8 * s, ky + 9 * s, kx + 8 * s, ky + 9 * s, kx, ky + 17 * s};
    g.polygon(h, 3, FARBE(0x3DAE5B));
  }
}

// der Wolf (vierbeinig), mit dicken Backen zum Pusten
static void wolf(Gfx &g, float x, float y, float s, float dir, float t, float lauf, float mund, float pusten = 0, bool zu = false) {
  const Farbe fell = FARBE(0x7E8696), hell = FARBE(0xC9CDD6), dunkel = FARBE(0x5E6474), pf = FARBE(0x2A2630);
  auto PX = [&](float dx) { return x + dx * s * dir; };
  auto PY = [&](float dy) { return y + dy * s; };
  g.kontur(1.5f * s, OUT);
  for (int i = 0; i < 4; i++) {
    float bx = i < 2 ? -24 + i * 10 : 18 + (i - 2) * 10;
    float sw = lauf >= 0 ? sinf(lauf * 6.2831853f + i * 1.6f) * 9 : 0;
    g.linie(PX(bx), PY(-28), PX(bx + sw), PY(-5), 6 * s, i & 1 ? fell : dunkel);
    g.ellipse(PX(bx + sw + 1), PY(-3), 5 * s, 3 * s, pf);
  }
  g.kurve(PX(-30), PY(-36), PX(-50), PY(-34), PX(-54), PY(-20), 10 * s, fell);
  g.ellipse(PX(0), PY(-36), 34 * s, 15 * s, fell);
  float kx = PX(36), ky = PY(-50);
  float o1[] = {kx - 12 * s * dir, ky - 8 * s, kx - 8 * s * dir, ky - 24 * s, kx - 1 * s * dir, ky - 10 * s};
  float o2[] = {kx + 0 * s * dir, ky - 10 * s, kx + 6 * s * dir, ky - 24 * s, kx + 9 * s * dir, ky - 8 * s};
  g.polygon(o1, 3, fell);
  g.polygon(o2, 3, fell);
  g.kreis(kx, ky, 14 * s, fell);
  if (pusten > 0) g.kreis(kx + 6 * s * dir, ky + 4 * s, (6 + pusten * 6) * s, hell);  // dicke Backen
  float sch[] = {kx + 6 * s * dir, ky - 4 * s, kx + 28 * s * dir, ky, kx + 28 * s * dir, ky + 4 * s + mund * 6 * s, kx + 6 * s * dir, ky + 8 * s};
  g.polygon(sch, 4, fell);
  g.konturAus();
  if (mund > 0.05f) {
    float m[] = {kx + 8 * s * dir, ky + 6 * s, kx + 26 * s * dir, ky + 6 * s + mund * 8 * s, kx + 26 * s * dir, ky + 10 * s + mund * 10 * s, kx + 8 * s * dir, ky + 10 * s};
    g.polygon(m, 4, FARBE(0x7A2A2A));
  }
  g.kreis(kx + 27 * s * dir, ky + 1 * s, 3 * s, SCHWARZ);
  if (zu) g.bogen(kx + 4 * s * dir, ky - 2 * s, 3 * s, 3.5f, 5.9f, 1.6f * s, OUT);
  else {
    g.kreis(kx + 4 * s * dir, ky - 2 * s, 3 * s, FARBE(0xF2D24A));
    g.kreis(kx + 4.8f * s * dir, ky - 2 * s, 1.4f * s, SCHWARZ);
    g.linie(kx, ky - 7 * s, kx + 8 * s * dir, ky - 5 * s, 1.6f * s, OUT);
  }
  (void)t;
}

// Wind beim Pusten: Linien von (x, y) nach rechts
static void wind(Gfx &g, float x, float y, float dir, float t, float k) {
  for (int i = 0; i < 6; i++) {
    float ph = fmodf(t * 3 + i * 0.17f, 1);
    float wx = x + ph * 90 * dir, wy = y - 14 + i * 6 + sinf(ph * 8 + i) * 3;
    g.linie(wx, wy, wx + 16 * dir, wy, 1.6f, WEISS, (uint8_t)(220 * k * (1 - ph)));
  }
}

// ---------- die Haeuser ----------
static void strohhaus(Gfx &g, float x, float y, float kaputt, float t) {
  if (kaputt >= 1) return;
  float w = sinf(t * 20) * kaputt * 6;
  g.kontur(1.4f, OUT);
  g.rechteck(x - 30 + w, y - 46, 60, 46, FARBE(0xE8C860));
  float d[] = {x - 38 + w, y - 44, x + w * 2, y - 84, x + 38 + w, y - 44};
  g.polygon(d, 3, FARBE(0xD8B048));
  g.konturAus();
  for (int i = 0; i < 9; i++) g.linie(x - 26 + i * 6 + w, y - 44, x - 27 + i * 6 + w, y - 2, 1, FARBE(0xB8984E));
  // Strohbuendel auf dem Dach, Halme haengen an der Traufe heraus
  for (int r = 0; r < 4; r++) {
    float yy = y - 76 + r * 9, hb = 6 + r * 9;
    for (float hx = -hb; hx <= hb; hx += 4) g.linie(x + hx + w, yy, x + hx * 1.1f + w + 1, yy + 8, 1, FARBE(0xB8903A), 180);
  }
  for (int i = 0; i < 14; i++) g.linie(x - 36 + i * 5.4f + w, y - 44, x - 37 + i * 5.4f + w, y - 39 - (i % 3), 1, FARBE(0xC8A040));
  g.rundRechteck(x - 8 + w, y - 26, 16, 26, 6, FARBE(0xA8823E));
  g.kreis(x + 4 + w, y - 13, 1.4f, OUT);
  g.ellipse(x + 20 + w, y - 30, 6, 5, FARBE(0x5A4A2A));  // rundes Guckloch
}
static void holzhaus(Gfx &g, float x, float y, float kaputt, float t) {
  if (kaputt >= 1) return;
  float w = sinf(t * 18) * kaputt * 6;
  g.kontur(1.4f, OUT);
  g.rechteck(x - 32 + w, y - 50, 64, 50, HOLZ);
  float d[] = {x - 40 + w, y - 48, x + w * 2, y - 86, x + 40 + w, y - 48};
  g.polygon(d, 3, HOLZ_D);
  g.konturAus();
  for (int i = 0; i < 5; i++) g.linie(x - 32 + w, y - 46 + i * 10, x + 32 + w, y - 46 + i * 10, 1.2f, HOLZ_D);
  for (int i = 0; i < 12; i++) g.kreis(x - 28 + zufall(i + 40) * 56 + w, y - 44 + zufall(i + 60) * 40, 1, HOLZ_D);  // Aeste
  for (int i = 1; i < 6; i++) {  // Bretter auf dem Dach
    float k = i / 6.0f;
    g.linie(x + w * 2 - k * 40, y - 86 + k * 38, x + w * 2, y - 86, 1, HOLZ, 150);
    g.linie(x + w * 2 + k * 40, y - 86 + k * 38, x + w * 2, y - 86, 1, HOLZ, 150);
  }
  holztuer(g, x - 9 + w, y, 18, 28, HOLZ_D, true);
  fenster(g, x + 14 + w, y - 40, 12, 12, 0, false, false);
  g.rechteck(x - 30 + w, y - 40, 10, 14, HOLZ_D);  // Fensterladen
}
static void steinhaus(Gfx &g, float x, float y, float t, float rauch) {
  g.kontur(1.6f, OUT);
  g.rechteck(x + 18, y - 98, 12, 28, FARBE(0x9A4A3A));  // Schornstein
  g.rechteck(x + 16, y - 102, 16, 5, FARBE(0x7E3626));
  g.rechteck(x - 40, y - 60, 80, 60, FARBE(0xE8D8C8));
  g.konturAus();
  // Ziegelsteine mit hellen Fugen, jeder ein wenig anders
  for (int r = 0; r < 6; r++)
    for (int c = 0; c < 6; c++) {
      float bx = x - 40 + c * 16 - (r & 1) * 8;
      float bw = fminf(fminf(15, bx + 15 - (x - 40)), x + 40 - bx) - 1;
      if (bw < 2) continue;
      g.rechteck(fmaxf(bx, x - 40) + 0.5f, y - 59 + r * 10, bw, 8.6f,
                 mischeFarbe(FARBE(0xC0503A), FARBE(0x8E2E22), zufall(r * 7 + c)));
    }
  ziegeldach(g, x - 40, x + 40, y - 58, 40, FARBE(0x6E5A5A));
  holztuer(g, x - 10, y, 20, 32, HOLZ, true);
  fenster(g, x - 32, y - 46, 14, 14, 1, true, true);
  fenster(g, x + 18, y - 46, 14, 14, 1, true, true);
  for (int i = 0; i < 3 && rauch > 0; i++) {
    float k = fmodf(t * 0.4f + i * 0.33f, 1);
    g.kreis(x + 24 + sinf(k * 5 + i) * 6, y - 102 - k * 40, 4 + k * 6, FARBE(0xE8E4EE), (uint8_t)(150 * (1 - k) * rauch));
  }
}

static void landschaft(Gfx &g, float t) {
  himmel(g, 0, t, 196);
  sonne(g, 248, 30, 11, t);
  wolke(g, fmodf(80 + t * 4, 340) - 30, 40, 0.8f);
  huegel(g, 0, 0.3f, 196, 26, 110, FARBE(0x8ACB6A));
  boden(g, 196);
  blumen(g, 0, 222, 7, 4);
}

static void strohFliegt(Gfx &g, float x, float y, float k) {
  for (int i = 0; i < 16; i++) {
    float a = -0.6f + zufall(i + 300) * 1.2f;
    float r = k * (60 + zufall(i + 320) * 120);
    g.linie(x + cosf(a) * r, y - 30 + sinf(a) * r - k * 40, x + cosf(a) * r + 8, y - 30 + sinf(a) * r - k * 40 + 3, 1.4f,
            FARBE(0xE8C860), (uint8_t)(255 * (1 - k)));
  }
}
static void bretterFliegen(Gfx &g, float x, float y, float k) {
  for (int i = 0; i < 10; i++) {
    float a = -0.5f + zufall(i + 400) * 1.0f;
    float r = k * (50 + zufall(i + 420) * 110);
    float bx = x + cosf(a) * r, by = y - 30 + sinf(a) * r - k * 30 + k * k * 60;
    g.linie(bx, by, bx + cosf(k * 8 + i) * 12, by + sinf(k * 8 + i) * 12, 3, HOLZ, (uint8_t)(255 * (1 - k)));
  }
}

// =====================================================================
// Szenen
// =====================================================================
// 1) Die Mutter schickt die drei hinaus in die Welt
static void szeneAufbruch(Gfx &g, float u) {
  landschaft(g, u);
  // die Mutter
  schwein(g, 1, 60, 214, 1.4f, 1, u, -1, u > 2 && u < 9 ? 0.5f : 0, false, u > 12 ? 2.6f + sinf(u * 6) * 0.3f : 0.6f);
  for (int i = 0; i < 3; i++) {
    float k = seg(u, 12 + i * 0.6f, 21);
    schwein(g, i, lerp(110 + i * 32, 330 + i * 30, k), 214, 1, 1, u, k > 0 && k < 1 ? fmodf(u * 1.8f + i * 0.3f, 1) : -1, k <= 0 ? 0.3f : 0);
  }
  g.kameraAus();
  vers(g, {"Es war einmal eine alte Sau,", "die hatte drei kleine Schweinchen."}, aufzu(u, 0.4f, 4));
  blase(g, 80, 60, {"Ihr seid groß. Zieht hinaus", "und baut euch ein eigenes Haus!"}, aufzu(u, 4.2f, 11), 70, 150);
  blende(g, u, 22, 140, 160);
}

// 2) Drei Haeuser: Stroh, Holz und Stein
static void szeneBauen(Gfx &g, float u) {
  landschaft(g, u);
  float b0 = seg(u, 1, 5), b1 = seg(u, 4, 12), b2 = seg(u, 8, 22);
  g.ausschnitt(0, 214 - b0 * 90, 284, 240);
  strohhaus(g, 50, 214, 0, u);
  g.ausschnittAus();
  g.ausschnitt(0, 214 - b1 * 90, 284, 240);
  holzhaus(g, 142, 214, 0, u);
  g.ausschnittAus();
  g.ausschnitt(0, 214 - b2 * 104, 284, 240);
  steinhaus(g, 236, 214, u, 0);
  g.ausschnittAus();
  schwein(g, 0, 86, 222, 0.9f, -1, u, -1, u > 6 ? 0.4f : 0, false, u > 6 ? 2.6f : 1.6f + fabsf(sinf(u * 8)) * 0.6f);
  schwein(g, 1, 180, 222, 0.9f, -1, u, -1, u > 13 ? 0.4f : 0, false, u > 13 ? 2.6f : 1.6f + fabsf(sinf(u * 8)) * 0.6f);
  schwein(g, 2, 200, 222, 0.9f, 1, u, -1, 0.1f, false, 1.6f + fabsf(sinf(u * 6)) * 0.8f);  // mauert fleissig
  g.kameraAus();
  vers(g, {"Das erste baute sein Haus", "aus Stroh, das ging schnell,", "und dann spielte es."}, aufzu(u, 0.4f, 7));
  vers(g, {"Das zweite baute sein Haus aus Holz."}, aufzu(u, 7.2f, 12.6f));
  vers(g, {"Das dritte baute sein Haus", "aus Stein. Das dauerte lange,", "aber es war fest."}, aufzu(u, 13, 25.6f));
  blende(g, u, 26, 140, 160);
}

// ein Wolf-Besuch an einem Haus: Dialog, Pusten, Haus fliegt, Schweinchen flieht
static void besuch(Gfx &g, float u, int haus, float hx, Farbe dummy) {
  (void)dummy;
  float kaputt = seg(u, 18, 21);
  float wx = lerp(-60, hx - 100, sanft(seg(u, 0, 4)));
  float pusten = (u > 13 && u < 19) ? 0.5f + 0.5f * sinf(u * 4) : 0;
  wolf(g, wx, 222, 0.85f, 1, u, u < 4 ? fmodf(u * 1.4f, 1) : -1, (u > 4 && u < 7) || (u > 10 && u < 13) ? 0.5f : 0, pusten);
  if (pusten > 0 || (u > 18 && u < 21)) wind(g, wx + 50, 176, 1, u, u > 13 ? 1 : 0);
  if (haus == 0) strohhaus(g, hx, 222, kaputt, u);
  else holzhaus(g, hx, 222, kaputt, u);
  if (u > 18 && u < 24) { if (haus == 0) strohFliegt(g, hx, 222, seg(u, 18, 24)); else bretterFliegen(g, hx, 222, seg(u, 18, 24)); }
  // das Schweinchen: erst im Fenster/hinter der Tuer, dann rennt es davon zum naechsten Haus
  if (u > 20) {
    float k = seg(u, 21, 26);
    for (int i = 0; i <= haus; i++)
      schwein(g, i, lerp(hx + i * 10, 330 + i * 20, k), 222, 0.9f, 1, u, k > 0 && k < 1 ? fmodf(u * 2.4f + i * 0.4f, 1) : -1, 0.7f, true, 2.6f);
  } else if (u > 6) {
    for (int i = 0; i <= haus; i++) {
      g.ausschnitt(hx - 8, 196, 16, 26);
      schwein(g, i, hx + i * 4, 238, 0.8f, -1, u, -1, (u > 7 && u < 10) ? 0.6f : 0, u > 13);
      g.ausschnittAus();
    }
  }
}

// 3) Am Strohhaus: "Kleines Schweinchen, lass mich herein!"
static void szeneStroh(Gfx &g, float u) {
  landschaft(g, u);
  besuch(g, u, 0, 200, 0);
  g.kameraAus();
  vers(g, {"Da kam der Wolf an das Strohhaus", "und klopfte an."}, aufzu(u, 0.4f, 4));
  blase(g, 90, 60, {"Kleines Schweinchen,", "lass mich herein!"}, aufzu(u, 4.2f, 7), 70, 160);
  blase(g, 200, 60, {"Nein, nein, beim Barthaar", "an meinem Kinn!"}, aufzu(u, 7.2f, 10), 200, 180);
  blase(g, 90, 60, {"Dann huste ich und puste ich", "und puste dein Haus zusammen!"}, aufzu(u, 10.2f, 13.4f), 70, 160);
  vers(g, {"Und er hustete und pustete -", "und pustete das Haus zusammen!"}, aufzu(u, 18, 26));
  blende(g, u, 34, 140, 160);
}

// 4) Am Holzhaus: dasselbe noch einmal
static void szeneHolz(Gfx &g, float u) {
  landschaft(g, u);
  besuch(g, u, 1, 200, 0);
  g.kameraAus();
  blase(g, 90, 60, {"Kleine Schweinchen,", "lasst mich herein!"}, aufzu(u, 4.2f, 7), 70, 160);
  blase(g, 200, 60, {"Nein, nein, beim Barthaar", "an unserem Kinn!"}, aufzu(u, 7.2f, 10), 200, 180);
  blase(g, 90, 60, {"Dann huste ich und puste ich", "und puste euer Haus zusammen!"}, aufzu(u, 10.2f, 13.4f), 70, 160);
  vers(g, {"Krach! Die Bretter flogen,", "und die zwei rannten", "zum Steinhaus."}, aufzu(u, 18.4f, 28));
  blende(g, u, 30, 140, 160);
}

// 5) Am Steinhaus: er pustet und pustet - umsonst
static void szeneStein(Gfx &g, float u) {
  landschaft(g, u);
  steinhaus(g, 200, 222, u, 1);
  float pusten = (u > 10 && u < 26) ? 0.5f + 0.5f * sinf(u * 4) : 0;
  float schwindlig = seg(u, 26, 28);
  float wx = lerp(-60, 100, sanft(seg(u, 0, 4)));
  wolf(g, wx, 222, 0.85f, 1, u, u < 4 ? fmodf(u * 1.4f, 1) : -1, (u > 4 && u < 7) || (u > 7.4f && u < 10) ? 0.5f : 0, pusten, schwindlig > 0);
  if (pusten > 0) wind(g, wx + 50, 176, 1, u, 1);
  if (schwindlig > 0) for (int i = 0; i < 4; i++) { float a = u * 4 + i * 1.57f; g.stern(wx + 30 + cosf(a) * 14, 160 + sinf(a) * 5, 3, 1.3f, 5, a, GOLD); }
  // die drei im Fenster lachen
  for (int i = 0; i < 2; i++) {
    g.ausschnitt(200 + (i ? 18 : -32), 176, 14, 14);
    schwein(g, i * 2, 200 + (i ? 25 : -25), 210, 0.6f, -1, u, -1, u > 26 ? 0.6f : 0.2f);
    g.ausschnittAus();
  }
  g.kameraAus();
  blase(g, 90, 60, {"Kleine Schweinchen,", "lasst mich herein!"}, aufzu(u, 4.2f, 7), 80, 160);
  blase(g, 210, 70, {"Nein, nein, beim Barthaar", "an unserem Kinn!"}, aufzu(u, 7.2f, 10), 200, 168);
  vers(g, {"Er hustete und pustete, und er", "pustete und hustete -", "aber das Haus blieb stehen."}, aufzu(u, 12, 26));
  blende(g, u, 34, 140, 160);
}

// 6) Durch den Schornstein - in den Topf! Platsch! Und davon
static void szeneSchornstein(Gfx &g, float u) {
  if (u < 10) {
    landschaft(g, u);
    steinhaus(g, 160, 222, u, 1);
    // der Wolf klettert aufs Dach
    float k = seg(u, 0, 7);
    float wx = lerp(60, 170, k), wy = lerp(222, 128, k);
    wolf(g, wx, wy, 0.7f, 1, u, k < 1 ? fmodf(u * 1.4f, 1) : -1, 0.2f);
    g.kameraAus();
    vers(g, {"Da kletterte der Wolf aufs Dach", "und wollte durch den Schornstein."}, aufzu(u, 0.4f, 9));
    return;
  }
  float v = u - 10;
  // in der Stube: der grosse Topf mit kochendem Wasser im Kamin
  g.verlauf(0, 200, FARBE(0xE8D2B0), FARBE(0xD6B88E));
  g.rechteck(0, 200, 284, 40, FARBE(0x9E7656));
  g.kontur(1.8f, OUT);
  g.rechteck(100, 0, 90, 130, FARBE(0xC0503A));
  g.rundRechteck(84, 130, 122, 72, 4, FARBE(0xA0402E));
  g.konturAus();
  g.rundRechteck(104, 150, 82, 52, 16, FARBE(0x2E2220));
  for (int i = 0; i < 3; i++) g.ellipse(124 + i * 20, 196, 7, 3 + sinf(u * 8 + i), FARBE(0xFF8A2A));
  g.kontur(1.4f, OUT);
  g.ellipse(145, 178, 34, 10, FARBE(0x3A3436));
  g.konturAus();
  g.ellipse(145, 176, 30, 7, FARBE(0x8ECCF0));
  for (int i = 0; i < 4; i++) {
    float k = fmodf(u * 0.8f + i * 0.25f, 1);
    g.kreis(126 + i * 12, 170 - k * 20, 3 + k * 4, WEISS, (uint8_t)(160 * (1 - k)));
  }
  // er faellt hinein - Platsch! - und springt heraus und rennt davon
  float fall = seg(v, 2, 3.2f);
  if (v > 2 && v < 3.3f) wolf(g, 140, lerp(-20, 190, hinein(fall)), 0.7f, 1, u, -1, 0.8f);
  if (v > 3.2f && v < 4.6f) for (int i = 0; i < 12; i++) { float a = 3.3f + i * 0.24f, k = seg(v, 3.2f, 4.6f); g.kreis(145 + cosf(a) * k * 60, 176 + sinf(a) * k * 50, 3, FARBE(0xBFE6FF), (uint8_t)(255 * (1 - k))); }
  if (v > 4.6f) {
    float k = seg(v, 4.6f, 8);
    wolf(g, lerp(150, 330, k), 214 - huepf(fmodf(k * 3, 1)) * 30, 0.8f, 1, u, fmodf(u * 3, 1), 0.9f);
    for (int i = 0; i < 3; i++) g.kreis(lerp(150, 330, k) - 30 - i * 8, 180 - i * 6, 4, WEISS, (uint8_t)(160 * (1 - k)));  // dampfender Hintern
  }
  for (int i = 0; i < 3; i++) schwein(g, i, 30 + i * 22, 214, 0.9f, 1, u, -1, v > 3.2f ? 0.6f : 0.2f, false, v > 4.6f ? 2.6f : 0.8f);
  if (v > 3 && v < 3.5f) g.wackeln(sinf(u * 90) * 4, 2);
  g.kameraAus();
  if (v < 1) g.rechteck(0, 0, 284, 240, 0, (uint8_t)(255 * (1 - v)));
  vers(g, {"Platsch! - da fiel er mitten", "in den Topf mit heißem Wasser."}, aufzu(v, 3.4f, 9));
  vers(g, {"Er sprang heraus und rannte davon", "und kam nie, nie wieder."}, aufzu(v, 9.2f, 18));
  blende(g, u, 28, 140, 160);
}

// 7) Die drei tanzen vor dem Steinhaus
static void szeneTanz(Gfx &g, float u) {
  landschaft(g, u);
  steinhaus(g, 200, 214, u, 1);
  for (int i = 0; i < 3; i++) {
    float a = u * 1.6f + i * 2.09f;
    schwein(g, i, 80 + cosf(a) * 40, 214 - fabsf(sinf(u * 5 + i)) * 6, 1, sinf(a) > 0 ? -1 : 1, u, -1, 0.5f, false, 2.6f + sinf(u * 6 + i) * 0.3f);
  }
  herzen(g, 80, 150, u, 4, 40);
  g.kameraAus();
  vers(g, {"Und die drei kleinen Schweinchen", "lebten froh und sicher", "im Steinhaus."}, aufzu(u, 0.6f, 14));
  endeBild(g, u, 20);
  if (u < 1.2f) g.iris(142, 150, lerp(0, 330, sanft(u / 1.2f)));
}

static void bild(Gfx &g, float t) {
  if (t < P2) szeneAufbruch(g, t - P1);
  else if (t < P3) szeneBauen(g, t - P2);
  else if (t < P4) szeneStroh(g, t - P3);
  else if (t < P5) szeneHolz(g, t - P4);
  else if (t < P6) szeneStein(g, t - P5);
  else if (t < P7) szeneSchornstein(g, t - P6);
  else szeneTanz(g, t - P7);
}

// =====================================================================
// Musik
// =====================================================================
// das Schweinchenlied (G-Dur, froh und huepfend)
static const Ton3 MEL_SCHWEIN[] = {
  {67, .5, 43}, {67, .5, 0}, {71, .5, 0}, {74, .5, 0}, {72, .5, 48}, {71, .5, 0}, {69, 1, 0},
  {69, .5, 50}, {69, .5, 0}, {72, .5, 0}, {76, .5, 0}, {74, .5, 43}, {72, .5, 0}, {71, 1, 0},
  {67, .5, 43}, {71, .5, 0}, {74, .5, 0}, {79, .5, 0}, {78, .5, 50}, {76, .5, 0}, {74, .5, 0}, {72, .5, 0}, {71, .5, 43}, {69, .5, 0}, {67, 1, 43}};
static const Ton3 MEL_WOLF[] = {
  {52, 1, 40}, {0, .5, 0}, {55, .5, 0}, {54, 1, 0}, {52, 1, 0}, {55, 1, 40}, {0, .5, 0}, {59, .5, 0}, {58, 1, 0}, {55, 1, 0}};

static void klopf(Synth &s, float x) { for (int i = 0; i < 3; i++) s.ton(x + i * 0.3f, 160, 0.3f, 0.002f, 0.12f, SINUS, GERAEUSCH, 90); }
static void pust(Synth &s, float x, float d) { s.rauschen(x, d, BANDPASS, 900, 0.2f, 0.6f, 0.2f, 300); }
static void grunz(Synth &s, float x) { for (int i = 0; i < 2; i++) s.ton(x + i * 0.18f, 140, 0.12f, 0.005f, 0.1f, SAEGE, GERAEUSCH, 110); }

static void besuchKlang(Synth &s, float a, float t0, float t1, bool kracht) {
  spieleMelodie(s, M(MEL_WOLF), a + 0.4f, a + 4, 0.3f, t0, t1, 0.8f);
  if (bei(a + 3.6f, t0, t1)) klopf(s, a + 3.6f);
  if (bei(a + 7.4f, t0, t1)) grunz(s, a + 7.4f);
  for (float x = a + 13.4f; x < a + 18; x += 1.6f) if (bei(x, t0, t1)) pust(s, x, 1.2f);
  if (kracht && bei(a + 18, t0, t1)) { s.rauschen(a + 18, 1.0f, BANDPASS, 1400, 0.35f, 0.6f, 0.002f, 300); s.ton(a + 18, 100, 0.3f, 0.002f, 0.4f, SINUS, GERAEUSCH, 50); }
  if (kracht) spieleMelodie(s, M(MEL_JAGD), a + 20.6f, a + 27, 0.14f, t0, t1, 0.85f);
}

static void ton(Synth &s, float t0, float t1) {
  float a = P1;
  spieleMelodie(s, M(MEL_SCHWEIN), a + 0.5f, a + 21.5f, 0.3f, t0, t1, 0.8f);
  a = P2;
  for (float x = ceilf(t0 / 0.3f) * 0.3f; x < t1; x += 0.3f)
    if (x > a + 1 && x < a + 22) s.ton(x, x < a + 5 ? 2400 : x < a + 12 ? 700 : 1200, 0.04f, 0.002f, 0.05f, SINUS, GERAEUSCH);  // rascheln, haemmern, klopfen
  spieleMelodie(s, M(MEL_SCHWEIN), a + 0.4f, a + 25.5f, 0.32f, t0, t1, 0.6f);
  besuchKlang(s, P3, t0, t1, true);
  besuchKlang(s, P4, t0, t1, true);
  a = P5;
  besuchKlang(s, a, t0, t1, false);
  for (float x = a + 18; x < a + 26; x += 1.6f) if (bei(x, t0, t1)) pust(s, x, 1.2f);
  if (bei(a + 26, t0, t1)) for (int k = 0; k < 8; k++) s.note(a + 26 + k * 0.12f, 72 - k * 2, 0.5f);  // schwindlig
  spieleMelodie(s, M(MEL_SCHWEIN), a + 28, a + 33.5f, 0.28f, t0, t1, 0.75f);
  a = P6;
  spieleMelodie(s, M(MEL_WOLF), a + 0.4f, a + 9.6f, 0.3f, t0, t1, 0.75f);
  if (bei(a + 12, t0, t1)) for (int k = 0; k < 8; k++) s.note(a + 12 + k * 0.1f, 84 - k * 3, 0.6f);  // er faellt
  if (bei(a + 13.2f, t0, t1)) { s.rauschen(a + 13.2f, 0.7f, BANDPASS, 900, 0.35f, 0.8f, 0.01f, 300); s.ton(a + 13.2f, 160, 0.2f, 0.005f, 0.3f, SINUS, GERAEUSCH, 60); }
  for (int i = 0; i < 4; i++) if (bei(a + 14.6f + i * 0.5f, t0, t1)) s.ton(a + 14.6f + i * 0.5f, 900 - i * 80, 0.05f, 0.005f, 0.14f, DREIECK, GERAEUSCH, 600);  // au, au!
  spieleMelodie(s, M(MEL_SCHWEIN), a + 19, a + 27.5f, 0.26f, t0, t1, 0.85f);
  a = P7;
  spieleMelodie(s, M(MEL_SCHWEIN), a + 0.4f, a + 20, 0.24f, t0, t1, 0.9f);
  endeKlang(s, a + 22.2f, t0, t1);
}

// =====================================================================
// Startbild, Vorschau, Erkennungszeichen
// =====================================================================
static void titelSzene(Gfx &g, float t) {
  landschaft(g, t);
  strohhaus(g, 40, 214, 0, t);
  holzhaus(g, 130, 214, 0, t);
  steinhaus(g, 230, 214, t, 1);
  for (int i = 0; i < 3; i++) schwein(g, i, 76 + i * 64, 228, 0.9f, 1, t, -1, 0.3f, false, 1.6f + sinf(t * 3 + i) * 0.4f);
}
static void titelbild(Gfx &g, float t) { titelSzene(g, t); titelRahmen(g, t, "Die drei kleinen Schweinchen", 228, SCHRIFT_B); }
static void vorschau(Gfx &g, float t) { titelSzene(g, t); }
static void symbol(Gfx &g, float x, float y, float t) {
  for (int i = 0; i < 3; i++) schwein(g, i, x - 36 + i * 36, y - fabsf(sinf(t * 4 + i)) * 4, 0.95f, 1, t, -1, 0.4f, false, 2.4f);
}

const Maerchen SCHWEINCHEN = {"Die drei kleinen Schweinchen", PDAUER, FARBE(0xF6B0B8), bild, ton, titelbild, vorschau, symbol,
                              LIED(MEL_SCHWEIN, 0.3f), "aus England"};

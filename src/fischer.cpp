// Vom Fischer und seiner Frau - ein Spieluhr-Film fuer die Maerchenbox, nach den Bruedern Grimm (KHM 19).
// Der verwunschene Butt, "Manntje, Manntje, Timpe Te", Huette, Schloss, Koenig, Kaiser, Papst - und das Meer
// wird mit jedem Wunsch dunkler und wilder, bis die Ilsebill wieder in ihrer alten Huette sitzt.
#include "erzaehlen.h"

// ---------- Ablauf (Sekunden) ----------
static const float P1 = 0, P2 = 24, P3 = 46, P4 = 70, P5 = 90, P6 = 114, P7 = 134, P8 = 158, P9 = 178, P10 = 202,
                   P11 = 222, P12 = 246, P13 = 268, P14 = 294, PDAUER = 314;

static Aussehen fischerLook() {
  Aussehen a; a.weste = FARBE(0x4E6E8E); a.hemd = FARBE(0xE8E0D0); a.hose = FARBE(0x5A4A3A); a.haar = FARBE(0x9A9A9A);
  a.hut = FARBE(0x3E4E6E); a.hutD = FARBE(0x2E3A54); a.feder = FARBE(0x3E4E6E);
  return a;
}
static Aussehen dienerLook() {
  Aussehen a; a.weste = FARBE(0xC0392B); a.hemd = WEISS; a.hose = FARBE(0xF2E8D0); a.haar = FARBE(0xE8E8E8); a.muetze = 1;
  a.halstuch = GOLD;
  return a;
}
static const Aussehen FISCHER = fischerLook(), DIENER = dienerLook();
// Ilsebill je nach Stand: 0 arm, 1 Huette, 2 Schloss, 3 Koenig, 4 Kaiser, 5 Papst
static FrauLook ilseLook(int st) {
  FrauLook l; l.haar = FARBE(0x8E5A2A); l.frisur = 2; l.wangen = false;
  const Farbe k[] = {FARBE(0x7A6A5A), FARBE(0x8E6E4E), FARBE(0x8E3A5A), FARBE(0xC0392B), FARBE(0x7A2A8E), WEISS};
  l.kleid = k[st];
  l.schuerze = st == 0 ? FARBE(0xA8987A) : st == 1 ? FARBE(0xF2E8D0) : 0;
  l.flicken = st == 0;
  l.krone = st >= 3 && st <= 4;
  l.glanz = st >= 2;
  l.rockBreite = st >= 2 ? 1.25f : 1;
  return l;
}
static const FrauLook ILSE[] = {ilseLook(0), ilseLook(1), ilseLook(2), ilseLook(3), ilseLook(4), ilseLook(5)};

// =====================================================================
// Figuren und Dinge
// =====================================================================
static void fischer(Gfx &g, float x, float y, float s, float dir, float t, float mund, float armV = 0.4f, float lauf = -1,
                    float armH = 0.2f, bool zu = false) {
  Pose p = posis(FISCHER, t);
  p.mund = mund;
  p.armV = armV;
  p.armH = armH;
  p.lauf = lauf;
  p.zu = zu || p.zu;
  schneiderlein(g, x, y, s, dir, p);
}
static void ilse(Gfx &g, int st, float x, float y, float s, float dir, float t, float mund, float armV = 0.4f, float boese = 0.6f,
                 float armH = 0.2f) {
  FrauPose p = fposis(ILSE[st], t, 2);
  p.mund = mund;
  p.armV = armV;
  p.armH = armH;
  p.boese = boese;
  frau(g, x, y, s, dir, p);
  if (st == 5) {  // die hohe Papstkrone
    g.kontur(1.2f * s, OUT);
    float m[] = {x - 9 * s, y - 80 * s, x + 9 * s, y - 80 * s, x + 5 * s, y - 108 * s, x - 5 * s, y - 108 * s};
    g.polygon(m, 4, WEISS);
    g.konturAus();
    for (int i = 0; i < 3; i++) g.rechteck(x - 8 * s + i * 0.6f * s, y - 86 * s - i * 8 * s, 16 * s - i * 1.2f * s, 2.4f * s, GOLD);
    g.stern(x, y - 110 * s, 3 * s, 1 * s, 4, 0, GOLD);
  }
}
// der Butt: ein flacher Fisch mit beiden Augen oben, darauf ein winziges Kroenchen (verwunschener Prinz)
static void butt(Gfx &g, float x, float y, float s, float t, float mund = 0) {
  float w = sinf(t * 3) * 0.1f;
  g.kontur(1.4f * s, OUT);
  float sw[] = {x - 22 * s, y, x - 34 * s, y - 10 * s, x - 34 * s, y + 10 * s};
  g.polygon(sw, 3, FARBE(0xA8885A));
  g.ellipse(x, y, 24 * s, 14 * s, FARBE(0xC8A46A), 255, w);
  g.konturAus();
  for (int i = 0; i < 7; i++) g.kreis(x - 12 * s + (i % 4) * 7 * s, y - 4 * s + (i / 4) * 7 * s, 1.8f * s, FARBE(0xE8783A));
  g.kreis(x + 12 * s, y - 6 * s, 3 * s, WEISS);
  g.kreis(x + 17 * s, y - 4 * s, 3 * s, WEISS);
  g.kreis(x + 13 * s, y - 6 * s, 1.5f * s, SCHWARZ);
  g.kreis(x + 18 * s, y - 4 * s, 1.5f * s, SCHWARZ);
  if (mund > 0.05f) g.ellipse(x + 22 * s, y + 3 * s, 2.4f * s, (1 + mund * 2.4f) * s, FARBE(0x7A2A2A));
  float k[] = {x + 10 * s, y - 13 * s, x + 12 * s, y - 19 * s, x + 14 * s, y - 15 * s, x + 16 * s, y - 20 * s, x + 18 * s, y - 13 * s};
  g.polygon(k, 5, GOLD);
}

// das Meer nach jedem Wunsch: 0 klar, 1 gruen-gelb, 2 violett-grau, 3 schwarzgrau, 4 schwarz und kochend,
// 5 schwarz mit rotem Gewitter, 6 Sturm mit Wellen wie Kirchtuerme
static void meer(Gfx &g, float t, int st) {
  const Farbe oben[] = {FARBE(0x8ED0FF), FARBE(0xB8D8C8), FARBE(0x8E8AB8), FARBE(0x6E6E7E), FARBE(0x3A3A4E), FARBE(0x4A2A3A), FARBE(0x1A1A2A)};
  const Farbe unten[] = {FARBE(0xD8F0FF), FARBE(0xE8E8B0), FARBE(0xB8A8C8), FARBE(0x8E8E9A), FARBE(0x5A5A6E), FARBE(0x8E3A3A), FARBE(0x2A2A3A)};
  const Farbe see[] = {FARBE(0x3E8ED8), FARBE(0x6EA86E), FARBE(0x4A4A8E), FARBE(0x3A443E), FARBE(0x1E2230), FARBE(0x1A1A24), FARBE(0x10141E)};
  g.verlauf(0, 140, oben[st], unten[st]);
  if (st == 0) sonne(g, 236, 34, 11, t);
  if (st <= 1) wolke(g, 60, 50, 0.9f);
  else for (int i = 0; i < 5; i++) wolke(g, fmodf(i * 70 + t * (st * 4), 360) - 40, 30 + (i & 1) * 20, 1.2f, (uint8_t)(120 + st * 18));
  float hoch = st < 3 ? 2 + st * 2 : st * 5 + (st == 6 ? 18 : 0);
  g.rechteck(0, 140, 284, 100, see[st]);
  for (int r = 0; r < 5; r++)
    for (int i = 0; i < 9; i++) {
      float x = fmodf(i * 40 + r * 17 + t * (10 + st * 6), 330) - 20, y = 146 + r * 18;
      float h = hoch * (0.6f + 0.4f * sinf(t * 2 + i + r));
      float wv[] = {x - 20, y, x, y - h, x + 20, y};
      g.polygon(wv, 3, mischeFarbe(see[st], WEISS, 0.15f + st * 0.02f));
      if (st >= 4) g.ellipse(x, y - h, 6, 2, WEISS, 200);
    }
  if (st == 4) for (int i = 0; i < 8; i++) g.kreis(fmodf(zufall(i) * 284 + t * 5, 284), 150 + zufall(i + 3) * 80, 2 + fmodf(t + i * 0.3f, 1) * 4, WEISS, 160);
  if (st >= 5 && fmodf(t, 3.1f) < 0.25f) {  // Blitz
    float bx = 60 + zufall((int)(t / 3.1f)) * 160;
    float b[] = {bx, 0, bx + 10, 0, bx - 4, 50, bx + 8, 50, bx - 10, 120, bx + 2, 60, bx - 10, 60};
    g.polygon(b, 7, st == 5 ? FARBE(0xFF7A5A) : FARBE(0xFFF6B0));
    g.rechteck(0, 0, 284, 240, WEISS, 50);
  }
  if (st == 6) for (int i = 0; i < 3; i++) {  // Wellen wie Kirchtuerme
    float x = fmodf(i * 110 + t * 30, 360) - 40;
    float w[] = {x - 30, 240, x - 10, 120 + sinf(t + i) * 10, x, 90 + sinf(t + i) * 10, x + 10, 120 + sinf(t + i) * 10, x + 30, 240};
    g.polygon(w, 5, FARBE(0x2A3A5A));
    g.ellipse(x, 92 + sinf(t + i) * 10, 10, 5, WEISS);
  }
}
// das Ufer mit dem Fischer, der ruft
static void ufer(Gfx &g) {
  float u[] = {-10, 240, -10, 196, 90, 186, 130, 240};
  g.polygon(u, 4, FARBE(0xD8C49A));
  g.ellipse(60, 196, 50, 8, FARBE(0xC8B48A));
}
static void ruf(Gfx &g, float u, float a, float b) {
  blase(g, 150, 40, {"Männlein, Männlein, Timpe Te,", "Buttje, Buttje in der See,", "meine Frau, die Ilsebill,", "will nicht so, wie ich wohl will."},
        aufzu(u, a, b), 60, 140);
}

// Wohnorte
static void pisspott(Gfx &g, float t) {
  himmel(g, 0, t, 196);
  sonne(g, 240, 34, 11, t);
  g.rechteck(0, 150, 284, 50, FARBE(0x3E8ED8));
  boden(g, 196, FARBE(0xC8B48A));
  g.kontur(1.6f, OUT);
  g.rechteck(150, 140, 90, 62, FARBE(0x8E7A5A));
  float d[] = {140, 142, 195, 110, 250, 142};
  g.polygon(d, 3, FARBE(0x6E5A3A));
  g.konturAus();
  for (int i = 0; i < 3; i++) g.linie(152, 152 + i * 14, 238, 154 + i * 14, 1.2f, FARBE(0x6E5A3A));
  g.rundRechteck(180, 164, 20, 38, 4, FARBE(0x3A2E26));
  g.rechteck(208, 160, 14, 12, FARBE(0x5A6E8E));
  // Netz zum Trocknen
  g.linie(40, 150, 40, 200, 2, HOLZ_D);
  g.linie(110, 150, 110, 200, 2, HOLZ_D);
  for (int i = 0; i < 6; i++) g.linie(40 + i * 14, 152, 40 + i * 14 + 8, 194, 0.8f, FARBE(0x5A4A3A));
  for (int i = 0; i < 4; i++) g.linie(40, 160 + i * 10, 110, 160 + i * 10, 0.8f, FARBE(0x5A4A3A));
}
static void huette(Gfx &g, float t) {
  himmel(g, 0, t, 196);
  sonne(g, 240, 34, 11, t);
  boden(g, 196);
  haeuschen(g, 195, 202, 110, 82, FARBE(0xF0DCC0), FARBE(0xC0503A), t);
  // Garten mit Gemuese und Obstbaum, Huehner und Enten
  laubbaum(g, 46, 202, 0.7f);
  for (int i = 0; i < 4; i++) g.kreis(30 + i * 10, 152 + (i & 1) * 6, 3, FARBE(0xE8473D));
  for (int i = 0; i < 6; i++) g.ellipse(20 + i * 18, 214, 6, 3, FARBE(0x5DAE48));
  for (int i = 0; i < 3; i++) {
    float hx = 100 + i * 16 + sinf(t * 2 + i) * 4;
    g.ellipse(hx, 222, 6, 4, i == 2 ? WEISS : FARBE(0xE8C890));
    g.kreis(hx + 5, 217, 3, i == 2 ? WEISS : FARBE(0xE8C890));
    g.kreis(hx + 7, 214, 1.4f, FARBE(0xE8473D));
  }
}
static void saal(Gfx &g, float t, int st) {
  const Farbe wand[] = {0, 0, FARBE(0xE8D8B8), FARBE(0xC8B0D8), FARBE(0xD8C0A0), FARBE(0xE8E0D0)};
  g.verlauf(0, 200, wand[st], mischeFarbe(wand[st], FARBE(0x6E5A6E), 0.3f));
  for (int i = 0; i < 5; i++) {  // Saeulen
    float x = 14 + i * 64;
    g.rechteck(x, 20, 18, 180, FARBE(0xF6EEDC));
    g.rechteck(x - 4, 16, 26, 8, GOLD);
  }
  g.rechteck(0, 200, 284, 40, st >= 4 ? FARBE(0x8E2A3A) : FARBE(0xA8845A));
  // Kronleuchter
  g.linie(142, 0, 142, 20, 2, GOLD);
  g.ellipse(142, 28, 30, 6, GOLD);
  for (int i = 0; i < 5; i++) g.ellipse(118 + i * 12, 16 + sinf(t * 8 + i) * 0.6f, 2, 4, FARBE(0xFFB83D));
  // Thron: hoeher mit jedem Stand
  if (st >= 3) {
    float h = st == 3 ? 40 : st == 4 ? 70 : 100;
    g.kontur(1.6f, OUT);
    g.rechteck(110, 200 - h * 0.4f, 64, h * 0.4f, FARBE(0xC8A040));
    g.rundRechteck(118, 200 - h - 60, 48, 70, 10, st == 5 ? FARBE(0xE8E0F0) : FARBE(0xC0392B));
    g.konturAus();
    for (int i = 0; i < 4; i++) g.rechteck(110, 200 - h * 0.4f + i * h * 0.1f, 64, 2, FARBE(0x9A7A2A));
  }
}

// =====================================================================
// Szenen
// =====================================================================
// 1) Der verwunschene Butt
static void szeneButt(Gfx &g, float u) {
  meer(g, u, 0);
  ufer(g);
  bool zieht = u > 4 && u < 8;
  fischer(g, 70, 198, 0.9f, 1, u, u > 13.4f && u < 16 ? 0.4f : 0, zieht ? 2.4f : 1.6f);
  float rute = zieht ? -0.4f : 0;
  float rx = 92, ry = 140, ex = 190 + rute * 40, ey = 110 + rute * 40;
  g.linie(rx, ry + 20, ex, ey, 2.2f, HOLZ_D);
  float fy = u < 5 ? 170 : u < 16 ? lerp(170, 150, seg(u, 5, 7)) : lerp(150, 190, seg(u, 16, 18));
  g.linie(ex, ey, 190, fy, 0.8f, OUT);
  if (u > 5 && u < 18) butt(g, 190, fy + 8, 0.9f, u, u > 7.4f && u < 13 ? 0.5f + 0.4f * sinf(u * 9) : 0);
  if (u > 18) for (int i = 0; i < 3; i++) g.linie(190 + i * 4, 196 + i * 8, 186 + i * 6, 220 + i * 6, 1.4f, FARBE(0xC0392B), 140);
  g.kameraAus();
  vers(g, {"Ein Fischer wohnte mit seiner Frau", "in einer armen Hütte am Meer."}, aufzu(u, 0.4f, 4.6f));
  blase(g, 200, 50, {"Hör, Fischer, lass mich leben!", "Ich bin kein richtiger Butt,", "ich bin ein verwunschener Prinz."}, aufzu(u, 7.4f, 13), 200, 150);
  blase(g, 90, 50, {"Einen Butt, der sprechen kann,", "lass ich gern wieder schwimmen."}, aufzu(u, 13.4f, 17.6f), 76, 120);
  vers(g, {"Da schwamm der Butt in die Tiefe."}, aufzu(u, 18, 23.6f));
}

// 2) Ilsebill: "Hast du dir nichts gewuenscht?"
static void szeneWunsch(Gfx &g, float u) {
  pisspott(g, u);
  ilse(g, 0, 150, 222, 0.88f, -1, u, u > 6.4f ? 0.5f + 0.3f * sinf(u * 9) : 0, u > 6.4f ? 2.2f : 0.4f, 1);
  fischer(g, 96, 222, 0.88f, 1, u, u > 0.4f && u < 6 ? 0.4f : 0);
  g.kameraAus();
  blase(g, 90, 50, {"Ich hab einen Butt gefangen,", "der war ein verwunschener Prinz.", "Ich ließ ihn wieder schwimmen."}, aufzu(u, 0.4f, 6), 96, 140);
  blase(g, 190, 50, {"Hast du dir denn nichts", "gewünscht? Hier im Pisspott", "zu wohnen ist so eklig!"}, aufzu(u, 6.4f, 13), 150, 140);
  blase(g, 190, 50, {"Geh hin und ruf ihn. Sag ihm,", "wir wollen eine kleine Hütte!"}, aufzu(u, 13.4f, 21.6f), 150, 140);
}

// der Gang ans Meer: rufen, der Butt kommt, die Antwort
static void szeneRufen(Gfx &g, float u, int st, const char *will1, const char *will2, const char *ant1, const char *ant2,
                       const char *v1, const char *v2) {
  meer(g, u, st);
  ufer(g);
  float zittern = st >= 4 ? sinf(u * 30) * 1 : 0;
  fischer(g, 60 + zittern, 198, 0.88f, 1, u, (u > 6 && u < 13) || (u > 14 && u < 17.6f) ? 0.5f + 0.2f * sinf(u * 8) : 0,
          u > 6 && u < 13 ? 2.4f : 0.6f, -1, u > 6 && u < 13 ? 2.4f : 0.2f, st >= 5 && u < 6);
  float by = lerp(230, 176, sanft(seg(u, 12.6f, 14)));
  if (u > 12.6f) butt(g, 186, by, 1, u, (u > 13.6f && u < 14.2f) || (u > 18 && u < 22) ? 0.5f + 0.4f * sinf(u * 9) : 0);
  g.kameraAus();
  vers(g, {v1, v2}, aufzu(u, 0.4f, 5.8f));
  ruf(g, u, 6, 12.6f);
  blase(g, 200, 70, {"Na, was will sie denn?"}, aufzu(u, 13.6f, 14.6f), 196, 160);
  blase(g, 90, 60, {will1, will2}, aufzu(u, 14.8f, 17.6f), 70, 120);
  blase(g, 200, 70, {ant1, ant2}, aufzu(u, 18, 23.6f), 196, 160);
}

// zu Hause: Freude, dann neuer Wunsch
static void szeneZuhause(Gfx &g, float u, int st, const char *a1, const char *a2, const char *w1, const char *w2,
                         const char *w3) {
  if (st == 1) huette(g, u);
  else saal(g, u, st);
  float ix = st >= 3 ? 142 : 160;
  float iy = st == 3 ? 184 : st == 4 ? 172 : st == 5 ? 160 : 222;
  float is = st >= 3 ? 0.75f : 0.88f;
  if (st == 4) for (int i = 0; i < 2; i++) {  // Trabanten: Riesen und Zwerge
    RiesePose r; r.farbe = 1; r.armV = 0.3f;
    riese(g, i ? 250 : 34, 228, 0.42f, i ? -1 : 1, r);
  }
  if (st == 2 || st == 3) for (int i = 0; i < 2; i++) {  // Diener und Soldaten
    Pose p = posis(DIENER, u);
    p.armV = 0.4f;
    schneiderlein(g, i ? 244 : 40, 222, 0.7f, i ? -1 : 1, p);
  }
  if (st == 5) for (int i = 0; i < 6; i++) {  // Kerzen
    float x = 30 + i * 46;
    g.rechteck(x - 2, 180, 4, 22, WEISS);
    g.ellipse(x, 176, 2, 4 + sinf(u * 9 + i), FARBE(0xFFB83D));
  }
  ilse(g, st, ix, iy, is, st >= 3 ? 1 : -1, u, (u > 6.4f) ? 0.5f + 0.3f * sinf(u * 9) : 0.3f, u > 6.4f ? 2.2f : 0.8f, u > 6.4f ? 1 : 0.3f);
  fischer(g, st >= 3 ? 70 : 104, 222, 0.85f, 1, u, u > 0.4f && u < 6 ? 0.5f : 0, u < 6 ? 1.6f : 0.4f);
  g.kameraAus();
  blase(g, 90, 60, {a1, a2}, aufzu(u, 0.4f, 6), st >= 3 ? 76 : 104, 140);
  if (w3) blase(g, 190, 50, {w1, w2, w3}, aufzu(u, 6.4f, 19.6f), ix, iy - 80 * is);
  else blase(g, 190, 50, {w1, w2}, aufzu(u, 6.4f, 19.6f), ix, iy - 80 * is);
}

static void szenePapstNacht(Gfx &g, float u) {
  saal(g, u, 5);
  // Morgenroete im Fenster: die Sonne geht auf
  g.rundRechteck(30, 40, 50, 60, 20, mischeFarbe(FARBE(0x2A2A4E), FARBE(0xFFC890), seg(u, 6, 10)));
  sonne(g, 55, lerp(110, 70, seg(u, 6, 10)), 8, u);
  ilse(g, 5, 142, 160, 0.75f, 1, u, u > 10.4f ? 0.6f + 0.3f * sinf(u * 9) : 0, u > 10.4f ? 2.6f : 0.8f, u > 10.4f ? 1 : 0.4f);
  float fall = seg(u, 17, 18.4f);
  Pose p = posis(FISCHER, u);
  p.mund = 0.9f;
  p.armV = 2.6f;
  p.armH = 2.6f;
  schneiderlein(g, 70, lerp(222, 232, fall), 0.85f, 1, p);
  if (fall > 0 && u < 20) for (int i = 0; i < 4; i++) g.stern(70 + cosf(i * 1.57f + u * 3) * 16, 140, 3, 1.2f, 4, u * 6, GOLD);
  g.kameraAus();
  vers(g, {"Nachts konnte sie nicht schlafen.", "Am Morgen sah sie die Sonne", "aufgehen..."}, aufzu(u, 0.4f, 10));
  blase(g, 190, 40, {"Kann ich nicht auch Sonne und", "Mond aufgehen lassen? Mann,", "ich will werden wie der liebe Gott!"}, aufzu(u, 10.4f, 17), 142, 90);
  vers(g, {"Da fiel der Mann vor Schreck", "aus dem Bett."}, aufzu(u, 17.4f, 21.8f));
}

static void szeneSturm(Gfx &g, float u) {
  meer(g, u, 6);
  ufer(g);
  float zittern = sinf(u * 30) * 1.4f;
  fischer(g, 60 + zittern, 198, 0.88f, 1, u, u > 5.4f && u < 12 ? 0.8f : u > 13 && u < 17 ? 0.6f : 0, u > 5.4f && u < 12 ? 2.6f : 0.6f, -1,
          u > 5.4f && u < 12 ? 2.6f : 0.2f);
  float by = lerp(230, 170, sanft(seg(u, 12, 13.4f)));
  if (u > 12) butt(g, 186, by, 1, u, u > 17.4f && u < 25 ? 0.5f + 0.4f * sinf(u * 9) : 0);
  g.kameraAus();
  vers(g, {"Draußen tobte ein Sturm. Die Wellen", "waren so hoch wie Kirchtürme."}, aufzu(u, 0.4f, 5.2f));
  ruf(g, u, 5.4f, 12);
  blase(g, 90, 60, {"Ach, sie will werden", "wie der liebe Gott."}, aufzu(u, 13, 17), 70, 120);
  blase(g, 200, 70, {"Geh nur hin - sie sitzt schon", "wieder in ihrem Pisspott."}, aufzu(u, 17.4f, 25.6f), 196, 160);
}

static void szeneEnde(Gfx &g, float u) {
  pisspott(g, u);
  ilse(g, 0, 160, 222, 0.88f, -1, u, 0, 0.4f, 0.3f);
  fischer(g, 104, 222, 0.88f, 1, u, 0.2f, 0.4f);
  g.kameraAus();
  vers(g, {"Da saßen sie wieder in ihrer", "alten Hütte - und dort sitzen sie", "noch bis auf den heutigen Tag."}, aufzu(u, 0.4f, 12));
  endeBild(g, u, 14, 142, 170);
}

static void bild(Gfx &g, float t) {
  if (t < P2) szeneButt(g, t - P1);
  else if (t < P3) szeneWunsch(g, t - P2);
  else if (t < P4) szeneRufen(g, t - P3, 1, "Meine Frau will eine", "kleine Hütte haben.", "Geh nur hin -", "sie hat sie schon.",
                              "Das Meer war ganz grün und gelb", "und gar nicht mehr so blank.");
  else if (t < P5) szeneZuhause(g, t - P4, 1, "Sieh nur, Frau,", "ist das nicht hübsch?", "Die Hütte ist viel zu eng!",
                                "Ich will in einem großen", "steinernen Schloss wohnen!");
  else if (t < P6) szeneRufen(g, t - P5, 2, "Sie will in einem großen", "steinernen Schloss wohnen.", "Geh nur hin - sie steht",
                              "schon vor der Tür.", "Das Meer war violett und", "dunkelblau und grau und dick.");
  else if (t < P7) szeneZuhause(g, t - P6, 2, "Nun wollen wir zufrieden", "sein, Frau!", "Steh auf, Mann! Geh zum Butt -",
                                "wir wollen König sein über", "das ganze Land!");
  else if (t < P8) szeneRufen(g, t - P7, 3, "Ach, sie will", "König werden.", "Geh nur hin -", "sie ist es schon.",
                              "Das Meer war ganz schwarzgrau", "und stank von unten herauf.");
  else if (t < P9) szeneZuhause(g, t - P8, 3, "Ach Frau, wie steht dir", "das schön, dass du König bist!", "König bin ich nun -",
                                "jetzt will ich", "Kaiser werden!");
  else if (t < P10) szeneRufen(g, t - P9, 4, "Ach, sie will", "Kaiser werden.", "Geh nur hin -", "sie ist es schon.",
                               "Das Meer war ganz schwarz und", "kochte, dass es Blasen warf.");
  else if (t < P11) szeneZuhause(g, t - P10, 4, "Frau, was bist du nun", "für ein großer Kaiser!", "Ich bin Kaiser -",
                                 "nun will ich auch", "Papst werden!");
  else if (t < P12) szeneRufen(g, t - P11, 5, "Ach, sie will", "Papst werden.", "Geh nur hin -", "sie ist es schon.",
                               "Das Meer tobte, und am Himmel", "stand ein rotes Gewitter.");
  else if (t < P13) szenePapstNacht(g, t - P12);
  else if (t < P14) szeneSturm(g, t - P13);
  else szeneEnde(g, t - P14);
}

// =====================================================================
// Musik
// =====================================================================
// Seemannsweise (G-Dur, schaukelnd im Dreiertakt)
static const Ton3 MEL_FISCHER[] = {
  {67, 1, 43}, {71, .5, 0}, {74, 1.5, 0}, {72, 1, 45}, {71, .5, 0}, {69, 1.5, 0}, {67, 1, 43}, {69, .5, 0}, {71, 1, 0},
  {72, .5, 48}, {74, 1.5, 0}, {76, 1, 47}, {74, .5, 0}, {72, 1, 0}, {71, .5, 43}, {69, 1, 50}, {67, 2, 43}};
static void ruf(Synth &s, float x, float t0, float t1, float staerke) {
  const int m[] = {74, 74, 76, 74, 71, 72, 74, 71};
  for (int i = 0; i < 8; i++) if (bei(x + i * 0.75f, t0, t1)) s.note(x + i * 0.75f, m[i], 0.5f * staerke);
}
static void wellen(Synth &s, float a, float d, float st, float t0, float t1) {
  for (float x = a; x < a + d; x += 2.4f - st * 0.25f) if (bei(x, t0, t1)) s.rauschen(x, 2.2f, TIEFPASS, 500 + st * 150, 0.04f + st * 0.025f, 0.7f, 0.8f, 200);
}
static void ton(Synth &s, float t0, float t1) {
  float a = P1;
  wellen(s, a, 24, 0, t0, t1);
  spieleMelodie(s, M(MEL_FISCHER), a + 0.5f, a + 23.6f, 0.34f, t0, t1, 0.6f);
  a = P2;
  spieleMelodie(s, M(MEL_SPUK), a + 6.4f, a + 21.6f, 0.42f, t0, t1, 0.35f);
  const float R[] = {P3, P5, P7, P9, P11};
  for (int k = 0; k < 5; k++) {
    a = R[k];
    wellen(s, a, 24, k + 1, t0, t1);
    ruf(s, a + 6.4f, t0, t1, 1);
    if (bei(a + 18, t0, t1)) for (int j = 0; j < 5; j++) s.note(a + 18 + j * 0.07f, 84 + j * 2, 0.4f);
    if (k >= 3) for (int j = 0; j < 4; j++) if (bei(a + 2 + j * 6, t0, t1)) s.rauschen(a + 2 + j * 6, 1.2f, TIEFPASS, 300, 0.3f, 1, 0.01f);  // Donner
  }
  const float Z[] = {P4, P6, P8, P10};
  for (int k = 0; k < 4; k++) {
    a = Z[k];
    if (k < 2) spieleMelodie(s, M(MEL_F), a + 0.4f, a + 6, 0.3f, t0, t1, 0.5f);
    else spieleMelodie(s, M(MEL_KEGEL), a + 0.4f, a + 6, 0.3f, t0, t1, 0.5f);
    spieleMelodie(s, M(MEL_SPUK), a + 6.4f, a + 19.6f, 0.4f - k * 0.02f, t0, t1, 0.35f);
    if (k >= 1) for (int j = 0; j < 3; j++) if (bei(a + 0.4f + j * 0.4f, t0, t1)) s.ton(a + 0.4f + j * 0.4f, Synth::mtof(67 + j * 4), 0.15f, 0.02f, 0.6f, SAEGE, MUSIK);  // Fanfare
  }
  a = P12;
  for (int i = 0; i < 4; i++) if (bei(a + 0.4f + i * 2.5f, t0, t1)) s.glocke(a + 0.4f + i * 2.5f, 60, 0.5f);
  if (bei(a + 17.4f, t0, t1)) s.ton(a + 17.4f, 160, 0.4f, 0.002f, 0.3f, SINUS, GERAEUSCH, 50);
  a = P13;
  wellen(s, a, 26, 6, t0, t1);
  for (int j = 0; j < 8; j++) if (bei(a + 1 + j * 3.1f, t0, t1)) s.rauschen(a + 1 + j * 3.1f, 1.6f, TIEFPASS, 260, 0.4f, 1, 0.01f);
  ruf(s, a + 5.4f, t0, t1, 1.3f);
  a = P14;
  wellen(s, a, 20, 0, t0, t1);
  spieleMelodie(s, M(MEL_FISCHER), a + 0.4f, a + 13.6f, 0.38f, t0, t1, 0.6f);
  endeKlang(s, a + 14.4f, t0, t1);
}

// =====================================================================
// Startbild, Vorschau, Erkennungszeichen
// =====================================================================
static void titelSzene(Gfx &g, float t) {
  meer(g, t, 0);
  ufer(g);
  fischer(g, 66, 200, 0.9f, 1, t, 0.4f, 1.6f);
  butt(g, 190, 172, 1.1f, t, 0.3f + 0.3f * sinf(t * 3));
}
static void titelbild(Gfx &g, float t) { titelSzene(g, t); titelRahmen(g, t, "Der Fischer und seine Frau", 236, SCHRIFT_B); }
static void vorschau(Gfx &g, float t) { titelSzene(g, t); }
static void symbol(Gfx &g, float x, float y, float t) {
  butt(g, x, y - 30, 2, t, 0.3f);
  glitzern(g, x, y - 30, 46, t, 5);
}

const Maerchen FISCHER_FRAU = {"Der Fischer und seine Frau", PDAUER, FARBE(0x3E8ED8), bild, ton, titelbild, vorschau, symbol,
                               LIED(MEL_FISCHER, 0.34f), nullptr};

// Der standhafte Zinnsoldat - ein Spieluhr-Film fuer die Maerchenbox, nach Hans Christian Andersen (1838).
// Fuenfundzwanzig Zinnsoldaten, einer mit nur einem Bein; die Taenzerin aus Papier mit dem Flitterstern,
// der Kobold in der Schnupftabaksdose, der Sturz aus dem Fenster, das Papierschiffchen, die Wasserratte,
// der Fisch - und am Ende ein kleines Herz aus Zinn (das Feuer nur angedeutet).
#include "erzaehlen.h"

// ---------- Ablauf (Sekunden) ----------
static const float P1 = 0, P2 = 22, P3 = 44, P4 = 66, P5 = 90, P6 = 114, P7 = 138, P8 = 162, P9 = 188, P10 = 210,
                   P11 = 234, P12 = 258, PDAUER = 280;

static FrauLook taenzerinLook() {
  FrauLook l; l.kleid = WEISS; l.haar = FARBE(0x3A2A20); l.frisur = 2; l.glanz = true; l.rockBreite = 1.4f; l.schuh = FARBE(0xFF9AC0);
  return l;
}
static FrauLook koechinLook() { FrauLook l; l.kleid = FARBE(0x6E8EB8); l.schuerze = WEISS; l.haar = FARBE(0xC98A3A); l.frisur = 3; l.tuch = WEISS; return l; }
static const FrauLook TAENZERIN = taenzerinLook(), KOECHIN = koechinLook();
static Aussehen jungeLook(int i) {
  Aussehen a; a.weste = i ? FARBE(0x5A7A3A) : FARBE(0x3E7FDC); a.haar = i ? FARBE(0x3A2A20) : FARBE(0xE0A23C); a.muetze = 1;
  return a;
}
static const Aussehen JUNGE = jungeLook(0), STRASSE1 = jungeLook(1), STRASSE2 = jungeLook(0);
static const Farbe ROT_R = FARBE(0xC0392B), BLAU_H = FARBE(0x2E3A8E), ZINN = FARBE(0xA8ACB8);

// =====================================================================
// Figuren
// =====================================================================
// der Zinnsoldat (einbeinig, Gewehr geschultert); s = Groesse (1 = ca. 60 px)
static void soldat(Gfx &g, float x, float y, float s, float t, float kipp = 0, bool einbein = true, uint8_t a = 255) {
  float c = cosf(kipp), sn = sinf(kipp);
  auto P = [&](float dx, float dy, float &ox, float &oy) { ox = x + (dx * c - dy * sn) * s; oy = y + (dx * sn + dy * c) * s; };
  float ax, ay, bx, by;
  g.kontur(1.2f * s, OUT);
  P(-2, -2, ax, ay); P(-2, -22, bx, by);
  g.linie(ax, ay, bx, by, 5 * s, BLAU_H, a);
  if (!einbein) { P(4, -2, ax, ay); P(4, -22, bx, by); g.linie(ax, ay, bx, by, 5 * s, BLAU_H, a); }
  P(-2, -1, ax, ay);
  g.ellipse(ax, ay, 4 * s, 2 * s, SCHWARZ, a);
  float k[8];
  P(-7, -22, k[0], k[1]); P(9, -22, k[2], k[3]); P(8, -42, k[4], k[5]); P(-6, -42, k[6], k[7]);
  g.polygon(k, 4, ROT_R, a);
  P(1, -42, ax, ay);
  g.kreis(ax, ay - 0 * s, 0, HAUT, a);
  P(1, -48, ax, ay);
  g.kreis(ax, ay, 6.5f * s, HAUT, a);
  float sh[8];
  P(-5, -52, sh[0], sh[1]); P(7, -52, sh[2], sh[3]); P(6, -66, sh[4], sh[5]); P(-4, -66, sh[6], sh[7]);
  g.polygon(sh, 4, SCHWARZ, a);
  g.konturAus();
  P(1, -64, ax, ay);
  g.kreis(ax, ay, 2 * s, GOLD, a);
  P(3, -48, ax, ay);
  g.kreis(ax, ay, 1.2f * s, SCHWARZ, a);
  P(0, -45, ax, ay);
  g.ellipse(ax, ay, 3 * s, 1 * s, FARBE(0x5A3A2A), a);
  // Gewehr
  P(10, -26, ax, ay); P(12, -64, bx, by);
  g.linie(ax, ay, bx, by, 2 * s, FARBE(0x5A3A2A), a);
  P(12, -64, ax, ay); P(12, -72, bx, by);
  g.linie(ax, ay, bx, by, 1 * s, ZINN, a);
  P(-1, -36, ax, ay);
  g.linie(ax - 6 * s, ay, ax + 6 * s, ay + 4 * s, 1.2f * s, WEISS, a);
}
static void taenzerin(Gfx &g, float x, float y, float s, float dir, float t, float drehen = 0, uint8_t a = 255) {
  // das hochgehobene Bein (hinter dem Rock)
  g.linie(x - 4 * s * dir, y - 22 * s, x - 22 * s * dir, y - 44 * s, 3 * s, HAUT, a);
  FrauPose p = fposis(TAENZERIN, t, 1);
  p.armV = 2.6f;
  p.armH = 2.6f;
  p.drehen = drehen;
  frau(g, x, y, s, dir, p);
  // blaues Band mit dem Flitterstern
  g.linie(x - 10 * s, y - 50 * s, x + 10 * s, y - 40 * s, 2.4f * s, FARBE(0x3E7FDC), a);
  g.stern(x + 2 * s, y - 45 * s, 5 * s, 2 * s, 5, t, GOLD, a);
  g.stern(x + 2 * s, y - 45 * s, 3 * s, 1 * s, 4, t * 2, WEISS, a);
}
static void dose(Gfx &g, float x, float y, float s, float t, float auf, float mund = 0) {
  g.kontur(1.4f * s, OUT);
  g.rundRechteck(x - 18 * s, y - 20 * s, 36 * s, 20 * s, 3 * s, FARBE(0x6E4A2E));
  g.konturAus();
  if (auf > 0) {
    // Kobold auf der Feder
    float h = auf * 40 * s + sinf(t * 10) * 3 * s * auf;
    for (int i = 0; i < 4; i++) g.bogen(x, y - 20 * s - i * h / 4, 5 * s, 0, 3.14f, 1.4f * s, ZINN);
    g.kontur(1.2f * s, OUT);
    g.kreis(x, y - 24 * s - h, 11 * s, SCHWARZ);
    g.konturAus();
    g.kreis(x - 4 * s, y - 26 * s - h, 2.4f * s, FARBE(0xFFD54A));
    g.kreis(x + 4 * s, y - 26 * s - h, 2.4f * s, FARBE(0xFFD54A));
    g.ellipse(x, y - 18 * s - h, 5 * s, (1 + mund * 3) * s, FARBE(0xC0392B));
    float d1[] = {x - 8 * s, y - 30 * s - h, x - 12 * s, y - 42 * s - h, x - 4 * s, y - 33 * s - h};
    float d2[] = {x + 8 * s, y - 30 * s - h, x + 12 * s, y - 42 * s - h, x + 4 * s, y - 33 * s - h};
    g.polygon(d1, 3, SCHWARZ);
    g.polygon(d2, 3, SCHWARZ);
    g.rundRechteck(x - 18 * s, y - 34 * s, 36 * s, 6 * s, 2 * s, FARBE(0x8E6A44), 255);
  } else g.rundRechteck(x - 19 * s, y - 24 * s, 38 * s, 6 * s, 2 * s, FARBE(0x8E6A44));
}
static void ratte(Gfx &g, float x, float y, float s, float dir, float t, float mund) {
  auto PX = [&](float d) { return x + d * s * dir; };
  auto PY = [&](float d) { return y + d * s; };
  g.kurve(PX(-12), PY(-5), PX(-26), PY(-2), PX(-34), PY(-10), 1.6f * s, FARBE(0xE8A0A8));
  g.kontur(1 * s, OUT);
  g.ellipse(PX(0), PY(-8), 14 * s, 8 * s, FARBE(0x6E6E78));
  g.ellipse(PX(13), PY(-10), 8 * s, 5 * s, FARBE(0x6E6E78), 255, 0.3f * dir);
  g.konturAus();
  g.kreis(PX(8), PY(-16), 4 * s, FARBE(0xE8A0A8));
  g.kreis(PX(15), PY(-13), 1.6f * s, FARBE(0xFFD54A));
  if (mund > 0.05f) g.ellipse(PX(20), PY(-8), 2 * s, (0.6f + mund * 2) * s, FARBE(0x7A2A2A));
  for (int i = -1; i <= 1; i++) g.linie(PX(20), PY(-10), PX(28), PY(-10 + i * 3), 0.6f * s, OUT);
}
static void fisch(Gfx &g, float x, float y, float s, float dir, float t, float maul = 0) {
  auto PX = [&](float d) { return x + d * s * dir; };
  auto PY = [&](float d) { return y + d * s; };
  float w = sinf(t * 5) * 4;
  g.kontur(1.4f * s, OUT);
  float sw[] = {PX(-30), PY(0), PX(-46), PY(-14 + w), PX(-46), PY(14 + w)};
  g.polygon(sw, 3, FARBE(0x6E8EA8));
  g.ellipse(PX(0), PY(0), 34 * s, 18 * s, FARBE(0x8EAAC8));
  g.konturAus();
  for (int i = 0; i < 6; i++) g.bogen(PX(-16 + i * 7), PY(0), 5 * s, 4.0f, 5.4f, 1 * s, FARBE(0x6E8EA8));
  g.kreis(PX(20), PY(-6), 3 * s, WEISS);
  g.kreis(PX(21), PY(-6), 1.6f * s, SCHWARZ);
  if (maul > 0) g.ellipse(PX(32), PY(4), 6 * s, maul * 8 * s, FARBE(0x3A2A3A));
}

// =====================================================================
// Orte
// =====================================================================
static void kinderzimmer(Gfx &g, float t, float nacht, bool fenster = false, float fensterAuf = 0) {
  g.verlauf(0, 170, mischeFarbe(FARBE(0xE8D8C0), FARBE(0x3A3450), nacht), mischeFarbe(FARBE(0xD8C0A0), FARBE(0x2A2440), nacht));
  for (int i = 0; i < 8; i++) g.rechteck(i * 40, 0, 2, 170, mischeFarbe(FARBE(0xD0BCA0), FARBE(0x2E2848), nacht));
  // Fenster
  g.kontur(1.6f, OUT);
  g.rundRechteck(196, 20, 70, 80, 4, HOLZ);
  g.konturAus();
  g.rundRechteck(201, 25, 60, 70, 2, mischeFarbe(FARBE(0x9FD8FF), FARBE(0x22285A), nacht));
  if (nacht > 0.5f) g.kreis(240, 44, 7, FARBE(0xFFF1B0));
  if (fensterAuf > 0) g.rundRechteck(201, 25, 30 * (1 - fensterAuf * 0.8f), 70, 2, FARBE(0xC8E8FF));
  else g.rechteck(229, 25, 3, 70, HOLZ);
  // Tischplatte (gross, Spielzeug-Perspektive)
  g.rechteck(0, 170, 284, 70, mischeFarbe(FARBE(0xA8744A), FARBE(0x4A3426), nacht));
  g.rechteck(0, 170, 284, 6, mischeFarbe(FARBE(0xC8945A), FARBE(0x5A4436), nacht));
  (void)fenster;
}
static void papierschloss(Gfx &g, float x, float y, float t) {
  g.kontur(1.2f, OUT);
  g.rechteck(x - 40, y - 60, 80, 60, FARBE(0xF6F2EA));
  for (int i = 0; i < 2; i++) {
    float tx = x - 40 + i * 64;
    g.rechteck(tx, y - 80, 16, 80, FARBE(0xF6F2EA));
    float d[] = {tx - 3, y - 80, tx + 8, y - 96, tx + 19, y - 80};
    g.polygon(d, 3, FARBE(0xE8B0C0));
  }
  g.konturAus();
  g.rundRechteck(x - 14, y - 40, 28, 40, 12, FARBE(0x2A2A3A));
  // Spiegel-See mit Wachsschwaenen
  g.ellipse(x - 70, y - 2, 26, 6, FARBE(0xBFE6FF));
  for (int i = 0; i < 2; i++) {
    float sx = x - 80 + i * 18 + sinf(t + i) * 2;
    g.ellipse(sx, y - 6, 5, 3, WEISS);
    g.kurve(sx + 3, y - 7, sx + 5, y - 12, sx + 4, y - 14, 1.4f, WEISS);
  }
  for (int i = 0; i < 3; i++) laubbaum(g, x + 50 + i * 12, y, 0.2f);
}
static void strasse(Gfx &g, float t, float regen, float scroll = 0) {
  himmel(g, 0.3f * regen, t, 150);
  for (int i = 0; i < 4; i++) {
    float x = fmodf(i * 90 - scroll * 0.3f + 900, 360) - 60;
    g.kontur(1.4f, OUT);
    g.rechteck(x, 20, 70, 140, i & 1 ? FARBE(0xE8D8C0) : FARBE(0xD8C0B0));
    g.konturAus();
    for (int r = 0; r < 3; r++) for (int c = 0; c < 2; c++) g.rundRechteck(x + 12 + c * 30, 30 + r * 40, 16, 22, 2, FARBE(0x9FD8FF));
  }
  g.rechteck(0, 160, 284, 80, FARBE(0x8E8A86));
  for (int r = 0; r < 4; r++) for (int c = 0; c < 12; c++) g.ellipse(fmodf(c * 26 + (r & 1) * 13 - scroll + 1300, 312) - 14, 172 + r * 18, 11, 6, FARBE(0x9E9A96));
  g.rechteck(0, 214, 284, 14, FARBE(0x5A6E8E));  // Rinnstein
  if (regen > 0) for (int i = 0; i < 40; i++) {
    float x = fmodf(zufall(i) * 300 + t * 20, 300), y = fmodf(zufall(i + 40) * 240 + t * 200, 240);
    g.linie(x, y, x - 2, y + 8, 1, FARBE(0xBFE6FF), (uint8_t)(200 * regen));
  }
}
static void papierboot(Gfx &g, float x, float y, float s, float t, float sinkt = 0) {
  float w = sinf(t * 3) * 3;
  float b[] = {x - 28 * s, y - 10 * s + w, x + 28 * s, y - 10 * s - w, x + 18 * s, y + 4 * s, x - 18 * s, y + 4 * s};
  g.kontur(1.2f * s, OUT);
  g.polygon(b, 4, FARBE(0xF2EEE4), (uint8_t)(255 * (1 - sinkt * 0.6f)));
  float m[] = {x - 10 * s, y - 10 * s, x, y - 26 * s, x + 10 * s, y - 10 * s};
  g.polygon(m, 3, FARBE(0xE8E4DC), (uint8_t)(255 * (1 - sinkt * 0.6f)));
  g.konturAus();
  for (int i = 0; i < 3; i++) g.linie(x - 20 * s + i * 8 * s, y - 4 * s, x - 14 * s + i * 8 * s, y - 4 * s, 0.8f * s, FARBE(0x5A5A6E));
}
static void kueche(Gfx &g, float t, float feuer) {
  g.verlauf(0, 200, FARBE(0xE0C49A), FARBE(0xC0A07A));
  g.rechteck(0, 200, 284, 40, FARBE(0x8E6A46));
  g.kontur(2, OUT);
  g.rundRechteck(170, 90, 100, 112, 6, FARBE(0x5A5A6E));
  g.konturAus();
  g.rundRechteck(186, 130, 68, 56, 18, FARBE(0x2E2620));
  if (feuer > 0) for (int i = 0; i < 4; i++) {
    float h = (16 + 6 * sinf(t * 9 + i * 1.7f)) * feuer;
    float f[] = {196 + i * 14 - 7, 184, 196 + i * 14 + 7, 184, 196 + i * 14 + sinf(t * 7 + i) * 2, 184 - h};
    g.polygon(f, 3, i & 1 ? FARBE(0xFFD54A) : FARBE(0xFF8A2A));
  }
  if (feuer > 0) g.kreis(220, 160, 50, FARBE(0xFFB050), (uint8_t)(40 * feuer));
  g.kontur(1.4f, OUT);
  g.rechteck(20, 170, 110, 8, HOLZ);
  g.konturAus();
  g.rechteck(28, 178, 6, 24, HOLZ_D);
  g.rechteck(116, 178, 6, 24, HOLZ_D);
}

// =====================================================================
// Szenen
// =====================================================================
static void szeneSchachtel(Gfx &g, float u) {
  kinderzimmer(g, u, 0);
  bool auf = u > 3;
  if (!auf) {
    g.kontur(1.4f, OUT);
    g.rundRechteck(90, 150, 100, 40, 4, FARBE(0xE8C860));
    g.konturAus();
  }
  if (auf) {
    for (int i = 0; i < 12; i++) soldat(g, 30 + i * 18, 212, 0.62f, u, 0, false);
    soldat(g, 250, 214, 0.75f, u, 0, true);
  }
  Pose p = posis(JUNGE, u);
  p.mund = u > 3 && u < 8 ? 0.8f : 0.3f;
  p.armV = u > 3 && u < 8 ? 2.6f : 1.2f;
  p.armH = u > 3 && u < 8 ? 2.6f : 0.2f;
  schneiderlein(g, 40, 172, 0.9f, 1, p);
  g.kameraAus();
  blase(g, 110, 40, {"Zinnsoldaten!"}, aufzu(u, 3.4f, 8), 50, 90);
  vers(g, {"Fünfundzwanzig Zinnsoldaten - alle", "Brüder, aus einem alten Löffel", "gegossen."}, aufzu(u, 8.4f, 14));
  vers(g, {"Einer hatte nur ein Bein - das", "Zinn hatte nicht gereicht. Aber er", "stand so fest wie die anderen."}, aufzu(u, 14.4f, 21.6f));
}

static void szeneTaenzerin(Gfx &g, float u) {
  kinderzimmer(g, u, 0);
  papierschloss(g, 126, 210, u);
  taenzerin(g, 126, 210, 0.5f, 1, u);
  soldat(g, 234, 214, 0.75f, u);
  g.kameraAus();
  vers(g, {"Auf dem Tisch stand ein Schloss", "aus Papier, mit einem Spiegel-See", "und Schwänen aus Wachs."}, aufzu(u, 0.4f, 8));
  vers(g, {"In der Tür stand eine kleine", "Tänzerin mit einem blauen Band und", "einem glitzernden Flitterstern."}, aufzu(u, 8.4f, 15));
  vers(g, {"Ein Bein hob sie so hoch, dass der", "Soldat meinte, sie hätte - wie er -", "nur ein einziges."}, aufzu(u, 15.4f, 21.6f));
}

static void szeneLiebe(Gfx &g, float u) {
  kinderzimmer(g, u, 0.3f);
  papierschloss(g, 126, 210, u);
  taenzerin(g, 126, 210, 0.5f, 1, u);
  dose(g, 204, 214, 1, u, 0);
  soldat(g, 238, 214, 0.75f, u);
  herzen(g, 220, 150, u, 3, 14);
  g.kameraAus();
  blase(g, 200, 70, {"Das wäre eine Frau für mich!", "Aber sie ist so vornehm, sie", "wohnt in einem Schloss..."}, aufzu(u, 0.4f, 9), 236, 150);
  vers(g, {"Er legte sich hinter die", "Schnupftabaksdose und sah", "sie unverwandt an."}, aufzu(u, 9.4f, 21.6f));
}

static void szeneNacht(Gfx &g, float u) {
  kinderzimmer(g, u, 1);
  papierschloss(g, 126, 210, u);
  taenzerin(g, 126, 210, 0.5f, 1, u);
  dose(g, 204, 214, 1, u, 0);
  soldat(g, 238, 214, 0.75f, u);
  // die Spielsachen spielen: Nussknacker schlaegt Purzelbaeume, Kreisel tanzt
  float kx = 40 + fmodf(u * 20, 60), kw = u * 6;
  soldat(g, kx, 200, 0.5f, u, kw, false);
  float rk = u * 8;
  g.kontur(1, OUT);
  float kr[] = {100 + cosf(rk) * 8, 210, 100, 190, 100 - cosf(rk) * 8, 210};
  g.polygon(kr, 3, FARBE(0xE8473D));
  g.konturAus();
  vogel(g, 60, 120 + sinf(u * 2) * 10, 1.2f, u, 1);
  g.kameraAus();
  vers(g, {"Nachts spielten die Spielsachen:", "Besuch, Krieg und Ball. Der Nuss-", "knacker schlug Purzelbäume."}, aufzu(u, 0.4f, 12));
  vers(g, {"Nur der Zinnsoldat und die", "Tänzerin rührten sich nicht."}, aufzu(u, 12.4f, 23.6f));
}

static void szeneKobold(Gfx &g, float u) {
  kinderzimmer(g, u, 1);
  papierschloss(g, 126, 210, u);
  taenzerin(g, 126, 210, 0.5f, 1, u);
  float auf = federnd(seg(u, 2, 3));
  dose(g, 200, 214, 1.1f, u, auf, (u > 4.4f && u < 11) || (u > 15.4f && u < 22) ? 0.5f + 0.4f * sinf(u * 9) : 0);
  soldat(g, 240, 214, 0.75f, u);
  g.kameraAus();
  vers(g, {"Als die Uhr zwölf schlug - klapp!", "sprang der Deckel der Dose auf."}, aufzu(u, 0.4f, 4));
  blase(g, 160, 40, {"Zinnsoldat! Sieh doch nicht", "immer nach dem, was dich", "nichts angeht!"}, aufzu(u, 4.4f, 11), 200, 140);
  vers(g, {"Aber der Zinnsoldat tat,", "als hörte er es nicht."}, aufzu(u, 11.4f, 15));
  blase(g, 160, 40, {"Ja, warte nur bis morgen!"}, aufzu(u, 15.4f, 23.6f), 200, 140);
}

static void szeneFenster(Gfx &g, float u) {
  float auf = seg(u, 6, 7);
  kinderzimmer(g, u, 0, true, auf);
  float fall = seg(u, 8, 11);
  if (fall <= 0) soldat(g, 240, 98, 0.6f, u, sinf(u * 2) * 0.05f);
  if (u < 8) {
    Pose p = posis(JUNGE, u);
    p.armV = u < 4 ? 2.4f : 0.4f;
    schneiderlein(g, 160, 172, 0.9f, 1, p);
  }
  if (fall > 0 && fall < 1) soldat(g, lerp(240, 270, fall), lerp(98, 260, hinein(fall)), 0.6f, u, 3.14f * fall);
  g.kameraAus();
  vers(g, {"Am Morgen stellte der Junge ihn", "ins Fenster..."}, aufzu(u, 0.4f, 5.6f));
  vers(g, {"Da flog das Fenster auf - ob der", "Wind oder der Kobold, wer weiß -", "und er stürzte hinab!"}, aufzu(u, 6, 13.6f));
  vers(g, {"Drei Stockwerke tief, kopfüber.", "Das Bajonett blieb zwischen den", "Pflastersteinen stecken."}, aufzu(u, 14, 23.6f));
}

static void szeneStrasse(Gfx &g, float u) {
  float regen = u < 8 ? 1 : 1 - seg(u, 8, 10);
  strasse(g, u, regen);
  soldat(g, 150, 168, 0.8f, u, u < 12 ? 3.14f : 0);
  if (u > 10) {
    Pose a = posis(STRASSE1, u), b = posis(STRASSE2, u, 1);
    float k = seg(u, 10, 13);
    a.lauf = b.lauf = k < 1 ? fmodf(u * 1.6f, 1) : -1;
    a.mund = u > 13.4f && u < 18 ? 0.7f : 0.4f;
    b.armV = u > 16 ? 1.6f : 0.4f;
    schneiderlein(g, lerp(-20, 110, k), 222, 0.85f, 1, a);
    schneiderlein(g, lerp(320, 196, k), 222, 0.85f, -1, b);
    if (u > 17) papierboot(g, 172, 200, 0.8f, u);
  }
  g.kameraAus();
  vers(g, {"Er rief nicht um Hilfe - in", "Uniform schreit man nicht laut.", "Dann fing es an zu regnen."}, aufzu(u, 0.4f, 9.6f));
  blase(g, 110, 60, {"Sieh mal, ein Zinnsoldat!", "Der muss Schiff fahren!"}, aufzu(u, 13.4f, 18.6f), 110, 140);
  vers(g, {"Aus Zeitungspapier falteten sie", "ein Boot und setzten ihn hinein."}, aufzu(u, 19, 23.6f));
}

static void szeneRatte(Gfx &g, float u) {
  g.verlauf(0, 240, FARBE(0x2A2A34), FARBE(0x1A1A22));
  for (int i = 0; i < 6; i++) g.rundRechteck(i * 50 - 10, 20, 46, 30, 4, FARBE(0x3A3A44));
  g.rechteck(0, 200, 284, 40, FARBE(0x2E3A4E));
  for (int i = 0; i < 8; i++) g.linie(fmodf(i * 37 + u * 30, 300) - 10, 206 + (i % 3) * 10, fmodf(i * 37 + u * 30, 300) + 6, 206 + (i % 3) * 10, 1.4f, WEISS, 60);
  float bx = 160 + sinf(u * 0.5f) * 10;
  papierboot(g, bx, 204, 1, u);
  soldat(g, bx, 194, 0.62f, u, sinf(u * 3) * 0.1f);
  bool jagt = u > 10;
  float rx = jagt ? lerp(70, 120, seg(u, 10, 14)) : 70;
  ratte(g, rx, 204, 1.1f, 1, u, (u > 0.4f && u < 9) || (u > 10.4f && u < 16) ? 0.5f + 0.4f * sinf(u * 10) : 0);
  if (u > 18) taenzerin(g, 230, 150, 0.3f, -1, u, 0, 140);
  g.kameraAus();
  blase(g, 80, 70, {"Hast du einen Pass?", "Zeig deinen Pass her!"}, aufzu(u, 0.4f, 9), 80, 180);
  blase(g, 80, 70, {"Haltet ihn! Haltet ihn!", "Er hat keinen Zoll bezahlt!"}, aufzu(u, 10.4f, 16), 110, 180);
  blase(g, 200, 70, {"Ach, säße doch die kleine", "Tänzerin hier bei mir im Boot!"}, aufzu(u, 17, 25.6f), 166, 150);
}

static void szeneFisch(Gfx &g, float u) {
  g.verlauf(0, 240, FARBE(0x3E6E8E), FARBE(0x1E3A5A));
  float sturz = seg(u, 2, 6);
  float by = lerp(40, 180, hinein(sturz));
  if (u < 6) {  // der Wasserfall
    g.rechteck(110, 0, 80, by, FARBE(0x8EC8F0), 160);
    for (int i = 0; i < 8; i++) g.linie(116 + i * 10, 0, 116 + i * 10, by, 1, WEISS, 120);
  }
  float sink = seg(u, 6, 11);
  papierboot(g, 150, by + 20, 1, u, sink);
  soldat(g, 150, by + 10 + sink * 20, 0.62f, u);
  float fk = seg(u, 11, 14);
  if (u > 10) fisch(g, lerp(330, 170, fk), 214, 1.2f, -1, u, u > 13 && u < 14.4f ? 1 : 0.2f);
  g.kameraAus();
  vers(g, {"Am Ende stürzte das Wasser in", "einen großen Kanal. Das Boot", "drehte sich und füllte sich."}, aufzu(u, 0.4f, 7));
  vers(g, {"Der Soldat stand bis zum Hals im", "Wasser - aufrecht wie immer."}, aufzu(u, 7.4f, 13));
  vers(g, {"Da kam ein großer Fisch - und", "schnapp! hatte er ihn verschluckt."}, aufzu(u, 14.4f, 21.6f));
}

static void szeneHeim(Gfx &g, float u) {
  if (u < 10) {
    g.verlauf(0, 240, FARBE(0x1A1A22), FARBE(0x2A2A34));
    soldat(g, 142, 170, 1, u);
    for (int i = 0; i < 4; i++) g.bogen(142, 140, 60 + i * 20, 0, 6.29f, 6, FARBE(0x3A2A3A), 120);
    g.kameraAus();
    vers(g, {"Im Fisch war es noch dunkler", "als unter der Brücke. Aber der", "Soldat lag ganz still."}, aufzu(u, 0.4f, 9.6f));
    return;
  }
  float v = u - 10;
  kueche(g, u, 0.4f);
  fisch(g, 80, 160, 1, 1, u);
  {
    FrauPose p = fposis(KOECHIN, u);
    p.mund = v > 2 && v < 7 ? 0.8f : 0.3f;
    p.armV = 1.6f;
    frau(g, 140, 222, 0.85f, -1, p);
  }
  if (v > 2) soldat(g, 110, 168, 0.6f, u);
  g.kameraAus();
  blase(g, 200, 50, {"Der Zinnsoldat!"}, aufzu(v, 2, 7), 140, 140);
  vers(g, {"Der Fisch war gefangen und", "verkauft worden - in dasselbe", "Haus! Er kam ins selbe Zimmer."}, aufzu(v, 7.4f, 13.6f));
}

static void szeneWieder(Gfx &g, float u) {
  kinderzimmer(g, u, 0.2f);
  papierschloss(g, 126, 210, u);
  taenzerin(g, 126, 210, 0.5f, 1, u);
  soldat(g, 234, 214, 0.75f, u);
  herzen(g, 200, 150, u, 4, 30);
  g.kameraAus();
  vers(g, {"Da stand die kleine Tänzerin!", "Sie war standhaft geblieben -", "wie er."}, aufzu(u, 0.4f, 10));
  vers(g, {"Er hätte fast Zinn geweint. Sie", "sahen einander an, aber sie", "sagten nichts."}, aufzu(u, 10.4f, 23.6f));
}

static void szeneOfen(Gfx &g, float u) {
  if (u < 14) {
    kueche(g, u, 1);
    if (u < 6) {
      Pose p = posis(JUNGE, u);
      p.armV = u > 3 ? 2.2f : 0.4f;
      schneiderlein(g, 150, 222, 0.9f, 1, p);
    }
    float k = seg(u, 4, 6);
    if (k < 1) soldat(g, lerp(160, 210, k), lerp(150, 176, k) - huepf(k) * 30, 0.5f, u, k * 2);
    else soldat(g, 210, 182, 0.5f, u, 0, true, 200);
    float tk = seg(u, 8, 10);
    if (u > 8) taenzerin(g, lerp(80, 226, tk), lerp(120, 182, tk) - huepf(tk) * 40, 0.3f, 1, u, tk * 6, 200);
    g.kameraAus();
    vers(g, {"Da warf ihn der Junge ohne", "Grund in den Ofen - der Kobold", "war gewiss schuld daran."}, aufzu(u, 0.4f, 7.6f));
    vers(g, {"Die Tür ging auf, der Luftzug", "fasste die Tänzerin - und sie", "flog zu ihm hinein."}, aufzu(u, 8, 13.8f));
    return;
  }
  float v = u - 14;
  kueche(g, u, 0);
  g.ellipse(220, 182, 26, 6, FARBE(0x8E8A86));
  // ein kleines Herz aus Zinn und der Flitterstern
  g.kontur(1, OUT);
  herz(g, 214, 176, 6, ZINN, 255);
  g.konturAus();
  g.stern(232, 180, 5, 2, 5, 0, FARBE(0x3A3A44));
  glitzern(g, 220, 176, 20, u, 4);
  g.kameraAus();
  vers(g, {"Am Morgen fand das Mädchen in der", "Asche ein kleines Herz aus Zinn -", "und daneben den Flitterstern."}, aufzu(v, 0.4f, 7.4f));
  endeBild(g, v, 8, 220, 176);
}

static void bild(Gfx &g, float t) {
  if (t < P2) szeneSchachtel(g, t - P1);
  else if (t < P3) szeneTaenzerin(g, t - P2);
  else if (t < P4) szeneLiebe(g, t - P3);
  else if (t < P5) szeneNacht(g, t - P4);
  else if (t < P6) szeneKobold(g, t - P5);
  else if (t < P7) szeneFenster(g, t - P6);
  else if (t < P8) szeneStrasse(g, t - P7);
  else if (t < P9) szeneRatte(g, t - P8);
  else if (t < P10) szeneFisch(g, t - P9);
  else if (t < P11) szeneHeim(g, t - P10);
  else if (t < P12) szeneWieder(g, t - P11);
  else szeneOfen(g, t - P12);
}

// =====================================================================
// Musik
// =====================================================================
// Walzer der Taenzerin (Es-Dur, Dreiertakt)
static const Ton3 MEL_WALZ[] = {
  {75, 1, 39}, {79, .5, 0}, {82, .5, 0}, {82, 1, 0}, {80, 1, 44}, {79, .5, 0}, {77, .5, 0}, {77, 1, 0},
  {79, 1, 46}, {82, .5, 0}, {84, .5, 0}, {82, 1, 0}, {79, 1, 39}, {77, .5, 0}, {75, .5, 0}, {75, 1, 0}};
// Soldatenmarsch (B-Dur, wie eine Spieldose)
static const Ton3 MEL_ZINN[] = {
  {70, .5, 46}, {70, .25, 0}, {70, .25, 0}, {74, .5, 0}, {77, .5, 0}, {77, .5, 41}, {75, .5, 0}, {74, 1, 0},
  {72, .5, 41}, {72, .25, 0}, {72, .25, 0}, {75, .5, 0}, {79, .5, 0}, {77, .5, 46}, {74, .5, 0}, {70, 1, 0}};
static void ton(Synth &s, float t0, float t1) {
  float a = P1;
  spieleMelodie(s, M(MEL_ZINN), a + 0.5f, a + 21.6f, 0.3f, t0, t1, 0.6f);
  a = P2;
  spieleMelodie(s, M(MEL_WALZ), a + 0.4f, a + 21.6f, 0.3f, t0, t1, 0.55f);
  a = P3;
  spieleMelodie(s, M(MEL_WIEGE), a + 0.4f, a + 21.6f, 0.44f, t0, t1, 0.5f);
  a = P4;
  spieleMelodie(s, M(MEL_KEGEL), a + 0.4f, a + 12, 0.3f, t0, t1, 0.45f);
  spieleMelodie(s, M(MEL_WALZ), a + 12.4f, a + 23.6f, 0.34f, t0, t1, 0.4f);
  a = P5;
  for (int i = 0; i < 12; i++) if (bei(a + 0.2f + i * 0.15f, t0, t1)) s.glocke(a + 0.2f + i * 0.15f, 72, 0.15f);
  if (bei(a + 2, t0, t1)) { s.ton(a + 2, 300, 0.4f, 0.002f, 0.3f, RECHTECK, GERAEUSCH, 900); for (int k = 0; k < 6; k++) s.ton(a + 2.1f + k * 0.08f, 600 - k * 40, 0.1f, 0.002f, 0.1f, SINUS, GERAEUSCH); }
  spieleMelodie(s, M(MEL_SPUK), a + 4.4f, a + 23.6f, 0.4f, t0, t1, 0.35f);
  a = P6;
  spieleMelodie(s, M(MEL_ZINN), a + 0.4f, a + 6, 0.3f, t0, t1, 0.45f);
  if (bei(a + 6, t0, t1)) s.rauschen(a + 6, 1.6f, BANDPASS, 800, 0.15f, 0.8f, 0.2f, 1600);
  if (bei(a + 8, t0, t1)) s.ton(a + 8, 1200, 0.2f, 0.01f, 2.8f, SINUS, GERAEUSCH, 200);
  if (bei(a + 11, t0, t1)) s.ton(a + 11, 1800, 0.25f, 0.001f, 0.1f, DREIECK, GERAEUSCH, 1400);  // klink
  a = P7;
  if (bei(a, t0, t1)) s.rauschen(a, 10, HOCHPASS, 3000, 0.06f, 1, 0.5f);
  spieleMelodie(s, M(MEL_WANDER), a + 10, a + 23.6f, 0.32f, t0, t1, 0.5f);
  a = P8;
  if (bei(a, t0, t1)) s.rauschen(a, 26, BANDPASS, 500, 0.05f, 0.7f, 1);
  for (int i = 0; i < 8; i++) if (bei(a + 0.6f + i * 0.5f, t0, t1)) s.ton(a + 0.6f + i * 0.5f, 2400, 0.05f, 0.002f, 0.05f, SINUS, GERAEUSCH, 2000);
  spieleMelodie(s, M(MEL_SPUK), a + 0.4f, a + 16, 0.4f, t0, t1, 0.35f);
  spieleMelodie(s, M(MEL_WIEGE), a + 17, a + 25.6f, 0.44f, t0, t1, 0.45f);
  a = P9;
  if (bei(a + 2, t0, t1)) s.rauschen(a + 2, 4, BANDPASS, 900, 0.2f, 0.7f, 0.5f, 300);
  if (bei(a + 13.4f, t0, t1)) s.ton(a + 13.4f, 260, 0.35f, 0.002f, 0.2f, SINUS, GERAEUSCH, 80);  // schnapp
  spieleMelodie(s, M(MEL_N), a + 6, a + 21.6f, 0.42f, t0, t1, 0.35f);
  a = P10;
  spieleMelodie(s, M(MEL_N), a + 0.4f, a + 9.6f, 0.44f, t0, t1, 0.35f);
  if (bei(a + 12, t0, t1)) for (int j = 0; j < 5; j++) s.note(a + 12 + j * 0.08f, 84 + j * 2, 0.45f);
  spieleMelodie(s, M(MEL_ZINN), a + 13, a + 23.6f, 0.3f, t0, t1, 0.5f);
  a = P11;
  spieleMelodie(s, M(MEL_WALZ), a + 0.4f, a + 23.6f, 0.34f, t0, t1, 0.55f);
  a = P12;
  if (bei(a, t0, t1)) s.rauschen(a, 14, BANDPASS, 1600, 0.04f, 1, 1);
  spieleMelodie(s, M(MEL_WIEGE), a + 0.4f, a + 13.6f, 0.46f, t0, t1, 0.45f);
  spieleMelodie(s, M(MEL_ZINN), a + 14.4f, a + 21.6f, 0.4f, t0, t1, 0.4f);
  endeKlang(s, a + 22.4f, t0, t1);
}

// =====================================================================
// Startbild, Vorschau, Erkennungszeichen
// =====================================================================
static void titelSzene(Gfx &g, float t) {
  kinderzimmer(g, t, 0.2f);
  papierschloss(g, 130, 210, t);
  taenzerin(g, 130, 210, 0.5f, 1, t);
  soldat(g, 230, 214, 0.8f, t);
}
static void titelbild(Gfx &g, float t) { titelSzene(g, t); titelRahmen(g, t, "Der standhafte Zinnsoldat", 232, SCHRIFT_B); }
static void vorschau(Gfx &g, float t) { titelSzene(g, t); }
static void symbol(Gfx &g, float x, float y, float t) {
  soldat(g, x, y, 1.2f, t);
  glitzern(g, x, y - 40, 40, t, 5);
}

const Maerchen ZINNSOLDAT = {"Der standhafte Zinnsoldat", PDAUER, FARBE(0xC0392B), bild, ton, titelbild, vorschau, symbol,
                             LIED(MEL_ZINN, 0.3f), "H. C. Andersen"};

// Frau Holle - ein Spieluhr-Film fuer die Maerchenbox, nach dem Maerchen der Brueder Grimm.
// Die Spule im Brunnen, das Brot im Ofen, der Apfelbaum, Frau Holle schuettelt die Betten (es schneit in der Welt),
// der Goldregen und das Pech, und der Hahn auf dem Brunnen: "Kikeriki, unsere goldene Jungfrau ist wieder hie."
#include "erzaehlen.h"

// ---------- Ablauf (Sekunden) ----------
static const float P1 = 0, P2 = 32, P3 = 48, P4 = 74, P5 = 96, P6 = 140, P7 = 174, P8 = 232, PDAUER = 268;

// ---------- Aussehen ----------
static FrauLook fleissigLook() {
  FrauLook l; l.kleid = FARBE(0x5B8FD7); l.schuerze = FARBE(0xF6F0E4); l.haar = FARBE(0xF2C94E); l.frisur = 1;
  return l;
}
static FrauLook goldLook() { FrauLook l = fleissigLook(); l.kleid = GOLD; l.schuerze = FARBE(0xFFF1B0); l.glanz = true; l.krone = false; return l; }
static FrauLook faulLook() {
  FrauLook l; l.kleid = FARBE(0xC9884A); l.schuerze = FARBE(0xE8D0A0); l.haar = FARBE(0x4A3020); l.frisur = 0;
  l.wangen = false;
  return l;
}
static FrauLook pechLook() { FrauLook l = faulLook(); l.kleid = FARBE(0x2A2430); l.schuerze = FARBE(0x3A3440); l.haar = FARBE(0x1E1A22); return l; }
static FrauLook witweLook() {
  FrauLook l; l.kleid = FARBE(0x6E3A5A); l.haar = FARBE(0x6E5A4A); l.frisur = 2; l.wangen = false; l.schuerze = FARBE(0x8E7A8A);
  return l;
}
static FrauLook holleLook() {
  FrauLook l; l.kleid = FARBE(0xA0B8E8); l.schuerze = WEISS; l.haar = FARBE(0xF2F2F2); l.frisur = 3; l.tuch = WEISS;
  l.rockBreite = 1.35f;
  return l;
}
static const FrauLook FLEISSIG = fleissigLook(), GOLDM = goldLook(), FAUL = faulLook(), PECH = pechLook(), WITWE = witweLook(),
                      HOLLE = holleLook();

// ---------- Dinge ----------
static void brunnen(Gfx &g, float x, float y) {
  g.kontur(1.6f, OUT);
  g.rundRechteck(x - 34, y - 34, 68, 34, 4, STEIN);
  g.konturAus();
  for (int r = 0; r < 3; r++) for (int c = 0; c < 4; c++) g.rundRechteck(x - 32 + c * 17 + (r & 1) * 8, y - 32 + r * 11, 14, 9, 2, STEIN_D, 120);
  g.ellipse(x, y - 34, 32, 6, FARBE(0x2A3040));
  g.rechteck(x - 30, y - 92, 5, 60, HOLZ_D);
  g.rechteck(x + 25, y - 92, 5, 60, HOLZ_D);
  g.kontur(1.4f, OUT);
  float d[] = {x - 40, y - 88, x, y - 110, x + 40, y - 88};
  g.polygon(d, 3, FARBE(0x9A5A3A));
  g.konturAus();
  g.linie(x - 25, y - 78, x + 25, y - 78, 3, HOLZ);
  g.linie(x, y - 78, x, y - 50, 1, FARBE(0x6E5A4A));
}

static void spinnrad(Gfx &g, float x, float y, float t, float dreh) {
  g.kontur(1.2f, OUT);
  g.kreis(x, y - 30, 18, HOLZ);
  g.konturAus();
  g.kreis(x, y - 30, 14, FARBE(0xE8D2B0));
  for (int i = 0; i < 6; i++) {
    float a = dreh + i * 1.047f;
    g.linie(x, y - 30, x + cosf(a) * 14, y - 30 + sinf(a) * 14, 1.6f, HOLZ_D);
  }
  g.linie(x - 10, y, x, y - 30, 2, HOLZ_D);
  g.linie(x + 10, y, x, y - 30, 2, HOLZ_D);
}

static void spule(Gfx &g, float x, float y, float s, float w = 0) {
  g.ellipse(x, y, 3 * s, 6 * s, FARBE(0xE8D8B0), 255, w);
  g.ellipse(x, y, 2 * s, 5 * s, FARBE(0xF6EED6), 255, w);
}

// ein kleines Gesicht (fuer Brote und Aepfel, die sprechen)
static void gesicht(Gfx &g, float x, float y, float s, float mund, bool lacht = false) {
  g.kreis(x - 3 * s, y - 1 * s, 1 * s, OUT);
  g.kreis(x + 3 * s, y - 1 * s, 1 * s, OUT);
  if (mund > 0.05f) g.ellipse(x, y + 3 * s, 1.6f * s, (0.6f + mund * 1.6f) * s, FARBE(0x8A2A2A));
  else if (lacht) g.bogen(x, y + 1 * s, 2.6f * s, 0.5f, 2.6f, 0.9f * s, OUT);
}

static void ofen(Gfx &g, float x, float y, float t, int brote, float sprechen, float raus) {
  g.kontur(1.8f, OUT);
  g.rundRechteck(x - 60, y - 80, 120, 80, 30, FARBE(0xC0705A));
  g.konturAus();
  for (int r = 0; r < 4; r++) for (int c = 0; c < 6; c++) g.rechteck(x - 58 + c * 20 + (r & 1) * 10, y - 72 + r * 16, 18, 1.4f, FARBE(0x9A4E3E));
  g.rundRechteck(x - 36, y - 56, 72, 40, 18, FARBE(0x2E2220));
  for (int i = 0; i < 3; i++) g.ellipse(x - 14 + i * 14, y - 18, 5, 3 + 1.5f * sinf(t * 8 + i), FARBE(0xFF8A2A));
  for (int i = 0; i < brote; i++) {
    float bx = x - 22 + i * 22, by = y - 34;
    if (raus > 0) { bx = lerp(bx, x + 70 + i * 22, raus); by = lerp(by, y - 6, raus); }
    g.kontur(1, OUT);
    g.ellipse(bx, by, 10, 7, FARBE(0xB8702E));
    g.konturAus();
    g.linie(bx - 5, by - 3, bx - 2, by - 6, 1.2f, FARBE(0xE0A050));
    g.linie(bx + 1, by - 3, bx + 4, by - 6, 1.2f, FARBE(0xE0A050));
    gesicht(g, bx, by + 1, 1, sprechen > 0 ? 0.3f + 0.4f * fabsf(sinf(t * 9 + i)) : 0, true);
  }
}

static void apfelbaum(Gfx &g, float x, float y, float t, float schuetteln, float gefallen) {
  float sw = sinf(t * 18) * 3 * schuetteln;
  g.kontur(1.4f, OUT);
  g.linie(x, y, x + 2, y - 70, 12, HOLZ_D);
  g.konturAus();
  g.kreis(x - 30 + sw, y - 96, 34, FARBE(0x4CA85A));
  g.kreis(x + 30 + sw, y - 90, 32, FARBE(0x3E9A4B));
  g.kreis(x + sw, y - 120, 34, FARBE(0x5DAE48));
  for (int i = 0; i < 9; i++) {
    float ax = x - 40 + (i % 5) * 20 + sw, ay = y - 120 + (i / 5) * 30 + (i & 1) * 8;
    float k = seg(gefallen, i * 0.08f, i * 0.08f + 0.4f);
    float fy = lerp(ay, y + 6 - (i % 3) * 4, hinein(k));
    float fx = lerp(ax, ax + (i - 4) * 6, k);
    g.kreis(fx, fy, 6, FARBE(0xE8473D));
    g.kreis(fx - 2, fy - 2, 1.6f, WEISS, 160);
    gesicht(g, fx, fy + 1, 0.8f, schuetteln > 0 || gefallen > 0 ? 0 : 0.3f + 0.3f * fabsf(sinf(t * 9 + i)), true);
  }
}

static void hahn(Gfx &g, float x, float y, float s, float dir, float t, float kraeht = 0) {
  auto PX = [&](float d) { return x + d * s * dir; };
  auto PY = [&](float d) { return y + d * s; };
  g.linie(PX(-2), PY(-8), PX(-3), PY(-1), 1.4f * s, FARBE(0xE0A23C));
  g.linie(PX(3), PY(-8), PX(4), PY(-1), 1.4f * s, FARBE(0xE0A23C));
  for (int i = 0; i < 4; i++)
    g.kurve(PX(-9), PY(-16), PX(-18 - i * 2), PY(-28 + i * 4), PX(-16 - i * 3), PY(-12 + i * 3), 3 * s, i & 1 ? FARBE(0x2E6E4E) : FARBE(0x1E3A5A));
  g.kontur(1.1f * s, OUT);
  g.ellipse(PX(0), PY(-14), 11 * s, 8 * s, FARBE(0xC9702E));
  g.konturAus();
  float kx = PX(8), ky = PY(-24 - kraeht * 3);
  g.kontur(1.0f * s, OUT);
  g.kreis(kx, ky, 5 * s, FARBE(0xE0A050));
  g.konturAus();
  g.kreis(kx + 0.5f * s * dir, ky - 5 * s, 3.2f * s, FARBE(0xE0302A));
  g.kreis(kx - 2 * s * dir, ky - 5.5f * s, 2.6f * s, FARBE(0xE0302A));
  float auf = kraeht * 3;
  float sn1[] = {kx + 4 * s * dir, ky - 1 * s, kx + 9 * s * dir, ky - 0.5f * s - auf * s, kx + 4 * s * dir, ky + 1 * s};
  float sn2[] = {kx + 4 * s * dir, ky + 1 * s, kx + 9 * s * dir, ky + 1.5f * s + auf * s, kx + 4 * s * dir, ky + 2.5f * s};
  g.polygon(sn1, 3, FARBE(0xF2B230));
  g.polygon(sn2, 3, FARBE(0xF2B230));
  g.kreis(kx + 1.5f * s * dir, ky - 1 * s, 1.1f * s, SCHWARZ);
}

// Frau Holle hat so grosse Zaehne
static void holle(Gfx &g, float x, float y, float s, float dir, float t, float mund, float armV, float *hx = nullptr, float *hy = nullptr) {
  FrauPose p = fposis(HOLLE, t, 2);
  p.mund = mund;
  p.armV = armV;
  frau(g, x, y, s, dir, p, hx, hy);
  // grosse Zaehne
  float mx = x + 4 * s * dir, my = y - 63 * s + (mund > 0.05f ? 0 : 1.5f * s);
  g.kontur(0.8f, OUT);
  g.rundRechteck(mx - 3.2f * s, my - 1 * s, 3 * s, 4 * s, 0.8f * s, WEISS);
  g.rundRechteck(mx + 0.4f * s, my - 1 * s, 3 * s, 4 * s, 0.8f * s, WEISS);
  g.konturAus();
}

// ---------- Kulissen ----------
static void hof(Gfx &g, float t) {
  himmel(g, 0, t, 196);
  sonne(g, 248, 30, 12, t);
  boden(g, 196);
  schornstein(g, 80, 98, t);
  hauswand(g, 10, 120, 90, 76, FARBE(0xF1DDB0), true);
  ziegeldach(g, 10, 100, 122, 40, FARBE(0xA8563A));
  holztuer(g, 44, 196, 22, 40);
  fenster(g, 18, 140, 18, 16);
  fenster(g, 76, 140, 18, 16);
  // Holzstoss an der Hauswand
  for (int i = 0; i < 5; i++) {
    g.kreis(108 + (i % 3) * 7 + (i / 3) * 3.5f, 192 - (i / 3) * 6, 3.4f, HOLZ_D);
    g.kreis(108 + (i % 3) * 7 + (i / 3) * 3.5f, 192 - (i / 3) * 6, 1.8f, FARBE(0xE0B880));
  }
  brunnen(g, 200, 214);
}

static void wiese(Gfx &g, float t, float tor = 0) {
  himmel(g, 0, t, 196);
  sonne(g, 248, 30, 12, t);
  wolke(g, fmodf(80 + t * 4, 340) - 30, 40, 0.8f);
  boden(g, 196, FARBE(0x8ADB6A));
  for (int i = 0; i < 26; i++) {
    float x = zufall(i + 900) * 284, y = 200 + zufall(i + 920) * 36;
    g.kreis(x, y, 2.6f, i % 3 == 0 ? FARBE(0xF2A0C8) : i % 3 == 1 ? GOLD : WEISS);
  }
  if (tor > 0) {  // das grosse Tor
    g.kontur(1.8f, OUT);
    g.rechteck(110, 80, 18, 120, FARBE(0xE8E0D0));
    g.rechteck(196, 80, 18, 120, FARBE(0xE8E0D0));
    float b[] = {104, 82, 162, 50, 220, 82};
    g.polygon(b, 3, FARBE(0xC0503A));
    g.konturAus();
  }
}

// Frau Holles Haus; durchs Fenster sieht man tief unten die Welt
static void holleStube(Gfx &g, float t, float schnee) {
  g.verlauf(0, 200, FARBE(0xE6D6B6), FARBE(0xD6BE96));
  g.rechteck(0, 200, 284, 40, FARBE(0x9E7656));
  g.kontur(1.8f, OUT);
  g.rundRechteck(160, 30, 110, 100, 8, HOLZ);
  g.konturAus();
  g.ausschnitt(166, 36, 98, 88);
  g.verlauf(36, 124, FARBE(0x9FD8FF), FARBE(0xD8F0FF));
  for (int i = 0; i < 3; i++) wolke(g, 180 + i * 30, 60 + (i & 1) * 12, 0.5f);
  // die Welt tief unten, wird weiss
  g.ellipse(215, 128, 70, 20, mischeFarbe(GRAS, WEISS, schnee));
  for (int i = 0; i < 4; i++) {
    float hx = 180 + i * 22;
    g.rechteck(hx, 108, 12, 10, mischeFarbe(FARBE(0xF4E4C4), WEISS, schnee * 0.3f));
    float d[] = {hx - 2, 109, hx + 6, 102, hx + 14, 109};
    g.polygon(d, 3, mischeFarbe(FARBE(0xC0503A), WEISS, schnee));
  }
  if (schnee > 0)
    for (int i = 0; i < 26; i++) {
      float x = 166 + zufall(i + 40) * 98, y = fmodf(36 + zufall(i + 60) * 88 + t * 18, 88) + 36;
      g.kreis(x, y, 1.4f, WEISS, (uint8_t)(230 * klemme(schnee * 3)));
    }
  g.ausschnittAus();
  g.rechteck(156, 128, 118, 6, HOLZ_D);
  // Bett mit dicker Federdecke
  g.kontur(1.6f, OUT);
  g.rundRechteck(14, 150, 120, 50, 6, HOLZ);
  g.konturAus();
  g.rundRechteck(20, 140, 40, 18, 8, WEISS);
  g.rundRechteck(52, 136, 78, 30, 12, FARBE(0xF2EEFF));
}

// =====================================================================
// Szenen
// =====================================================================
// 1) Die fleissige Tochter spinnt am Brunnen, die Spule faellt hinein
static void szeneBrunnen(Gfx &g, float u) {
  hof(g, u);
  FrauPose w = fposis(WITWE, u);
  w.boese = 1;
  w.mund = u > 17 && u < 23 ? 0.6f : 0;
  w.armV = u > 17 && u < 23 ? 1.8f : 0.6f;
  frau(g, u > 14 ? 120 : 330, 214, 1, 1, w);
  FrauPose f = fposis(FAUL, u, 1);
  f.zu = true;
  if (u < 14) frau(g, 30, 214, 0.9f, 1, f);  // die Faule sitzt am Haus
  FrauPose m = fposis(FLEISSIG, u, 2);
  float k = seg(u, 26, 29);
  m.armV = u < 10 ? 1.2f + sinf(u * 6) * 0.2f : u < 26 ? 1.0f : 2.6f;
  m.weint = u > 12 && u < 26;
  m.mund = u > 23.4f && u < 25.6f ? 0.4f : 0;
  if (k < 1) frau(g, lerp(170, 200, k), 214 - huepf(k) * 40 + k * k * 60, 0.9f, -1, m);
  if (u < 12) {
    spinnrad(g, 140, 214, u, u * 4);
  }
  if (u > 9 && u < 12) spule(g, lerp(176, 200, seg(u, 9, 12)), lerp(160, 186, hinein(seg(u, 9, 12))), 1, u * 6);
  g.kameraAus();
  vers(g, {"Eine Witwe hatte zwei Töchter,", "die eine schön und fleißig, die andere", "hässlich und faul."}, aufzu(u, 0.4f, 8.6f));
  vers(g, {"Da sprang ihr die Spule", "in den Brunnen hinab."}, aufzu(u, 9.4f, 16));
  blase(g, 140, 40, {"Hast du die Spule hinunterfallen", "lassen, so hol sie auch", "wieder herauf!"}, aufzu(u, 17, 23), 128, 120);
  vers(g, {"Da sprang sie in den Brunnen,", "um die Spule zu holen."}, aufzu(u, 25.8f, 31.6f));
  blende(g, u, 32, 200, 160);
}

// 2) Der Fall durch den Brunnen - und dann die schoene Wiese
static void szeneWiese(Gfx &g, float u) {
  if (u < 6) {
    // ein Strudel aus Blau, Sternen und Blaettern
    g.fuellen(FARBE(0x3A4E8E));
    for (int r = 0; r < 6; r++) g.kreis(142, 120, 140 - r * 22, r & 1 ? FARBE(0x5A7AC8) : FARBE(0x4A62A8));
    for (int i = 0; i < 16; i++) {
      float a = u * 3 + i * 0.393f, rr = 20 + fmodf(i * 17 + u * 40, 110);
      g.stern(142 + cosf(a) * rr, 120 + sinf(a) * rr, 3, 1.2f, 4, a, i & 1 ? GOLD : WEISS);
    }
    FrauPose m = fposis(FLEISSIG, u, 2);
    m.zu = true;
    m.armV = 2.6f; m.armH = 2.6f;
    m.drehen = sinf(u * 8) * 5;
    frau(g, 142 + sinf(u * 4) * 20, 160, lerp(0.4f, 0.9f, u / 6), 1, m);
    return;
  }
  float v = u - 6;
  wiese(g, u);
  FrauPose m = fposis(FLEISSIG, u, 2);
  m.zu = v < 3;
  m.mund = v > 3 ? 0.4f : 0;
  m.armV = v > 3 ? 1.8f : 0.4f;
  frau(g, 120, 214, 0.9f, 1, m);
  if (v > 3) glitzern(g, 120, 150, 30, u, 6, (uint8_t)(255 * (1 - seg(v, 7, 9))));
  g.kameraAus();
  if (v < 0.8f) g.rechteck(0, 0, 284, 240, WEISS, (uint8_t)(255 * (1 - v / 0.8f)));
  vers(g, {"Als sie erwachte, war sie auf einer", "schönen Wiese, da schien die Sonne."}, aufzu(v, 1, 9.6f));
}

// 3) Der Backofen voller Brot
static void szeneBrot(Gfx &g, float u) {
  wiese(g, u);
  float raus = seg(u, 12, 16);
  ofen(g, 150, 214, u, 3, u > 3 && u < 10 ? 1 : 0, raus);
  FrauPose m = fposis(FLEISSIG, u, 2);
  m.armV = u > 11 && u < 17 ? 1.8f + fabsf(sinf(u * 4)) * 0.3f : 0.8f;
  m.mund = 0.2f;
  float mx = lerp(-30, 60, sanft(seg(u, 0, 3)));
  m.lauf = u < 3 ? fmodf(u * 1.5f, 1) : -1;
  float hx, hy;
  frau(g, mx, 214, 0.9f, 1, m, &hx, &hy);
  if (u > 11 && u < 17) g.linie(hx, hy, hx + 40, hy + 10, 3, HOLZ);  // Brotschieber
  g.kameraAus();
  blase(g, 150, 70, {"Ach, zieh mich raus, zieh mich raus,", "sonst verbrenn ich: ich bin", "schon längst ausgebacken!"}, aufzu(u, 3, 10.6f), 150, 170);
  vers(g, {"Da trat sie hinzu und holte", "mit dem Brotschieber alles", "nacheinander heraus."}, aufzu(u, 11, 18));
  blende(g, u, 26, 140, 150);
}

// 4) Der Apfelbaum
static void szeneApfel(Gfx &g, float u) {
  wiese(g, u);
  float sch = u > 9 && u < 14 ? 1 : 0;
  apfelbaum(g, 170, 214, u, sch, seg(u, 10, 15));
  FrauPose m = fposis(FLEISSIG, u, 2);
  m.armV = sch > 0 ? 2.6f + sinf(u * 18) * 0.2f : 0.8f;
  frau(g, lerp(-30, 120, sanft(seg(u, 0, 3))), 214, 0.9f, 1, m);
  g.kameraAus();
  blase(g, 130, 40, {"Ach, schüttel mich, schüttel mich,", "wir Äpfel sind alle", "miteinander reif!"}, aufzu(u, 3, 9.4f), 170, 110);
  vers(g, {"Da schüttelte sie den Baum, dass die", "Äpfel fielen, als regneten sie."}, aufzu(u, 14, 21));
  blende(g, u, 22, 140, 150);
}

// 5) Frau Holle: "Ich bin die Frau Holle" - die Betten schuetteln, dann schneit es in der Welt
static void szeneHolle(Gfx &g, float u) {
  if (u < 18) {
    wiese(g, u);
    g.kontur(1.8f, OUT);
    g.rechteck(150, 120, 110, 80, FARBE(0xF4ECDC));
    float d[] = {140, 122, 205, 80, 270, 122};
    g.polygon(d, 3, FARBE(0xC0503A));
    g.konturAus();
    g.rundRechteck(160, 140, 24, 20, 2, FARBE(0x8FD3FF));
    g.rundRechteck(222, 140, 24, 20, 2, FARBE(0x8FD3FF));
    holle(g, 200, 214, 1, -1, u, u > 6 && u < 17 ? 0.5f : 0, 1.5f);
    FrauPose m = fposis(FLEISSIG, u, 2);
    m.armV = u < 6 ? 2.2f : 0.8f;
    m.mund = u < 6 ? 0.6f : 0.2f;
    frau(g, u < 3 ? lerp(60, 110, u / 3) : 110, 214, 0.9f, 1, m);
    g.kameraAus();
    vers(g, {"Eine alte Frau guckte heraus.", "Weil sie so große Zähne hatte,", "ward dem Mädchen angst."}, aufzu(u, 0.4f, 6));
    blase(g, 160, 40, {"Was fürchtest du dich, liebes Kind?", "Bleib bei mir! Wenn du alle", "Arbeit im Hause ordentlich tun willst,"}, aufzu(u, 6.2f, 11.6f), 200, 130);
    blase(g, 160, 40, {"schüttle mein Bett fleißig auf,", "dass die Federn fliegen,", "dann schneit es in der Welt."}, aufzu(u, 11.8f, 17.6f), 200, 130);
    return;
  }
  float v = u - 18;
  float schnee = seg(v, 4, 20);
  holleStube(g, u, schnee);
  FrauPose m = fposis(FLEISSIG, u, 2);
  float sch = v > 2 && v < 22 ? 1 : 0;
  m.armV = sch > 0 ? 2.2f + sinf(u * 10) * 0.5f : 1.2f;
  m.armH = sch > 0 ? 2.2f - sinf(u * 10) * 0.5f : 0.4f;
  m.mund = 0.4f;
  float hx, hy;
  frau(g, 190, 214, 0.95f, -1, m, &hx, &hy);
  if (sch > 0) {
    // das Federbett, aus dem Fenster geschuettelt
    float w = sinf(u * 10) * 6;
    g.kontur(1.2f, OUT);
    g.rundRechteck(hx - 30, hy - 26 + w, 44, 22, 9, FARBE(0xF2EEFF));
    g.konturAus();
    for (int i = 0; i < 14; i++) {
      float k = fmodf(u * 0.6f + i / 14.0f, 1);
      float fx = hx - 10 + sinf(k * 9 + i) * 16 + k * 40, fy = hy - 30 - k * 40 + k * k * 100;
      g.ellipse(fx, fy, 2.4f, 1.4f, WEISS, (uint8_t)(255 * (1 - k)), k * 6 + i);
    }
  }
  holle(g, 60, 214, 0.95f, 1, u, v > 22 ? 0.4f : 0, 1.4f);
  g.kameraAus();
  vers(g, {"Sie schüttelte das Bett gewaltig,", "dass die Federn wie", "Schneeflocken umherflogen."}, aufzu(v, 3, 12));
  vers(g, {"Und unten in der Welt", "schneite es."}, aufzu(v, 12.4f, 19.6f));
  blase(g, 100, 60, {"Ich bin die Frau Holle."}, aufzu(v, 21, 25.6f), 64, 140);
  blende(g, u, 44, 150, 150);
}

// 6) Heimweh; der Goldregen unter dem Tor; der Hahn auf dem Brunnen
static void szeneGold(Gfx &g, float u) {
  if (u < 20) {
    wiese(g, u, 1);
    float gold = seg(u, 6, 9);
    holle(g, 70, 214, 0.95f, 1, u, u > 1 && u < 6 ? 0.5f : 0, 1.6f);
    FrauPose m = fposis(gold > 0.5f ? GOLDM : FLEISSIG, u, 2);
    m.armV = 2.0f;
    m.mund = 0.4f;
    m.zu = gold > 0 && gold < 1;
    float hx, hy;
    frau(g, 162, 214, 0.95f, 1, m, &hx, &hy);
    if (u > 6 && u < 13)  // goldener Regen
      for (int i = 0; i < 30; i++) {
        float x = 120 + zufall(i + 100) * 84, y = fmodf(84 + zufall(i + 120) * 120 + u * 90, 130) + 82;
        g.kreis(x, y, 1.8f + zufall(i) * 1.2f, GOLD);
      }
    if (u > 10) spule(g, hx + 4, hy, 1.2f);
    if (gold > 0.5f) glitzern(g, 162, 150, 30, u, 8);
    g.kameraAus();
    blase(g, 110, 40, {"Das sollst du haben,", "weil du so fleißig gewesen bist."}, aufzu(u, 1, 6), 80, 130);
    vers(g, {"Da fiel ein gewaltiger Goldregen,", "und alles Gold blieb an ihr hängen."}, aufzu(u, 7, 13));
    vers(g, {"Auch die Spule gab ihr Frau Holle", "wieder, und das Tor ward", "verschlossen."}, aufzu(u, 13.2f, 19.6f));
    if (u > 19) g.rechteck(0, 0, 284, 240, WEISS, (uint8_t)(255 * seg(u, 19, 20)));
    return;
  }
  float v = u - 20;
  hof(g, u);
  hahn(g, 200, 180, 1.2f, -1, u, v > 1 && v < 7 ? 0.6f + 0.4f * fabsf(sinf(u * 8)) : 0);
  FrauPose m = fposis(GOLDM, u, 2);
  m.armV = 1.6f;
  m.mund = 0.4f;
  frau(g, 120, 214, 0.95f, 1, m);
  glitzern(g, 120, 150, 30, u, 6);
  FrauPose w = fposis(WITWE, u);
  w.mund = 0.6f;
  w.armV = 2.4f;
  frau(g, 40, 214, 0.95f, 1, w);
  g.kameraAus();
  if (v < 1) g.rechteck(0, 0, 284, 240, WEISS, (uint8_t)(255 * (1 - v)));
  blase(g, 170, 50, {"Kikeriki, unsere goldene", "Jungfrau ist wieder hie!"}, aufzu(v, 1, 8), 206, 150);
  blende(g, u, 34, 140, 150);
}

// 7) Die Faule will es auch so gut haben - aber sie mag nicht arbeiten
static void szeneFaul(Gfx &g, float u) {
  if (u < 10) {
    hof(g, u);
    FrauPose f = fposis(FAUL, u, 1);
    float k = seg(u, 5, 8);
    f.armV = 2.4f;
    f.mund = 0.3f;
    if (k < 1) frau(g, lerp(150, 200, k), 214 - huepf(k) * 40 + k * k * 60, 0.9f, 1, f);
    spule(g, 150, 180, 1);
    g.kameraAus();
    vers(g, {"Die Mutter wollte der faulen Tochter", "gern dasselbe Glück verschaffen."}, aufzu(u, 0.4f, 5));
    vers(g, {"Sie warf die Spule in den Brunnen", "und sprang selber hinein."}, aufzu(u, 5.2f, 9.8f));
    return;
  }
  float v = u - 10;
  wiese(g, u);
  FrauPose f = fposis(FAUL, u, 1);
  f.boese = 0.4f;
  if (v < 16) {
    // am Ofen vorbei
    ofen(g, 170, 214, u, 3, v > 2 && v < 9 ? 1 : 0, 0);
    f.lauf = v > 10 ? fmodf(u * 1.4f, 1) : -1;
    f.mund = v > 9.4f && v < 14 ? 0.6f : 0;
    f.armV = 0.5f;
    frau(g, v < 10 ? 70 : lerp(70, 330, seg(v, 13, 16)), 214, 0.9f, 1, f);
    g.kameraAus();
    blase(g, 150, 70, {"Ach, zieh mich raus,", "sonst verbrenn ich!"}, aufzu(v, 2, 9), 170, 170);
    blase(g, 80, 70, {"Da hätt ich Lust,", "mich schmutzig zu machen!"}, aufzu(v, 9.4f, 14), 70, 130);
  } else if (v < 30) {
    float w = v - 16;
    apfelbaum(g, 180, 214, u, 0, 0);
    f.mund = w > 6 && w < 11 ? 0.6f : 0;
    f.lauf = w > 11 ? fmodf(u * 1.4f, 1) : -1;
    frau(g, w < 11 ? 80 : lerp(80, 330, seg(w, 11, 14)), 214, 0.9f, 1, f);
    g.kameraAus();
    blase(g, 140, 40, {"Ach, schüttel mich,", "wir Äpfel sind alle reif!"}, aufzu(w, 1, 6), 180, 110);
    blase(g, 80, 70, {"Du kommst mir recht,", "es könnte mir einer", "auf den Kopf fallen!"}, aufzu(w, 6.2f, 11), 80, 130);
  } else {
    // bei Frau Holle: am ersten Tag fleissig, dann faulenzt sie im Bett, das Bett wird nicht geschuettelt
    float w = v - 30;
    holleStube(g, u, 0);
    g.kreis(84, 146, 9, FAUL.haut);
    g.ellipse(76, 140, 10, 6, FAUL.haar);
    zzz(g, 92, 126, u);
    g.rundRechteck(52, 140, 78, 28, 12, FARBE(0xF2EEFF));
    holle(g, 200, 214, 0.95f, -1, u, w > 9 && w < 15 ? 0.4f : 0, w > 9 ? 1.8f : 1.2f);
    g.kameraAus();
    vers(g, {"Am ersten Tag tat sie sich Gewalt an,", "am zweiten fing sie an zu faulenzen."}, aufzu(w, 0.6f, 8.6f));
    vers(g, {"Sie schüttelte der Frau Holle das Bett", "nicht, dass die Federn aufflogen."}, aufzu(w, 8.8f, 15.6f));
    if (w > 22) g.rechteck(0, 0, 284, 240, 0, (uint8_t)(255 * seg(w, 22, 24)));
  }
  if (v < 0.8f) { g.kameraAus(); g.rechteck(0, 0, 284, 240, WEISS, (uint8_t)(255 * (1 - v / 0.8f))); }
}

// 8) Das Pech - und der Hahn
static void szenePech(Gfx &g, float u) {
  if (u < 16) {
    wiese(g, u, 1);
    float pech = seg(u, 5, 8);
    holle(g, 70, 214, 0.95f, 1, u, u > 1 && u < 5 ? 0.5f : 0, 1.6f);
    FrauPose f = fposis(pech > 0.5f ? PECH : FAUL, u, 1);
    f.armV = 2.4f;
    f.mund = pech > 0.5f ? 0.7f : 0.4f;
    f.zu = pech > 0 && pech < 1;
    frau(g, 162, 214, 0.95f, 1, f);
    if (u > 5 && u < 10)
      for (int i = 0; i < 16; i++) {
        float x = 130 + zufall(i + 100) * 66, y = fmodf(84 + zufall(i + 120) * 120 + u * 60, 130) + 82;
        g.ellipse(x, y, 2.6f, 4, FARBE(0x1E1A22));
      }
    if (pech > 0.5f) for (int i = 0; i < 6; i++) g.ellipse(150 + (i % 3) * 12, 140 + (i / 3) * 30, 6, 9, FARBE(0x1E1A22), 230);
    g.kameraAus();
    blase(g, 110, 40, {"Das ist zur Belohnung", "deiner Dienste."}, aufzu(u, 1, 5), 80, 130);
    vers(g, {"Statt des Goldes wurde ein großer", "Kessel voll Pech ausgeschüttet."}, aufzu(u, 5.4f, 12));
    if (u > 15) g.rechteck(0, 0, 284, 240, 0, (uint8_t)(255 * seg(u, 15, 16)));
    return;
  }
  float v = u - 16;
  hof(g, u);
  hahn(g, 200, 180, 1.2f, -1, u, v > 1 && v < 7 ? 0.6f + 0.4f * fabsf(sinf(u * 8)) : 0);
  FrauPose f = fposis(PECH, u, 1);
  f.weint = true;
  f.armV = 1.4f;
  frau(g, 120, 214, 0.95f, 1, f);
  g.kameraAus();
  if (v < 1) g.rechteck(0, 0, 284, 240, 0, (uint8_t)(255 * (1 - v)));
  blase(g, 170, 50, {"Kikeriki, unsere schmutzige", "Jungfrau ist wieder hie!"}, aufzu(v, 1, 7), 206, 150);
  vers(g, {"Das Pech aber blieb fest an ihr hängen", "und wollte nicht abgehen."}, aufzu(v, 7.4f, 14));
  endeBild(g, v, 16);
}

static void bild(Gfx &g, float t) {
  if (t < P2) szeneBrunnen(g, t - P1);
  else if (t < P3) szeneWiese(g, t - P2);
  else if (t < P4) szeneBrot(g, t - P3);
  else if (t < P5) szeneApfel(g, t - P4);
  else if (t < P6) szeneHolle(g, t - P5);
  else if (t < P7) szeneGold(g, t - P6);
  else if (t < P8) szeneFaul(g, t - P7);
  else szenePech(g, t - P8);
}

// =====================================================================
// Musik
// =====================================================================
// Frau Holles Lied (A-Dur, schwebend wie Schneeflocken)
static const Ton3 MEL_HOLLE[] = {
  {76, 1, 45}, {81, 1, 0}, {80, .5, 0}, {78, .5, 0}, {76, 1, 0}, {73, 1, 42}, {74, .5, 0}, {76, .5, 0}, {78, 2, 0},
  {76, 1, 45}, {78, .5, 0}, {80, .5, 0}, {81, 1, 0}, {83, 1, 0}, {85, 1, 49}, {83, .5, 0}, {80, .5, 0}, {81, 2, 45}};

static void kikeriki(Synth &s, float x) {
  const float f[5] = {560, 700, 700, 840, 620};
  for (int i = 0; i < 5; i++) s.ton(x + i * 0.18f, f[i], 0.06f, 0.01f, i == 4 ? 0.5f : 0.16f, DREIECK, GERAEUSCH, f[i] * 1.1f);
}

static void ton(Synth &s, float t0, float t1) {
  float a = P1;
  spieleMelodie(s, M(MEL_WIEGE), a + 0.5f, a + 9, 0.45f, t0, t1, 0.7f);
  for (float x = ceilf(t0 / 0.5f) * 0.5f; x < t1; x += 0.5f)
    if (x > a && x < a + 9) s.rauschen(x, 0.3f, BANDPASS, 700, 0.03f);  // Spinnrad surrt
  if (bei(a + 11, t0, t1)) { s.note(a + 11, 84, 0.5f); s.ton(a + 12, 300, 0.2f, 0.002f, 0.3f, SINUS, GERAEUSCH, 150); }  // plumps
  spieleMelodie(s, M(MEL_SPUK), a + 17, a + 25, 0.36f, t0, t1, 0.7f);
  if (bei(a + 27, t0, t1)) for (int k = 0; k < 10; k++) s.note(a + 27 + k * 0.1f, 84 - k * 3, 0.6f);
  a = P2;
  if (bei(a + 0.2f, t0, t1)) for (int k = 0; k < 24; k++) s.glocke(a + 0.2f + k * 0.22f, 72 + (k * 5) % 14, 0.35f);  // Strudel
  spieleMelodie(s, M(MEL_HOLLE), a + 6.4f, a + 15.5f, 0.38f, t0, t1, 0.8f);
  a = P3;
  spieleMelodie(s, M(MEL_HOLLE), a + 0.4f, a + 3, 0.38f, t0, t1, 0.7f);
  for (int i = 0; i < 6; i++) if (bei(a + 3.4f + i * 1.1f, t0, t1)) for (int k = 0; k < 3; k++) s.note(a + 3.4f + i * 1.1f + k * 0.1f, 79 + k * 2 + (i & 1) * 3, 0.4f);  // die Brote rufen
  spieleMelodie(s, M(MEL_WANDER), a + 11, a + 25.5f, 0.28f, t0, t1, 0.75f);
  a = P4;
  for (int i = 0; i < 5; i++) if (bei(a + 3.2f + i * 1.2f, t0, t1)) for (int k = 0; k < 3; k++) s.note(a + 3.2f + i * 1.2f + k * 0.1f, 84 + k * 2 - (i & 1) * 2, 0.4f);
  for (int i = 0; i < 9; i++) if (bei(a + 10.4f + i * 0.5f, t0, t1)) s.ton(a + 10.4f + i * 0.5f, 180, 0.15f, 0.002f, 0.08f, SINUS, GERAEUSCH, 110);  // Aepfel fallen
  spieleMelodie(s, M(MEL_HOLLE), a + 14.5f, a + 21.5f, 0.36f, t0, t1, 0.75f);
  a = P5;
  spieleMelodie(s, M(MEL_SPUK), a + 0.4f, a + 6, 0.4f, t0, t1, 0.6f);
  spieleMelodie(s, M(MEL_HOLLE), a + 6.2f, a + 43.5f, 0.42f, t0, t1, 0.85f);
  for (float x = ceilf(t0 / 0.3f) * 0.3f; x < t1; x += 0.3f)
    if (x > a + 20 && x < a + 40) s.glocke(x, 91 + ((int)(x * 7) % 4) * 2, 0.08f);  // Schneeflocken
  a = P6;
  spieleMelodie(s, M(MEL_WIEGE), a + 0.4f, a + 6, 0.45f, t0, t1, 0.6f);
  for (float x = ceilf(t0 / 0.1f) * 0.1f; x < t1; x += 0.1f)
    if (x > a + 6 && x < a + 13) s.glocke(x, 88 + ((int)(x * 13) % 6) * 2, 0.15f);  // Goldregen
  spieleMelodie(s, M(MEL_F), a + 13.4f, a + 19.5f, 0.28f, t0, t1, 0.8f);
  if (bei(a + 21, t0, t1)) kikeriki(s, a + 21);
  if (bei(a + 24, t0, t1)) kikeriki(s, a + 24);
  spieleMelodie(s, M(MEL_HOLLE), a + 27, a + 33.5f, 0.36f, t0, t1, 0.8f);
  a = P7;
  spieleMelodie(s, M(MEL_SPUK), a + 0.4f, a + 9, 0.36f, t0, t1, 0.65f);
  if (bei(a + 8, t0, t1)) for (int k = 0; k < 10; k++) s.note(a + 8 + k * 0.1f, 84 - k * 3, 0.6f);
  spieleMelodie(s, M(MEL_WANDER), a + 10.5f, a + 40, 0.36f, t0, t1, 0.6f);
  spieleMelodie(s, M(MEL_WIEGE), a + 40, a + 56, 0.6f, t0, t1, 0.5f);
  a = P8;
  for (float x = ceilf(t0 / 0.14f) * 0.14f; x < t1; x += 0.14f)
    if (x > a + 5 && x < a + 10) s.ton(x, 120 + ((int)(x * 9) % 3) * 20, 0.05f, 0.002f, 0.08f, SINUS, GERAEUSCH, 80);  // Pech platscht
  spieleMelodie(s, M(MEL_SPUK), a + 10, a + 15.5f, 0.36f, t0, t1, 0.6f);
  if (bei(a + 17, t0, t1)) kikeriki(s, a + 17);
  spieleMelodie(s, M(MEL_HOLLE), a + 23.4f, a + 32, 0.36f, t0, t1, 0.75f);
  endeKlang(s, a + 32.2f, t0, t1);
}

// =====================================================================
// Startbild, Vorschau, Erkennungszeichen
// =====================================================================
static void titelSzene(Gfx &g, float t) {
  holleStube(g, t, 1);
  FrauPose m = fposis(FLEISSIG, t, 2);
  m.armV = 2.2f + sinf(t * 6) * 0.4f;
  m.armH = 2.2f - sinf(t * 6) * 0.4f;
  float hx, hy;
  frau(g, 190, 214, 0.95f, -1, m, &hx, &hy);
  g.kontur(1.2f, OUT);
  g.rundRechteck(hx - 30, hy - 26 + sinf(t * 6) * 4, 44, 22, 9, FARBE(0xF2EEFF));
  g.konturAus();
  for (int i = 0; i < 10; i++) {
    float k = fmodf(t * 0.4f + i / 10.0f, 1);
    g.ellipse(hx - 10 + sinf(k * 9 + i) * 16 + k * 40, hy - 30 - k * 40 + k * k * 100, 2.4f, 1.4f, WEISS, (uint8_t)(255 * (1 - k)), k * 6 + i);
  }
  holle(g, 60, 214, 0.95f, 1, t, 0, 1.4f);
}
static void titelbild(Gfx &g, float t) { titelSzene(g, t); titelRahmen(g, t, "Frau Holle", 150); }
static void vorschau(Gfx &g, float t) { titelSzene(g, t); }
static void symbol(Gfx &g, float x, float y, float t) {
  // das Federbett, aus dem es schneit
  g.kontur(1.4f, OUT);
  g.rundRechteck(x - 34, y - 66 + sinf(t * 5) * 4, 68, 34, 14, FARBE(0xF2EEFF));
  g.konturAus();
  for (int i = 0; i < 4; i++) g.kreis(x - 20 + i * 13, y - 50 + sinf(t * 5) * 4, 2, FARBE(0xC9C0E6));
  for (int i = 0; i < 14; i++) {
    float k = fmodf(t * 0.35f + i / 14.0f, 1);
    float fx = x - 44 + zufall(i + 30) * 88 + sinf(k * 7 + i) * 6, fy = y - 40 + k * 48;
    g.stern(fx, fy, 3, 1.2f, 6, k * 3, WEISS, (uint8_t)(255 * (1 - k * 0.6f)));
  }
  g.rechteck(x - 48, y - 2, 96, 4, WEISS);
}

const Maerchen FRAUHOLLE = {"Frau Holle", PDAUER, FARBE(0x8EA6E8), bild, ton, titelbild, vorschau, symbol, LIED(MEL_HOLLE, 0.4f),
                            nullptr};

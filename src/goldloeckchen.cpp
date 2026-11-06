// Goldloeckchen und die drei Baeren - ein Spieluhr-Film fuer die Maerchenbox, nach dem englischen Maerchen.
// Zu heiss, zu kalt, genau richtig: der Brei, die Stuehle, die Betten -
// "Wer hat von meinem Brei gegessen?" - "... und da liegt sie ja noch!"
#include "erzaehlen.h"

// ---------- Ablauf (Sekunden) ----------
static const float P1 = 0, P2 = 22, P3 = 50, P4 = 72, P5 = 94, P6 = 140, PDAUER = 168;

static FrauLook goldLook() {
  FrauLook l; l.kleid = FARBE(0xE86F8A); l.schuerze = WEISS; l.haar = FARBE(0xF6D04A); l.frisur = 0; l.blume = FARBE(0x5BB8E8);
  return l;
}
static const FrauLook GOLD_L = goldLook();

// ---------- die drei Baeren ----------
// art 0 Papa (gross, Hut), 1 Mama (mittel, Schuerze), 2 Baby (klein, Latz)
static void baer(Gfx &g, int art, float x, float y, float dir, float t, float lauf = -1, float mund = 0, float armV = 0.6f,
                 bool staunt = false) {
  float s = art == 0 ? 1.15f : art == 1 ? 0.95f : 0.62f;
  const Farbe fell = art == 0 ? FARBE(0x7A4E2A) : art == 1 ? FARBE(0x9A6A3E) : FARBE(0xB8844E);
  const Farbe hell = FARBE(0xE0C09A);
  auto PX = [&](float d) { return x + d * s * dir; };
  auto PY = [&](float d) { return y + d * s; };
  float hop = lauf >= 0 ? fabsf(sinf(lauf * 6.28f)) * 2 * s : 0;
  g.kontur(1.3f * s, OUT);
  for (int k = 0; k < 2; k++) {
    float sw = lauf >= 0 ? sinf(lauf * 6.28f + k * 3.14f) * 5 : 0;
    g.linie(PX(-7 + k * 14), PY(-22) - hop, PX(-7 + k * 14 + sw), PY(-4), 9 * s, fell);
    g.ellipse(PX(-7 + k * 14 + sw + 2), PY(-3), 7 * s, 4 * s, fell);
  }
  g.ellipse(PX(0), PY(-40) - hop, 20 * s, 24 * s, fell);
  g.konturAus();
  g.ellipse(PX(2), PY(-36) - hop, 12 * s, 15 * s, hell);  // Bauch
  if (art == 1) {  // Schuerze
    float sch[] = {PX(-12), PY(-46) - hop, PX(14), PY(-46) - hop, PX(16), PY(-18) - hop, PX(-14), PY(-18) - hop};
    g.polygon(sch, 4, FARBE(0xF2A0C8));
  }
  if (art == 2) g.ellipse(PX(3), PY(-50) - hop, 9 * s, 6 * s, FARBE(0x5BB8E8));  // Latz
  if (art == 0) g.rundRechteck(PX(-14) < PX(14) ? PX(-14) : PX(14), PY(-52) - hop, 28 * s, 6 * s, 2 * s, FARBE(0x3E6EA8));  // Weste
  // Arm
  float ax = PX(14), ay = PY(-50) - hop;
  g.kontur(1.2f * s, OUT);
  g.linie(ax, ay, ax + sinf(armV) * 18 * s * dir, ay + cosf(armV) * 18 * s, 8 * s, fell);
  // Kopf
  float kx = PX(4), ky = PY(-72) - hop;
  g.kreis(kx - 11 * s * dir, ky - 12 * s, 6 * s, fell);
  g.kreis(kx + 10 * s * dir, ky - 12 * s, 6 * s, fell);
  g.kreis(kx, ky, 15 * s, fell);
  g.ellipse(kx + 8 * s * dir, ky + 4 * s, 9 * s, 7 * s, hell);
  g.konturAus();
  g.kreis(kx - 11 * s * dir, ky - 12 * s, 3 * s, hell);
  g.kreis(kx + 10 * s * dir, ky - 12 * s, 3 * s, hell);
  g.ellipse(kx + 14 * s * dir, ky + 1 * s, 3.4f * s, 2.6f * s, SCHWARZ);
  for (int k = 0; k < 2; k++) {
    float ex = kx + (k ? 6 : -3) * s * dir, ey = ky - 4 * s;
    if (staunt) { g.kreis(ex, ey, 2.6f * s, WEISS); g.kreis(ex, ey, 1.3f * s, SCHWARZ); }
    else if (fmodf(t + art * 1.3f, 3.9f) < 0.13f) g.bogen(ex, ey + 0.5f * s, 1.8f * s, 3.5f, 5.9f, 1.1f * s, OUT);
    else g.kreis(ex, ey, 1.7f * s, SCHWARZ);
  }
  if (mund > 0.05f) g.ellipse(kx + 10 * s * dir, ky + 8 * s, 2.6f * s, (1 + mund * 3) * s, FARBE(0x8A2A2A));
  else g.bogen(kx + 10 * s * dir, ky + 6 * s, 3 * s, 0.4f, 2.7f, 1.1f * s, OUT);
  if (art == 0) {  // Hut
    g.kontur(1, OUT);
    g.rundRechteck(kx - 12 * s, ky - 26 * s, 24 * s, 12 * s, 3 * s, FARBE(0x3A3436));
    g.konturAus();
    g.rechteck(kx - 16 * s, ky - 15 * s, 32 * s, 3 * s, FARBE(0x3A3436));
  }
  if (art == 1) g.kreis(kx - 4 * s * dir, ky - 14 * s, 4 * s, FARBE(0xE8473D));  // Schleife
}

// ---------- Dinge ----------
static void schuessel(Gfx &g, float x, float y, float s, float voll, bool dampf, float t, bool leer = false) {
  g.kontur(1.2f, OUT);
  float q[] = {x - 14 * s, y - 10 * s, x + 14 * s, y - 10 * s, x + 9 * s, y, x - 9 * s, y};
  g.polygon(q, 4, FARBE(0x5BB8E8));
  g.konturAus();
  if (!leer) g.ellipse(x, y - 10 * s, 12 * s, 3 * s * voll + 0.5f, FARBE(0xF2E2B0));
  g.linie(x + 12 * s, y - 12 * s, x + 18 * s, y - 22 * s, 1.6f, FARBE(0xC0C8D4));  // Loeffel
  if (dampf)
    for (int i = 0; i < 2; i++) {
      float k = fmodf(t * 0.7f + i * 0.5f, 1);
      g.kurve(x - 4 + i * 8, y - 14 * s - k * 14, x - 7 + i * 8, y - 18 * s - k * 14, x - 2 + i * 8, y - 22 * s - k * 14, 1.2f, WEISS, (uint8_t)(180 * (1 - k)));
    }
}

static void stuhl(Gfx &g, float x, float y, float s, Farbe c, float kaputt = 0) {
  float k = kaputt;
  g.kontur(1.2f, OUT);
  g.rundRechteck(x - 12 * s, y - 50 * s, 24 * s, 30 * s, 4 * s, c);
  g.rundRechteck(x - 14 * s, y - 24 * s + k * 14 * s, 28 * s, 7 * s, 3 * s, c);
  g.konturAus();
  g.linie(x - 11 * s, y - 18 * s + k * 14 * s, x - 11 * s - k * 8 * s, y, 3 * s, HOLZ_D);
  g.linie(x + 11 * s, y - 18 * s + k * 14 * s, x + 11 * s + k * 8 * s, y, 3 * s, HOLZ_D);
}

static void bett(Gfx &g, float x, float y, float s, Farbe decke) {
  g.kontur(1.4f, OUT);
  g.rundRechteck(x - 40 * s, y - 40 * s, 10 * s, 40 * s, 3 * s, HOLZ);
  g.rundRechteck(x + 30 * s, y - 28 * s, 10 * s, 28 * s, 3 * s, HOLZ);
  g.rundRechteck(x - 38 * s, y - 22 * s, 76 * s, 14 * s, 3 * s, HOLZ);
  g.konturAus();
  g.rundRechteck(x - 34 * s, y - 30 * s, 18 * s, 10 * s, 4 * s, WEISS);
  g.rundRechteck(x - 18 * s, y - 28 * s, 52 * s, 12 * s, 4 * s, decke);
}

static void stube(Gfx &g, float t) {
  g.verlauf(0, 200, FARBE(0xE8D2B0), FARBE(0xD6B88E));
  g.rechteck(0, 200, 284, 40, FARBE(0x9E7656));
  g.kontur(1.6f, OUT);
  g.rundRechteck(220, 40, 50, 60, 4, HOLZ);
  g.konturAus();
  g.rundRechteck(225, 45, 40, 50, 2, FARBE(0x9FD8FF));
  laubbaum(g, 245, 95, 0.4f, FARBE(0x4CB85A));
  g.rechteck(243, 45, 3, 50, HOLZ);
}

static void wald(Gfx &g, float t) {
  himmel(g, 0, t, 200);
  sonne(g, 248, 30, 11, t);
  for (int i = 0; i < 4; i++) tanne(g, i * 80 + 4, 200, 0.85f + (i % 2) * 0.15f, FARBE(0x2F8F4E));
  boden(g, 200);
  blumen(g, 0, 222, 6, 2);
  // das Baerenhaus
  g.kontur(1.8f, OUT);
  g.rechteck(150, 120, 100, 80, FARBE(0xC9A46A));
  float d[] = {140, 122, 200, 78, 260, 122};
  g.polygon(d, 3, FARBE(0x8E5A3A));
  g.konturAus();
  for (int i = 0; i < 6; i++) g.linie(150, 128 + i * 12, 250, 128 + i * 12, 1.4f, HOLZ_D);
  g.rundRechteck(188, 158, 24, 42, 10, HOLZ_D);
  g.rundRechteck(160, 138, 18, 16, 2, FARBE(0x8FD3FF));
  g.rundRechteck(222, 138, 18, 16, 2, FARBE(0x8FD3FF));
}

// =====================================================================
// Szenen
// =====================================================================
// 1) Die drei Baeren: der Brei ist zu heiss, sie gehen spazieren
static void szeneBaeren(Gfx &g, float u) {
  stube(g, u);
  g.kontur(1.4f, OUT);
  g.rundRechteck(40, 170, 140, 8, 2, HOLZ);
  g.konturAus();
  g.rechteck(50, 178, 6, 36, HOLZ_D);
  g.rechteck(164, 178, 6, 36, HOLZ_D);
  schuessel(g, 70, 170, 1.3f, 1, true, u);
  schuessel(g, 112, 170, 1.0f, 1, true, u);
  schuessel(g, 148, 170, 0.7f, 1, true, u);
  float k = seg(u, 12, 20);
  for (int i = 0; i < 3; i++) baer(g, i, lerp(60 + i * 50, 330 + i * 40, k), 214, 1, u, k > 0 && k < 1 ? fmodf(u * 1.4f + i * 0.3f, 1) : -1,
                                   u > 4 && u < 11 && ((int)(u / 2.3f) % 3) == i ? 0.5f : 0.1f);
  g.kameraAus();
  vers(g, {"Es waren einmal drei Bären:", "ein großer, ein mittlerer und", "ein kleiner Bär."}, aufzu(u, 0.4f, 4));
  blase(g, 140, 50, {"Der Brei ist noch zu heiß.", "Gehen wir ein wenig spazieren!"}, aufzu(u, 4.2f, 11), 80, 150);
  blende(g, u, 22, 140, 160);
}

// 2) Goldloeckchen und der Brei: zu heiss, zu kalt, genau richtig
static void szeneBrei(Gfx &g, float u) {
  if (u < 7) {
    wald(g, u);
    FrauPose p = fposis(GOLD_L, u);
    p.lauf = fmodf(u * 1.4f, 1);
    p.armV = 1.2f;
    frau(g, lerp(-30, 186, seg(u, 0, 6.6f)), 214, 0.75f, 1, p);
    g.kameraAus();
    vers(g, {"Da kam ein kleines Mädchen mit", "goldenen Locken durch den Wald."}, aufzu(u, 0.4f, 6.6f));
    return;
  }
  float v = u - 7;
  stube(g, u);
  g.kontur(1.4f, OUT);
  g.rundRechteck(40, 170, 150, 8, 2, HOLZ);
  g.konturAus();
  g.rechteck(50, 178, 6, 36, HOLZ_D);
  g.rechteck(174, 178, 6, 36, HOLZ_D);
  int dran = v < 6 ? 0 : v < 12 ? 1 : 2;
  float gx = 70 + dran * 40;
  schuessel(g, 70, 170, 1.3f, 1, true, u);
  schuessel(g, 112, 170, 1.0f, 1, false, u);
  schuessel(g, 148, 170, 0.7f, 1 - seg(v, 13, 19), false, u, v > 19);
  FrauPose p = fposis(GOLD_L, u);
  p.armV = 1.8f + (fmodf(v, 6) > 1 && fmodf(v, 6) < 3 ? 0.3f : 0);
  p.mund = fmodf(v, 6) > 2.6f && fmodf(v, 6) < 5.4f ? 0.6f : 0.2f;
  p.zu = dran == 2 && v > 14;
  frau(g, gx + 8, 214, 0.75f, -1, p);
  g.kameraAus();
  blase(g, 70, 100, {"Zu heiß!"}, aufzu(v, 2.6f, 5.4f), 78, 150);
  blase(g, 112, 100, {"Zu kalt!"}, aufzu(v, 8.6f, 11.4f), 118, 150);
  blase(g, 150, 100, {"Genau richtig!"}, aufzu(v, 14.6f, 17.6f), 158, 150);
  vers(g, {"Und sie aß den kleinen Teller", "ganz leer."}, aufzu(v, 18, 21));
  blende(g, u, 28, 120, 160);
}

// 3) Die Stuehle: zu hart, zu weich, genau richtig - und krach!
static void szeneStuehle(Gfx &g, float u) {
  stube(g, u);
  stuhl(g, 60, 214, 1.3f, FARBE(0x8E5A3A));
  stuhl(g, 130, 214, 1.0f, FARBE(0xC0632E));
  float krach = seg(u, 16, 16.6f);
  stuhl(g, 196, 214, 0.7f, FARBE(0x5BB8E8), krach);
  int dran = u < 6 ? 0 : u < 12 ? 1 : 2;
  float sitzX[3] = {60, 130, 196}, sitzY[3] = {214 - 24 * 1.3f + 4, 214 - 24 + 4, 214 - 24 * 0.7f + 4};
  FrauPose p = fposis(GOLD_L, u);
  p.armV = 1.0f;
  p.mund = fmodf(u, 6) > 2 && fmodf(u, 6) < 5 ? 0.6f : 0.2f;
  float py = sitzY[dran];
  if (dran == 2 && krach > 0) { py = lerp(py, 214, krach); p.mund = 0.9f; p.armV = 2.6f; p.armH = 2.6f; }
  frau(g, sitzX[dran], py, 0.75f, 1, p);
  if (u > 16 && u < 18) for (int i = 0; i < 4; i++) g.stern(196 + cosf(i * 1.57f) * 20, 196 + sinf(i * 1.57f) * 10, 3, 1.2f, 4, u * 6, OUT);
  g.kameraAus();
  blase(g, 60, 100, {"Zu hart!"}, aufzu(u, 2, 5), 64, 150);
  blase(g, 130, 100, {"Zu weich!"}, aufzu(u, 8, 11), 134, 150);
  blase(g, 196, 100, {"Genau richtig!"}, aufzu(u, 13, 15.8f), 198, 150);
  if (u > 16 && u < 20) g.text(SCHRIFT_G, "Krach!", 142, 70, FARBE(0xC0392B), 1, (uint8_t)(255 * aufzu(u, 16, 20)));
  blende(g, u, 22, 140, 160);
}

// 4) Die Betten: zu hart, zu weich, genau richtig - sie schlaeft ein
static void szeneBetten(Gfx &g, float u) {
  g.verlauf(0, 200, FARBE(0xE6D6C6), FARBE(0xD6C0A8));
  g.rechteck(0, 200, 284, 40, FARBE(0x9E7656));
  g.kontur(1.6f, OUT);
  g.rundRechteck(120, 30, 50, 50, 22, HOLZ);
  g.konturAus();
  g.rundRechteck(126, 36, 38, 38, 18, FARBE(0x9FD8FF));
  bett(g, 56, 214, 1.2f, FARBE(0x8E5A3A));
  bett(g, 150, 214, 0.95f, FARBE(0xE86F8A));
  bett(g, 236, 214, 0.65f, FARBE(0x5BB8E8));
  int dran = u < 6 ? 0 : u < 12 ? 1 : 2;
  float bx[3] = {56, 150, 236}, by[3] = {214 - 26 * 1.2f, 214 - 26 * 0.95f, 214 - 26 * 0.65f}, bs[3] = {1.2f, 0.95f, 0.65f};
  // sie liegt: Kopf auf dem Kissen
  float kx = bx[dran] - 26 * bs[dran], ky = by[dran] - 4;
  g.ellipse(kx - 2, ky - 2, 10, 7, GOLD_L.haar);
  g.kreis(kx + 2, ky, 7, HAUT);
  if (dran == 2 && u > 15) { g.bogen(kx + 4, ky, 1.8f, 0.3f, 2.8f, 1, OUT); zzz(g, kx + 10, ky - 14, u); }
  else { g.kreis(kx + 4, ky - 1, 1.1f, SCHWARZ); g.ellipse(kx + 5, ky + 4, 1.6f, fmodf(u, 6) > 2 ? 2 : 0.6f, FARBE(0x8A2A2A)); }
  g.kameraAus();
  blase(g, 56, 110, {"Zu hart!"}, aufzu(u, 2, 5), bx[0] - 20, 170);
  blase(g, 150, 110, {"Zu weich!"}, aufzu(u, 8, 11), bx[1] - 14, 170);
  blase(g, 220, 110, {"Genau richtig!"}, aufzu(u, 13, 15.6f), bx[2] - 10, 180);
  vers(g, {"Und sie schlief ganz fest ein."}, aufzu(u, 16, 21.6f));
  blende(g, u, 22, 236, 190);
}

// 5) Die Baeren kommen heim: "Wer hat ...?"
static void szeneHeim(Gfx &g, float u) {
  if (u < 30) {
    stube(g, u);
    g.kontur(1.4f, OUT);
    g.rundRechteck(40, 170, 150, 8, 2, HOLZ);
    g.konturAus();
    g.rechteck(50, 178, 6, 36, HOLZ_D);
    g.rechteck(174, 178, 6, 36, HOLZ_D);
    schuessel(g, 70, 170, 1.3f, 1, false, u);
    schuessel(g, 112, 170, 1.0f, 1, false, u);
    schuessel(g, 148, 170, 0.7f, 0, false, u, true);
    stuhl(g, 214, 214, 0.7f, FARBE(0x5BB8E8), 1);
    for (int i = 0; i < 3; i++) {
      bool spricht = (u > 2 + i * 4 && u < 5.8f + i * 4) || (u > 16 + i * 4 && u < 19.8f + i * 4);
      baer(g, i, 60 + i * 56, 214 + (i == 2 ? 0 : 0), 1, u, -1, spricht ? 0.6f : 0.1f, spricht ? 1.8f : 0.6f, spricht);
    }
    g.kameraAus();
    blase(g, 90, 40, {"Wer hat von meinem", "Brei gegessen?"}, aufzu(u, 2, 5.8f), 66, 120);
    blase(g, 140, 40, {"Wer hat von meinem", "Brei gegessen?"}, aufzu(u, 6, 9.8f), 120, 130);
    blase(g, 190, 46, {"Wer hat von meinem Brei gegessen", "und ihn ganz aufgegessen?"}, aufzu(u, 10, 13.8f), 176, 166);
    blase(g, 90, 40, {"Wer hat auf meinem", "Stuhl gesessen?"}, aufzu(u, 16, 19.8f), 66, 120);
    blase(g, 140, 40, {"Wer hat auf meinem", "Stuhl gesessen?"}, aufzu(u, 20, 23.8f), 120, 130);
    blase(g, 190, 46, {"Wer hat auf meinem Stuhl gesessen", "und ihn kaputt gemacht?"}, aufzu(u, 24, 28), 176, 166);
    if (u > 29) g.rechteck(0, 0, 284, 240, 0, (uint8_t)(255 * seg(u, 29, 30)));
    return;
  }
  float v = u - 30;
  g.verlauf(0, 200, FARBE(0xE6D6C6), FARBE(0xD6C0A8));
  g.rechteck(0, 200, 284, 40, FARBE(0x9E7656));
  bett(g, 236, 214, 0.65f, FARBE(0x5BB8E8));
  float kx = 236 - 26 * 0.65f, ky = 214 - 26 * 0.65f - 4;
  g.ellipse(kx - 2, ky - 2, 10, 7, GOLD_L.haar);
  g.kreis(kx + 2, ky, 7, HAUT);
  g.bogen(kx + 4, ky, 1.8f, 0.3f, 2.8f, 1, OUT);
  zzz(g, kx + 10, ky - 14, u);
  for (int i = 0; i < 3; i++) baer(g, i, 40 + i * 54, 214, 1, u, -1, v > 1 + i * 4 && v < 4.6f + i * 4 ? 0.6f : 0.1f, 1.2f, i == 2 && v > 8);
  g.kameraAus();
  if (v < 1) g.rechteck(0, 0, 284, 240, 0, (uint8_t)(255 * (1 - v)));
  blase(g, 70, 40, {"Wer hat in meinem", "Bett gelegen?"}, aufzu(v, 1, 4.6f), 46, 120);
  blase(g, 120, 40, {"Wer hat in meinem", "Bett gelegen?"}, aufzu(v, 5, 8.6f), 100, 130);
  blase(g, 170, 46, {"Wer hat in meinem Bett gelegen -", "und da liegt sie ja noch!"}, aufzu(v, 9, 15.6f), 150, 170);
  blende(g, u, 46, 230, 180);
}

// 6) Goldloeckchen wacht auf, springt aus dem Fenster und laeuft heim
static void szeneFlucht(Gfx &g, float u) {
  if (u < 8) {
    g.verlauf(0, 200, FARBE(0xE6D6C6), FARBE(0xD6C0A8));
    g.rechteck(0, 200, 284, 40, FARBE(0x9E7656));
    g.kontur(1.6f, OUT);
    g.rundRechteck(150, 40, 60, 60, 26, HOLZ);
    g.konturAus();
    g.rundRechteck(156, 46, 48, 48, 22, FARBE(0x9FD8FF));
    bett(g, 236, 214, 0.65f, FARBE(0x5BB8E8));
    FrauPose p = fposis(GOLD_L, u);
    p.mund = 0.9f;
    p.armV = 2.8f; p.armH = 2.8f;
    float k = seg(u, 2, 5);
    frau(g, lerp(230, 180, k), lerp(200, 100, huepf(k) * 0.6f + k * 0.4f), 0.7f, -1, p);
    for (int i = 0; i < 3; i++) baer(g, i, 40 + i * 50, 214, 1, u, -1, 0.6f, 1.6f, true);
    g.kameraAus();
    vers(g, {"Da wachte Goldlöckchen auf,", "sah die drei Bären und sprang", "aus dem Fenster."}, aufzu(u, 0.4f, 7.6f));
    return;
  }
  float v = u - 8;
  wald(g, u);
  FrauPose p = fposis(GOLD_L, u);
  p.lauf = fmodf(u * 2.4f, 1);
  p.armV = 1.6f;
  frau(g, lerp(150, -40, seg(v, 0, 7)), 214, 0.75f, -1, p);
  for (int i = 0; i < 3; i++) baer(g, i, 172 + i * 30, 214, -1, u, -1, 0.3f, 2.6f + sinf(u * 6 + i) * 0.3f);  // winken
  g.kameraAus();
  vers(g, {"Sie lief, so schnell sie konnte,", "nach Hause und kam nie wieder."}, aufzu(v, 1, 10));
  endeBild(g, v, 14);
}

static void bild(Gfx &g, float t) {
  if (t < P2) szeneBaeren(g, t - P1);
  else if (t < P3) szeneBrei(g, t - P2);
  else if (t < P4) szeneStuehle(g, t - P3);
  else if (t < P5) szeneBetten(g, t - P4);
  else if (t < P6) szeneHeim(g, t - P5);
  else szeneFlucht(g, t - P6);
}

// =====================================================================
// Musik
// =====================================================================
// Goldloeckchens Lied (F-Dur, verspielt)
static const Ton3 MEL_GOLD[] = {
  {72, .5, 41}, {77, .5, 0}, {76, .5, 0}, {74, .5, 0}, {72, 1, 48}, {69, 1, 0}, {70, .5, 46}, {72, .5, 0}, {74, .5, 0}, {76, .5, 0},
  {77, 2, 41}, {79, .5, 48}, {77, .5, 0}, {76, .5, 0}, {74, .5, 0}, {72, 1, 46}, {74, 1, 0}, {72, .5, 48}, {70, .5, 0}, {69, .5, 0}, {67, .5, 0}, {65, 2, 41}};
// die Baeren (tief und gemuetlich: gross, mittel, klein)
static void brumm(Synth &s, float x, int art) {
  float f = art == 0 ? 110 : art == 1 ? 165 : 260;
  for (int i = 0; i < 3; i++) s.ton(x + i * 0.3f, f, 0.12f, 0.02f, 0.3f, SAEGE, GERAEUSCH, f * 0.9f);
}
static void ton(Synth &s, float t0, float t1) {
  float a = P1;
  spieleMelodie(s, M(MEL_RIESE), a + 0.5f, a + 21.5f, 0.32f, t0, t1, 0.7f);
  a = P2;
  spieleMelodie(s, M(MEL_GOLD), a + 0.4f, a + 7, 0.28f, t0, t1, 0.8f);
  for (int i = 0; i < 3; i++) if (bei(a + 9.6f + i * 6, t0, t1)) { s.note(a + 9.6f + i * 6, 84 - i * 6 + (i == 2) * 12, 0.6f); }
  spieleMelodie(s, M(MEL_GOLD), a + 21.6f, a + 27.5f, 0.24f, t0, t1, 0.8f);
  a = P3;
  for (int i = 0; i < 3; i++) if (bei(a + 2 + i * 6, t0, t1)) s.note(a + 2 + i * 6, 76 + i * 3, 0.5f);
  if (bei(a + 16, t0, t1)) { s.ton(a + 16, 900, 0.15f, 0.002f, 0.2f, SAEGE, GERAEUSCH, 300); s.rauschen(a + 16, 0.4f, BANDPASS, 1400, 0.2f); }  // Krach
  a = P4;
  for (int i = 0; i < 3; i++) if (bei(a + 2 + i * 6, t0, t1)) s.note(a + 2 + i * 6, 79 - i * 3, 0.5f);
  spieleMelodie(s, M(MEL_WIEGE), a + 15, a + 21.5f, 0.42f, t0, t1, 0.6f);
  a = P5;
  for (int i = 0; i < 3; i++) {
    if (bei(a + 2 + i * 4, t0, t1)) brumm(s, a + 2 + i * 4, i);
    if (bei(a + 16 + i * 4, t0, t1)) brumm(s, a + 16 + i * 4, i);
    if (bei(a + 31 + i * 4, t0, t1)) brumm(s, a + 31 + i * 4, i);
  }
  spieleMelodie(s, M(MEL_RIESE), a + 0.4f, a + 45.5f, 0.42f, t0, t1, 0.4f);
  a = P6;
  if (bei(a + 0.6f, t0, t1)) for (int k = 0; k < 6; k++) s.note(a + 0.6f + k * 0.08f, 84 + k * 2, 0.6f);  // huch!
  spieleMelodie(s, M(MEL_JAGD), a + 2, a + 10, 0.16f, t0, t1, 0.85f);
  spieleMelodie(s, M(MEL_GOLD), a + 10.4f, a + 16, 0.28f, t0, t1, 0.8f);
  endeKlang(s, a + 16.2f, t0, t1);
}

// =====================================================================
// Startbild, Vorschau, Erkennungszeichen
// =====================================================================
static void titelSzene(Gfx &g, float t) {
  wald(g, t);
  for (int i = 0; i < 3; i++) baer(g, i, 40 + i * 44, 222, 1, t, -1, 0.3f, 1.2f);
  FrauPose p = fposis(GOLD_L, t);
  p.armV = 2.2f;
  p.mund = 0.4f;
  frau(g, 230, 222, 0.8f, -1, p);
}
static void titelbild(Gfx &g, float t) { titelSzene(g, t); titelRahmen(g, t, "Goldlöckchen", 180); }
static void vorschau(Gfx &g, float t) { titelSzene(g, t); }
static void symbol(Gfx &g, float x, float y, float t) {
  schuessel(g, x - 34, y, 1.5f, 1, true, t);
  schuessel(g, x, y, 1.15f, 1, true, t);
  schuessel(g, x + 30, y, 0.8f, 1, true, t);
  baer(g, 2, x + 2, y - 26, 1, t, -1, 0.4f, 1.6f);
}

const Maerchen GOLDLOECKCHEN = {"Goldlöckchen und die drei Bären", PDAUER, FARBE(0xF6D04A), bild, ton, titelbild, vorschau, symbol,
                                LIED(MEL_GOLD, 0.28f), "aus England"};

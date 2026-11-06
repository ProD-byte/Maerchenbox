// Die zertanzten Schuhe - ein Spieluhr-Film fuer die Maerchenbox, nach den Bruedern Grimm.
// Zwoelf Prinzessinnen, jeden Morgen sind die Schuhe durchgetanzt. Ein alter Soldat mit einem unsichtbar machenden
// Maentelchen folgt ihnen: durch Silber-, Gold- und Diamantwald, uebers Wasser ins Tanzschloss.
#include "erzaehlen.h"

// ---------- Ablauf (Sekunden) ----------
static const float P1 = 0, P2 = 26, P3 = 50, P4 = 84, P5 = 110, P6 = 140, PDAUER = 172;

static FrauLook prinzLook(int i) {
  const Farbe k[] = {FARBE(0xE86FA8), FARBE(0x9AC8F2), FARBE(0xF2C94E), FARBE(0xB28DFF), FARBE(0x7ED957), FARBE(0xFF9A6A)};
  const Farbe h[] = {FARBE(0xF2C14E), FARBE(0x7A4524), FARBE(0x2E2220), FARBE(0xE0A23C), FARBE(0xC98A3A), FARBE(0xF2C14E)};
  FrauLook l; l.kleid = k[i % 6]; l.haar = h[(i * 5) % 6]; l.krone = true; l.frisur = i % 3 == 2 ? 1 : 0; l.glanz = i == 0;
  return l;
}
static const FrauLook PR[6] = {prinzLook(0), prinzLook(1), prinzLook(2), prinzLook(3), prinzLook(4), prinzLook(5)};
static FrauLook alteLook() { FrauLook l; l.kleid = FARBE(0x6E5A4A); l.haar = FARBE(0xD8D8D8); l.frisur = 3; l.tuch = FARBE(0x8E6E5A); return l; }
static const FrauLook ALTE = alteLook();
static Aussehen soldatLook() {
  Aussehen a; a.weste = FARBE(0x2E5FAE); a.hemd = WEISS; a.hose = FARBE(0xE8E4DC); a.haar = FARBE(0x8E5A2A);
  a.hut = FARBE(0x1E1A22); a.hutD = OUT; a.feder = FARBE(0xE8473D);
  return a;
}
static Aussehen prinzenLook(int i) { Aussehen a; a.weste = i & 1 ? FARBE(0x6E2A8E) : FARBE(0x2E6E8E); a.hemd = GOLD; a.haar = i & 1 ? FARBE(0x2E2220) : FARBE(0xC98A3A); a.muetze = 3; return a; }
static const Aussehen SOLDAT = soldatLook(), PZ0 = prinzenLook(0), PZ1 = prinzenLook(1);

// ---------- Dinge ----------
static void schuhPaar(Gfx &g, float x, float y, float s, Farbe c, bool kaputt) {
  for (int i = 0; i < 2; i++) {
    float sx = x + i * 9 * s;
    g.kontur(1, OUT);
    g.ellipse(sx, y - 3 * s, 5 * s, 3 * s, c);
    g.konturAus();
    if (kaputt) g.kreis(sx - 1 * s, y - 2 * s, 1.6f * s, OUT);
  }
}

// Baum mit Blaettern aus Silber (0), Gold (1) oder Diamanten (2)
static void zauberbaum(Gfx &g, float x, float y, float s, int art, float t) {
  const Farbe f[] = {FARBE(0xD8DCE6), FARBE(0xFFD54A), FARBE(0xBFE6FF)};
  const Farbe d[] = {FARBE(0xA0A6B4), FARBE(0xE0A020), FARBE(0x7EC0EE)};
  g.linie(x, y, x, y - 60 * s, 7 * s, art == 1 ? FARBE(0xC09030) : FARBE(0x9A9AA6));
  g.linie(x, y - 36 * s, x - 18 * s, y - 56 * s, 3 * s, art == 1 ? FARBE(0xC09030) : FARBE(0x9A9AA6));
  g.linie(x, y - 42 * s, x + 18 * s, y - 62 * s, 3 * s, art == 1 ? FARBE(0xC09030) : FARBE(0x9A9AA6));
  for (int i = 0; i < 7; i++) {
    float a = i * 0.9f;
    float lx = x + cosf(a) * 22 * s, ly = y - 72 * s + sinf(a) * 14 * s;
    if (art == 2) g.stern(lx, ly, 8 * s, 4 * s, 4, a, f[2]);
    else g.ellipse(lx, ly, 10 * s, 7 * s, f[art], 255, a);
    g.ellipse(lx - 2 * s, ly - 2 * s, 3 * s, 2 * s, WEISS, 160);
    (void)d;
  }
  float b = fmodf(t * 0.7f + x * 0.01f, 1);
  g.stern(x + sinf(t + x) * 16 * s, y - 80 * s + b * 10, 3, 1, 4, t * 2, WEISS, (uint8_t)(255 * huepf(b)));
}

// das Schlafzimmer mit zwoelf Betten in zwei Reihen
static void schlafsaal(Gfx &g, float t, float nacht, int wach) {
  g.verlauf(0, 200, mischeFarbe(FARBE(0xF2E2F0), FARBE(0x3A3456), nacht), mischeFarbe(FARBE(0xE2C8E0), FARBE(0x2A2440), nacht));
  g.rechteck(0, 200, 284, 40, mischeFarbe(FARBE(0xB0889E), FARBE(0x3A2E3A), nacht));
  for (int r = 0; r < 2; r++)
    for (int i = 0; i < 6; i++) {
      float x = 26 + i * 46 + r * 10, y = r ? 206 : 160;
      g.kontur(1, OUT);
      g.rundRechteck(x - 18, y - 22, 6, 22, 2, HOLZ);
      g.rundRechteck(x - 16, y - 12, 36, 8, 2, HOLZ);
      g.konturAus();
      int n = r * 6 + i;
      if (n >= wach) {
        g.rundRechteck(x - 10, y - 18, 30, 8, 3, PR[n % 6].kleid);
        g.kreis(x - 10, y - 18, 5, HAUT);
        g.ellipse(x - 13, y - 21, 5, 3, PR[n % 6].haar);
        float kr[] = {x - 13, y - 23, x - 12, y - 27, x - 10, y - 24, x - 8, y - 27, x - 7, y - 23};
        g.polygon(kr, 5, GOLD);
      }
    }
}

// Unterirdisches Wasser mit Booten
static void boot(Gfx &g, float x, float y, float t) {
  g.kontur(1.2f, OUT);
  float b[] = {x - 22, y - 8, x + 22, y - 8, x + 16, y, x - 16, y};
  g.polygon(b, 4, FARBE(0xE8B020));
  g.konturAus();
  g.linie(x - 22, y - 8, x - 26, y - 14, 2, FARBE(0xE8B020));
  g.linie(x + 4, y - 8, x + 18, y + 6 + sinf(t * 3) * 3, 1.6f, HOLZ_D);  // Ruder
}

// =====================================================================
// Szenen
// =====================================================================
// 1) Jeden Morgen sind die Schuhe zertanzt
static void szeneMorgen(Gfx &g, float u) {
  schlafsaal(g, u, 0, 12);
  for (int i = 0; i < 12; i++) schuhPaar(g, 14 + i * 22, 230, 0.9f, PR[i % 6].kleid, true);
  koenig(g, 60, 214, 1, 1, u > 4 && u < 10 ? 0.6f : 0);
  for (int i = 0; i < 6; i++) {
    FrauPose p = fposis(PR[i], u, i);
    p.zu = fmodf(u + i, 3) < 1.2f;  // muede
    frau(g, 120 + i * 26, 214, 0.55f, -1, p);
  }
  if (u > 4) fragezeichen(g, 60, 110, 1, (uint8_t)(255 * aufzu(u, 4, 14)));
  g.kameraAus();
  vers(g, {"Ein König hatte zwölf Töchter.", "Jeden Abend wurde ihre Tür", "verschlossen."}, aufzu(u, 0.4f, 6.6f));
  vers(g, {"Doch jeden Morgen waren ihre", "Schuhe durchgetanzt!"}, aufzu(u, 7, 13.6f));
  blase(g, 110, 60, {"Wer herausfindet, wo sie tanzen,", "darf eine zur Frau nehmen!"}, aufzu(u, 14, 24), 66, 130);
  blende(g, u, 26, 140, 160);
}

// 2) Der Soldat und die alte Frau: das Maentelchen
static void szeneAlte(Gfx &g, float u) {
  himmel(g, 0, u, 190);
  sonne(g, 248, 30, 11, u);
  for (int i = 0; i < 4; i++) tanne(g, 10 + i * 80, 200, 0.8f);
  boden(g, 200);
  FrauPose a = fposis(ALTE, u);
  a.armV = u > 4 && u < 16 ? 1.6f : 0.4f;
  a.mund = (u > 4 && u < 10) || (u > 11 && u < 16) ? 0.5f : 0;
  frau(g, 70, 222, 0.85f, 1, a);
  // das Maentelchen
  bool gegeben = u > 14;
  float mx = gegeben ? 170 : 96, my = gegeben ? 156 : 170;
  if (u < 18) {
    g.kontur(1, OUT);
    float m[] = {mx - 10, my - 10, mx + 10, my - 10, mx + 14, my + 14, mx - 14, my + 14};
    g.polygon(m, 4, FARBE(0x5A4A8E));
    g.konturAus();
    glitzern(g, mx, my, 16, u, 4);
  }
  Pose s = posis(SOLDAT, u);
  s.mund = u > 0.4f && u < 4 ? 0.5f : 0;
  s.armV = gegeben ? 1.8f : 0.4f;
  float alpha = 1 - seg(u, 18, 20) * 0.7f;
  schneiderlein(g, 190, 222, 0.95f, -1, s);
  if (alpha < 1) g.kreis(190, 170, 60, FARBE(0xBFE6FF), (uint8_t)(180 * (1 - alpha)));
  g.kameraAus();
  blase(g, 200, 60, {"Ich will herausfinden,", "wo die Prinzessinnen tanzen."}, aufzu(u, 0.4f, 4), 196, 130);
  blase(g, 90, 50, {"Trink den Wein nicht,", "den sie dir abends bringen."}, aufzu(u, 4.4f, 10), 76, 130);
  blase(g, 90, 50, {"Und mit diesem Mäntelchen", "bist du unsichtbar."}, aufzu(u, 11, 16), 76, 130);
  blende(g, u, 24, 190, 160);
}

// 3) Nachts: Wein ausgekippt, Schnarchen, die Falltuer
static void szeneNacht(Gfx &g, float u) {
  if (u < 12) {
    g.verlauf(0, 200, FARBE(0x3A3456), FARBE(0x2A2440));
    g.rechteck(0, 200, 284, 40, FARBE(0x3A2E3A));
    g.kontur(1.4f, OUT);
    g.rundRechteck(150, 186, 110, 16, 3, HOLZ);
    g.konturAus();
    FrauPose p = fposis(PR[0], u);
    p.armV = u < 5 ? 1.6f : 0.4f;
    p.mund = u > 1 && u < 4 ? 0.4f : 0;
    float px = lerp(60, 110, seg(u, 0, 2));
    frau(g, u < 6 ? px : lerp(110, -40, seg(u, 6, 9)), 214, 0.8f, u < 6 ? 1 : -1, p);
    if (u < 5) { g.rechteck(px + 16, 150, 8, 12, FARBE(0xE8DCC0)); g.rechteck(px + 17, 152, 6, 6, FARBE(0x8E2A4A)); }
    // der Soldat: nimmt den Becher, kippt ihn heimlich weg, schnarcht
    float kipp = seg(u, 6, 7.4f);
    // er liegt im Bett: Kopf auf dem Kissen, blaue Decke
    g.rundRechteck(150, 172, 30, 12, 5, WEISS);
    g.rundRechteck(176, 172, 80, 16, 5, SOLDAT.weste);
    g.kreis(166, 170, 11, HAUT);
    g.ellipse(160, 164, 10, 6, SOLDAT.haar);
    if (u > 8) g.bogen(170, 170, 2.4f, 0.3f, 2.8f, 1.2f, OUT);
    else { g.kreis(171, 168, 1.6f, SCHWARZ); g.kreis(174, 176, 1.6f, FARBE(0x8A2A2A)); }
    if (u > 4.6f && u < 8) {  // die Hand mit dem Becher, heimlich ueber die Bettkante gekippt
      g.linie(182, 178, 140 + kipp * 4, 170 + kipp * 14, 4, HAUT);
      g.rechteck(132 + kipp * 4, 160 + kipp * 12, 8, 12, FARBE(0xE8DCC0));
    }
    if (kipp > 0 && kipp < 1) for (int i = 0; i < 4; i++) tropfen(g, 134 + i * 2, 178 + kipp * 24 + i * 6, 2, FARBE(0x8E2A4A));
    if (u > 8.4f) zzz(g, 180, 140, u);
    g.kameraAus();
    vers(g, {"Die Älteste brachte ihm Wein -", "er goss ihn heimlich aus", "und schnarchte laut."}, aufzu(u, 0.4f, 11.6f));
    return;
  }
  float v = u - 12;
  schlafsaal(g, u, 1, 12);
  // Falltuer im Boden: die Prinzessinnen steigen hinab, das Bett der Aeltesten versinkt
  float auf = seg(v, 2, 4);
  g.rechteck(110, 214, 70, 20, FARBE(0x100A1A));
  g.rechteck(110, 214 - auf * 18, 70, 4, HOLZ);
  for (int i = 0; i < 6; i++) {
    float st = i < 4 ? 4 + i * 1.1f : 13 + (i - 4) * 2.5f;
    float k = seg(v, st, st + 2);
    if (k >= 1) continue;
    FrauPose p = fposis(PR[i], u, i);
    p.lauf = fmodf(u * 1.2f + i * 0.2f, 1);
    p.mund = 0.3f;
    float x = lerp(30 + i * 14, 146, k), y = 214 + k * k * 40;
    if (y < 236) frau(g, x, y, 0.55f, 1, p);
  }
  // der unsichtbare Soldat (nur zart zu sehen) tritt auf das Kleid der Juengsten
  if (v > 10) {
    float k = seg(v, 10, 16);
    Pose s = posis(SOLDAT, u);
    s.lauf = fmodf(u, 1);
    g.kreis(lerp(40, 146, k), 180 + k * k * 30, 30, FARBE(0xBFE6FF), 60);
    schneiderlein(g, lerp(40, 146, k), 214 + k * k * 30, 0.6f, 1, s, nullptr, nullptr, 0);
    g.rechteck(0, 0, 284, 240, FARBE(0x2A2440), (uint8_t)(60));
  }
  g.kameraAus();
  vers(g, {"Kaum schlief er, da zogen sie", "ihre schönsten Kleider an."}, aufzu(v, 0.4f, 6));
  vers(g, {"Ein Bett sank hinab - eine Treppe!", "Unsichtbar folgte er ihnen."}, aufzu(v, 6.4f, 12));
  blase(g, 100, 70, {"Da hat mich jemand", "am Kleid gezogen!"}, aufzu(v, 13, 17), 146, 200);
  blase(g, 190, 70, {"Ach, du bist albern -", "das war ein Haken."}, aufzu(v, 17.4f, 21.6f), 146, 200);
}

// 4) Silberwald, Goldwald, Diamantwald: er bricht Zweige ab - knack!
static void szeneWald(Gfx &g, float u) {
  int art = u < 8.6f ? 0 : u < 17.2f ? 1 : 2;
  float v = fmodf(u, 8.6f);
  g.verlauf(0, 200, FARBE(0x1E1A3A), FARBE(0x3A2E5A));
  for (int i = 0; i < 4; i++) zauberbaum(g, 30 + i * 76, 200, 0.85f + (i & 1) * 0.1f, art, u);
  g.rechteck(0, 200, 284, 40, FARBE(0x4A3E6A));
  for (int i = 0; i < 6; i++) {
    FrauPose p = fposis(PR[i], u, i);
    p.lauf = fmodf(u * 1.2f + i * 0.2f, 1);
    p.blick = v > 4 && v < 8 ? -1 : 0;
    p.mund = v > 4 && v < 8 ? 0.4f : 0.1f;
    frau(g, fmodf(80 + i * 34 + u * 12, 340) - 30, 222, 0.5f, 1, p);
  }
  // der Zweig in der Hand des unsichtbaren Soldaten
  float br = seg(v, 3, 3.4f);
  float zx = 60, zy = 120 + br * 40;
  if (art == 2) g.stern(zx, zy, 6, 3, 4, u, FARBE(0xBFE6FF));
  else g.ellipse(zx, zy, 6, 4, art ? FARBE(0xFFD54A) : FARBE(0xD8DCE6));
  g.kreis(zx - 6, zy + 8, 14, FARBE(0xBFE6FF), 50);
  if (v > 3 && v < 4.4f) g.text(SCHRIFT_G, "Knack!", 120, 100, WEISS, 1, (uint8_t)(255 * aufzu(v, 3, 4.4f)));
  g.kameraAus();
  const char *namen[3] = {"Bäume mit Blättern aus Silber.", "Bäume mit Blättern aus Gold.", "Bäume aus lauter Diamanten."};
  vers(g, {namen[art], "Er brach einen Zweig ab..."}, aufzu(v, 0.2f, 3));
  if (art == 0) {
    blase(g, 140, 140, {"Hast du den Knall gehört?"}, aufzu(v, 4.4f, 6.4f), 150, 190);
    blase(g, 140, 140, {"Das sind Freudenschüsse!"}, aufzu(v, 6.6f, 8.4f), 150, 190);
  } else blase(g, 140, 140, {"Da war es wieder!"}, aufzu(v, 4.4f, 8), 150, 190);
  if (u > 25) g.rechteck(0, 0, 284, 240, 0, (uint8_t)(255 * seg(u, 25, 26)));
}

// 5) Uebers Wasser ins Tanzschloss - bis drei Uhr wird getanzt
static void szeneTanz(Gfx &g, float u) {
  if (u < 9) {
    g.verlauf(0, 170, FARBE(0x1E1A3A), FARBE(0x3A2E5A));
    // das hell erleuchtete Schloss am anderen Ufer
    g.kontur(1.4f, OUT);
    g.rechteck(180, 90, 90, 80, FARBE(0x8E86B8));
    g.konturAus();
    for (int i = 0; i < 4; i++) g.rundRechteck(188 + i * 20, 110, 12, 18, 5, FARBE(0xFFD86A));
    g.rechteck(0, 170, 284, 70, FARBE(0x2E4A7A));
    for (int i = 0; i < 6; i++) g.linie(fmodf(i * 60 + u * 20, 300) - 10, 190 + i * 7, fmodf(i * 60 + u * 20, 300) + 20, 190 + i * 7, 1.4f, FARBE(0x5B7FC7));
    for (int i = 0; i < 3; i++) {
      float bx = lerp(-40 + i * 50, 120 + i * 50, seg(u, 0, 8));
      boot(g, bx, 196 + i * 10, u);
      Pose pz = posis(i & 1 ? PZ1 : PZ0, u, i);
      pz.armV = 1.6f + sinf(u * 3) * 0.3f;
      schneiderlein(g, bx + 10, 188 + i * 10, 0.45f, 1, pz);
      FrauPose p = fposis(PR[i], u, i);
      frau(g, bx - 8, 188 + i * 10, 0.42f, 1, p);
    }
    g.kameraAus();
    vers(g, {"Am Wasser warteten zwölf Boote", "mit zwölf Prinzen. Der Soldat", "fuhr mit der Jüngsten."}, aufzu(u, 0.4f, 8.6f));
    return;
  }
  float v = u - 9;
  g.verlauf(0, 190, FARBE(0x6E5A9E), FARBE(0x3A2E6E));
  for (int i = 0; i < 5; i++) g.kreis(30 + i * 56, 30, 8, FARBE(0xFFD86A), 200);  // Kronleuchter
  g.rechteck(0, 190, 284, 50, FARBE(0x8E7AB8));
  // sechs Paare drehen sich im Kreis
  for (int i = 0; i < 6; i++) {
    float a = v * 0.8f + i * 1.047f;
    float x = 142 + cosf(a) * 96, y = 214 + sinf(a) * 10;
    FrauPose p = fposis(PR[i], u, i);
    p.drehen = sinf(v * 4 + i);
    p.armV = 1.6f;
    p.mund = 0.4f;
    frau(g, x - 8, y, 0.55f, 1, p);
    Pose pz = posis(i & 1 ? PZ1 : PZ0, u, i);
    pz.armV = 1.6f;
    schneiderlein(g, x + 10, y, 0.55f, -1, pz);
  }
  // der Becher: der Soldat trinkt ihn aus
  if (v > 10 && v < 18) {
    float k = seg(v, 11, 13);
    g.kontur(1, OUT);
    g.rechteck(220, 110 - k * 20, 10, 14, GOLD);
    g.konturAus();
    g.kreis(222, 118, 18, FARBE(0xBFE6FF), 60);
  }
  for (int i = 0; i < 6; i++) schuhPaar(g, 20 + i * 44, 236, 0.8f, PR[i].kleid, v > 18);
  g.kameraAus();
  vers(g, {"Im Schloss tanzten sie mit den", "Prinzen bis drei Uhr morgens,", "bis die Schuhe durchgetanzt waren."}, aufzu(v, 0.4f, 10));
  vers(g, {"Der Soldat trank heimlich", "aus ihrem Becher."}, aufzu(v, 10.4f, 18));
  blende(g, u, 30, 142, 160);
}

// 6) Vor dem Koenig: Zweige und Becher - Hochzeit
static void szeneKoenig(Gfx &g, float u) {
  g.verlauf(0, 190, FARBE(0xF6E2B4), FARBE(0xE8C483));
  for (int i = 0; i < 4; i++) g.rechteck(20 + i * 82 - 10, 30, 20, 160, FARBE(0xFFF3D6));
  g.rechteck(0, 190, 284, 50, FARBE(0xC0392B));
  koenig(g, 50, 214, 1, 1, u > 10 ? 0.6f : 0);
  Pose s = posis(SOLDAT, u);
  s.armV = u > 4 && u < 12 ? 1.8f : 0.6f;
  s.mund = u > 0.4f && u < 10 ? 0.5f : 0.2f;
  schneiderlein(g, 116, 214, 0.95f, 1, s);
  if (u > 4 && u < 12) {
    g.ellipse(140, 160, 6, 4, FARBE(0xD8DCE6));
    g.ellipse(146, 166, 6, 4, FARBE(0xFFD54A));
    g.stern(140, 172, 6, 3, 4, u, FARBE(0xBFE6FF));
    g.rechteck(150, 150, 9, 13, GOLD);
  }
  for (int i = 0; i < 6; i++) {
    FrauPose p = fposis(PR[i], u, i);
    p.mund = i == 0 && u > 14 ? 0.4f : 0.1f;
    p.blick = -1;
    frau(g, 170 + i * 20, 214, i == 0 ? 0.8f : 0.6f, -1, p);
  }
  if (u > 15) herzen(g, 140, 160, u, 5, 40);
  g.kameraAus();
  blase(g, 110, 50, {"Sie tanzen mit zwölf Prinzen", "in einem Schloss unter der Erde!"}, aufzu(u, 0.4f, 6), 120, 130);
  vers(g, {"Er zeigte die Zweige und den", "Becher. Da gestanden sie alles."}, aufzu(u, 6.4f, 13.6f));
  blase(g, 200, 50, {"Ich bin nicht mehr jung -", "so nehme ich die Älteste."}, aufzu(u, 14, 20), 122, 130);
  vers(g, {"Noch am selben Tag", "wurde Hochzeit gefeiert."}, aufzu(u, 20.4f, 27.6f));
  endeBild(g, u, 28, 140, 160);
}

static void bild(Gfx &g, float t) {
  if (t < P2) szeneMorgen(g, t - P1);
  else if (t < P3) szeneAlte(g, t - P2);
  else if (t < P4) szeneNacht(g, t - P3);
  else if (t < P5) szeneWald(g, t - P4);
  else if (t < P6) szeneTanz(g, t - P5);
  else szeneKoenig(g, t - P6);
}

// =====================================================================
// Musik
// =====================================================================
// der heimliche Ball (Walzer, d-Moll -> F-Dur)
static const Ton3 MEL_TANZ[] = {
  {74, 1, 50}, {77, 1, 0}, {81, 1, 0}, {79, 2, 46}, {77, 1, 0}, {76, 1, 45}, {77, 1, 0}, {79, 1, 0}, {81, 3, 41},
  {82, 1, 46}, {81, 1, 0}, {79, 1, 0}, {77, 2, 48}, {76, 1, 0}, {74, 1, 45}, {73, 1, 0}, {76, 1, 0}, {74, 3, 50}};
static void knack(Synth &s, float x) { s.rauschen(x, 0.08f, HOCHPASS, 2500, 0.4f); s.ton(x, 1400, 0.15f, 0.001f, 0.08f, RECHTECK, GERAEUSCH, 500); }
static void ton(Synth &s, float t0, float t1) {
  float a = P1;
  spieleMelodie(s, M(MEL_TANZ), a + 0.5f, a + 25.5f, 0.32f, t0, t1, 0.45f);
  a = P2;
  spieleMelodie(s, M(MEL_WANDER), a + 0.4f, a + 11, 0.36f, t0, t1, 0.5f);
  if (bei(a + 14, t0, t1)) for (int k = 0; k < 6; k++) s.note(a + 14 + k * 0.07f, 79 + k * 2, 0.5f);
  if (bei(a + 18, t0, t1)) for (int k = 0; k < 6; k++) s.note(a + 18 + k * 0.1f, 91 - k * 2, 0.4f);
  a = P3;
  spieleMelodie(s, M(MEL_WIEGE), a + 0.4f, a + 6, 0.42f, t0, t1, 0.4f);
  for (int i = 0; i < 4; i++) if (bei(a + 8.4f + i * 0.9f, t0, t1)) s.ton(a + 8.4f + i * 0.9f, 90, 0.3f, 0.15f, 0.5f, SAEGE, GERAEUSCH, 70);  // schnarch
  spieleMelodie(s, M(MEL_SPUK), a + 12.4f, a + 34, 0.42f, t0, t1, 0.4f);
  a = P4;
  spieleMelodie(s, M(MEL_TANZ), a + 0.2f, a + 25.5f, 0.42f, t0, t1, 0.35f);
  for (int i = 0; i < 3; i++) if (bei(a + 3 + i * 8.6f, t0, t1)) knack(s, a + 3 + i * 8.6f);
  for (int i = 0; i < 9; i++) if (bei(a + 1 + i * 2.7f, t0, t1)) s.glocke(a + 1 + i * 2.7f, 96, 0.15f);
  a = P5;
  for (int i = 0; i < 8; i++) if (bei(a + 0.4f + i * 1.1f, t0, t1)) s.rauschen(a + 0.4f + i * 1.1f, 0.6f, BANDPASS, 600, 0.06f);  // Wasser
  spieleMelodie(s, M(MEL_TANZ), a + 9.4f, a + 29.5f, 0.26f, t0, t1, 0.85f);
  a = P6;
  spieleMelodie(s, M(MEL_KEGEL), a + 0.4f, a + 13, 0.32f, t0, t1, 0.5f);
  spieleMelodie(s, M(MEL_TANZ), a + 14, a + 27.5f, 0.28f, t0, t1, 0.8f);
  endeKlang(s, a + 28.2f, t0, t1);
}

// =====================================================================
// Startbild, Vorschau, Erkennungszeichen
// =====================================================================
static void titelSzene(Gfx &g, float t) {
  g.verlauf(0, 200, FARBE(0x1E1A3A), FARBE(0x3A2E5A));
  for (int i = 0; i < 4; i++) zauberbaum(g, 30 + i * 76, 200, 0.8f, i % 3, t);
  g.rechteck(0, 200, 284, 40, FARBE(0x4A3E6A));
  for (int i = 0; i < 4; i++) {
    FrauPose p = fposis(PR[i], t, i);
    p.drehen = sinf(t * 3 + i);
    p.armV = 1.8f + sinf(t * 2 + i) * 0.3f;
    frau(g, 50 + i * 62, 226, 0.65f, i & 1 ? -1 : 1, p);
  }
}
static void titelbild(Gfx &g, float t) { titelSzene(g, t); titelRahmen(g, t, "Die zertanzten Schuhe", 210, SCHRIFT_M); }
static void vorschau(Gfx &g, float t) { titelSzene(g, t); }
static void symbol(Gfx &g, float x, float y, float t) {
  for (int i = 0; i < 3; i++) {
    float sx = x - 52 + i * 38, sy = y - 6 - fabsf(sinf(t * 3 + i)) * 10;
    schuhPaar(g, sx, sy, 2.6f, PR[i].kleid, true);
  }
  glitzern(g, x, y - 30, 40, t, 5);
}

const Maerchen ZERTANZT = {"Die zertanzten Schuhe", PDAUER, FARBE(0xB28DFF), bild, ton, titelbild, vorschau, symbol,
                           LIED(MEL_TANZ, 0.32f), nullptr};

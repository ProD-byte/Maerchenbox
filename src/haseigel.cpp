// Der Hase und der Igel - ein Spieluhr-Film fuer die Maerchenbox, nach dem Maerchen der Brueder Grimm.
// Der Spott ueber die krummen Beine, die Wette, Frau Igel am anderen Ende der Furche: "Ick bün all hier!" -
// dreiundsiebzigmal. Beim vierundsiebzigsten Mal bleibt der Hase erschoepft liegen.
#include "erzaehlen.h"

// ---------- Ablauf (Sekunden) ----------
static const float P1 = 0, P2 = 22, P3 = 62, P4 = 82, P5 = 140, PDAUER = 172;

// ---------- Figuren ----------
static void igel(Gfx &g, float x, float y, float s, float dir, float t, float lauf = -1, float mund = 0, bool frau = false,
                 bool zu = false) {
  auto PX = [&](float d) { return x + d * s * dir; };
  auto PY = [&](float d) { return y + d * s; };
  const Farbe stachel = FARBE(0x6E4A2A), hell = FARBE(0xD8B88E), dunkel = FARBE(0x4A3020);
  float hop = lauf >= 0 ? fabsf(sinf(lauf * 6.28f)) * 2 * s : 0;
  // Beinchen (krumm!)
  for (int i = 0; i < 2; i++) {
    float sw = lauf >= 0 ? sinf(lauf * 6.28f + i * 3.14f) * 3 : 0;
    g.linie(PX(-4 + i * 9), PY(-4) - hop, PX(-6 + i * 9 + sw), PY(0), 2.6f * s, dunkel);
  }
  // Stacheln
  for (int i = 0; i < 11; i++) {
    float a = 3.4f + i * 0.24f;
    float bx = PX(-2 + cosf(a) * 13), by = PY(-11 + sinf(a) * 10) - hop;
    float ex = PX(-2 + cosf(a) * 21), ey = PY(-11 + sinf(a) * 17) - hop;
    g.linie(bx, by, ex, ey, 3.4f * s, i & 1 ? stachel : dunkel);
  }
  g.kontur(1.1f * s, OUT);
  g.ellipse(PX(-2), PY(-11) - hop, 15 * s, 10 * s, stachel);
  g.ellipse(PX(9), PY(-9) - hop, 8 * s, 7 * s, hell);  // Gesicht
  g.konturAus();
  g.kreis(PX(17), PY(-9) - hop, 2.2f * s, SCHWARZ);  // Nase
  if (zu) g.bogen(PX(10), PY(-12) - hop, 1.6f * s, 3.5f, 5.9f, 1 * s, OUT);
  else g.kreis(PX(10), PY(-12) - hop, 1.5f * s, SCHWARZ);
  if (mund > 0.05f) g.ellipse(PX(13), PY(-6) - hop, 1.8f * s, (0.6f + mund * 2) * s, FARBE(0x8A2A2A));
  else g.bogen(PX(13), PY(-7) - hop, 2.4f * s, 0.5f, 2.6f, 0.9f * s, OUT);
  if (frau) {  // ein kleines Haeubchen, sonst genau wie er
    g.ellipse(PX(4), PY(-19) - hop, 6 * s, 3 * s, WEISS);
    g.kreis(PX(0), PY(-18) - hop, 2 * s, FARBE(0xE8473D));
  }
}

static void hase(Gfx &g, float x, float y, float s, float dir, float t, float lauf = -1, float mund = 0, bool zu = false,
                 float erschoepft = 0) {
  auto PX = [&](float d) { return x + d * s * dir; };
  auto PY = [&](float d) { return y + d * s; };
  const Farbe fell = FARBE(0xB89A7A), hell = FARBE(0xEEDCC4), dunkel = FARBE(0x8E6E4E);
  if (erschoepft > 0) {  // liegt lang ausgestreckt, japst
    g.kontur(1.1f * s, OUT);
    g.ellipse(PX(0), PY(-7), 24 * s, 7 * s, fell);
    g.kreis(PX(22), PY(-8), 7 * s, fell);
    g.ellipse(PX(30), PY(-14), 12 * s, 3 * s, fell, 255, 0.3f * dir);
    g.ellipse(PX(26), PY(-16), 12 * s, 3 * s, dunkel, 255, 0.5f * dir);
    g.konturAus();
    g.bogen(PX(24), PY(-10), 1.6f * s, 3.5f, 5.9f, 1 * s, OUT);
    g.ellipse(PX(27), PY(-5), 2 * s, (1 + fabsf(sinf(t * 10)) * 1.5f) * s, FARBE(0xE86F7A));
    g.kreis(PX(-24), PY(-8), 4 * s, WEISS);
    return;
  }
  float l = lauf >= 0 ? lauf * 6.2832f : 0;
  float strecke = lauf >= 0 ? sinf(l) : 0;
  float hop = lauf >= 0 ? fabsf(sinf(l)) * 8 * s : 0;
  g.kontur(1.1f * s, OUT);
  // Hinterbein
  g.ellipse(PX(-10 - strecke * 8), PY(-6) - hop, 10 * s, 5 * s, dunkel, 255, -0.2f * dir);
  g.ellipse(PX(0), PY(-16) - hop, 16 * s, 11 * s, fell, 255, -0.1f * dir);
  g.kreis(PX(-15), PY(-18) - hop, 4.5f * s, WEISS);  // Stummelschwanz
  // Vorderbein
  g.linie(PX(8), PY(-10) - hop, PX(12 + strecke * 8), PY(-2) - hop * 0.5f, 3.4f * s, fell);
  // Kopf
  float kx = PX(15), ky = PY(-28) - hop;
  g.kreis(kx, ky, 8 * s, fell);
  // lange Ohren
  float wipp = sinf(t * 3) * 0.15f + (lauf >= 0 ? 0.5f : 0);
  for (int i = 0; i < 2; i++) {
    float w = -1.3f + i * 0.35f - wipp;
    float ox = kx - 2 * s * dir + cosf(w) * 0 * dir, oy = ky - 6 * s;
    float ex = ox + sinf(-w) * -14 * s * dir * -1, ey = oy - cosf(w + 1.57f) * 0 - 18 * s;
    g.ellipse((ox + ex) / 2 - (i ? 2 : 4) * s * dir - wipp * 8 * s * dir, (oy + ey) / 2, 3.6f * s, 11 * s, i ? fell : dunkel, 255, (-0.2f - wipp) * dir);
  }
  g.konturAus();
  g.ellipse(kx + 5 * s * dir, ky + 2 * s, 4 * s, 3 * s, hell);
  g.kreis(kx + 8.5f * s * dir, ky + 1 * s, 1.4f * s, FARBE(0xE87A8A));
  if (zu) g.bogen(kx + 2 * s * dir, ky - 2 * s, 1.8f * s, 3.5f, 5.9f, 1 * s, OUT);
  else { g.kreis(kx + 2 * s * dir, ky - 2 * s, 1.8f * s, SCHWARZ); g.kreis(kx + 2.6f * s * dir, ky - 2.6f * s, 0.6f * s, WEISS); }
  if (mund > 0.05f) g.ellipse(kx + 5 * s * dir, ky + 5 * s, 1.6f * s, (0.6f + mund * 2) * s, FARBE(0x8A2A2A));
  for (int i = -1; i <= 1; i++) g.linie(kx + 7 * s * dir, ky + 2 * s, kx + 13 * s * dir, ky + 1 * s + i * 2 * s, 0.5f * s, OUT);
}

// ---------- Kulissen ----------
static void acker(Gfx &g, float t) {
  himmel(g, 0, t, 170);
  sonne(g, 60, 32, 12, t);
  wolke(g, fmodf(150 + t * 3, 340) - 30, 36, 0.7f);
  huegel(g, 0, 0.3f, 170, 26, 110, FARBE(0x8ACB6A));
  g.rechteck(0, 170, 284, 70, FARBE(0x9A7A4E));
  for (int i = 0; i < 5; i++) {  // Furchen
    float y = 176 + i * 12;
    g.rechteck(0, y, 284, 3, FARBE(0x7A5A36));
  }
  // Steckrueben am oberen Rand
  for (int i = 0; i < 8; i++) {
    float x = 14 + i * 36;
    g.kreis(x, 172, 5, FARBE(0xE8D0E8));
    for (int k = 0; k < 3; k++) g.ellipse(x - 4 + k * 4, 162, 2.4f, 6, FARBE(0x4CA85A), 255, (k - 1) * 0.4f);
  }
  // Schlehenbusch (dort traf der Igel den Hasen)
  g.kreis(250, 166, 16, FARBE(0x3E7A40));
  g.kreis(236, 172, 12, FARBE(0x4E8A4A));
}

static void haeuschen(Gfx &g, float t) {
  himmel(g, 0, t, 196);
  sonne(g, 60, 32, 12, t);
  boden(g, 196);
  // das Igelhaeuschen unter dem Busch
  g.kreis(150, 186, 50, FARBE(0x3E7A40));
  g.kreis(110, 196, 30, FARBE(0x4E8A4A));
  g.kreis(196, 196, 30, FARBE(0x4E8A4A));
  g.rundRechteck(130, 180, 40, 34, 16, FARBE(0x3A2A1E));
  for (int i = 0; i < 6; i++) g.kreis(100 + i * 18, 206, 2.4f, i & 1 ? GOLD : WEISS);
}

// =====================================================================
// Szenen
// =====================================================================
// 1) Sonntagmorgen: der Igel singt vor seiner Tuer und will nach seinen Steckrueben sehen
static void szeneMorgen(Gfx &g, float u) {
  haeuschen(g, u);
  float x = u < 12 ? 120 : lerp(120, 330, seg(u, 12, 21));
  igel(g, x, 214, 1.2f, 1, u, u > 12 ? fmodf(u * 1.4f, 1) : -1, u < 10 ? 0.4f + 0.3f * fabsf(sinf(u * 5)) : 0, false, u < 10);
  if (u < 10)
    for (int i = 0; i < 3; i++) {
      float ph = fmodf(u * 0.45f + i / 3.0f, 1);
      notenzeichen(g, x + 16 + ph * 26, 186 - ph * 40, i & 1 ? FARBE(0xB28DFF) : FARBE(0xE8473D), (uint8_t)(230 * huepf(ph)));
    }
  igel(g, 160, 214, 1, -1, u + 1, -1, 0, true, true);  // Frau Igel waescht die Kinder
  g.kameraAus();
  vers(g, {"Es war an einem Sonntagmorgen", "zur Herbstzeit, gerade als der", "Buchweizen blühte."}, aufzu(u, 0.4f, 7));
  vers(g, {"Der Igel stand vor seiner Tür und", "sang ein Liedchen vor sich hin."}, aufzu(u, 7.2f, 12));
  vers(g, {"Er wollte nur eben nach", "seinen Steckrüben sehen."}, aufzu(u, 12.2f, 20));
  blende(g, u, 22, 140, 170);
}

// 2) Der Hase spottet ueber die krummen Beine; die Wette
static void szeneWette(Gfx &g, float u) {
  acker(g, u);
  float ix = lerp(-20, 100, sanft(seg(u, 0, 4)));
  bool spricht = (u > 9 && u < 12) || (u > 17.6f && u < 22) || (u > 26 && u < 32);
  igel(g, ix, 214, 1.3f, 1, u, u < 4 ? fmodf(u * 1.4f, 1) : -1, spricht ? 0.5f : 0);
  bool haseSpricht = (u > 4.6f && u < 9) || (u > 12.2f && u < 17.4f) || (u > 22.2f && u < 26);
  hase(g, 190, 214, 1.3f, -1, u, -1, haseSpricht ? 0.5f : 0, u > 22.2f && u < 26);
  g.kameraAus();
  blase(g, 200, 70, {"Wie kommt es, dass du schon", "so früh im Felde herumläufst?"}, aufzu(u, 4.6f, 9), 180, 160);
  blase(g, 90, 80, {"Ich gehe spazieren."}, aufzu(u, 9.2f, 12), 110, 180);
  blase(g, 200, 70, {"Mich deucht, du könntest deine", "Beine auch wohl zu besseren", "Dingen gebrauchen!"}, aufzu(u, 12.2f, 17.4f), 180, 160);
  blase(g, 90, 70, {"Du meinst wohl, dass du mit", "deinen Beinen mehr ausrichten", "kannst?"}, aufzu(u, 17.6f, 22), 110, 180);
  blase(g, 200, 70, {"Das mein ich!"}, aufzu(u, 22.2f, 26), 180, 160);
  blase(g, 90, 70, {"Ich wette, wenn wir in die Wette", "laufen, ich laufe an dir vorbei!"}, aufzu(u, 26.2f, 32), 110, 180);
  vers(g, {"Der Hase lachte. Sie wetteten", "um einen goldenen Louisdor."}, aufzu(u, 32.4f, 39.6f));
  blende(g, u, 40, 140, 170);
}

// 3) Der Igel holt seine Frau: sie sieht genau so aus wie er
static void szeneFrau(Gfx &g, float u) {
  haeuschen(g, u);
  igel(g, 120, 214, 1.3f, 1, u, -1, u > 2 && u < 11 ? 0.4f + 0.3f * fabsf(sinf(u * 6)) : 0);
  igel(g, 170, 214, 1.3f, -1, u + 2, -1, u > 11 && u < 14 ? 0.6f : 0, true);
  if (u > 11 && u < 16) fragezeichen(g, 180, 150, 1, (uint8_t)(255 * aufzu(u, 11, 16)));
  g.kameraAus();
  blase(g, 110, 80, {"Frau, zieh dich an,", "du musst mit mir ins Feld!"}, aufzu(u, 2, 6), 130, 180);
  blase(g, 110, 80, {"Stell dich unten an die Furche.", "Wenn der Hase kommt, rufst du:", "Ick bün all hier!"}, aufzu(u, 6.2f, 11), 130, 180);
  vers(g, {"Frau Igel sah genau so aus", "wie ihr Mann."}, aufzu(u, 13, 19.6f));
  blende(g, u, 20, 140, 170);
}

// 4) Das Wettrennen: "Ick bün all hier!" - dreiundsiebzigmal
static void szeneRennen(Gfx &g, float u) {
  acker(g, u);
  // die beiden Igel an den Enden der Furche
  bool rufLinks = false, rufRechts = false;
  // Runden werden immer schneller: Rundendauer von 6 s auf 0,38 s
  float zeit = u - 6;
  int runde = 0;
  float rest = zeit, dauer = 6;
  while (rest > dauer && runde < 74) { rest -= dauer; runde++; dauer = dauer * 0.7f < 0.38f ? 0.38f : dauer * 0.7f; }
  float k = zeit < 0 ? 0 : klemme(rest / dauer);
  bool vorbei = runde >= 73;
  if (zeit > 0 && !vorbei) {
    bool nachRechts = (runde & 1) == 0;
    float hx = nachRechts ? lerp(44, 240, k) : lerp(240, 44, k);
    hase(g, hx, 214, 1.1f, nachRechts ? 1 : -1, u, fmodf(u * (2 + runde * 0.1f), 1), 0.3f);
    if (k > 0.92f) { if (nachRechts) rufRechts = true; else rufLinks = true; }
  }
  if (vorbei) hase(g, 140, 220, 1.1f, 1, u, -1, 0, true, 1);
  float jx = 28, fx = 256;
  igel(g, jx, 214, 1.1f, 1, u, -1, rufLinks ? 0.7f : 0);
  igel(g, fx, 214, 1.1f, -1, u + 3, -1, rufRechts ? 0.7f : 0, true);
  if (u < 6) {
    hase(g, 50, 214, 1.1f, 1, u, -1, u > 2 ? 0.6f : 0);
    g.kameraAus();
    blase(g, 90, 80, {"Eins, zwei, drei!"}, aufzu(u, 2, 5.6f), 60, 180);
  }
  g.kameraAus();
  if (rufLinks) blase(g, 60, 110, {"Ick bün all hier!"}, 1, 30, 190);
  if (rufRechts) blase(g, 220, 110, {"Ick bün all hier!"}, 1, 254, 190);
  if (zeit > 0) {
    char b[16];
    snprintf(b, sizeof b, "%d.", runde + 1 > 74 ? 74 : runde + 1);
    g.rundRechteck(112, 6, 60, 30, 10, FARBE(0x1E1A3A), 160);
    g.text(SCHRIFT_M, b, 142, 28, GOLD, 1);
  }
  if (vorbei) vers(g, {"Beim vierundsiebzigsten Mal", "kam der Hase nicht mehr", "bis ans Ende."}, aufzu(u, 52, 58));
}

// 5) Heim mit dem goldenen Louisdor; die Lehre
static void szeneLehre(Gfx &g, float u) {
  haeuschen(g, u);
  float x = lerp(-40, 120, sanft(seg(u, 0, 5)));
  igel(g, x, 214, 1.3f, 1, u, u < 5 ? fmodf(u * 1.4f, 1) : -1, 0.5f);
  igel(g, x + 46, 214, 1.3f, 1, u + 2, u < 5 ? fmodf(u * 1.4f + 0.5f, 1) : -1, 0.5f, true);
  float gx = x + 24, gy = 186 - fabsf(sinf(u * 3)) * 4;
  g.kontur(1.2f, FARBE(0x9A6A1E));
  g.kreis(gx, gy, 7, GOLD);
  g.konturAus();
  g.stern(gx, gy, 4, 1.4f, 5, u, WEISS, 200);
  herzen(g, x + 24, 170, u, 3, 20);
  g.kameraAus();
  vers(g, {"Der Igel und seine Frau gingen", "vergnügt mit dem Goldstück", "nach Haus."}, aufzu(u, 2, 10));
  vers(g, {"Und die Lehre: Keiner soll sich", "über den geringen Mann", "lustig machen!"}, aufzu(u, 10.2f, 22));
  endeBild(g, u, 26);
  if (u < 1.2f) g.iris(142, 160, lerp(0, 330, sanft(u / 1.2f)));
}

static void bild(Gfx &g, float t) {
  if (t < P2) szeneMorgen(g, t - P1);
  else if (t < P3) szeneWette(g, t - P2);
  else if (t < P4) szeneFrau(g, t - P3);
  else if (t < P5) szeneRennen(g, t - P4);
  else szeneLehre(g, t - P5);
}

// =====================================================================
// Musik
// =====================================================================
// das Liedchen des Igels (G-Dur, gemuetlich)
static const Ton3 MEL_IGEL[] = {
  {67, .5, 43}, {69, .5, 0}, {71, 1, 0}, {74, 1, 0}, {72, .5, 48}, {71, .5, 0}, {69, 1, 0}, {67, 1, 0},
  {69, .5, 50}, {71, .5, 0}, {72, 1, 0}, {76, 1, 0}, {74, .5, 43}, {72, .5, 0}, {71, 1, 0}, {67, 1, 43}};

static void ton(Synth &s, float t0, float t1) {
  float a = P1;
  spieleMelodie(s, M(MEL_IGEL), a + 0.5f, a + 21.5f, 0.32f, t0, t1, 0.8f);
  a = P2;
  spieleMelodie(s, M(MEL_IGEL), a + 0.4f, a + 4.4f, 0.3f, t0, t1, 0.7f);
  spieleMelodie(s, M(MEL_WANDER), a + 4.6f, a + 39.5f, 0.32f, t0, t1, 0.6f);
  a = P3;
  spieleMelodie(s, M(MEL_IGEL), a + 0.4f, a + 19.5f, 0.28f, t0, t1, 0.75f);
  a = P4;
  for (int i = 0; i < 3; i++) if (bei(a + 2.4f + i * 0.9f, t0, t1)) s.note(a + 2.4f + i * 0.9f, 72 + i * 4, 0.7f);  // eins, zwei, drei
  spieleMelodie(s, M(MEL_JAGD), a + 6, a + 52, 0.14f, t0, t1, 0.75f);
  // jedes "Ick bün all hier!" ein heller Ruf
  float zeit = 0, dauer = 6;
  for (int r = 0; r < 73; r++) {
    zeit += dauer;
    float x = a + 6 + zeit - dauer * 0.08f;
    if (bei(x, t0, t1)) { s.note(x, 84, 0.5f); s.note(x + 0.12f, 88, 0.5f); }
    dauer = dauer * 0.7f < 0.38f ? 0.38f : dauer * 0.7f;
  }
  if (bei(a + 51.6f, t0, t1)) for (int k = 0; k < 8; k++) s.note(a + 51.6f + k * 0.12f, 76 - k * 3, 0.6f);  // plumps
  spieleMelodie(s, M(MEL_WIEGE), a + 53, a + 57.5f, 0.42f, t0, t1, 0.5f);
  a = P5;
  spieleMelodie(s, M(MEL_IGEL), a + 0.4f, a + 26, 0.3f, t0, t1, 0.85f);
  endeKlang(s, a + 28.2f, t0, t1);
}

// =====================================================================
// Startbild, Vorschau, Erkennungszeichen
// =====================================================================
static void titelSzene(Gfx &g, float t) {
  acker(g, t);
  hase(g, 170, 222, 1.4f, -1, t, -1, 0.3f);
  igel(g, 90, 222, 1.5f, 1, t, -1, 0.4f);
}
static void titelbild(Gfx &g, float t) { titelSzene(g, t); titelRahmen(g, t, "Der Hase und der Igel", 204, SCHRIFT_M); }
static void vorschau(Gfx &g, float t) { titelSzene(g, t); }
static void symbol(Gfx &g, float x, float y, float t) {
  hase(g, x + 22, y, 1.3f, -1, t, fmodf(t * 1.4f, 1));
  igel(g, x - 26, y, 1.5f, 1, t, -1, fmodf(t, 2) < 1 ? 0.6f : 0);
}

const Maerchen HASE_IGEL = {"Der Hase und der Igel", PDAUER, FARBE(0x9A7A4E), bild, ton, titelbild, vorschau, symbol,
                            LIED(MEL_IGEL, 0.3f), nullptr};

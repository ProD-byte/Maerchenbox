// Die goldene Gans - ein Spieluhr-Film fuer die Maerchenbox, nach den Bruedern Grimm (KHM 64, Ausgabe 1857).
// Der Dummling teilt Aschenkuchen und saures Bier mit dem grauen Maennlein, findet die goldene Gans,
// und alle, die sie anfassen, bleiben kleben: drei Wirtstoechter, Pfarrer, Kuester und zwei Bauern.
// Die Koenigstochter lacht, der Weinkeller, der Brotberg und das Schiff zu Land und zu Wasser.
#include "erzaehlen.h"

// ---------- Ablauf (Sekunden) ----------
static const float P1 = 0, P2 = 22, P3 = 46, P4 = 66, P5 = 92, P6 = 114, P7 = 140, P8 = 168, P9 = 192, P10 = 216,
                   P11 = 238, P12 = 262, PDAUER = 284;

static Aussehen bLook(int i) {
  Aussehen a;
  const Farbe w[] = {FARBE(0xE07A2A), FARBE(0x8E2A2A), FARBE(0x3E6E3E), FARBE(0x2A2A34), FARBE(0x5A5A6E), FARBE(0x7A5A3A), FARBE(0x5A7A3A)};
  const Farbe h[] = {FARBE(0xE0A23C), FARBE(0x3A2A20), FARBE(0x5A3A26), FARBE(0xC8C8C8), FARBE(0x8E8E8E), FARBE(0xC98A3A), FARBE(0x6E4A2E)};
  a.weste = w[i]; a.haar = h[i]; a.muetze = i == 0 || i >= 5 ? 1 : 0; a.hut = FARBE(0x2A2A34); a.hutD = OUT; a.feder = a.hut;
  if (i == 3) { a.hemd = WEISS; a.halstuch = WEISS; }
  return a;
}
// 0 Dummling, 1 aeltester Bruder, 2 zweiter Bruder, 3 Pfarrer, 4 Kuester, 5/6 Bauern
static const Aussehen DUMM = bLook(0), BR1 = bLook(1), BR2 = bLook(2), PFARRER = bLook(3), KUESTER = bLook(4), BAUER1 = bLook(5),
                      BAUER2 = bLook(6);
static FrauLook tLook(int i) {
  FrauLook l;
  const Farbe k[] = {FARBE(0xC84A5A), FARBE(0x4E8EDC), FARBE(0x8ECC5A)};
  const Farbe h[] = {FARBE(0xF2C94E), FARBE(0x8E5A2A), FARBE(0x3A2A20)};
  l.kleid = k[i]; l.haar = h[i]; l.frisur = i; l.schuerze = WEISS;
  return l;
}
static FrauLook prinzLook() {
  FrauLook l; l.kleid = FARBE(0xB28DFF); l.haar = FARBE(0xF2C14E); l.krone = true; l.glanz = true; l.rockBreite = 1.2f;
  return l;
}
static FrauLook mutterLook() { FrauLook l; l.kleid = FARBE(0x6E5A8E); l.schuerze = WEISS; l.haar = FARBE(0x9A9A9A); l.frisur = 2; return l; }
static const FrauLook T0 = tLook(0), T1 = tLook(1), T2 = tLook(2), PRINZESSIN = prinzLook(), MUTTER = mutterLook();

// =====================================================================
// Figuren und Dinge
// =====================================================================
static void mann(Gfx &g, const Aussehen &a, float x, float y, float s, float dir, float t, float mund, float armV = 0.4f,
                 float lauf = -1, float armH = 0.2f, bool zu = false) {
  Pose p = posis(a, t);
  p.mund = mund;
  p.armV = armV;
  p.armH = armH;
  p.lauf = lauf;
  p.zu = zu || p.zu;
  schneiderlein(g, x, y, s, dir, p);
}
static void maedchen(Gfx &g, const FrauLook &l, float x, float y, float s, float dir, float t, float mund, float armV = 0.4f,
                     float lauf = -1, float armH = 0.2f) {
  FrauPose p = fposis(l, t);
  p.mund = mund;
  p.armV = armV;
  p.armH = armH;
  p.lauf = lauf;
  frau(g, x, y, s, dir, p);
}
// das alte graue Maennlein
static void maennlein(Gfx &g, float x, float y, float s, float dir, float t, float mund, float armV = 0.6f) {
  auto PX = [&](float d) { return x + d * s * dir; };
  auto PY = [&](float d) { return y + d * s; };
  const Farbe grau = FARBE(0x9A9AA8);
  g.kontur(1.2f * s, OUT);
  float m[] = {PX(-14), PY(0), PX(14), PY(0), PX(8), PY(-40), PX(-8), PY(-40)};
  g.polygon(m, 4, grau);
  g.linie(PX(6), PY(-34), PX(6) + sinf(armV) * 14 * s * dir, PY(-34) + cosf(armV) * 14 * s, 5 * s, grau);
  g.kreis(PX(2), PY(-48), 10 * s, HAUT);
  g.konturAus();
  float b[] = {PX(-6), PY(-46), PX(10), PY(-46), PX(4), PY(-26)};
  g.polygon(b, 3, FARBE(0xE8E8E8));
  float h[] = {PX(-12), PY(-52), PX(14), PY(-52), PX(2), PY(-74)};
  g.polygon(h, 3, FARBE(0x6E6E7E));
  g.kreis(PX(6), PY(-50), 1.4f * s, SCHWARZ);
  if (mund > 0.05f) g.ellipse(PX(6), PY(-42), 2.4f * s, (0.8f + mund * 2) * s, FARBE(0x7A2A2A));
  g.linie(PX(-12), PY(-30), PX(-16), PY(0), 2 * s, HOLZ_D);
}
static void gans(Gfx &g, float x, float y, float s, float dir, float t, float lauf = -1) {
  auto PX = [&](float d) { return x + d * s * dir; };
  auto PY = [&](float d) { return y + d * s; };
  float w = lauf >= 0 ? fabsf(sinf(lauf * 6.28f)) * 1.5f : 0;
  for (int k = 0; k < 2; k++) g.linie(PX(-2 + k * 5), PY(-6), PX(-2 + k * 5 + (lauf >= 0 ? sinf(lauf * 6.28f + k * 3) * 3 : 0)), PY(0), 2 * s, FARBE(0xF28A2A));
  g.kontur(1.1f * s, FARBE(0x9A6A1E));
  g.ellipse(PX(0), PY(-12 - w), 14 * s, 8 * s, GOLD);
  g.konturAus();
  float sw[] = {PX(-12), PY(-14 - w), PX(-19), PY(-20 - w), PX(-14), PY(-9 - w)};
  g.polygon(sw, 3, FARBE(0xFFE070));
  g.linie(PX(9), PY(-15 - w), PX(12), PY(-30 - w), 4.5f * s, GOLD);
  g.kontur(1, FARBE(0x9A6A1E));
  g.kreis(PX(12), PY(-32 - w), 4.5f * s, GOLD);
  g.konturAus();
  float sn[] = {PX(15), PY(-33 - w), PX(22), PY(-31 - w), PX(15), PY(-29 - w)};
  g.polygon(sn, 3, FARBE(0xF28A2A));
  g.kreis(PX(13), PY(-34 - w), 0.9f * s, SCHWARZ);
  for (int i = 0; i < 3; i++) g.stern(PX(-6 + i * 6), PY(-14 - w), 2.4f * s, 0.8f * s, 4, t * 2 + i, WEISS, (uint8_t)(150 + 100 * sinf(t * 3 + i)));
}
static void kuchen(Gfx &g, float x, float y, float s, bool fein) {
  g.kontur(1 * s, OUT);
  g.ellipse(x, y, 10 * s, 5 * s, fein ? FARBE(0xF2C060) : FARBE(0x8E8A86));
  g.konturAus();
  if (fein) g.ellipse(x - 2 * s, y - 1 * s, 5 * s, 2 * s, FARBE(0xFFE8A0));
}
static void krug(Gfx &g, float x, float y, float s, bool wein) {
  g.kontur(1 * s, OUT);
  g.rundRechteck(x - 5 * s, y - 14 * s, 10 * s, 14 * s, 3 * s, wein ? FARBE(0x8E2A4A) : FARBE(0xA89A7A));
  g.konturAus();
  g.bogen(x + 5 * s, y - 8 * s, 4 * s, -1.4f, 1.4f, 1.6f * s, OUT);
}
static void axt(Gfx &g, float x, float y, float s, float w) {
  float c = cosf(w), sn = sinf(w);
  g.linie(x, y, x + c * 24 * s, y - sn * 24 * s, 2.4f * s, HOLZ_D);
  float b[] = {x + c * 20 * s - sn * 2, y - sn * 20 * s - c * 2, x + c * 26 * s - sn * 8 * s, y - sn * 26 * s - c * 8 * s,
               x + c * 28 * s, y - sn * 28 * s};
  g.polygon(b, 3, STEIN);
}

static void wald(Gfx &g, float t, float baumWeg = 0) {
  himmel(g, 0, t, 200);
  sonne(g, 240, 32, 11, t);
  for (int i = 0; i < 6; i++) {
    float x = i * 56 - 20;
    if (i & 1) tanne(g, x, 204, 0.95f);
    else laubbaum(g, x, 204, 0.8f);
  }
  boden(g, 200);
  g.rechteck(0, 210, 284, 12, FARBE(0xC9B48A));
  if (baumWeg < 1) laubbaum(g, 200, 206, 1.1f - baumWeg * 0.2f, FARBE(0x3E9A4E));
}
static void dorf(Gfx &g, float t, float scroll = 0) {
  himmel(g, 0, t, 196);
  sonne(g, 240, 32, 11, t);
  for (int i = 0; i < 5; i++) {
    float x = fmodf(i * 90 - scroll * 0.6f + 900, 450) - 80;
    g.kontur(1.4f, OUT);
    g.rechteck(x, 130, 60, 70, i & 1 ? FARBE(0xF0DCC0) : FARBE(0xE8D0B0));
    float d[] = {x - 6, 132, x + 30, 100, x + 66, 132};
    g.polygon(d, 3, FARBE(0xC0503A));
    g.konturAus();
    g.rundRechteck(x + 22, 168, 16, 32, 4, HOLZ_D);
  }
  boden(g, 200);
  g.rechteck(0, 208, 284, 14, FARBE(0xC9B48A));
  blumen(g, scroll, 230, 6, 9);
}

// die lange Reihe: jede(r) haelt sich am Vordermann fest; n = wie viele schon kleben
static void reihe(Gfx &g, float x, float t, int n, float lauf, float s = 0.62f) {
  const float ab = 30 * s / 0.62f;
  gans(g, x + 18 * s / 0.62f, 196, 0.8f * s / 0.62f, 1, t, lauf);  // die Gans unterm Arm
  mann(g, DUMM, x, 222, s, 1, t, 0.4f, lauf, 1.3f);
  const FrauLook *tt[] = {&T0, &T1, &T2};
  for (int i = 0; i < n && i < 3; i++) maedchen(g, *tt[i], x - ab * (i + 1), 222, s * 0.95f, 1, t + i, 0.5f, 1.5f, lauf >= 0 ? fmodf(lauf + i * 0.2f, 1) : -1);
  const Aussehen *mm[] = {&PFARRER, &KUESTER, &BAUER1, &BAUER2};
  for (int i = 3; i < n && i < 7; i++) mann(g, *mm[i - 3], x - ab * (i + 1), 222, s, 1, t + i, 0.6f, 1.5f, lauf >= 0 ? fmodf(lauf + i * 0.2f, 1) : -1);
  for (int i = 5; i < n && i < 7; i++) g.linie(x - ab * (i + 1) - 10, 160, x - ab * (i + 1) - 4, 214, 2, HOLZ_D);  // Hacken
}

// =====================================================================
// Szenen
// =====================================================================
static void szeneFamilie(Gfx &g, float u) {
  dorf(g, u);
  mann(g, BR1, 100, 222, 0.9f, 1, u, u > 6.4f && u < 12 ? 0.8f : 0.5f, u > 6.4f && u < 12 ? 2.2f : 0.4f);
  mann(g, BR2, 150, 222, 0.88f, -1, u, u > 6.4f && u < 12 ? 0.8f : 0.5f, 0.4f);
  mann(g, DUMM, 210, 222, 0.82f, -1, u, 0.2f, 0.4f, -1, 0.2f, u > 6.4f && u < 12);
  g.kameraAus();
  vers(g, {"Ein Mann hatte drei Söhne. Den", "jüngsten nannten sie den Dummling."}, aufzu(u, 0.4f, 6));
  blase(g, 110, 50, {"Hahaha, der Dummling!"}, aufzu(u, 6.4f, 12), 110, 140);
  vers(g, {"Er wurde verspottet und bei jeder", "Gelegenheit zurückgesetzt."}, aufzu(u, 12.4f, 21.6f));
}

// 2) und 3) die beiden Brueder geizen - und treffen sich mit der Axt (nur angedeutet)
static void szeneBruder(Gfx &g, float u, int nr) {
  wald(g, u);
  const Aussehen &b = nr == 1 ? BR1 : BR2;
  maennlein(g, 170, 222, 1, -1, u, u > 4.4f && u < 9 ? 0.5f + 0.3f * sinf(u * 9) : 0, u > 4.4f && u < 9 ? 1.4f : 0.6f);
  bool hackt = u > 13 && u < 16;
  bool au = u > 15.4f;
  mann(g, b, 110, 222, 0.9f, 1, u, (u > 9.4f && u < 13) ? 0.6f : au ? 0.9f : 0, hackt ? 2.4f + sinf(u * 12) * 0.5f : au ? 1.2f : 0.4f, -1, au ? 1.6f : 0.2f, au);
  if (!au) { kuchen(g, 90, 192, 0.8f, true); krug(g, 82, 200, 0.8f, true); }
  if (hackt) axt(g, 124, 160, 1, 0.6f + sinf(u * 12) * 0.5f);
  if (au && u < 19) for (int i = 0; i < 4; i++) g.stern(110 + cosf(i * 1.57f + u * 3) * 16, 130 + sinf(i * 1.57f + u * 3) * 5, 3, 1.2f, 4, u * 6, GOLD);
  g.kameraAus();
  if (nr == 1) vers(g, {"Der Älteste ging mit Eierkuchen", "und Wein in den Wald, Holz hauen."}, aufzu(u, 0.4f, 4.2f));
  else vers(g, {"Dann ging der zweite Sohn - auch", "er bekam Kuchen und Wein mit."}, aufzu(u, 0.4f, 4.2f));
  blase(g, 190, 50, {"Gib mir ein Stück Kuchen und", "einen Schluck Wein, ich bin", "so hungrig und durstig!"}, aufzu(u, 4.4f, 9), 170, 140);
  blase(g, 110, 50, {"Gebe ich dir was, hab ich selber", "weniger - pack dich deiner Wege!"}, aufzu(u, 9.4f, 13), 110, 140);
  if (nr == 1) vers(g, {"Da fuhr ihm die Axt in den Arm.", "Das hatte das Männlein gemacht."}, aufzu(u, 15.4f, 23.6f));
  else vers(g, {"Auch ihn traf die Axt - ins Bein.", "Er musste nach Hause hinken."}, aufzu(u, 15.4f, 19.6f));
}

// 4) Der Dummling teilt Aschenkuchen und saures Bier
static void szeneTeilen(Gfx &g, float u) {
  if (u < 8) {
    dorf(g, u);
    mann(g, BR1, 220, 222, 0.85f, -1, u, u > 0.4f && u < 4 ? 0.5f : 0, 0.4f);
    maedchen(g, MUTTER, 170, 222, 0.85f, -1, u, u > 4.4f ? 0.4f : 0, u > 4.4f ? 1.6f : 0.4f);
    mann(g, DUMM, 110, 222, 0.85f, 1, u, 0.4f, 0.4f);
    if (u > 4.4f) { kuchen(g, 150, 160, 0.8f, false); krug(g, 158, 170, 0.8f, false); }
    g.kameraAus();
    blase(g, 220, 50, {"Geh nur hin - durch Schaden", "wirst du klug werden."}, aufzu(u, 0.4f, 4.2f), 220, 140);
    vers(g, {"Die Mutter gab ihm einen Aschen-", "kuchen und eine Flasche saures Bier."}, aufzu(u, 4.4f, 7.8f));
    return;
  }
  float v = u - 8;
  wald(g, u);
  bool fein = v > 11;
  maennlein(g, 176, 222, 1, -1, u, (v > 0.4f && v < 3.6f) ? 0.5f + 0.3f * sinf(u * 9) : fein ? 0.5f : 0.1f, fein ? 1.4f : 0.8f);
  mann(g, DUMM, 112, 222, 0.85f, 1, u, (v > 4 && v < 10) ? 0.5f : fein ? 0.7f : 0.2f, 1.2f);
  kuchen(g, 144, 214, 0.9f, fein);
  krug(g, 156, 220, 0.9f, fein);
  puff(g, 150, 200, seg(v, 10.6f, 12));
  g.kameraAus();
  blase(g, 190, 50, {"Gib mir ein Stück Kuchen", "und einen Schluck!"}, aufzu(v, 0.4f, 3.6f), 176, 140);
  blase(g, 110, 50, {"Ich hab nur Aschenkuchen und", "saures Bier. Ist dir das recht,", "so wollen wir zusammen essen."}, aufzu(v, 4, 10.4f), 112, 140);
  vers(g, {"Da war es feiner Eierkuchen", "und guter Wein geworden!"}, aufzu(v, 12, 17.6f));
}

// 5) Die goldene Gans unter der Baumwurzel
static void szeneGans(Gfx &g, float u) {
  float weg = seg(u, 9, 11);
  wald(g, u, weg);
  maennlein(g, 70, 222, 1, 1, u, u > 0.4f && u < 8 ? 0.5f + 0.3f * sinf(u * 9) : 0, 1.2f);
  bool hackt = u > 8 && u < 11;
  mann(g, DUMM, 160, 222, 0.85f, 1, u, u > 13 ? 0.8f : 0.3f, hackt ? 2.4f + sinf(u * 12) * 0.5f : u > 13 ? 2.4f : 0.4f);
  if (hackt) axt(g, 174, 160, 1, 0.6f + sinf(u * 12) * 0.5f);
  if (weg >= 1) {
    g.kontur(1.4f, OUT);
    g.ellipse(214, 206, 24, 8, HOLZ_D);
    g.konturAus();
    gans(g, 214, 202, 1, -1, u);
    glitzern(g, 214, 186, 30, u, 5);
  }
  g.kameraAus();
  blase(g, 90, 50, {"Weil du ein gutes Herz hast,", "will ich dir Glück bescheren.", "Hau den alten Baum dort ab!"}, aufzu(u, 0.4f, 8), 70, 140);
  vers(g, {"In den Wurzeln saß eine Gans", "mit Federn aus reinem Gold."}, aufzu(u, 12, 21.6f));
}

// 6) Im Wirtshaus: die drei Toechter bleiben kleben
static void szeneWirtshaus(Gfx &g, float u) {
  g.verlauf(0, 200, FARBE(0xD8B88A), FARBE(0xB8986A));
  g.rechteck(0, 200, 284, 40, FARBE(0x8E6A46));
  g.kontur(1.4f, OUT);
  g.rechteck(20, 150, 70, 8, HOLZ);
  g.konturAus();
  g.rechteck(28, 158, 5, 44, HOLZ_D);
  g.rechteck(78, 158, 5, 44, HOLZ_D);
  for (int i = 0; i < 3; i++) krug(g, 36 + i * 18, 150, 1, i == 1);
  g.ellipse(50, 60, 26, 20, FARBE(0xFFE8A0), 80);
  gans(g, 120, 222, 1, 1, u);
  mann(g, DUMM, 60, 222, 0.8f, 1, u, 0, 0.4f, -1, 0.2f, true);
  zzz(g, 70, 140, u);
  // eine nach der anderen
  float k1 = seg(u, 3, 6), k2 = seg(u, 10, 13), k3 = seg(u, 17, 20);
  maedchen(g, T0, lerp(240, 150, k1), 222, 0.8f, -1, u, k1 >= 1 ? 0.8f : 0.2f, k1 >= 1 ? 1.6f : 0.4f, k1 > 0 && k1 < 1 ? fmodf(u * 1.4f, 1) : -1);
  if (u > 8) maedchen(g, T1, lerp(260, 178, k2), 222, 0.8f, -1, u, k2 >= 1 ? 0.8f : 0.2f, k2 >= 1 ? 1.6f : 0.4f, k2 > 0 && k2 < 1 ? fmodf(u * 1.4f, 1) : -1);
  if (u > 15) maedchen(g, T2, lerp(280, 206, k3), 222, 0.8f, -1, u, k3 >= 1 ? 0.8f : 0.2f, k3 >= 1 ? 1.6f : 0.4f, k3 > 0 && k3 < 1 ? fmodf(u * 1.4f, 1) : -1);
  g.kameraAus();
  vers(g, {"Im Wirtshaus wollte die älteste", "Tochter eine goldene Feder..."}, aufzu(u, 0.4f, 3));
  blase(g, 200, 60, {"Ich kann nicht mehr los!"}, aufzu(u, 6, 9.6f), 150, 140);
  blase(g, 220, 60, {"Ich will auch eine Feder -", "oh! Ich klebe fest!"}, aufzu(u, 13, 16.6f), 178, 140);
  blase(g, 200, 60, {"Ich auch! Ich auch!"}, aufzu(u, 20, 25.6f), 206, 140);
}

// 7) Der Zug: Pfarrer, Kuester und zwei Bauern bleiben auch haengen
static void szeneZug(Gfx &g, float u) {
  dorf(g, u, u * 30);
  int n = 3 + (u > 6 ? 1 : 0) + (u > 13 ? 1 : 0) + (u > 20 ? 2 : 0);
  reihe(g, 250, u, n, fmodf(u * 1.2f, 1), 0.55f);
  g.kameraAus();
  vers(g, {"Der Dummling nahm die Gans unter", "den Arm - die Mädchen mussten mit."}, aufzu(u, 0.4f, 5.8f));
  blase(g, 90, 50, {"Schämt euch, ihr Mädchen!", "Lauft doch dem Burschen nicht nach!"}, aufzu(u, 6, 12.6f), 60, 160);
  blase(g, 90, 50, {"Herr Pfarrer, wohin so geschwind?", "Wir haben heut noch eine Kindtaufe!"}, aufzu(u, 13, 19.6f), 50, 160);
  vers(g, {"Auch zwei Bauern mit Hacken blieben", "hängen. Nun waren es sieben!"}, aufzu(u, 20, 27.6f));
}

// 8) Die Koenigstochter, die nie lachte
static void szeneLachen(Gfx &g, float u) {
  himmel(g, 0, u, 200);
  sonne(g, 40, 32, 10, u);
  schloss(g, 200, 200, u, 0);
  boden(g, 200);
  g.rechteck(0, 208, 284, 14, FARBE(0xC9B48A));
  float k = seg(u, 4, 12);
  if (u > 4) reihe(g, lerp(-60, 150, k), u, 7, k < 1 ? fmodf(u * 1.2f, 1) : fmodf(u * 2, 1) * 0.3f, 0.45f);
  bool lacht = u > 13;
  maedchen(g, PRINZESSIN, 230, 222, 0.8f, -1, u, lacht ? 0.7f + 0.3f * sinf(u * 14) : 0, lacht ? 1.2f + sinf(u * 10) * 0.3f : 0.4f);
  if (lacht) for (int i = 0; i < 3; i++) g.text(SCHRIFT_K, "ha", 250 + i * 8, 136 - fmodf(u * 30 + i * 12, 36), FARBE(0xB28DFF), 1, 200);
  g.kameraAus();
  vers(g, {"Der König hatte eine Tochter, die", "so ernst war, dass niemand sie", "zum Lachen bringen konnte."}, aufzu(u, 0.4f, 8));
  vers(g, {"Als sie die sieben sah, die hinter-", "einander herliefen, fing sie an zu", "lachen und wollte nicht aufhören."}, aufzu(u, 14, 23.6f));
}

// 9) Der Weinkeller und der Durstige
static void szeneKeller(Gfx &g, float u) {
  if (u < 8) {
    himmel(g, 0, u, 200);
    schloss(g, 200, 200, u, 0);
    boden(g, 200);
    koenig(g, 210, 222, 0.85f, -1, u > 0.4f && u < 7 ? 1 : 0);
    mann(g, DUMM, 110, 222, 0.85f, 1, u, 0.3f);
    g.kameraAus();
    blase(g, 190, 50, {"Erst bring mir einen Mann, der", "einen ganzen Keller voll Wein", "austrinken kann!"}, aufzu(u, 0.4f, 7.6f), 210, 140);
    return;
  }
  float v = u - 8;
  g.verlauf(0, 210, FARBE(0x6E6258), FARBE(0x4E443C));
  g.rechteck(0, 210, 284, 30, FARBE(0x3E3630));
  float leer = seg(v, 6, 14);
  for (int i = 0; i < 5; i++) {
    float x = 30 + i * 52;
    g.kontur(1.6f, OUT);
    g.ellipse(x, 186, 24, 22, HOLZ);
    g.konturAus();
    g.bogen(x, 186, 16, 4.4f, 5.0f, 2.4f, HOLZ_D);
    if (i < (int)(leer * 5)) g.ellipse(x, 208, 6, 2, FARBE(0x8E2A4A), 80);
  }
  Pose d = posis(BAUER1, u);
  d.armV = 2.6f;
  d.mund = 0.6f;
  schneiderlein(g, 142, 222, 0.9f, 1, d);
  krug(g, 156, 150, 1.4f, true);
  mann(g, DUMM, 70, 222, 0.82f, 1, u, v > 14 ? 0.7f : 0.3f, 0.4f);
  g.kameraAus();
  blase(g, 190, 60, {"Ich habe so großen Durst - ein", "Fass Wein ist für mich nur", "ein Tropfen auf den heißen Stein!"}, aufzu(v, 0.4f, 6), 142, 140);
  vers(g, {"Er trank und trank, bis alle Fässer", "leer waren - noch vor Abend."}, aufzu(v, 6.4f, 15.6f));
}

// 10) Der Brotberg und der Hungrige
static void szeneBrot(Gfx &g, float u) {
  himmel(g, 0, u, 200);
  sonne(g, 240, 32, 11, u);
  boden(g, 200);
  float weg = seg(u, 6, 16);
  float h = 110 * (1 - weg);
  if (h > 4) {
    float b[] = {90, 210, 160, 210 - h, 240, 210};
    g.kontur(1.4f, OUT);
    g.polygon(b, 3, FARBE(0xC88A4A));
    g.konturAus();
    for (int i = 0; i < 12; i++) {
      float x = 110 + zufall(i) * 110, y = 208 - zufall(i + 7) * h * 0.8f;
      if (y > 210 - h + fabsf(x - 160) * h / 75) g.ellipse(x, y, 7, 4, FARBE(0xE8B060));
    }
  }
  Pose d = posis(BAUER2, u);
  d.armV = 2 + sinf(u * 8) * 0.4f;
  d.mund = 0.5f + 0.4f * sinf(u * 10);
  schneiderlein(g, 70, 222, 0.9f, 1, d);
  mann(g, DUMM, 250, 222, 0.82f, -1, u, weg >= 1 ? 0.7f : 0.3f, 0.4f);
  g.kameraAus();
  vers(g, {"Dann verlangte der König einen", "Mann, der einen Berg Brot aufisst."}, aufzu(u, 0.4f, 5.6f));
  vers(g, {"Der Hungrige aß und aß -", "und in einem Tag war der", "ganze Berg verschwunden."}, aufzu(u, 6, 15.6f));
  blase(g, 90, 50, {"So, jetzt bin ich", "endlich satt!"}, aufzu(u, 16, 21.6f), 70, 140);
}

// 11) Das Schiff zu Land und zu Wasser
static void szeneSchiff(Gfx &g, float u) {
  himmel(g, 0, u, 200);
  sonne(g, 240, 32, 11, u);
  g.rechteck(0, 180, 284, 22, FARBE(0x3E8ED8));
  boden(g, 200);
  float k = seg(u, 8, 16);
  float sx = lerp(330, 150, k);
  // das Schiff auf Raedern
  g.kontur(1.6f, OUT);
  float r[] = {sx - 60, 170, sx + 60, 170, sx + 44, 200, sx - 44, 200};
  g.polygon(r, 4, HOLZ);
  g.konturAus();
  g.linie(sx, 170, sx, 80, 3, HOLZ_D);
  float se[] = {sx + 2, 84, sx + 2, 160, sx + 50, 150};
  g.polygon(se, 3, WEISS);
  for (int i = -1; i <= 1; i += 2) {
    g.bogen(sx + i * 30, 206, 10, 0, 6.29f, 3, HOLZ_D);
    for (int j = 0; j < 4; j++) { float a = u * 4 + j * 0.785f; g.linie(sx + i * 30, 206, sx + i * 30 + cosf(a) * 9, 206 + sinf(a) * 9, 1.4f, HOLZ_D); }
  }
  maennlein(g, 70, 222, 1, 1, u, u > 0.4f && u < 7.6f ? 0.5f + 0.3f * sinf(u * 9) : 0, 1.2f);
  if (k >= 1) mann(g, DUMM, sx + 10, 168, 0.6f, -1, u, 0.6f, 2.4f);
  g.kameraAus();
  blase(g, 90, 50, {"Weil du barmherzig gegen mich", "warst, schenk ich dir das Schiff,", "das zu Land und zu Wasser fährt."}, aufzu(u, 0.4f, 7.6f), 70, 140);
  vers(g, {"Als der König das Schiff sah,", "konnte er ihm seine Tochter", "nicht länger verweigern."}, aufzu(u, 16, 23.6f));
}

// 12) Hochzeit
static void szeneHochzeit(Gfx &g, float u) {
  himmel(g, 0, u, 200);
  schloss(g, 142, 200, u, 0);
  boden(g, 200);
  maedchen(g, PRINZESSIN, 156, 222, 0.88f, -1, u, 0.6f, 1.2f);
  mann(g, DUMM, 118, 222, 0.88f, 1, u, 0.6f, 1.2f);
  gans(g, 200, 222, 1, -1, u);
  koenig(g, 244, 222, 0.75f, -1, 0.5f + 0.5f * sinf(u * 3));
  herzen(g, 138, 150, u, 6, 28);
  for (int i = 0; i < 3; i++) feuerwerk(g, 50 + i * 90, 50 + (i & 1) * 16, fmodf(u * 0.4f + i * 0.33f, 1), i == 1 ? GOLD : FARBE(0xFF8FB0));
  g.kameraAus();
  vers(g, {"Da wurde Hochzeit gehalten. Nach", "des Königs Tod erbte der Dummling", "das Reich und lebte vergnügt."}, aufzu(u, 0.4f, 11.6f));
  endeBild(g, u, 14, 138, 170);
}

static void bild(Gfx &g, float t) {
  if (t < P2) szeneFamilie(g, t - P1);
  else if (t < P3) szeneBruder(g, t - P2, 1);
  else if (t < P4) szeneBruder(g, t - P3, 2);
  else if (t < P5) szeneTeilen(g, t - P4);
  else if (t < P6) szeneGans(g, t - P5);
  else if (t < P7) szeneWirtshaus(g, t - P6);
  else if (t < P8) szeneZug(g, t - P7);
  else if (t < P9) szeneLachen(g, t - P8);
  else if (t < P10) szeneKeller(g, t - P9);
  else if (t < P11) szeneBrot(g, t - P10);
  else if (t < P12) szeneSchiff(g, t - P11);
  else szeneHochzeit(g, t - P12);
}

// =====================================================================
// Musik
// =====================================================================
// Gaensemarsch (C-Dur, huepfend)
static const Ton3 MEL_GANS[] = {
  {72, .5, 48}, {72, .5, 0}, {79, .5, 0}, {79, .5, 0}, {81, .5, 53}, {79, .5, 0}, {76, 1, 0}, {77, .5, 53}, {77, .5, 0},
  {76, .5, 48}, {76, .5, 0}, {74, .5, 55}, {74, .5, 0}, {72, 1, 48}, {79, .5, 48}, {77, .5, 0}, {76, .5, 0}, {74, .5, 0},
  {72, .5, 53}, {74, .5, 0}, {76, .5, 55}, {74, .5, 0}, {72, 2, 48}};
static void schnatter(Synth &s, float x) { s.ton(x, 620, 0.14f, 0.01f, 0.18f, RECHTECK, GERAEUSCH, 520); s.ton(x + 0.25f, 640, 0.12f, 0.01f, 0.16f, RECHTECK, GERAEUSCH, 540); }
static void hack(Synth &s, float x) { s.rauschen(x, 0.08f, BANDPASS, 900, 0.3f); s.ton(x, 200, 0.2f, 0.002f, 0.1f, SINUS, GERAEUSCH, 120); }
static void ton(Synth &s, float t0, float t1) {
  float a = P1;
  spieleMelodie(s, M(MEL_GANS), a + 0.5f, a + 21.6f, 0.3f, t0, t1, 0.6f);
  const float B[] = {P2, P3};
  for (int k = 0; k < 2; k++) {
    a = B[k];
    spieleMelodie(s, M(MEL_WANDER), a + 0.4f, a + 9, 0.32f, t0, t1, 0.45f);
    for (int i = 0; i < 6; i++) if (bei(a + 13 + i * 0.5f, t0, t1)) hack(s, a + 13 + i * 0.5f);
    if (bei(a + 15.4f, t0, t1)) for (int j = 0; j < 3; j++) s.note(a + 15.4f + j * 0.15f, 64 - j * 3, 0.6f);
  }
  a = P4;
  spieleMelodie(s, M(MEL_GANS), a + 0.4f, a + 7.6f, 0.32f, t0, t1, 0.5f);
  if (bei(a + 18.6f, t0, t1)) for (int j = 0; j < 6; j++) s.note(a + 18.6f + j * 0.07f, 84 + j * 2, 0.45f);
  spieleMelodie(s, M(MEL_WIEGE), a + 19, a + 25.6f, 0.42f, t0, t1, 0.5f);
  a = P5;
  for (int i = 0; i < 6; i++) if (bei(a + 8 + i * 0.5f, t0, t1)) hack(s, a + 8 + i * 0.5f);
  if (bei(a + 11, t0, t1)) { for (int j = 0; j < 6; j++) s.note(a + 11 + j * 0.07f, 84 + j * 2, 0.5f); schnatter(s, a + 12); }
  spieleMelodie(s, M(MEL_GANS), a + 12.4f, a + 21.6f, 0.3f, t0, t1, 0.55f);
  a = P6;
  for (int i = 0; i < 3; i++) if (bei(a + 6 + i * 7, t0, t1)) { s.ton(a + 6 + i * 7, 900, 0.15f, 0.005f, 0.3f, DREIECK, GERAEUSCH, 1200); schnatter(s, a + 6.6f + i * 7); }
  spieleMelodie(s, M(MEL_KEGEL), a + 0.4f, a + 25.6f, 0.34f, t0, t1, 0.35f);
  a = P7;
  spieleMelodie(s, M(MEL_GANS), a + 0.4f, a + 27.6f, 0.26f, t0, t1, 0.6f);
  a = P8;
  spieleMelodie(s, M(MEL_GANS), a + 4, a + 23.6f, 0.24f, t0, t1, 0.6f);
  for (int i = 0; i < 10; i++) if (bei(a + 13 + i * 0.3f, t0, t1)) s.note(a + 13 + i * 0.3f, 84 + (i & 1) * 3, 0.3f);
  a = P9;
  spieleMelodie(s, M(MEL_RIESE), a + 0.4f, a + 7.6f, 0.4f, t0, t1, 0.45f);
  for (int i = 0; i < 14; i++) if (bei(a + 14 + i * 0.5f, t0, t1)) s.ton(a + 14 + i * 0.5f, 220, 0.12f, 0.02f, 0.2f, SINUS, GERAEUSCH, 260);  // gluck
  a = P10;
  for (int i = 0; i < 20; i++) if (bei(a + 6 + i * 0.45f, t0, t1)) s.rauschen(a + 6 + i * 0.45f, 0.08f, BANDPASS, 2400, 0.08f);  // mampf
  spieleMelodie(s, M(MEL_RIESE), a + 6, a + 21.6f, 0.38f, t0, t1, 0.4f);
  a = P11;
  spieleMelodie(s, M(MEL_WANDER), a + 0.4f, a + 23.6f, 0.32f, t0, t1, 0.55f);
  a = P12;
  for (int i = 0; i < 6; i++) if (bei(a + 0.4f + i * 1.2f, t0, t1)) s.glocke(a + 0.4f + i * 1.2f, i & 1 ? 67 : 72, 0.6f);
  spieleMelodie(s, M(MEL_F), a + 0.8f, a + 13.6f, 0.28f, t0, t1, 0.7f);
  endeKlang(s, a + 14.4f, t0, t1);
}

// =====================================================================
// Startbild, Vorschau, Erkennungszeichen
// =====================================================================
static void titelSzene(Gfx &g, float t) {
  dorf(g, t, t * 20);
  reihe(g, 230, t, 4, fmodf(t * 1.2f, 1), 0.6f);
}
static void titelbild(Gfx &g, float t) { titelSzene(g, t); titelRahmen(g, t, "Die goldene Gans", 196, SCHRIFT_M); }
static void vorschau(Gfx &g, float t) { titelSzene(g, t); }
static void symbol(Gfx &g, float x, float y, float t) {
  gans(g, x, y - 4, 2.2f, 1, t);
  glitzern(g, x, y - 36, 44, t, 5);
}

const Maerchen GOLDENE_GANS = {"Die goldene Gans", PDAUER, FARBE(0xFFD54A), bild, ton, titelbild, vorschau, symbol,
                               LIED(MEL_GANS, 0.3f), nullptr};

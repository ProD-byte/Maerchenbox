// Das tapfere Schneiderlein, Teil 2: Aufbruch, Riese, Baum, Nacht, Einhorn, Wildschwein, Hochzeit.
#include <initializer_list>
// Wird am Ende von schneiderlein.cpp eingebunden (nutzt dessen Figuren und Farben).

static const float T3 = 64, T4 = 100, T5 = 142, T6 = 170, T7 = 208, T8 = 234, T9 = 260;


// =====================================================================
// Szene 3: Aufbruch (36 s)
// =====================================================================
static void szeneAufbruch(Gfx &g, float u) {
  const float LOS = 10;  // ab hier wird gewandert
  float scroll = u > LOS ? (u - LOS) * 44 : 0;
  himmel(g, 0, u);
  sonne(g, 238, 38, 15, u);
  wolke(g, fmodf(40 - scroll * 0.1f + u * 3, 360) - 40, 40, 1);
  wolke(g, fmodf(200 - scroll * 0.1f + u * 2, 360) - 40, 62, 0.75f);
  // der Berg kommt naeher
  float bx = lerp(420, 230, seg(u, LOS, 34)), bs = lerp(0.8f, 1.35f, seg(u, LOS, 34));
  float berg[] = {bx - 110 * bs, 190, bx, 190 - 130 * bs, bx + 110 * bs, 190};
  g.kontur(1.6f, OUT);
  g.polygon(berg, 3, FARBE(0x9C8FB8));
  g.konturAus();
  float gipfel[] = {bx - 30 * bs, 190 - 98 * bs, bx, 190 - 130 * bs, bx + 30 * bs, 190 - 98 * bs, bx + 12 * bs, 190 - 104 * bs,
                    bx, 190 - 96 * bs, bx - 12 * bs, 190 - 104 * bs};
  g.polygon(gipfel, 6, WEISS);
  huegel(g, scroll, 0.3f, 186, 34, 150, FARBE(0xA7DE85));
  huegel(g, scroll, 0.6f, 200, 30, 110, FARBE(0x8FD46A));
  boden(g, 200);
  // Weg
  g.rechteck(0, 210, 284, 12, FARBE(0xE8D3A2));
  for (int i = 0; i < 8; i++) g.kreis(fmodf(i * 47 - scroll + 4000, 376) - 20, 216, 2, FARBE(0xCBB27C));
  // Haus (bleibt zurueck)
  float hx = 20 - scroll;
  if (hx > -120) {
    g.kontur(1.8f, OUT);
    g.rechteck(hx, 120, 80, 80, FARBE(0xFFF1D6));
    float dach[] = {hx - 10, 122, hx + 40, 80, hx + 90, 122};
    g.polygon(dach, 3, FARBE(0xE0533F));
    g.rundRechteck(hx + 28, 160, 24, 40, 10, HOLZ);
    g.konturAus();
    g.rundRechteck(hx + 8, 134, 14, 14, 2, FARBE(0x8FD3FF));
    g.rundRechteck(hx + 58, 134, 14, 14, 2, FARBE(0x8FD3FF));
    // Bank mit Kaese
    g.rechteck(hx + 92, 188, 30, 5, HOLZ);
    g.rechteck(hx + 95, 193, 4, 8, HOLZ_D);
    g.rechteck(hx + 115, 193, 4, 8, HOLZ_D);
    if (u < 7) kaese(g, hx + 107, 188, 1);
    // Busch mit Vogel
    g.kontur(1.4f, OUT);
    g.kreis(hx + 190, 192, 16, FARBE(0x4CB85A));
    g.kreis(hx + 206, 194, 13, FARBE(0x4CB85A));
    g.konturAus();
  }
  // Baeume und Blumen am Weg
  for (int i = 0; i < 3; i++) {
    float tx = fmodf(330 + i * 160 - scroll + 4800, 480) - 60;
    if (u > LOS || tx > 240) laubbaum(g, tx, 204, 0.8f + (i & 1) * 0.15f);
  }
  blumen(g, scroll, 200, 14, 7);

  // Vogel: sitzt im Busch, huepft in die Hand
  Pose p;
  p.guertel = true;
  float sx = 150;
  float dir = -1;
  if (u < 3.5f) {  // winkt
    p.armV = 2.6f + sinf(u * 10) * 0.4f;
    p.mund = 0.6f;
    sx = 170;
  } else if (u < 7) {  // geht zur Bank, nimmt Kaese
    sx = lerp(170, 140, seg(u, 3.5f, 5.5f));
    p.lauf = u < 5.5f ? fmodf(u * 1.6f, 1) : -1;
    if (u > 5.5f) p.armV = 1.4f;
  } else if (u < LOS) {  // Vogel huepft heran
    sx = 150; dir = 1;
    p.armV = 1.3f;
    p.mund = 0.5f;
  } else {
    sx = 120; dir = 1;
    p.lauf = fmodf(u * 1.7f, 1);
    p.armV = 0.4f + sinf(u * 10.7f) * 0.5f;
    p.armH = 0.4f - sinf(u * 10.7f) * 0.5f;
    p.ranzen = true;
  }
  if (u >= LOS - 1.2f) p.ranzen = true;
  p.zu = fmodf(u + 0.7f, 3.9f) < 0.12f;
  float hop = p.lauf >= 0 ? fabsf(sinf(p.lauf * 6.2831853f)) * 3 : 0;
  float hx2, hy2;
  schneiderlein(g, u < 3.5f ? sx : u < LOS ? sx : sx, 214 - hop, 1.05f, dir, p, &hx2, &hy2);
  if (u > 5.6f && u < LOS - 1.2f) kaese(g, hx2 - 3, hy2 + 4, 0.7f);
  if (u < LOS - 1.2f) {
    float vx, vy;
    if (u < 7) { vx = hx + 198; vy = 178 - huepf(fmodf(u, 0.6f) / 0.6f) * 6; }
    else {
      float k = seg(u, 7, 8.3f);
      vx = lerp(hx + 198, hx2 + 4, k);
      vy = lerp(178, hy2 - 6, k) - sinf(k * 3.14159f) * 24;
    }
    vogel(g, vx, vy, 1, u, -1);
  } else if (u < LOS + 0.6f) {
    // verschwindet im Beutel
    float k = seg(u, LOS - 1.2f, LOS + 0.6f);
    g.stern(sx - 13, 170, 6 * (1 - k) + 1, 2, 5, u * 3, GOLD, (uint8_t)(255 * (1 - k)));
  }
  // Schmetterling unterwegs
  if (u > LOS) {
    float bxs = 180 + sinf(u * 0.9f) * 50, bys = 140 + sinf(u * 2.3f) * 20;
    float fl = fabsf(sinf(u * 14));
    g.ellipse(bxs - 4, bys, 5 * fl + 1, 6, FARBE(0xFF7FA8), 230);
    g.ellipse(bxs + 4, bys, 5 * fl + 1, 6, FARBE(0xFF7FA8), 230);
    g.ellipse(bxs, bys, 1.5f, 5, OUT);
  }
  blende(g, u, 36, 150, 160);
}

// =====================================================================
// Szene 4: Der Riese auf dem Berg (42 s)
// =====================================================================
static void szeneRiese(Gfx &g, float u) {
  himmel(g, 0, u, 200);
  wolke(g, 20 + u * 2, 30, 1.1f);
  wolke(g, 160 + u * 1.5f, 50, 0.8f);
  // ferne Gipfel
  float b1[] = {-20, 200, 50, 120, 120, 200};
  float b2[] = {80, 200, 160, 100, 240, 200};
  float b3[] = {190, 200, 260, 130, 320, 200};
  g.polygon(b1, 3, FARBE(0xB9B0D6));
  g.polygon(b2, 3, FARBE(0xA79CC9));
  g.polygon(b3, 3, FARBE(0xB9B0D6));
  // Felsplateau
  g.kontur(1.8f, OUT);
  g.rundRechteck(-20, 196, 324, 70, 14, FARBE(0xB7AC9A));
  g.konturAus();
  g.rechteck(-20, 196, 324, 4, FARBE(0xCFC6B6));
  for (int i = 0; i < 6; i++) g.ellipse(20 + i * 50, 222 + (i & 1) * 8, 12, 4, FARBE(0x9E937F));
  // Riese sitzt auf einem Felsen
  g.kontur(1.8f, OUT);
  g.ellipse(214, 202, 46, 18, STEIN_D);
  g.konturAus();

  RiesePose r;
  r.sitzt = true;
  r.armV = 0.5f; r.armH = 0.3f;
  float kleinX = lerp(-30, 70, sanft(seg(u, 0.5f, 4)));
  // Ablauf
  bool steinInHand = u > 8 && u < 21.5f;
  if (u > 4 && u < 8) r.kopf = -0.25f;                       // schaut herunter
  if (u > 5.5f && u < 8) r.staunt = 0.6f;                    // liest den Guertel
  if (u > 8 && u < 13) { r.armV = lerp(0.5f, 1.9f, seg(u, 8, 9)); r.mund = u > 10 ? 0.3f : 0; }  // drueckt Stein
  if (u > 13 && u < 19) { r.staunt = 1; r.mund = 0.9f; }     // Kaese-Wasser!
  if (u > 19 && u < 21.5f) r.armV = lerp(1.9f, 3.0f, seg(u, 19, 20.6f));  // holt aus und wirft
  if (u > 21.5f && u < 26) { r.armV = 2.4f; r.kopf = 0.3f; r.mund = 0.4f; }  // schaut dem Stein nach
  if (u > 28 && u < 36) { r.kopf = 0.35f + sinf(u * 1.5f) * 0.1f; r.staunt = 0.8f; }  // wartet auf den Vogel
  if (u > 36) { r.armV = 2.7f + sinf(u * 6) * 0.2f; r.kopf = 0; }  // kratzt sich am Kopf
  r.zu = fmodf(u + 1.1f, 4.3f) < 0.14f;
  float rhx, rhy;
  riese(g, 214, 206, 1, -1, r, &rhx, &rhy);
  // Stein in der Hand des Riesen / Wassertropfen
  if (steinInHand) steinbrocken(g, rhx - 6, rhy - 6, 9);
  if (u > 10 && u < 13)
    for (int i = 0; i < 3; i++) {
      float k = fmodf((u - 10) * 1.3f + i * 0.33f, 1);
      tropfen(g, rhx - 6, rhy + 6 + k * 40, 2.4f, FARBE(0x6EC6FF), (uint8_t)(255 * (1 - k)));
    }
  // Steinwurf: steigt aus dem Bild und faellt spaeter zurueck
  if (u > 20.6f && u < 22.5f) steinbrocken(g, lerp(rhx, 200, seg(u, 20.6f, 22.5f)), lerp(rhy - 8, -30, hinaus(seg(u, 20.6f, 22.5f))), 9);
  if (u > 24.2f && u < 25.2f) steinbrocken(g, 120, lerp(-20, 205, hinein(seg(u, 24.2f, 25.2f))), 9);
  if (u > 25.2f) steinbrocken(g, 120, 205, 9);
  if (u > 25.2f && u < 25.7f) g.wackeln(sinf(u * 90) * 4, cosf(u * 80) * 3);
  if (u > 25.2f && u < 26.4f) staubwolke(g, 120, 205, 0.6f * seg(u, 25.2f, 25.6f), u, (uint8_t)(220 * (1 - seg(u, 25.6f, 26.4f))));

  // Schneiderlein
  Pose p;
  p.guertel = true;
  p.ranzen = true;
  p.lauf = u < 4 ? fmodf(u * 1.7f, 1) : -1;
  p.zu = fmodf(u + 0.4f, 3.7f) < 0.12f;
  if (u > 4.5f && u < 8) { p.armV = 0.3f; p.armH = 2.4f; p.mund = 0.4f; }  // zeigt auf den Guertel
  bool kaeseInHand = u > 13 && u < 19;
  if (kaeseInHand) p.armV = lerp(1.0f, 2.0f, seg(u, 13, 14));
  if (u > 26.5f && u < 29) p.armV = lerp(1.2f, 3.0f, seg(u, 26.5f, 27.3f));  // laesst den Vogel fliegen
  if (u > 29) { p.armV = 1.1f; p.armH = 1.1f; p.mund = 0.2f; }  // Arme verschraenkt, grinst
  float hx, hy;
  schneiderlein(g, kleinX, 212, 0.95f, 1, p, &hx, &hy);
  if (kaeseInHand) {
    kaese(g, hx + 2, hy + 3, 0.7f);
    if (u > 14.2f)
      for (int i = 0; i < 8; i++) {  // spritzt viel mehr als beim Riesen
        float k = fmodf((u - 14.2f) * 1.6f + i * 0.125f, 1);
        float vx = hx + cosf(i * 0.8f) * 20 * k, vy = hy + k * 50 - sinf(k * 3.14f) * 10;
        tropfen(g, vx, vy, 2.6f, FARBE(0x8FD8FF), (uint8_t)(255 * (1 - k)));
      }
  }
  // Vogel fliegt davon und kommt nie zurueck
  if (u > 27 && u < 34) {
    float k = seg(u, 27, 34);
    vogel(g, lerp(hx, 300, k) + sinf(k * 20) * 6, lerp(hy - 6, -10, sanft(k)), lerp(1, 0.4f, k), u, 1);
  }
  // Riese: Fragezeichen
  if (u > 6 && u < 8) fragezeichen(g, 250, 40, 1, (uint8_t)(255 * seg(u, 6, 6.4f)));
  if (u > 30 && u < 37) {
    for (int i = 0; i < 3; i++) {
      float k = seg(u, 30 + i * 1.2f, 31 + i * 1.2f);
      if (k > 0) fragezeichen(g, 250 + i * 12, 46 - i * 14, 0.8f + i * 0.15f, (uint8_t)(255 * k));
    }
  }
  // Schweisstropfen beim Staunen
  if (u > 15 && u < 19) tropfen(g, 236, 70 + fmodf(u * 30, 20), 3, FARBE(0x8FD8FF));
  blende(g, u, 42, 150, 140);
}

// =====================================================================
// Szene 5: Der Baum (28 s)
// =====================================================================
static void szeneBaum(Gfx &g, float u) {
  float scroll = u < 20 ? u * 30 : 600;
  himmel(g, 0, u);
  sonne(g, 40, 40, 14, u);
  huegel(g, scroll, 0.25f, 180, 40, 140, FARBE(0xA7DE85));
  for (int i = 0; i < 4; i++) {
    float tx = fmodf(i * 110 - scroll * 0.55f + 4400, 440) - 50;
    tanne(g, tx, 196, 0.9f + (i & 1) * 0.2f, FARBE(0x3FA35E));
  }
  boden(g, 196);
  blumen(g, scroll, 200, 12, 31);
  // Riese traegt den Stamm auf der Schulter (Krone hinten links)
  RiesePose r;
  bool traegt = u < 20;
  r.lauf = traegt ? fmodf(u * 0.9f, 1) : -1;
  r.armV = traegt ? 2.6f : 0.6f;
  r.armH = traegt ? 2.6f : 0.4f;
  r.mund = traegt ? 0.6f : 0.9f;
  r.boese = traegt ? 0.3f : 0;
  r.staunt = u > 21 ? 0.8f : 0;
  r.kopf = traegt ? 0.15f : 0;
  float rx = 196, ry = 214 - (traegt ? fabsf(sinf(r.lauf * 6.2831853f)) * 4 : 0);
  // Stamm
  float stY = traegt ? ry - 118 : 208;
  float st0 = rx + 28, st1 = rx - 128;
  float kipp = traegt ? 0.06f : 0;
  g.kontur(1.8f, OUT);
  g.linie(st0, stY - 8 + kipp * 0, st1, stY + 10 + (traegt ? 0 : 0), 16, FARBE(0x8E5B3A));
  g.konturAus();
  // Krone hinten
  float kx = st1 - 4, ky = stY + 26;
  g.kontur(1.6f, OUT);
  g.kreis(kx - 14, ky - 10, 22, FARBE(0x4CB85A));
  g.kreis(kx + 14, ky - 16, 20, FARBE(0x4CB85A));
  g.kreis(kx, ky + 12, 20, FARBE(0x4CB85A));
  g.konturAus();
  // Schneiderlein sitzt in den Aesten und pfeift
  Pose p;
  p.guertel = true;
  p.ranzen = true;
  p.zu = traegt ? fmodf(u, 2.2f) < 1.1f : false;  // genießt es
  p.armV = 1.2f; p.armH = 0.8f;
  p.mund = 0.2f;
  float px = kx + 6, py = ky - 4;
  if (u > 20) {  // springt herunter und tut so, als truege es
    float k = seg(u, 20, 20.8f);
    px = lerp(kx + 6, kx + 30, k);
    py = lerp(ky - 4, 214, k) - sinf(k * 3.14159f) * 30;
    p.zu = false;
    if (u > 20.8f) { p.armV = 2.7f; p.armH = 2.7f; p.mund = 0.5f; }
  }
  schneiderlein(g, px, py, 0.85f, 1, p);
  if (traegt)
    for (int i = 0; i < 3; i++) {
      float ph = fmodf(u * 0.5f + i / 3.0f, 1);
      notenzeichen(g, px + 10 + ph * 26, py - 70 - ph * 40, i & 1 ? FARBE(0x3E7FDC) : FARBE(0xE8473D), (uint8_t)(230 * huepf(ph)));
    }
  riese(g, rx, ry, 0.95f, 1, r);
  // Schweiss
  if (traegt && u > 6)
    for (int i = 0; i < 2; i++) {
      float k = fmodf(u * 0.8f + i * 0.5f, 1);
      tropfen(g, rx - 18 + i * 44, ry - 160 + k * 30, 3, FARBE(0x8FD8FF), (uint8_t)(255 * (1 - k)));
    }
  if (u > 20 && u < 20.5f) g.wackeln(sinf(u * 90) * 5, cosf(u * 80) * 4);
  if (u > 20 && u < 21.4f) staubwolke(g, rx - 90, 210, seg(u, 20, 20.4f), u, (uint8_t)(220 * (1 - seg(u, 20.5f, 21.4f))));
  blende(g, u, 28, 140, 140);
}

// =====================================================================
// Szene 6: Nacht im Wald (38 s)
// =====================================================================
static void szeneNacht(Gfx &g, float u) {
  float morgen = seg(u, 33, 37);
  float nacht = 1 - morgen * 0.85f;
  himmel(g, nacht, u, 200);
  if (morgen < 0.9f) mond(g, 236, 40, 16);
  for (int i = 0; i < 5; i++) tanne(g, i * 64 - 10, 196, 1.1f, mischeFarbe(FARBE(0x3FA35E), FARBE(0x1F4B45), nacht));
  boden(g, 196, mischeFarbe(GRAS, FARBE(0x2F5E4A), nacht));
  // grosser Baum in der Mitte
  g.kontur(1.8f, OUT);
  g.rundRechteck(130, 60, 24, 140, 6, FARBE(0x6E4426));
  g.linie(142, 118, 190, 102, 10, FARBE(0x6E4426));
  Farbe kr = mischeFarbe(FARBE(0x4CB85A), FARBE(0x2B6B4A), nacht);
  g.kreis(110, 52, 34, kr);
  g.kreis(176, 50, 34, kr);
  g.kreis(142, 26, 36, kr);
  g.konturAus();

  // Gluehwuermchen
  for (int i = 0; i < 8; i++) {
    float fx = 30 + zufall(i + 700) * 230 + sinf(u * 0.7f + i) * 14, fy = 120 + zufall(i + 710) * 70 + cosf(u * 1.1f + i) * 10;
    g.kreis(fx, fy, 3.5f, FARBE(0xFFF27A), (uint8_t)(90 * nacht * (0.5f + 0.5f * sinf(u * 4 + i))));
    g.kreis(fx, fy, 1.5f, FARBE(0xFFFBC8), (uint8_t)(255 * nacht));
  }

  // Ablauf: Stein auf A (4), A schubst B (6), schlafen; Stein auf B (13), B schubst A; Stein auf A (19): Streit (22-30)
  float stoss1 = seg(u, 5.8f, 6.4f) * (1 - seg(u, 6.6f, 7.2f));
  float stoss2 = seg(u, 14.8f, 15.4f) * (1 - seg(u, 15.6f, 16.2f));
  bool streit = u > 21.5f && u < 30;
  bool flieht = u >= 30;
  RiesePose a, b;
  a.sitzt = b.sitzt = true;
  a.zu = b.zu = true;
  b.farbe = 1;
  a.armV = 0.6f; b.armV = 0.6f;
  if (u > 4.6f && u < 8.5f) { a.zu = false; a.boese = 0.8f; a.kopf = 0.2f; a.armV = 1.2f + stoss1 * 0.6f; }
  if (u > 6.4f && u < 8.5f) { b.zu = false; b.staunt = 0.7f; }
  if (u > 13.6f && u < 17.5f) { b.zu = false; b.boese = 0.8f; b.kopf = 0.2f; b.armV = 1.2f + stoss2 * 0.6f; }
  if (u > 15.4f && u < 17.5f) { a.zu = false; a.staunt = 0.7f; }
  if (u > 19.6f) { a.zu = false; a.boese = 1; b.zu = false; b.boese = 1; a.mund = 0.8f; b.mund = 0.8f; }
  float atmen = sinf(u * 1.6f) * 2;
  if (!streit && !flieht) {
    riese(g, 70 + stoss2 * -6, 212 + (a.zu ? atmen : 0), 0.8f, 1, a);
    riese(g, 216 + stoss1 * 6, 212 + (b.zu ? -atmen : 0), 0.8f, -1, b);
    if (a.zu) zzz(g, 76, 86, u, (uint8_t)(255 * nacht));
    if (b.zu) zzz(g, 214, 86, u + 0.9f, (uint8_t)(255 * nacht));
  } else if (streit) {
    // Raufen: grosse Staubwolke mit Armen und Sternen
    RiesePose ka = a, kb = b;
    ka.sitzt = kb.sitzt = false;
    ka.armV = 2.2f + sinf(u * 13) * 0.8f;
    kb.armV = 2.2f + cosf(u * 12) * 0.8f;
    riese(g, 110 + sinf(u * 7) * 6, 214, 0.75f, 1, ka);
    riese(g, 176 - sinf(u * 7) * 6, 214, 0.75f, -1, kb);
    staubwolke(g, 143, 150, 1.5f, u, 255);
    for (int i = 0; i < 5; i++) {
      float an = u * 5 + i * 1.26f;
      g.stern(143 + cosf(an) * 40, 130 + sinf(an) * 26, 7, 3, 5, an, GOLD);
    }
  } else {
    // fliehen in beide Richtungen
    float k = seg(u, 30, 33);
    RiesePose ka = a, kb = b;
    ka.sitzt = kb.sitzt = false;
    ka.lauf = fmodf(u * 2.5f, 1); kb.lauf = fmodf(u * 2.5f + 0.5f, 1);
    ka.boese = kb.boese = 0; ka.staunt = kb.staunt = 1;
    ka.armV = 2.6f; kb.armV = 2.6f;
    if (k < 1) {
      riese(g, lerp(110, -140, k), 214, 0.75f, -1, ka);
      riese(g, lerp(176, 430, k), 214, 0.75f, 1, kb);
      staubwolke(g, lerp(150, -60, k), 206, 0.6f, u, 160);
      staubwolke(g, lerp(140, 360, k), 206, 0.6f, u + 1, 160);
    }
  }
  // Schneiderlein auf dem Ast, wirft Steine
  Pose p;
  p.guertel = true;
  p.ranzen = true;
  p.zu = fmodf(u + 0.3f, 3.5f) < 0.12f;
  float wurf = (u > 3 && u < 4.6f) || (u > 12 && u < 13.6f) || (u > 18 && u < 19.6f);
  p.armV = wurf ? 1.6f : 0.9f;
  p.mund = streit ? 0.3f : 0;
  float px = 186, py = 104;
  if (u > 34) {  // klettert herunter, freut sich
    float k = seg(u, 34, 35.5f);
    px = lerp(186, 150, k);
    py = lerp(104, 214, sanft(k));
    p.armV = 2.7f; p.armH = 2.7f; p.mund = 0.6f;
  }
  schneiderlein(g, px, py, 0.8f, -1, p);
  // fallende Steine
  const float wuerfe[3][2] = {{3.6f, 76}, {12.6f, 214}, {18.6f, 76}};
  for (auto &w : wuerfe) {
    if (u > w[0] && u < w[0] + 1) {
      float k = seg(u, w[0], w[0] + 1);
      steinbrocken(g, lerp(180, w[1] + 4, k), lerp(92, 110, hinein(k)), w[0] > 18 ? 9 : 6);
    }
    if (u > w[0] + 1 && u < w[0] + 1.8f) {
      float k = seg(u, w[0] + 1, w[0] + 1.8f);
      g.stern(w[1] + 4, 104, 10 + k * 10, 4, 6, k * 4, GOLD, (uint8_t)(255 * (1 - k)));
    }
  }
  blende(g, u, 38, 150, 120);
}

// =====================================================================
// Szene 7: Das Einhorn (26 s)
// =====================================================================
static void szeneEinhorn(Gfx &g, float u) {
  himmel(g, 0, u);
  sonne(g, 240, 36, 13, u);
  huegel(g, 0, 0, 182, 40, 120, FARBE(0xA7DE85));
  tanne(g, 250, 196, 1.2f);
  tanne(g, 210, 196, 0.9f);
  boden(g, 196);
  blumen(g, 0, 202, 12, 55);
  // grosser Baum links
  laubbaum(g, 60, 206, 1.35f);
  float stamm = 60 + 8;
  // Einhorn
  float ex, lauf = fmodf(u * 2.2f, 1), senken = 0, zappel = 0;
  float dir = -1;
  bool steckt = u > 10.6f && u < 17;
  if (u < 4) { ex = lerp(330, 220, sanft(seg(u, 0, 4))); }           // kommt herangetrabt
  else if (u < 6.5f) { ex = 220; lauf = -1; senken = seg(u, 5, 6.2f); }  // scharrt, senkt den Kopf
  else if (u < 10.6f) { ex = lerp(220, stamm + 40, hinein(seg(u, 6.5f, 10.6f))); senken = 1; }  // Angriff!
  else if (steckt) { ex = stamm + 40; lauf = -1; senken = 1; zappel = u; }
  else { ex = lerp(stamm + 52, 400, seg(u, 19, 26)); lauf = u > 19 ? fmodf(u * 1.4f, 1) : -1; dir = 1; }
  if (u >= 17 && u < 19) { ex = stamm + 52; lauf = -1; dir = 1; }
  // Schneiderlein: steht vor dem Baum, springt im letzten Moment zur Seite
  Pose p;
  p.guertel = true;
  p.ranzen = true;
  float sx = stamm + 34, sy = 214;
  p.zu = fmodf(u + 0.5f, 3.3f) < 0.12f;
  if (u > 6.5f && u < 10.2f) { p.mund = 0.7f; p.armV = 2.4f; p.armH = 2.4f; }
  if (u > 10.2f) {
    float k = seg(u, 10.2f, 10.7f);
    sx = lerp(stamm + 34, stamm + 92, k);
    sy = 214 - sinf(k * 3.14159f) * 34;
  }
  if (steckt) { p.mund = 0.4f; p.armV = 1.5f; }
  if (u > 17) { sx = lerp(stamm + 92, 420, seg(u, 19, 26)); p.lauf = u > 19 ? fmodf(u * 1.4f, 1) : -1; p.armV = 1.5f; }
  if (u > 10.6f && u < 11) g.wackeln(sinf(u * 90) * 6, cosf(u * 80) * 4);
  einhorn(g, ex, 214, 1, lauf, dir, senken, zappel);
  float hx, hy;
  schneiderlein(g, sx + (u > 17 ? -30 : 0), sy, 0.9f, u > 10.2f && u < 17 ? -1 : 1, p, &hx, &hy);
  if (u > 10.6f && u < 11.5f) g.stern(stamm + 6, 160, 14, 6, 7, u * 3, GOLD, (uint8_t)(255 * (1 - seg(u, 10.6f, 11.5f))));
  // Seil vom Schneiderlein zum Einhorn (ab dem Festbinden)
  if (u > 13) g.kurve(hx, hy, (hx + ex + 30 * dir) / 2, 200, ex + 30 * dir, 160, 1.8f, FARBE(0xC98A45));
  // Herz: das Einhorn ist zahm
  if (u > 17 && u < 19.5f) herz(g, ex + 30, 120 - seg(u, 17, 19.5f) * 30, 7, FARBE(0xFF5A7A), (uint8_t)(255 * (1 - seg(u, 18.5f, 19.5f))));
  blende(g, u, 26, 120, 150);
}

// =====================================================================
// Szene 8: Das Wildschwein und die Kapelle (26 s)
// =====================================================================
static void szeneWildschwein(Gfx &g, float u) {
  himmel(g, 0, u);
  wolke(g, 30 + u * 3, 40, 0.9f);
  huegel(g, 0, 0, 186, 36, 130, FARBE(0xA7DE85));
  boden(g, 198);
  blumen(g, 0, 204, 10, 77);
  float kx = 180;
  float tuer = seg(u, 11.2f, 12);  // Tuer geht zu
  float wackel = u > 12.5f && u < 21 ? sinf(u * 40) * 2.5f * (0.5f + 0.5f * sinf(u * 3)) : 0;
  kapelle(g, kx, 206, tuer, wackel);
  // Schneiderlein rennt hinein (bis 7), springt aus dem Fenster (8.5-9.6), rennt zur Tuer (9.6-11.2)
  Pose p;
  p.guertel = true;
  p.ranzen = true;
  float sx = -20, sy = 214, dir = 1;
  if (u < 7) { sx = lerp(-20, kx, seg(u, 0, 7)); p.lauf = fmodf(u * 3, 1); p.mund = 0.8f; p.armV = 2.4f; }
  else if (u < 8.4f) sx = -999;  // in der Kapelle
  else if (u < 9.6f) {
    float k = seg(u, 8.4f, 9.6f);
    sx = lerp(kx + 30, kx + 70, k);
    sy = lerp(150, 214, k) - sinf(k * 3.14159f) * 24;
    p.armV = 2.8f; p.armH = 2.8f;
  } else if (u < 11.2f) {
    float k = seg(u, 9.6f, 11.2f);
    sx = lerp(kx + 70, kx + 18, k); dir = -1; p.lauf = fmodf(u * 3, 1);
  } else {
    sx = kx + 26; dir = 1;
    p.armV = u < 14 ? 1.6f : 2.6f + sinf(u * 8) * 0.3f;  // klopft sich die Haende ab, jubelt
    p.mund = u > 14 ? 0.6f : 0.2f;
    p.zu = u > 14 && fmodf(u, 0.8f) < 0.4f;
  }
  if (sx > -500) {
    float hop = p.lauf >= 0 ? fabsf(sinf(p.lauf * 6.2831853f)) * 4 : 0;
    schneiderlein(g, sx, sy - hop, 0.85f, dir, p);
  }
  // Wildschwein jagt hinterher und ist dann eingesperrt (schaut aus dem Fenster)
  if (u < 8.2f) {
    float wx = lerp(-110, kx - 6, seg(u, 0.3f, 8.2f));
    wildschwein(g, wx, 214, 1, fmodf(u * 3.4f, 1), 1);
    if (u > 1) staubwolke(g, wx - 40, 208, 0.45f, u, 150);
  }
  if (u > 12.5f && u < 21) {
    // Wildschwein-Kopf im Fenster, schnaubt
    g.ausschnitt(kx + 21 + wackel, 141, 18, 18);
    g.kreis(kx + 30 + wackel, 152, 9, FARBE(0x7A5236));
    g.ellipse(kx + 30 + wackel, 155, 5, 4, FARBE(0xE8A0A0));
    g.kreis(kx + 26 + wackel, 147, 1.6f, SCHWARZ);
    g.kreis(kx + 34 + wackel, 147, 1.6f, SCHWARZ);
    g.ausschnittAus();
    if (fmodf(u, 1.4f) < 0.5f) staubwolke(g, kx + 48, 140, 0.25f, u, 160);
  }
  blende(g, u, 26, 190, 160);
}

// =====================================================================
// Szene 9: Hochzeit (40 s)
// =====================================================================
static void szeneHochzeit(Gfx &g, float u) {
  float nacht = seg(u, 20, 25);
  himmel(g, nacht, u, 200);
  if (nacht < 0.5f) sonne(g, 240, lerp(36, 120, nacht * 2), 14, u);
  if (nacht > 0.5f) mond(g, 40, 36, 12);
  schloss(g, 142, 196, u, nacht);
  boden(g, 196, mischeFarbe(GRAS, FARBE(0x2F5E4A), nacht * 0.8f));
  // roter Teppich
  float tep[] = {120, 196, 164, 196, 200, 240, 84, 240};
  g.polygon(tep, 4, mischeFarbe(FARBE(0xD8283F), FARBE(0x7A2040), nacht * 0.6f));
  // Koenig und Prinzessin am Tor
  koenig(g, 64, 214, 0.9f, 1, u > 6 && u < 10 ? u : 0);
  float px = u < 9 ? 200 : lerp(200, 162, seg(u, 9, 11));
  prinzessin(g, px, 214, 0.9f, -1, u > 26 ? u : 0);
  // Schneiderlein kommt mit dem Einhorn
  Pose p;
  p.guertel = true;
  p.mund = 0.4f;
  float sx = lerp(330, 126, sanft(seg(u, 0, 6)));
  p.lauf = u < 6 ? fmodf(u * 1.7f, 1) : -1;
  p.zu = fmodf(u + 0.2f, 3.1f) < 0.12f;
  if (u > 11 && u < 20) { p.zu = true; p.mund = 0.8f; }  // Hochzeitskuss... und Freude
  if (u > 26) { p.armV = 2.6f + sinf(u * 9) * 0.3f; }
  {
    float ex = u < 6 ? sx + 110 : lerp(sx + 110, 246, seg(u, 6, 8));
    einhorn(g, ex, 214, 0.7f, u < 8 ? fmodf(u * 1.7f, 1) : -1, -1);
  }
  schneiderlein(g, sx, 214, 0.9f, -1, p);
  // Herzen
  if (u > 11 && u < 20)
    for (int i = 0; i < 6; i++) {
      float k = fmodf((u - 11) * 0.4f + i / 6.0f, 1);
      herz(g, 142 + sinf(k * 7 + i) * 26, 150 - k * 110, 5 + (i % 3), FARBE(0xFF5A7A), (uint8_t)(255 * huepf(k)));
    }
  // Blueten fallen
  if (u > 12 && u < 24)
    for (int i = 0; i < 20; i++) {
      float x = zufall(i + 900) * 284 + sinf(u * 2 + i) * 12;
      float y = fmodf(zufall(i + 950) * 240 + (u - 12) * 40, 250) - 10;
      g.ellipse(x, y, 3, 2, i & 1 ? FARBE(0xFF9FC8) : WEISS, 230, u + i);
    }
  // Feuerwerk
  if (nacht > 0.6f) {
    const Farbe fw[] = {GOLD, FARBE(0xFF6B8A), FARBE(0x6EC6FF), FARBE(0x7ED957), FARBE(0xB28DFF)};
    for (int i = 0; i < 6; i++) {
      float start = 24 + i * 1.3f;
      float k = fmodf(u - start, 4) / 2.2f;
      if (u > start) feuerwerk(g, 40 + zufall(i + 60) * 200 + fmodf(floorf((u - start) / 4), 3) * 20, 30 + zufall(i + 70) * 50, k, fw[i % 5]);
    }
  }
  // Ende
  g.kameraAus();
  if (u < 1.2f) g.iris(142, 140, lerp(0, 330, sanft(u / 1.2f)));
  if (u > 33) g.iris(142, 160, lerp(330, 0, sanft(seg(u, 33, 35.5f))));
  if (u > 35.5f) {
    uint8_t a = (uint8_t)(255 * sanft(seg(u, 35.5f, 36.5f)));
    g.text(SCHRIFT_G, "Ende", 142, 122, GOLD, 1, a);
    for (int i = 0; i < 6; i++) {
      float an = u * 1.2f + i * 1.047f;
      g.stern(142 + cosf(an) * 60, 112 + sinf(an) * 30, 5, 2, 5, an, GOLD, a);
    }
  }
}

static void teil2(Gfx &g, float t) {
  if (t < T4) szeneAufbruch(g, t - T3);
  else if (t < T5) szeneRiese(g, t - T4);
  else if (t < T6) szeneBaum(g, t - T5);
  else if (t < T7) szeneNacht(g, t - T6);
  else if (t < T8) szeneEinhorn(g, t - T7);
  else if (t < T9) szeneWildschwein(g, t - T8);
  else szeneHochzeit(g, t - T9);
}

// =====================================================================
// Musik ab dem Aufbruch
// =====================================================================

static void ton2(Synth &s, float t0, float t1) {
  // Aufbruch
  spieleMelodie(s, M(MEL_F), T3, T3 + 7, 0.24f, t0, t1, 0.9f);
  spieleMelodie(s, M(MEL_WANDER), T3 + 9.5f, T4 - 1.5f, 0.26f, t0, t1, 1);
  // Vogel piept (aus dem Busch, beim Hineinhuepfen)
  for (float x : {T3 + 4.0f, T3 + 5.5f, T3 + 7.2f, T3 + 8.0f})
    if (bei(x, t0, t1)) { s.note(x, 93, 0.5f); s.note(x + 0.08f, 96, 0.45f); }
  // Schritte beim Wandern: ganz leises Tappen
  for (float x = ceilf(t0 / 0.294f) * 0.294f; x < t1; x += 0.294f)
    if (x > T3 + 10 && x < T4 - 2) s.rauschen(x, 0.03f, TIEFPASS, 500, 0.06f, 0.7f);

  // Riese
  float r = T4;
  spieleMelodie(s, M(MEL_RIESE), r + 1, r + 20, 0.42f, t0, t1, 1);
  spieleMelodie(s, M(MEL_RIESE), r + 26.5f, r + 41, 0.36f, t0, t1, 0.9f);
  if (bei(r + 5.6f, t0, t1)) for (int k = 0; k < 4; k++) s.note(r + 5.6f + k * 0.12f, 72 + k * 4, 0.6f);   // liest den Guertel
  for (int i = 0; i < 3; i++) if (bei(r + 10.3f + i * 0.77f, t0, t1)) s.note(r + 10.3f + i * 0.77f, 84 - i * 2, 0.6f);  // Tropfen
  for (int i = 0; i < 10; i++) if (bei(r + 14.3f + i * 0.3f, t0, t1)) s.note(r + 14.3f + i * 0.3f, 88 + (i % 3) * 2, 0.5f);  // spritzt!
  if (bei(r + 15, t0, t1)) { s.glocke(r + 15, 79, 0.7f); s.glocke(r + 15.2f, 83, 0.6f); }
  if (bei(r + 20.6f, t0, t1)) for (int k = 0; k < 10; k++) s.note(r + 20.6f + k * 0.07f, 60 + k * 3, 0.5f);  // Wurf
  if (bei(r + 24.2f, t0, t1)) for (int k = 0; k < 8; k++) s.note(r + 24.2f + k * 0.12f, 90 - k * 3, 0.45f);  // faellt
  if (bei(r + 25.2f, t0, t1)) { s.ton(r + 25.2f, 100, 0.4f, 0.002f, 0.35f, SINUS, GERAEUSCH, 45); s.rauschen(r + 25.2f, 0.3f, TIEFPASS, 900, 0.3f); s.note(r + 25.2f, 43, 1); }
  if (bei(r + 27.2f, t0, t1)) for (int k = 0; k < 12; k++) s.note(r + 27.2f + k * 0.1f, 84 + (k & 1) * 5 + k, 0.4f * (1 - k / 13.0f));  // Vogel fliegt fort
  for (int i = 0; i < 3; i++) if (bei(r + 30 + i * 1.2f, t0, t1)) s.note(r + 30 + i * 1.2f, 71 + i * 2, 0.6f);  // Fragezeichen

  // Baum
  float b = T5;
  spieleMelodie(s, M(MEL_WANDER), b + 0.5f, b + 19.5f, 0.3f, t0, t1, 1);
  for (float x = ceilf(t0 / 0.555f) * 0.555f; x < t1; x += 0.555f)
    if (x > b + 0.5f && x < b + 19.5f) s.note(x, 45, 0.45f);  // schwere Riesenschritte
  if (bei(b + 20, t0, t1)) { s.ton(b + 20, 90, 0.4f, 0.002f, 0.4f, SINUS, GERAEUSCH, 40); s.rauschen(b + 20, 0.4f, TIEFPASS, 800, 0.3f); s.note(b + 20, 40, 1); }
  if (bei(b + 21, t0, t1)) for (int k = 0; k < 5; k++) s.note(b + 21 + k * 0.14f, 76 + k * 3, 0.7f);

  // Nacht
  float n = T6;
  spieleMelodie(s, M(MEL_N), n + 0.5f, n + 21.5f, 0.42f, t0, t1, 0.8f);
  for (float w : {3.6f, 12.6f, 18.6f}) {
    if (bei(n + w, t0, t1)) for (int k = 0; k < 6; k++) s.note(n + w + k * 0.14f, 88 - k * 3, 0.45f);  // Stein faellt
    if (bei(n + w + 1, t0, t1)) { s.ton(n + w + 1, 240, 0.25f, 0.002f, 0.15f, SINUS, GERAEUSCH, 110); s.note(n + w + 1, 60, 0.8f); }  // Bonk
  }
  for (float x = ceilf(t0 / 2.2f) * 2.2f; x < t1; x += 2.2f)  // Schnarchen (tiefes Brummen)
    if ((x > n + 1 && x < n + 4.5f) || (x > n + 8.5f && x < n + 13.5f) || (x > n + 17.5f && x < n + 19)) {
      s.ton(x, 70, 0.05f, 0.5f, 0.6f, DREIECK, GERAEUSCH, 85);
    }
  spieleMelodie(s, M(MEL_JAGD), n + 21.5f, n + 30, 0.17f, t0, t1, 0.9f);
  for (float x = ceilf(t0 / 0.25f) * 0.25f; x < t1; x += 0.25f)
    if (x > n + 21.5f && x < n + 30) s.rauschen(x, 0.06f, TIEFPASS, 700, 0.12f);  // Raufen
  if (bei(n + 30, t0, t1)) for (int k = 0; k < 10; k++) s.note(n + 30 + k * 0.08f, 84 - k * 2, 0.5f);  // weg!
  spieleMelodie(s, M(MEL_F), n + 33.5f, n + 37.5f, 0.26f, t0, t1, 0.8f);

  // Einhorn
  float e = T7;
  spieleMelodie(s, M(MEL_WANDER), e + 0.5f, e + 5, 0.24f, t0, t1, 0.8f);
  spieleMelodie(s, M(MEL_JAGD), e + 6.5f, e + 10.6f, 0.15f, t0, t1, 1);
  for (float x = ceilf(t0 / 0.227f) * 0.227f; x < t1; x += 0.227f)
    if ((x > e && x < e + 4) || (x > e + 6.5f && x < e + 10.6f)) s.note(x, 79, 0.18f);  // Hufe (Glöckchen)
  if (bei(e + 10.6f, t0, t1)) { s.ton(e + 10.6f, 140, 0.35f, 0.002f, 0.2f, SINUS, GERAEUSCH, 70); s.glocke(e + 10.6f, 72); }
  if (bei(e + 17, t0, t1)) for (int k = 0; k < 6; k++) s.glocke(e + 17 + k * 0.15f, 76 + k * 2, 0.6f);
  spieleMelodie(s, M(MEL_F), e + 19, e + 25, 0.26f, t0, t1, 0.8f);

  // Wildschwein
  float w = T8;
  spieleMelodie(s, M(MEL_JAGD), w + 0.3f, w + 8.4f, 0.16f, t0, t1, 1);
  for (float x = ceilf(t0 / 0.3f) * 0.3f; x < t1; x += 0.3f)
    if (x > w + 1 && x < w + 8) s.rauschen(x, 0.05f, TIEFPASS, 600, 0.14f);  // Trampeln
  if (bei(w + 8.5f, t0, t1)) for (int k = 0; k < 8; k++) s.note(w + 8.5f + k * 0.08f, 72 + k * 2, 0.6f);  // Sprung
  if (bei(w + 11.8f, t0, t1)) { s.ton(w + 11.8f, 110, 0.4f, 0.002f, 0.25f, SINUS, GERAEUSCH, 60); s.note(w + 11.8f, 48, 1); }  // Tuer zu
  for (float x = ceilf(t0 / 1.4f) * 1.4f; x < t1; x += 1.4f)
    if (x > w + 12.5f && x < w + 21) s.ton(x, 95, 0.12f, 0.02f, 0.25f, DREIECK, GERAEUSCH, 70);  // Grunzen
  spieleMelodie(s, M(MEL_F), w + 14, w + 25, 0.24f, t0, t1, 1);

  // Hochzeit
  float h = T9;
  spieleMelodie(s, M(MEL_F), h + 0.5f, h + 11, 0.26f, t0, t1, 1);
  for (int i = 0; i < 16; i++) {  // Hochzeitsglocken
    float x = h + 11 + i * 0.6f;
    if (bei(x, t0, t1)) s.glocke(x, i & 1 ? 79 : 72 + (i % 4 == 2) * 4, 0.9f);
  }
  spieleMelodie(s, M(MEL_A), h + 21, h + 33, 0.28f, t0, t1, 0.8f);
  for (int i = 0; i < 6; i++)
    for (int k = 0; k < 3; k++) {
      float x = h + 24 + i * 1.3f + k * 4 + 0.55f;  // Feuerwerk platzt
      if (bei(x, t0, t1) && x < h + 34) { s.glocke(x, 84 + (i % 3) * 3, 0.5f); s.rauschen(x, 0.4f, HOCHPASS, 3000, 0.05f); }
    }
  if (bei(h + 35.5f, t0, t1)) { s.glocke(h + 35.5f, 72); s.glocke(h + 35.8f, 76); s.glocke(h + 36.1f, 79); s.glocke(h + 36.5f, 84); }
}

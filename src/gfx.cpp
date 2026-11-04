#include "gfx.h"
#include <esp_attr.h>
#include <string.h>

// Unterzeilen je Bildzeile fuer weiche Kanten (2 = schnell, 4 = feiner)
constexpr int SUB = 2;
constexpr float GEW = 1.0f / SUB;

static inline uint16_t tausch(uint16_t v) { return (uint16_t)((v >> 8) | (v << 8)); }

Farbe mischeFarbe(Farbe a, Farbe b, float u) {
  u = klemme(u);
  int ra = a >> 11, ga = (a >> 5) & 63, ba = a & 31;
  int rb = b >> 11, gb = (b >> 5) & 63, bb = b & 31;
  int r = ra + (int)((rb - ra) * u + 0.5f), g = ga + (int)((gb - ga) * u + 0.5f), bl = ba + (int)((bb - ba) * u + 0.5f);
  return (Farbe)((r << 11) | (g << 5) | bl);
}

// Pixel mit Deckkraft a (0..256) mischen; Ziel ist in Display-Byte-Reihenfolge
static inline void mische(uint16_t &ziel, Farbe c, uint32_t a) {
  if (a >= 256) { ziel = tausch(c); return; }
  uint16_t d = tausch(ziel);
  // Rot+Blau und Gruen getrennt mischen (je 5/6/5 Bit), 0..32 Stufen genuegen
  uint32_t a5 = (a + 4) >> 3;
  uint32_t dv = (d | ((uint32_t)d << 16)) & 0x07E0F81F;
  uint32_t cv = (c | ((uint32_t)c << 16)) & 0x07E0F81F;
  uint32_t r = ((((cv - dv) * a5) >> 5) + dv) & 0x07E0F81F;
  ziel = tausch((uint16_t)(r | (r >> 16)));
}

void Gfx::streifen(uint16_t *b, int ya, int yb) { buf = b; y0 = ya; y1 = yb; }

void Gfx::kamera(float cx, float cy, float zoom) {
  sc = zoom;
  ox = W / 2 - cx * zoom;
  oy = H / 2 - cy * zoom;
}

void Gfx::fuellen(Farbe c) {
  uint16_t v = tausch(c);
  int n = (y1 - y0) * W;
  for (int i = 0; i < n; i++) buf[i] = v;
}

IRAM_ATTR void Gfx::verlauf(float ya, float yb, Farbe oben, Farbe unten) {
  float sa = Y(ya), sb = Y(yb);
  int a = (int)floorf(sa), b = (int)ceilf(sb);
  if (a < y0) a = y0;
  if (a < ky0) a = ky0;
  if (b > y1) b = y1;
  if (b > ky1) b = ky1;
  for (int y = a; y < b; y++) {
    uint16_t v = tausch(mischeFarbe(oben, unten, (y + 0.5f - sa) / (sb - sa)));
    uint16_t *z = buf + (y - y0) * W;
    for (int x = kx0; x < kx1; x++) z[x] = v;
  }
}

void Gfx::ausschnitt(float x, float y, float w, float h) {
  kx0 = (int)floorf(X(x)); ky0 = (int)floorf(Y(y));
  kx1 = (int)ceilf(X(x + w)); ky1 = (int)ceilf(Y(y + h));
  if (kx0 < 0) kx0 = 0;
  if (ky0 < 0) ky0 = 0;
  if (kx1 > W) kx1 = W;
  if (ky1 > H) ky1 = H;
}

// Blende: alles ausserhalb des Kreises einfarbig; zeilenweise direkt (schnell), Kreisrand weich
IRAM_ATTR void Gfx::iris(float cx, float cy, float r, Farbe c) {
  if (r > 400) return;
  const uint16_t v = tausch(c);
  for (int y = y0; y < y1; y++) {
    uint16_t *z = buf + (y - y0) * W;
    float dy = y + 0.5f - cy;
    if (fabsf(dy) >= r) {
      for (int x = 0; x < W; x++) z[x] = v;
      continue;
    }
    float dx = sqrtf(r * r - dy * dy);
    float l = cx - dx, rr = cx + dx;
    int il = (int)floorf(l), ir = (int)floorf(rr);
    for (int x = 0; x < il && x < W; x++) z[x] = v;
    if (il >= 0 && il < W) mische(z[il], c, (uint32_t)((1 - (l - il)) * 256));
    if (ir >= 0 && ir < W) mische(z[ir], c, (uint32_t)((rr - ir) * 256));
    for (int x = ir + 1 < 0 ? 0 : ir + 1; x < W; x++) z[x] = v;
  }
}

IRAM_ATTR void Gfx::zeile(int y, int xa, int xb, Farbe c, uint8_t a) {
  uint16_t *z = buf + (y - y0) * W;
  for (int x = xa; x <= xb; x++) {
    float cv = cov[x];
    if (cv <= 0.004f) continue;
    if (cv > 1) cv = 1;
    mische(z[x], c, (uint32_t)(cv * (a + 1)));
  }
}

// Polygon in Bildschirmkoordinaten fuellen (gerade-ungerade-Regel), weiche Kanten.
// Schneller Weg: hat jede der 4 Unterzeilen genau eine Spanne (konvexe Formen), wird das Innere
// direkt gefuellt und nur an den Raendern die Deckung berechnet.
IRAM_ATTR void Gfx::fuellePoly(const float *p, int n, Farbe c, uint8_t a) {
  if (n < 3 || a == 0) return;
  float minx = p[0], maxx = p[0], miny = p[1], maxy = p[1];
  for (int i = 1; i < n; i++) {
    float x = p[2 * i], y = p[2 * i + 1];
    if (x < minx) minx = x; if (x > maxx) maxx = x;
    if (y < miny) miny = y; if (y > maxy) maxy = y;
  }
  int ya = (int)floorf(miny), yb = (int)ceilf(maxy);
  if (ya < y0) ya = y0;
  if (ya < ky0) ya = ky0;
  if (yb > y1 - 1) yb = y1 - 1;
  if (yb > ky1 - 1) yb = ky1 - 1;
  if (ya > yb) return;
  int xa = (int)floorf(minx), xb = (int)ceilf(maxx);
  if (xa < kx0) xa = kx0;
  if (xb > kx1 - 1) xb = kx1 - 1;
  if (xa > xb) return;

  const float lo = (float)xa, hi = (float)(xb + 1);
  auto dazu = [&](float l, float r) {
    if (l < lo) l = lo;
    if (r > hi) r = hi;
    if (r <= l) return;
    int il = (int)l, ir = (int)r;
    if (il == ir) { cov[il] += (r - l) * GEW; return; }
    cov[il] += (il + 1 - l) * GEW;
    for (int x = il + 1; x < ir; x++) cov[x] += GEW;
    if (ir <= xb) cov[ir] += (r - ir) * GEW;
  };
  // Kanten vorberechnen (Steigung einmal je Kante statt einer Division je Unterzeile)
  struct Kante { float ya, yb, xa, dx; };
  static Kante kan[MAXP];
  int nk = 0;
  {
    float ax = p[2 * n - 2], ay = p[2 * n - 1];
    for (int i = 0; i < n; i++) {
      float bx = p[2 * i], by = p[2 * i + 1];
      if (ay != by) {
        Kante &k = kan[nk++];
        if (ay < by) { k.ya = ay; k.yb = by; k.xa = ax; } else { k.ya = by; k.yb = ay; k.xa = bx; }
        k.dx = (bx - ax) / (by - ay);
      }
      ax = bx; ay = by;
    }
  }
  const uint16_t cs = tausch(c);
  static float xs[SUB][MAXP];
  static int akt[MAXP];
  int ks[SUB];
  for (int y = ya; y <= yb; y++) {
    // Kanten, die diese Zeile beruehren
    int na = 0;
    const float fy0 = (float)y, fy1 = (float)(y + 1);
    for (int i = 0; i < nk; i++)
      if (kan[i].ya < fy1 && kan[i].yb > fy0) akt[na++] = i;
    if (na < 2) continue;
    bool etwas = false, eins = true;
    for (int s = 0; s < SUB; s++) {
      float sy = y + (s + 0.5f) * GEW;
      float *q = xs[s];
      int k = 0;
      for (int j = 0; j < na; j++) {
        const Kante &e = kan[akt[j]];
        if (e.ya <= sy && sy < e.yb) q[k++] = e.xa + (sy - e.ya) * e.dx;
      }
      if (k > 2)
        for (int i = 1; i < k; i++) {  // Einfuegesortieren (wenige Werte)
          float v = q[i]; int j = i - 1;
          while (j >= 0 && q[j] > v) { q[j + 1] = q[j]; j--; }
          q[j + 1] = v;
        }
      else if (k == 2 && q[0] > q[1]) { float v = q[0]; q[0] = q[1]; q[1] = v; }
      ks[s] = k;
      if (k >= 2) etwas = true;
      if (k != 2) eins = false;
    }
    if (!etwas) continue;
    uint16_t *z = buf + (y - y0) * W;
    if (eins) {
      float lmin = 1e9f, lmax = -1e9f, rmin = 1e9f, rmax = -1e9f;
      for (int s = 0; s < SUB; s++) {
        float l = xs[s][0] < lo ? lo : xs[s][0], r = xs[s][1] > hi ? hi : xs[s][1];
        xs[s][0] = l; xs[s][1] = r;
        if (l < lmin) lmin = l; if (l > lmax) lmax = l;
        if (r < rmin) rmin = r; if (r > rmax) rmax = r;
      }
      int inL = (int)lmax; if (inL < lmax) inL++;
      int inR = (int)rmin;
      if (inR > inL) {
        int e0 = (int)lmin, e1 = (int)rmax; if (e1 < rmax) e1++;
        if (e1 > xb) e1 = xb;
        for (int x = e0; x < inL; x++) cov[x] = 0;
        for (int x = inR; x <= e1; x++) cov[x] = 0;
        for (int s = 0; s < SUB; s++) {
          float l = xs[s][0], r = xs[s][1];
          dazu(l, r < inL ? r : (float)inL);
          dazu(l > inR ? l : (float)inR, r);
        }
        if (e0 < inL) zeile(y, e0, inL - 1, c, a);
        if (inR <= e1) zeile(y, inR, e1, c, a);
        if (a == 255) for (int x = inL; x < inR; x++) z[x] = cs;
        else for (int x = inL; x < inR; x++) mische(z[x], c, a + 1);
        continue;
      }
    }
    for (int x = xa; x <= xb + 1; x++) cov[x] = 0;
    for (int s = 0; s < SUB; s++)
      for (int i = 0; i + 1 < ks[s]; i += 2) dazu(xs[s][i], xs[s][i + 1]);
    zeile(y, xa, xb, c, a);
  }
}

IRAM_ATTR void Gfx::polygon(const float *xy, int n, Farbe c, uint8_t a) {
  if (n > MAXP) n = MAXP;
  for (int i = 0; i < n; i++) { pts[2 * i] = X(xy[2 * i]); pts[2 * i + 1] = Y(xy[2 * i + 1]); }
  fuellePoly(pts, n, c, a);
}

IRAM_ATTR void Gfx::rechteck(float x, float y, float w, float h, Farbe c, uint8_t a) {
  float q[8] = {X(x), Y(y), X(x + w), Y(y), X(x + w), Y(y + h), X(x), Y(y + h)};
  fuellePoly(q, 4, c, a);
}

IRAM_ATTR int Gfx::ellipsePunkte(float cx, float cy, float rx, float ry, float winkel, float *out) {
  float r = (rx > ry ? rx : ry) * sc;
  int n = (int)(10 + r * 0.9f);
  if (n > 64) n = 64;
  if (n < 10) n = 10;
  float cw = winkel != 0 ? cosf(winkel) : 1, sw = winkel != 0 ? sinf(winkel) : 0;
  float dc = cosf(6.2831853f / n), ds = sinf(6.2831853f / n), u = 1, v = 0;
  for (int i = 0; i < n; i++) {
    float ex = u * rx, ey = v * ry;
    out[2 * i] = X(cx + ex * cw - ey * sw);
    out[2 * i + 1] = Y(cy + ex * sw + ey * cw);
    float nu = u * dc - v * ds;
    v = u * ds + v * dc;
    u = nu;
  }
  return n;
}

IRAM_ATTR void Gfx::ellipse(float cx, float cy, float rx, float ry, Farbe c, uint8_t a, float winkel) {
  // ganz ausserhalb des Streifens? (schnell weg)
  float r = (rx > ry ? rx : ry) + kDicke;
  float sy = Y(cy), sr = r * sc;
  if (sy + sr < y0 || sy - sr > y1) return;
  if (kDicke > 0) {
    int n = ellipsePunkte(cx, cy, rx + kDicke, ry + kDicke, winkel, pts);
    fuellePoly(pts, n, kFarbe, a);
  }
  int n = ellipsePunkte(cx, cy, rx, ry, winkel, pts);
  fuellePoly(pts, n, c, a);
}

void Gfx::kreis(float cx, float cy, float r, Farbe c, uint8_t a) { ellipse(cx, cy, r, r, c, a); }

IRAM_ATTR void Gfx::rundRechteck(float x, float y, float w, float h, float r, Farbe c, uint8_t a) {
  auto baue = [&](float x, float y, float w, float h, float r) {
    if (r > w / 2) r = w / 2;
    if (r > h / 2) r = h / 2;
    int seg = (int)(3 + r * sc * 0.4f);
    if (seg > 10) seg = 10;
    float ecken[4][2] = {{x + w - r, y + r}, {x + w - r, y + h - r}, {x + r, y + h - r}, {x + r, y + r}};
    int k = 0;
    for (int e = 0; e < 4; e++)
      for (int i = 0; i <= seg; i++) {
        float t = (e - 1 + (float)i / seg) * 1.5707963f;
        pts[2 * k] = X(ecken[e][0] + cosf(t) * r);
        pts[2 * k + 1] = Y(ecken[e][1] + sinf(t) * r);
        k++;
      }
    return k;
  };
  float sy = Y(y), sh = h * sc;
  if (sy + sh + kDicke * sc < y0 || sy - kDicke * sc > y1) return;
  if (kDicke > 0) { int k = baue(x - kDicke, y - kDicke, w + 2 * kDicke, h + 2 * kDicke, r + kDicke); fuellePoly(pts, k, kFarbe, a); }
  int k = baue(x, y, w, h, r);
  fuellePoly(pts, k, c, a);
}

bool Gfx::draussen(float ymin, float ymax) const {
  float a = Y(ymin), b = Y(ymax);
  if (a > b) { float t = a; a = b; b = t; }
  return b < y0 || a > y1 || b < ky0 || a > ky1;
}

IRAM_ATTR void Gfx::linie(float x0, float y0_, float x1, float y1_, float dicke, Farbe c, uint8_t a) {
  if (draussen(fminf(y0_, y1_) - dicke, fmaxf(y0_, y1_) + dicke)) return;
  float dx = x1 - x0, dy = y1_ - y0_;
  float w = atan2f(dy, dx);
  float r = dicke / 2;
  int seg = (int)(3 + r * sc * 0.6f);
  if (seg > 12) seg = 12;
  int k = 0;
  for (int i = 0; i <= seg; i++) {  // Kappe am Ende
    float t = w - 1.5707963f + 3.14159265f * i / seg;
    pts[2 * k] = X(x1 + cosf(t) * r); pts[2 * k + 1] = Y(y1_ + sinf(t) * r); k++;
  }
  for (int i = 0; i <= seg; i++) {  // Kappe am Anfang
    float t = w + 1.5707963f + 3.14159265f * i / seg;
    pts[2 * k] = X(x0 + cosf(t) * r); pts[2 * k + 1] = Y(y0_ + sinf(t) * r); k++;
  }
  fuellePoly(pts, k, c, a);
}

void Gfx::kurve(float x0, float y0_, float x1, float y1_, float x2, float y2_, float dicke, Farbe c, uint8_t a) {
  if (draussen(fminf(fminf(y0_, y1_), y2_) - dicke, fmaxf(fmaxf(y0_, y1_), y2_) + dicke)) return;
  const int N = 12;
  float px[N + 1], py[N + 1];
  for (int i = 0; i <= N; i++) {
    float u = (float)i / N, v = 1 - u;
    px[i] = v * v * x0 + 2 * v * u * x1 + u * u * x2;
    py[i] = v * v * y0_ + 2 * v * u * y1_ + u * u * y2_;
  }
  float r = dicke / 2;
  int k = 0;
  for (int i = 0; i <= N; i++) {
    int a0 = i > 0 ? i - 1 : 0, b0 = i < N ? i + 1 : N;
    float tx = px[b0] - px[a0], ty = py[b0] - py[a0], l = sqrtf(tx * tx + ty * ty) + 1e-6f;
    pts[2 * k] = X(px[i] - ty / l * r); pts[2 * k + 1] = Y(py[i] + tx / l * r); k++;
  }
  for (int i = N; i >= 0; i--) {
    int a0 = i > 0 ? i - 1 : 0, b0 = i < N ? i + 1 : N;
    float tx = px[b0] - px[a0], ty = py[b0] - py[a0], l = sqrtf(tx * tx + ty * ty) + 1e-6f;
    pts[2 * k] = X(px[i] + ty / l * r); pts[2 * k + 1] = Y(py[i] - tx / l * r); k++;
  }
  fuellePoly(pts, k, c, a);
  float kd = kDicke; kDicke = 0;
  kreis(x0, y0_, r * 0.98f, c, a);
  kreis(x2, y2_, r * 0.98f, c, a);
  kDicke = kd;
}

void Gfx::bogen(float cx, float cy, float r, float w0, float w1, float dicke, Farbe c, uint8_t a) {
  if (draussen(cy - r - dicke, cy + r + dicke)) return;
  int seg = (int)(6 + fabsf(w1 - w0) * r * sc * 0.25f);
  if (seg > 40) seg = 40;
  int k = 0;
  float ra = r + dicke / 2, ri = r - dicke / 2;
  for (int i = 0; i <= seg; i++) { float t = w0 + (w1 - w0) * i / seg; pts[2 * k] = X(cx + cosf(t) * ra); pts[2 * k + 1] = Y(cy + sinf(t) * ra); k++; }
  for (int i = seg; i >= 0; i--) { float t = w0 + (w1 - w0) * i / seg; pts[2 * k] = X(cx + cosf(t) * ri); pts[2 * k + 1] = Y(cy + sinf(t) * ri); k++; }
  fuellePoly(pts, k, c, a);
}

void Gfx::stern(float cx, float cy, float r1, float r2, int zacken, float winkel, Farbe c, uint8_t a) {
  if (draussen(cy - r1, cy + r1)) return;
  int k = 0;
  if (zacken > MAXP / 2) zacken = MAXP / 2;
  for (int i = 0; i < 2 * zacken; i++) {
    float t = winkel + i * 3.14159265f / zacken - 1.5707963f;
    float r = i & 1 ? r2 : r1;
    pts[2 * k] = X(cx + cosf(t) * r); pts[2 * k + 1] = Y(cy + sinf(t) * r); k++;
  }
  fuellePoly(pts, k, c, a);
}

// ---------- Text ----------
static uint32_t naechstesZeichen(const char *&s) {
  uint8_t c = (uint8_t)*s++;
  if (c < 0x80) return c;
  if ((c & 0xE0) == 0xC0) { uint32_t v = (c & 0x1F) << 6; v |= (*s++ & 0x3F); return v; }
  if ((c & 0xF0) == 0xE0) { uint32_t v = (c & 0x0F) << 12; v |= (*s++ & 0x3F) << 6; v |= (*s++ & 0x3F); return v; }
  s += 3; return '?';
}
static const Glyph *finde(const Schrift &f, uint32_t code) {
  for (int i = 0; i < f.n; i++) if (f.g[i].code == code) return &f.g[i];
  for (int i = 0; i < f.n; i++) if (f.g[i].code == '?') return &f.g[i];
  return nullptr;
}

int Gfx::textBreite(const Schrift &f, const char *s) {
  int w = 0;
  while (*s) { const Glyph *g = finde(f, naechstesZeichen(s)); if (g) w += g->adv; }
  return w;
}

void Gfx::text(const Schrift &f, const char *s, float x, float y, Farbe c, int ausr, uint8_t a) {
  if (ausr) x -= textBreite(f, s) * (ausr == 1 ? 0.5f : 1.0f);
  int bx = (int)roundf(x + wx), by = (int)roundf(y + wy);
  if (by + f.desc + 2 < y0 || by - f.asc - 4 > y1) return;
  while (*s) {
    const Glyph *g = finde(f, naechstesZeichen(s));
    if (!g) continue;
    int gx = bx + g->x, gy = by - g->y, stride = (g->w + 1) / 2;
    for (int yy = 0; yy < g->h; yy++) {
      int py = gy + yy;
      if (py < y0 || py >= y1) continue;
      uint16_t *z = buf + (py - y0) * W;
      const uint8_t *q = f.bits + g->off + yy * stride;
      for (int xx = 0; xx < g->w; xx++) {
        int px = gx + xx;
        if (px < 0 || px >= W) continue;
        uint8_t v = (q[xx >> 1] >> ((xx & 1) ? 0 : 4)) & 15;
        if (v) mische(z[px], c, (uint32_t)v * 17 * (a + 1) / 255);
      }
    }
    bx += g->adv;
  }
}

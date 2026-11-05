#include "synth.h"
#include <esp_attr.h>
#include <math.h>
#include <string.h>

static float SINUS_TAB[1025];
static bool tabFertig = false;

float Synth::mtof(float m) { return 440.0f * powf(2.0f, (m - 69) / 12.0f); }

void Synth::start() {
  if (!tabFertig) {
    for (int i = 0; i <= 1024; i++) SINUS_TAB[i] = sinf(i * 6.2831853f / 1024);
    tabFertig = true;
  }
  stille();
  memset(kamm, 0, sizeof kamm);
  memset(allp, 0, sizeof allp);
  for (int i = 0; i < 4; i++) kz[i] = 0;
  pos = 0;
}

void Synth::stille() {
  for (auto &s : st) s.aktiv = false;
}

Synth::Stimme *Synth::frei(uint32_t start) {
  Stimme *alt = &st[0];
  for (auto &s : st) {
    if (!s.aktiv) return &s;
    if (s.start < alt->start) alt = &s;
  }
  return alt;  // aelteste Stimme ersetzen
}

static uint32_t abSample(float t, uint32_t pos) {
  float s = t * Synth::RATE;
  if (s < pos) return pos;
  return (uint32_t)s;
}

void Synth::ton(float t, float f, float spitze, float a, float d, Welle w, Bus bus, float f2) {
  if (spitze <= 0) return;
  t = wann(t);
  Stimme &s = *frei(0);
  memset(&s, 0, sizeof s);
  s.aktiv = true;
  s.rausch = false;
  s.welle = w;
  s.bus = bus;
  s.start = abSample(t, pos);
  s.na = (uint32_t)(a * RATE) + 1;
  s.nd = (uint32_t)(d * RATE) + 1;
  s.inc = f / RATE;
  s.glide = f2 > 0 ? powf(f2 / f, 1.0f / (s.na + s.nd)) : 1;
  s.env = 0.0001f;
  s.ka = powf(spitze / 0.0001f, 1.0f / s.na);
  s.kd = powf(0.0001f / spitze, 1.0f / s.nd);
}

IRAM_ATTR void Synth::filterSetzen(Stimme &s) {
  float f = s.ff;
  if (f > RATE * 0.45f) f = RATE * 0.45f;
  if (f < 20) f = 20;
  float w0 = 6.2831853f * f / RATE, cw = cosf(w0), al = sinf(w0) / (2 * s.q);
  float a0 = 1 + al;
  if (s.ftyp == TIEFPASS) { s.b0 = (1 - cw) / 2; s.b1 = 1 - cw; s.b2 = (1 - cw) / 2; }
  else if (s.ftyp == HOCHPASS) { s.b0 = (1 + cw) / 2; s.b1 = -(1 + cw); s.b2 = (1 + cw) / 2; }
  else { s.b0 = al; s.b1 = 0; s.b2 = -al; }
  s.a1 = -2 * cw; s.a2 = 1 - al;
  s.b0 /= a0; s.b1 /= a0; s.b2 /= a0; s.a1 /= a0; s.a2 /= a0;
}

void Synth::rauschen(float t, float dauer, Filter typ, float f, float spitze, float q, float a, float f2, Bus bus) {
  if (spitze <= 0) return;
  t = wann(t);
  Stimme &s = *frei(0);
  memset(&s, 0, sizeof s);
  s.aktiv = true;
  s.rausch = true;
  s.bus = bus;
  s.ftyp = typ;
  s.start = abSample(t, pos);
  s.na = (uint32_t)(a * RATE) + 1;
  s.nd = (uint32_t)(dauer * RATE) + 1;
  s.env = 0.0001f;
  s.ka = powf(spitze / 0.0001f, 1.0f / s.na);
  s.kd = powf(0.0001f / spitze, 1.0f / s.nd);
  s.ff = f;
  s.q = q;
  s.fglide = f2 > 0 ? powf(f2 / f, 32.0f / (s.na + s.nd)) : 1;
  s.zufall = 0x12345u + s.start * 2654435761u;
  filterSetzen(s);
}

void Synth::note(float t, int m, float v) {
  float f = mtof(m);
  float d = 2.3f - (m - 60) * 0.045f;
  if (d < 0.6f) d = 0.6f;
  if (d > 2.4f) d = 2.4f;
  ton(t, f, 0.15f * v, 0.003f, d, SINUS, MUSIK);
  ton(t, f * 2.005f, 0.045f * v, 0.003f, d * 0.5f, SINUS, MUSIK);
  ton(t, f * 4.2f, 0.016f * v, 0.003f, d * 0.18f, SINUS, MUSIK);
  rauschen(t, 0.012f, HOCHPASS, 3000, 0.02f * v, 1, 0.001f, 0, MUSIK);  // feines Klicken der Stiftwalze
}

void Synth::glocke(float t, int m, float v) {
  float f = mtof(m);
  ton(t, f, 0.11f * v, 0.003f, 2.4f, SINUS, MUSIK);
  ton(t, f * 2, 0.05f * v, 0.003f, 1.6f, SINUS, MUSIK);
  ton(t, f * 2.76f, 0.045f * v, 0.003f, 1.2f, SINUS, MUSIK);
  ton(t, f * 5.4f, 0.02f * v, 0.003f, 0.6f, SINUS, MUSIK);
}

static inline float welle(uint8_t w, float ph) {
  switch (w) {
    case SINUS: {
      float x = ph * 1024;
      int i = (int)x;
      float fr = x - i;
      return SINUS_TAB[i] + (SINUS_TAB[i + 1] - SINUS_TAB[i]) * fr;
    }
    case DREIECK: return ph < 0.5f ? ph * 4 - 1 : 3 - ph * 4;
    case SAEGE: return ph * 2 - 1;
    default: return ph < 0.5f ? 0.7f : -0.7f;
  }
}

IRAM_ATTR void Synth::render(int16_t *aus, int n) {
  for (int i = 0; i < n; i++) {
    float musik = 0, ger = 0;
    uint32_t jetzt = pos + i;
    for (auto &s : st) {
      if (!s.aktiv || jetzt < s.start) continue;
      float v;
      if (s.rausch) {
        s.zufall = s.zufall * 1664525u + 1013904223u;
        float x = (int32_t)s.zufall * (1.0f / 2147483648.0f);
        float y = s.b0 * x + s.z1;
        s.z1 = s.b1 * x - s.a1 * y + s.z2;
        s.z2 = s.b2 * x - s.a2 * y;
        v = y;
        if (s.fglide != 1 && (s.n & 31) == 0) { s.ff *= s.fglide; filterSetzen(s); }
      } else {
        v = welle(s.welle, s.phase);
        s.phase += s.inc;
        if (s.phase >= 1) s.phase -= 1;
        s.inc *= s.glide;
      }
      v *= s.env;
      if (s.n < s.na) s.env *= s.ka; else s.env *= s.kd;
      if (++s.n >= s.na + s.nd) s.aktiv = false;
      if (s.bus == MUSIK) musik += v; else ger += v;
    }
    // Hall (4 Kammfilter + 2 Allpaesse)
    float ein = musik * 0.33f + ger * 0.1f, hall = 0;
    for (int k = 0; k < 4; k++) {
      float y = kamm[k][ki[k]];
      kz[k] = y * 0.7f + kz[k] * 0.3f;
      kamm[k][ki[k]] = ein + kz[k] * 0.8f;
      if (++ki[k] >= kl[k]) ki[k] = 0;
      hall += y;
    }
    hall *= 0.25f;
    for (int k = 0; k < 2; k++) {
      float b = allp[k][ai[k]];
      float y = -hall + b;
      allp[k][ai[k]] = hall + b * 0.5f;
      if (++ai[k] >= al[k]) ai[k] = 0;
      hall = y;
    }
    float x = (musik * 0.6f + ger + hall * 0.9f) * 1.6f;
    // weich begrenzen
    if (x > 3) x = 3;
    if (x < -3) x = -3;
    x = x * (27 + x * x) / (27 + 9 * x * x);
    aus[i] = (int16_t)(x * 30000);
  }
  pos += n;
}

void spieleMelodie(Synth &s, const Melodie &mel, float tStart, float tEnde, float schlag, float t0, float t1,
                   float staerke) {
  if (t1 <= tStart || t0 >= tEnde) return;
  float gesamt = 0;
  for (int i = 0; i < mel.n; i++) gesamt += mel.t[i].laenge;
  float runde = gesamt * schlag;
  int k = (int)floorf((t0 - tStart) / runde);
  if (k < 0) k = 0;
  for (; tStart + k * runde < t1; k++) {
    float b = tStart + k * runde;
    for (int i = 0; i < mel.n; i++) {
      const Ton3 &n = mel.t[i];
      if (b >= t0 && b < t1 && b < tEnde) {
        if (n.m) s.note(b, n.m, staerke);
        if (n.bass) {
          s.note(b, n.bass, 0.7f * staerke);
        }
      }
      b += n.laenge * schlag;
      if (b >= t1) break;
    }
  }
}

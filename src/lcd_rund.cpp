// Rundes AMOLED-Display 466 x 466 (CO5300 ueber QSPI) des Waveshare ESP32-S3-Touch-AMOLED-1.75C.
// Das 284 x 240 grosse Maerchenbild wird 1,25-fach vergroessert (355 x 300, passt genau in den Kreis); drumherum wird einmal ein fester Rahmen gezeichnet:
// Nachthimmel mit Sternen und Goldrand, wie das Fenster einer Spieluhr.
// Startsequenz und Pins wie in der Xiaozhi-Firmware fuer dieses Board (boards/waveshare/esp32-s3-touch-amoled-1.75).
// Das Panel hat 6 Pixel Spaltenversatz. Die Helligkeit stellt der Befehl 0x51 ein (AMOLED, keine Hintergrundbeleuchtung).
#ifdef BOARD_RUND
#include "lcd.h"
#include <Arduino.h>
#include <driver/spi_master.h>
#include <freertos/semphr.h>
#include <math.h>

namespace lcd {

constexpr int PIN_SCLK = 38, PIN_D0 = 4, PIN_D1 = 5, PIN_D2 = 6, PIN_D3 = 7, PIN_CS = 12, PIN_RST = 1;
constexpr int PW = 466, PH = 466;                     // Panel
constexpr int SPALTE0 = 6;                            // Spaltenversatz des Panels
// Das Panel nimmt Fenster nur mit gerader Startspalte/-zeile und gerader Breite/Hoehe an: alles gerade halten.
constexpr int SW = 356, SH = H * 5 / 4;              // das Maerchenbild etwa 1,25-fach: 356 x 300
constexpr int OX = 54, OY = 82;                       // Lage auf dem Panel (gerade)
constexpr int TEIL_ZEILEN = 20, TEILE = 3;            // 356 x 20 = 14 KB je Teil; Teile gehen ueber Streifengrenzen
static uint16_t *teil[TEILE];
static int teilNaechster = 0;
static int teilGefuellt = 0;                          // Zeilen im aktuellen Teil
static int teilStart = 0;                             // erste Ausgabezeile des aktuellen Teils

static spi_device_handle_t dev;
static uint16_t *buf[2];
static int naechster = 0;
static SemaphoreHandle_t frei;
static bool an = false, umgedreht = false;
static int offen = 0;  // noch nicht abgeholte Uebertragungen

struct Befehl { uint8_t cmd, n; uint8_t d[4]; uint16_t warte; };
static const Befehl INIT[] = {
  {0xFE, 1, {0x20}}, {0x19, 1, {0x10}}, {0x1C, 1, {0xA0}},  // QSPI-Betrieb
  {0xFE, 1, {0x00}}, {0xC4, 1, {0x80}}, {0x3A, 1, {0x55}}, {0x35, 1, {0x00}}, {0x53, 1, {0x20}},
  {0x51, 1, {0x00}}, {0x63, 1, {0xFF}},
  {0x2A, 4, {0x00, 0x06, 0x01, 0xD7}}, {0x2B, 4, {0x00, 0x00, 0x01, 0xD1}, 600},
  {0x11, 0, {}, 600}, {0x29, 0, {}},
};

// --- QSPI: Befehl 0x02 + 24-Bit-Adresse (Register << 8) einadrig, Bilddaten mit 0x32 vieradrig ---
static spi_transaction_ext_t tPar[TEILE][2], tDat[TEILE];
static uint8_t fenster[TEILE][2][4];

static void abholen(bool warte) {
  spi_transaction_t *r;
  while (offen > 0 && spi_device_get_trans_result(dev, &r, warte ? portMAX_DELAY : 0) == ESP_OK) offen--;
}

static void befehl(uint8_t reg, const uint8_t *d, int n) {
  abholen(true);
  spi_transaction_ext_t t = {};
  t.base.flags = SPI_TRANS_VARIABLE_CMD | SPI_TRANS_VARIABLE_ADDR;
  t.command_bits = 8; t.address_bits = 24;
  t.base.cmd = 0x02;
  t.base.addr = (uint32_t)reg << 8;
  t.base.length = n * 8;
  t.base.tx_buffer = n ? d : nullptr;
  spi_device_polling_transmit(dev, (spi_transaction_t *)&t);
}

static void IRAM_ATTR fertig(spi_transaction_t *t) {
  if ((uint32_t)t->user == 1) {
    BaseType_t weck = pdFALSE;
    xSemaphoreGiveFromISR(frei, &weck);
    if (weck) portYIELD_FROM_ISR();
  }
}

static void fensterWerte(uint8_t *ca, uint8_t *ra, int x0, int y0, int w, int h) {
  int xa = x0 + SPALTE0, xb = xa + w - 1, yb = y0 + h - 1;
  ca[0] = xa >> 8; ca[1] = xa & 0xFF; ca[2] = xb >> 8; ca[3] = xb & 0xFF;
  ra[0] = y0 >> 8; ra[1] = y0 & 0xFF; ra[2] = yb >> 8; ra[3] = yb & 0xFF;
}

// ein Rechteck aus dem Puffer schicken (blockierend), fuer den Rahmen
static void blockSenden(int x0, int y0, int w, int h, const uint16_t *p) {
  uint8_t ca[4], ra[4];
  fensterWerte(ca, ra, x0, y0, w, h);
  befehl(0x2A, ca, 4);
  befehl(0x2B, ra, 4);
  spi_transaction_ext_t t = {};
  t.base.flags = SPI_TRANS_VARIABLE_CMD | SPI_TRANS_VARIABLE_ADDR | SPI_TRANS_MODE_QIO;
  t.command_bits = 8; t.address_bits = 24;
  t.base.cmd = 0x32;
  t.base.addr = 0x2C << 8;
  t.base.length = w * h * 16;
  t.base.tx_buffer = p;
  spi_device_polling_transmit(dev, (spi_transaction_t *)&t);
}

// ---------- der feste Rahmen ----------
static inline uint16_t rgb(float r, float g, float b) {
  int R = r < 0 ? 0 : r > 255 ? 255 : (int)r, G = g < 0 ? 0 : g > 255 ? 255 : (int)g, B = b < 0 ? 0 : b > 255 ? 255 : (int)b;
  uint16_t v = ((R & 0xF8) << 8) | ((G & 0xFC) << 3) | (B >> 3);
  return (uint16_t)((v >> 8) | (v << 8));  // Display-Byte-Reihenfolge
}
static inline float stufe(float a, float b, float x) { float u = (x - a) / (b - a); return u < 0 ? 0 : u > 1 ? 1 : u; }
static inline uint32_t hash(uint32_t i) { i ^= i >> 16; i *= 0x7feb352d; i ^= i >> 15; i *= 0x846ca68b; i ^= i >> 16; return i; }

static uint16_t rahmenPixel(int x, int y) {
  const float RK = PW / 2.0f;
  float dx = x + 0.5f - PW / 2.0f, dy = y + 0.5f - PH / 2.0f;
  float r = sqrtf(dx * dx + dy * dy);
  float qx = fabsf(dx) - (SW / 2.0f - 6), qy = fabsf(dy) - (SH / 2.0f - 6);
  float ax = qx > 0 ? qx : 0, ay = qy > 0 ? qy : 0;
  float d = sqrtf(ax * ax + ay * ay) + (qx > qy ? (qx < 0 ? qx : 0) : (qy < 0 ? qy : 0)) - 6;
  float v = (y / (float)PH);
  float R = 22 + 40 * v, G = 20 + 14 * v, B = 70 + 40 * v;
  uint32_t h = hash((uint32_t)(x / 3) * 7919u + (uint32_t)(y / 3) * 104729u);
  if ((h & 1023) < 9 && (x % 3) == 1 && (y % 3) == 1) { float k = 0.5f + ((h >> 10) & 255) / 512.0f; R += 200 * k; G += 190 * k; B += 140 * k; }
  float gold = stufe(5.5f, 4.5f, d) * stufe(0.5f, 1.5f, d);
  float gold2 = stufe(9.5f, 8.7f, d) * stufe(7.3f, 8.1f, d) * 0.8f;
  float schein = stufe(26, 6, d) * 0.25f;
  R += (255 - R) * schein * 0.6f; G += (213 - G) * schein * 0.5f; B += (74 - B) * schein * 0.3f;
  float gg = gold > gold2 ? gold : gold2;
  R = R + (255 - R) * gg; G = G + (210 - G) * gg; B = B + (90 - B) * gg;
  float ring = stufe(RK - 10, RK - 8, r) * stufe(RK - 2, RK - 4, r);
  R = R + (230 - R) * ring; G = G + (180 - G) * ring; B = B + (60 - B) * ring;
  if (r > RK - 1) { R = G = B = 0; }
  return rgb(R, G, B);
}

static void rahmenZeichnen() {
  uint16_t *p = buf[0];
  const int zeilen = 30;  // 466 x 30 x 2 Byte = 28 KB (eine Uebertragung darf hoechstens 32 KB sein)
  for (int y0 = 0; y0 < PH; y0 += zeilen) {
    int n = PH - y0 < zeilen ? PH - y0 : zeilen;
    for (int y = 0; y < n; y++)
      for (int x = 0; x < PW; x++) {
        bool innen = x >= OX && x < OX + SW && y0 + y >= OY && y0 + y < OY + SH;
        p[y * PW + x] = innen ? 0 : rahmenPixel(x, y0 + y);
      }
    blockSenden(0, y0, PW, n, p);
  }
}

void start() {
  pinMode(PIN_RST, OUTPUT);
  digitalWrite(PIN_RST, LOW); delay(20);
  digitalWrite(PIN_RST, HIGH); delay(150);

  spi_bus_config_t bus = {};
  bus.data0_io_num = PIN_D0;
  bus.data1_io_num = PIN_D1;
  bus.sclk_io_num = PIN_SCLK;
  bus.data2_io_num = PIN_D2;
  bus.data3_io_num = PIN_D3;
  bus.data4_io_num = bus.data5_io_num = bus.data6_io_num = bus.data7_io_num = -1;
  bus.max_transfer_sz = 32768;
  bus.flags = SPICOMMON_BUSFLAG_MASTER | SPICOMMON_BUSFLAG_QUAD;
  spi_bus_initialize(SPI2_HOST, &bus, SPI_DMA_CH_AUTO);

  frei = xSemaphoreCreateCounting(TEILE, TEILE);
  spi_device_interface_config_t d = {};
  d.mode = 0;
  d.clock_speed_hz = 40 * 1000 * 1000;
  d.spics_io_num = PIN_CS;
  d.flags = SPI_DEVICE_HALFDUPLEX;
  d.queue_size = 14;
  d.post_cb = fertig;
  spi_bus_add_device(SPI2_HOST, &d, &dev);

  for (const auto &b : INIT) {
    befehl(b.cmd, b.d, b.n);
    if (b.warte) delay(b.warte);
  }
  for (int i = 0; i < 2; i++) buf[i] = (uint16_t *)heap_caps_malloc(W * STREIFEN * 2, MALLOC_CAP_DMA);
  for (int i = 0; i < TEILE; i++) teil[i] = (uint16_t *)heap_caps_malloc(SW * TEIL_ZEILEN * 2, MALLOC_CAP_DMA);
  rahmenZeichnen();
  Serial.println("AMOLED bereit");
}

uint16_t *puffer() {
  // die Eingangsstreifen sind sofort wieder frei: senden() vergroessert sie in die Teilpuffer
  uint16_t *p = buf[naechster];
  naechster ^= 1;
  return p;
}

void drehen(bool d) { umgedreht = d; }  // der Rahmen sieht gedreht genauso aus

// einen vollen Teil (TEIL_ZEILEN Ausgabezeilen ab Zeile start) abschicken
static void teilSchicken(int b, int start) {
  uint8_t *ca = fenster[b][0], *ra = fenster[b][1];
  fensterWerte(ca, ra, OX, OY + start, SW, TEIL_ZEILEN);
  for (int i = 0; i < 2; i++) {
    spi_transaction_ext_t &t = tPar[b][i];
    t = {};
    t.base.flags = SPI_TRANS_VARIABLE_CMD | SPI_TRANS_VARIABLE_ADDR;
    t.command_bits = 8; t.address_bits = 24;
    t.base.cmd = 0x02;
    t.base.addr = (i ? 0x2B : 0x2A) << 8;
    t.base.length = 32;
    t.base.tx_buffer = i ? ra : ca;
    spi_device_queue_trans(dev, (spi_transaction_t *)&t, portMAX_DELAY);
    offen++;
  }
  spi_transaction_ext_t &t = tDat[b];
  t = {};
  t.base.flags = SPI_TRANS_VARIABLE_CMD | SPI_TRANS_VARIABLE_ADDR | SPI_TRANS_MODE_QIO;
  t.command_bits = 8; t.address_bits = 24;
  t.base.cmd = 0x32;
  t.base.addr = 0x2C << 8;
  t.base.length = SW * TEIL_ZEILEN * 16;
  t.base.tx_buffer = teil[b];
  t.base.user = (void *)1;
  spi_device_queue_trans(dev, (spi_transaction_t *)&t, portMAX_DELAY);
  offen++;
}

// Streifen (284 x 60) auf 356 x 75 vergroessern (naechster Nachbar) und in Teilen zu je 20 Zeilen schicken.
// Die Streifen kommen der Reihe nach (y = 0, 60, 120, 180), auch beim gedrehten Bild.
void senden(uint16_t *p, int y) {
  static uint16_t xmap[SW];
  static bool xmapFertig = false;
  if (!xmapFertig) { for (int c = 0; c < SW; c++) xmap[c] = c * W / SW; xmapFertig = true; }
  int ys = y;
  if (umgedreht) {
    for (int i = 0, j = W * STREIFEN - 1; i < j; i++, j--) { uint16_t v = p[i]; p[i] = p[j]; p[j] = v; }
    ys = H - STREIFEN - y;
  }
  if (y == 0) { teilGefuellt = 0; teilStart = 0; }
  // Ausgabezeilen dieses Streifens auf dem Bildschirm (beim Drehen von unten nach oben)
  const int oa = ys * SH / H, ob = (ys + STREIFEN) * SH / H;
  for (int n = 0; n < ob - oa; n++) {
    int oy = umgedreht ? ob - 1 - n : oa + n;   // Bildschirmzeile
    if (teilGefuellt == 0) {
      xSemaphoreTake(frei, portMAX_DELAY);       // ein Teilpuffer ist frei geworden
      abholen(false);
      teilStart = umgedreht ? oy - TEIL_ZEILEN + 1 : oy;
    }
    int iy = oy * H / SH - ys;
    if (iy < 0) iy = 0;
    if (iy >= STREIFEN) iy = STREIFEN - 1;
    const uint16_t *zi = p + iy * W;
    uint16_t *zo = teil[teilNaechster] + (oy - teilStart) * SW;
    for (int c = 0; c < SW; c++) zo[c] = zi[xmap[c]];
    if (++teilGefuellt == TEIL_ZEILEN) {
      teilSchicken(teilNaechster, teilStart);
      teilNaechster = (teilNaechster + 1) % TEILE;
      teilGefuellt = 0;
    }
  }
}
void warten() {
  for (int i = 0; i < TEILE; i++) xSemaphoreTake(frei, portMAX_DELAY);
  abholen(true);
  for (int i = 0; i < TEILE; i++) xSemaphoreGive(frei);
}
// AMOLED: Helligkeit ueber Befehl 0x51 (0..255)
static float hell = 0.5f;
static void hellSetzen() {
  uint8_t v = an ? (uint8_t)(hell * 255) : 0;
  befehl(0x51, &v, 1);
}
void licht(bool a) { an = a; hellSetzen(); }
void helligkeit(float h) {
  hell = h < 0.03f ? 0.03f : h > 1 ? 1 : h;
  if (an) hellSetzen();
}
bool lichtAn() { return an; }

}  // namespace lcd
#endif

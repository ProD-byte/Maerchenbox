#include "lcd.h"
#include <Arduino.h>
#include <driver/spi_master.h>
#include <esp_lcd_panel_io.h>
#include <freertos/semphr.h>

namespace lcd {

constexpr int PIN_SCLK = 9, PIN_MOSI = 10, PIN_CS = 14, PIN_DC = 8, PIN_RST = 18, PIN_LICHT = 13;
constexpr int X_OFFSET = 36;      // wie in Xiaozhi (set_gap)
constexpr uint8_t MADCTL = 0xA8;  // BGR | quer | gespiegelt Y - wie in Xiaozhi

static esp_lcd_panel_io_handle_t io = nullptr;
static uint16_t *buf[2];
static int naechster = 0;
static SemaphoreHandle_t frei;  // zaehlt freie Puffer
static bool an = false;
static bool umgedreht = false;

static bool fertig(esp_lcd_panel_io_handle_t, esp_lcd_panel_io_event_data_t *, void *) {
  BaseType_t weck = pdFALSE;
  xSemaphoreGiveFromISR(frei, &weck);
  return weck == pdTRUE;
}

struct Befehl { uint8_t cmd, n; uint8_t d[8]; uint16_t warte; };
// Startsequenz genau wie in der Xiaozhi-Firmware dieses Boards
static const Befehl INIT[] = {
  {0xfd, 2, {0x06, 0x08}, 0}, {0x61, 2, {0x07, 0x04}, 0}, {0x62, 3, {0x00, 0x44, 0x45}, 0},
  {0x63, 4, {0x41, 0x07, 0x12, 0x12}, 0}, {0x64, 1, {0x37}, 0}, {0x65, 3, {0x09, 0x10, 0x21}, 0},
  {0x66, 3, {0x09, 0x10, 0x21}, 0}, {0x67, 2, {0x20, 0x40}, 0}, {0x68, 4, {0x90, 0x4c, 0x7c, 0x66}, 0},
  {0xb1, 3, {0x0f, 0x02, 0x01}, 0}, {0xb4, 1, {0x01}, 0}, {0xb5, 4, {0x02, 0x02, 0x0a, 0x14}, 0},
  {0xb6, 5, {0x04, 0x01, 0x9f, 0x00, 0x02}, 0}, {0xdf, 1, {0x11}, 0},
  {0xe2, 6, {0x13, 0x00, 0x00, 0x30, 0x33, 0x3f}, 0}, {0xe5, 6, {0x3f, 0x33, 0x30, 0x00, 0x00, 0x13}, 0},
  {0xe1, 2, {0x00, 0x57}, 0}, {0xe4, 2, {0x58, 0x00}, 0},
  {0xe0, 8, {0x01, 0x03, 0x0d, 0x0e, 0x0e, 0x0c, 0x15, 0x19}, 0},
  {0xe3, 8, {0x1a, 0x16, 0x0c, 0x0f, 0x0e, 0x0d, 0x02, 0x01}, 0},
  {0xe6, 2, {0x00, 0xff}, 0}, {0xe7, 6, {0x01, 0x04, 0x03, 0x03, 0x00, 0x12}, 0},
  {0xe8, 3, {0x00, 0x70, 0x00}, 0}, {0xec, 1, {0x52}, 0}, {0xf1, 3, {0x01, 0x01, 0x02}, 0},
  {0xf6, 4, {0x09, 0x10, 0x00, 0x00}, 0}, {0xfd, 2, {0xfa, 0xfc}, 0},
  {0x3a, 1, {0x05}, 0}, {0x35, 1, {0x00}, 0}, {0x36, 1, {MADCTL}, 0},
  {0x21, 0, {}, 0}, {0x11, 0, {}, 200}, {0x29, 0, {}, 10},
};

void start() {
  pinMode(PIN_LICHT, OUTPUT);
  digitalWrite(PIN_LICHT, LOW);
  pinMode(PIN_RST, OUTPUT);
  digitalWrite(PIN_RST, LOW); delay(50);    // laengerer Reset, das NV3023 braucht ihn
  digitalWrite(PIN_RST, HIGH); delay(150);

  spi_bus_config_t bus = {};
  bus.mosi_io_num = PIN_MOSI;
  bus.miso_io_num = -1;
  bus.sclk_io_num = PIN_SCLK;
  bus.quadwp_io_num = -1;
  bus.quadhd_io_num = -1;
  bus.max_transfer_sz = W * STREIFEN * 2 + 16;
  spi_bus_initialize(SPI3_HOST, &bus, SPI_DMA_CH_AUTO);

  frei = xSemaphoreCreateCounting(2, 2);
  esp_lcd_panel_io_spi_config_t c = {};
  c.cs_gpio_num = PIN_CS;
  c.dc_gpio_num = PIN_DC;
  c.spi_mode = 0;
  c.pclk_hz = 40 * 1000 * 1000;
  c.trans_queue_depth = 4;
  c.on_color_trans_done = fertig;
  c.lcd_cmd_bits = 8;
  c.lcd_param_bits = 8;
  esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t)SPI3_HOST, &c, &io);

  for (const auto &b : INIT) {
    esp_lcd_panel_io_tx_param(io, b.cmd, b.n ? b.d : nullptr, b.n);
    if (b.warte) delay(b.warte);
  }
  for (int i = 0; i < 2; i++) buf[i] = (uint16_t *)heap_caps_malloc(W * STREIFEN * 2, MALLOC_CAP_DMA);
}

uint16_t *puffer() {
  xSemaphoreTake(frei, portMAX_DELAY);
  uint16_t *p = buf[naechster];
  naechster ^= 1;
  return p;
}

void drehen(bool d) { umgedreht = d; }

void senden(uint16_t *p, int y) {
  if (umgedreht) {  // Streifen spiegeln und an die gegenueberliegende Stelle schicken
    for (int i = 0, j = W * STREIFEN - 1; i < j; i++, j--) { uint16_t v = p[i]; p[i] = p[j]; p[j] = v; }
    y = H - STREIFEN - y;
  }
  const int x0 = X_OFFSET, x1 = X_OFFSET + W - 1, y1 = y + STREIFEN - 1;
  uint8_t ca[4] = {(uint8_t)(x0 >> 8), (uint8_t)x0, (uint8_t)(x1 >> 8), (uint8_t)x1};
  uint8_t ra[4] = {(uint8_t)(y >> 8), (uint8_t)y, (uint8_t)(y1 >> 8), (uint8_t)y1};
  esp_lcd_panel_io_tx_param(io, 0x2A, ca, 4);
  esp_lcd_panel_io_tx_param(io, 0x2B, ra, 4);
  esp_lcd_panel_io_tx_color(io, 0x2C, p, W * STREIFEN * 2);
}

void warten() {
  // beide Puffer zurueckholen und wieder freigeben
  xSemaphoreTake(frei, portMAX_DELAY);
  xSemaphoreTake(frei, portMAX_DELAY);
  xSemaphoreGive(frei);
  xSemaphoreGive(frei);
}

void licht(bool a) {
  an = a;
  digitalWrite(PIN_LICHT, a ? HIGH : LOW);
}
bool lichtAn() { return an; }

}  // namespace lcd

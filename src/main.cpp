// Maerchenbox
// Spieluhr-Filme, die die Box live zeichnet (30 Bilder/s) und vertont.
//
// Knoepfe:  Start  1x = Start / Stopp, 2x = naechstes Maerchen, lang = Maerchenbuch
//           Laut / Leise = Lautstaerke; im Maerchenbuch: umblaettern
//           Knoepfe rechts am Geraet, von oben nach unten: GPIO0, GPIO39, GPIO40
//           normal:      oben GPIO0 = Lauter, Mitte GPIO39 = Start, unten GPIO40 = Leiser
//           Bild gedreht: das Geraet steht auf dem Kopf, also GPIO40 = Lauter, GPIO39 = Start,
//           GPIO0 = Leiser - oben ist immer Lauter, unten immer Leiser
// WLAN:     im Maerchenbuch auf der Seite "Einstellungen" (Handy mit "Märchenbox-Setup" verbinden).
//           Updates ueber WLAN an maerchenbox.local.
// Fuer die Entwicklung: http://maerchenbox.local/bild.bmp (Bildschirmfoto, ?t=Sekunden),
//           /taste?k=kurz|doppel|lang|laut|leise, /status
//
// Pins aus der Original-Firmware ausgelesen (ostb-2st-wifi-double-mic), siehe README.md.

#include <Arduino.h>
#include <functional>
#include <Wire.h>
#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <ArduinoOTA.h>
#include <Preferences.h>
#include <driver/i2s.h>
#include <time.h>
#include "gfx.h"
#include "lcd.h"
#include "synth.h"
#include "maerchen.h"
#include "schriften.h"

#ifndef OTA_PASSWORT
#define OTA_PASSWORT ""
#endif
#define NAME_IM_NETZ "maerchenbox"
#define SETUP_WLAN "Märchenbox-Setup"

constexpr int PIN_I2C_SDA = 12, PIN_I2C_SCL = 11;
constexpr int PIN_MCLK = 5, PIN_BCLK = 15, PIN_WS = 16, PIN_DOUT = 6;
constexpr int PIN_PA = 4;
constexpr int TASTE_A = 39, TASTE_B = 0, TASTE_C = 40;  // die drei Knoepfe (GPIO)
constexpr uint8_t ES8311_ADDR = 0x18;

// ---------- Maerchen ----------
static const Maerchen *const LISTE[] = {&SCHNEIDERLEIN, &FUERCHTEN, &ASCHENPUTTEL, &ROSENROT, &GEISSLEIN, &RAPUNZEL_M, &SCHNEEWITTCHEN, &CINDERELLA, &MAXMORITZ, &ROTKAEPPCHEN, &BREMEN};
constexpr int ANZAHL = sizeof(LISTE) / sizeof(LISTE[0]);

// ---------- Zustand ----------
enum Zustand { TITEL, SPIELT, MENUE, EINRICHTUNG, UPDATE, EINSTELLUNGEN, SCHLAF };
Zustand zustand = TITEL;
int auswahl = 0;            // aktuelles Maerchen
// Maerchenbuch: nur die Maerchen (Seite 0..ANZAHL-1). Die Einstellungen sind versteckt:
// Lauter + Leiser 3 s gleichzeitig halten. SEITE_DREHEN / SEITE_WLAN sind nur Bilder dafuer.
constexpr int SEITEN = ANZAHL, SEITE_DREHEN = ANZAHL, SEITE_WLAN = ANZAHL + 1, SEITE_LAUT = ANZAHL + 2, SEITE_LESEN = ANZAHL + 3, SEITE_ZEIT = ANZAHL + 4, SEITE_HELL = ANZAHL + 5;
constexpr int EINST_PUNKTE = 7;  // drehen, laut, hell, lesen, Nutzungszeit, WLAN, zurueck
constexpr uint32_t KOMBI_MS = 3000;
int einstZiel = 0;          // Drehrad: Ziel (ohne Kreis-Rest), einstPos = Rest
float einstAnim = 0;        // Drehrad: aktuelle, weich nachlaufende Stellung
int einstPos = 0;           // Auswahl in den Einstellungen: 0 drehen, 1 laut, 2 hell, 3 lesen, 4 Zeit, 5 WLAN, 6 zurueck
// Hoechstlautstaerke (nur von den Eltern einstellbar): Stufe 0..3, ab Werk 1 = halb so laut wie voll
// (etwa -10 dB, so viel wird als "halb so laut" empfunden). Die 10 Kinder-Stufen liegen darunter.
constexpr int GRENZEN = 4;
static const int GRENZE_DB[GRENZEN] = {-16, -10, -5, 0};
static const char *const GRENZE_TEXT[GRENZEN] = {"Max. laut: 30 %", "Max. laut: 50 %", "Max. laut: 70 %", "Max. laut: 100 %"};
int grenze = 1;
float kombiFortschritt = 0; // 0..1 waehrend Lauter + Leiser gehalten werden
bool gedreht = false;       // Bildschirm und Knoepfe um 180 Grad gedreht
int buchSeite = 0;
int blattNach = 0;          // Ziel beim Umblaettern
bool blattVor = true;       // Richtung: vorwaerts (rechte Seite klappt nach links)
uint32_t blattSeit = 0;     // 0 = blaettert gerade nicht
uint32_t buchAuf = 0;       // Beginn des Aufklappens (0 = schon offen)
constexpr uint32_t BLATT_MS = 520, AUF_MS = 1750;
uint32_t bildMs = 0;        // Uhr fuer das ganze Bild (alle Streifen gleich)
volatile int8_t buchLied = -1;  // Seite, deren Melodie im Buch leise spielt (-1 = keine)
volatile uint8_t uiKlang = 0;  // 1 = umblaettern, 2 = aufklappen, 3 = los
int lautstaerke = 7;        // 0..10
constexpr int MAXLAUT = 10;
uint32_t lautGezeigt = 0;   // wann die Lautstaerke zuletzt geaendert wurde (Anzeige)
uint32_t letzteTaste = 0;
uint32_t zustandSeit = 0;
int updateProzent = -1;
Preferences prefs;
Gfx g;
float fps = 0;
String protokoll;  // die letzten Tastendruecke (fuer /status)
uint32_t zeichenUs = 0, wartUs = 0;  // letzte Bildzeit: zeichnen / auf DMA warten

// ---------- Audio (eigene Aufgabe) ----------
Synth synth;
volatile bool audioLaeuft = false;
volatile bool anfStart = false, anfStopp = false;
volatile bool pausiert = false;     // Film angehalten (Start druecken = weiter)
float pauseZeit = 0;               // Filmzeit, bei der angehalten wurde
uint32_t pauseSeit = 0;
volatile bool maerchenEnde = false;
volatile const Maerchen *audioMaerchen = nullptr;
volatile float audioZeit = 0;       // Maerchenzeit am Ende des zuletzt erzeugten Blocks
volatile float filmTempo = 1;       // 1 = normal; kleiner, solange eine Sprechblase zu lesen ist
volatile uint8_t blaseLesbar = 0;   // von den Sprechblasen gesetzt (beim Zeichnen)
volatile bool blaseOffen = false;   // im letzten Bild war eine Sprechblase lesbar
constexpr int LESEN_STUFEN = 3;
static const float LESE_TEMPO[LESEN_STUFEN] = {1.0f, 0.5f, 0.33f};
static const char *const LESE_TEXT[LESEN_STUFEN] = {"Lesen: normal", "Lesen: langsam", "Lesen: sehr langsam"};
int lesen = 1;              // ab Werk: langsam
constexpr int HELL_STUFEN = 4;
static const float HELL_PWM[HELL_STUFEN] = {0.10f, 0.28f, 0.58f, 1.0f};  // wirkt wie 25 / 50 / 75 / 100 %
static const char *const HELL_TEXT[HELL_STUFEN] = {"Helligkeit: 25 %", "Helligkeit: 50 %", "Helligkeit: 75 %",
                                                    "Helligkeit: 100 %"};
int hell = 1;               // ab Werk 50 %
// Nutzungszeit: ausserhalb von [von, bis) schlaeft die Box; wecken nur mit Lauter + Leiser 3 s
bool zeitAn = true;
int vonMin = 6 * 60, bisMin = 19 * 60;  // Minuten nach Mitternacht
int zeitFeld = -1;                      // Bearbeiten: -1 nein, 0 an/aus, 1 von, 2 bis
bool ueberbrueckt = false;              // Eltern haben ausserhalb der Zeit geweckt
uint32_t wachSeit = 0;                  // ... und zwar dann
constexpr uint32_t WACH_MS = 60UL * 60 * 1000;  // so lange bleibt sie dann wach

// Minuten nach Mitternacht (Ortszeit) oder -1, solange die Uhrzeit noch nicht aus dem Netz kam
static int uhrMinuten() {
  time_t jetzt = time(nullptr);
  if (jetzt < 1700000000) return -1;
  struct tm lt;
  localtime_r(&jetzt, &lt);
  return lt.tm_hour * 60 + lt.tm_min;
}
static bool nutzungErlaubt() {
  int m = uhrMinuten();
  if (!zeitAn || m < 0) return true;  // ohne Uhrzeit keine Sperre
  if (vonMin <= bisMin) return m >= vonMin && m < bisMin;
  return m >= vonMin || m < bisMin;  // ueber Mitternacht
}
static void zeitText(char *b, int n, int m) { snprintf(b, n, "%02d:%02d", m / 60, m % 60); }

// ---------- Akku ----------
constexpr int PIN_AKKU = 17;   // Akkuspannung ueber Spannungsteiler 1:2 (ADC2 Kanal 6)
constexpr int PIN_LADEN = 47;  // Ladestatus
float akkuMv = 0;              // geglaettet, 0 = noch kein Messwert
int akkuRohMv = 0, ladenRoh = -1;
bool akkuLaedt = false;
constexpr bool LADEN_BEI_LOW = true;  // Ladeanzeige-Pin des Laderegler-ICs ist meist "low = laedt"
static void akkuMessen() {
  int summe = 0, n = 0;
  for (int i = 0; i < 8; i++) {
    int mv = analogReadMilliVolts(PIN_AKKU);  // ADC2 kann waehrend WLAN-Betrieb ausfallen: 0 verwerfen
    if (mv > 300) { summe += mv; n++; }
  }
  if (n) {
    akkuRohMv = summe / n;
    float v = akkuRohMv * 2.0f;
    akkuMv = akkuMv <= 0 ? v : akkuMv * 0.85f + v * 0.15f;
  }
  ladenRoh = digitalRead(PIN_LADEN);
  akkuLaedt = LADEN_BEI_LOW ? ladenRoh == LOW : ladenRoh == HIGH;
}
// Ladezustand in Prozent aus der Spannung (Li-Ion-Kurve, grob)
static int akkuProzent() {
  static const float V[] = {3400, 3500, 3650, 3750, 3850, 4000, 4150};
  static const float P[] = {0, 5, 20, 40, 60, 80, 100};
  if (akkuMv <= 0) return -1;
  if (akkuMv <= V[0]) return 0;
  for (int i = 1; i < 7; i++)
    if (akkuMv < V[i]) return (int)(P[i - 1] + (P[i] - P[i - 1]) * (akkuMv - V[i - 1]) / (V[i] - V[i - 1]));
  return 100;
}
volatile uint32_t audioZeitUs = 0;  // wann das war
constexpr int BLOCK = 256;
constexpr float LATENZ = 8 * 256 / (float)Synth::RATE;  // DMA-Puffer zwischen Erzeugen und Hoeren

// =====================================================================
// ES8311
// =====================================================================
static bool esWrite(uint8_t reg, uint8_t val) {
  Wire.beginTransmission(ES8311_ADDR);
  Wire.write(reg);
  Wire.write(val);
  return Wire.endTransmission() == 0;
}
static uint8_t esRead(uint8_t reg) {
  Wire.beginTransmission(ES8311_ADDR);
  Wire.write(reg);
  Wire.endTransmission(false);
  Wire.requestFrom((int)ES8311_ADDR, 1);
  return Wire.available() ? Wire.read() : 0;
}
// Taktwerte aus der Tabelle des ES8311-Treibers von Espressif (MCLK 6,144 MHz, 24 kHz)
static bool es8311Init() {
  if (!esWrite(0x00, 0x1F)) return false;
  delay(20);
  esWrite(0x00, 0x00);
  esWrite(0x00, 0x80);
  esWrite(0x01, 0x3F);
  esWrite(0x06, esRead(0x06) & ~(1 << 5));
  esWrite(0x02, (esRead(0x02) & 0x07));
  esWrite(0x03, 0x10);
  esWrite(0x04, 0x10);
  esWrite(0x05, 0x00);
  esWrite(0x06, (esRead(0x06) & 0xE0) | 3);
  esWrite(0x07, (esRead(0x07) & 0xC0));
  esWrite(0x08, 0xFF);
  esWrite(0x00, esRead(0x00) & 0xBF);
  esWrite(0x09, 3 << 2);
  esWrite(0x0A, 3 << 2);
  esWrite(0x0D, 0x01);
  esWrite(0x0E, 0x02);
  esWrite(0x12, 0x00);
  esWrite(0x13, 0x10);
  esWrite(0x1C, 0x6A);
  esWrite(0x37, 0x08);
  esWrite(0x31, 0x00);
  return true;
}
static void setzeLautstaerke(bool merken) {
  // 0xBF = 0 dB, je Registerschritt 0,5 dB; oberste Kinder-Stufe = Hoechstlautstaerke, je Stufe 4 dB leiser
  int r = 0xBF + GRENZE_DB[grenze] * 2 - (MAXLAUT - lautstaerke) * 8;
  uint8_t reg = lautstaerke == 0 ? 0 : (uint8_t)constrain(r, 1, 0xBF);
  esWrite(0x32, reg);
  if (merken) { prefs.begin("box", false); prefs.putInt("laut", lautstaerke); prefs.end(); }
}

static void i2sInit() {
  i2s_config_t c = {};
  c.mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX);
  c.sample_rate = Synth::RATE;
  c.bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT;
  c.channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT;
  c.communication_format = I2S_COMM_FORMAT_STAND_I2S;
  c.dma_buf_count = 8;
  c.dma_buf_len = BLOCK;
  c.use_apll = true;
  c.tx_desc_auto_clear = true;
  c.mclk_multiple = I2S_MCLK_MULTIPLE_256;
  c.bits_per_chan = I2S_BITS_PER_CHAN_16BIT;
  i2s_driver_install(I2S_NUM_0, &c, 0, nullptr);
  i2s_pin_config_t p = {};
  p.mck_io_num = PIN_MCLK;
  p.bck_io_num = PIN_BCLK;
  p.ws_io_num = PIN_WS;
  p.data_out_num = PIN_DOUT;
  p.data_in_num = I2S_PIN_NO_CHANGE;
  i2s_set_pin(I2S_NUM_0, &p);
  i2s_zero_dma_buffer(I2S_NUM_0);
}

static void audioTask(void *) {
  static int16_t mono[BLOCK];
  static int16_t stereo[BLOCK * 2];
  for (;;) {
    if (anfStopp) {
      anfStopp = false;
      pausiert = false;
      audioLaeuft = false;
      synth.stille();
      i2s_zero_dma_buffer(I2S_NUM_0);
      digitalWrite(PIN_PA, LOW);
    }
    static bool warPause = false;
    if (audioLaeuft && pausiert) {
      if (!warPause) { warPause = true; synth.stille(); i2s_zero_dma_buffer(I2S_NUM_0); digitalWrite(PIN_PA, LOW); }
      vTaskDelay(pdMS_TO_TICKS(10));
      continue;
    }
    if (warPause) { warPause = false; if (audioLaeuft) { digitalWrite(PIN_PA, HIGH); audioZeitUs = micros(); } }
    if (anfStart) {
      anfStart = false;
      pausiert = false;
      synth.start();
      synth.abbildungAus();
      filmTempo = 1;
      audioZeit = 0;
      audioZeitUs = micros();
      digitalWrite(PIN_PA, HIGH);
      audioLaeuft = true;
    }
    static bool uiAktiv = false;
    static float uiBis = 0;
    if (!audioLaeuft) {
      synth.abbildungAus();
      uint8_t k = uiKlang;
      if (k) {
        uiKlang = 0;
        if (!uiAktiv) { synth.start(); digitalWrite(PIN_PA, HIGH); uiAktiv = true; }
        float t = synth.zeit() + 0.01f;
        if (k == 5 || k == 6) {  // Tschuuu: Rauschen gleitet, ein leiser Ton schwingt mit
          bool hoch = k == 5;
          synth.rauschen(t, 0.3f, BANDPASS, hoch ? 2800 : 900, 0.09f, 1.3f, 0.07f, hoch ? 800 : 3000);
          synth.ton(t + 0.02f, hoch ? 700 : 420, 0.022f, 0.03f, 0.22f, SINUS, MUSIK, hoch ? 420 : 700);
        } else if (k == 1) {  // Seite raschelt
          synth.rauschen(t, 0.24f, BANDPASS, 1400, 0.10f, 0.9f, 0.05f, 3600);
          synth.note(t + 0.18f, 86, 0.3f);
        } else if (k == 4) {  // Gute Nacht: Wiegenlied-Anfang
          const int m[9] = {72, 69, 72, 74, 72, 69, 70, 67, 65};
          const float d[9] = {0, 0.9f, 1.2f, 2.4f, 3.0f, 3.6f, 4.8f, 5.4f, 6.0f};
          for (int i = 0; i < 9; i++) synth.note(t + d[i], m[i], 0.45f);
          synth.glocke(t + 7.2f, 60, 0.3f);
        } else if (k == 2) {  // Buch klappt auf
          const int ak[5] = {0, 4, 7, 12, 16};
          for (int i = 0; i < 5; i++) synth.glocke(t + i * 0.16f, 72 + ak[i], 0.6f);
        } else {  // los geht's
          synth.note(t, 79, 0.7f); synth.note(t + 0.1f, 84, 0.7f); synth.note(t + 0.2f, 88, 0.7f);
        }
        uiBis = synth.zeit() + (k == 4 ? 10.5f : 2.6f);
      }
      // leise Erkennungsmelodie der offenen Seite
      static int liedAktiv = -1;
      static float liedStart = 0;
      int bl = buchLied;
      if (bl != liedAktiv) {
        liedAktiv = bl;
        if (bl >= 0) {
          if (!uiAktiv) { synth.start(); digitalWrite(PIN_PA, HIGH); uiAktiv = true; }
          liedStart = synth.zeit() + 0.7f;
        }
      }
      if (liedAktiv >= 0 && uiAktiv && LISTE[liedAktiv]->lied) {
        const Maerchen *lm = LISTE[liedAktiv];
        float t0 = synth.zeit(), t1 = t0 + BLOCK / (float)Synth::RATE;
        float runde = 0;
        for (int i = 0; i < lm->liedN; i++) runde += lm->lied[i].laenge;
        float ende = liedStart + 2 * runde * lm->liedSchlag;  // zweimal, dann ist Ruhe
        if (t0 < ende) {
          spieleMelodie(synth, Melodie{lm->lied, lm->liedN}, liedStart, ende, lm->liedSchlag, t0, t1, 0.4f);
          if (uiBis < t1 + 2.5f) uiBis = t1 + 2.5f;
        }
      }
      if (!uiAktiv) { vTaskDelay(pdMS_TO_TICKS(5)); continue; }
      synth.render(mono, BLOCK);
      for (int i = 0; i < BLOCK; i++) stereo[2 * i] = stereo[2 * i + 1] = mono[i];
      size_t n;
      i2s_write(I2S_NUM_0, stereo, sizeof stereo, &n, portMAX_DELAY);
      if (synth.zeit() > uiBis) { uiAktiv = false; synth.stille(); digitalWrite(PIN_PA, LOW); }
      continue;
    }
    uiAktiv = false;
    const Maerchen *m = (const Maerchen *)audioMaerchen;
    // Zeitlupe beim Lesen: das Tempo gleitet sanft zum Ziel
    float ziel = blaseOffen ? LESE_TEMPO[lesen] : 1.0f;
    float tempo = filmTempo + (ziel - filmTempo) * 0.06f;
    if (fabsf(tempo - ziel) < 0.002f) tempo = ziel;
    filmTempo = tempo;
    float s0 = synth.zeit(), dt = BLOCK / (float)Synth::RATE;
    float t0 = audioZeit, t1 = t0 + dt * tempo;  // Filmzeit
    synth.abbildung(t0, s0, tempo);
    m->ton(synth, t0, t1);
    synth.render(mono, BLOCK);
    for (int i = 0; i < BLOCK; i++) stereo[2 * i] = stereo[2 * i + 1] = mono[i];
    size_t n;
    i2s_write(I2S_NUM_0, stereo, sizeof stereo, &n, portMAX_DELAY);
    audioZeit = t1;
    audioZeitUs = micros();
    if (t0 > m->dauer + 1) { audioLaeuft = false; maerchenEnde = true; digitalWrite(PIN_PA, LOW); }
  }
}

// Zeit im Maerchen, die gerade zu hoeren ist (fuer das Bild)
static float hoerZeit() {
  float tempo = filmTempo;
  float t = audioZeit + (micros() - audioZeitUs) / 1e6f * tempo;
  float max = audioZeit + 0.03f * tempo;
  if (t > max) t = max;
  t -= LATENZ * tempo;
  return t < 0 ? 0 : t;
}

// =====================================================================
// Bildschirme
// =====================================================================
static const Farbe GOLD = FARBE(0xFFD54A), DUNKEL = FARBE(0x3A2418), WEISS = FARBE(0xFFFFFF);

// ---------- Das Maerchenbuch ----------
static const Farbe PAPIER = FARBE(0xFBF1DC), PAPIER_D = FARBE(0xE2CDA4), LEDER = FARBE(0x8E2F3A),
                   TINTE = FARBE(0x5A3418), GOLD_D = FARBE(0xB8862B);
constexpr float BX = 14, BY = 20, BW = 128, BH = 202, MITTE = 142;  // Seitenmasse

static const Farbe NACHT = FARBE(0x1B1846), NACHT_D = FARBE(0x0D0B26), SAMT = FARBE(0x7A1F3A), SAMT_D = FARBE(0x4E1026),
                   BLATT = FARBE(0x6E9A4E), ROSE = FARBE(0xC8485E);

// Nachthimmel im Maerchenwald: Mond, funkelnde Sterne, Tannen am Rand
static void buchTisch(Gfx &g, float t) {
  g.verlauf(0, 240, NACHT, NACHT_D);
  g.kreis(248, 22, 30, FARBE(0xFFF1B0), 22);
  g.kreis(248, 22, 13, FARBE(0xFFF1B0));
  g.kreis(254, 18, 11, NACHT);
  for (int i = 0; i < 22; i++) {
    float fu = 0.5f + 0.5f * sinf(t * 2.2f + i * 1.7f);
    g.stern(zufall(i + 11) * 284, zufall(i + 61) * 240, 1.2f + fu * 1.8f, 0.6f, 4, 0, i % 3 ? WEISS : GOLD,
            (uint8_t)(60 + fu * 170));
  }
  for (int i = 0; i < 9; i++) {  // dunkle Tannen unten am Rand
    float x = i * 34 - 6, h = 34 + zufall(i + 5) * 22;
    float tan[] = {x - 16, 240, x, 240 - h, x + 16, 240};
    g.polygon(tan, 3, FARBE(0x0F2A2E));
  }
}

// beim offenen Buch: Himmel nur in den schmalen Raendern, Einband nur als Rand (die Seiten liegen darueber)
static void buchRahmenOffen(Gfx &g, float t) {
  const float x = 6, y = BY - 6, w = 272, h = BH + 12;
  g.rechteck(0, 0, 284, y, NACHT);
  g.rechteck(0, y + h, 284, 240 - y - h, NACHT_D);
  g.rechteck(0, y, x, h, NACHT);
  g.rechteck(x + w, y, 284 - x - w, h, NACHT);
  for (int i = 0; i < 22; i++) {
    float sx = zufall(i + 11) * 284, sy = zufall(i + 61) * 240;
    if (sx > x - 2 && sx < x + w + 2 && sy > y - 2 && sy < y + h + 2) continue;
    float fu = 0.5f + 0.5f * sinf(t * 2.2f + i * 1.7f);
    g.stern(sx, sy, 1.2f + fu * 1.8f, 0.6f, 4, 0, i % 3 ? WEISS : GOLD, (uint8_t)(60 + fu * 170));
  }
  // Samtrand mit Goldlinien
  g.rechteck(x, y, w, 8, SAMT);
  g.rechteck(x, y + h - 8, w, 8, SAMT);
  g.rechteck(x, y, 8, h, SAMT);
  g.rechteck(x + w - 8, y, 8, h, SAMT);
  g.rechteck(x + 4, y + 4, w - 8, 1.5f, GOLD_D);
  g.rechteck(x + 4, y + h - 5.5f, w - 8, 1.5f, GOLD_D);
  g.rechteck(x + 4, y + 4, 1.5f, h - 8, GOLD_D);
  g.rechteck(x + w - 5.5f, y + 4, 1.5f, h - 8, GOLD_D);
  for (int i = 0; i < 4; i++) {
    float sx = i & 1 ? -1 : 1, sy = i & 2 ? -1 : 1;
    float ex = i & 1 ? x + w : x, ey = i & 2 ? y + h : y;
    float b[] = {ex, ey, ex + sx * 22, ey, ex, ey + sy * 22};
    g.kontur(1, FARBE(0x7A5418));
    g.polygon(b, 3, GOLD);
    g.konturAus();
    g.kreis(ex + sx * 6, ey + sy * 6, 2.4f, SAMT_D);
    g.kreis(ex + sx * 6, ey + sy * 6, 1.1f, GOLD);
  }
}

// Gluehwuermchen und Zauberstaub, die ueber dem Buch schweben (zuletzt zeichnen)
static void zauberstaub(Gfx &g, float t) {
  for (int i = 0; i < 9; i++) {
    float ph = t * (0.05f + zufall(i + 300) * 0.05f) + zufall(i + 310);
    float x = fmodf(zufall(i + 320) * 284 + sinf(ph * 9 + i) * 26 + 284, 284);
    float y = fmodf(240 - fmodf(ph, 1) * 260 + 260, 260) - 10;
    float fu = 0.5f + 0.5f * sinf(t * 5 + i * 2.3f);
    g.kreis(x, y, 4 + fu * 2, FARBE(0xFFE9A0), (uint8_t)(30 + 30 * fu));
    g.stern(x, y, 2 + fu * 1.5f, 0.8f, 4, t + i, FARBE(0xFFF6C8), (uint8_t)(120 + 120 * fu));
  }
}

static void einband(Gfx &g, float x, float y, float w, float h) {
  g.kontur(1.5f, FARBE(0x220812));
  g.rundRechteck(x, y, w, h, 10, SAMT);
  g.konturAus();
  g.rundRechteck(x + 2, y + 2, w - 4, 6, 4, FARBE(0x9A3550), 120);  // Glanz oben
  // doppelte Goldlinie
  g.rundRechteck(x + 4, y + 4, w - 8, h - 8, 8, GOLD_D);
  g.rundRechteck(x + 5.5f, y + 5.5f, w - 11, h - 11, 7, SAMT);
  g.rundRechteck(x + 7, y + 7, w - 14, h - 14, 6, GOLD_D, 140);
  g.rundRechteck(x + 8, y + 8, w - 16, h - 16, 5, SAMT);
  // goldene Eckbeschlaege mit Ranke
  for (int i = 0; i < 4; i++) {
    float sx = i & 1 ? -1 : 1, sy = i & 2 ? -1 : 1;
    float ex = i & 1 ? x + w - 1 : x + 1, ey = i & 2 ? y + h - 1 : y + 1;
    float b[] = {ex, ey, ex + sx * 22, ey, ex, ey + sy * 22};
    g.kontur(1, FARBE(0x7A5418));
    g.polygon(b, 3, GOLD);
    g.konturAus();
    g.kreis(ex + sx * 7, ey + sy * 7, 2.6f, SAMT_D);
    g.kreis(ex + sx * 7, ey + sy * 7, 1.2f, GOLD);
  }
}

// kleine Rankenecke: Linien, Kringel, Blaetter, eine Rosenknospe
static void rankenEcke(Gfx &g, float x, float y, float sx, float sy) {
  g.linie(x, y, x + sx * 26, y, 1.2f, GOLD_D);
  g.linie(x, y, x, y + sy * 26, 1.2f, GOLD_D);
  g.kreis(x + sx * 29, y + sy * 2.5f, 2.2f, GOLD_D);
  g.kreis(x + sx * 2.5f, y + sy * 29, 2.2f, GOLD_D);
  g.ellipse(x + sx * 13, y + sy * 3.5f, 4, 1.8f, BLATT, 255, sx * sy * 0.5f);
  g.ellipse(x + sx * 3.5f, y + sy * 13, 1.8f, 4, BLATT, 255, sx * sy * 0.5f);
  g.ellipse(x + sx * 20, y - sy * 2.5f, 3, 1.5f, BLATT, 255, -sx * sy * 0.5f);
  g.kreis(x + sx * 5, y + sy * 5, 3.4f, ROSE);
  g.kreis(x + sx * 5, y + sy * 5, 1.4f, FARBE(0xF2A0B0));
}

static void seitenGrund(Gfx &g, bool links) {
  float x = links ? BX : MITTE;
  g.rechteck(x, BY, BW, BH, PAPIER);
  // altes Pergament: leichte Flecken, dunklere Raender
  for (int i = 0; i < 2; i++)
    g.ellipse(x + 14 + zufall(i + (links ? 400 : 420)) * (BW - 28), BY + 14 + zufall(i + 440) * (BH - 28),
              14 + zufall(i + 460) * 10, 10 + zufall(i + 480) * 8, PAPIER_D, 45);
  g.rechteck(x, BY, BW, 4, PAPIER_D, 110);
  g.rechteck(x, BY + BH - 4, BW, 4, PAPIER_D, 110);
  g.rechteck(links ? x : x + BW - 4, BY, 4, BH, PAPIER_D, 110);
  // zur Buchmitte hin dunkler (Woelbung)
  g.rechteck(links ? MITTE - 22 : MITTE, BY, 22, BH, PAPIER_D, 70);
  g.rechteck(links ? MITTE - 8 : MITTE, BY, 8, BH, PAPIER_D, 150);
  // Rankenrahmen an den aeusseren Ecken
  float ax = links ? x + 6 : x + BW - 6;
  float s = links ? 1 : -1;
  rankenEcke(g, ax, BY + 6, s, 1);
  rankenEcke(g, ax, BY + BH - 6, s, -1);
  // Seitenkanten unten (mehrere Blaetter)
  g.rechteck(x + 2, BY + BH, BW - 4, 1.2f, FARBE(0xD9C49A));
  g.rechteck(x + 4, BY + BH + 1.6f, BW - 8, 1.2f, FARBE(0xCDB68A));
}

// Schnoerkel-Trennlinie: zwei Ranken und ein Herz in der Mitte
static void schnoerkel(Gfx &g, float cx, float y, float b, Farbe c) {
  g.linie(cx - b, y, cx - 7, y, 1.2f, c);
  g.linie(cx + 7, y, cx + b, y, 1.2f, c);
  g.kreis(cx - b - 2, y, 2.2f, c);
  g.kreis(cx + b + 2, y, 2.2f, c);
  g.ellipse(cx - b * 0.55f, y - 3, 3.4f, 1.5f, BLATT, 255, 0.5f);
  g.ellipse(cx + b * 0.55f, y - 3, 3.4f, 1.5f, BLATT, 255, -0.5f);
  g.kreis(cx - 2, y - 1.5f, 2.6f, ROSE);
  g.kreis(cx + 2, y - 1.5f, 2.6f, ROSE);
  float h[] = {cx - 4.4f, y - 0.5f, cx + 4.4f, y - 0.5f, cx, y + 4.5f};
  g.polygon(h, 3, ROSE);
}

// Titel auf mehrere Zeilen verteilen (Woerter, max. Breite)
static int zeilen(Gfx &g, const char *titel, char out[4][40], float maxB, const Schrift &sch = SCHRIFT_B,
                  bool *passt = nullptr) {
  if (passt) *passt = true;
  int n = 0;
  out[0][0] = 0;
  const char *p = titel;
  while (*p) {
    const char *e = p;
    while (*e && *e != ' ') e++;
    char wort[40];
    snprintf(wort, sizeof wort, "%.*s", (int)(e - p), p);
    char probe[80];
    snprintf(probe, sizeof probe, "%s%s%s", out[n], out[n][0] ? " " : "", wort);
    if (passt && g.textBreite(sch, wort) > maxB) *passt = false;
    if (out[n][0] && g.textBreite(sch, probe) > maxB) {
      if (n == 3) { if (passt) *passt = false; break; }
      n++;
      snprintf(out[n], 40, "%s", wort);
    } else {
      snprintf(out[n], 40, "%s", probe);
    }
    p = *e ? e + 1 : e;
  }
  return n + 1;
}

static void startKnopf(Gfx &g, float x, float y, float t, float r = 15) {
  float b = 1 + sinf(t * 4) * 0.06f;
  // Strahlenkranz und goldenes Siegel mit rotem Herz in der Mitte
  for (int i = 0; i < 12; i++) {
    float a = i * 0.5236f + t * 0.4f;
    g.linie(x + cosf(a) * r * 1.05f * b, y + sinf(a) * r * 1.05f * b, x + cosf(a) * r * 1.4f * b, y + sinf(a) * r * 1.4f * b,
            1.4f, GOLD, (uint8_t)(110 + 80 * sinf(t * 3 + i)));
  }
  g.kontur(1.4f, FARBE(0x7A5418));
  g.stern(x, y, r * 1.08f * b, r * 0.92f * b, 16, t * 0.2f, GOLD);
  g.konturAus();
  g.kreis(x, y, r * 0.8f * b, FARBE(0xC0392B));
  g.bogen(x, y, r * 0.8f * b, 0, 6.29f, 1.2f, FARBE(0x8E1F1F));
  g.kreis(x - r * 0.28f, y - r * 0.32f, r * 0.24f, WEISS, 80);
  float tri[] = {x - r * 0.26f, y - r * 0.42f, x - r * 0.26f, y + r * 0.42f, x + r * 0.46f, y};
  g.polygon(tri, 3, FARBE(0xFFF3C0));
}

// goldener Bogenrahmen um das Bild (oben rund wie ein Maerchenfenster); deckt die Ecken mit Pergament ab
static void bogenFenster(Gfx &g, float x, float y, float w, float h, float t, Farbe grund = PAPIER) {
  const int N = 10;
  float cx = x + w / 2, rx = w / 2, ry = 26, by = y + ry;  // Bogen: Ellipse um (cx, by)
  float bx[N + 1], byy[N + 1];
  for (int i = 0; i <= N; i++) {
    float a = 3.14159f + i * 3.14159f / N;
    bx[i] = cx + cosf(a) * rx;
    byy[i] = by + sinf(a) * ry;
  }
  // Ecken ausserhalb des Bogens mit Pergament fuellen
  float l[2 * (N / 2 + 3)], r[2 * (N / 2 + 3)];
  int nl = 0, nr = 0;
  l[nl++] = x - 1; l[nl++] = y - 1;
  for (int i = 0; i <= N / 2; i++) { l[nl++] = bx[i]; l[nl++] = byy[i] - 0.5f; }
  r[nr++] = x + w + 1; r[nr++] = y - 1;
  for (int i = N; i >= N / 2; i--) { r[nr++] = bx[i]; r[nr++] = byy[i] - 0.5f; }
  g.polygon(l, nl / 2, grund);
  g.polygon(r, nr / 2, grund);
  // der Rahmen: dunkle Kante und Gold, der Bogen jeweils als ein geschlossenes Band
  for (int k = 0; k < 2; k++) {
    Farbe c = k == 0 ? FARBE(0x7A5418) : GOLD;
    float d = k == 0 ? 2.6f : 1.6f;  // halbe Breite
    float band[4 * (N + 1)];
    for (int i = 0; i <= N; i++) {
      float a = 3.14159f + i * 3.14159f / N, ca = cosf(a), sa = sinf(a);
      band[2 * i] = cx + ca * (rx + d); band[2 * i + 1] = by + sa * (ry + d);
      band[4 * (N + 1) - 2 - 2 * i] = cx + ca * (rx - d); band[4 * (N + 1) - 1 - 2 * i] = by + sa * (ry - d);
    }
    g.polygon(band, 2 * (N + 1), c);
    g.rechteck(x - d, by, 2 * d, y + h - by + d, c);
    g.rechteck(x + w - d, by, 2 * d, y + h - by + d, c);
    g.rechteck(x - d, y + h - d, w + 2 * d, 2 * d, c);
  }
  // Schlussstein mit Stern und funkelnde Ecken
  g.kontur(1, FARBE(0x7A5418));
  g.kreis(cx, y + 1, 6, GOLD);
  g.konturAus();
  g.stern(cx, y + 1, 4.5f, 1.8f, 5, -1.5708f, FARBE(0xC0392B));
  float fu = 0.5f + 0.5f * sinf(t * 3);
  g.stern(x + 3, y + h - 2, 4 + fu * 2, 1.3f, 4, 0, WEISS, (uint8_t)(150 + 100 * fu));
  g.stern(x + w - 3, y + h - 2, 6 - fu * 2, 1.3f, 4, 0, WEISS, (uint8_t)(250 - 100 * fu));
}

static void goldRahmen(Gfx &g, float x, float y, float w, float h, float d, Farbe c, uint8_t a = 255) {
  g.rechteck(x, y, w, d, c, a);
  g.rechteck(x, y + h - d, w, d, c, a);
  g.rechteck(x, y, d, h, c, a);
  g.rechteck(x + w - d, y, d, h, c, a);
}

// die linke Seite als Buchcover des Maerchens: Leinen in der Farbe des Maerchens, Goldtitel, Bild, Autor
static void cover(Gfx &g, const Maerchen *m, float t) {
  const float x = 16, y = 22, w = 124, h = 198;
  Farbe stoff = mischeFarbe(m->farbe, FARBE(0x1A1430), 0.62f);
  Farbe hell = mischeFarbe(stoff, WEISS, 0.14f), dunkel = mischeFarbe(stoff, FARBE(0x0A0610), 0.45f);
  g.kontur(1.2f, FARBE(0x1A0E10));
  g.rundRechteck(x, y, w, h, 4, stoff);
  g.konturAus();
  for (int i = 0; i < 7; i++) g.rechteck(x + 2, y + 9 + i * 28, w - 4, 0.8f, hell, 70);  // Leinen
  g.rechteck(x + w - 10, y, 10, h, dunkel, 110);  // Falz zur Buchmitte
  goldRahmen(g, x + 5, y + 5, w - 15, h - 10, 1.6f, GOLD_D);
  goldRahmen(g, x + 8, y + 8, w - 21, h - 16, 0.8f, GOLD, 170);
  for (int i = 0; i < 4; i++) {  // Eckornamente
    float ex = i & 1 ? x + w - 13 : x + 8, ey = i & 2 ? y + h - 8 : y + 8;
    g.stern(ex, ey, 5, 1.6f, 4, 0.785f, GOLD);
    g.kreis(ex, ey, 1.6f, FARBE(0xC0392B));
  }
  // Titel in Goldschrift
  float cx = x + (w - 5) / 2;
  char z[4][40];
  bool passt;
  const Schrift *sch = &SCHRIFT_B;
  float zh = 20;
  int n = zeilen(g, m->titel, z, w - 26, SCHRIFT_B, &passt);
  if (!passt || n > 3) { sch = &SCHRIFT_K; zh = 16; n = zeilen(g, m->titel, z, w - 26, SCHRIFT_K); }  // lange Titel kleiner
  float ty = y + (zh > 16 ? 30 : 27);
  for (int i = 0; i < n; i++) {
    g.text(*sch, z[i], cx + 1, ty + i * zh + 1, dunkel, 1);
    g.text(*sch, z[i], cx, ty + i * zh, GOLD, 1);
  }
  float ly = ty + (n - 1) * zh + 9;
  float fu = 0.5f + 0.5f * sinf(t * 2.4f);
  g.stern(cx, ly, 3.5f + fu, 1.4f, 4, 0, GOLD);
  g.linie(cx - 30, ly, cx - 7, ly, 1, GOLD_D);
  g.linie(cx + 7, ly, cx + 30, ly, 1, GOLD_D);
  // Bild im Bogenfenster
  float wx = x + 12, ww = w - 29, wy = ly + 9, wh = y + h - 30 - wy;
  float zm = fmaxf(wh / 240.0f, ww / 284.0f);
  float ox = wx + ww / 2 - 142 * zm, oy = wy + wh - 240 * zm;
  g.ansicht(ox, oy, zm);
  g.ausschnitt((wx - ox) / zm, (wy - oy) / zm, ww / zm, wh / zm);
  m->vorschau(g, t);
  g.ausschnittAus();
  g.kameraAus();
  bogenFenster(g, wx, wy, ww, wh, t, stoff);
  // Autor
  g.text(SCHRIFT_K, m->autor ? m->autor : "Brüder Grimm", cx, y + h - 13, FARBE(0xF2D98A), 1);
}

static void linkeSeite(Gfx &g, int seite, float t) {
  if (seite < ANZAHL) {
    g.rechteck(BX, BY, BW, BH, PAPIER);  // das Cover liegt fast ueber der ganzen Seite
    g.rechteck(BX + 2, BY + BH, BW - 4, 1.2f, FARBE(0xD9C49A));
    cover(g, LISTE[seite], t);
    return;
  }
  seitenGrund(g, true);
  // Rahmen fuer das Bild
  g.rundRechteck(23, 41, 110, 94, 6, GOLD_D);
  g.rundRechteck(25, 43, 106, 90, 5, GOLD);
  if (seite < ANZAHL) {
    g.ansicht(26, 44, 104.0f / 284);
    g.ausschnitt(0, 0, 284, 240);
    LISTE[seite]->vorschau(g, t);
    g.ausschnittAus();
    g.kameraAus();
  } else if (seite == SEITE_LAUT) {
    g.rundRechteck(26, 44, 104, 88, 4, FARBE(0xFFF6E4));
    float l[] = {40, 78, 50, 78, 64, 66, 64, 110, 50, 98, 40, 98};
    g.polygon(l, 6, TINTE);
    for (int i = 0; i < GRENZEN; i++) {
      float h = 14 + i * 10;
      g.rundRechteck(74 + i * 13, 110 - h, 9, h, 3, i <= grenze ? LEDER : PAPIER_D);
    }
    float w = 0.5f + 0.5f * sinf(t * 4);
    for (int i = 0; i <= grenze && i < 3; i++) g.bogen(64, 88, 10 + i * 8, -0.7f, 0.7f, 2, LEDER, (uint8_t)(120 + 135 * w));
  } else if (seite == SEITE_HELL) {
    g.rundRechteck(26, 44, 104, 88, 4, FARBE(0xFFF6E4));
    float k = (hell + 1) / (float)HELL_STUFEN;
    for (int i = 0; i < 12; i++) {
      float a = i * 0.5236f + t * 0.3f;
      g.linie(78 + cosf(a) * 20, 86 + sinf(a) * 20, 78 + cosf(a) * (22 + 16 * k), 86 + sinf(a) * (22 + 16 * k), 3, GOLD_D);
    }
    g.kontur(1.4f, TINTE);
    g.kreis(78, 86, 16, mischeFarbe(FARBE(0xB8A070), GOLD, k));
    g.konturAus();
    for (int i = 0; i < HELL_STUFEN; i++) g.rundRechteck(56 + i * 12, 122, 9, 5, 2, i <= hell ? LEDER : PAPIER_D);
  } else if (seite == SEITE_ZEIT) {
    g.rundRechteck(26, 44, 104, 88, 4, FARBE(0xFFF6E4));
    int m = uhrMinuten();
    float mx = 78, my = 82, r = 26;
    g.kontur(1.6f, TINTE);
    g.kreis(mx, my, r, WEISS);
    g.konturAus();
    for (int i = 0; i < 12; i++) g.kreis(mx + sinf(i * 0.5236f) * (r - 5), my - cosf(i * 0.5236f) * (r - 5), 1.3f, TINTE);
    // erlaubte Zeit als goldener Bogen am Rand (12-Stunden-Zifferblatt: nur grob)
    if (zeitAn) {
      float a0 = vonMin / 720.0f * 6.2832f - 1.5708f, a1 = bisMin / 720.0f * 6.2832f - 1.5708f;
      if (a1 <= a0) a1 += 6.2832f;
      if (bisMin - vonMin >= 720 || (bisMin < vonMin && bisMin + 1440 - vonMin >= 720)) { a0 = 0; a1 = 6.29f; }
      g.bogen(mx, my, r + 4, a0, a1, 3, GOLD_D);
    }
    if (m >= 0) {
      float ah = (m % 720) / 720.0f * 6.2832f, am = (m % 60) / 60.0f * 6.2832f;
      g.linie(mx, my, mx + sinf(ah) * r * 0.5f, my - cosf(ah) * r * 0.5f, 2.6f, TINTE);
      g.linie(mx, my, mx + sinf(am) * r * 0.78f, my - cosf(am) * r * 0.78f, 1.6f, TINTE);
      char b[24], z[8];
      zeitText(z, sizeof z, m);
      snprintf(b, sizeof b, "jetzt %s", z);
      g.text(SCHRIFT_K, b, mx, 128, TINTE, 1);
    } else {
      g.text(SCHRIFT_K, "Uhrzeit fehlt", mx, 128, LEDER, 1);
    }
    g.kreis(mx, my, 2.4f, LEDER);
    // Mond oben rechts
    g.kreis(116, 54, 7, FARBE(0xFFE9A0));
    g.kreis(119, 52, 6, FARBE(0xFFF6E4));
  } else if (seite == SEITE_LESEN) {
    g.rundRechteck(26, 44, 104, 88, 4, FARBE(0xFFF6E4));
    g.kontur(1.4f, TINTE);
    g.rundRechteck(36, 52, 70, 34, 9, WEISS);
    g.konturAus();
    float sp[] = {50, 85, 60, 85, 46, 96};
    g.polygon(sp, 3, WEISS);
    for (int i = 0; i < 2; i++) g.rundRechteck(44, 61 + i * 12, i ? 40 : 54, 4, 2, PAPIER_D);
    // Schnecke: je langsamer das Lesen, desto gemuetlicher kriecht sie
    float sx = 50 + fmodf(t * 10 * LESE_TEMPO[lesen], 56);
    g.ellipse(sx, 124, 18, 4, FARBE(0x9AB86A));
    g.kreis(sx + 16, 118, 4, FARBE(0x9AB86A));
    g.linie(sx + 16, 115, sx + 19, 107, 1.2f, FARBE(0x6E8A4A));
    g.kontur(1.2f, TINTE);
    g.kreis(sx - 2, 113, 10, FARBE(0xC98A4A));
    g.konturAus();
    g.bogen(sx - 2, 113, 5, 0, 5, 1.6f, LEDER);
  } else if (seite == SEITE_DREHEN) {
    // kleiner Bildschirm, der sich dreht
    g.rundRechteck(26, 44, 104, 88, 4, FARBE(0xFFF6E4));
    float w = sanft(fmodf(t * 0.4f, 1) * 1.6f) * 3.14159265f;
    float c = cosf(w), s = sinf(w), mx = 78, my = 88;
    auto R = [&](float x, float y, float *o) { o[0] = mx + x * c - y * s; o[1] = my + x * s + y * c; };
    float q[8];
    R(-30, -20, q); R(30, -20, q + 2); R(30, 20, q + 4); R(-30, 20, q + 6);
    g.polygon(q, 4, TINTE);
    R(-26, -16, q); R(26, -16, q + 2); R(26, 16, q + 4); R(-26, 16, q + 6);
    g.polygon(q, 4, FARBE(0x8FD3FF));
    float sx[2], sy[2];
    R(-8, -2, sx); R(8, 4, sy);
    g.kreis(sx[0], sx[1], 5, GOLD);  // Sonne oben links im Bildchen
    g.kreis(sy[0], sy[1], 4, FARBE(0x3DAE5B));
    g.bogen(mx, my, 40, -2.6f, -0.6f, 3, LEDER);
    float pf[] = {mx + 40 * cosf(-0.6f) - 5, my + 40 * sinf(-0.6f) - 6, mx + 40 * cosf(-0.6f) + 7, my + 40 * sinf(-0.6f) - 2,
                  mx + 40 * cosf(-0.6f) - 1, my + 40 * sinf(-0.6f) + 8};
    g.polygon(pf, 3, LEDER);
  } else {
    g.rundRechteck(26, 44, 104, 88, 4, FARBE(0x2B2A6B));
    g.kreis(104, 66, 11, FARBE(0xFFF1B0));
    g.kreis(110, 62, 10, FARBE(0x2B2A6B));
    for (int i = 0; i < 9; i++) {
      float fu = 0.5f + 0.5f * sinf(t * 3 + i * 2.1f);
      g.stern(34 + zufall(i + 81) * 60, 52 + zufall(i + 91) * 70, 2 + fu * 1.5f, 1, 4, 0, GOLD, (uint8_t)(120 + fu * 130));
    }
  }
  // Verzierung unter dem Bild
  g.rechteck(36, 154, 84, 1.5f, GOLD_D);
  g.stern(78, 155, 5, 2, 4, 0, GOLD_D);
  for (int i = 0; i < 3; i++) g.kreis(56 + i * 22, 176, 2.5f, i == 1 ? LEDER : GOLD_D);
}

static void rechteSeite(Gfx &g, int seite, float t) {
  seitenGrund(g, false);
  const float cx = MITTE + BW / 2;
  if (seite < ANZAHL) {
    const Maerchen *m = LISTE[seite];
    g.text(SCHRIFT_K, "Es war einmal", cx, 44, FARBE(0x9A7650), 1);
    schnoerkel(g, cx, 54, 34, GOLD_D);
    // grosses Erkennungszeichen (fuer Kinder, die noch nicht lesen)
    g.ausschnitt(MITTE + 6, 60, BW - 12, 98);
    if (m->symbol) m->symbol(g, cx, 152, t);
    g.ausschnittAus();
    g.kameraAus();
    schnoerkel(g, cx, 160, 30, GOLD_D);
    char d[24];
    snprintf(d, sizeof d, "%d Minuten", (int)(m->dauer / 60 + 0.5f));
    g.text(SCHRIFT_K, d, cx, 177, FARBE(0x9A7650), 1);
    startKnopf(g, cx, 196, t, 11);
  }
  // Seitenpunkte
  if (SEITEN <= 12) {
    for (int i = 0; i < SEITEN; i++) {
      float px = cx + (i - (SEITEN - 1) / 2.0f) * 10;
      if (i == seite) g.stern(px, 215, 4.2f, 1.8f, 5, -1.5708f, GOLD_D);
      else g.kreis(px, 215, 1.8f, PAPIER_D);
    }
  } else if (seite < ANZAHL) {  // viele Maerchen: "3 / 20" mit Sternchen
    char b[12];
    snprintf(b, sizeof b, "%d / %d", seite + 1, SEITEN);
    g.text(SCHRIFT_K, b, cx, 219, FARBE(0xB8A078), 1);
    g.stern(cx - 26, 214, 3, 1.2f, 5, -1.5708f, GOLD_D);
    g.stern(cx + 26, 214, 3, 1.2f, 5, -1.5708f, GOLD_D);
  }
}

// ein Blatt beim Umblaettern; k = 0..1, vor = von rechts nach links
static void blatt(Gfx &g, float k, bool vor, Farbe vorne, Farbe hinten) {
  float phi = k * 3.14159265f, c = cosf(phi), s = sinf(phi);
  float xo = vor ? MITTE + BW * c : MITTE - BW * c;
  float mx = (MITTE + xo) / 2;
  float pts[2 * 2 * 9];
  int n = 0;
  for (int i = 0; i <= 8; i++) {  // obere Kante, hebt sich
    float u = i / 8.0f, v = 1 - u;
    pts[2 * n] = v * v * MITTE + 2 * v * u * mx + u * u * xo;
    pts[2 * n + 1] = v * v * BY + 2 * v * u * (BY - s * 22) + u * u * (BY - s * 12);
    n++;
  }
  for (int i = 8; i >= 0; i--) {  // untere Kante
    float u = i / 8.0f, v = 1 - u;
    pts[2 * n] = v * v * MITTE + 2 * v * u * mx + u * u * xo;
    pts[2 * n + 1] = v * v * (BY + BH) + 2 * v * u * (BY + BH + s * 6) + u * u * (BY + BH + s * 3);
    n++;
  }
  // Schatten auf der darunterliegenden Seite
  float d = (c >= 0 ? -1 : 1) * (vor ? 1 : -1) * s * 18;
  float sh[8] = {MITTE, BY, xo + d, BY + 6, xo + d, BY + BH, MITTE, BY + BH};
  g.polygon(sh, 4, FARBE(0x3A2418), (uint8_t)(70 * s));
  bool rueck = k > 0.5f;
  g.polygon(pts, n, mischeFarbe(rueck ? hinten : vorne, FARBE(0x8A6A40), 0.25f * s));
  for (int i = 0; i < 7; i++) {  // Zauberstaub an der Blattkante
    float sy = BY + 14 + i * 28 + sinf(k * 9 + i) * 6;
    float sx = xo + (vor ? 1 : -1) * (6 + zufall(i + 90) * 14) * s;
    g.stern(sx, sy, 2 + 2.5f * s, 0.8f, 4, k * 6 + i, i & 1 ? GOLD : WEISS, (uint8_t)(230 * s));
  }
}

static void buchBild(Gfx &g, float t) {
  if (buchAuf && bildMs - buchAuf < 1050) buchTisch(g, t);
  uint32_t jetzt = bildMs;
  // Aufklappen: zu (Titel), Deckel rueckt nach rechts, Deckel klappt auf
  if (buchAuf) {
    float ms = jetzt - buchAuf;
    if (ms < 1050) {
      float k = sanft(seg(ms, 780, 1050));
      float x = lerp(42, MITTE - 6, k), w = lerp(200, BW + 12, k);
      einband(g, x, BY - 6, w, BH + 12);
      uint8_t a = (uint8_t)(255 * (1 - klemme(seg(ms, 780, 880))));
      float mx = x + w / 2;
      // Medaillon mit Krone, "Es war einmal", Titel
      g.ellipse(mx, 118, 82, 52, GOLD_D, a);
      g.ellipse(mx, 118, 79, 49, SAMT_D, a);
      float kr[] = {mx - 16, 74, mx - 16, 60, mx - 8, 68, mx, 56, mx + 8, 68, mx + 16, 60, mx + 16, 74};
      g.polygon(kr, 7, GOLD, a);
      g.text(SCHRIFT_K, "Es war einmal ...", mx, 96, FARBE(0xF2D98A), 1, a);
      g.text(SCHRIFT_G, "Märchenbox", mx, 132, GOLD, 1, a);
      for (int i = 0; i < 8; i++) {
        float an = t * 1.2f + i * 0.785f;
        g.stern(mx + cosf(an) * 92, 118 + sinf(an) * 62, 4, 1.7f, 5, an, GOLD, a);
      }
      zauberstaub(g, t);
      return;
    }
    buchRahmenOffen(g, t);
    linkeSeite(g, buchSeite, t);
    rechteSeite(g, buchSeite, t);
    float k = seg(ms, 1050, AUF_MS);
    if (k < 1) blatt(g, k, true, LEDER, FARBE(0xC9A46A));
    return;
  }
  buchRahmenOffen(g, t);
  if (blattSeit) {
    float k = klemme((jetzt - blattSeit) / (float)BLATT_MS);
    bool vor = blattVor;
    int links = vor ? buchSeite : blattNach, rechts = vor ? blattNach : buchSeite;
    linkeSeite(g, links, t);
    rechteSeite(g, rechts, t);
    blatt(g, sanft(k), vor, PAPIER, PAPIER);
  } else {
    linkeSeite(g, buchSeite, t);
    rechteSeite(g, buchSeite, t);
  }
  // Lesebaendchen
  float lb[] = {259, BY - 6, 266, BY - 6, 266, BY + 26, 262.5f, BY + 21, 259, BY + 26};
  g.polygon(lb, 5, FARBE(0xC0392B));
  zauberstaub(g, t);
}

// ---------- Einstellungen (fuer Eltern) ----------
static void einstellungenBild(Gfx &g, float t) {
  buchRahmenOffen(g, t);
  linkeSeite(g, einstPos == 0 ? SEITE_DREHEN : einstPos == 1 ? SEITE_LAUT : einstPos == 2 ? SEITE_HELL : einstPos == 3 ? SEITE_LESEN : einstPos == 4 ? SEITE_ZEIT : einstPos == 5 ? SEITE_WLAN : auswahl, t);
  seitenGrund(g, false);
  const float cx = MITTE + BW / 2;
  g.text(SCHRIFT_B, "Für Eltern", cx, 50, TINTE, 1);
  g.rechteck(cx - 36, 60, 72, 1.5f, GOLD_D);
  char zeitZeile[40], v[8], b[8];
  zeitText(v, sizeof v, vonMin);
  zeitText(b, sizeof b, bisMin);
  if (zeitFeld == 0) snprintf(zeitZeile, sizeof zeitZeile, "Zeitgrenze: %s", zeitAn ? "an" : "aus");
  else if (zeitFeld == 1) snprintf(zeitZeile, sizeof zeitZeile, "Von: %s", v);
  else if (zeitFeld == 2) snprintf(zeitZeile, sizeof zeitZeile, "Bis: %s", b);
  else if (zeitAn) snprintf(zeitZeile, sizeof zeitZeile, "Zeit: %s-%s", v, b);
  else snprintf(zeitZeile, sizeof zeitZeile, "Zeit: immer");
  const char *namen[EINST_PUNKTE] = {gedreht ? "Bild drehen: an" : "Bild drehen: aus", GRENZE_TEXT[grenze],
                                     HELL_TEXT[hell], LESE_TEXT[lesen], zeitZeile, "WLAN einrichten", "Zurück zum Buch"};
  // Drehrad: die Punkte liegen auf einer Walze, die sich nach oben oder unten dreht (endlos im Kreis)
  const float MY = 134, R = 66, SCHRITT = 0.42f;
  int reihe[EINST_PUNKTE];
  float dd[EINST_PUNKTE];
  for (int i = 0; i < EINST_PUNKTE; i++) {
    float d = fmodf(i - einstAnim + EINST_PUNKTE * 100.0f + EINST_PUNKTE / 2.0f, (float)EINST_PUNKTE) - EINST_PUNKTE / 2.0f;
    dd[i] = d;
    reihe[i] = i;
  }
  for (int a = 0; a < EINST_PUNKTE; a++)  // hinten zuerst zeichnen
    for (int b = a + 1; b < EINST_PUNKTE; b++)
      if (fabsf(dd[reihe[b]]) > fabsf(dd[reihe[a]])) { int x = reihe[a]; reihe[a] = reihe[b]; reihe[b] = x; }
  g.rundRechteck(MITTE + 3, MY - 15, BW - 6, 30, 9, FARBE(0xE9D6AE), 160);  // Mulde fuer den gewaehlten Punkt
  for (int k = 0; k < EINST_PUNKTE; k++) {
    int i = reihe[k];
    float w = dd[i] * SCHRITT;
    if (fabsf(w) > 1.45f) continue;
    float c = cosf(w), y = MY + sinf(w) * R;
    float hh = 22 * c, bw = (BW - 12) * (0.78f + 0.22f * c);
    bool mitte = fabsf(dd[i]) < 0.5f;
    bool bearbeiten = mitte && i == 4 && zeitFeld >= 0;
    uint8_t a = (uint8_t)(255 * klemme(c * c * 1.1f));
    if (mitte) g.kontur(bearbeiten ? 2.4f : 2, bearbeiten ? LEDER : GOLD_D);
    g.rundRechteck(cx - bw / 2, y - hh / 2, bw, hh, 8 * c, bearbeiten ? WEISS : mitte ? FARBE(0xFFF7E6) : FARBE(0xF1E2C2), a);
    g.konturAus();
    if (c > 0.62f)
      g.text(SCHRIFT_K, namen[i], cx, y + 5.5f * c, mitte ? TINTE : FARBE(0x9A7650), 1, (uint8_t)(255 * klemme((c - 0.62f) * 3.5f)));
  }
  // kleine Pfeile: oben und unten geht es weiter
  float ob[] = {cx - 6, 72, cx + 6, 72, cx, 66};
  float un[] = {cx - 6, 196, cx + 6, 196, cx, 202};
  g.polygon(ob, 3, GOLD_D, 170);
  g.polygon(un, 3, GOLD_D, 170);
  g.text(SCHRIFT_K, zeitFeld >= 0 ? "+/- und Start" : "Start: auswählen", cx, 215, FARBE(0x9A7650), 1);
}

// kleines Akkusymbol oben rechts; bei fast leer rot und "Bitte aufladen"
static void akkuAnzeige(Gfx &g, float t, bool mitText) {
  int p = akkuProzent();
  if (p < 0) return;
  const float x = 236, y = 6.5f, w = 22, h = 10;  // etwas eingerueckt: das Display hat runde Ecken
  bool leer = p <= 10 && !akkuLaedt;
  Farbe c = leer ? FARBE(0xFF5A4A) : p <= 25 ? FARBE(0xFFC23D) : FARBE(0x7CDB6A);
  uint8_t a = leer ? (uint8_t)(150 + 105 * fabsf(sinf(t * 3))) : 255;
  g.rundRechteck(x - 1, y - 1, w + 2, h + 2, 3, FARBE(0x10102A), 170);
  g.kontur(1.2f, WEISS);
  g.rundRechteck(x, y, w, h, 2.5f, FARBE(0x10102A));
  g.konturAus();
  g.rechteck(x + w + 0.5f, y + 3, 2, 4, WEISS);
  float f = (w - 4) * (p < 6 ? 0.06f : p / 100.0f);
  g.rundRechteck(x + 2, y + 2, f, h - 4, 1.5f, c, a);
  if (akkuLaedt) {  // Blitz
    float b[] = {x + 12, y + 0.5f, x + 7.5f, y + 5.8f, x + 10.5f, y + 5.8f, x + 9.5f, y + 9.5f, x + 14.5f, y + 4.2f, x + 11.5f, y + 4.2f};
    g.polygon(b, 6, GOLD);
  }
  if (mitText) {
    char s[32];
    snprintf(s, sizeof s, "%d %%  %.2f V", p, akkuMv / 1000.0f);
    g.text(SCHRIFT_K, s, x - 4, y + 10, WEISS, 2);
  } else if (leer) {
    g.text(SCHRIFT_K, "Bitte aufladen", x - 4, y + 10, FARBE(0xFFB0A0), 2, a);
  }
}

// Gute Nacht: Mond, Sterne, das geschlossene Buch schlaeft
uint32_t schlafSeit = 0;
static void schlafBild(Gfx &g, float t) {
  g.verlauf(0, 240, FARBE(0x1E1A4A), FARBE(0x0E0C24));
  for (int i = 0; i < 24; i++) {
    float fu = 0.5f + 0.5f * sinf(t * 1.6f + i * 1.9f);
    g.stern(zufall(i + 11) * 284, zufall(i + 61) * 150, 1.5f + fu * 1.6f, 0.7f, 4, 0, GOLD, (uint8_t)(60 + fu * 150));
  }
  g.kreis(214, 58, 26, FARBE(0xFFF1B0));
  g.kreis(226, 50, 23, FARBE(0x1E1A4A));
  g.kreis(214, 58, 44, FARBE(0xFFF1B0), 25);
  einband(g, 62, 150, 160, 62);
  g.text(SCHRIFT_K, "Märchenbox", 142, 186, GOLD, 1);
  for (int i = 0; i < 3; i++) {
    float k = fmodf(t * 0.5f + i / 3.0f, 1);
    g.text(SCHRIFT_M, "z", 228 + k * 24 + i * 3, 168 - k * 50, WEISS, 1, (uint8_t)(255 * sinf(k * 3.14159f)));
  }
  g.text(SCHRIFT_G, "Gute Nacht!", 142, 110, WEISS, 1);
}

// waehrend Lauter + Leiser gehalten werden: Kreis fuellt sich
static void kombiAnzeige(Gfx &g) {
  if (kombiFortschritt < 0.3f) return;
  float k = (kombiFortschritt - 0.3f) / 0.7f;
  g.rundRechteck(82, 70, 120, 100, 16, DUNKEL, 220);
  g.bogen(142, 112, 22, 0, 6.2831853f, 5, FARBE(0x6B5A50));
  if (k > 0.01f) g.bogen(142, 112, 22, -1.5708f, -1.5708f + 6.2831853f * k, 5, GOLD);
  g.text(SCHRIFT_K, "Einstellungen", 142, 158, WEISS, 1);
}

static void einrichtungBild(Gfx &g, float t) {
  g.verlauf(0, 240, FARBE(0x2B2A6B), FARBE(0x5B3D8F));
  g.text(SCHRIFT_G, "WLAN einrichten", 142, 48, GOLD, 1);
  g.text(SCHRIFT_K, "Mit dem Handy verbinden:", 142, 86, WEISS, 1);
  g.text(SCHRIFT_M, SETUP_WLAN, 142, 114, WEISS, 1);
  g.text(SCHRIFT_K, "Die Seite öffnet sich von selbst.", 142, 146, FARBE(0xCFC8F5), 1);
  g.text(SCHRIFT_K, "Dort WLAN wählen und speichern.", 142, 166, FARBE(0xCFC8F5), 1);
  g.rechteck(62, 184, 160, 1.5f, FARBE(0x8F82C8));
  float b = 0.75f + 0.25f * sinf(t * 3);
  g.text(SCHRIFT_K, "Zurück: Start lang drücken", 142, 212, GOLD, 1, (uint8_t)(255 * b));
}

// zurueck ins Maerchenbuch: Setup-WLAN schliessen und neu starten (die Box startet mit dem Buch)
static void einrichtungAbbrechen() {
  WiFi.softAPdisconnect(true);
  ESP.restart();
}

static void updateBild(Gfx &g) {
  g.verlauf(0, 240, FARBE(0x2B2A6B), FARBE(0x5B3D8F));
  g.text(SCHRIFT_G, "Update", 142, 96, GOLD, 1);
  g.text(SCHRIFT_K, "Bitte nicht ausschalten", 142, 124, FARBE(0xCFC8F5), 1);
  g.rundRechteck(42, 150, 200, 18, 9, FARBE(0x473F86));
  if (updateProzent > 0) g.rundRechteck(42, 150, 2 * updateProzent, 18, 9, GOLD);
}

// Pause: Bild steht und wird dunkler, grosses Weiter-Zeichen, goldene Fortschrittsleiste
static void pauseAnzeige(Gfx &g, float uhr) {
  const Maerchen *m = LISTE[auswahl];
  g.rechteck(0, 0, 284, 240, FARBE(0x0D0B26), 150);
  float b = 1 + sinf(uhr * 3) * 0.05f;
  g.kreis(142, 104, 46 * b, FARBE(0xFFE9A0), 40);
  g.kontur(2, FARBE(0x7A5418));
  g.kreis(142, 104, 34 * b, GOLD);
  g.konturAus();
  g.kreis(142, 104, 28 * b, FARBE(0xC0392B));
  float tri[] = {132, 90, 132, 118, 157, 104};
  g.polygon(tri, 3, FARBE(0xFFF3C0));
  for (int i = 0; i < 6; i++) {
    float an = uhr * 0.8f + i * 1.047f;
    g.stern(142 + cosf(an) * 52, 104 + sinf(an) * 46, 3.5f, 1.4f, 4, an, GOLD, (uint8_t)(140 + 110 * sinf(uhr * 2 + i)));
  }
  // Tafel mit Fortschritt (verdeckt auch Erzaehltexte des Films)
  float k = klemme(pauseZeit / m->dauer);
  g.kontur(1.2f, GOLD_D);
  g.rundRechteck(26, 170, 232, 62, 14, FARBE(0x10102A), 235);
  g.konturAus();
  g.rundRechteck(40, 184, 204, 10, 5, FARBE(0x2E2A55));
  g.rundRechteck(41, 185, 202 * k < 8 ? 8 : 202 * k, 8, 4, GOLD);
  g.stern(41 + 202 * k, 189, 7, 2.8f, 5, -1.5708f, FARBE(0xFFF3C0));
  g.text(SCHRIFT_K, "Pause  -  Start drücken: weiter", 142, 220, FARBE(0xF2D98A), 1);
}

static void lautAnzeige(Gfx &g) {
  uint32_t seit = millis() - lautGezeigt;
  if (lautGezeigt == 0 || seit > 1500) return;
  uint8_t a = seit > 1200 ? (uint8_t)(255 * (1500 - seit) / 300) : 255;
  g.rundRechteck(52, 196, 180, 30, 15, DUNKEL, (uint8_t)(a * 0.85f));
  // Lautsprecher
  float l[] = {66, 206, 72, 206, 80, 200, 80, 222, 72, 216, 66, 216};
  g.polygon(l, 6, WEISS, a);
  for (int i = 0; i < MAXLAUT; i++) {
    float h = 6 + i * 1.4f;
    g.rundRechteck(92 + i * 13, 219 - h, 9, h, 3, i < lautstaerke ? GOLD : FARBE(0x6B5A50), a);
  }
}

// Ein ganzes Bild zeichnen und (wenn ziel == nullptr) zum Display schicken
static void zeichneInhalt(Gfx &g, float t, float uhr) {
  const Maerchen *m = LISTE[auswahl];
  switch (zustand) {
    case SPIELT: m->bild(g, t); break;
    case TITEL: m->titelbild(g, uhr); break;
    case MENUE: buchBild(g, uhr); break;
    case EINRICHTUNG: einrichtungBild(g, uhr); break;
    case UPDATE: updateBild(g); break;
    case EINSTELLUNGEN: einstellungenBild(g, uhr); break;
    case SCHLAF: schlafBild(g, uhr); break;
  }
  g.kameraAus();
  g.wackeln(0, 0);
  g.ausschnittAus();
  g.konturAus();
  if (zustand == SPIELT && pausiert) pauseAnzeige(g, uhr);
  if (zustand == SPIELT || zustand == TITEL) lautAnzeige(g);
  if (zustand == MENUE || zustand == TITEL || zustand == EINSTELLUNGEN) akkuAnzeige(g, uhr, zustand == EINSTELLUNGEN);
  if (zustand != EINSTELLUNGEN && zustand != EINRICHTUNG && zustand != UPDATE && zustand != SCHLAF) kombiAnzeige(g);
}

static void neuesBild() {
  bildMs = millis();
  float t = zustand == SPIELT ? (pausiert ? pauseZeit : hoerZeit()) : 0;
  float uhr = millis() / 1000.0f;
  uint32_t zu = 0, wu = 0;
  blaseLesbar = 0;
  for (int s = 0; s < lcd::ANZAHL; s++) {
    uint32_t a = micros();
    uint16_t *p = lcd::puffer();
    uint32_t b = micros();
    g.streifen(p, s * lcd::STREIFEN, (s + 1) * lcd::STREIFEN);
    g.kameraAus();
    g.wackeln(0, 0);
    g.ausschnittAus();
    g.konturAus();
    zeichneInhalt(g, t, uhr);
    uint32_t c = micros();
    lcd::senden(p, s * lcd::STREIFEN);
    wu += (b - a) + (micros() - c);
    zu += c - b;
  }
  zeichenUs = zu;
  wartUs = wu;
  blaseOffen = zustand == SPIELT && !pausiert && blaseLesbar;
}

// =====================================================================
// Abspielen
// =====================================================================
static void wechsle(Zustand z) {
  zustand = z;
  zustandSeit = millis();
  lcd::licht(true);
}
static void starte(int nr) {
  auswahl = (nr + ANZAHL) % ANZAHL;
  prefs.begin("box", false); prefs.putInt("auswahl", auswahl); prefs.end();
  audioMaerchen = LISTE[auswahl];
  anfStart = true;
  wechsle(SPIELT);
  Serial.printf("Start: %s\n", LISTE[auswahl]->titel);
}
static void merke(const char *was);
static void pauseUmschalten() {
  if (!pausiert) {
    pauseZeit = hoerZeit();
    pausiert = true;
    pauseSeit = millis();
    merke("Pause");
  } else {
    pausiert = false;
    lcd::licht(true);
    merke("weiter");
  }
}

static void stoppe() {
  anfStopp = true;
  wechsle(TITEL);
  Serial.println("Stopp");
}

// =====================================================================
// WLAN
// =====================================================================
WebServer server(80);
DNSServer *dns = nullptr;
bool otaGestartet = false, serverLaeuft = false;
String netze;

static String htmlEsc(const String &s) {
  String r;
  for (char c : s) {
    if (c == '<') r += "&lt;"; else if (c == '>') r += "&gt;"; else if (c == '&') r += "&amp;";
    else if (c == '"') r += "&quot;"; else r += c;
  }
  return r;
}
static void netzeSuchen() {
  int n = WiFi.scanNetworks();
  netze = "";
  for (int i = 0; i < n; i++) {
    String s = WiFi.SSID(i);
    if (s.length() && netze.indexOf(">" + htmlEsc(s) + "<") < 0) netze += "<option>" + htmlEsc(s) + "</option>";
  }
  WiFi.scanDelete();
}
static const char SEITE_KOPF[] PROGMEM =
  "<!doctype html><html lang=de><meta charset=utf-8><meta name=viewport content='width=device-width,initial-scale=1'>"
  "<title>Märchenbox</title><style>body{font-family:system-ui,sans-serif;background:#2B2A6B;color:#f6ead2;"
  "max-width:420px;margin:0 auto;padding:20px}h1{font-size:1.4em;color:#FFD54A}label{display:block;margin:14px 0 4px}"
  "select,input{width:100%;box-sizing:border-box;font-size:1.1em;padding:10px;border-radius:8px;border:0}"
  "button{margin-top:20px;width:100%;font-size:1.1em;padding:12px;border:0;border-radius:8px;background:#FFD54A;color:#2B2A6B;font-weight:700}"
  "a{color:#FFD54A}</style><h1>Märchenbox</h1>";

static void seiteStart() {
  String s = FPSTR(SEITE_KOPF);
  s += "<p>Mit welchem WLAN soll sich die Box verbinden?</p><form method=post action=/save>"
       "<label>WLAN</label><select name=ssid>" + netze + "</select>"
       "<label>Passwort</label><input name=pass type=password autocomplete=off>"
       "<button>Speichern</button></form><p><a href=/scan>Netze neu suchen</a></p>";
  server.send(200, "text/html", s);
}
static void seiteSpeichern() {
  String ssid = server.arg("ssid"), pass = server.arg("pass");
  if (!ssid.length()) { server.sendHeader("Location", "/"); server.send(302); return; }
  prefs.begin("wlan", false);
  prefs.putString("ssid", ssid);
  prefs.putString("pass", pass);
  prefs.end();
  String s = FPSTR(SEITE_KOPF);
  s += "<p>Gespeichert. Die Box startet neu und verbindet sich mit <b>" + htmlEsc(ssid) + "</b>.</p>";
  server.send(200, "text/html", s);
  delay(1500);
  ESP.restart();
}

static void einrichtungStarten() {
  if (zustand == SPIELT) anfStopp = true;
  wechsle(EINRICHTUNG);
  neuesBild();
  WiFi.disconnect();
  WiFi.mode(WIFI_AP_STA);
  netzeSuchen();
  WiFi.softAP(SETUP_WLAN);
  dns = new DNSServer();
  dns->start(53, "*", WiFi.softAPIP());
  server.on("/", seiteStart);
  server.on("/scan", []() { netzeSuchen(); server.sendHeader("Location", "/"); server.send(302); });
  server.on("/save", HTTP_POST, seiteSpeichern);
  server.onNotFound([]() { server.sendHeader("Location", "http://" + WiFi.softAPIP().toString() + "/"); server.send(302); });
  if (!serverLaeuft) { server.begin(); serverLaeuft = true; }
}

// --- Entwicklungshilfen im Heimnetz ---
static uint16_t fotoPuffer[lcd::W * lcd::STREIFEN];
static void sendeBildschirmfoto() {
  bool mitZeit = server.hasArg("t");
  float t = mitZeit ? server.arg("t").toFloat() : (zustand == SPIELT ? hoerZeit() : 0);
  Zustand alt = zustand;
  int altAuswahl = auswahl;
  if (server.hasArg("m")) auswahl = constrain(server.arg("m").toInt(), 0, ANZAHL - 1);
  if (mitZeit) zustand = SPIELT;
  if (server.hasArg("z")) zustand = (Zustand)constrain(server.arg("z").toInt(), 0, (int)SCHLAF);  // nur fuers Foto
  uint16_t *tmp = fotoPuffer;
  static uint8_t zeile[lcd::W * 3];
  const uint32_t groesse = 54 + lcd::W * 3 * lcd::H;
  uint8_t kopf[54] = {'B', 'M'};
  auto u32 = [&](int o, uint32_t v) { for (int i = 0; i < 4; i++) kopf[o + i] = v >> (8 * i); };
  u32(2, groesse); u32(10, 54); u32(14, 40); u32(18, lcd::W); u32(22, (uint32_t)(-lcd::H));
  kopf[26] = 1; kopf[28] = 24; u32(34, lcd::W * 3 * lcd::H);
  server.setContentLength(groesse);
  server.send(200, "image/bmp", "");
  server.sendContent((const char *)kopf, 54);
  float uhr = millis() / 1000.0f;
  bildMs = millis();
  uint32_t altBlatt = blattSeit, altAuf = buchAuf;
  int altNach = blattNach;
  if (server.hasArg("blatt") && buchSeite < SEITEN - 1) {
    blattNach = (buchSeite + 1) % SEITEN;
    blattVor = true;
    blattSeit = bildMs - (uint32_t)(server.arg("blatt").toFloat() * BLATT_MS);
  }
  if (server.hasArg("auf")) buchAuf = bildMs - server.arg("auf").toInt();
  for (int s = 0; s < lcd::ANZAHL; s++) {
    g.streifen(tmp, s * lcd::STREIFEN, (s + 1) * lcd::STREIFEN);
    g.kameraAus(); g.wackeln(0, 0); g.ausschnittAus(); g.konturAus();
    zeichneInhalt(g, t, uhr);
    for (int y = 0; y < lcd::STREIFEN; y++) {
      for (int x = 0; x < lcd::W; x++) {
        uint16_t v = tmp[y * lcd::W + x];
        v = (v >> 8) | (v << 8);
        zeile[3 * x + 2] = (v >> 11) << 3;
        zeile[3 * x + 1] = ((v >> 5) & 63) << 2;
        zeile[3 * x + 0] = (v & 31) << 3;
      }
      server.sendContent((const char *)zeile, sizeof zeile);
    }
  }
  zustand = alt;
  auswahl = altAuswahl;
  blattSeit = altBlatt; buchAuf = altAuf; blattNach = altNach;
}
static void tasteVonFern();

// Messtest: was kostet welche Form? (ein ganzes Bild in 4 Streifen)
static void sendeMesstest() {
  uint16_t *tmp = fotoPuffer;
  String aus;
  auto miss = [&](const char *name, std::function<void(Gfx &)> f) {
    uint32_t a = micros();
    for (int s = 0; s < lcd::ANZAHL; s++) {
      g.streifen(tmp, s * lcd::STREIFEN, (s + 1) * lcd::STREIFEN);
      g.kameraAus(); g.ausschnittAus(); g.konturAus();
      f(g);
    }
    aus += String(name) + ": " + String((micros() - a) / 1000.0f, 2) + " ms\n";
  };
  if (server.hasArg("buch")) {  // was kostet welcher Teil des Maerchenbuchs?
    int mi = server.hasArg("m") ? constrain(server.arg("m").toInt(), 0, ANZAHL - 1) : 0;
    const Maerchen *mm = LISTE[mi];
    float tt = 3.3f;
    miss("rahmen offen", [tt](Gfx &g) { buchRahmenOffen(g, tt); });
    miss("seitengrund rechts", [](Gfx &g) { seitenGrund(g, false); });
    miss("cover ganz", [mm, tt](Gfx &g) { cover(g, mm, tt); });
    miss("vorschau gross", [mm, tt](Gfx &g) { g.ansicht(28, 90, 0.42f); g.ausschnitt(0, 0, 284, 240); mm->vorschau(g, tt); g.ausschnittAus(); g.kameraAus(); });
    miss("bogenfenster", [tt](Gfx &g) { bogenFenster(g, 28, 90, 95, 100, tt); });
    miss("rechte seite", [mi, tt](Gfx &g) { rechteSeite(g, mi, tt); });
    miss("symbol", [mm, tt](Gfx &g) { mm->symbol(g, 206, 152, tt); g.kameraAus(); });
    miss("zauberstaub", [tt](Gfx &g) { zauberstaub(g, tt); });
    miss("startknopf", [tt](Gfx &g) { startKnopf(g, 206, 196, tt, 11); });
    miss("schnoerkel x2", [](Gfx &g) { schnoerkel(g, 206, 54, 34, GOLD_D); schnoerkel(g, 206, 160, 30, GOLD_D); });
    miss("buch ganz", [tt](Gfx &g) { buchBild(g, tt); });
    server.send(200, "text/plain", aus);
    return;
  }
  aus += "CPU " + String(getCpuFrequencyMhz()) + " MHz, Flash " + String(ESP.getFlashChipSpeed() / 1000000) + " MHz\n";
  { uint32_t a = ESP.getCycleCount(); volatile int x = 0; for (int i = 0; i < 100000; i++) x += i; aus += "100k Additionen: " + String(ESP.getCycleCount() - a) + " Takte\n"; }
  miss("fuellen", [](Gfx &g) { g.fuellen(0x1234); });
  miss("verlauf ganz", [](Gfx &g) { g.verlauf(0, 240, 0x1234, 0x4321); });
  miss("rechteck ganz", [](Gfx &g) { g.rechteck(0, 0, 284, 240, 0x1234); });
  miss("rechteck ganz a128", [](Gfx &g) { g.rechteck(0, 0, 284, 240, 0x1234, 128); });
  miss("100 kreise r10", [](Gfx &g) { for (int i = 0; i < 100; i++) g.kreis(20 + (i % 10) * 26, 20 + (i / 10) * 22, 10, 0x1234); });
  miss("100 kreise r3", [](Gfx &g) { for (int i = 0; i < 100; i++) g.kreis(20 + (i % 10) * 26, 20 + (i / 10) * 22, 3, 0x1234); });
  miss("100 linien d3 l20", [](Gfx &g) { for (int i = 0; i < 100; i++) g.linie(20 + (i % 10) * 26, 20 + (i / 10) * 22, 35 + (i % 10) * 26, 34 + (i / 10) * 22, 3, 0x1234); });
  miss("100 rundrecht 20x10", [](Gfx &g) { for (int i = 0; i < 100; i++) g.rundRechteck(10 + (i % 10) * 26, 10 + (i / 10) * 22, 20, 10, 4, 0x1234); });
  miss("100 kreise kontur", [](Gfx &g) { g.kontur(1.5f, 0); for (int i = 0; i < 100; i++) g.kreis(20 + (i % 10) * 26, 20 + (i / 10) * 22, 10, 0x1234); g.konturAus(); });
  miss("400 dreiecke klein", [](Gfx &g) { for (int i = 0; i < 400; i++) { float x = 10 + (i % 20) * 13, y = 10 + (i / 20) * 11; float q[] = {x, y, x + 6, y, x + 3, y + 6}; g.polygon(q, 3, 0x1234); } });
  miss("400 rechtecke 6x6", [](Gfx &g) { for (int i = 0; i < 400; i++) g.rechteck(10 + (i % 20) * 13, 10 + (i / 20) * 11, 6, 6, 0x1234); });
  miss("400 kreise r3", [](Gfx &g) { for (int i = 0; i < 400; i++) g.kreis(13 + (i % 20) * 13, 13 + (i / 20) * 11, 3, 0x1234); });
  miss("400 aussen kreise", [](Gfx &g) { for (int i = 0; i < 400; i++) g.kreis(13 + (i % 20) * 13, 400, 3, 0x1234); });
  miss("400x sinf+cosf", [](Gfx &g) { volatile float v = 0; for (int i = 0; i < 400; i++) v += sinf(i * 0.01f) + cosf(i * 0.02f); });
  miss("4000 div", [](Gfx &g) { volatile float v = 1.3f, w = 0; for (int i = 0; i < 4000; i++) w += v / (i + 1.5f); });
  miss("text 10x", [](Gfx &g) { for (int i = 0; i < 10; i++) g.text(SCHRIFT_M, "Schneiderlein", 10, 20 + i * 22, 0xFFFF); });
  static const float zeiten[] = {6, 40, 50, 66, 80, 115, 150, 175, 195, 220, 245, 270, 290};
  for (float zt : zeiten) {
    char n[24];
    snprintf(n, sizeof n, "szene t=%.0f", zt);
    int mi = server.hasArg("m") ? constrain(server.arg("m").toInt(), 0, ANZAHL - 1) : 0;
    miss(n, [zt, mi](Gfx &g) { LISTE[mi]->bild(g, zt); });
  }
  server.send(200, "text/plain", aus);
}

static void heimnetzServer() {
  server.on("/bild.bmp", sendeBildschirmfoto);
  server.on("/taste", tasteVonFern);
  server.on("/messtest", sendeMesstest);
  server.on("/status", []() {
    char s[520], uz[8] = "--:--";
    int um = uhrMinuten();
    if (um >= 0) zeitText(uz, sizeof uz, um);
    snprintf(s, sizeof s, "{\"zustand\":%d,\"maerchen\":%d,\"t\":%.2f,\"fps\":%.1f,\"zeichnenMs\":%.1f,\"wartenMs\":%.1f,\"laut\":%d,\"heap\":%u,\"uhr\":\"%s\",\"erlaubt\":%d,\"akkuMv\":%.0f,\"akkuRohMv\":%d,\"akkuProzent\":%d,\"ladePin\":%d,\"tasten\":\"%s\"}",
             (int)zustand, auswahl, zustand == SPIELT ? hoerZeit() : 0.0f, fps, zeichenUs / 1000.0f, wartUs / 1000.0f,
             lautstaerke, ESP.getFreeHeap(), uz, nutzungErlaubt() ? 1 : 0, akkuMv, akkuRohMv, akkuProzent(), ladenRoh,
             protokoll.c_str());
    server.send(200, "application/json", s);
  });
  if (!serverLaeuft) { server.begin(); serverLaeuft = true; }
}

static void otaStarten() {
  ArduinoOTA.setHostname(NAME_IM_NETZ);
  if (strlen(OTA_PASSWORT) > 0) ArduinoOTA.setPassword(OTA_PASSWORT);
  ArduinoOTA.onStart([]() {
    anfStopp = true;
    updateProzent = 0;
    wechsle(UPDATE);
    neuesBild();
  });
  ArduinoOTA.onProgress([](unsigned int f, unsigned int gesamt) {
    int p = f * 100 / gesamt;
    if (p != updateProzent) { updateProzent = p; neuesBild(); }
  });
  ArduinoOTA.onError([](ota_error_t) { ESP.restart(); });
  ArduinoOTA.begin();
  otaGestartet = true;
  configTzTime("CET-1CEST,M3.5.0,M10.5.0/3", "pool.ntp.org", "time.google.com", "ptbtime1.ptb.de");
  heimnetzServer();
  Serial.printf("WLAN: %s (" NAME_IM_NETZ ".local)\n", WiFi.localIP().toString().c_str());
}

static void wlanStart() {
  prefs.begin("wlan", true);
  String ssid = prefs.getString("ssid", ""), pass = prefs.getString("pass", "");
  prefs.end();
  if (!ssid.length()) return;  // Einrichtung ueber das Menue
  WiFi.mode(WIFI_STA);
  WiFi.setHostname(NAME_IM_NETZ);
  WiFi.setAutoReconnect(true);
  WiFi.begin(ssid.c_str(), pass.c_str());
}

// =====================================================================
// Knoepfe
// =====================================================================
enum Ereignis { NICHTS, KURZ, DOPPEL, LANG };
struct MitteKnopf {
  int pin = TASTE_A;
  bool unten = false, lang = false;
  uint32_t seit = 0, los = 0, prell = 0;
  int klicks = 0;
  Ereignis lies() {
    uint32_t t = millis();
    bool jetzt = digitalRead(pin) == LOW;
    if (jetzt != unten && t - prell > 25) {
      prell = t;
      unten = jetzt;
      if (jetzt) { seit = t; lang = false; }
      else if (!lang) {
        klicks++;
        los = t;
        if (klicks >= 2) { klicks = 0; return DOPPEL; }
      }
    }
    if (unten && !lang && t - seit > 900) { lang = true; klicks = 0; return LANG; }
    if (!unten && klicks == 1 && t - los > 320) { klicks = 0; return KURZ; }
    return NICHTS;
  }
} mitte;
struct WiederholKnopf {
  int pin; bool unten = false; uint32_t seit = 0, naechste = 0, prell = 0;
  explicit WiederholKnopf(int p) : pin(p) {}
  bool lies() {  // true beim Druecken und beim Gedrueckthalten alle 220 ms
    uint32_t t = millis();
    bool jetzt = digitalRead(pin) == LOW;
    if (jetzt != unten && t - prell > 25) {
      prell = t; unten = jetzt;
      if (jetzt) { seit = t; naechste = t + 500; return true; }
    }
    if (unten && t >= naechste) { naechste = t + 220; return true; }
    return false;
  }
};
WiederholKnopf leiser(TASTE_C), lauter(TASTE_B);

// Rollen der Knoepfe: Start = A (Mitte), Lauter = B (oben), Leiser = C (unten);
// bei gedrehtem Bild tauschen oben und unten, damit oben immer Lauter ist
static void tastenBelegen() {
  mitte = MitteKnopf();
  mitte.pin = TASTE_A;
  lauter = WiederholKnopf(gedreht ? TASTE_C : TASTE_B);
  leiser = WiederholKnopf(gedreht ? TASTE_B : TASTE_C);
  // gerade gedrueckte Knoepfe nicht als neuen Druck werten
  mitte.unten = digitalRead(mitte.pin) == LOW; mitte.lang = mitte.unten;
  lauter.unten = digitalRead(lauter.pin) == LOW; lauter.naechste = UINT32_MAX;
  leiser.unten = digitalRead(leiser.pin) == LOW; leiser.naechste = UINT32_MAX;
}

static void drehenUmschalten() {
  gedreht = !gedreht;
  lcd::drehen(gedreht);
  tastenBelegen();
  prefs.begin("box", false); prefs.putBool("gedreht", gedreht); prefs.end();
}

// die letzten Tastendruecke (fuer /status)
static void merke(const char *was) {
  protokoll = String(millis() / 1000) + "s " + was + (protokoll.length() ? ", " : "") + protokoll;
  if (protokoll.length() > 160) protokoll = protokoll.substring(0, 160);
}

static void buchOeffnen() {
  buchSeite = auswahl;
  blattSeit = 0;
  buchAuf = millis();
  wechsle(MENUE);
  uiKlang = 2;
}

static void blaettern(int d) {
  buchAuf = 0;
  if (blattSeit) { buchSeite = blattNach; blattSeit = 0; }
  // im Kreis: nach dem letzten Maerchen kommt wieder das erste
  if (SEITEN < 2) return;
  int ziel = ((buchSeite + d) % SEITEN + SEITEN) % SEITEN;
  blattVor = d > 0;
  blattNach = ziel;
  blattSeit = millis();
  uiKlang = 1;
}

static void einschlafen() {
  if (zustand == SPIELT) anfStopp = true;
  zeitFeld = -1;
  wechsle(SCHLAF);
  schlafSeit = millis();
  uiKlang = 4;
  merke("Gute Nacht");
}
static void aufwachen() {
  if (!nutzungErlaubt()) { ueberbrueckt = true; wachSeit = millis(); }
  setCpuFrequencyMhz(240);
  merke("aufgewacht");
  buchOeffnen();
}

static void einstellungenOeffnen() {
  if (zustand == SPIELT) anfStopp = true;
  einstPos = 0;
  einstZiel = 0;
  einstAnim = 0;
  wechsle(EINSTELLUNGEN);
  uiKlang = 2;
}

// Ereignis verarbeiten (von den Knoepfen oder von /taste)
static void ereignis(Ereignis e, int laut) {
  letzteTaste = millis();
  if (!lcd::lichtAn() && zustand == TITEL) { lcd::licht(true); return; }  // erster Druck weckt nur
  if (!lcd::lichtAn() && zustand == SPIELT && pausiert) { lcd::licht(true); pauseSeit = millis(); return; }
  if (zustand == SCHLAF) { if (nutzungErlaubt()) aufwachen(); return; }
  if (zustand == EINRICHTUNG) {
    if (e == LANG) einrichtungAbbrechen();
    return;
  }
  if (zustand == EINSTELLUNGEN && zeitFeld >= 0) {  // Nutzungszeit bearbeiten
    if (laut) {
      if (zeitFeld == 0) zeitAn = !zeitAn;
      else if (zeitFeld == 1) vonMin = (vonMin + laut * 15 + 1440) % 1440;
      else bisMin = (bisMin + laut * 15 + 1440) % 1440;
      uiKlang = 1;
    }
    if (e == KURZ || e == DOPPEL || e == LANG) {
      zeitFeld = (e == LANG || (zeitFeld == 0 && !zeitAn)) ? 3 : zeitFeld + 1;
      if (zeitFeld > 2) {
        zeitFeld = -1;
        prefs.begin("box", false);
        prefs.putBool("zeitAn", zeitAn); prefs.putInt("von", vonMin); prefs.putInt("bis", bisMin);
        prefs.end();
      }
      uiKlang = 3;
    }
    return;
  }
  if (zustand == EINSTELLUNGEN) {
    if (laut) {
      einstZiel -= laut;
      einstPos = ((einstZiel % EINST_PUNKTE) + EINST_PUNKTE) % EINST_PUNKTE;
      uiKlang = laut > 0 ? 5 : 6;  // Tschuuu nach oben / unten
    }
    if (e == KURZ || e == DOPPEL) {
      if (einstPos == 0) { drehenUmschalten(); uiKlang = 3; }
      else if (einstPos == 1) {  // Hoechstlautstaerke weiterschalten
        grenze = (grenze + 1) % GRENZEN;
        prefs.begin("box", false); prefs.putInt("grenze", grenze); prefs.end();
        setzeLautstaerke(false);
        uiKlang = 3;
      }
      else if (einstPos == 2) {  // Helligkeit weiterschalten
        hell = (hell + 1) % HELL_STUFEN;
        lcd::helligkeit(HELL_PWM[hell]);
        prefs.begin("box", false); prefs.putInt("hell", hell); prefs.end();
        uiKlang = 3;
      }
      else if (einstPos == 3) {  // Lesetempo der Sprechblasen weiterschalten
        lesen = (lesen + 1) % LESEN_STUFEN;
        prefs.begin("box", false); prefs.putInt("lesen", lesen); prefs.end();
        uiKlang = 3;
      }
      else if (einstPos == 4) { zeitFeld = 0; uiKlang = 3; }
      else if (einstPos == 5) einrichtungStarten();
      else buchOeffnen();
    }
    if (e == LANG) buchOeffnen();
    return;
  }
  if (zustand == UPDATE) return;
  if (zustand == MENUE) {
    if (laut) blaettern(-laut);
    if (e == KURZ || e == DOPPEL) {
      int seite = blattSeit ? blattNach : buchSeite;
      uiKlang = 3;
      starte(seite);
    }
    if (e == LANG) wechsle(TITEL);
    return;
  }
  if (laut) {
    lautstaerke = constrain(lautstaerke + laut, 0, MAXLAUT);
    setzeLautstaerke(true);
    lautGezeigt = millis();
  }
  if (e == KURZ) { if (zustand == SPIELT) pauseUmschalten(); else starte(auswahl); }
  if (e == DOPPEL) starte(auswahl + 1);
  if (e == LANG) {
    if (zustand == SPIELT) anfStopp = true;
    buchOeffnen();
  }
}

static void tasteVonFern() {
  String k = server.arg("k");
  merke(("fern " + k).c_str());
  if (k == "kurz") ereignis(KURZ, 0);
  else if (k == "doppel") ereignis(DOPPEL, 0);
  else if (k == "lang") ereignis(LANG, 0);
  else if (k == "laut") ereignis(NICHTS, 1);
  else if (k == "leise") ereignis(NICHTS, -1);
  else if (k == "einstellungen") einstellungenOeffnen();
  else if (k == "schlafen") einschlafen();
  else if (k == "pause" && zustand == SPIELT) pauseUmschalten();
  else if (k == "wecken") aufwachen();
  server.send(200, "text/plain", "ok");
}

// =====================================================================
void setup() {
  Serial.begin(115200);
  pinMode(PIN_PA, OUTPUT); digitalWrite(PIN_PA, LOW);
  pinMode(TASTE_A, INPUT_PULLUP);
  pinMode(TASTE_B, INPUT_PULLUP);
  pinMode(TASTE_C, INPUT_PULLUP);
  lcd::start();

  i2sInit();  // MCLK muss laufen, bevor der ES8311 eingestellt wird
  delay(10);
  Wire.begin(PIN_I2C_SDA, PIN_I2C_SCL, 100000);
  Serial.println(es8311Init() ? "ES8311 bereit" : "FEHLER: ES8311 antwortet nicht");
  prefs.begin("box", true);
  lautstaerke = prefs.getInt("laut", 7);
  grenze = constrain(prefs.getInt("grenze", 1), 0, GRENZEN - 1);
  lesen = constrain(prefs.getInt("lesen", 1), 0, LESEN_STUFEN - 1);
  hell = constrain(prefs.getInt("hell", 1), 0, HELL_STUFEN - 1);
  zeitAn = prefs.getBool("zeitAn", true);
  vonMin = constrain(prefs.getInt("von", 6 * 60), 0, 1439);
  bisMin = constrain(prefs.getInt("bis", 19 * 60), 0, 1439);
  gedreht = prefs.getBool("gedreht", false);
  auswahl = constrain(prefs.getInt("auswahl", 0), 0, ANZAHL - 1);
  prefs.end();
  setzeLautstaerke(false);
  lcd::drehen(gedreht);
  lcd::helligkeit(HELL_PWM[hell]);
  tastenBelegen();
  pinMode(PIN_LADEN, INPUT_PULLUP);  // Ladeanzeige des Ladereglers ist open-drain: zieht nur beim Laden auf LOW
  analogSetPinAttenuation(PIN_AKKU, ADC_11db);
  akkuMessen();
  synth.start();
  xTaskCreatePinnedToCore(audioTask, "audio", 6144, nullptr, 5, nullptr, 0);

  wlanStart();
  neuesBild();
  lcd::licht(true);
  buchOeffnen();  // beim Einschalten klappt das Maerchenbuch auf
}

void loop() {
  static uint32_t letztesBild = 0, bilder = 0, fpsSeit = 0;
  if (dns) dns->processNextRequest();
  if (serverLaeuft) server.handleClient();
  if (!otaGestartet && WiFi.status() == WL_CONNECTED) otaStarten();
  if (otaGestartet) ArduinoOTA.handle();
  if (zustand == UPDATE) return;

  Ereignis e = mitte.lies();
  int l = (lauter.lies() ? 1 : 0) - (leiser.lies() ? 1 : 0);
  // Einstellungen: Lauter + Leiser zusammen 3 s halten
  static uint32_t beideSeit = 0;
  static bool beideErledigt = false;
  static int lautRuhe = lautstaerke;
  bool obenUnten = digitalRead(lauter.pin) == LOW, untenUnten = digitalRead(leiser.pin) == LOW;
  if (obenUnten && untenUnten) {
    if (!beideSeit) beideSeit = millis();
    l = 0;
    kombiFortschritt = beideErledigt ? 0 : klemme((millis() - beideSeit) / (float)KOMBI_MS);
    if (!beideErledigt && millis() - beideSeit >= KOMBI_MS) {
      beideErledigt = true;
      kombiFortschritt = 0;
      if (lautstaerke != lautRuhe) { lautstaerke = lautRuhe; setzeLautstaerke(true); }  // erster Druck zaehlt nicht
      if (zustand == SCHLAF) aufwachen();
      else if (zustand != EINRICHTUNG && zustand != UPDATE) { merke("Einstellungen"); einstellungenOeffnen(); }
    }
  } else {
    beideSeit = 0;
    beideErledigt = false;
    kombiFortschritt = 0;
    if (!obenUnten && !untenUnten) lautRuhe = lautstaerke;
  }
  if (e != NICHTS || l) {
    merke(e == KURZ ? "Knopf kurz" : e == DOPPEL ? "Knopf doppel" : e == LANG ? "Knopf lang" : l > 0 ? "Knopf laut" : "Knopf leise");
    ereignis(e, l);
  }

  if (maerchenEnde) { maerchenEnde = false; wechsle(TITEL); }
  // Buch: Umblaettern / Aufklappen fertig?
  if (blattSeit && millis() - blattSeit >= BLATT_MS) { buchSeite = blattNach; blattSeit = 0; }
  if (buchAuf && millis() - buchAuf >= AUF_MS) buchAuf = 0;
  buchLied = (zustand == MENUE && !blattSeit && !buchAuf) ? buchSeite : -1;
  // Display aus, wenn lange nichts passiert
  if (zustand == TITEL && lcd::lichtAn() && millis() - zustandSeit > 90000 && millis() - letzteTaste > 90000) lcd::licht(false);
  if (zustand == MENUE && millis() - zustandSeit > 60000 && millis() - letzteTaste > 60000) wechsle(TITEL);
  if (zustand == EINSTELLUNGEN && millis() - zustandSeit > 60000 && millis() - letzteTaste > 60000) { zeitFeld = -1; buchOeffnen(); }
  // Nutzungszeit: ein laufendes Maerchen darf zu Ende gehen, dann Gute Nacht
  static uint32_t akkuGemessen = 0;
  if (millis() - akkuGemessen > 2000) { akkuGemessen = millis(); akkuMessen(); }
  static uint32_t zeitGeprueft = 0;
  if (millis() - zeitGeprueft > 1000) {
    zeitGeprueft = millis();
    if (nutzungErlaubt()) ueberbrueckt = false;
    else if (ueberbrueckt && millis() - wachSeit > WACH_MS) ueberbrueckt = false;
    bool sperre = !nutzungErlaubt() && !ueberbrueckt;
    if (sperre && (zustand == TITEL || zustand == MENUE || (zustand == SPIELT && pausiert))) einschlafen();
  }
  if (zustand == SPIELT && pausiert && lcd::lichtAn() && millis() - pauseSeit > 10UL * 60 * 1000 &&
      millis() - letzteTaste > 10UL * 60 * 1000) lcd::licht(false);
  if (zustand == SCHLAF && lcd::lichtAn() && millis() - schlafSeit > 12000) {
    lcd::licht(false);
    setCpuFrequencyMhz(80);  // sparsam warten, WLAN bleibt fuer Updates an
  }

  // Drehrad laeuft weich hinterher
  static uint32_t animSeit = millis();
  float dtA = (millis() - animSeit) / 1000.0f;
  animSeit = millis();
  einstAnim += (einstZiel - einstAnim) * (dtA * 12 > 1 ? 1 : dtA * 12);
  if (fabsf(einstZiel - einstAnim) < 0.002f) einstAnim = einstZiel;

  // Bild (hoechstens 30 je Sekunde)
  uint32_t jetzt = millis();
  if (lcd::lichtAn() && jetzt - letztesBild >= 33) {
    letztesBild = jetzt;
    neuesBild();
    bilder++;
    if (jetzt - fpsSeit >= 2000) { fps = bilder * 1000.0f / (jetzt - fpsSeit); bilder = 0; fpsSeit = jetzt; }
  } else {
    delay(1);
  }
}

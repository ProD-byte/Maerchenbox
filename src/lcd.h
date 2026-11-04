#pragma once
// NV3023-Display (284 x 240 sichtbar) am SPI-Bus mit DMA.
// Gezeichnet wird in zwei Streifenpuffer im Wechsel: waehrend einer per DMA zum Display geht,
// entsteht der naechste.
#include <stdint.h>

namespace lcd {
constexpr int W = 284, H = 240;
constexpr int STREIFEN = 60;              // Zeilen je Streifen
constexpr int ANZAHL = H / STREIFEN;      // 4 Streifen je Bild

void start();                     // Reset, Startsequenz, Puffer anlegen
uint16_t *puffer();               // freien Streifenpuffer holen (wartet, bis DMA ihn freigibt)
void senden(uint16_t *p, int y);  // Streifen ab Zeile y abschicken (kehrt sofort zurueck)
void warten();                    // bis alles beim Display angekommen ist
void licht(bool an);
void drehen(bool gedreht);        // Bild um 180 Grad drehen
bool lichtAn();
}  // namespace lcd

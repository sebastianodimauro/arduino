/*
  Rings pattern, controlled over E1.31
*/
#define FASTLED_ALLOW_INTERRUPTS 0
#include <FastLED.h>
#include <ESP8266WiFi.h>
#include <E131.h>

#define UNIVERSE 1      /* Universe to listen for */
#define CHANNEL_START 0 /* Channel to start listening at */
#define LED_PIN 12      /* Pixel output - GPIO0 */

#define COLOR_ORDER GBR
#define CHIPSET     WS2811

#define RINGS 6
#define NUM_LEDS 100

int JUMP = 15;
int SPEED = 1200;
boolean INWARD = true;
int BRIGHTNESS = 15;

const char ssid[] = "ManstonManor";         /* Replace with your SSID */
const char passphrase[] = "BigTitties29!";   /* Replace with your WPA2 passphrase */

E131 e131;
CRGB leds[NUM_LEDS];      //naming our LED array
int hue[RINGS];



void setup() {
  Serial.begin(115200);
  delay(10);

  /* Choose one to begin listening for E1.31 data */
  //e131.begin(ssid, passphrase);                       /* via Unicast on the default port */
  e131.beginMulticast(ssid, passphrase, UNIVERSE);  /* via Multicast for Universe 1 */

  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS); //.setCorrection(TypicalSMD5050);
  FastLED.setBrightness( BRIGHTNESS );

  int h = 0;
  for(int r=0; r < RINGS; r++) {
    hue[r] = h;
    h += JUMP;
  }
}

void loop() {
  /* Parse a packet and update pixels */
  if (e131.parsePacket()) {
    if (e131.universe == UNIVERSE) {
      BRIGHTNESS = map(e131.data[(CHANNEL_START  + 0)], 0, 255, 0, 50); // ChangeME
      JUMP = map(e131.data[(CHANNEL_START  + 1)], 0, 255, 5, 40);
      SPEED = map(e131.data[(CHANNEL_START  + 2)], 0, 255, 1000, 20);
      if (e131.data[(CHANNEL_START  + 3)] < 125) {
        INWARD = true;
      }
      else {
        INWARD = false;
      }
      Serial.print("e");
      //Serial.print(e131.data[0]);
    }
  }
  FastLED.setBrightness(BRIGHTNESS);

  EVERY_N_SECONDS( 2 ) {
    Serial.print("BRIGHTNESS = ");
    Serial.print(BRIGHTNESS);

    Serial.print("  JUMP = ");
    Serial.print(JUMP);

    Serial.print("  SPEED = ");
    Serial.print(SPEED);

    Serial.print("  INWARD = ");
    Serial.print(INWARD);

    Serial.println();
  }

  rings();
}

void rings() {
  for (int r = 2; r <= RINGS; r++) {
    setRing(r, CHSV(hue[(r - 1)], 255, 255));
  }
  FastLED.delay(SPEED);
  if (INWARD) {
    for (int r = 2; r < RINGS; r++) {
      hue[(r - 1)] = hue[r];
    }
    hue[(RINGS - 1)] += JUMP;
  }
  else {
    for (int r = (RINGS - 1); r > 1; r--) {
      hue[r] = hue[(r - 1)];
    }
    //    if(random(0,10) > 2) {
    hue[1] += JUMP;
    //    }
    //    else {
    //      hue[1] += random(0,255);
    //    }
  }
}

void setRing(int ring, CRGB colour) {
  int offset = 1;
  int count = 0;
  switch (ring) {
    case 1:
      count = 0;
      break;
    case 2:
      offset = 1;
      count = 8;
      break;
    case 3:
      offset = 9;
      count = 12;
      break;
    case 4:
      offset = 21;
      count = 16;
      break;
    case 5:
      offset = 37;
      count = 24;
      break;
    case 6:
      offset = 61;
      count = 32;
      break;
  }
  for (int i = 0; i < count; i++) {
    leds[(offset + i - 1)] = colour;
  }
}

#include <ESP8266WiFi.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <qdec.h>

#define SDA_PIN 4 //d2
#define SCL_PIN 5 //d1 

#define ENC_PIN_CLK (uint16_t)12 //d6
#define ENC_PIN_DAT (uint16_t)13 //d7
#define ENC_PIN_SW  15 //d8
#define SW_HELPER   14 //d5
#define OLED_RESET -1
Adafruit_SSD1306 display(128, 64, &Wire, OLED_RESET);
volatile int rotary_pos = 0;
volatile int click_count = 0;
int previous_pos = 0;
int previous_count = 0;

::SimpleHacks::QDecoder decoder(ENC_PIN_CLK, ENC_PIN_DAT, true);

ICACHE_RAM_ATTR void DecoderISR(void) {
  ::SimpleHacks::QDECODER_EVENT event = decoder.update();
  if (event & ::SimpleHacks::QDECODER_EVENT_CW){
    rotary_pos = rotary_pos + 1;
  }
  else if (event & ::SimpleHacks::QDECODER_EVENT_CCW){
    rotary_pos = rotary_pos - 1;
  }
  return;
}

ICACHE_RAM_ATTR void ButtonISR(void) {
  if (digitalRead(ENC_PIN_SW) == LOW){
    click_count = click_count + 1;
  }
  return;
}

void disp_init(Adafruit_SSD1306 *display) {
  display->begin(SSD1306_SWITCHCAPVCC, 0x3c, true);
  display->display();
  delay(100);
  display->clearDisplay();
  display->fillRect(0, 0, display->width(), display->height(), WHITE);
  display->setTextSize(1);
  display->setTextColor(BLACK, WHITE);
  display->setCursor(2,7);
  display->println("WEMOTE");
  display->setCursor(100, 17);
  display->println("Pro");
  display->display();
}
void setup() {
  // put your setup code here, to run once:
  disp_init(&display);
  decoder.begin();
  delay(2000);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.display();
  attachInterrupt(digitalPinToInterrupt(ENC_PIN_CLK), DecoderISR, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENC_PIN_DAT), DecoderISR, CHANGE);
  pinMode(ENC_PIN_SW, INPUT_PULLUP);
  pinMode(SW_HELPER, OUTPUT);
  digitalWrite(SW_HELPER, HIGH);
  attachInterrupt(digitalPinToInterrupt(ENC_PIN_SW), ButtonISR, FALLING);
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(100);
  bool changed = (click_count != previous_count)||(rotary_pos != previous_pos);
  if (changed) {
    int npos = rotary_pos;
    int nclick = click_count;
    display.clearDisplay();
    display.setCursor(2, 7);
    display.println(nclick);
    display.setCursor(2, 17);
    display.println(npos);
    display.display();
    previous_count = nclick;
    previous_pos = npos;
  }

}

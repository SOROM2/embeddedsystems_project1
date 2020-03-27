#include <DMD2.h>
#include <fonts/Arial14.h>
#include <fonts/SystemFont5x7.h>

#define WIDTH 1
#define HEIGHT 1

enum mode {
    celclius,
    fahrenheit,
    both
};

#define TEMP_PIN 0
#define DELAY_MS 2000

const uint8_t* FONT_1ROW = Arial14;
const uint8_t* FONT_2ROW = SystemFont5x7;
enum mode current_mode;

SoftDMD dmd(WIDTH, HEIGHT);

void setup() {
   dmd.setBrightness(255);
   dmd.begin();
   dmd.selectFont(FONT_1ROW);
}

void loop() {
    dmd.drawString(0,0, "Hello, World!");
    delay(DELAY_MS);
}


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

/**
 * Changes the global mode and the font for the mode
 * 
 * Param 1 is a mode enum which decides the font to be used
 */
void change_mode(enum mode m) {
    current_mode = m;
    dmd.selectFont(m = both ? FONT_2ROW : FONT_1ROW);
}

/**
   Code from: https://www.jaycar.co.nz/medias/sys_master/images/images/9369168412702/XC4494-manualMain.pdf
*/
double thermister(int RawADC) {
  double Temp;
  Temp = log(((10240000 / RawADC) - 10000));
  Temp = 1 / (0.001129148 + (0.000234125 + (0.0000000876741 * Temp * Temp )) * Temp );
  Temp -= 273.15;
  return Temp;
}

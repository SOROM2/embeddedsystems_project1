#include <stdlib.h>
#include <string.h>
#include <DMD2.h>
#include <fonts/Arial14.h>
#include <fonts/SystemFont5x7.h>

#define WIDTH 1
#define HEIGHT 1

enum mode {
    celcius,
    fahrenheit,
    both
};

#define TEMP_PIN 0
#define DELAY_MS 2000
#define BUFFER_LEN 8

const uint8_t* FONT_1ROW = Arial14;
const uint8_t* FONT_2ROW = SystemFont5x7;

char* main_buf;

double temp_c;
double temp_f;

enum mode current_mode;

SoftDMD dmd(WIDTH, HEIGHT);

void setup() {
    main_buf = (char*)malloc(sizeof(char) * BUFFER_LEN);

    dmd.setBrightness(255);
    dmd.begin();
    change_mode(celcius);
}

void loop() {
    temp_c = thermister(analogRead(TEMP_PIN));
    temp_f = (temp_c * 1.8) + 32.0;

    double_tostr(main_buf, temp_c);
    dmd.clearScreen();
    dmd.drawString(0,0, main_buf);
    delay(DELAY_MS);
}

/**
 * Takes a buffer and a double, converts double to string (1dp)
 * 
 * Param 1 is a pointer to a string, this contains pre-allocatem memory ready for writing.
 * Param 2 is a double, this will be converted into a string and written into the param 1's memory.
 */
void double_tostr(char* buffer, double d) {
    int d_int;
    int d_1fint;

    char* tmp = (char*)malloc(sizeof(char) * BUFFER_LEN); // allocates memory for a temporary string to go

    d_int = (int)d;                             // int d casted to an integer
    d_1fint = (int)((d - (double)d_int) * 10);  // getting the 1 decimal place value as an integer

    itoa(d_int, tmp, 10);       // convert d_int (base 10) into a char array placed in tmp
    strcpy(buffer, tmp);        // copy the string from tmp into buffer
    strcat(buffer, ".");        // concat "." to the string in buffer
    itoa(d_1fint, tmp, 10);     // convert d_1fint (base10) into a char array placed in tmp
    strcat(buffer, tmp);        // concat the string in tmp to the string in buffer

    free(tmp);  // free the allocated memory for tmp
}

/**
 * Changes the global mode and the font for the mode
 * 
 * Param 1 is a mode enum which decides the font to be used.
 */
void change_mode(enum mode m) {
    current_mode = m;
    dmd.selectFont(m == both ? FONT_2ROW : FONT_1ROW);
}

/**
 * Code from: https://www.jaycar.co.nz/medias/sys_master/images/images/9369168412702/XC4494-manualMain.pdf
 */
double thermister(int RawADC) {
    double Temp;
    Temp = log(((10240000 / RawADC) - 10000));
    Temp = 1 / (0.001129148 + (0.000234125 + (0.0000000876741 * Temp * Temp )) * Temp );
    Temp -= 273.15;
    return Temp;
}

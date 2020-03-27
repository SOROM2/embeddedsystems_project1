#include <stdlib.h>
#include <string.h>
#include <DMD2.h>
#include <fonts/Arial14.h>
#include <fonts/SystemFont5x7.h>

#define WIDTH 1
#define HEIGHT 1

/**
 * Enumeration for modal display
 */
enum mode {
    celcius,
    fahrenheit,
    both
};

#define TEMP_PIN 0
#define DELAY_MS 2000
#define BUFFER_LEN 8
#define C_CHAR "C"
#define F_CHAR "F"

const uint8_t* FONT_1ROW = Arial14;
const uint8_t* FONT_2ROW = SystemFont5x7;

char* main_buf;
char* row_1;
char* row_2;

double temp_c;
double temp_f;

enum mode current_mode;

SoftDMD dmd(WIDTH, HEIGHT);

void setup() {
    // allocate memory for row buffers
    row_1 = (char*)malloc(sizeof(char) * BUFFER_LEN);
    row_2 = (char*)malloc(sizeof(char) * BUFFER_LEN);
    main_buf = row_1; // row 1 is also the main buffer

    // default the mode to both, set matrix led brightness
    set_mode(both);
    dmd.setBrightness(255);

    dmd.begin();
}

void loop() {
    // get celcius and fahrenheit temps
    temp_c = thermister(analogRead(TEMP_PIN));
    temp_f = (temp_c * 1.8) + 32.0;

    format_string();
    dmd.clearScreen();
    draw_screen();

    delay(DELAY_MS);
}

/**
 * Draws either 1 row or both depending on the current mode.
 * If mode is both, the draw row 2 then draw row 1
 * Any other mode state will only draw row 1.
 */
void draw_screen() {
    switch(current_mode) {
        case both:
            dmd.drawString(1, 9, row_2);
        default:
            dmd.drawString(1, 0, row_1);
            break;
    }
}

/**
 * Prepares buffers for display depending on the current mode.
 */
void format_string() {
    switch (current_mode) {
        case celcius:
            double_tostr(main_buf, temp_c);
            strcat(main_buf, C_CHAR);
            break;
        case fahrenheit:
            double_tostr(main_buf, temp_f);
            strcat(main_buf, F_CHAR);
            break;
        default:
            double_tostr(row_1, temp_c);
            strcat(row_1, C_CHAR);
            double_tostr(row_2, temp_f);
            strcat(row_2, F_CHAR);
            break;
    }
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
void set_mode(enum mode m) {
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

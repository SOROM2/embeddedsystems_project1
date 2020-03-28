#include <stdlib.h> // for malloc and free
#include <string.h> // for strcpy and strcat
#include <DMD2.h>   // dotmatrix library

// dotmatrix fonts
#include <fonts/Arial14.h>
#include <fonts/SystemFont5x7.h>

// dmd2 daisychain constants
#define WIDTH 1
#define HEIGHT 1

/**
 * Enumeration for modal display
 * 
 * len is always at the end which should represent the amount of modes in the enum,
 * this is so if a mode is added or removed, the next_mode function will always account for this.
 */
enum mode {
    celcius,
    fahrenheit,
    both,
  
    len
};

// constant compile time values
#define TEMP_PIN 0
#define BUTTON_PIN 2
#define BUTTON_DEBOUNCE 200
#define DELAY_MS 2000
#define BUFFER_LEN 8
#define C_CHAR "C"
#define F_CHAR "F"
#define SP_CHAR " "

// constant runtime values
const uint8_t* FONT_1ROW = Arial14;
const uint8_t* FONT_2ROW = SystemFont5x7;

// string buffer pointers
char* main_buf;
char* row_1;
char* row_2;

// temperatures
double voltage;
double temp_c;
double temp_f;

unsigned long start_time;       // for button delays

enum mode current_mode; // for state machine

SoftDMD dmd(WIDTH, HEIGHT);

void setup() {
    pinMode(BUTTON_PIN, INPUT_PULLUP);

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
    // get celcius and fahrenheit temps (from SIK_Circuit_4B)
    voltage = analogRead(TEMP_PIN) * 0.004882814;   // convert analog to voltage from 0-5v
    temp_c = (voltage - 0.5) * 100.0;       // convert the voltage to a temperature in degrees Celsius
    temp_f = temp_c * (9.0/5.0) + 32.0;   // convert the voltage to a temperature in degrees Fahrenheit

    // format the displayed buffers and draw the screen
    format_string();
    draw_screen();

    // delay & wait for button presses
    button_delay(DELAY_MS);
}

/**
 * Increments the enum integer, rolling through each item of the enum
 */
void next_mode() {
    enum mode new_mode = (enum mode)((current_mode + 1) % len);
    set_mode(new_mode);
}

/**
 *  Creates a delay of limit long, whilst checking for button presses.
 *  If the button is pressed, call next_mode then display the new mode.
 *  Debounce added because without it, the mode changes many times per buttonclick.
 */
void button_delay(unsigned long limit) {
    start_time = millis();

    while (millis() - start_time < limit) {
        if (digitalRead(BUTTON_PIN) == LOW) {
            next_mode();
            format_string();
            draw_screen();
            delay(BUTTON_DEBOUNCE);
        }
    }
}

/**
 * Draws either 1 row or both depending on the current mode.
 * If mode is both, the draw row 2 then draw row 1
 * Any other mode state will only draw row 1.
 */
void draw_screen() {
    dmd.clearScreen();

    switch (current_mode) {
        case celcius:
            dmd.drawString(1, 2, main_buf);
        case fahrenheit:
            dmd.drawString(1, 2, main_buf);
            break;
        default:
            dmd.drawString(1, 0, row_1);
            dmd.drawString(1, 9, row_2);
            break;
    }
}

/**
 * Prepares buffers for display depending on the current mode.
 */
void format_string() {
    switch (current_mode) {
        case celcius:
            double_tostr(main_buf, temp_c, true);
            strcat(main_buf, C_CHAR);
            break;
        case fahrenheit:
            double_tostr(main_buf, temp_f, true);
            strcat(main_buf, F_CHAR);
            break;
        default:
            align_point(row_1, temp_c);
            strcat(row_1, C_CHAR);

            align_point(row_2, temp_f);
            strcat(row_2, F_CHAR);
            break;
    }
}

/**
 * Ensures the digits before the '.' in the buffer will be 2 chars.
 *
 * Param 1 is a pointer to a buffer, this is where the string will end up.
 * Param 2 is a double, this is the value to be formatted into the buffer.
 */
void align_point(char* buffer, double d) {
    if ((d < 10.0) && (d >= 0.0)) {
        strcpy(buffer, SP_CHAR);
        double_tostr(buffer, d, false);
    } else {
        double_tostr(buffer, d, true);
    }
}

/**
 * Takes a buffer and a double, converts double to string (1dp)
 *
 * Param 1 is a pointer to a string, this contains pre-allocatem memory ready for writing.
 * Param 2 is a double, this will be converted into a string and written into the param 1's memory.
 * Param 3 is a bool, this decides if the formatted input should initially be strcat or strcpy into the buffer.
 */
void double_tostr(char* buffer, double d, bool copy) {
    int d_int;
    int d_1fint;
    
    char* tmp = (char*)malloc(sizeof(char) * BUFFER_LEN); // allocates memory for a temporary string to go
    
    d_int = (int)d;                             // int d casted to an integer
    d_1fint = (int)((d - (double)d_int) * 10);  // getting the 1 decimal place value as an integer
    
    itoa(d_int, tmp, 10);       // convert d_int (base 10) into a char array placed in tmp
    if (copy) {
        strcpy(buffer, tmp);        // copy the string from tmp into buffer
    } else {
        strcat(buffer, tmp);        // copy the string from tmp into buffer
    }
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


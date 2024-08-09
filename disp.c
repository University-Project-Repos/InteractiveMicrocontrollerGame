/**
* @file     disp.c
* @authors  Courtney Bracefield (and/or tutor(s)), and Adam Ross
* @date     12 Oct 2016
* @brief    C program for an interactive memory game between microcontrollers - displaying of chars
*/

#include "tinygl.h"
#include "../fonts/font3x5_1.h"
#include "../fonts/font5x7_1.h"
#include "play.h"

#define ONE 'A'
#define TWO 'B'
#define THREE 'C'

/**
 * Changes tinygl settings for displaying single chars
 */
void setStaticDisplay ( void ) {
    tinygl_font_set ( &font5x7_1 );
    tinygl_text_mode_set ( TINYGL_TEXT_MODE_STEP );
    tinygl_text_dir_set ( TINYGL_TEXT_DIR_NORMAL );
}

/**
 * Changes tinygl settings for displaying scrolling strings
 */
void setScrollDisplay ( void ) {
    tinygl_font_set ( &font3x5_1 );
    tinygl_text_mode_set ( TINYGL_TEXT_MODE_SCROLL );
    tinygl_text_dir_set ( TINYGL_TEXT_DIR_ROTATE );
}

/**
 * Displays a single char to LED.
 * If char is 'A', 'B', 'C', converts to a corresponding
 * number between 1-3 for displaying the game difficulty.
 */
void displayChar ( char *dispChar ) {
    char newChar;

    if ( *dispChar == ONE ) {
        newChar = '1';
    } else if ( *dispChar == TWO ) {
        newChar = '2';
    } else if ( *dispChar == THREE ) {
        newChar = '3';
    } else {
        newChar = *dispChar;
    }

    tinygl_clear();
    tinygl_draw_char ( newChar, tinygl_point ( 0, 0 ) );
    tinygl_update ();
}

/**
 * Displays a constant to LED
 */
void displayConst ( char constant ) {
    tinygl_clear ();
    tinygl_draw_char ( constant, tinygl_point ( 0, 0 ) );
    tinygl_update ();
}

/**
 * Displays a scrolling string of chars
 */
void displayString ( const char str[] ) {
    tinygl_clear ();
    tinygl_text ( str );
    tinygl_update ();
}

/**
 * Converts tinygl settings for scrolling strings, displays a
 * scrolling string continuously until navswitch is pushed
 * then converts tinygl settings back to displaying chars
 */
void continuousScroll ( const char str[] ) {
    setScrollDisplay ();
    tinygl_clear ();
    displayString ( str );
    navPushUpdate ();
    setStaticDisplay ();
}

/**
 * Displays a scroll of text
 */
void singleScroll ( const char str[] ) {
    setScrollDisplay ();
    tinygl_clear ();
    displayString ( str );
    setStaticDisplay ();
}

/**
 * Initialiser for tinygl and scrolling string setter for game start
 */
void displayInit ( int loopRate ) {
    tinygl_init ( loopRate );
    setScrollDisplay ();
}

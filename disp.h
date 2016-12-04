/**
* @file     disp.h
* @authors  Courtney Bracefield and Adam Ross
* @date     12 Oct 2016
* @brief    Header file for disp.c of the interactive memory game between microcontrollers - displaying of chars
*/


#ifndef DISP_H
#define DISP_H


/**
 * Changes tinygl settings for displaying single chars
 */
void setStaticDisplay ( void );


/**
 * Changes tinygl settings for displaying scrolling strings
 */
void setScrollDisplay ( void );


/**
 * Displays a single char to LED.
 * If char is 'A', 'B', 'C', converts to a corresponding
 * number between 1-3 for displaying the game difficulty.
 */
void displayChar ( char *dispChar );


/**
 * Displays a constant to LED
 */
void displayConst ( char constant );


/**
 * Displays a scrolling string of chars
 */
void displayString ( const char str[] );


/**
 * Converts tinygl settings for scrolling strings, displays a
 * scrolling string continuously until navswitch is pushed
 * then converts tinygl settings back to displaying chars
 */
void continuousScroll ( const char str[] );


/**
 * Displays a scroll of text
 */
void singleScroll ( const char str[] );


/**
 * Initialiser for tinygl and scrolling string setter for game start
 */
void displayInit ( int loopRate );
#endif

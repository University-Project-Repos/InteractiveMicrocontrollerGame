/**
* @file     game.c
* @authors  Adam Ross
* @date     12 Oct 2016
* @brief    C program for an interactive memory game between microcontrollers - main
*/

#include "system.h"
#include "led.h"
#include "ir_uart.h"
#include "pacer.h"
#include "navswitch.h"
#include "tinygl.h"
#include "disp.h"
#include "play.h"
#include <stdbool.h>

#define PACER_RATE 300
#define SENDER '$'
#define RECEIVER 'R'
#define NO_PLAY 'V'

int main ( void ) {
    system_init ();
    ir_uart_init ();
    navswitch_init ();
    pacer_init ( PACER_RATE );
    displayInit ( PACER_RATE );
    led_init ();

    ///Boolean initialisers for whether the game is won and for confirmed receptions of infra-red transmitted data
    bool gameWon = false, parametersTransmitted = false;

    ///Char initialiser for game-play turn
    char play = NO_PLAY;

    while ( 1 ) {
        pacer_wait ();
        tinygl_update ();
        navswitch_update ();

        while ( !gameWon ) {
            pacer_wait ();
            tinygl_update ();

            if ( play == RECEIVER ) {
                if ( !parametersTransmitted ) {
                    setGameParameters ( &parametersTransmitted );
                }

                if ( parametersTransmitted ) {
                    playerTurnReceiver ( &gameWon, &parametersTransmitted, &play );
                }
            }

            if ( play == SENDER ) {
                playerTurnSender ( &parametersTransmitted, &play );
            }

            if ( play == NO_PLAY ) {
                gameStart ( &play, PACER_RATE );
            }
        }

        gameWin ( &gameWon, &play );
    }
}

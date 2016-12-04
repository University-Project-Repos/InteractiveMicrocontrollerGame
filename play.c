/**
* @file     play.c
* @authors  Adam Ross
* @date     12 Oct 2016
* @brief    C program for an interactive memory game between microcontrollers - gameplay
*/

#include "ir_uart.h"
#include "pacer.h"
#include "led.h"
#include "pio.h"
#include "navswitch.h"
#include "tinygl.h"
#include "disp.h"
#include <stdbool.h>

#define MAXIMUM_DIRECTIONS 8
#define MAX_SPEED 200
#define PAUSE 400
#define LVL_ONE 'A'
#define LVL_TWO 'B'
#define LVL_THREE 'C'
#define CHANGE_PLAY 'Y'
#define START_NEW_GAME 'V'
#define RESET '-'
#define RESIGN 'X'
#define NORTH 'N'
#define SOUTH 'S'
#define EAST 'E'
#define WEST 'W'
#define SENDER '$'
#define RECEIVER 'R'

const char GAME_FAIL[] = "YOU FAILED";
const char GAME_WIN[] = "YOU WON!";
const char LVL_CHOOSE[] = "CHOOSE LEVEL 1-3";
const char GAME_START[] = "START GAME";
const char RECEIVER_START[] = "PLAY DIRECTIONS";
const char LEVEL_WON[] = "GAME LEVEL WON!";
const char GO[] = "GO";

char directionsArray[MAXIMUM_DIRECTIONS] = { 0 };

char difficulty = LVL_ONE, gameLevel = LVL_ONE, count = 'A', direction;

int score = 0, directionsTransmitted = 0, numberOfDirections = MAXIMUM_DIRECTIONS / 2, displaySpeed;

bool activityComplete = false, parametersConfirmed = false, allDirectionsRepeated = false, recepted = true;

/**
 * Because the game difficulty chosen by SENDER at game start is
 * converted to a corresponding char for data transmission,
 * it must be re-converted back to an interger for game play
 * parameter setting after infra-red reception at RECEIVER board
 */
void convertDifficultyToInt ( void ) {
    if ( difficulty == LVL_ONE ) {
        displaySpeed = PAUSE * 2 + MAX_SPEED;
    } else if ( difficulty == LVL_TWO ) {
        displaySpeed = PAUSE + MAX_SPEED;
    } else if ( difficulty == LVL_THREE ) {
        displaySpeed = MAX_SPEED;
    }
}

/**
 * Converts a char to an integer value equal to the number of
 * directions for the gameLevel level the char is representative of
 */
void convertGameLeveltoInt ( void ) {
    if ( gameLevel == LVL_ONE ) {
        numberOfDirections = MAXIMUM_DIRECTIONS / 2;
    } else if ( gameLevel == LVL_TWO ) {
        numberOfDirections = MAXIMUM_DIRECTIONS - 2;
    } else if ( gameLevel == LVL_THREE ) {
        numberOfDirections = MAXIMUM_DIRECTIONS;
    }
}

/**
 * A simple loop to delay run time of game for display purposes
 */
void timerDelay ( char *directionChar, int *time ) {
    long delay = 0;
    long delayTime = *time;

    while ( delay < delayTime ) {
        displayChar ( directionChar );
        delay++;
    }
}

/**
 * Sets a char with the max number of directions
 * for the round of the game for displaying purposes
 */
void displayNumberOfDirections ( char *charInput ) {
    if ( numberOfDirections == MAXIMUM_DIRECTIONS ) {
        *charInput = '8';
    } else if ( numberOfDirections == MAXIMUM_DIRECTIONS - 2 ) {
        *charInput = '6';
    } else if ( numberOfDirections == MAXIMUM_DIRECTIONS / 2 ) {
        *charInput = '4';
    }
}

/**
 * The SENDER player enters the directions to be sent to the RECEIVER
 * player by moving the navswitch in any direction until the total
 * number of directions to be played have been chosen.
 * So long as not all directions permitted are given, player SENDER can
 * reset the input of directions by pressing the navswitch button down.
 */
void chooseSendingDirections ( void ) {
    int inputCount = 0;
    char charInput;
    convertGameLeveltoInt ();
    displayNumberOfDirections ( &charInput );

    while ( inputCount < numberOfDirections ) {
        pacer_wait ();
        tinygl_update ();
        navswitch_update ();

        if ( navswitch_push_event_p ( NAVSWITCH_NORTH ) ) {
            charInput = directionsArray[ inputCount ] = NORTH;
            inputCount++;
        } else if ( navswitch_push_event_p ( NAVSWITCH_SOUTH ) ) {
            charInput = directionsArray[ inputCount ] = SOUTH;
            inputCount++;
        } else if ( navswitch_push_event_p ( NAVSWITCH_EAST ) ) {
            charInput = directionsArray[ inputCount ] = EAST;
            inputCount++;
        } else if ( navswitch_push_event_p ( NAVSWITCH_WEST ) ) {
            charInput = directionsArray[ inputCount ] = WEST;
            inputCount++;
        } else if ( navswitch_push_event_p ( NAVSWITCH_PUSH ) ) {
            charInput = RESET;
            inputCount = 0;
        }
        displayChar ( &charInput );
    }
}

/**
 * Checks if each attempt made at repeating direction by RECEIVER player
 * is a correct choice, and increments score if so, and attempt count
 */
void attemptEvaluation ( char *attempt, int *attemptNum ) {
    displayChar ( attempt );

    if ( directionsArray[ *attemptNum ] == *attempt ) {
        score++;
    }
    *attemptNum += 1;
}

/**
 * Player RECEIVER attempts to repeat directions given by player SENDER.
 * A score count is implemented for each successful direction repeated
 * by player RECEIVER and the final score is returned once all attempts at
 * repeating the given directions have been completed.
 */
void repeatDirections ( void ) {
    int attemptCount = 0, pause = PAUSE;
    char repeatAttempt = '*';
    continuousScroll ( GO );
    tinygl_clear ();

    while ( attemptCount < numberOfDirections ) {
        pacer_wait ();
        navswitch_update ();
        tinygl_update ();

        if ( navswitch_push_event_p ( NAVSWITCH_NORTH ) ) {
            repeatAttempt = NORTH;
            attemptEvaluation ( &repeatAttempt, &attemptCount );
        } else if ( navswitch_push_event_p ( NAVSWITCH_SOUTH ) ) {
            repeatAttempt = SOUTH;
            attemptEvaluation ( &repeatAttempt, &attemptCount );
        } else if ( navswitch_push_event_p ( NAVSWITCH_EAST ) ) {
            repeatAttempt = EAST;
            attemptEvaluation ( &repeatAttempt, &attemptCount );
        } else if ( navswitch_push_event_p ( NAVSWITCH_WEST ) ) {
            repeatAttempt = WEST;
            attemptEvaluation ( &repeatAttempt, &attemptCount );
        }

        if (navswitch_push_event_p ( NAVSWITCH_PUSH ) ) {
            repeatAttempt = RESIGN;
            attemptCount = numberOfDirections;
        }
        displayChar ( &repeatAttempt );
    }
    timerDelay ( &repeatAttempt, &pause );
}

/**
 * Loops until the nav button has been pushed. This is used for when
 * the game is awaiting upon a player's activation for continuation.
 */
void navPushUpdate ( void ) {
    bool navPush = false;

    while ( !navPush ) {
        pacer_wait ();
        navswitch_update ();
        tinygl_update ();

        if ( navswitch_push_event_p ( NAVSWITCH_PUSH ) ) {
            navPush = true;
        }
    }
}

/**
 * Player SENDER chooses the level of difficulty of the game from 1 to 3;
 * easiest to hardest. The level of difficulty determines temporal
 * speed of which the directions are displayed to player RECEIVER board.
 */
void chooseGameDifficulty ( void ) {
    bool pushed = false;
    difficulty = LVL_ONE;

    while ( !pushed ) {
        pacer_wait ();
        tinygl_update ();
        navswitch_update ();

        if ( navswitch_push_event_p ( NAVSWITCH_EAST ) ) {
            if ( difficulty < LVL_THREE ) {
                difficulty++;
            }
        } else if ( navswitch_push_event_p ( NAVSWITCH_NORTH ) ) {
            if ( difficulty < LVL_THREE ) {
                difficulty++;
            }
        } else if ( navswitch_push_event_p ( NAVSWITCH_SOUTH ) ) {
            if ( difficulty > LVL_ONE ) {
                difficulty--;
            }
        } else if ( navswitch_push_event_p ( NAVSWITCH_WEST ) ) {
            if ( difficulty > LVL_ONE ) {
                difficulty--;
            }
        } else if ( navswitch_push_event_p ( NAVSWITCH_PUSH ) ) {
            pushed = true;
        }
        displayChar ( &difficulty );
    }
    convertDifficultyToInt ();
}

/**
 * Starts the game when navswitch button on either board is pushed.
 * Board that navswitch button is pressed becomes player SENDER and
 * a message "R" is transmitted to the other board for declaration
 */
void gameStart ( char *player, int loopRate ) {
    bool gameStart = false;
    int flasher = 0;
    char receptionChar;
    displayString ( GAME_START );
    pio_config_set ( LED1_PIO, PIO_OUTPUT_HIGH );

    while ( !gameStart ) {
        pacer_wait ();
        navswitch_update ();
        tinygl_update ();
        flasher++;

        if ( flasher > loopRate / 2 ) {
            flasher = 0;
            pio_output_toggle ( LED1_PIO );
        }

        if ( navswitch_push_event_p ( NAVSWITCH_PUSH ) ) {
            gameStart = true;
            *player = SENDER;
            setStaticDisplay ();
            ir_uart_putc ( RECEIVER );
        } else if ( ir_uart_read_ready_p () ) {
            receptionChar = ir_uart_getc ();

            if ( receptionChar == RECEIVER ) {
                gameStart = true;
                *player = RECEIVER;
                setStaticDisplay ();
                displayChar ( &receptionChar );
            }
        }
    }
    led_set ( LED1, 0 );
}

/**
 *  Player RECEIVER receives the game difficulty chosen by player SENDER,
 * 	as a char and sets the difficulty to determine the temporal rate
 *  for the display of each recepted direction from teh SENDER board.
 * 	A confirmation package is transmitted back to the SENDER board.
 */
void setGameParameters ( bool *confirmation ) {
    if ( ir_uart_read_ready_p () ) {
        difficulty = ir_uart_getc ();

        if ( ( difficulty == LVL_ONE ) || ( difficulty == LVL_TWO ) || ( difficulty == LVL_THREE ) ) {
            convertDifficultyToInt ();
            activityComplete = false;
            *confirmation = true;
            ir_uart_putc ( CHANGE_PLAY );
        }
    }
}

/**
 * Displays a count down from 3 to 1 before directions are displayed.
 */
void countDown ( void ) {
    char counter = LVL_THREE;
    int maxTime = PAUSE;

    while ( counter >= LVL_ONE ) {
        displayChar ( &counter );
        timerDelay ( &counter, &maxTime );
        counter--;
    }
}

/**
 * Displays all of the directions transmitted to the RECEIVER board after
 * each transmission of directions has been successfully completed.
 */
void directionDisplay ( void ) {
    int pause = displaySpeed;

    while ( directionsTransmitted < numberOfDirections ) {
        displayChar ( &directionsArray[ directionsTransmitted ] );
        timerDelay ( &directionsArray[ directionsTransmitted ], &pause );
        directionsTransmitted++;
        //timer_wait ( ( timer_tick_t ) ( TIMER_RATE * .03 ) );
    }
    display_clear ();
    directionsTransmitted = 0;
}

/**
 * Reception of infra-red transmitted data packages from SENDER board.
 * For every data package recepted that contains either a N, S, E, or W
 * char, an equal data package is transmitted back to the SENDER board
 * for confirmation of each direction transmitions success.
 */
void directionReception ( bool *transmitted ) {
    if ( directionsTransmitted < numberOfDirections ) {
        if ( ir_uart_read_ready_p () ) {
            direction = ir_uart_getc ();

            if ( ( direction == NORTH ) || ( direction == SOUTH ) || ( direction == EAST ) || ( direction == WEST ) ) {
                directionsArray[ directionsTransmitted ] = direction;
                directionsTransmitted++;
                ir_uart_putc ( direction );
            }
        }
    }

    if ( directionsTransmitted == numberOfDirections ) {
        *transmitted = false;
        directionsTransmitted = 0;
    }
}

/**
 * Data packages containing chars representative of chosen directions
 * are infra-red transmitted from the SENDER board to the RECEIVER board.
 * Waits for confirmation of each data package transmission before
 * continuing to transmit the next until all directions transmitted.
 */
void transmitDirections ( bool *transmitted ) {
    if ( ( directionsTransmitted > 0 ) && ( !recepted ) ) {
        if ( ir_uart_read_ready_p () ) {
            char confirmationChar = ir_uart_getc ();

            if ( confirmationChar  == directionsArray[ directionsTransmitted - 1 ] ) {
                recepted = true;
            }
        }
    }

    if ( ( directionsTransmitted < numberOfDirections ) && ( recepted ) ) {
        ir_uart_putc ( directionsArray[ directionsTransmitted ] );
        directionsTransmitted++;
        recepted = false;
    }

    if ( directionsTransmitted == numberOfDirections && ( !recepted ) ) {
        *transmitted = true;
        recepted = true;
        directionsTransmitted = 0;
    }
}

/**
 * Sets booleans to false for the next level of game play,
 * or for game restart.
 */
void endOfSenderPlay ( bool *transmitted ) {
    parametersConfirmed = false;
    activityComplete = false;
    *transmitted = false;
}

/**
 * SENDER player chooses game difficulty at game start and transmits
 * the choice to RECEIVER player, then awaits a confirmation from RECEIVER.
 * Othwerwise, if game started, awaits a transmition of level increase
 */
void senderParameterSetting ( bool *transmitted ) {
    if ( !*transmitted ) {
        led_set ( LED1, 1 );

        if ( ( !activityComplete ) && ( gameLevel == LVL_ONE ) ) {
            continuousScroll ( LVL_CHOOSE );
            chooseGameDifficulty ();
            tinygl_clear ();
            ir_uart_putc ( difficulty );
            activityComplete = true;
        }
    }

    if ( ir_uart_read_ready_p () ) {
        char reception = ir_uart_getc ();

        if ( reception == CHANGE_PLAY ) {
            *transmitted = true;
            gameLevel = LVL_ONE;
        }
    } else if ( gameLevel > LVL_ONE ) {
        *transmitted = true;
        activityComplete = true;
    }

    if ( *transmitted ) {
        parametersConfirmed = true;
        activityComplete = false;
        *transmitted = false;
    }
}

/**
 * Player SENDER sends chosen directions to player RECEIVER then
 * awaits transmission from player RECEIVER of game play outcome
 */
void senderGamePlay ( bool *transmitted, char *play ) {
    if ( !*transmitted ) {
        if ( !activityComplete ) {
            chooseSendingDirections ();
            activityComplete = true;
        }
        transmitDirections ( transmitted );
        displayConst ( SENDER );
    }

    if ( *transmitted ) {
        led_set ( LED1, 0 );

        if ( ir_uart_read_ready_p () ) {
            char reception = ir_uart_getc ();

            if ( reception == CHANGE_PLAY ) {
                endOfSenderPlay ( transmitted );
            } else if ( ( reception == LVL_TWO ) || ( reception == LVL_THREE ) ) {
                endOfSenderPlay ( transmitted );
                gameLevel = reception;
            } else if ( reception == RECEIVER )
            {
                *play = RECEIVER;
                displayConst ( RECEIVER );
                gameLevel = LVL_ONE;
                endOfSenderPlay ( transmitted );
            }
        }
    }
}

/**
 * Player SENDER game play schematics
 */
void playerTurnSender ( bool *transmitted, char *play ) {
    if ( !parametersConfirmed ) {
         senderParameterSetting ( transmitted );
    }

    if ( parametersConfirmed ) {
        senderGamePlay (transmitted, play);
    }
}

/**
 * Player RECEIVER receives transmitted directions, displays them at
 * a temporal rate dependent on game difficulty, and then attempts
 * to repeat them in the same order as displayed for score points.
 */
void receiverGamePlay ( bool *transmitted ) {
    if ( *transmitted ) {
        convertGameLeveltoInt ();
        directionReception ( transmitted );
    }

    if ( !*transmitted ) {
        tinygl_clear ();
        led_set ( LED1, 1 );
        continuousScroll ( RECEIVER_START );
        countDown ();
        directionDisplay ();
        repeatDirections ();
        activityComplete = true;
    }
}

/**
 * Score points from player RECEIVER's game play are compared to
 * the number of directions played. If equal, then RECEIVER wins
 * level. If RECEIVER wins three consecutive levels, wins game.
 * The status of game play continuation is determined here
 * dependent on whether RECEIVER player wins, or loses levels, or
 * wins the game.
 *
 */
void receiverPlayOutcome ( bool *gameWon, bool *transmitted, char *play ) {
    if ( score == numberOfDirections ) {
        allDirectionsRepeated = true;
    }
    score = 0;

    if ( allDirectionsRepeated ) {
        allDirectionsRepeated = false;

        if ( gameLevel == LVL_THREE ) {
            *gameWon = true;
            gameLevel = LVL_ONE;
        } else if ( gameLevel < LVL_THREE ) {
            activityComplete = false;
            *transmitted = true;
            continuousScroll ( LEVEL_WON );
            tinygl_clear ();
            gameLevel++;
            ir_uart_putc ( gameLevel );
            displayConst ( RECEIVER );
            led_set ( LED1, 0 );
        }
        activityComplete = false;
    } else {
        continuousScroll ( GAME_FAIL );
        tinygl_clear ();
        gameLevel = LVL_ONE;
        *play = SENDER;
        endOfSenderPlay ( transmitted );
        ir_uart_putc ( RECEIVER );
    }
}

/**
 * Player RECEIVER game play schematics
 */
void playerTurnReceiver ( bool *gameWon, bool *transmitted, char *play ) {
    if ( !activityComplete ) {
        receiverGamePlay ( transmitted );
    }

    if ( activityComplete ) {
        receiverPlayOutcome ( gameWon, transmitted, play );
    }
}

/**
 * Declares that the winning player has won by displaying "GAME WON!"
 * on the winning player's LED and resets the game for a new gameLevel.
 * For this new gameLevel, the player roles swap. The player playing the
 * role of RECEIVER starts playing the role of SENDER, and vice-versa.
 */
void gameWin ( bool *gameWon, char *play ) {
    *gameWon = false;
    *play = SENDER;
    ir_uart_putc ( RECEIVER );
    continuousScroll ( GAME_WIN );
}

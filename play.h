/**
* @file     play.h
* @authors  Adam Ross
* @date     12 Oct 2016
* @brief    Header file for play.c of the interactive memory game between microcontrollers - gameplay
*/

#ifndef PLAY_H
#define PLAY_H


/**
 * Loops until the nav button has been pushed. This is used for when
 * the game is waiting upon a player's activation for continuation.
 */
void navPushUpdate ( void );


/**
 * Starts the game when navswitch button on either board is pushed.
 * Board that navswitch button is pressed becomes player SENDER and
 * a message "R" is transmitted to the other board for declaration
 */
void gameStart ( char *player, int loopRate );


/**
 *  Player RECEIVER receives the game difficulty chosen by player SENDER,
 * 	as a char and sets the difficulty to determine the temporal rate
 *  for the display of each recepted direction from the SENDER board.
 * 	A confirmation package is transmitted back to the SENDER board.
 */
void setGameParameters ( bool *confirmation );


/**
 * Player SENDER game play schematics
 */
void playerTurnSender ( bool *transmitted, char *play );


/**
 * Player RECEIVER game play schematics
 */
void playerTurnReceiver ( bool *gameWon, bool *transmitted, char *play );


/**
 * Declares that the winning player has won by displaying "GAME WON!"
 * on the winning player's LED and resets the game for a new gameLevel.
 * For this new gameLevel, the player roles swap. The player playing the
 * role of RECEIVER starts playing the role of SENDER, and vice-versa.
 */
void gameWin ( bool *gameWon, char *play );
#endif

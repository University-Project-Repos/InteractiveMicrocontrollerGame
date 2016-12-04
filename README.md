# ENCE260 Microcontroller Assignment   

Simple embedded C program for an interactive, multi-tiered memory game between UCFK4 microcontrollers using real-time infrared communication.

My first project implementing C, embedded microcontrollers, and basically in fulfilling user-specified requirements (with exception to underlying project specifications) to a "Simon Say's" theme.

### Team
* Courtney Bracefield
* Adam Ross

# Instructions

## Game Play

* First of the two boards to have the nav-switch button pushed when START GAME is scrolling across the display starts the game as Player "SENDER" - the nav-switch is used for all actions.
* Player "SENDER" chooses a difficulty level by moving the nav-switch N/E to increment difficulty selection, or S/W to decrement difficulty selection.
* Player "SENDER" then chooses directions to infrared-transmit to Player "RECEIVER" by moving the nav-switch 'N', 'E', 'S', or 'W' a maximum number of directions respective of the selected difficulty level.
* After pressing the nav-switch button when PLAY DIRECTIONS is scrolling on the display and the counter expires, Player "RECEIVER" then repeats the displayed directions transmitted by Player "SENDER".
* If Player "RECEIVER" repeats each direction in sequential order of instruction for three consecutive gameplay-levels of incrementing difficulty, or fails to in any one attempt, the boards swap player roles and play restarts.
* For each board that is active in play, an LED light will be on, and the other board will be "locked-down" with either a '$' or 'R' char dependent on player "SENDER" and "RECEIVER" role to indicate this, respectively.

## Requirements

* 2 UCFK4 microcontrollers
* Excluded driver files, etc.

## Install

With a microcontroller connected to the installation device by USB-cable, execute

```bash
make clean
```

```bash
make
```

```bash
make program
```

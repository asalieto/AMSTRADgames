#ifndef _DEFINITIONS_H_
#define _DEFINITIONS_H_

#define VMEM (u8*) 0xc000
#define YOFFSET 24

#define LEVEL_DURATION 900

#define CONST_U8 *(u8 *)&
#define CONST_U16 *(u16 *)&
#define CONST_POINTER *(u16 *)&

//GAME STATES
#define GAME_STATE_INTRO       2
#define GAME_STATE_MENU        3
#define GAME_STATE_GAME        4
#define GAME_STATE_CREDITS     5
#define GAME_STATE_GAMEOVER    6
#define GAME_STATE_WIN    	   7
#define GAME_STATE_CONTROLS    8     
#define GAME_STATE_PAUSE       9
#define GAME_STATE_EXIT        10
#define INITIAL_GAME_STATE     GAME_STATE_INTRO

/*
//GAME STATES
#define GAME_STATE_MENU        2
#define GAME_STATE_HELP        3
#define GAME_STATE_GAME        4
#define GAME_STATE_EXIT        5
#define GAME_STATE_WIN         6
#define GAME_STATE_GAMEOVER    7
#define GAME_STATE_CREDITS     8
#define GAME_STATE_INTRO       9
#define INITIAL_GAME_STATE     GAME_STATE_INTRO
*/
#endif
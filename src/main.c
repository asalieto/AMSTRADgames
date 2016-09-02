//-----------------------------LICENSE NOTICE------------------------------------
//  This file is part of CPCtelera: An Amstrad CPC Game Engine
//  Copyright (C) 2015 ronaldo / Fremos / Cheesetea / ByteRealms (@FranGallegoBR)
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.
//------------------------------------------------------------------------------

#include <cpctelera.h>
#include "Definitions.h"
#include "GlobalVars.h"
#include "Utils.h"
#include "MapManager.h"
#include "Render.h"
#include "Logic.h"
#include "Entities.h"
#include "Sprites.h"
#include "Musica.h"
#include "musica/music.h"

//INIT*************************************************************************************************************************

void init(){
    cpct_disableFirmware();
    cpct_setVideoMode(1);
    cpct_fw2hw(g_palette[0],4);
    cpct_fw2hw(g_palette[1],4);
    cpct_setPalette(g_palette[0],4);
    cpct_setInterruptHandler ( syncCounter );
    CONST_U8 g_state = INITIAL_GAME_STATE;
}

//MENU FUNCTIONS*************************************************************************************************************************




//GAME FUNCTIONS*************************************************************************************************************************

void initLevel(){
    CONST_U16 g_gameTimer = LEVEL_DURATION;
    cpct_setPalette(g_palette[g_currentLevel],4);
    cpct_setBorder(g_palette[g_currentLevel][2]);
    cpct_clearScreen(g_undiscoveredMapColor);
    renderInterfaceBg_Game();
    initKeyPos();
    init_Entities();
    drawOnPosition(g_Ghost.position.x,g_Ghost.position.y);
}

void newGame(){

    cpct_akp_musicInit(G_gameMusic);    // Initialize the music/////////////////
    cpct_akp_SFXInit(G_gameMusic);

    CONST_U8 g_partidaCreada=1;

    initLevel();

}

void nextLevel(){
    CONST_U8 g_currentLevel+=1;
    initLevel();
}

//INPUT*************************************************************************************************************************

void input_Game(){
    u8* memptr;
        cpct_scanKeyboard_f();
        CONST_U8(g_Ghost.dir)=g_Ghost.dir&0xF0;
        if(cpct_isKeyPressed(Key_CursorUp)){
            CONST_U8(g_Ghost.dir)=(0x01+(g_Ghost.dir&0xFE));//mascara: 1111 1110
            CONST_U8 g_inputCount+=1;
        }
        if(cpct_isKeyPressed(Key_CursorDown)){
            CONST_U8(g_Ghost.dir)=(0x02+(g_Ghost.dir&0xFD));//mascara: 1111 1101
            CONST_U8 g_inputCount+=1;
        }
        if(cpct_isKeyPressed(Key_CursorRight)){
            CONST_U8(g_Ghost.dir)=(0x04+(g_Ghost.dir&0xFB));//mascara: 1111 1011
            CONST_U8 g_inputCount+=1;
        }
        if(cpct_isKeyPressed(Key_CursorLeft)){
            CONST_U8(g_Ghost.dir)=(0x08+(g_Ghost.dir&0xF7));//mascara: 1111 0111
            CONST_U8 g_inputCount+=1;
        }
        if(cpct_isKeyPressed(Key_T)){
            plantTrap();
            CONST_U8 g_inputCount+=1;
        }
        if(cpct_isKeyPressed(Key_Esc)){
            CONST_U8 g_state = GAME_STATE_PAUSE;
            CONST_U8 g_changeState = 1;
            CONST_U8 g_inputCount+=1;
        }
}

void input_Menu(){
    
    cpct_scanKeyboard_f();
    if(cpct_isKeyPressed(Key_CursorUp) && g_frameMenuTimer >= 10){
        if(g_menuOption!=0){
            CONST_U8 g_menuOption-=1;
            CONST_U8 g_frameMenuTimer = 0;
        }
            CONST_U8 g_inputCount+=1;
    }else if(cpct_isKeyPressed(Key_CursorDown) && g_frameMenuTimer >= 10){
        if(g_menuOption!=3){
            CONST_U8 g_menuOption+=1;
            CONST_U8 g_frameMenuTimer = 0;
        }
            CONST_U8 g_inputCount+=1;
    }else if(cpct_isKeyPressed(Key_Return)){
        CONST_U8 g_changeState = 1;
            CONST_U8 g_inputCount+=1;
    }
}


//RENDER*************************************************************************************************************************


void render_Game(){
    THuman* tempH;
    u8 i=0;
    if(gt_redrawMap){//Redibujo mapa si esta el trigger activado
        CONST_U8 gt_redrawMap=0;
        CONST_U8 gt_clearSprites=0;
        // detenerCancion();
        drawMap();
        // sonarMusicaJuego();
    }
    else{
        CONST_U8 gt_clearSprites=1;
    }
    
    if(gt_redrawPosition){
        CONST_U8 gt_redrawPosition=0;
        drawOnPosition(g_Ghost.position.x,g_Ghost.position.y);
    }

    
    renderInterfaceClock_Game();
    // cpct_setBorder(g_palette[0][2]);
    if(g_SYNC25<=5){
        //cpct_akp_musicPlay();
        cpct_waitVSYNC();
        for(i=0;i<50;i++);
        // cpct_setBorder(g_palette[0][0]);
    }
    cpct_waitVSYNC();//VSYNC JUSTO ANTES DEL BORRADO PARA EVITAR FLICKERING

    // cpct_setBorder(g_palette[0][1]);
    CONST_U8 g_SYNC25=0;
   
    if(g_Ghost.trap.mapPart==g_currentMap){
        if(g_Ghost.trap.state==3){
            cpct_drawSprite(trap, g_Ghost.trap.memptr, 4, 16);
        }else if(g_Ghost.trap.state==0 || g_Ghost.trap.state==1){    
            cpct_drawSolidBox(g_Ghost.trap.memptr, 0, 4, 16);
        }
    }
        
    
    
    if(gt_clearSprites){
        cpct_drawSolidBox(g_Ghost.memptr, 0, 3, 12);
        tempH=g_Humans;
        for(i=0;i<3;i++){
        if((((tempH->distanceToGhost<=52)|| tempH->visible==1)) && ((tempH->mapPart==g_currentMap)||(tempH->lastMapPart==g_currentMap))){
                cpct_drawSolidBox(tempH->memptr, 0, 3, 12);
                CONST_U8 tempH->lastMapPart=tempH->mapPart;
            }
            tempH++;
        }
        if(((g_Ghost.distanceToKey>48)&&(g_Ghost.distanceToKey<=52)) && g_Keys_video_position[g_currentMap]!=0 && map_0_objects[g_currentMap]!=255){
            cpct_drawTileAligned2x8_f(g_blankTile, g_Keys_video_position[g_currentMap]);
        }
    }
    // cpct_setBorder(g_palette[0][0]);
    
    if(g_Ghost.distanceToKey<=48 && g_Keys_video_position[g_currentMap]!=0 && map_0_objects[g_currentMap]!=255){
        cpct_drawTileAligned2x8_f(llave, g_Keys_video_position[g_currentMap]);
    }

    tempH=g_Humans;
    for(i=0;i<3;i++){
        if(((tempH->distanceToGhost<=48) || tempH->visible==1) && (tempH->mapPart==g_currentMap)){
            CONST_POINTER(tempH->memptr) = (u16)cpct_getScreenPtr(VMEM, tempH->position.x, tempH->position.y);
            cpct_drawSprite(tempH->sprite, tempH->memptr, 3, 12);
        }
            tempH++;
    }
    
    CONST_POINTER(g_Ghost.memptr) = (u16)cpct_getScreenPtr(VMEM, g_Ghost.position.x, g_Ghost.position.y);
    cpct_drawSprite(g_Ghost.sprite, g_Ghost.memptr, 3, 12);
    // cpct_setBorder(g_palette[0][3]);
    
    
    //DEBUG
    // if(g_Humans[0].mapPart==g_currentMap){
        // printPointer(octogonalDistance(&(g_Ghost.position), &(g_Humans[0].position)), 40,8);
    // }
    // else{
        // printPointer(0, 40,8);
    // }
}

//UPDATE*************************************************************************************************************************

void update_Game(){
    update_Ghost();
    
    update_Humans();
    // printNumberDigits(g_Humans[0].mapPart,1,40,0);
    // printNumberDigits(g_Humans[1].mapPart,1,46,0);
    // printNumberDigits(g_Humans[2].mapPart,1,52,0);
    
    update_Traps();
}

void update_Counter(){

    //Aumenta el contador cada 50 frames (1 segundo) y lo imprime en la interfaz
    if(g_frameCounterTimer>25){
        CONST_U8 g_frameCounterTimer=0;
        CONST_U16 g_gameTimer-=1;
    }else{
        CONST_U8 g_frameCounterTimer+=1;
    }

    if((u16)g_gameTimer==0){
        CONST_U8 g_state = GAME_STATE_WIN;
        CONST_U8 g_changeState = 1;
        CONST_U8 g_inputCount+=1;
    }
}

void update_Menu(){

    //Aumenta el contador cada 50 frames (1 segundo) y lo imprime en la interfaz
    CONST_U8 g_frameMenuTimer+=1;
}

//STATES*************************************************************************************************************************

u8 menu(){
    u8* memptr;

    // El cursor
    u16 xCursor;
    u8 yCursor;
    u8* spriteCursor;
    u8* spriteLogo;
    xCursor=26;
    yCursor=100;
    spriteCursor=flecha_tileset[0];
    spriteLogo=logo_tileset[0];

    // Borra pantalla
    cpct_clearScreen(0);

    cpct_setBorder(g_palette[g_currentLevel][0]);
    // LOGO
    //Pinta el logo
    memptr= cpct_getScreenPtr(VMEM, 10, 32);
    cpct_drawSprite(spriteLogo, memptr, 60, 30);


    memptr= cpct_getScreenPtr(VMEM, 30, 90);
    cpct_drawStringM1_f("Start game", memptr, 1, 0);
    memptr= cpct_getScreenPtr(VMEM, 30, 110);
    cpct_drawStringM1_f("Controls", memptr, 1, 0);
    memptr= cpct_getScreenPtr(VMEM, 30, 130);
    cpct_drawStringM1_f("Credits", memptr, 1, 0);
    memptr= cpct_getScreenPtr(VMEM, 30, 150);
    cpct_drawStringM1_f("Exit", memptr, 1, 0);

    do{
        cpct_waitVSYNC();

        // Bloque input
        input_Menu();
        update_Menu();
        // sonarMusicaMenu();
        
        //Bloque render
        memptr= cpct_getScreenPtr(VMEM, xCursor, yCursor);
        cpct_drawSolidBox(memptr, 0, 2, 8);
        
        switch(g_menuOption){
            case 0: yCursor = 90; break;
            case 1: yCursor = 110; break;
            case 2: yCursor = 130; break;
            case 3: yCursor = 150; break;
        }

        //Pinta el cursor
        memptr= cpct_getScreenPtr(VMEM, xCursor, yCursor);
        cpct_drawSprite(spriteCursor, memptr, 2, 8);


    }while(g_changeState == 0);

    CONST_U8 g_changeState = 0;

    if(g_menuOption == 0){
        // detenerCancion();////////////////////////////////////////
        return GAME_STATE_GAME;
    }else if(g_menuOption == 1){
        // detenerCancion();
        return GAME_STATE_CONTROLS;
        // detenerCancion();
    }else if(g_menuOption == 2){
        return GAME_STATE_CREDITS;
        // detenerCancion();
    }else if(g_menuOption == 3){
        // detenerCancion();////////////////////////////////////
        return GAME_STATE_EXIT;
    }
    return GAME_STATE_EXIT;
}

u8 intro(){
    u8* memptr;
    u16 frameCounter=0;
    u8* spritelogointro;

    // Borra pantalla
    cpct_clearScreen(0);

    cpct_setBorder(g_palette[g_currentLevel][0]);
    spritelogointro=g_tile_logointro[0];

    // LOGO INTRO
    //Pinta el logo intro
    memptr= cpct_getScreenPtr(VMEM, 14, 24);
    cpct_drawSprite(g_tile_logointro, memptr, 50, 40);

    /*
    memptr= cpct_getScreenPtr(VMEM, 6, 24);
    cpct_drawStringM1_f("  _   _        _______    _      ", memptr, 1, 2);
    memptr= cpct_getScreenPtr(VMEM, 6, 32);
    cpct_drawStringM1_f(" | \\ | |      |__   __|  | |     ", memptr, 1, 2);
    memptr= cpct_getScreenPtr(VMEM, 6, 40);
    cpct_drawStringM1_f(" |  \\| | _____  _| | __ _| | ___ ", memptr, 1, 2);
    memptr= cpct_getScreenPtr(VMEM, 6, 48);
    cpct_drawStringM1_f(" | . ` |/ _ \\ \\/ / |/ _` | |/ _ \\", memptr, 1, 2);
    memptr= cpct_getScreenPtr(VMEM, 6, 56);
    cpct_drawStringM1_f(" | |\\  | (_) >  <| | (_| | |  __/", memptr, 1, 2);
    memptr= cpct_getScreenPtr(VMEM, 6, 64);
    cpct_drawStringM1_f(" |_| \\_|\\___/_/\\_\\_|\\__,_|_|\\___|", memptr, 1, 2);
    */

    
    memptr= cpct_getScreenPtr(VMEM, 20, 80);
    cpct_drawStringM1_f("NOXTALE PRODUCTIONS", memptr, 1, 2);

    memptr= cpct_getScreenPtr(VMEM, 30, 100);
    cpct_drawStringM1_f("Presents", memptr, 1, 0);

    // Cambiar cancion
    cpct_akp_musicInit(G_menuMusic);//////////////////
    cpct_akp_SFXInit(G_menuMusic);///////////////////////

    do{
        cpct_waitVSYNC();
        if(frameCounter>1500){
            frameCounter=0;
            return GAME_STATE_MENU;
        }
        frameCounter++;
    }while(1);
}

u8 creditos(){
 u8* memptr;
 
 //borra pantalla
 cpct_clearScreen(0);
 
 //40-(tamPalabra/2)
 //Bloque render
 memptr= cpct_getScreenPtr(VMEM, 40-8, 24);
 cpct_drawStringM1_f("Credits", memptr, 3, 0);
 
 
 memptr= cpct_getScreenPtr(VMEM, 12, 60);
 cpct_drawStringM1_f("Albert Sirvent | @piterayo", memptr, 1, 0);
 memptr= cpct_getScreenPtr(VMEM, 10, 80);
 cpct_drawStringM1_f("Alberto Salieto | @Venisir", memptr, 1, 0);
 memptr= cpct_getScreenPtr(VMEM, 14, 100);
 cpct_drawStringM1_f("Carlos Oliver | @Oli_Rull", memptr, 1, 0);;
 
 memptr= cpct_getScreenPtr(VMEM, 40-12, 160);
 cpct_drawStringM1_f("PRESS INTRO", memptr, 1, 0);
 //comprueba teclado
 do{
 cpct_waitVSYNC();
 cpct_scanKeyboard_f();
 // sonarMusicaMenu();
 }while(!cpct_isKeyPressed(Key_Return));
 CONST_U8 g_inputCount+=1;
 
 // detenerCancion();
 return GAME_STATE_MENU;
}


u8 game(){

    if(g_partidaCreada==0){
        newGame();
    }else{
        CONST_U8 gt_redrawMap=1;
        cpct_clearScreen(g_undiscoveredMapColor);
        renderInterfaceBg_Game();
    }


    do{
        input_Game();
        update_Counter();
        update_Game();
        render_Game();
    }while(g_changeState == 0);

    CONST_U8 g_changeState = 1;

    if(g_state == GAME_STATE_PAUSE){
        return GAME_STATE_PAUSE;
    }else if(g_state == GAME_STATE_GAMEOVER){
        return GAME_STATE_GAMEOVER;
    }else if(g_state == GAME_STATE_WIN){
        return GAME_STATE_WIN;
    }else{
        return GAME_STATE_MENU;
    }
}

u8 gameOver(){
 u8* memptr;
 u8 sal;
 CONST_U8 g_changeState = 0;
 //borra pantalla
 cpct_clearScreen(0);
 
 sal = 0;

 //40-(tamPalabra/2)
 //Bloque render
 memptr= cpct_getScreenPtr(VMEM, 30, 24);
 cpct_drawStringM1_f("GAME OVER", memptr, 3, 0);
 

 memptr= cpct_getScreenPtr(VMEM, 40-12, 160);
 cpct_drawStringM1_f("PRESS INTRO", memptr, 1, 0);
 //comprueba teclado
 do{
 cpct_waitVSYNC();
 cpct_scanKeyboard_f();
 if(cpct_isKeyPressed(Key_Return)){
 sal = 2;
 }
 if(cpct_isKeyPressed(Key_Esc)){
 sal = 1;
 }
 }while(sal==0);
/*
 CONST_U8 g_changeState = 1;
 CONST_U8 g_state = GAME_STATE_MENU;*/
 if(sal==1){
 sal = 0;
 return GAME_STATE_GAME;
 }else if(sal==2){
 sal = 0;
 return GAME_STATE_MENU;
 }
 //init();

}

u8 win(){
 u8* memptr;
 CONST_U8 g_changeState = 0;
 CONST_U8 g_partidaCreada = 0;
 //borra pantalla
 cpct_clearScreen(0);
 
 memptr= cpct_getScreenPtr(VMEM, 30, 24);
 cpct_drawStringM1_f("WINNER!", memptr, 3, 0);
 
 memptr= cpct_getScreenPtr(VMEM, 40-12, 160);
 cpct_drawStringM1_f("PRESS INTRO", memptr, 1, 0);
 //comprueba teclado
 do{
 cpct_waitVSYNC();
 cpct_scanKeyboard_f();
 }while(!cpct_isKeyPressed(Key_Return));

 return GAME_STATE_MENU;
}

u8 pause(){
 u8* memptr;
 CONST_U8 g_changeState = 0;
 //borra pantalla
 cpct_clearScreen(0);

 memptr= cpct_getScreenPtr(VMEM, 34, 64);
 cpct_drawStringM1_f("PAUSE", memptr, 3, 0);
 
 
 //comprueba teclado
 do{
 cpct_waitVSYNC();
 cpct_scanKeyboard_f();
 }while(!cpct_isKeyPressed(Key_Esc));

 return GAME_STATE_GAME;
}

u8 controles(){
 u8* memptr;
 
 //borra pantalla
 cpct_clearScreen(0);
 
 //40-(tamPalabra/2)
 //Bloque render
 memptr= cpct_getScreenPtr(VMEM, 40-8, 24);
 cpct_drawStringM1_f("Controls", memptr, 3, 0);
 

 memptr= cpct_getScreenPtr(VMEM, 16, 60);
 cpct_drawStringM1_f("Move: Directional arrows", memptr, 1, 0);
 memptr= cpct_getScreenPtr(VMEM, 16, 80);
 cpct_drawStringM1_f("Place trap: T", memptr, 1, 0);
 memptr= cpct_getScreenPtr(VMEM, 16, 100);
 cpct_drawStringM1_f("Exit: ESC", memptr, 1, 0);;
 
 memptr= cpct_getScreenPtr(VMEM, 40-12, 160);
 cpct_drawStringM1_f("PRESS INTRO", memptr, 1, 0);
 //comprueba teclado
 do{
 cpct_waitVSYNC();
 cpct_scanKeyboard_f();
 // sonarMusicaMenu();
 }while(!cpct_isKeyPressed(Key_Return));
 CONST_U8 g_inputCount+=1;
 // detenerCancion();
 
 return GAME_STATE_MENU;
}
//MAIN EXECUTION*************************************************************************************************************************

void main(void) {
   init();
    while (g_state != GAME_STATE_EXIT) {
        switch(g_state) {
            case GAME_STATE_INTRO:
                CONST_U8 g_state = intro();
            break;
            case GAME_STATE_MENU:
                // detenerCancion();
                CONST_U8 g_state = menu();
            break;

            case GAME_STATE_GAME:
                CONST_U8 g_state = game();
            break;

            case GAME_STATE_CREDITS:
                CONST_U8 g_state = creditos();
            break;

            case GAME_STATE_CONTROLS:
                CONST_U8 g_state = controles();
            break;

            case GAME_STATE_GAMEOVER:
                CONST_U8 g_state = win();
            break;

            case GAME_STATE_WIN:
                CONST_U8 g_state = gameOver();
            break;

            case GAME_STATE_PAUSE:
                CONST_U8 g_state = pause();
            break;

            default:
                CONST_U8 g_state = GAME_STATE_EXIT;
            break;
        }
    }
}
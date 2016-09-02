#include "Entities.h"
#include "Sprites.h"
#include "Definitions.h"
#include "GlobalVars.h"
#include "MapManager.h"
#include "Logic.h"
#include "Utils.h" //DEBUG

const TGhost g_Ghost;
const THuman g_Humans[3];

void returnToSpawner_Ghost(){
    u8 x,y;
    getSpawnPosition(g_Ghost.spawner, &x,&y);
    CONST_U8(g_Ghost.position.x)=x;
    CONST_U8(g_Ghost.position.y)=y;
    CONST_U8 g_currentMap=g_Ghost.spawner;
    CONST_U8 gt_redrawMap=1;
    CONST_U8 gt_redrawPosition=1;
}

void activateTrap(u8 i){

    // // VERSION ACTIVARLA MANUALMENTE
    // if(g_Ghost.trap.state==3){
    //     CONST_U8(g_Ghost.trap.state)=0;
    //     drawTrapIndicator();
    // }

    // VERSION SE ACTIVA AL PISARLA UN HUMANO

    THuman* tempH=g_Humans;
    u8 cont;

    cont = 0;

    for(cont;cont<i;cont++){
        tempH++;
    }

    // Efectos sobre el humano (el humano i)
    CONST_U8(tempH->visible)=1;
    CONST_U8(g_Ghost.trap.state)=0;

    drawTrapIndicator();


}

void update_Traps(){

    u8 i=0;
    u8 aux_G = 0;
    u8 aux_H = 0;

    THuman* tempH=g_Humans;

    if(g_Ghost.trap.state<2){//Si estan activadas
        if(g_Ghost.trap.counter>TRAP_COUNTDOWN){
            CONST_U8(g_Ghost.trap.state)=2;//Usada, se recarga
            CONST_U8(g_Ghost.trap.counter)=0;//Contador a 0
            for(i;i<3;i++){
                CONST_U8(tempH->visible)=0;
                tempH++;
            }
            drawTrapIndicator();
        }else{
            CONST_U8(g_Ghost.trap.counter)+=1;
            if((g_Ghost.trap.counter&0x0F)==0){
                CONST_U8(g_Ghost.trap.state)=(g_Ghost.trap.state+1)&1;//Cambia de tile entre 0 y 1 
                drawTrapIndicator();
            }
        }
    }else if(g_Ghost.trap.state==3){

        //Si estan puestas y las pisa un HUMANO
        for(i;i<3;i++){
            aux_H = posToByte(tempH->position.x, tempH->position.y);
            aux_G = posToByte(g_Ghost.trap.position.x, g_Ghost.trap.position.y);
            // position=((tempH->position.y+6)>>4)*20+((tempH->position.x+6)>>2);

            if(aux_G==aux_H && g_Ghost.trap.mapPart==tempH->mapPart){
                activateTrap(i);
            }

            tempH++;
        }
    }
}

void plantTrap(){
    if(g_Ghost.trap.state==2){
        CONST_U8(g_Ghost.trap.state)=3;
        CONST_U8(g_Ghost.trap.position.x)=((u8)g_Ghost.position.x&0xFC);
        CONST_U8(g_Ghost.trap.position.y)=((u8)(g_Ghost.position.y&0xF0)+8);
        CONST_U8(g_Ghost.trap.mapPart)=(u8)g_currentMap;
        CONST_POINTER(g_Ghost.trap.memptr)=(u16)cpct_getScreenPtr(VMEM, g_Ghost.trap.position.x, g_Ghost.trap.position.y);

        drawTrapIndicator();
    }
}

void drawTrapIndicator(){
    u8* memptr;
    u8 index;

    index=g_Ghost.trap.state;

    memptr = cpct_getScreenPtr(VMEM, 71 + ((0&3)<<1), 8);
    cpct_drawTileAligned2x8_f(Trap_Indicator_tileset[(index==5)?0:index],memptr);
}

void update_Ghost(){
    u8 position=((g_Ghost.position.y)>>4)*20+(g_Ghost.position.x>>2);
    u8 val;
    Vec2 positionK;

    if(g_Ghost.dir&0x01){//mascara: 0000 0001 //Direccion arriba
        if(!(getCollision(g_Ghost.position.x,(g_Ghost.position.y-1))||getCollision((g_Ghost.position.x+2),(g_Ghost.position.y-1)))){
            if(g_Ghost.position.y>YOFFSET){
                CONST_U8(g_Ghost.position.y)-=2;
            }
            else if(g_currentMap==2){//Pantalla 2 y sale por arriba //masc: 0011 0000
                CONST_U8(g_Ghost.position.y)=188; //Se le posiciona abajo
                //g_Ghost.dir=(g_Ghost.dir&0xCF)+0x10;//Pasa a estar en la pantalla 1 //masc: 1100 1111
                CONST_U8 g_currentMap=1;
                CONST_U8 gt_redrawMap=1;
            }
            else if(g_currentMap==3){//Pantalla 3 y sale por arriba
                CONST_U8(g_Ghost.position.y)=176; //Se le posiciona abajo
                //g_Ghost.dir=(g_Ghost.dir&0xCF);//Pasa a estar en la pantalla 0
                CONST_U8 g_currentMap=0;
                CONST_U8 gt_redrawMap=1;
            }
            CONST_POINTER(g_Ghost.sprite)=(u16)ghost_tileset[3];
        }
    }
    else if(g_Ghost.dir&0x02){//mascara: 0000 0010 //Direccion abajo
        if(!(getCollision(g_Ghost.position.x,(g_Ghost.position.y+12))||getCollision((g_Ghost.position.x+2),(g_Ghost.position.y+12)))){
            if(g_Ghost.position.y<188){
                CONST_U8(g_Ghost.position.y)+=2;
            }
            else if(g_currentMap==0){//Pantalla 0 y sale por abajo
                CONST_U8(g_Ghost.position.y)=YOFFSET; //Se le posiciona arriba
                //g_Ghost.dir=(g_Ghost.dir&0xCF)+0x30;//Pasa a estar en la pantalla 3
                CONST_U8 g_currentMap=3;
                CONST_U8 gt_redrawMap=1;
            }
            else if(g_currentMap==1){//Pantalla 1 y sale por abajo
                CONST_U8(g_Ghost.position.y)=YOFFSET; //Se le posiciona arriba
                //g_Ghost.dir=(g_Ghost.dir&0xCF)+0x20;//Pasa a estar en la pantalla 2
                CONST_U8 g_currentMap=2;
                CONST_U8 gt_redrawMap=1;
            }
            CONST_POINTER(g_Ghost.sprite)=(u16)ghost_tileset[2];
        }
    }
    if((g_Ghost.dir&0x04)){//mascara: 0000 0100 //Direccion derecha
        if((g_Ghost.XCount==0)){
            if(!(getCollision((g_Ghost.position.x+3),g_Ghost.position.y)||getCollision((g_Ghost.position.x+3),(g_Ghost.position.y+11)))){
                if(g_Ghost.position.x<77){
                    CONST_U8(g_Ghost.position.x)+=1;
                }
                else if(g_currentMap==0){//Pantalla 0 y sale por derecha
                    CONST_U8(g_Ghost.position.x)=0; //Se le posiciona izquierda
                    //g_Ghost.dir=(g_Ghost.dir&0xCF)+0x10;//Pasa a estar en la pantalla 1
                    CONST_U8 g_currentMap=1;
                    CONST_U8 gt_redrawMap=1;
                }
                else if(g_currentMap==3){//Pantalla 3 y sale por derecha
                    CONST_U8(g_Ghost.position.x)=0; //Se le posiciona izquierda
                    //g_Ghost.dir=(g_Ghost.dir&0xCF)+0x20;//Pasa a estar en la pantalla 2
                    CONST_U8 g_currentMap=2;
                    CONST_U8 gt_redrawMap=1;
                }
                CONST_POINTER(g_Ghost.sprite)=(u16)ghost_tileset[1];
            }
        }
        CONST_U8(g_Ghost.XCount)=(g_Ghost.XCount+2)&0x03;
    }
    else if(g_Ghost.dir&0x08){//mascara: 0000 1000 //Direccion izquierda
        if((g_Ghost.XCount==0)){
            if(!(getCollision((g_Ghost.position.x-1),g_Ghost.position.y)||getCollision((g_Ghost.position.x-1),(g_Ghost.position.y+11)))){
                if(g_Ghost.position.x>0){
                    CONST_U8(g_Ghost.position.x)-=1;
                }
                else if(g_currentMap==1){//Pantalla 1 y sale por izquierda
                    CONST_U8(g_Ghost.position.x)=77; //Se le posiciona derecha
                    //g_Ghost.dir=(g_Ghost.dir&0xCF);//Pasa a estar en la pantalla 0
                    CONST_U8 g_currentMap=0;
                    CONST_U8 gt_redrawMap=1;
                }
                else if(g_currentMap==2){//Pantalla 2 y sale por izquierda
                    CONST_U8(g_Ghost.position.x)=77; //Se le posiciona derecha
                    //g_Ghost.dir=(g_Ghost.dir&0xCF)+0x30;//Pasa a estar en la pantalla 3
                    CONST_U8 g_currentMap=3;
                    CONST_U8 gt_redrawMap=1;
                }
                CONST_POINTER(g_Ghost.sprite)=(u16)ghost_tileset[0];
            }
        }
        CONST_U8(g_Ghost.XCount)=(g_Ghost.XCount-2)&0x03;
    }
    if(position!=((g_Ghost.position.y>>4)*20+(g_Ghost.position.x>>2))){
        CONST_U8 gt_redrawPosition=1;
    }

    if(g_Keys_video_position[g_currentMap]!=0 && map_0_objects[g_currentMap]!=255){

        val=map_0_objects[g_currentMap];
        positionK.x=(val%20)<<2; 
        positionK.y=((val/20)<<4)+YOFFSET;

        CONST_U16(g_Ghost.distanceToKey)=octogonalDistance(&(g_Ghost.position), &(positionK));
    }else{
        CONST_U16(g_Ghost.distanceToKey)=0xFFFF;
    }
}

void update_Humans(){
    u8 i=0;
    u8 p1,p2,p3;//,p4;
    u8 p1b,p2b,p3b;//,p4b;
    THuman* tempH=g_Humans;
    for(i;i<3;i++){
        if(tempH->mapPart==g_currentMap){
            if (tempH->position.x < g_Ghost.position.x+3 &&
               tempH->position.x + 3 > g_Ghost.position.x &&
               tempH->position.y < g_Ghost.position.y + 12 &&
               12 + tempH->position.y > g_Ghost.position.y) {
                   humanCaptured();
           }
        }
        
        
        
        calculateHumanMovement(tempH);
        getKey(tempH);
        if((g_Ghost.dir&0x0F)||(tempH->dir&0x0F)){
            if(tempH->mapPart==g_currentMap){
                CONST_U16(tempH->distanceToGhost)=octogonalDistance(&(g_Ghost.position), &(tempH->position));
            }else{
                CONST_U16(tempH->distanceToGhost)=0xFFFF;
            }
        }
        
        switch(tempH->dir&0x0F){
            case 0x01:
                CONST_POINTER(tempH->sprite)=(u16)human_tileset[3];
            break;
            case 0x02:
                CONST_POINTER(tempH->sprite)=(u16)human_tileset[2];
            break;
            case 0x04:
                CONST_POINTER(tempH->sprite)=(u16)human_tileset[1];
            break;
            case 0x08:
                CONST_POINTER(tempH->sprite)=(u16)human_tileset[0];
            break;
            
        }
        
        if(tempH->dir&0x40){
            p1=((((tempH->position.y)-YOFFSET)&0xF0)+YOFFSET)*20+((tempH->position.x)&0xFC);
            p2=((((tempH->position.y+12)-YOFFSET)&0xF0)+YOFFSET)*20+((tempH->position.x)&0xFC);
            p3=((((tempH->position.y)-YOFFSET)&0xF0)+YOFFSET)*20+((tempH->position.x+3)&0xFC);
            //p4=((((tempH->position.y+12)-YOFFSET)&0xF0)+YOFFSET)*20+((tempH->position.x+3)&0xFC);
            if(tempH->dir&0x01){//mascara: 0000 0001 //Direccion arriba
                if(tempH->position.y>YOFFSET){
                    switch(tempH->state){
                        case 0://Vel normal
                            if(tempH->position.y>=YOFFSET+2)CONST_U8(tempH->position.y)-=2;
                            else CONST_U8(tempH->position.y)=YOFFSET;
                        break;
                        case 1://Vel lenta
                            CONST_U8(tempH->position.y)-=1;
                        break;
                        case 2://Sprint
                            if(tempH->position.y>=YOFFSET+4)CONST_U8(tempH->position.y)-=4;
                            else CONST_U8(tempH->position.y)=YOFFSET;
                        break;
                    }
                }
                else if(tempH->mapPart==2){//Pantalla 2 y sale por arriba //masc: 0011 0000
                    CONST_U8(tempH->position.y)=188; //Se le posiciona abajo
                    CONST_U8 tempH->lastMapPart=tempH->mapPart;
                    CONST_U8 tempH->mapPart=1;
                }
                else if(tempH->mapPart==3){//Pantalla 3 y sale por arriba
                    CONST_U8(tempH->position.y)=176; //Se le posiciona abajo
                    CONST_U8 tempH->lastMapPart=tempH->mapPart;
                    CONST_U8 tempH->mapPart=0;
                }
                CONST_POINTER(tempH->sprite)=(u16)human_tileset[3];
            }
            else if(tempH->dir&0x02){//mascara: 0000 0010 //Direccion abajo
                if(tempH->position.y<188){
                    switch(tempH->state){
                        case 0://Vel normal
                            if(tempH->position.y<186)CONST_U8(tempH->position.y)+=2;
                            else CONST_U8(tempH->position.y)=188;
                        break;
                        case 1://Vel lenta
                            CONST_U8(tempH->position.y)+=1;
                        break;
                        case 2://Sprint
                            if(tempH->position.y<184)CONST_U8(tempH->position.y)+=4;
                            else CONST_U8(tempH->position.y)=188;
                        break;
                    }
                }
                else if(tempH->mapPart==0){//Pantalla 0 y sale por abajo
                    CONST_U8(tempH->position.y)=YOFFSET; //Se le posiciona arriba
                    CONST_U8 tempH->lastMapPart=tempH->mapPart;
                    CONST_U8 tempH->mapPart=3;
                }
                else if(tempH->mapPart==1){//Pantalla 1 y sale por abajo
                    CONST_U8(tempH->position.y)=YOFFSET; //Se le posiciona arriba
                    CONST_U8 tempH->lastMapPart=tempH->mapPart;
                    CONST_U8 tempH->mapPart=2;
                }
                CONST_POINTER(tempH->sprite)=(u16)human_tileset[2];
            }
            if((tempH->dir&0x04)){//mascara: 0000 0100 //Direccion derecha
                if((tempH->XCount==0)){
                    if(tempH->position.x<77){
                        CONST_U8(tempH->position.x)+=1;
                    }
                    else if(tempH->mapPart==0){//Pantalla 0 y sale por derecha
                        CONST_U8(tempH->position.x)=0; //Se le posiciona izquierda
                    CONST_U8 tempH->lastMapPart=tempH->mapPart;
                        CONST_U8 tempH->mapPart=1;
                    }
                    else if(tempH->mapPart==3){//Pantalla 3 y sale por derecha
                        CONST_U8(tempH->position.x)=0; //Se le posiciona izquierda
                    CONST_U8 tempH->lastMapPart=tempH->mapPart;
                        CONST_U8 tempH->mapPart=2;
                    }
                    CONST_POINTER(tempH->sprite)=(u16)human_tileset[1];
                }
                switch(tempH->state){
                    case 0://Vel normal
                        CONST_U8(tempH->XCount)=(tempH->XCount+1)&0x01;
                    break;
                    case 1://Vel lenta
                        CONST_U8(tempH->XCount)=(tempH->XCount+1)&0x03;
                    break;
                    case 2://Sprint
                        CONST_U8(tempH->XCount)=0;
                    break;
                }
            }
            else if(tempH->dir&0x08){//mascara: 0000 1000 //Direccion izquierda
                if((tempH->XCount==0)){
                        if(tempH->position.x>0){
                            CONST_U8(tempH->position.x)-=1;
                        }
                        else if(tempH->mapPart==1){//Pantalla 1 y sale por izquierda
                            CONST_U8(tempH->position.x)=77; //Se le posiciona derecha
                    CONST_U8 tempH->lastMapPart=tempH->mapPart;
                            CONST_U8 tempH->mapPart=0;
                        }
                        else if(tempH->mapPart==2){//Pantalla 2 y sale por izquierda
                            CONST_U8(tempH->position.x)=77; //Se le posiciona derecha
                    CONST_U8 tempH->lastMapPart=tempH->mapPart;
                            CONST_U8 tempH->mapPart=3;
                        }
                        CONST_POINTER(tempH->sprite)=(u16)human_tileset[0];
                }
                switch(tempH->state){
                    case 0://Vel normal
                        CONST_U8(tempH->XCount)=(tempH->XCount-1)&0x01;
                    break;
                    case 1://Vel lenta
                        CONST_U8(tempH->XCount)=(tempH->XCount-1)&0x03;
                    break;
                    case 2://Sprint
                        CONST_U8(tempH->XCount)=0;
                    break;
                }
            }
            
            p1b=((((tempH->position.y)-YOFFSET)&0xF0)+YOFFSET)*20+((tempH->position.x)&0xFC);
            p2b=((((tempH->position.y+12)-YOFFSET)&0xF0)+YOFFSET)*20+((tempH->position.x)&0xFC);
            p3b=((((tempH->position.y)-YOFFSET)&0xF0)+YOFFSET)*20+((tempH->position.x+3)&0xFC);
            //p4b=((((tempH->position.y+12)-YOFFSET)&0xF0)+YOFFSET)*20+((tempH->position.x+3)&0xFC);
            if(((tempH->dir&0x02)&&(p1!=p1b))||((tempH->dir&0x01)&&(p2!=p2b))||((tempH->dir&0x08)&&(p3!=p3b))||((tempH->dir&0x04)&&(p1!=p1b))){
                CONST_U8 tempH->dir=(tempH->dir&0x3F);//Se para al entrar en un nuevo tile
                //Redraw vision cone
                //Poner peso en la posicion anterior
            }
        }
        
        switch(tempH->state){
            case 0://Van con linterna encendida
                CONST_U16(tempH->light)+=1;
                if(tempH->light>=LIGHT_DURATION){
                    CONST_U8(tempH->state)=1;
                }
                
                if(tempH->sprint>1){
                    CONST_U16(tempH->sprint)-=1;
                }
            break;
            case 1://Sin luz
                if(tempH->light>4){
                    CONST_U16(tempH->light)-=4;
                }
                else{
                    CONST_U16(tempH->light)=0;
                }
                
                if(tempH->sprint>1){
                    CONST_U16(tempH->sprint)-=1;
                }
            break;
            case 2://Sprint (sin lux)
                CONST_U8(tempH->sprint)+=1;
                if(tempH->sprint>=SPRINT_DURATION){
                    CONST_U8(tempH->state)=1;
                }
                
                if(tempH->light>4){
                    CONST_U16(tempH->light)-=4;
                }
                else{
                    CONST_U16(tempH->light)=0;
                }
            break;
        }
        tempH++;
    }
}

void humanCaptured(){
    CONST_U16 g_gameTimer-=30;
    //AGREGAR EFECTOS SOBRE EL HUMANO
    
    returnToSpawner_Ghost();
}

void calculateHumanMovement(THuman* H){
   if(H->mapPart==g_currentMap){//Reacciones al fantasma aqui, si no estan en mismo mapa no reaccionan al fantasma
        if(H->distanceToGhost<=36){//Reacciones inmediatas
            CONST_U8(H->state)=2;
            CONST_U8 H->dir=(H->dir&0x30)|calculateHumanDirectionbyDistance(H);
        }
    }
    if((H->distanceToGhost>36) ||(H->mapPart!=g_currentMap)){
        if((H->distanceToGhost<=48)&&(H->mapPart==g_currentMap))CONST_U8(H->state)=0;
        else CONST_U8(H->state)=1;
        //printNumberDigits(H->dir,3,40,8);
        if(((H->dir&0xC0)==0)){//Si no tiene movimiento
            CONST_U8 H->dir=(H->dir&0x30)|calculateHumanDirectionExploring(H);
        }
    }
 
}

u8 calculateHumanDirectionbyDistance(THuman* H){
    u8 u,d,l,r,m,y;
    u=(g_Ghost.position.y>= H->position.y);
    l=(g_Ghost.position.x>= H->position.x);
    d=(g_Ghost.position.y< H->position.y);
    r=(g_Ghost.position.x< H->position.x);
    y=(((H->position.y-YOFFSET)&0xF0)+YOFFSET);
    if(u){
        u=(!getCollisionDirTile(H->position.x,H->position.y,0x01,H->mapPart));
    }
    if(d){
        d=(!getCollisionDirTile(H->position.x,H->position.y,0x02,H->mapPart));
    }
    if(l){
        l=(!getCollisionDirTile(H->position.x,H->position.y,0x08,H->mapPart));
    }
    if(r){
        r=(!getCollisionDirTile(H->position.x,H->position.y,0x04,H->mapPart));
    }

    //u=0 r=0 l=1 d=1
    m=(u|d|l|r)<<6;
    return (m|(l<<3)|(r<<2)|(d<<1)|u);
}

u8 calculateHumanDirectionExploring(THuman* H){
    u8 m, Eu, Ed, El, Er, c, r,y;
    m=Eu=Ed=El=Er=c=0;
    r=(220*H->mapPart);
    y=(((H->position.y-YOFFSET)&0xF0)+YOFFSET);
    setBit(1,posToByte(H->position.x,H->position.y)+r,H->explored);
    if(!(H->dir&0x04)){
        El=!getBit(posToByte((H->position.x&0xFC)-1,H->position.y)+r,H->explored);
        if(El){
            if(getCollisionDirTile(H->position.x,H->position.y,0x08,H->mapPart)){
                El=0;
                setBit(1,posToByte((H->position.x&0xFC)-1,H->position.y)+r,H->explored);
            }
        }
    }
    if(!(H->dir&0x08)){
        Er=!getBit(posToByte((H->position.x&0xFC)+4,H->position.y)+r,H->explored);
        if(Er){
            if(getCollisionDirTile(H->position.x,H->position.y,0x04,H->mapPart)){
                Er=0;
                setBit(1,posToByte((H->position.x&0xFC)+4,H->position.y)+r,H->explored);
            }
        }
    }
    if(!(H->dir&0x02)){
        Eu=!getBit(posToByte(H->position.x,y-1)+r,H->explored);
        if(Eu){
            if(getCollisionDirTile(H->position.x,H->position.y,0x01,H->mapPart)){
                Eu=0;
                setBit(1,posToByte((H->position.x),y-1)+r,H->explored);
            }
        }
    }
    if(!(H->dir&0x01)){
        Ed=!getBit(posToByte(H->position.x,y+16)+r,H->explored);
        if(Ed){
            if(getCollisionDirTile(H->position.x,H->position.y,0x02,H->mapPart)){
                Ed=0;
                setBit(1,posToByte((H->position.x),y+16)+r,H->explored);
            }
        }
    }
    c=El+Er+Eu+Ed;
    // printNumberDigits(c,1,40,8);
    if(c==1){//Solo un camino a explorar
        return (0x40|(El<<3)|(Er<<2)|(Ed<<1)|Eu);
    }
    else if(c>1){//Varios caminos a explorar
        r=cpct_getRandomUniform_u8_f(g_inputCount)%c;
        c=0;
        if(El){//casilla izquierda
            if(c==r){
                return (0x48);
            }
            c++;
        }
        if(Er){//derecha
            if(c==r){
                return (0x44);
            }
            c++;
        }
        if(Eu){//arriba
            if(c==r){
                return (0x41);
            }
            c++;
        }
        if(Ed){//abajo
            if(c==r){
                return (0x42);
            }
        }
    }
    else{
        Er=El=Eu=Ed=0;
        if(!(H->dir&0x04)){
            if(!getCollisionDirTile(H->position.x,H->position.y,0x08,H->mapPart)){
                El=1;
                setBit(1,posToByte((H->position.x&0xFC)-1,H->position.y)+r,H->explored);
            }
        }
        else{
                El=1;
        }
        if(!(H->dir&0x08)){
            if(!getCollisionDirTile(H->position.x,H->position.y,0x04,H->mapPart)){
                Er=1;
                setBit(1,posToByte((H->position.x&0xFC)+4,H->position.y)+r,H->explored);
            }
        }
        else{
                Er=1;
        }
        if(!(H->dir&0x02)){
            if(!getCollisionDirTile(H->position.x,H->position.y,0x01,H->mapPart)){
                Eu=1;
                setBit(1,posToByte((H->position.x),y-1)+r,H->explored);
            }
        }
        else{
                Eu=1;
        }
        if(!(H->dir&0x01)){
            if(!getCollisionDirTile(H->position.x,H->position.y,0x02,H->mapPart)){
                Ed=1;
                setBit(1,posToByte((H->position.x),y+16)+r,H->explored);
            }
        }
        else{
                Ed=1;
        }
        
        c=El+Er+Eu+Ed;
        
        if(c==1){//Solo puede volver
            return (0x40|(El<<3)|(Er<<2)|(Ed<<1)|Eu);
        }
        else if(c>1){//Varios caminos a explorar
            r=cpct_getRandomUniform_u8_f(g_inputCount)%(c-1);
            c=0;
            if(El&&(!(H->dir&0x04))){//casilla izquierda
                if(c==r){
                    return (0x48);
                }
                c++;
            }
            if(Er&&(!(H->dir&0x08))){//derecha
                if(c==r){
                    return (0x44);
                }
                c++;
            }
            if(Eu&&(!(H->dir&0x01))){//arriba
                if(c==r){
                    return (0x41);
                }
                c++;
            }
            if(Ed&&(!(H->dir&0x02))){//abajo
                if(c==r){
                    return (0x42);
                }
            }
        }
    }
    
    return 0;//No tiene casilla por explorar cerca, no se mueve
}

void getKey(THuman* hum){

    u8 positionH = posToByte(hum->position.x, hum->position.y);

    //u8 positionGh = posToByte(g_Ghost.position.x, g_Ghost.position.y);
    u8* memptr;
    
    if(g_Keys_video_position[g_currentMap]!=0){
        if(((u8)map_0_objects[hum->mapPart]==positionH)){
            CONST_U8(g_numKeys) = (u8)g_numKeys-1;    

            memptr= cpct_getScreenPtr(VMEM, 38, 8);
            cpct_drawSolidBox(memptr, 0xF0, 4, 4);
                
            // x de las llaves
            memptr= cpct_getScreenPtr(VMEM, 39, 8);
            cpct_drawStringM1_f("x", memptr, 0, 1);

            // Contador de las llaves
            printNumberDigits((u8)g_numKeys,1,38,8);

            // Le pone una casilla max para que no la pinta
            g_Keys_video_position[g_currentMap] = 0; 

            if((u8)g_numKeys == 0){
                CONST_U8 g_state = GAME_STATE_GAMEOVER;
                CONST_U8 g_changeState = 1;
                CONST_U8 g_inputCount+=1;
            }
        }
    }
}

void init_Ghost(u8 x, u8 y, u8 spawner){
    u8 i=0;
    CONST_U8(g_Ghost.position.x)=x;
    CONST_U8(g_Ghost.position.y)=y;
    CONST_U8(g_Ghost.dir)=0;
    for(i;i<110;i++){
        CONST_U8(g_Ghost.explored[i])=0;
    }
    CONST_U8(g_Ghost.spawner)=spawner;
    CONST_U8(g_Ghost.XCount)=0;
    CONST_POINTER(g_Ghost.sprite) = (u16)ghost_tileset[1];
    CONST_POINTER(g_Ghost.memptr) = (u16)cpct_getScreenPtr(VMEM, x, y);
    CONST_U16(g_Ghost.distanceToKey)=0xFFFF;
    
    CONST_U16(g_Ghost.trap.position)=0;
    CONST_U8(g_Ghost.trap.state)=2;
    CONST_U8(g_Ghost.trap.counter)=0;
    drawTrapIndicator();
}

void init_Humans(u8 x, u8 y, u8 initMap){
    u8 i=0,j=0;
    THuman* tempH=g_Humans;
    for(i;i<3;i++){
        CONST_U8(tempH->position.x)=x;
        CONST_U8(tempH->position.y)=y;
        CONST_U8(tempH->dir)=0;
        CONST_U8(tempH->mapPart)=initMap;
        CONST_U8(tempH->lastMapPart)=initMap;
        for(j;j<110;j++){
            CONST_U8(tempH->explored[j])=0;
        }
        CONST_U8(tempH->XCount)=0;
        CONST_U8(tempH->state)=0;
        CONST_U16(tempH->light)=0;
        CONST_U8(tempH->sprint)=0;
        CONST_U8(tempH->visible)=0;
        CONST_U8(tempH->hasObject)=0;
        CONST_U16(tempH->distanceToGhost)=0xFFFF;
        CONST_U8(tempH->exitDiscovered)=0;
        CONST_U8(tempH->bytePos)=posToByte(x,y);
        CONST_POINTER(tempH->sprite)=(u16)human_tileset[1];
        CONST_POINTER(tempH->memptr)=(u16)cpct_getScreenPtr(VMEM, x, y);
        //generateTraveledMap(i);
        tempH++;
    }
}

void init_Entities(){
    u8 x,y;
    u8 index=(cpct_getRandomUniform_u8_f(g_inputCount)&3);
    CONST_U8 g_currentMap=index;
    getSpawnPosition(index, &x, &y);
    init_Ghost(x,y,index);
    index=(index+((cpct_getRandomUniform_u8_f(g_inputCount)%3)+1))&3;
    getSpawnPosition(index, &x, &y);
    init_Humans(x,y,index);
}

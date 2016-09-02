#ifndef _ENTITIES_H_
#define _ENTITIES_H_

#include <cpctelera.h>

#define TRAP_COUNTDOWN 200
#define LIGHT_DURATION 500
#define SPRINT_DURATION 150

typedef struct Vector2{
    u8 x;
    u8 y;
}Vec2;

typedef struct Trap{
    Vec2 position;
    u8 mapPart;
    
    //u8 state;//0 usada, 1 usada, 2 sin plantar, 3 usada y sacabao, 4, 5 plantada    
    u8 state;//0 usada, 1 usada, 2 sin plantar, 3 plantada
    u8 counter;
    
    u8* memptr;
}TTrap;

typedef struct Ghost{
    Vec2 position;
    u8 dir; //Direccion de movimiento y contador de frames de movimiento (para mov en X) // (byte) 00MM DDDD
    u8 explored[110];
    u8 XCount;
    
    u8* sprite;
    u8* memptr;
    
    u8 spawner;
    
    u16 distanceToKey;

    TTrap trap;
}TGhost;

typedef struct Human{
    Vec2 position;
    u8 dir; //Direccion a la que mira y movimiento// (byte)0m00 DDDD 
    u8 explored[110];
    u8 XCount;
    
    u8* sprite;
    u8* memptr;
    
    u8 state;// 0 normal, 1 sin luz, 2 sprint
    u16 light;
    u8 sprint;
    u8 mapPart;
    u8 lastMapPart;
    u8 hasObject;
    u8 exitDiscovered;
    u16 distanceToGhost;
    u8 bytePos;

    u8 visible;
}THuman;

extern const TGhost g_Ghost;
extern const THuman g_Humans[3];

extern void init_Ghost(u8 x, u8 y, u8 spawner);
extern void init_Humans(u8 x, u8 y, u8 initMap);
extern void init_Entities();
extern void returnToSpawner_Ghost();

extern void activateTrap(u8 i);
extern void plantTrap();
extern void update_Traps();
extern void drawTrapIndicator();

extern void update_Ghost();
extern void update_Humans();

extern void humanCaptured();
extern void calculateHumanMovement(THuman* H);//IA
extern u8 calculateHumanDirectionbyDistance(THuman* H);
extern u8 calculateHumanDirectionExploring(THuman* H);

extern void getKey(THuman* hum);

#endif
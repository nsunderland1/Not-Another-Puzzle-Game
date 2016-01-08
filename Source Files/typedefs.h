#ifndef TYPEDEFS_H
#define TYPEDEFS_H

#define TILE_SIDE 40

//Object collision box
struct box {
       int top, bottom;
       int left, right;
};

//Object collision data
struct collision {
       bool top, bottom;
       bool left, right;    
};

//Stores type of object
enum object_type {USER,MOVABLE,FIXED};

//Game objects
struct object {
       object_type type;
       BITMAP*sprite;            
       int h, w;                 //Height, width
       box side;                 //Collision box
       collision collide;        //Environment collision detection
       collision collide_user;   //Collision detection with user
};

//Stores type of tile
enum tile_type {PORTAL,BUTTON,END};

struct tile {
       tile_type type;
       BITMAP*sprite;    
       box side;         //Collision box
       bool overlap;     //Used for overlap with game object
       bool active;      //Enables and disables tile functionality  
       tile*target;      //Other tile modified by button
};

//User movement direction
enum direction {NONE, UP, DOWN, LEFT, RIGHT};

#endif

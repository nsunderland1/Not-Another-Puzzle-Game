#include <stdio.h>
#include <string.h>
#include <allegro.h>
#include "typedefs.h"

//Checks for successful bitmap load
void error_check(const char* sprite) {     
     if (!load_bitmap(sprite, NULL)) {
        allegro_message("Failed to load %s", sprite);
        exit(EXIT_FAILURE); 
     }
}

//Loads game object with bitmap and position
object load_object(object_type type, int x, int y) {    
     object thisObj;
     char sprite[25];
     
     switch (type) {
            case USER:
                 strcpy(sprite, "Bitmaps/person.bmp");
                 break;
            case MOVABLE:
                 strcpy(sprite, "Bitmaps/movable.bmp");
                 break;
            case FIXED:
                 strcpy(sprite, "Bitmaps/brick.bmp");
                 break;
     }
     
     thisObj.sprite = load_bitmap(sprite, NULL);
     thisObj.type = type;
     
     //Setting height and width
     thisObj.h = thisObj.sprite->h;
     thisObj.w = thisObj.sprite->w; 
     
     //Setting collision box     
     thisObj.side.left = x;
     thisObj.side.top = y;
     thisObj.side.right = x + thisObj.w;
     thisObj.side.bottom = y + thisObj.h;
     
     return thisObj;    
}

//Creates game object based on bitmap, position, and size
object new_object(object_type type, int x, int y, int h, int w) {       
       object newObj;    //Object to be created
       object component; //Object on which it is based
       char sprite[25];
       
       switch (type) {
            case USER:
                 strcpy(sprite, "Bitmaps/person.bmp");
                 break;
            case MOVABLE:
                 strcpy(sprite, "Bitmaps/movable.bmp");
                 break;
            case FIXED:
                 strcpy(sprite, "Bitmaps/brick.bmp");
                 break;
       }
       
       newObj.type = type;
       
       //Setting up component
       component.sprite = load_bitmap(sprite, NULL);
       component.h = component.sprite->h;
       component.w = component.sprite->w;
       
       //Creating bitmap for new object
       newObj.sprite = create_bitmap(w, h);
       
       //Filling new bitmap with component object
       for (int i = 0; i < h; i+=component.h) 
           for (int j = 0; j < w; j+=component.w)
               blit(component.sprite, newObj.sprite, 0,0, j, i, component.w, component.h);
       
       //Setting new object collision box
       newObj.side.left = x;
       newObj.side.top = y;
       newObj.side.right = x + newObj.sprite->w;
       newObj.side.bottom = y + newObj.sprite->h;
       
       newObj.h = h;
       newObj.w = w;
       
       return newObj;
}

//Loads special game tile with bitmap, position, and initial state
tile load_tile(tile_type type, int x, int y, bool active, tile*target) {
     tile thisTile;
     char sprite[25];
     
     switch (type) {
            case PORTAL:
                 strcpy(sprite, "Bitmaps/portal.bmp");
                 break;
            case BUTTON:
                 strcpy(sprite, "Bitmaps/button.bmp");
                 break;
            case END:
                 strcpy(sprite, "Bitmaps/exit_stair.bmp");
                 break;
     }
       
     thisTile.sprite = load_bitmap(sprite, NULL);
     thisTile.type = type;

     //Setting tile collision box
     thisTile.side.left = x;
     thisTile.side.top = y;
     thisTile.side.right = x + TILE_SIDE;
     thisTile.side.bottom = y + TILE_SIDE;
     
     //Setting tile state
     thisTile.overlap = false;
     thisTile.active = active;
     thisTile.target = target;
     
     return thisTile;
}

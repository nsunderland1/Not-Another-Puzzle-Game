#include <allegro.h>
#include "typedefs.h"

//User movement when not on ice
void move_user(collision collide, direction &moving) {
     //Setting user movement to direction of key
     //if not obstructed
     if (key[KEY_UP] && !collide.top)
        moving = UP;
     else if (key[KEY_DOWN] && !collide.bottom)
          moving = DOWN;
     if (key[KEY_LEFT] && !collide.left)
        moving = LEFT;
     else if (key[KEY_RIGHT] && !collide.right)
          moving = RIGHT;
     
     //Preventing user from inputting opposite directions
     if ((key[KEY_UP] && key[KEY_DOWN]) || (key[KEY_LEFT] && key[KEY_RIGHT]))
        moving = NONE;
}

void move_user_ice(collision collide, direction &moving) {
     //Waiting for user to be stationary
     if (moving == NONE) {
        //Setting user movement to direction of key
        //if not obstructed
        if (key[KEY_UP] && !collide.top)
           moving = UP;
        else if (key[KEY_DOWN] && !collide.bottom)
             moving = DOWN;
        if (key[KEY_LEFT] && !collide.left)
           moving = LEFT;
        else if (key[KEY_RIGHT] && !collide.right)
             moving = RIGHT;
     }
     
     //Stopping user movement upon head-on collision          
     if (collide.top && moving == UP)
        moving = NONE;
     if (collide.bottom && moving == DOWN)
        moving = NONE;
     if (collide.left && moving == LEFT)
        moving = NONE;
     if (collide.right && moving == RIGHT)
        moving = NONE;
}

#include <allegro.h>
#include "typedefs.h"

//Checks if user is pushing object and moves it if true
void push_object_check(object &A) {
     //Moving object if user is pushing on one side and there is
     //no collision on other side
     if (A.collide_user.top && !A.collide.bottom)
        A.side.top++;
     if (A.collide_user.bottom && !A.collide.top)
        A.side.top--;
     if (A.collide_user.left && !A.collide.right)
        A.side.left++;
     if (A.collide_user.right && !A.collide.left)
        A.side.left--;

               
     //Adjusting object collision box
     A.side.right = A.side.left + A.w;
     A.side.bottom = A.side.top + A.h;
}

//Checks if user is pushing object on ice
void push_object_ice(object &A, direction moving, collision &user) {
     //Moving if user is on one side and there is no collision on the other
     //side, and if user is moving towards object
     //Also disabling user collision in direction of movement in order to
     //continue motion until a wall is reached
     if (A.collide_user.top && !A.collide.bottom && (moving == DOWN || key[KEY_DOWN])) {
        A.side.top++;
        user.bottom = false;
     }
     if (A.collide_user.bottom && !A.collide.top && (moving == UP || key[KEY_UP])) {
        A.side.top--;
        user.top = false;
     }
     if (A.collide_user.left && !A.collide.right && (moving == RIGHT || key[KEY_RIGHT])) {
        A.side.left++;
        user.right = false;
     }
     if (A.collide_user.right && !A.collide.left && (moving == LEFT || key[KEY_LEFT])) {
        A.side.left--;
        user.left = false;
     }
               
     //Adjusting object collision box
     A.side.right = A.side.left + A.w;
     A.side.bottom = A.side.top + A.h;
}

//Checks if user is pulling object and moves it if true
void pull_object_check(object A, object &B, direction moving) {
     //Moving object if user is touching and moving away from the object, 
     //and if there is no obstruction
     if (!A.collide.top && B.collide_user.top && !B.collide.top && moving == UP)
        B.side.top--;
     if (!A.collide.bottom && B.collide_user.bottom && !B.collide.bottom && moving == DOWN)
        B.side.top++;
     if (!A.collide.left && B.collide_user.left && !B.collide.left && moving == LEFT)
        B.side.left--;
     if (!A.collide.right && B.collide_user.right && !B.collide.right && moving == RIGHT)
        B.side.left++;
     
     //Adjuting object collision box
     B.side.right = B.side.left + B.w;
     B.side.bottom = B.side.top + B.h;
}

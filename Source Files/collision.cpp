#include <allegro.h>
#include "typedefs.h"

//Setting room boundaries
#define WALL_T 40
#define WALL_B 600
#define WALL_L 40
#define WALL_R 920

//Setting collision state
void collision_init(collision &x, bool state) {
     x.top = state;
     x.bottom = state;
     x.left = state;
     x.right = state;
}

//Checking for object collision with wall
void wall_collision_check(object &A) {
     //Colliding if collision box coordinate matches wall
     //coordinate
     if (A.side.top == WALL_T)
        A.collide.top = true;
     if (A.side.bottom == WALL_B)
        A.collide.bottom = true;
     if (A.side.left == WALL_L)
        A.collide.left = true;
     if (A.side.right == WALL_R)
        A.collide.right = true;
}

//Checking for collision between two objects
void collision_check(box A_side, collision &A_collide, box B_side, collision &B_collide) {
          //Checks whether a collision is possible on each side of an object
          collision possible;
          collision_init(possible, true);
          
          //Checking if objects are within possible collision boundaries
          if (A_side.left >= B_side.right || A_side.right <= B_side.left) {
             possible.top = false;
             possible.bottom = false;
          }
          if (A_side.top >= B_side.bottom || A_side.bottom <= B_side.top) {
             possible.left = false;
             possible.right = false;
          }
          
          //Colliding if a collision is possible and object coordinates
          //match
          if (A_side.top == B_side.bottom && possible.top) {
             A_collide.top = true;
             B_collide.bottom = true;
          }
          if (A_side.bottom == B_side.top && possible.bottom) {
             A_collide.bottom = true;
             B_collide.top = true;
          }
          if (A_side.left == B_side.right && possible.left) {
             A_collide.left = true;
             B_collide.right = true;
          }
          if (A_side.right == B_side.left && possible.right) {
             A_collide.right = true;
             B_collide.left = true;
          }
}

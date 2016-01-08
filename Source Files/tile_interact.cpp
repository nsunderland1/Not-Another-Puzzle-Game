#include <allegro.h>
#include "typedefs.h"

#define GAP 21 //Amount of overlap required for tile activation
               //Set to 21 so a 40x40 brick cannot trigger two buttons at once

//Checks for overlap between object and tile
void overlap_check(box A_side, box B_side, bool &B_overlap) {
     //Overlaps if object is within boundaries of tile
     //(with gap taken into account)
     if (A_side.top<B_side.bottom-GAP && A_side.bottom>B_side.top+GAP && A_side.left<B_side.right-GAP && A_side.right>B_side.left+GAP)
        B_overlap = true;
}

//Checks for portal interaction and moves user if needed
void portal_check(object &A, tile portal[]) {    
     //Checking both portals
     for (int i = 0; i < 2; i++) {
         //Portal is interacted with if user is on top
         //and if it is active
         if (portal[i].active && portal[i].overlap) {
            //Moving user to other portal
            A.side.top = portal[!i].side.top;
            A.side.left = portal[!i].side.left;
            A.side.bottom = A.side.top + A.h;
            A.side.right = A.side.left + A.w;;
            //Disabling portals to prevent repeated
            //transporting
            portal[0].active = false;
            portal[1].active = false;
            break;
         }
     }
     
     //Activating portals if user is not standing on one
     if (!portal[0].overlap && !portal[1].overlap) {
         portal[0].active = true;
         portal[1].active = true;            
     }
}

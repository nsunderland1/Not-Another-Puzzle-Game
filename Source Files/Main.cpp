#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <allegro.h>
#include "typedefs.h"   //Contains definitions for object, tile, direction, and component types (e.g. box)
#include "prototypes.h" //Contains function prototypes for game processes

#define SCREEN_WIDTH 960        
#define SCREEN_HEIGHT 640 

#define WHITE makecol(255,255,255) 

#define GAME_SPEED 300 //Used for game speed counter
#define LEVEL_NUM 4   //Number of levels in the game

#define KEY_PUSH KEY_LCONTROL //Keybinding for pushing movable objects
#define KEY_PULL KEY_LSHIFT   //Keybinding for pulling movable objects

//Used to store user data
struct score_entry {
       char name[20];
       long score;       
};

volatile long speed_counter = 0; //Used for game speed timer
bool game_paused = true;                //Stores game pause state

//Setting up timer for game speed
void inc_speed_counter() {
     //Incrementing if game is not paused
     if (!game_paused)
        speed_counter++;     
}
END_OF_FUNCTION(inc_speed_counter);

//Setting up timer for user score
volatile long timer = 0; 
void inc_timer() {timer++;}
END_OF_FUNCTION(inc_timer);

void name_input(char*);
void load_level(object&,tile&,object*,int&,tile*,int&,bool&,char*);
bool run_level(object,tile&,object*,int,tile*,int,bool);

int main(int argc, char *argv[]) {
    allegro_init();
    install_keyboard();
    install_timer();

    score_entry userscore;  //Score and name of current user
    strcpy(userscore.name,""); //Initializing username
    
    //Running game speed timer
    LOCK_VARIABLE(speed_counter);
    LOCK_FUNCTION(inc_speed_counter);
    install_int_ex(inc_speed_counter, BPS_TO_TIMER(GAME_SPEED));
    
    //Running timer for user score
    LOCK_VARIABLE(timer);
    LOCK_FUNCTION(inc_timer);
    install_int_ex(inc_timer, BPS_TO_TIMER(1)); 
    
    set_color_depth(desktop_color_depth());
    set_gfx_mode(GFX_AUTODETECT_WINDOWED, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0);
    
    //Receiving username and checking for input
    while (!strlen(userscore.name))
          name_input(userscore.name);
    
    //Verifying succesful loading of start screen
    error_check("Bitmaps/start_screen.bmp");

    //Displaying start screen
    BITMAP*start = load_bitmap("Bitmaps/start_screen.bmp", NULL);
    blit(start, screen, 0,0,0,0, SCREEN_WIDTH, SCREEN_HEIGHT);
    
    //Pausing to prevent previous keypress from triggering game start
    rest(750);
    
    //Waiting for user to begin game
    while(!key[KEY_ENTER] && !key[KEY_ENTER_PAD]);
    
    destroy_bitmap(start);
    //Starting speed counter and setting timer to 0 for game start
    game_paused = false;
    timer = 0;
    
    //Checking for succesful loading of all bitmaps
    error_check("Bitmaps/background.bmp");
    error_check("Bitmaps/pause_screen.bmp");
    error_check("Bitmaps/person.bmp");
    error_check("Bitmaps/brick.bmp");
    error_check("Bitmaps/movable.bmp");
    error_check("Bitmaps/exit_stair.bmp");
    error_check("Bitmaps/button.bmp");
    error_check("Bitmaps/portal.bmp");
    error_check("Bitmaps/portal_off.bmp");
    
    object user, game_objects[30];   //Objects within game level
    tile end, game_tiles[30];        //Special tiles
    int object_amount, tile_amount;  //Sizes of above arrays 
    bool ice;                        //Level floor type
    
    //File containing current level
    char levelName[13] = "Levels/1.txt";
    
    //Looping through all of the levels
    for (int i = 0; i < LEVEL_NUM; i++) {
        //Taking level information from file
        load_level(user, end, game_objects, object_amount, game_tiles, tile_amount, ice, levelName);
        
        //Running level
        while (run_level(user, end, game_objects, object_amount, game_tiles, tile_amount, ice));
        
        //Incrementing to next level
        levelName[7]++;
    }
    
    //Recording and displaying user score
    userscore.score = timer;
    allegro_message("Congratulations, you have completed the game!");
    allegro_message("Your time was %d minutes and %d seconds.", userscore.score/60, userscore.score%60);
    
    //Checking scoreboard
    FILE*scoreboard = fopen("High Scores.txt", "r");
    if (!scoreboard) {
       allegro_message("Failed to load High Scores.txt");
       exit(EXIT_FAILURE);
    }
    
    int scoreCount;  //Number of entries in the scoreboard
    
    //Reading in scoreCount and checking for empty file
    if(fscanf(scoreboard, "%d", &scoreCount) == EOF)
         scoreCount = 0;
    
    //Incrementing to accomodate for current user score
    scoreCount++;
    //Creating array for all scores
    score_entry*highScores = (score_entry*) calloc(scoreCount,sizeof(score_entry));
    
    //Reading in previous scores and checking for incorrect score count
    for (int i = 0; i < scoreCount-1; i++) {
        if(fscanf(scoreboard, "%s %ld", highScores[i].name, &highScores[i].score) == EOF) {
             allegro_message("ERROR: Score count is higher than number of scores listed.");
             allegro_message("Please modify High Scores.txt accordingly, or clear the scoreboard.");
             exit(EXIT_FAILURE);
        }
    }
    fclose(scoreboard);
    
    //Setting final score to current user's
    highScores[scoreCount-1] = userscore;
    
    score_entry temp;  //Temporary storage while sorting
    int userPosition = scoreCount-1;  //Records position in scoreboard of current user
    
    //Sorting current user score into scoreboard
    for (int i = scoreCount-2; i >= 0; i--) {
        //Exchange if smaller than the item being checked
        if (highScores[userPosition].score < highScores[i].score) {
           //Swapping user score with score being checked
           temp = highScores[userPosition];
           highScores[userPosition] = highScores[i];
           highScores[i] = temp;
           userPosition = i;   //Updating position of user                 
        }
        else break;
    }
    //Displaying user position (adjusted to start with 1 rather than 0)
    allegro_message("You are #%d overall.", userPosition+1);
    
    //Updating scoreboard with newly sorted list   
    scoreboard = fopen("High Scores.txt", "w");
    fprintf(scoreboard, "%d\n\n\n", scoreCount);
    for (int i = 0; i < scoreCount; i++)
        fprintf(scoreboard, "%s %d\n", highScores[i].name, highScores[i].score);
    fclose(scoreboard);
    free(highScores);
    
    allegro_message("You can check everybody's scores in High Scores.txt");
}
END_OF_MAIN();

void name_input(char*name) {
    int input = 0;  //Stores user keypress
    bool backspace = false;  //Used to detect use of backspace key
    int i = 0;  //Counter variable
    BITMAP*buffer = create_bitmap(SCREEN_WIDTH, SCREEN_HEIGHT);
    
    //Looping until user hits Enter key
    while(input != 13) {
        clear_bitmap(buffer);
        //Displaying greeting text and input prompt  
        textprintf_ex(buffer, font, 0, 0, WHITE, -1, "Welcome to Not Another Puzzle Game!");
        textprintf_ex(buffer, font, 0, 40, WHITE, -1, "Please enter your name (only letters, no spaces): ");
        
        //Printing all input so far
        textprintf_ex(buffer, font, 400, 40, WHITE, -1, "%s", name);
        blit(buffer, screen,0,0,0,0, SCREEN_W, SCREEN_W);
        
        //Going back one character if user uses backspace
        if (backspace) {
           i--;
           backspace = false;
        }

        //Clearing key buffer for user input
        clear_keybuf();
        //Reading user input and converting to ASCII character code
        input = readkey() & 0xff;
        
        //If backspace is pressed
        if (input == 8) {
           //Erasing previous character if not at 0
           if (i!=0) {
              name[i-1] = 0;
              backspace = true;
           }
           //Resetting input if at 0
           else input = 0;
        }
        //Accepting all letters of the alphabet and spaces, if max length is not reached
        if (((input > 96 && input < 123) || (input > 64 && input < 91)) && i < 19) {
             //Setting character ASCII value to input
             name[i] = input;
             //Clearing next character for input
             name[i+1] = 0;
        }
        //Exiting if Escape is pressed
        else if (input == 27)
           exit(EXIT_SUCCESS);
        //Preventing from moving to next character if input is invalid
        else i--;
             
        //Advancing to next character
        i++;
    }
    destroy_bitmap(buffer);
}

void load_level(object &user,tile &end,object*game_objects,int &object_amount,tile*game_tiles,int &tile_amount,bool &ice,char*filename) {      
      FILE*level = fopen(filename, "r");  //Level file
      
      if (!level) {
         allegro_message("Failed to load %s", filename);
         exit(EXIT_FAILURE);           
      }
      
      char string[20];  //Used to read in labels, types, and booleans
      int counter = 0;  //Counts through arrays and sets their size 
      
      //Reading level start point
      fscanf(level, "%s", string); //"entry" label
      fscanf(level, "%d %d", &user.side.left, &user.side.top);
      
      //Reading level end point
      fscanf(level, "%s", string); //"exit" label
      fscanf(level, "%d %d", &end.side.left, &end.side.top);
      fscanf(level, "%s", string);
      if (!strcmp(string, "active"))
         end.active = true;
      else 
           end.active = false;
      
      fscanf(level, "%s", string); //"OBJECTS" label
      
      //Reading objects in until break
      while(1) {
           fscanf(level, "%s", string); //"object" label
           
           //Reading objects until the "TILES" label is reached
           if (!strcmp(string, "object")) {
              fscanf(level, "%s", string); //object type
              //Setting object type
              if (!strcmp(string, "fixed"))
                 game_objects[counter].type = FIXED;
              else if (!strcmp(string, "movable"))
                   game_objects[counter].type = MOVABLE;
                   
              //Reading object height, width
              fscanf(level, "%s", string); //"hw" label
              fscanf(level, "%d %d", &game_objects[counter].h, &game_objects[counter].w);     
              
              //Reading object x, y values
              fscanf(level, "%s", string); //"xy" label
              fscanf(level, "%d %d", &game_objects[counter].side.left, &game_objects[counter].side.top);
           }    
           else break;
           
           //Moving to next object
           counter++;
      }
      //Setting total object amount and resetting counter for tiles
      object_amount = counter;
      counter = 0;
      
      //Reading tiles in until break
      while(1) {
           fscanf(level, "%s", string); //"tile" label
           
           //Reading tiles until the "ICE" label is reached
           if (!strcmp(string, "tile")) {
              fscanf(level, "%s", string);  //Tile type
              //Setting tile type
              if (!strcmp(string, "portal"))
                 game_tiles[counter].type = PORTAL;
              else if (!strcmp(string, "button"))
                   game_tiles[counter].type = BUTTON;      
              
              //Reading tile x, y values
              fscanf(level, "%s", string); //"xy" label
              fscanf(level, "%d %d", &game_tiles[counter].side.left, &game_tiles[counter].side.top);
              
              //Reading tile state
              fscanf(level, "%s", string);  //Tile state
              //Setting tile state
              if (!strcmp(string, "active"))
                 game_tiles[counter].active = true;
              else 
                   game_tiles[counter].active = false;
              
              if (game_tiles[counter].type == BUTTON) {
                 fscanf(level, "%s", string);
                 if (!strcmp(string, "end"))
                    game_tiles[counter].target = &end;
                 else if (!strcmp(string, "portal"))
                      game_tiles[counter].target = &game_tiles[counter-2];
                 game_tiles[counter].active = !(*game_tiles[counter].target).active;
              }
              else 
                   game_tiles[counter].target = NULL;     
           }    
           else break;
           
           //Moving to next tile
           counter++;
      } 
      //Setting total tile amount
      tile_amount = counter;
      
      fscanf(level, "%s", string); //Floor type
      //Setting floor type
      if (!strcmp(string, "true"))
         ice = true;
      else
          ice = false;   
          
      fclose(level); 
}

bool run_level(object user, tile &end, object*game_objects, int object_amount, tile*game_tiles, int tile_amount, bool ice) {
    BITMAP*buffer = create_bitmap(SCREEN_WIDTH, SCREEN_HEIGHT);
    BITMAP*background = load_bitmap("Bitmaps/background.bmp", NULL);
    BITMAP*pause = load_bitmap("Bitmaps/pause_screen.bmp", NULL);
    BITMAP*portal_off = load_bitmap("Bitmaps/portal_off.bmp", NULL);
    
    long pauseTimer = -1; //Used to stop timer when game is paused
    game_paused = false;  //Setting initial game state
    speed_counter = 0;    //Resetting speed counter (for level restarts)
    
    //Creating box array to store initial object locations
    box*initial_positions = (box*) calloc(object_amount,sizeof(box));
    
    collision portal_collide; //Used to run collision_check() between movable
                            //objects and portals
    
    //Recording initial values of objects
    for (int i = 0; i < object_amount; i++)
        initial_positions[i] = game_objects[i].side;
    
    //Loading user character and exit tile
    user = load_object(USER, user.side.left, user.side.top);
    end = load_tile(END, end.side.left, end.side.top, end.active, end.target);
    
    //Loading all game objects
    for (int i = 0; i < object_amount; i++) {
        //Creating a standard object if of normal size, or large object if not
        if (game_objects[i].h == 40 && game_objects[i].w == 40)
           game_objects[i] = load_object(game_objects[i].type, game_objects[i].side.left, game_objects[i].side.top);
        else {
            game_objects[i] = new_object(game_objects[i].type, game_objects[i].side.left, game_objects[i].side.top, game_objects[i].h, game_objects[i].w);
        }
    }
    
    //Loading all game tiles
    for (int i = 0; i < tile_amount; i++) {
        game_tiles[i] = load_tile(game_tiles[i].type, game_tiles[i].side.left, game_tiles[i].side.top, game_tiles[i].active, game_tiles[i].target); 
    }
    
    //Initializing user movement
    direction moving = NONE;
    
    while (!key[KEY_ESC]) {
          //Game logic
         while (speed_counter > 0) {
               //Initializing user collision status
               collision_init(user.collide, false);
               //Checking for collision between user and wall
               wall_collision_check(user);
               
               //Initializing collision status and checking wall collisions for
               //all movable objects
               for (int i = 0; i < object_amount; i++) {
                   if (game_objects[i].type == MOVABLE) {
                      collision_init(game_objects[i].collide, false);
                      collision_init(game_objects[i].collide_user, false);
                      wall_collision_check(game_objects[i]);
                   }
               }
                
               for (int i = 0; i < object_amount; i++) {
                   //Checking for collisions between user and all objects
                   collision_check(user.side, user.collide, game_objects[i].side, game_objects[i].collide_user);
                   
                   //Checking for collisions between movable objects and all
                   // other objects (and portals)
                   if (game_objects[i].type == MOVABLE)
                      for (int j = 0; j < object_amount; j++)
                          if (i!=j)
                             collision_check(game_objects[i].side, game_objects[i].collide, game_objects[j].side, game_objects[j].collide);
                      for (int j = 0; j < tile_amount; j++)
                          if (game_tiles[j].type == PORTAL)
                            collision_check(game_objects[i].side, game_objects[i].collide, game_tiles[j].side, portal_collide);
               }
               
               //Checking for user interaction with end tile
               end.overlap = false;
               overlap_check(user.side, end.side, end.overlap);
               
               //Checking for interaction (push/pull) with movable objects
               for (int i = 0; i < object_amount; i++) {
                   if (game_objects[i].type == MOVABLE && key[KEY_PUSH] && !ice)
                      push_object_check(game_objects[i]);
                   else if (game_objects[i].type == MOVABLE && ice)
                        push_object_ice(game_objects[i], moving, user.collide);
               }

               //Pausing game if user hits P and has not paused in the last second
               if (key[KEY_P] && pauseTimer < timer) {
                  //Recording timer value and stopping speed counter
                  pauseTimer = timer;
                  game_paused = true;
                  
                  //Displaying pause screen
                  blit(pause, screen, 0,0,0,0, SCREEN_WIDTH, SCREEN_HEIGHT);
                  
                  //Waiting for user to unpause game
                  while (!key[KEY_ENTER] && !key[KEY_ENTER_PAD]);
                  
                  //Resuming game speed counter and timer
                  game_paused = false;
                  timer = pauseTimer;
               }
               //Checking for user movement when not on ice
               else if (!ice) {
                  move_user(user.collide, moving);
               }
               //Checking for user movement on ice
               else {
                    move_user_ice(user.collide, moving);
               }
               //Moving user
               switch (moving) {
                      case UP:
                           user.side.top--;
                           break;      
                      case DOWN:
                           user.side.top++;
                           break;  
                      case LEFT:
                           user.side.left--;
                           break; 
                      case RIGHT:
                           user.side.left++;
                           break;
                      case NONE: break; 
               }
               //Adjusting user collision box
               user.side.right = user.side.left + user.w;
               user.side.bottom = user.side.top + user.h;
               
               //Checking for user pulling interaction with movable objects
               for (int i = 0; i < object_amount; i++) {
                   if (game_objects[i].type == MOVABLE && key[KEY_PULL])
                        pull_object_check(user, game_objects[i], moving);
               }
               
               //Initializing tile interaction status and checking interaction
               //with user and movable objects
               for (int i = 0; i < tile_amount; i++) {
                   game_tiles[i].overlap = false;
                   
                   for (int j = 0; j < object_amount; j++) {
                       //Checking for movable block interaction with non-portal tiles
                       if (game_objects[j].type == MOVABLE && game_tiles[i].type != PORTAL) {                    
                          overlap_check(game_objects[j].side, game_tiles[i].side, game_tiles[i].overlap);
                       }
                   }
                   //Checking for user interaction with tiles
                   overlap_check(user.side, game_tiles[i].side, game_tiles[i].overlap);    
               }
               
               //Running button processes
               for (int i = 0; i < tile_amount; i++) {
                   if (game_tiles[i].type == BUTTON) {
                      //If button is pressed
                      if (game_tiles[i].overlap) {
                         //Turning exit on or off
                         if (game_tiles[i].target == &end)
                            game_tiles[i].target->active = game_tiles[i].active;
                         //Turning portals on if user is not interacting with them
                         else if (game_tiles[i].active && !game_tiles[i].target->overlap && !((game_tiles[i].target)[1]).overlap) {
                            game_tiles[i].target->active = game_tiles[i].active;
                            ((game_tiles[i].target)[1]).active = game_tiles[i].active;
                         }
                         //Turning portals off 
                         else if (!game_tiles[i].active) {
                            game_tiles[i].target->active = game_tiles[i].active;
                            ((game_tiles[i].target)[1]).active = game_tiles[i].active;
                            //Preventing portals from immediately reappearing
                            game_tiles[i].target->overlap = true;
                         }
                      }
                      //If button is not pressed
                      else if (game_tiles[i].target == &end)
                          //Turning exit on or off
                         game_tiles[i].target->active = !game_tiles[i].active;
                      //Turning portals on or off if user is not interacting with them
                      else if (game_tiles[i].target != &end && !game_tiles[i].target->overlap && !((game_tiles[i].target)[1]).overlap) {
                            game_tiles[i].target->active = !game_tiles[i].active;
                            ((game_tiles[i].target)[1]).active = !game_tiles[i].active;
                            //Preventing portals from immediately reappearing
                            if (game_tiles[i].active) 
                               game_tiles[i].target->overlap = true;
                      }
                   }
               }
               
               //Running portal interaction check
               for (int i = 0; i < tile_amount; i++) {
                   if (game_tiles[i].type == PORTAL) {
                      portal_check(user, &game_tiles[i]);
                      i++;
                   }
               }
                
               //Stopping user movement if not on ice
               if (!ice) moving = NONE;
               
               //Restarting level if user chooses to do so
               if (key[KEY_R]) {
                  game_paused = true;
                                       
                  //Destroying all bitmaps
                  destroy_bitmap(buffer);
                  destroy_bitmap(background);
                  destroy_bitmap(pause);
                  destroy_bitmap(portal_off);
                  destroy_bitmap(user.sprite);
                  destroy_bitmap(end.sprite);
                  for (int i = 0; i < object_amount; i++)
                      destroy_bitmap(game_objects[i].sprite);
                  for (int i = 0; i < tile_amount; i++)
                      destroy_bitmap(game_tiles[i].sprite);
                  
                  //Resetting object positions to initial values
                  for (int i = 0; i < object_amount; i++)
                      game_objects[i].side = initial_positions[i];
                  
                  free(initial_positions);
                  
                  //Returning true so as to continue the run_level loop
                  return true;
               }
               
               //Ending level upon interaction with exit
               if (end.overlap && end.active) {            
                  //Destroying all bitmaps
                  destroy_bitmap(buffer);
                  destroy_bitmap(background);
                  destroy_bitmap(pause);
                  destroy_bitmap(portal_off);
                  destroy_bitmap(user.sprite);
                  destroy_bitmap(end.sprite);
                  for (int i = 0; i < object_amount; i++)
                      destroy_bitmap(game_objects[i].sprite);
                  for (int i = 0; i < tile_amount; i++)
                      destroy_bitmap(game_tiles[i].sprite);
                      
                  free(initial_positions);

                  //Returning false so as to end run_level loop
                  return false;                 
               }
               
               speed_counter--;
         }
         //Displaying all bitmaps
         blit(background, buffer, 0,0,0,0, SCREEN_WIDTH, SCREEN_HEIGHT);
         if (end.active)
            blit(end.sprite, buffer, 0,0, end.side.left, end.side.top, TILE_SIDE, TILE_SIDE);
         for (int i = 0; i < tile_amount; i++) {
             if (game_tiles[i].type != PORTAL || game_tiles[i].active)
                blit(game_tiles[i].sprite, buffer, 0,0, game_tiles[i].side.left, game_tiles[i].side.top, TILE_SIDE, TILE_SIDE);
             
             else if (!game_tiles[i].active)
                  blit(portal_off, buffer, 0,0, game_tiles[i].side.left, game_tiles[i].side.top, TILE_SIDE, TILE_SIDE);
         }  
         draw_sprite(buffer, user.sprite, user.side.left, user.side.top);
         for (int i = 0; i < object_amount; i++)
             draw_sprite(buffer, game_objects[i].sprite, game_objects[i].side.left, game_objects[i].side.top);
         blit(buffer, screen, 0,0,0,0, SCREEN_WIDTH, SCREEN_HEIGHT);
         clear_bitmap(buffer);
    }
    //Destroying all bitmaps if user exits game
    destroy_bitmap(buffer);
    destroy_bitmap(background);
    destroy_bitmap(pause);
    destroy_bitmap(portal_off);
    destroy_bitmap(user.sprite);
    destroy_bitmap(end.sprite);
    for (int i = 0; i < object_amount; i++)
        destroy_bitmap(game_objects[i].sprite);
    for (int i = 0; i < tile_amount; i++)
        destroy_bitmap(game_tiles[i].sprite);
        
    free(initial_positions);
    
    //Exiting game
    exit(EXIT_SUCCESS);
}

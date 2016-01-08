#ifndef PROTOTYPES_H
#define PROTOTYPES_H

#ifndef TYPEDEFS_H
#include "typedefs.h"
#endif

void error_check(const char*);
object load_object(object_type,int,int);
object new_object(object_type,int,int,int,int);
tile load_tile(tile_type,int,int,bool,tile*);

void collision_init(collision&,bool);
void wall_collision_check(object&);
void collision_check(box,collision&,box,collision&);

void move_user(collision,direction&);
void move_user_ice(collision, direction&);

void push_object_check(object&);
void push_object_ice(object&,direction,collision&);
void pull_object_check(object, object&, direction);

void overlap_check(box,box,bool&);
void portal_check(object&,tile*);

#endif

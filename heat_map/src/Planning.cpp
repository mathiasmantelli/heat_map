#include <Planning.h>
#include "Objects.h"

Planning::Planning(){
    goal_object = "tvmonitor"; 
}

Planning::~Planning(){
}

void Planning::setGrid(Grid *g){
    grid = g;
}

void Planning::initialize(){

}

bool Planning::run(){
   this->object_found(goal_object);
}

bool Planning::object_found(std::string obj_class){
    
    for(int i = 0; i < objs.list_objects.size(); i++){
        if(objs.list_objects[i]->obj_class == obj_class){
            return true;
        }
    }

}
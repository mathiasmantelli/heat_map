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
   //objs.writeObjectListOnFile();
   //this->updateHeatValeuWithinMap();
}

bool Planning::object_found(std::string obj_class){
/*     std::cout << "LIST OF OBJECTS:" << std::endl; 
    for(int i = 0; i < objs.list_objects.size(); i++){
        std::cout << " - " << objs.list_objects[i].obj_class << std::endl;
    }
    std::cout << std::endl;
    std::cout << std::endl;
 */
    for(int i = 0; i < objs.list_objects.size(); i++){
        if(objs.list_objects[i].obj_class == obj_class){
            return true;
        }
    }

}

void Planning::updateHeatValeuWithinMap(){
    std::cout << "UPDATE HEATMAP VALUE --- SIZE: " << objs.list_objects.size() << std::endl;
    for(int i = 0; i < objs.list_objects.size(); i++){
        grid->global_counter++;
        int size = 1; 
        int radius = 500; 
        int object_x = (objs.list_objects[i].obj_odom_x - grid->map_ROS_origin_x_) / grid->map_ROS_resolution_;
        int object_y = (objs.list_objects[i].obj_odom_y - grid->map_ROS_origin_y_) / grid->map_ROS_resolution_;
        std::vector<std::pair<int, int>> to_be_processed; 
        to_be_processed.clear(); 
        to_be_processed.push_back(std::make_pair(object_x, object_y)); 
        while(!to_be_processed.empty()){   
            std::pair<int, int> index = to_be_processed.back(); 
            to_be_processed.pop_back(); 

            for(int l = index.second - size; l <= index.second + size; ++l){
                for(int k = index.first - size; k <= index.first + size; ++k){            
                    if(l > object_y - radius && l < object_y + radius && k > object_x - radius && k < object_x + radius){
                        Cell *c = grid->getCell(k, l);
                        float dist = pow(l - object_y, 2) + pow(k - object_x, 2);
                        if(dist <= radius && c->last_time_used != grid->global_counter && c->value == 0){
                            //c->heat_map_value.push_back(1 - (radius - dist)/radius);    
                            c->heat_map_value = 1 - (radius - dist)/radius;    
                            c->object_name = objs.list_objects[i].obj_class;
                            c->last_time_used = grid->global_counter;
                            to_be_processed.push_back(std::make_pair(k, l));
                        }
                    }
                }
            }
        }
    }
}
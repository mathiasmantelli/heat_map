#include <Planning.h>
#include <cmath>


Planning::Planning(std::string goal_obj_class){
    goal_object = goal_obj_class; 
    goal_counter_ = 0;
    updating_grid_now = false;
    semanticHP = new SemanticHP();
   
}

Planning::~Planning(){
}

void Planning::setGrid(Grid *g){
    grid = g;
}

void Planning::initialize(){
    semanticHP->initialize(goal_object, &updating_grid_now);
}

bool Planning::run(){
    updateHeatValeuWithinMap();
    if(logMode_ == QUERYING){
        semanticHP->findMostLikelyPosition(grid, objs.list_objects);
    }
   //this->object_found(goal_object);
   //objs.writeObjectListOnFile();
   
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
    updating_grid_now = true;
    grid->cleanHeatMapVector();
    for(int i = 0; i < objs.list_objects.size(); i++){
        grid->global_counter++;
        int size = 1; 
        int radius = 23; 
        int object_x = (objs.list_objects[i].obj_odom_x - grid->map_ROS_origin_x_) / grid->map_ROS_resolution_;
        int object_y = (objs.list_objects[i].obj_odom_y - grid->map_ROS_origin_y_) / grid->map_ROS_resolution_;

        int robot_x = (objs.list_objects[i].robot_odom_x - grid->map_ROS_origin_x_) / grid->map_ROS_resolution_;
        int robot_y = (objs.list_objects[i].robot_odom_y - grid->map_ROS_origin_y_) / grid->map_ROS_resolution_;

        int ax, ay, bx, by; 
        ax = object_x - robot_x;
        ay = object_y - robot_y;
        std::cout << "r:(" << robot_x << ", " << robot_y << ") - o:(" << object_x << ", " << object_y << ") - ax:" << ax << " - ay:" << ay << std::endl;
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
                        float dist = sqrt(pow(l - object_y, 2) + pow(k - object_x, 2));
                        if(dist <= radius && c->last_time_used != grid->global_counter && c->value == 0 && (c->object_name == objs.list_objects[i].obj_class || c->object_name == "none")){
                            bx = object_x - k;
                            by = object_y - l;
                            int ab = (ax * bx) + (ay * by);
                            int norm = sqrt(pow(ax,2) + pow(ay,2)) * sqrt(pow(bx,2) + pow(by,2));
                            float angle = acos((ab == 0 and norm == 0)?0:ab/norm) * 180/M_PI;
                            std::cout << "ANGLE: " << angle << std::endl;
                            c->heat_map_value.push_back(1 - (radius - dist)/radius);    
                            //c->heat_map_value = 1 - (radius - dist)/radius;    
                            c->object_name = objs.list_objects[i].obj_class;
                            c->last_time_used = grid->global_counter;
                            to_be_processed.push_back(std::make_pair(k, l));
                        }
                    }
                }
            }
        }
    }
    updating_grid_now = false;
}

void Planning::setLogMode(LogMode log){
    logMode_ = log;
}
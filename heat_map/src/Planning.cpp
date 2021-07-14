#include "Misc.h"
#include <Planning.h>

Planning::Planning(std::string goal_obj_class){
    goal_object = goal_obj_class; 
    goal_counter_ = 0;
    updating_grid_now = false;
    semanticHP = new SemanticHP();
    searchingMode = NONE_SEARCHING;
    the_misc = new Misc();
    brute_force_goals_counter_ = 0; 
    is_robot_near_goal = false;
    current_goal = the_misc->getNextGoal(brute_force_goals_counter_);
}

Planning::~Planning(){
}

void Planning::setGrid(Grid *g){
    grid = g;
}

void Planning::initialize(){
    // std::cout << "PLANNING - INITIALIZED" << std::endl;
    searchingMode = SEMANTIC;
    semanticHP->initialize(goal_object, &updating_grid_now);
}

bool Planning::run(){
    if(searchingMode == BRUTE_FORCE){
        // std::cout << "PLANNING - RUN - BRUTE FORCE - BRUTEFORCE COUNTER: " << brute_force_goals_counter_ << " - Goal: [" << current_goal.robot_odom_x << ", " << current_goal.robot_odom_y << ", " << current_goal.robot_yaw << "]" << std::endl;
    }else if(searchingMode == SEMANTIC){
        // std::cout << "PLANNING - running2 - semantic" << std::endl;
        updateHeatValeuWithinMapSemantic();
        if(logMode_ == QUERYING){
            // std::cout << "PLANNING - QUERYING MODE - SEMANTIC" << std::endl;
            semanticHP->findMostLikelyPosition(grid, objs.list_objects);
        }        
    }else if(searchingMode == LAST_SEEN){
        updateHeatValeuWithinMap();
        if(logMode_ == QUERYING){
            // std::cout << "PLANNING - QUERYING MODE - SEMANTIC" << std::endl;
            semanticHP->findMostLikelyPosition(grid, objs.list_objects);
        }        
    }
    return true;
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

void Planning::updateHeatValeuWithinMapSemantic(){
    updating_grid_now = true;
    grid->cleanHeatMapVector();    
    int size = 1; 
    int radius = 15;   
    std::cout << "updateHeatValeuWithinMapSemantic" << std::endl;
    current_time_ = std::time(nullptr);
    calendar_time_ = *std::localtime(std::addressof(current_time_));    
    int current_hour = calendar_time_.tm_hour;    
    for(int i = 0; i < objs.list_objects.size(); i++){
        if(objs.list_objects[i].obj_class == goal_object){
            grid->global_counter++; 
            int object_x = (objs.list_objects[i].obj_odom_x - grid->map_ROS_origin_x_) / grid->map_ROS_resolution_;
            int object_y = (objs.list_objects[i].obj_odom_y - grid->map_ROS_origin_y_) / grid->map_ROS_resolution_;

            int robot_x = (objs.list_objects[i].robot_odom_x - grid->map_ROS_origin_x_) / grid->map_ROS_resolution_;
            int robot_y = (objs.list_objects[i].robot_odom_y - grid->map_ROS_origin_y_) / grid->map_ROS_resolution_;
            // std::cout << "PLANNING - running2 - updateHeatValeuWithinMap: Robot: [" << robot_x << ", " << robot_y << "] - Obj: [" << object_x << ", " << object_y << "]"<< std::endl;
            int robot_obj, cell_obj, robot_cell; 
            robot_obj = sqrt(pow(object_x - robot_x, 2) + pow(object_y - robot_y, 2));
            std::vector<std::pair<int, int>> to_be_processed; 
            to_be_processed.clear(); 
            to_be_processed.push_back(std::make_pair(object_x, object_y)); 
            while(!to_be_processed.empty()){   
                std::pair<int, int> index = to_be_processed.back(); 
                to_be_processed.pop_back(); 
                for(int l = index.second - size; l <= index.second + size; ++l){
                    for(int k = index.first - size; k <= index.first + size; ++k){            
                        if(l > object_y - radius && l < object_y + radius && k > object_x - radius && k < object_x + radius && l > 0 && l < grid->half_num_cels_in_row_ && k > 0 && k < grid->half_num_cels_in_row_){
                            Cell *c = grid->getCell(k, l);                        
                            float dist = sqrt(pow(l - object_y, 2) + pow(k - object_x, 2));
                            cell_obj = sqrt(pow(object_x - k, 2) + pow(object_y - l, 2));
                            robot_cell = sqrt(pow(robot_x - k, 2) + pow(robot_y - l, 2));
                            float value = (pow(robot_obj, 2) + pow(cell_obj, 2) - pow(robot_cell, 2))/(2 * robot_obj * cell_obj);
                            value = std::min((float)1, value);
                            value = std::max((float)-1, value);
                            float angle = acos(value) * 180/M_PI;                        
                            if(angle < 12 && dist <= radius && c->last_time_used != grid->global_counter && c->value == 0 && (c->object_name == objs.list_objects[i].obj_class || c->object_name == "none")){
                                c->heat_map_value += (radius - dist)/radius * semanticHP->hour_weight_table[current_hour][objs.list_objects[i].hours_detection];    
                                c->object_name = objs.list_objects[i].obj_class;
                                c->last_time_used = grid->global_counter;
                                c->obj_x = object_x;
                                c->obj_y = object_y;
                                to_be_processed.push_back(std::make_pair(k, l));
                            }
                        }
                    }
                }
            }
        }
    }
    updating_grid_now = false;    
}

void Planning::updateHeatValeuWithinMap(){
    
    updating_grid_now = true;
    grid->cleanHeatMapVector();
    int size = 1; 
    int radius = 15;    
    for(int i = 0; i < objs.list_objects.size(); i++){
        grid->global_counter++; 
        int object_x = (objs.list_objects[i].obj_odom_x - grid->map_ROS_origin_x_) / grid->map_ROS_resolution_;
        int object_y = (objs.list_objects[i].obj_odom_y - grid->map_ROS_origin_y_) / grid->map_ROS_resolution_;

        int robot_x = (objs.list_objects[i].robot_odom_x - grid->map_ROS_origin_x_) / grid->map_ROS_resolution_;
        int robot_y = (objs.list_objects[i].robot_odom_y - grid->map_ROS_origin_y_) / grid->map_ROS_resolution_;
        // std::cout << "PLANNING - running2 - updateHeatValeuWithinMap: Robot: [" << robot_x << ", " << robot_y << "] - Obj: [" << object_x << ", " << object_y << "]"<< std::endl;
        int robot_obj, cell_obj, robot_cell; 
        robot_obj = sqrt(pow(object_x - robot_x, 2) + pow(object_y - robot_y, 2));
        std::vector<std::pair<int, int>> to_be_processed; 
        to_be_processed.clear(); 
        to_be_processed.push_back(std::make_pair(object_x, object_y)); 
        while(!to_be_processed.empty()){   
            std::pair<int, int> index = to_be_processed.back(); 
            to_be_processed.pop_back(); 
            for(int l = index.second - size; l <= index.second + size; ++l){
                for(int k = index.first - size; k <= index.first + size; ++k){            
                    if(l > object_y - radius && l < object_y + radius && k > object_x - radius && k < object_x + radius && l > 0 && l < grid->half_num_cels_in_row_ && k > 0 && k < grid->half_num_cels_in_row_){
                        Cell *c = grid->getCell(k, l);                        
                        float dist = sqrt(pow(l - object_y, 2) + pow(k - object_x, 2));
                        cell_obj = sqrt(pow(object_x - k, 2) + pow(object_y - l, 2));
                        robot_cell = sqrt(pow(robot_x - k, 2) + pow(robot_y - l, 2));
                        float value = (pow(robot_obj, 2) + pow(cell_obj, 2) - pow(robot_cell, 2))/(2 * robot_obj * cell_obj);
                        value = std::min((float)1, value);
                        value = std::max((float)-1, value);
                        float angle = acos(value) * 180/M_PI;                        
                        if(angle < 120 && dist <= radius && c->last_time_used != grid->global_counter && c->value == 0 && (c->object_name == objs.list_objects[i].obj_class || c->object_name == "none")){
                            c->heat_map_value = std::max(c->heat_map_value, (radius - dist)/radius);    
                            c->object_name = objs.list_objects[i].obj_class;
                            c->last_time_used = grid->global_counter;
                            c->obj_x = object_x;
                            c->obj_y = object_y;
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

void Planning::setSearchingMode(SearchingMode the_searching_mode){
    //searchingMode = the_searching_mode;
}

void Planning::increaseBruteForceGoalCounter(){
    usleep(5000);
    brute_force_goals_counter_++;
    current_goal = the_misc->getNextGoal(brute_force_goals_counter_);
}
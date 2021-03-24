#include <Objects.h>

Object::Object(){
    obj_odom_x = obj_odom_y = robot_odom_x = robot_odom_y = hours_detection = -1; 
    obj_class = "empty";    
}

Objects::Objects(){
    
}

bool Objects::updateObjects(const std::vector<Object> current_list){
    Object *new_obj; 
     
    for(int i = 0; i < current_list.size(); i++){
        if(insertIfNotExist(current_list[i])){
            new_obj->robot_odom_x = current_list[i].robot_odom_x;
            new_obj->robot_odom_y = current_list[i].robot_odom_y;
            new_obj->obj_odom_x = current_list[i].obj_odom_x;
            new_obj->obj_odom_y = current_list[i].obj_odom_y;
            new_obj->obj_class = current_list[i].obj_class;
            new_obj->hours_detection = current_list[i].hours_detection;
            list_objects.push_back(new_obj);
        }
    }
    return true;
}

bool Objects::insertIfNotExist(Object the_object){
    bool should_insert = true;
    int tollerance = 5;
    for(int i = 0; i < list_objects.size(); i++){
        if(list_objects[i]->obj_class == the_object.obj_class && list_objects[i]->hours_detection == the_object.hours_detection){
            int robot_list_x = list_objects[i]->robot_odom_x * 10;
            int robot_list_y = list_objects[i]->robot_odom_y * 10;
            int robot_new_x = the_object.robot_odom_x * 10;
            int robot_new_y = the_object.robot_odom_y * 10;
            int diff_x = abs(robot_list_x - robot_new_x);
            int diff_y = abs(robot_list_y - robot_new_y);
            if(diff_x <= tollerance && diff_y <= tollerance)
                should_insert = false;            
        }
    }
    return should_insert;
}

/* 
void Robot_ROS::writeNewObjectToFile(ObjectInfo new_object){
    if(my_file.is_open()){
        my_file << "#\n" << 
        new_object.obj_class << "\n" << 
        new_object.obj_map_x << "\n" << 
        new_object.obj_map_y << "\n" << 
        new_object.robot_map_x << "\n" << 
        new_object.robot_map_y << "\n" << 
        new_object.hours_detection << "\n" <<
        "@\n";
    }
}

bool Robot_ROS::checkObjectClass(std::string objects_class){
    if(objects_class == "mug" || 
       objects_class == "cup" || 
       objects_class == "monitor" || 
       objects_class == "book" || 
       objects_class == "Book" || 
       objects_class == "Mug" || 
       objects_class == "Mobile phone" || 
       objects_class == "Computer mouse" || 
       objects_class == "Pen" || 
       objects_class == "Computer monitor" || 
       objects_class == "tvmonitor" )
        return true;
    else
        return false;
}

void Robot_ROS::updateHeatValeuWithinMap(){
    for(int i = 0; i < objects_list_.size(); i++){
        if(objects_list_[i].already_plotted == false){
            global_counter_++;
            //objects_list_[i].already_plotted = true;
            int size = 1; 
            int radius = 500; 
            int object_x = (objects_list_[i].obj_map_x - mapROS_.info.origin.position.x) / mapROS_.info.resolution;
            int object_y = (objects_list_[i].obj_map_y - mapROS_.info.origin.position.y) / mapROS_.info.resolution;
            std::vector<std::pair<int, int>> to_be_processed; 
            to_be_processed.clear(); 
            to_be_processed.push_back(std::make_pair(object_x, object_y)); 
            while(!to_be_processed.empty()){   
                std::pair<int, int> index = to_be_processed.back(); 
                to_be_processed.pop_back(); 

                for(int l = index.second - size; l <= index.second + size; ++l){
                    for(int k = index.first - size; k <= index.first + size; ++k){            
                        if(l > object_y - radius && l < object_y + radius && k > object_x - radius && k < object_x + radius){
                            Cell *c = grid_->getCell(k, l);
                            float dist = pow(l - object_y, 2) + pow(k - object_x, 2);
                            if(dist <= radius && c->last_time_used != global_counter_ && c->value == 0){
                                //c->heat_map_value.push_back(1 - (radius - dist)/radius);    
                                c->heat_map_value = 1 - (radius - dist)/radius;    
                                c->object_name = objects_list_[i].obj_class;
                                c->last_time_used = global_counter_;
                                to_be_processed.push_back(std::make_pair(k, l));
                            }
                        }
                    }
                }
            }
        }
    }    
}

 */


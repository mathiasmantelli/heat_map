#include <Objects.h>
#include <cstdio>

Object::Object(std::string class_obj, float odom_obj_x, float odom_obj_y, float odom_robot_x, float odom_robot_y, float hours){
    obj_odom_x = odom_obj_x;
    obj_odom_y = odom_obj_y; 
    robot_odom_x = odom_robot_x;
    robot_odom_y = odom_obj_y;
    hours_detection = hours; 
    obj_class = class_obj;    
}

Objects::Objects(){
    list_objects_.open("src/heat_map/config/list_objects.txt", std::ios::in);
    object_classes_.clear(); 
    std::string new_line;
    if(list_objects_.is_open()){
        while(std::getline(list_objects_, new_line)){
            object_classes_.push_back(new_line);
        }
        list_objects_.close();
    }else{
        std::cout << "The file 'list_objects' is not open!" << std::endl;
    }
}

bool Objects::updateObjects(const std::vector<Object*> current_list){
    std::cout << "Object class - size of objects to be included: " << current_list.size() << std::endl; 
    Object *the_objct; 
    for(auto iter = current_list.begin(); iter != current_list.end(); iter++){
        if(insertIfNotExist(*iter) && correctObjectClass(*iter)){

            if(writeObjectListOnFile(*iter))
                std::cout << "################# " << (*iter)->obj_class << " is written in the file." << std::endl;
            else
                std::cout << (*iter)->obj_class << " is NOT written in the file." << std::endl;
            std::cout << "@@@@@@@@@@@@@@@@@ " << (*iter)->obj_class << std::endl;
            the_objct = new Object((*iter)->obj_class, (*iter)->obj_odom_x, (*iter)->obj_odom_y, (*iter)->robot_odom_x, (*iter)->robot_odom_y, (*iter)->hours_detection);
            list_objects.push_back(the_objct);
        }else{
            std::cout << "OBJECT NOT INSERTED - EXIST: " << insertIfNotExist(*iter) << " or CORRECT CLASS: " << correctObjectClass(*iter) << std::endl;
        }
    }
    return true;
}

bool Objects::insertIfNotExist(Object* the_object){
    bool should_insert = true;
    int tollerance = 2;
    std::cout << "insertIfNotExist | " << the_object->obj_class << " - " << the_object->hours_detection << " List: ";
    for(int i = 0; i < list_objects.size(); i++){
        std::cout << i << " " << list_objects[i]->obj_class << " - " << list_objects[i]->hours_detection << " | ";
        if(list_objects[i]->obj_class == the_object->obj_class && list_objects[i]->hours_detection == the_object->hours_detection){
            int robot_list_x = list_objects[i]->robot_odom_x * 10;
            int robot_list_y = list_objects[i]->robot_odom_y * 10;
            int robot_new_x = the_object->robot_odom_x * 10;
            int robot_new_y = the_object->robot_odom_y * 10;
            int diff_x = abs(robot_list_x - robot_new_x);
            int diff_y = abs(robot_list_y - robot_new_y);
            std::cout << robot_list_x << "," << robot_new_x << std::endl;
            if(diff_x <= tollerance && diff_y <= tollerance)
                should_insert = false;            
        }
    }
    std::cout << std::endl;
    return should_insert;
}

bool Objects::correctObjectClass(Object* the_object){
    bool same_class = false;
    for(auto i:object_classes_)
        if(the_object->obj_class == i)
            same_class = true;
    
    return same_class;    

}

bool Objects::writeObjectListOnFile(Object* the_object){
    output_file.open("list_objects.txt", std::ios::trunc);    
    if(output_file.is_open()){        
        output_file << "#\n" << 
        the_object->obj_class << "\n" <<
        the_object->obj_odom_x << "\n" <<
        the_object->obj_odom_y << "\n" <<
        the_object->robot_odom_x << "\n" <<
        the_object->robot_odom_y << "\n" <<            
        the_object->hours_detection << "\n" <<
        "@\n";
        output_file.close();
        return true;
    }else {
        std::cout << "THE OUTPUT FILE IS NOT OPEN!" << std::endl;
        return false;
    }
    
}


#include <Objects.h>
#include <cstdio>

Object::Object(){
    obj_odom_x = obj_odom_y = robot_odom_x = robot_odom_y = hours_detection = -1; 
    obj_class = "empty";    
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

bool Objects::updateObjects(const std::vector<Object> &current_list){
    std::cout << "Object class - size of objects to be included: " << current_list.size() << std::endl; 
    std::vector<Object>::iterator it;
    for(it = current_list.begin(); it != current_list.end(); it++){
        const Object& the_obj = *i;
        if(insertIfNotExist(*i) && correctObjectClass(*i)){
            Object *new_obj; 
            new_obj->obj_class = current_list[i].obj_class;
            new_obj->obj_odom_x = current_list[i].obj_odom_x;
            new_obj->obj_odom_y = current_list[i].obj_odom_y;
            new_obj->robot_odom_x = current_list[i].robot_odom_x;
            new_obj->robot_odom_y = current_list[i].robot_odom_y;

            if(writeObjectListOnFile(new_obj))
                std::cout << "################# " << new_obj->obj_class << " is written in the file." << std::endl;
            else
                std::cout << new_obj->obj_class << " is NOT written in the file." << std::endl;
            std::cout << "@@@@@@@@@@@@@@@@@ " << new_obj->obj_class << std::endl;
            list_objects.push_back(new_obj);
            delete new_obj;
        }else{
            std::cout << "OBJECT NOT INSERTED - EXIST: " << insertIfNotExist(current_list[i]) << " or CORRECT CLASS: " << correctObjectClass(current_list[i]) << std::endl;
        }
    }
    return true;
}

bool Objects::insertIfNotExist(Object the_object){
    bool should_insert = true;
    int tollerance = 2;
    std::cout << "insertIfNotExist | " << the_object.obj_class << " - " << the_object.hours_detection << " List: ";
    for(int i = 0; i < list_objects.size(); i++){
        std::cout << i << " " << list_objects[i]->obj_class << " - " << list_objects[i]->hours_detection << " | ";
        if(list_objects[i]->obj_class == the_object.obj_class && list_objects[i]->hours_detection == the_object.hours_detection){
            int robot_list_x = list_objects[i]->robot_odom_x * 10;
            int robot_list_y = list_objects[i]->robot_odom_y * 10;
            int robot_new_x = the_object.robot_odom_x * 10;
            int robot_new_y = the_object.robot_odom_y * 10;
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

bool Objects::correctObjectClass(Object the_object){
    bool same_class = false;
    for(auto i:object_classes_)
        if(the_object.obj_class == i)
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


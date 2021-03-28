#include <Objects.h>

Object::Object(){
    obj_odom_x = obj_odom_y = robot_odom_x = robot_odom_y = hours_detection = -1; 
    obj_class = "empty";    
}

Objects::Objects(){
    output_file.open("list_objects.txt", std::ios::trunc);    
    list_objects_.open("src/heat_map/config/list_objects.txt", std::ios::in);
    object_classes_.clear(); 
    

///home/mathias/catkin_yolo/src/heat_map/config/    
}

bool Objects::updateObjects(const std::vector<Object> current_list){
    Object *new_obj; 
     
    for(int i = 0; i < current_list.size(); i++){
        if(insertIfNotExist(current_list[i]) && correctObjectClass(current_list[i])){
            float obj_odom_X, obj_odom_Y;
            obj_odom_X = current_list[i].obj_odom_x;
            obj_odom_Y = current_list[i].obj_odom_y;
            new_obj->robot_odom_x = current_list[i].robot_odom_x;
            new_obj->robot_odom_y = current_list[i].robot_odom_y;
            new_obj->obj_odom_x = obj_odom_X;
            new_obj->obj_odom_y = obj_odom_Y;
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

bool Objects::correctObjectClass(Object the_object){
    if(the_object.obj_class == "mug" || 
       the_object.obj_class == "cup" || 
       the_object.obj_class == "monitor" || 
       the_object.obj_class == "book" || 
       the_object.obj_class == "Book" || 
       the_object.obj_class == "Mug" || 
       the_object.obj_class == "Mobile phone" || 
       the_object.obj_class == "Computer mouse" || 
       the_object.obj_class == "Pen" || 
       the_object.obj_class == "Computer monitor" || 
       the_object.obj_class == "tvmonitor" )
        return true;
    else
        return false;    

}

bool Objects::writeObjectListOnFile(){
    if(output_file.is_open()){
        for(int i = 0; i < list_objects.size(); i++){
            output_file << "#\n" << 
            list_objects[i]->obj_class << "\n" <<
            list_objects[i]->obj_odom_x << "\n" <<
            list_objects[i]->obj_odom_y << "\n" <<
            list_objects[i]->robot_odom_x << "\n" <<
            list_objects[i]->robot_odom_y << "\n" <<            
            list_objects[i]->hours_detection << "\n" <<
            "@\n";
        }
        return true;
    }else {
        std::cout << "THE OUTPUT FILE IS NOT OPEN!" << std::endl;
        return false;
    }
    
}


#include <Objects.h>
#include <cstdio>
#include <tuple>
using std::cout;

Object::Object() {
  obj_odom_x = -1;
  obj_odom_y = -1;
  robot_odom_x = -1;
  robot_odom_y = -1;
  hours_detection = -1;
  obj_class = "-";
}
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

bool Objects::updateObjects(const std::vector<Object> current_list){
    std::cout << "Object class - size of objects to be included: " << current_list.size() << std::endl; 
    
    for(auto iter = current_list.begin(); iter != current_list.end(); iter++){
        printObject(*iter);
        const Object& cur_obj = *iter;
        if(insertIfNotExist(cur_obj) && correctObjectClass(cur_obj)){
            if(writeObjectListOnFile(cur_obj))
                std::cout << "################# " << cur_obj.obj_class << " is written in the file." << std::endl;
            std::cout << "@@@@@@@@@@@@@@@@@ " << cur_obj.obj_class << std::endl;
            Object the_objct(cur_obj.obj_class, cur_obj.obj_odom_x, cur_obj.obj_odom_y, cur_obj.robot_odom_x, cur_obj.robot_odom_y, cur_obj.hours_detection);
            list_objects.push_back(the_objct);
        }else{
            std::cout << "OBJECT NOT INSERTED - EXIST: " << insertIfNotExist(cur_obj) << " or CORRECT CLASS: " << correctObjectClass(cur_obj) << std::endl;
        }
    }
    return true;
}

bool Objects::insertIfNotExist(Object the_object){
    bool should_insert = true;
    int tollerance = 2;
    for(int i = 0; i < list_objects.size(); i++){
        if(list_objects[i].obj_class == the_object.obj_class && list_objects[i].hours_detection == the_object.hours_detection){
             int robot_list_x = list_objects[i].robot_odom_x * 10;
            int robot_list_y = list_objects[i].robot_odom_y * 10;
            int robot_new_x = the_object.robot_odom_x * 10;
            int robot_new_y = the_object.robot_odom_y * 10;
            /*int diff_x = abs(robot_list_x - robot_new_x);
            int diff_y = abs(robot_list_y - robot_new_y); */
            float obj_euclidean, robot_euclidean;
            std::tie(obj_euclidean, robot_euclidean) = euclideanDistance(list_objects[i], the_object);
            std::cout << i << " - " << "Robot_list: [ " << robot_list_x << ", " << robot_list_y << " - Robot_new: [ " << robot_new_x << ", " << robot_new_y << "] - Diffs: [ " << obj_euclidean << ", " << robot_euclidean << "]" << std::endl; 
            if(obj_euclidean <= tollerance)
                should_insert = false;            
        }
    }
    //std::cout << std::endl;
    return should_insert;
}

std::tuple<float, float> Objects::euclideanDistance(Object list_obj, Object new_obj){
    float obj_euclidean, robot_euclidean;
    obj_euclidean = sqrt(powf(list_obj.obj_odom_x - new_obj.obj_odom_x, 2) + powf(list_obj.obj_odom_x - new_obj.obj_odom_x, 2));
    robot_euclidean = sqrt(powf(list_obj.robot_odom_x - new_obj.robot_odom_x, 2) + powf(list_obj.robot_odom_y - new_obj.robot_odom_y, 2));
    return std::make_tuple(obj_euclidean, robot_euclidean);
}

bool Objects::correctObjectClass(Object the_object){
    bool same_class = false;
    for(auto i:object_classes_)
        if(the_object.obj_class == i)
            same_class = true;
    
    return same_class;    

}

bool Objects::writeObjectListOnFile(Object the_object){
    output_file.open("list_objects.txt", std::ios::app);    
    if(output_file.is_open()){        
        output_file << "#\n" << 
        the_object.obj_class << "\n" <<
        the_object.obj_odom_x << "\n" <<
        the_object.obj_odom_y << "\n" <<
        the_object.robot_odom_x << "\n" <<
        the_object.robot_odom_y << "\n" <<            
        the_object.hours_detection << "\n" <<
        "@\n";
        output_file.close();
        return true;
    }else {
        std::cout << "THE OUTPUT FILE IS NOT OPEN!" << std::endl;
        return false;
    }
    
}

void Objects::printObject(Object the_object){
  cout << "Class: " << the_object.obj_class << "\n - OBJ:[ "
       << the_object.obj_odom_x << "," << the_object.obj_odom_y
       << " ]\n - ROBOT:[ " << the_object.robot_odom_x << ","
       << the_object.robot_odom_y
       << " ]\n - HOUR: " << the_object.hours_detection << std::endl;
}
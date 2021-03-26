#ifndef OBJECTS_H
#define OBJECTS_H

#include <string> 
#include <vector>
#include <tuple>
#include <set>
#include <GL/glut.h>
#include <GL/freeglut.h>
#include <iostream>
#include <fstream>

class Object{
public:
    Object();
    float obj_odom_x, obj_odom_y, robot_odom_x, robot_odom_y, hours_detection; 
    std::string obj_class;
};

class Objects{
public:
    Objects(); 
    bool updateObjects(const std::vector<Object> current_list);
    bool insertIfNotExist(Object the_object);
    std::vector<Object*> list_objects;
    bool writeObjectListOnFile();
private:
    std::ofstream output_file;    
    bool correctObjectClass(Object the_object);
};

#endif
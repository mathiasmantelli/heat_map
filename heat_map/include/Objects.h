#ifndef OBJECTS_H
#define OBJECTS_H

#include <string> 
#include <vector>
#include <set>
#include <GL/glut.h>
#include <GL/freeglut.h>

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
};

#endif
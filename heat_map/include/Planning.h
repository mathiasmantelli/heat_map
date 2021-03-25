#ifndef __PLANNING_H__
#define __PLANNING_H__

class Planning;

#include <pthread.h>
#include <queue>
#include "Grid.h"
#include "Objects.h"
#include "Robot.h"
#include <string>


class Planning{
public:
    Planning();
    ~Planning(); 

    bool run(); 
    
    void initialize(); 
    void setGrid(Grid* g);
    void updateHeatValeuWithinMap();
    
    Grid* grid; 

    Objects objs; 

private:
    std::string goal_object; 
    bool object_found(std::string obj_class);
};

#endif /* __PLANNING_H__ */

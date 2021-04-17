#ifndef __PLANNING_H__
#define __PLANNING_H__

class Planning;

#include <pthread.h>
#include <queue>
#include "Grid.h"
#include "Objects.h"
#include "Robot.h"
#include <string>
#include "SemanticHP.h"
#include "Misc.h"
#include <cmath>

class Planning{
public:
    Planning(std::string goal_obj_class);
    ~Planning(); 

    bool run(); 
    
    void initialize(); 
    void setGrid(Grid* g);
    void updateHeatValeuWithinMap();
    void setLogMode(LogMode log);
    bool updating_grid_now;
    Grid* grid; 
    SearchingMode searchingMode;
    Objects objs; 

private:
    LogMode logMode_;
    SemanticHP* semanticHP;
    std::string goal_object; 
    int goal_counter_;
    bool object_found(std::string obj_class);
};

#endif /* __PLANNING_H__ */

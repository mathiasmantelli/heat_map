#ifndef SEMANTICHP_H
#define SEMANTICHP_H

#include "Grid.h"
#include "Objects.h"
#include <iostream>
#include <algorithm>
#include <deque>
#include <vector>
#include <iomanip>
#include <cmath>

class SemanticHP{
public:
    SemanticHP();
    ~SemanticHP();

    void findMostLikelyPosition(Grid *grid, const std::vector<Object> list_objects);
    void setCurrentObjClassGoal(std::string new_goal);
    void initialize(std::string goal_obj_class, bool *upating_map);
    float analyseGridPatch(Grid* grid, Cell* c);

private:
    std::string goal_object_class_;
    MapSize map_size;
    bool *updating_grid_now_;
    int patch_size_, global_counter_, offset_size_;

};
#endif /* __SEMANTICHP_H__ */
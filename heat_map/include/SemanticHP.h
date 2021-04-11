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

    Cell* findMostLikelyPosition(Grid *grid, const std::vector<Object> list_objects);
    void setCurrentObjClassGoal(std::string new_goal);
    void initialize();
    float analyseGridPatch(Grid* grid, Cell* c);

private:
    Cell* goal_position_on_grid_;
    int patch_size_, global_counter_, offset_size_;

};
#endif /* __SEMANTICHP_H__ */
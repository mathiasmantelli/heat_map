#ifndef __SEMANTICHP_H__
#define __SEMANTICHP_H__

#include <Grid.h>
#include <Objects.h>

#include <algorithm>
#include <deque>

class SemanticHP{
public:
    SemanticHP(std::string obj_class);
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
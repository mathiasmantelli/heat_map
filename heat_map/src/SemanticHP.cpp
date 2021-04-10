#include "Grid.h"
#include <SemanticHP.h>

//eight-neighbor offset
int offset[][8]={{-1,  1},
                 { 0,  1},
                 { 1,  1},
                 { 1,  0},
                 { 1, -1},
                 { 0, -1},
                 {-1, -1},
                 {-1,  0}
                };

SemanticHP::SemanticHP(std::string obj_class){
    goal_position_on_grid_->object_name = obj_class;
    goal_position_on_grid_->x = -1;    
    goal_position_on_grid_->y = -1;
    patch_size_ = 500;

    global_counter_ = 0;
    offset_size_ = 8;
}

void SemanticHP::initialize(){
    
}

Cell* SemanticHP::findMostLikelyPosition(Grid *grid, const std::vector<Object> list_objects){
    int num_cells_in_row = grid->getMapNumCellsInRow();
    Cell *current_cell;
    float current_sum, biggest_sum; 
    biggest_sum = -1;
    for(unsigned int j = 0; j < num_cells_in_row; ++j){
        for(unsigned int i = 0; i < num_cells_in_row; ++i){
            unsigned int index = j * num_cells_in_row + i; 
            current_cell = grid->getCell(i, j);
            if(current_cell->value == 0 and current_cell->object_name == goal_position_on_grid_->object_name){
                current_sum = analyseGridPatch(grid, current_cell);
                if(current_sum > biggest_sum){
                    biggest_sum = current_sum;
                    goal_position_on_grid_->x = current_cell->x;
                    goal_position_on_grid_->y = current_cell->y;
                }
            }
        }
    } 
    return goal_position_on_grid_;   
}

float SemanticHP::analyseGridPatch(Grid* grid, Cell* c){
    std::deque<Cell*> to_be_analysed;
    Cell* current_cell, *neighboor_cell;
    global_counter_++;
    c->last_time_used = global_counter_;
    to_be_analysed.clear();
    to_be_analysed.emplace_back(c);
    int width = grid->getMapWidth();
    int height = grid->getMapHeight();
    float sum = 0;
    while(!to_be_analysed.empty()){
        current_cell = to_be_analysed.front();
        to_be_analysed.pop_front();
        for(int n = 0; n < offset_size_; n++){
            int new_x, new_y;
            new_x = current_cell->x+offset[n][0];
            new_y = current_cell->y+offset[n][1];
            if(new_x > 0 and new_x < width and new_y > 0 and new_y < height){
                neighboor_cell = grid->getCell(new_x, new_y);        
                if(neighboor_cell->value == 0 and 
                neighboor_cell->last_time_used != global_counter_ and 
                neighboor_cell->object_name == goal_position_on_grid_->object_name){
                    neighboor_cell->last_time_used = global_counter_;
                    to_be_analysed.emplace_back(neighboor_cell);
                    for(int i = 0; i < neighboor_cell->heat_map_value.size(); i++)
                        sum += neighboor_cell->heat_map_value[i];
                }
            }
        }
    } 
    return sum;
}

void SemanticHP::setCurrentObjClassGoal(std::string new_goal){
    goal_position_on_grid_->object_name = new_goal;
}
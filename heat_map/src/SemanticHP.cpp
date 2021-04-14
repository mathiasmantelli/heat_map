#include "SemanticHP.h"
#include "Grid.h"

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

SemanticHP::SemanticHP(){
    patch_size_ = 30;
    
    global_counter_ = 0;
    offset_size_ = 8;
    
}

void SemanticHP::initialize(std::string goal_obj_class, bool *updating_map){
    goal_object_class_ = goal_obj_class;    
    updating_grid_now_ = updating_map;
}

void SemanticHP::findMostLikelyPosition(Grid *grid, const std::vector<Object> list_objects){
    if(!*updating_grid_now_){
        int num_cells_in_row = grid->getMapNumCellsInRow();
        int goal_i, goal_j;
        goal_j = goal_i = 0;
        map_size = grid->map_limits;
        Cell *current_cell;
        float current_sum, biggest_sum; 
        biggest_sum = -1;
        for(int j = map_size.min_y; j <= map_size.max_y; j++){
            for(int i = map_size.min_x; i <= map_size.max_x; i++){ 
                current_cell = grid->getCell(i, j);
                if(current_cell->value == 0 and current_cell->object_name == goal_object_class_ and !current_cell->heat_map_value.empty()){
                    current_sum = analyseGridPatch(grid, current_cell);
                    if(current_sum > biggest_sum){
                        biggest_sum = current_sum;
                        //grid->goal_cell = current_cell;
                        goal_i = i; 
                        goal_j = j;
                    }
                }
            }
        }  
        grid->goal_cell.x = grid->getCell(goal_i, goal_j)->x;
        grid->goal_cell.y = grid->getCell(goal_i, goal_j)->y;
    }else{
        std::cout << "IGNORED FINDING" << std::endl;
    }
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
                float dist = pow(new_y - c->y, 2) + pow(new_x - c->x, 2);
                if(dist <= patch_size_ and neighboor_cell->value == 0 and 
                neighboor_cell->last_time_used != global_counter_ and 
                neighboor_cell->object_name == goal_object_class_){
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
    goal_object_class_ = new_goal;
}
#include "Grid.h"
#include <GL/gl.h>

Grid::Grid(){
    map_scale_ = 20; 
    map_width_ = map_height_ = 2000;
    num_cells_in_row_ = map_width_; 
    half_num_cels_in_row_ = map_width_ / 2; 
    my_map_ = new Cell[map_width_ * map_height_];

    for(unsigned int j = 0; j < num_cells_in_row_; ++j){
        for(unsigned int i = 0; i < num_cells_in_row_; ++i){
            unsigned int index = j * num_cells_in_row_ + i; 
            my_map_[index].x = -half_num_cels_in_row_ + 1 + i;
            my_map_[index].y = half_num_cels_in_row_ - j;
            my_map_[index].value = -1;
        }
    }

    min_x = min_y = map_width_ / 2; 
    max_x = max_y = map_height_ / 2;

    num_view_modes = 5;
    view_mode = 0; 
    iterations = 0;
}

Grid::Cell* Grid::getCell(int x, int y){
    int i = x + half_num_cels_in_row_ - 1;
    int j = half_num_cels_in_row_ - y; 
    return &(my_map_[j * num_cells_in_row_ + i]);
}

int Grid::getMapScale(){
    return map_scale_;
}

int Grid::getMapHeight(){
    return map_height_;
}

int Grid::getMapWidth(){
    return map_width_;
}

void Grid::updateBoundaries(int i, int j){
    if(i < min_x) min_x = i;
    if(i > max_x) max_x = i;

    if(j < min_y) min_y = j;
    if(j > max_y) max_y = j;    
}

void Grid::draw(int xi, int yi, int xf, int yf){
    glLoadIdentity();

    for(int i = 0; i <= map_height_; ++i){
        for(int j = 0; j <= map_height_; ++j){
            drawCell(i + j * num_cells_in_row_);
        }
    }
}

void Grid::drawText(unsigned int i){

}

void Grid::drawCell(unsigned int i){
    glColor3f(1.0, 1,0);
}
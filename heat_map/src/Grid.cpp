#include "Grid.h"
#include <GL/gl.h>

Grid::Grid(){
    map_scale_ = 10; 
    map_width_ = map_height_ = 2000;
    num_cells_in_row_ = map_width_; 
    half_num_cels_in_row_ = map_width_ / 2; 
    my_map_ = new Cell[map_width_ * map_height_];

    for(unsigned int j = 0; j < num_cells_in_row_; ++j){
        for(unsigned int i = 0; i < num_cells_in_row_; ++i){
            unsigned int index = j * num_cells_in_row_ + i; 
            my_map_[index].x = -half_num_cels_in_row_ + i + 1;
            my_map_[index].y = half_num_cels_in_row_ - j;
            my_map_[index].value = -1;
        }
    }

    map_limits.min_x = map_limits.min_y = 1000000;
    map_limits.max_x = map_limits.max_y = -1000000;

    num_view_modes = 5;
    view_mode = 0; 
    iterations = 0;
}

Cell* Grid::getCell(int x, int y){
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

    for(int i = xi; i <= xf; ++i){
        for(int j = yi; j <= yf; ++j){
            drawCell(j * num_cells_in_row_ + i);
        }
    }
}

void Grid::drawText(unsigned int i){

}

void Grid::drawCell(unsigned int n){
    if(my_map_[n].value == 100)
        glColor3f(1.0, 0, 0);
    else if(my_map_[n].value == 0)
        glColor3f(0, 0, 1.0);
    else if(my_map_[n].value == -1)
        glColor3f(0, 1.0, 0);
    glBegin( GL_QUADS );
    { 
        glVertex2f(my_map_[n].x+1, my_map_[n].y+1);
        glVertex2f(my_map_[n].x+1, my_map_[n].y  );
        glVertex2f(my_map_[n].x  , my_map_[n].y  );
        glVertex2f(my_map_[n].x  , my_map_[n].y+1);
    }
    glEnd();    
}

void Grid::setMapWidth(int width){
    map_width_ = width;
}
void Grid::setMapHeight(int height){
    map_height_ = height;
}
void Grid::setMapScale(float scale){
    map_scale_ = scale;
}
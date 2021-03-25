#include "Grid.h"
#include <GL/gl.h>
#include <algorithm>

Grid::Grid(){
    map_scale_ = 30; 
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
            my_map_[index].robot_path = false;
            //my_map_[index].heat_map_value.clear();
            my_map_[index].heat_map_value = 0;
            my_map_[index].object_name = "none";
            my_map_[index].last_time_used = 0; 
        }
    }

    num_view_modes = 3;
    view_mode = 0; 
    global_counter = 0;
    map_limits.min_x = map_limits.min_y = 1000000;
    map_limits.max_x = map_limits.max_y = -1000000;
}

Cell* Grid::getCell(int x, int y){
    int i = x + half_num_cels_in_row_ - 1; 
    int j = half_num_cels_in_row_ - y; 
    return &(my_map_[j * num_cells_in_row_ + i]);
}

int Grid::getMapScale(){
    return map_scale_;
}

int Grid::getMapWidth(){
    return map_width_;
}

int Grid::getMapHeight(){
    return map_height_;
}

/* void Grid::updateBoundaries(int i, int j){
    if(i < min_x) min_x = i;
    if(i > max_x) max_x = i;

    if(j < min_y) min_y = j;
    if(j > max_y) max_y = j;    
} */

void Grid::draw(int xi, int yi, int xf, int yf){
    glLoadIdentity();

    for(int i = xi; i <= xf; ++i){
        for(int j = yi; j <= yf; ++j){
            drawCell(i + j * num_cells_in_row_);
        }
    }
}

void Grid::drawText(unsigned int i){

}

void Grid::drawCell(unsigned int n){
    switch (view_mode) {
    case 0:
        if(my_map_[n].value == 100)
            glColor3f(0, 0, 0);
        else if(my_map_[n].value == 0)
            glColor3f(.95, .95, .95);
        else if(my_map_[n].value == -1)
            glColor4f(0.0f, 1.0f, 1.0f, 1.0f);
        break;
    case 1:
        if(my_map_[n].robot_path)
            glColor3f(1.0, 0, 0);
        else if(my_map_[n].value == 100)
            glColor3f(0, 0, 0);
        else if(my_map_[n].value == 0)
            glColor3f(.95, .95, .95);
        else if(my_map_[n].value == -1)
            glColor4f(0.0f, 1.0f, 1.0f, 1.0f); 
        break;
    case 2:
/*         if(!my_map_[n].heat_map_value.empty()){
            int sum = 0;
            for(auto i : my_map_[n].heat_map_value)
                sum += i;
            glColor3f(1, sum, 0);  */
        if(my_map_[n].heat_map_value != 0){
            glColor3f(1, my_map_[n].heat_map_value, 0);                
        }else if(my_map_[n].value == 100)
            glColor3f(0, 0, 0);
        else if(my_map_[n].value == 0)
            glColor3f(0, .95, 0);
        else if(my_map_[n].value == -1)
            glColor4f(0.0f, 1.0f, 1.0f, 1.0f);    
        break;
    }


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

void Grid::setMapROSWidth(int new_width){
    map_ROS_width_ = new_width; 
}

void Grid::setMapROSOrigin(float ros_map_origin_x, float ros_map_origin_y){
    map_ROS_origin_x_ = ros_map_origin_x;
    map_ROS_origin_y_ = ros_map_origin_y;
}

void Grid::setMapROSResolution(float ros_map_resolution){
    map_ROS_resolution_ = ros_map_resolution;
}

int Grid::matrixToVectorIndex(int i, int j){
    return i + j * map_ROS_width_;
}

std::tuple<int, int> Grid::vectorToMatrixIndex(int index){
    int j = index / map_ROS_width_; 
    int i = index - j * map_ROS_width_; 
    return std::make_tuple(i, j);
}

std::tuple<int, int> Grid::transformCoordinateOdomToMap(float x, float y){
    int j = (y - map_ROS_origin_y_)/map_ROS_resolution_;
    int i = (x - map_ROS_origin_x_)/map_ROS_resolution_;
    return std::make_tuple(i, j);
}

std::tuple<float, float> Grid::transformCoordinateMapToOdom(int x, int y){
    float i = (x + map_ROS_origin_x_/map_ROS_resolution_)*map_ROS_resolution_;
    float j = (y + map_ROS_origin_y_/map_ROS_resolution_)*map_ROS_resolution_;
    return std::make_tuple(i, j);
}
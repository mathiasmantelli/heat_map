#ifndef __GRID_H__
#define __GRID_H__
#include <cstdio>
#include <GL/glut.h>
#include <iostream>
#include <sstream>
#include <iomanip>

#include "math.h"

struct Cell{
    int x, y, value; 
};

struct MapSize{
    int min_x, min_y, max_x, max_y;
};

class Grid{
public:
    Grid(); 
    Cell* getCell(int x, int y); 
    int getMapScale();
    int getMapWidth();
    int getMapHeight();
    void setMapWidth(int width);
    void setMapHeight(int height); 
    void setMapScale(float scale); 

    void draw(int xi, int yi, int xf, int yf);

    void updateBoundaries(int i, int j);

    int min_x, min_y, max_x, max_y;
    int num_view_modes, view_mode, iterations;

    pthread_mutex_t* grid_mutex; 

    MapSize map_limits; 
    
private:
    int map_width_, map_height_, num_cells_in_row_, half_num_cels_in_row_; 
    float map_scale_;

    Cell* my_map_;
    void drawCell(unsigned int i); 
    void drawText(unsigned int i);
};
#endif // __GRID_H__
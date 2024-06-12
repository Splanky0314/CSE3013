//
//  water_line.h
//  firstSketch
//
//  Created by 민동현 on 26/09/2019.
//

#ifndef water_line_h
#define water_line_h

#include "ofMain.h"

#define EPSILON 0.01

typedef struct {
    int x1;
    int y1;
    int x2;
    int y2;
    double slope;
    double constant;
    double x_coef;
    double y_coef;
} LineSegment;

typedef struct {
    int x1;
    int y1;
    int x2;
    int y2;
} Dot;

class WaterLine{
    
public:
    WaterLine(int num_of_line);
    ~WaterLine();
    
    void reset();
    void update();
    void draw();
    void calculate_path(LineSegment *lineseg, int num_of_line);
    
    Dot* path;
    
    float uniqueColor_r, uniqueColor_g, uniqueColor_b;
    float scale;
    int hexcolor;
    
    int path_idx;
    int draw_complete;
    int calc_complete;
    ofPoint start_dot;
    float dot_radius;
    
};

#endif /* water_line_h */

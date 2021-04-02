#include "../include/GlutClass.h"
#include "Robot_ROS.h"
#include <GL/freeglut_std.h>
#include <GL/gl.h>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>

GlutClass::GlutClass(){
}

GlutClass::~GlutClass(){
}

GlutClass* GlutClass::instance_ = 0; 

GlutClass* GlutClass::getInstance (){
    if(instance_ == 0)
        instance_ = new GlutClass;

    return instance_;
}

void GlutClass::initialize(){
/*     cv::Mat image = imread("map_of_objects.pgm", cv::IMREAD_GRAYSCALE);
//    robot_pose_ = robot_->getRobotsPose();
//    image.at<float>(robot_pose_.robot_map_y, robot_pose_.robot_map_x) = 255;
    cv::imshow("Image", image);
    cv::waitKey(500);                  */
    halfWindowSize = 700; 
    x_aux = 0;
    y_aux = 35; 
    glutWindowSize = 900;

    while(robot_->isReady() == false){
        usleep(100000);
    }
    grid_ = robot_->grid_map;

	int argc=0;char** argv=0;
    glutInit(&argc, argv);
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize (halfWindowSize, halfWindowSize);
    id_ = glutCreateWindow("Heat Map");
    
    glClearColor (1.0, 1.0, 1.0, 0.0);
    glClear (GL_COLOR_BUFFER_BIT);

    glEnable (GL_BLEND); glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glutDisplayFunc(display); 
    glutKeyboardFunc(keyboard);   
}

void GlutClass::process(){
    glutMainLoop();
}

void GlutClass::terminate(){
    //robot_->motionMode = ENDING;
}

void GlutClass::setRobot(Robot *r){
    robot_ = r;
}

void GlutClass::render(){
    if(robot_->isRunning() == false){
        exit(0);
    }

    int mapWidth = grid_->getMapWidth();
    float scale = 1; //grid_->getMapScale();

    RobotPose current_pose; 

    current_pose = robot_->getRobotsPose();

    double x_robot = current_pose.robot_map_x * scale; 
    double y_robot = current_pose.robot_map_y * scale; 
    double ang_robot = current_pose.robot_yaw; 

    double x_center, y_center; 
    x_center = x_robot; 
    y_center = y_robot; 

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho((int)(x_center) + x_aux - halfWindowSize, 
            (int)(x_center) + x_aux + halfWindowSize - 1, 
            (int)(y_center) - y_aux - halfWindowSize, 
            (int)(y_center) - y_aux + halfWindowSize - 1, 
            -1, 50);
    glMatrixMode(GL_MODELVIEW);
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    int xi, yi, xf, yf;
    int x = x_center + mapWidth/2 - 1; 
    int y = mapWidth/2 - y_center; 

    xi = x + x_aux - halfWindowSize;
    if( xi < 0){
        xi = 0; 
        xf = halfWindowSize * 2 - 1;
    }else{
        xf = x + x_aux + halfWindowSize - 1; 
        if(xf > mapWidth - 1){
            xi = mapWidth - 2 * halfWindowSize;
            xf = mapWidth - 1; 
        }
    }

    yi = y + y_aux - halfWindowSize; 
    if(yi < 0){
        yi = 0; 
        yf = halfWindowSize * 2 - 1; 
    }else{
        yf = y + y_aux + halfWindowSize - 1; 
        if(yf > mapWidth - 1){
            yi = mapWidth - 2 * halfWindowSize; 
            yf = mapWidth - 1; 
        }
    }

    grid_->draw(xi, yi, xf, yf);

    robot_->drawRobot(x_robot, y_robot, ang_robot);
    robot_->plan->updateHeatValeuWithinMap();

    glutSwapBuffers();
    glutPostRedisplay();

    usleep(5000);
}

void GlutClass::display(){
    instance_->render();
}

void GlutClass::keyboard(unsigned char key, int x, int y){
    switch(key){
        case 'b':
            instance_->grid_->view_mode++;
            if(instance_->grid_->view_mode == instance_->grid_->num_view_modes)
                instance_->grid_->view_mode = 0;
            break;
        case 'v':
            instance_->grid_->view_mode--;
            if(instance_->grid_->view_mode == -1)
                instance_->grid_->view_mode = instance_->grid_->num_view_modes - 1;            
            break;
        case 's':
            instance_->y_aux += 10;
            break;
        case 'a':
            instance_->x_aux -= 10;
            break;
        case 'd':
            instance_->x_aux += 10;
            break;        
        case 'w':
            instance_->y_aux -= 10;
            break;            
        case '+':
        case '=':
            instance_->halfWindowSize -= 10;
            if(instance_->halfWindowSize < instance_->grid_->getMapScale()) 
                instance_->halfWindowSize = instance_->grid_->getMapScale();
            break;
        case '-':
            instance_->halfWindowSize += 10;
            if((unsigned int)instance_->halfWindowSize > instance_->grid_->getMapWidth()/2)
                instance_->halfWindowSize = instance_->grid_->getMapWidth()/2;
            break;
        default:
            break;
    }
}
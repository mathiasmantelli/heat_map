#include "../include/GlutClass.h"
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

    while(robot_->isReady() == false){
        usleep(100000);
    }
    grid_ = robot_->grid_map;

	int argc=0;char** argv=0;
    glutInit(&argc, argv);
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize (700, 700);
    id_ = glutCreateWindow("Janela");

    glClearColor (1.0, 1.0, 1.0, 0.0);
    glClear (GL_COLOR_BUFFER_BIT);

    glEnable (GL_BLEND); glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glutDisplayFunc(display);    
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
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    pthread_mutex_lock(grid_->grid_mutex);
    grid_->draw(1,0,0,1);
    pthread_mutex_unlock(grid_->grid_mutex);

    glutSwapBuffers();
    glutPostRedisplay();

    usleep(5000);
}

void GlutClass::display(){
    instance_->render();
}
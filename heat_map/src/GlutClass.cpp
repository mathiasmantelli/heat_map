#include "../include/GlutClass.h"

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
    //std::cout << "INITIALIZE" << std::endl;
    halfWindowSize = 50;
    x_aux = 0;
    y_aux = 0;
    glutWindowSize = 700;

    while(robot_->isReady() == false){
        usleep(100000);
    }

	int argc=0;char** argv=0;
    glutInit(&argc, argv);
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize (glutWindowSize,glutWindowSize);

    id_ = glutCreateWindow("Janela");
    lockCameraOnRobot = true;

    glClearColor (1.0, 1.0, 1.0, 0.0);
    glClear (GL_COLOR_BUFFER_BIT);

    glEnable (GL_BLEND); glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glutDisplayFunc(display); 
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKeys);             
}

void GlutClass::process(){
    std::cout << "PROCESS" << std::endl;
}

void GlutClass::setRobot(Robot *r){
    robot_=r;
}

void GlutClass::render(){
    if(robot_->isRunning() == false){
        exit(0);
    }
}

void GlutClass::display(){
    instance_->render();
}

void GlutClass::reshape(int w, int h){
    glViewport (0, 0, (GLsizei) w, (GLsizei) h); 
    glMatrixMode (GL_PROJECTION);
    glLoadIdentity ();
    glOrtho (-100,100,-100,100,0, 50);
    glMatrixMode (GL_MODELVIEW);

    glClearColor(0.8, 0.8, 0.8, 0);
    glClear (GL_COLOR_BUFFER_BIT);    
}

void GlutClass::keyboard(unsigned char key, int x, int y){
    
}

void GlutClass::specialKeys(int key, int x, int y){
    
}
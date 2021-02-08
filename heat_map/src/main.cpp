#include <pthread.h>
#include <iostream>
#include <string.h>

#include "../include/Robot.h"
#include "../include/GlutClass.h"

pthread_mutex_t* mutex;

void* startRobotThread(void* ref){
    Robot* robot = (Robot*) ref; 

     robot->initialize();

    while(robot->isRunning()){
        robot->run();
    } 

    return NULL;
}

void* startGlutThread(void* ref){
    GlutClass* glut = GlutClass::getInstance();

    glut->setRobot((Robot*) ref);
    glut->initialize();
    glut->process();

    return NULL;
}

int main(int argc, char** argv){

    Robot* r; 
    r = new Robot();

    pthread_t robotThread, glutThread; 
    mutex = new pthread_mutex_t;
    pthread_mutex_unlock(mutex);

    pthread_create(&(robotThread),NULL,startRobotThread,(void*)r);
    //pthread_create(&(glutThread),NULL,startGlutThread,(void*)r);

    pthread_join(robotThread, 0);
    //pthread_join(glutThread, 0);

    return 0;    
}

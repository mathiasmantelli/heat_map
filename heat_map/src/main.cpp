#include <pthread.h>
#include <iostream>
#include <string.h>

#include "../include/Robot.h"
#include "../include/GlutClass.h"

pthread_mutex_t* mutex;

void* startRobotThread(void* ref){
    Robot* robot = (Robot*) ref; 
    std::cout << " ROBOT THREAD" << std::endl;
    robot->initialize();

    while(robot->isReady()){
        std::cout << " WHILE ROBOT READY" << std::endl;
        if(robot->isRunning()){
            robot->run();
        }
    } 

    return NULL;
}

void* startGlutThread(void* ref){
    GlutClass* glut = GlutClass::getInstance();
    std::cout << "GLUT THREAD" << std::endl;
    glut->setRobot((Robot*) ref);
    std::cout << "GLUT - ROBOT IS SET" << std::endl;
    glut->initialize();
    std::cout << "GLUT - INITIALIZE" << std::endl;
    glut->process();
    std::cout << "GLUT - PROCESS" << std::endl;

    return NULL;
}

int main(int argc, char** argv){

    Robot* r; 
    r = new Robot();

    r->grid_map->grid_mutex = new pthread_mutex_t;
    if(pthread_mutex_init(r->grid_map->grid_mutex, NULL) != 0){
        std::cout << "MUTEX INIT HAS FAILED" << std::endl;
        return 1;
    }

    pthread_t robotThread, glutThread; 
    mutex = new pthread_mutex_t;
    pthread_mutex_unlock(mutex);

    pthread_create(&(robotThread),NULL,startRobotThread,(void*)r);
    pthread_create(&(glutThread),NULL,startGlutThread,(void*)r);

    pthread_join(robotThread, 0);
    pthread_join(glutThread, 0);

    return 0;    
}

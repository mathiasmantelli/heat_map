#include <pthread.h>
#include <iostream>
#include <string.h>

#include "../include/Robot.h"
#include "../include/GlutClass.h"

LogMode logMode; 
std::string filename;

pthread_mutex_t* mutex;

void* startRobotThread(void* ref){
    Robot* robot = (Robot*) ref; 
    robot->initialize(logMode);

    while(robot->isReady()){
        if(robot->isRunning()){
            robot->run();
        }
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

void* startPlanningThread(void* ref){
    Robot* robot = (Robot*) ref; 
    while(!robot->isReady()){
        usleep(100000);
    }
}

int main(int argc, char** argv){

    logMode = NONE; 
    filename = ""; 

    if(argc > 2){
        if(!strncmp(argv[2], "-R", 2))
            logMode = RECORDING; 
        else if(!strncmp(argv[2], "-r", 2))
            logMode = RECORDING; 
        else if(!strncmp(argv[2], "-P", 2)){
            logMode = PLAYBACK;     
            filename = argv[3];
        }else if(!strncmp(argv[2], "-p", 2)){
            logMode = PLAYBACK;
            filename = argv[3];                     
        }else if(!strncmp(argv[2], "-n", 2))
            logMode = NONE;    
    }

    Robot* r; 
    r = new Robot();

    r->grid_map->grid_mutex = new pthread_mutex_t;
    if(pthread_mutex_init(r->grid_map->grid_mutex, NULL) != 0){
        std::cout << "MUTEX INIT HAS FAILED" << std::endl;
        return 1;
    }

    pthread_t robotThread, glutThread, planningThread; 
    mutex = new pthread_mutex_t;
    pthread_mutex_unlock(mutex);

    pthread_create(&(robotThread),NULL,startRobotThread,(void*)r);
    pthread_create(&(glutThread),NULL,startGlutThread,(void*)r);
    pthread_create(&(planningThread),NULL,startPlanningThread,(void*)r);

    pthread_join(robotThread, 0);
    pthread_join(glutThread, 0);
    pthread_join(planningThread, 0);

    return 0;    
}

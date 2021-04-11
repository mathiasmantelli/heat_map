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
    robot->initialize(logMode, filename);
    
    while(robot->isRunning()){
            std::cout << "ROBOT is running #################" << std::endl;
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

void* startPlanningThread(void* ref){
    Robot* robot=(Robot*) ref;
    
    while(!robot->isReady()){
        usleep(100000);
    }

    robot->plan->initialize();
    while(robot->isRunning()){
        robot->plan->run();
        std::cout << "PLANNING is running @@@@@@@@@@@@@@@@@@" << std::endl;
        usleep(100000);
    }
    return NULL;
}

int main(int argc, char** argv){

    logMode = NONE; 
    filename = ""; // ../../../list_objects.txt
    
    if(argc > 1){
        if(!strncmp(argv[1], "-R", 2)){
            logMode = RECORDING; 
            std::cout << argc << " - " << argv[1] << std::endl;
        }else if(!strncmp(argv[1], "-r", 2)){
            logMode = RECORDING; 
            std::cout << argc << " - " << argv[1] << std::endl;
        }else if(!strncmp(argv[1], "-Q", 2)){
            logMode = QUERYING;     
            filename = argv[2];
            std::cout << argc << " - " << argv[1] << " and " << argv[2]<< std::endl;
        }else if(!strncmp(argv[1], "-q", 2)){
            logMode = QUERYING;
            filename = argv[2];                     
            std::cout << argc << " - " << argv[1] << " and " << argv[2]<< std::endl;
        }else if(!strncmp(argv[1], "-n", 2))
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

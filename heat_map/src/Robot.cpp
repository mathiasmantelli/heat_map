#include "../include/Robot.h"


Robot::Robot(){
    ready_ = false;
    running_ = false;

}

Robot::~Robot(){
    
}

void Robot::initialize(){
    ready_ = true;
    bool success = robotRos.initialize();
    if(success){
        std::cout << "ROBOT ROS INITIALIZED" << std::endl;
        running_ = true;
    }else{
        std::cout << "ROBOT ROS IS NOT INITIALIZED YET" << std::endl;
    }
}

void Robot::run(){
    robotRos.justPrint();
    
    robotRos.resumeMovement();
    usleep(50000);
}

bool Robot::isReady()
{
    return ready_;
}

bool Robot::isRunning()
{
    return running_;
}
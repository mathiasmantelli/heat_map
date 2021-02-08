#include "../include/Robot.h"


Robot::Robot(){
    ready_ = false;
    running_ = true;

}

Robot::~Robot(){
    
}

void Robot::initialize(){
    ready_ = true;
    bool success = robotRos.initialize();
    if(success)
        std::cout << "ROBOT ROS INITIALIZED" << std::endl;
}

void Robot::run(){
    //robotRos.justPrint();
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
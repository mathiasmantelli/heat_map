#include "../include/Robot.h"


Robot::Robot(){
    ready_ = false;
    running_ = true;

    grid_map = new Grid(); 
    robotRos.setGrid(grid_map);

}
Robot::~Robot(){
    if(grid_map!=NULL)
        delete grid_map;    
}

void Robot::initialize(){
    ready_ = true;
    bool success = robotRos.initialize();
}

void Robot::run(){
    pthread_mutex_lock(grid_map->grid_mutex);
    
    robotRos.combineAllInformation();
    robotRos.justPrint();
    //robotRos.saveOccupancyGrid("test");
    robot_pose_ = robotRos.getRobotsPose();
    
    pthread_mutex_unlock(grid_map->grid_mutex);

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

RobotPose Robot::getRobotsPose(){
    return robot_pose_;
}
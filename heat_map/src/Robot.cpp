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
    robotRos.justPrint();
    robotRos.resumeMovement();
    if(robotRos.getImageIsConverted()){    
        cv::Mat my_img = robotRos.getRGBImageOpencv();
        cv::imshow("TESTING", my_img);
        cv::waitKey(30);
    }

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
#include "../include/GlutClass.h"
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
    //std::cout << "INITIALIZE" << std::endl;
    
    cv::Mat image = imread("map_of_objects.pgm", cv::IMREAD_GRAYSCALE);
//    robot_pose_ = robot_->getRobotsPose();
//    image.at<float>(robot_pose_.robot_map_y, robot_pose_.robot_map_x) = 255;

//    std::cout << "ROBOT: mx:" << robot_pose_.robot_map_x << " my:" << robot_pose_.robot_map_y << " - ox:" << robot_pose_.robot_odom_x << " oy:" << robot_pose_.robot_odom_y << " - yaw:" << robot_pose_.robot_yaw << std::endl;
    cv::imshow("Image", image);
    cv::waitKey(500);                 
}

void GlutClass::setRobot(Robot *r){
    robot_=r;
}

#ifndef ROBOT_H
#define ROBOT_H

#include <iostream>
#include <sys/time.h>
#include <unistd.h>
#include <opencv2/core/core.hpp>

#include "Robot_ROS.h"

class Robot{

public:
    Robot();
    ~Robot();

    void initialize();
    void run();    

    bool isReady();
    bool isRunning();
    Robot_ROS::RobotPose getRobotsPose();

protected:
    bool ready_;
    bool running_;   
    Robot_ROS::RobotPose robot_pose_;

    darknet_ros_msgs::BoundingBoxes darknet_objects_;

    sensor_msgs::Image rgb_image_, rgbd_image_, rgb_darknet_image_;
    sensor_msgs::PointCloud2 point_cloud_;
    darknet_ros_msgs::ObjectCount n_boxes_;


    Robot_ROS robotRos; 
};

#endif // ROBOT_H
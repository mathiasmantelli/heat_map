#ifndef ROBOT_H
#define ROBOT_H

#include <iostream>
#include <sys/time.h>
#include <unistd.h>
#include <opencv2/core/core.hpp>

#include "Robot_ROS.h"
#include "Grid.h"
#include "nav_msgs/OccupancyGrid.h"
#include "Planning.h"

enum LogMode {NONE, RECORDING, PLAYBACK};

class Robot{

public:
    Robot();
    ~Robot();

    void initialize(LogMode logMode);
    void run();    

    bool isReady();
    bool isRunning();
    RobotPose getRobotsPose();

    void drawRobot(const float robot_x, const float robot_y, const float robot_yaw);

    Grid* grid_map;
    Planning* plan;
    std::vector<Object*> current_object_list;
    std::vector<Object*> all_objects_list;

protected:
    bool ready_;
    bool running_;   
    RobotPose robot_pose_;

    int windowSize_;

    darknet_ros_msgs::BoundingBoxes darknet_objects_;

    sensor_msgs::Image rgb_image_, rgbd_image_, rgb_darknet_image_;
    sensor_msgs::PointCloud2 point_cloud_;
    darknet_ros_msgs::ObjectCount n_boxes_;
    nav_msgs::OccupancyGrid grid_map_;

    Robot_ROS robotRos; 
};

#endif // ROBOT_H
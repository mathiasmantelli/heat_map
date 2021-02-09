#ifndef ROBOT_ROS_H
#define ROBOT_ROS_H

#include <math.h>
#include <ros/ros.h>
#include <ros/package.h>

#include "tf/tf.h"
#include "tf/transform_listener.h"
#include "tf2_ros/transform_listener.h"
#include "darknet_ros_msgs/BoundingBox.h"
#include "darknet_ros_msgs/BoundingBoxes.h"
#include "darknet_ros_msgs/CheckForObjectsAction.h"
#include "darknet_ros_msgs/ObjectCount.h"
#include "std_msgs/Int8.h"
#include "std_msgs/String.h"
#include "image_transport/image_transport.h"
#include "cv_bridge/cv_bridge.h"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <sensor_msgs/Image.h>
#include <sensor_msgs/PointCloud2.h>
#include <nav_msgs/OccupancyGrid.h>
#include <geometry_msgs/Transform.h>

class Robot_ROS{

public:
    Robot_ROS();
    bool initialize();
    void justPrint();
    void resumeMovement();

    sensor_msgs::Image getRGBImage();
    cv::Mat getRGBImageOpencv();
    sensor_msgs::Image getRGBDImage();
    sensor_msgs::Image getRGBDarnetImage();
    sensor_msgs::PointCloud getPointCloud();
    darknet_ros_msgs::BoundingBoxes getDarknetObjects();
    darknet_ros_msgs::ObjectCount getObjectCount();
    bool getImageIsConverted();
    void objectsWithinMap();

private:
    ros::NodeHandle* node_;
    ros::Rate* rate_;

    ros::Subscriber sub_tf_, sub_map_, sub_rgb_image_, sub_rgbd_image_, sub_point_cloud_, sub_rgb_darknet_image_, sub_bounding_boxes_, sub_objects_bounding_boxes_;

    tf2_ros::TransformListener* listener_tf2;
    tf2_ros::Buffer* tf_buffer_;     

    nav_msgs::OccupancyGrid mapROS_;
    geometry_msgs::Pose husky_pose_;

    darknet_ros_msgs::BoundingBoxes darknet_objects_;

    sensor_msgs::Image rgb_image_, rgbd_image_, rgb_darknet_image_;
    sensor_msgs::PointCloud point_cloud_;
    darknet_ros_msgs::ObjectCount n_boxes_;

    cv::Mat bridged_image_;
    bool image_is_converted_, point_cloud_read_, robot_pose_, grid_map_;
    int pose_map_x_, pose_map_y_;
    double roll_, pitch_, yaw_;

    void receiveMap(const nav_msgs::OccupancyGrid::ConstPtr &value);
    void receiveTf(const tf::tfMessage::ConstPtr &value);
    void receiveRGBImage(const sensor_msgs::ImageConstPtr &value);
    void receiveRGBDImage(const sensor_msgs::Image &value);
    void receivePointCloud(const sensor_msgs::PointCloud &value);
    void receiveRGBDarknetImage(const sensor_msgs::Image &value);
    void receiveBoundingBoxes(const darknet_ros_msgs::ObjectCount::ConstPtr &value);
    void receiveObjectsBoundingBoxes(const darknet_ros_msgs::BoundingBoxes::ConstPtr &value);
};

#endif // ROBOT_ROS_H
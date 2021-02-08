#include "../include/Robot_ROS.h"

Robot_ROS::Robot_ROS(){
    int argc = 0; 
    char** argv = NULL; 

    ros::init(argc, argv, "heat_map_info");

    node_ = new ros::NodeHandle("~");
    rate_ = new ros::Rate(30);

    tf_buffer_ = new tf2_ros::Buffer;
    listener_tf2 = new tf2_ros::TransformListener(*tf_buffer_);

    sub_map_ = node_->subscribe("/map", 10, &Robot_ROS::receiveMap, this);
    sub_tf_ = node_->subscribe("/tf", 1, &Robot_ROS::receiveTf, this);
    sub_rgb_image_ = node_->subscribe("/realsense/color/image_raw", 10,&Robot_ROS::receiveRGBImage, this);
    sub_rgbd_image_ = node_->subscribe("/realsense/depth/image_rect_raw", 10,&Robot_ROS::receiveRGBDImage, this);
    sub_point_cloud_ = node_->subscribe("/realsense/depth/color/points", 10,&Robot_ROS::receivePointCloud, this);
    sub_rgb_darknet_image_ = node_->subscribe("/darknet_ros/detection_image", 1,&Robot_ROS::receiveRGBDarknetImage, this);
    sub_objects_bounding_boxes_ = node_->subscribe("/darknet_ros/found_object", 1, &Robot_ROS::receiveBoundingBoxes, this);
    sub_bounding_boxes_ = node_->subscribe("/darknet_ros/bounding_boxes", 1, &Robot_ROS::receiveObjectsBoundingBoxes, this);

    pose_map_x_ = 0;
    pose_map_y_ = 0;

    roll_ = 0;
    pitch_ = 0;
    yaw_ = 0;
}

bool Robot_ROS::initialize(){
    return true;
}

//#########################################
//          SUBSCRIBE FUNCTIONS
//#########################################
void Robot_ROS::receiveMap(const nav_msgs::OccupancyGrid::ConstPtr &value){
    mapROS_.header = value->header;
    mapROS_.info = value->info;
    mapROS_.data = value->data;
}

void Robot_ROS::receiveTf(const tf::tfMessage::ConstPtr &value){

    try{
        auto transform = tf_buffer_->lookupTransform("map", "base_link", ros::Time(0), ros::Duration(1.0)); 

        husky_pose_.position.x = transform.transform.translation.x;
        husky_pose_.position.y = transform.transform.translation.y;
        husky_pose_.position.z = transform.transform.translation.z;

        husky_pose_.orientation.x = transform.transform.rotation.x;
        husky_pose_.orientation.y = transform.transform.rotation.y;
        husky_pose_.orientation.z = transform.transform.rotation.z;
        husky_pose_.orientation.w = transform.transform.rotation.w;                        

        tf::Quaternion my_quat(transform.transform.rotation.x,
                                transform.transform.rotation.y,
                                transform.transform.rotation.z,
                                transform.transform.rotation.w);

        tf::Matrix3x3 my_mat(my_quat);
        my_mat.getRPY(roll_, pitch_, yaw_);

        pose_map_x_ = transform.transform.translation.x / mapROS_.info.resolution - mapROS_.info.origin.position.x / mapROS_.info.resolution;
        pose_map_y_ = transform.transform.translation.y / mapROS_.info.resolution - mapROS_.info.origin.position.y / mapROS_.info.resolution;
    }catch(tf2::TransformException &ex){
        ROS_WARN("THE TRANSFORMATION HAS FAILED");
        ros::Duration(0.5).sleep();
    }
}

void Robot_ROS::receiveRGBImage(const sensor_msgs::Image &value){
    rgb_image_ = value;
}

void Robot_ROS::receiveRGBDImage(const sensor_msgs::Image &value){
    rgbd_image_ = value;
}

void Robot_ROS::receivePointCloud(const sensor_msgs::PointCloud2 &value){
    point_cloud_ = value;
}

void Robot_ROS::receiveRGBDarknetImage(const sensor_msgs::Image &value){
    rgb_darknet_image_ = value;
}

void Robot_ROS::receiveBoundingBoxes(const darknet_ros_msgs::ObjectCount::ConstPtr &value){
    n_boxes_.count = value->count;
    n_boxes_.header = value->header;
}

void Robot_ROS::receiveObjectsBoundingBoxes(const darknet_ros_msgs::BoundingBoxes::ConstPtr &value){
    darknet_objects_.header = value->header;
    darknet_objects_.image_header = value->image_header;
    darknet_objects_.bounding_boxes = value->bounding_boxes;
} 

//#########################################
//              GET FUNCTIONS
//#########################################
sensor_msgs::Image Robot_ROS::getRGBImage(){
    return rgb_image_;
}

sensor_msgs::Image Robot_ROS::getRGBDImage(){
    return rgbd_image_;
}

sensor_msgs::Image Robot_ROS::getRGBDarnetImage(){
    return rgb_darknet_image_;
}

sensor_msgs::PointCloud2 Robot_ROS::getPointCloud(){
    return point_cloud_;
}

darknet_ros_msgs::BoundingBoxes Robot_ROS::getDarknetObjects(){
    return darknet_objects_;
}

darknet_ros_msgs::ObjectCount Robot_ROS::getObjectCount(){
    return n_boxes_;
}
 
//#########################################
//              GET FUNCTIONS
//#########################################
void Robot_ROS::justPrint(){
    std::cout << "Just print - - " << std::endl;
    std::cout << mapROS_.header.seq << std::endl;
    std::cout << "Robot's pose: [ " << husky_pose_.position.x << ", " << husky_pose_.position.y << "]" << std::endl;
    std::cout << "On the map: [ " << pose_map_x_ << ", " << pose_map_y_ << "]" << std::endl;    
    std::cout << "RGB Image: " << rgb_image_.header.seq << std::endl;
    std::cout << "RGBD Image: " << rgbd_image_.header.seq << std::endl;
    std::cout << "Point Cloud: " << point_cloud_.header.seq << std::endl;
    std::cout << "RGB Darknet Image: " << rgb_darknet_image_.header.seq << std::endl;
    std::cout << "Amount of boxes: " << (int)n_boxes_.count << std::endl;
    std::cout << "Size of vector of objects: " << darknet_objects_.bounding_boxes.size() << std::endl;
}

void Robot_ROS::resumeMovement(){
    ros::spinOnce();
    rate_->sleep();
}
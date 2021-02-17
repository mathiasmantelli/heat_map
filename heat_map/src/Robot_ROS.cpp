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
    pub_map_output_ = node_->advertise<nav_msgs::OccupancyGrid>("/map_output", 1);

    pose_map_x_ = 0;
    pose_map_y_ = 0;

    roll_ = 0;
    pitch_ = 0;
    yaw_ = 0;

    image_is_converted_ = false;
    point_cloud_read_ = false;
    robot_pose_ = false;
    grid_map_ = false;
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
    map_output_ = mapROS_;
    grid_map_ = true;
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

/*         if(yaw_ > M_PI) //(-M_PI, M_PI]
        yaw_ -= 2 * M_PI;
        else if(yaw_ <= -M_PI)
        yaw_ += 2 * M_PI; */
        std::cout << "============================= YAW: " << yaw_ << std::endl;
        if(yaw_ < 0)
            yaw_ += 2 * M_PI;

        std::cout << "============================= YAW: " << yaw_ << std::endl;
        yaw_ = abs(yaw_);

        pose_map_x_ = transform.transform.translation.x / mapROS_.info.resolution - mapROS_.info.origin.position.x / mapROS_.info.resolution;
        pose_map_y_ = transform.transform.translation.y / mapROS_.info.resolution - mapROS_.info.origin.position.y / mapROS_.info.resolution;
        plotSquareWithinMap(pose_map_x_, pose_map_y_);
        robot_pose_ = true;
    }catch(tf2::TransformException &ex){
        ROS_WARN("THE TRANSFORMATION HAS FAILED");
        ros::Duration(0.5).sleep();
    }
}

void Robot_ROS::receiveRGBImage(const sensor_msgs::ImageConstPtr &value){
    rgb_image_.data = value->data;
    rgb_image_.header = value->header;
    rgb_image_.height = value->height;
    rgb_image_.width = value->width;
    rgb_image_.encoding = value->encoding;
    rgb_image_.step = value->step;
    rgb_image_.is_bigendian = value->is_bigendian;
}

void Robot_ROS::receiveRGBDImage(const sensor_msgs::Image &value){
    rgbd_image_ = value;
    cv_bridge::CvImagePtr cv_ptr;
    try{
        cv_ptr = cv_bridge::toCvCopy(value, sensor_msgs::image_encodings::TYPE_32FC1);
    }catch(cv_bridge::Exception& e){
        ROS_ERROR("CV BRIDGE IS NOT WORKING");
    }
    //std::cout << " --------------------- NEW IMAGE SIZE: " << cv_ptr->image.rows << ", " << cv_ptr->image.cols << std::endl;
    cv_ptr->image.copyTo(bridged_image_);
    image_is_converted_ = true; 
    if(darknet_objects_.bounding_boxes.empty())
        image_is_converted_ = false;
}

void Robot_ROS::receivePointCloud(const sensor_msgs::PointCloud &value){
    point_cloud_ = value;
    point_cloud_read_ = true;
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

cv::Mat Robot_ROS::getRGBImageOpencv(){
    return bridged_image_;
}

sensor_msgs::Image Robot_ROS::getRGBDImage(){
    return rgbd_image_;
}

sensor_msgs::Image Robot_ROS::getRGBDarnetImage(){
    return rgb_darknet_image_;
}

sensor_msgs::PointCloud Robot_ROS::getPointCloud(){
    return point_cloud_;
}

darknet_ros_msgs::BoundingBoxes Robot_ROS::getDarknetObjects(){
    return darknet_objects_;
}

darknet_ros_msgs::ObjectCount Robot_ROS::getObjectCount(){
    return n_boxes_;
}

bool Robot_ROS::getImageIsConverted(){
    return image_is_converted_;
}
 
//#########################################
//              OTHER FUNCTIONS
//#########################################
void Robot_ROS::justPrint(){
    std::cout << "Just print - - " << std::endl;
    std::cout << mapROS_.header.seq << std::endl;
    std::cout << "Robot's pose: [ " << husky_pose_.position.x << ", " << husky_pose_.position.y << "]" << std::endl;
    std::cout << "On the map: [ " << pose_map_x_ << ", " << pose_map_y_ << "]" << std::endl;    
/*     std::cout << "RGB Image: " << rgb_image_.header.seq << " | Height: " << rgb_image_.height << " | Width: " << rgb_image_.width << std::endl;
    std::cout << "RGBD Image: " << rgbd_image_.header.seq << " | Height: " << rgbd_image_.height << " | Width: " << rgbd_image_.width << std::endl;
    std::cout << "Point Cloud: " << point_cloud_.header.seq << std::endl; */
    std::cout << "RGB Darknet Image: " << rgb_darknet_image_.header.seq << " | Height: " << rgb_darknet_image_.height << " | Width: " << rgb_darknet_image_.width << std::endl;
    std::cout << "Amount of boxes: " << (int)n_boxes_.count << std::endl;
    std::cout << "Size of vector of objects: " << darknet_objects_.bounding_boxes.size() << std::endl;
    for(int i = 0 ; i < darknet_objects_.bounding_boxes.size(); i++){
        int xcenter, ycenter; 
        xcenter = ((darknet_objects_.bounding_boxes[i].xmax - darknet_objects_.bounding_boxes[i].xmin)/2 + darknet_objects_.bounding_boxes[i].xmin);
        ycenter = ((darknet_objects_.bounding_boxes[i].ymax - darknet_objects_.bounding_boxes[i].ymin)/2 + darknet_objects_.bounding_boxes[i].ymin);

        std::cout << i << " - " << darknet_objects_.bounding_boxes[i].Class.c_str()
        << " | " << darknet_objects_.bounding_boxes[i].probability
        << " | " << darknet_objects_.bounding_boxes[i].xmin
        << " | " << darknet_objects_.bounding_boxes[i].xmax
        << " | " << darknet_objects_.bounding_boxes[i].ymin
        << " | " << darknet_objects_.bounding_boxes[i].ymax << std::endl << std::endl;

/*         std::cout << "RGB IMAGE:" << std::endl;
        std::cout << "xmin, ymin: " << (int)rgb_image_.data[darknet_objects_.bounding_boxes[i].xmin * rgb_image_.step + darknet_objects_.bounding_boxes[i].ymin]  << std::endl;
        std::cout << "xmin, ymin: " << (int)rgb_image_.data[darknet_objects_.bounding_boxes[i].xmax * rgb_image_.step + darknet_objects_.bounding_boxes[i].ymax]  << std::endl;      

        std::cout << "RGBD IMAGE:" << std::endl;
        std::cout << "xmin, ymin: " << (int)rgbd_image_.data[darknet_objects_.bounding_boxes[i].xmin * rgbd_image_.step + darknet_objects_.bounding_boxes[i].ymin]  << std::endl;
        std::cout << "xmin, ymin: " << (int)rgbd_image_.data[darknet_objects_.bounding_boxes[i].xmax * rgbd_image_.step + darknet_objects_.bounding_boxes[i].ymax]  << std::endl;
 */
        if(image_is_converted_ && robot_pose_ && grid_map_ && darknet_objects_.bounding_boxes[i].Class == "Book"){
            float distance = bridged_image_.at<float>(xcenter, ycenter); 
            std::cout << "OPENCV RGBD IMAGE:" << std::endl;
            std::cout << "DISTANCE:" << distance << std::endl;
            int x = husky_pose_.position.x + distance * cos(yaw_);
            int y = husky_pose_.position.y + distance * sin(yaw_);

            x = x / mapROS_.info.resolution - mapROS_.info.origin.position.x / mapROS_.info.resolution;
            y = y / mapROS_.info.resolution - mapROS_.info.origin.position.y / mapROS_.info.resolution;
            std::cout << "OBJECT WITHIN THE MAP: " << x << ", " << y << std::endl;
            plotSquareWithinMap(x, y);
        }

    }
    pub_map_output_.publish(map_output_);
}

void Robot_ROS::plotSquareWithinMap(int x, int y){
    int size = 1; 
    for(int l = y - size; l <= y + size; ++l){
        for(int k = x - size; k <= x + size; ++k){
            map_output_.data[k + l * map_output_.info.width] = 100;
        }
    }
}

float Robot_ROS::computeDistanceFromRobot2Object(int xmin, int xmax, int ymin, int ymax){
    float sum_distances = 0;
    int cont = 0; 
    for(int i = xmin; i <= xmax; i++){
        for(int j = ymin; j <= ymax; j++){
            sum_distances += bridged_image_.at<float>(i, j);
            cont++;
        }
    }
    return sum_distances/cont;
}

void Robot_ROS::resumeMovement(){
    ros::spinOnce();
    rate_->sleep();
}

void Robot_ROS::objectsWithinMap(){
    int x_min, y_min, x_max, y_max;
    for(int i = 0 ; i < darknet_objects_.bounding_boxes.size(); i++){
        x_min = (int)darknet_objects_.bounding_boxes[i].xmin;
        x_max = (int)darknet_objects_.bounding_boxes[i].xmax;
        y_min = (int)darknet_objects_.bounding_boxes[i].ymin;
        y_max = (int)darknet_objects_.bounding_boxes[i].ymax;
    }
}
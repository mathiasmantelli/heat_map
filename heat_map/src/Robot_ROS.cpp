#include "../include/Robot_ROS.h"
#include <ctime>
#include <memory>

Robot_ROS::Robot_ROS(){
    int argc = 0; 
    char** argv = NULL; 

    ros::init(argc, argv, "heat_map_info");

    node_ = new ros::NodeHandle("~");
    rate_ = new ros::Rate(30);

    tf_buffer_ = new tf2_ros::Buffer;
    listener_tf2 = new tf2_ros::TransformListener(*tf_buffer_);

    sub_map_ = node_->subscribe("/map", 10, &Robot_ROS::receiveMap, this);
    sub_tf_ = node_->subscribe("/tf", 10, &Robot_ROS::receiveTf, this);
    sub_rgb_image_ = node_->subscribe("/realsense/color/image_raw", 10,&Robot_ROS::receiveRGBImage, this);
    sub_rgbd_image_ = node_->subscribe("/realsense/depth/image_rect_raw", 10,&Robot_ROS::receiveRGBDImage, this);
    sub_point_cloud_ = node_->subscribe("/realsense/depth/color/points", 10,&Robot_ROS::receivePointCloud, this);
    sub_rgb_darknet_image_ = node_->subscribe("/darknet_ros/detection_image", 10,&Robot_ROS::receiveRGBDarknetImage, this);
    sub_objects_bounding_boxes_ = node_->subscribe("/darknet_ros/found_object", 10, &Robot_ROS::receiveBoundingBoxes, this);
    sub_bounding_boxes_ = node_->subscribe("/darknet_ros/bounding_boxes", 10, &Robot_ROS::receiveObjectsBoundingBoxes, this);
    pub_map_output_ = node_->advertise<nav_msgs::OccupancyGrid>("/heatmap/map_robo_path", 1);
    pub_obj_map_ = node_->advertise<nav_msgs::OccupancyGrid>("/heatmap/obj_map", 1);

    pose_map_x_ = 0;
    pose_map_y_ = 0;

    roll_ = 0;
    pitch_ = 0;
    yaw_ = 0;

    image_is_converted_ = false;
    point_cloud_read_ = false;
    robot_pose_ = false;
    grid_map_ = false;
    darknet_bounding_box_ = false;
    map_published_ = false;

    current_time_ = std::time(nullptr);
    calendar_time_ = *std::localtime(std::addressof(current_time_));

    amount_yaw_saved_ = 80;

    current_pose_robot_.robot_map_x = 0;
    current_pose_robot_.robot_map_y = 0;
    current_pose_robot_.robot_odom_x = 0;
    current_pose_robot_.robot_odom_y = 0;
    current_pose_robot_.robot_yaw = 0;

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
    map_objects_ = mapROS_;

    grid_->map_limits.min_x = grid_->map_limits.min_y = 1000000;
    grid_->map_limits.max_x = grid_->map_limits.max_y = -1000000;

    for(int j = 0; j < mapROS_.info.height; ++j){
        for(int i = 0; i < mapROS_.info.width; ++i){
            if(mapROS_.data[i + j * mapROS_.info.width] > -1){
                if(grid_->map_limits.min_x > i)
                    grid_->map_limits.min_x = i;
                if(grid_->map_limits.min_y > j)
                    grid_->map_limits.min_y = j;
                if(grid_->map_limits.max_x < i)
                    grid_->map_limits.max_x = i;
                if(grid_->map_limits.max_y < j)
                    grid_->map_limits.max_y = j;                    
            }       
        }
    }

    for(int j = grid_->map_limits.min_y; j <= grid_->map_limits.max_y; j++){
        for(int i = grid_->map_limits.min_x; i <= grid_->map_limits.max_x; i++){
            Cell *c = grid_->getCell(i-1500, j-1500);
            c->value = mapROS_.data[i + j * mapROS_.info.width];
        }
    }

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

        all_robot_poses_.push_back(husky_pose_);

        tf::Quaternion my_quat(transform.transform.rotation.x,
                                transform.transform.rotation.y,
                                transform.transform.rotation.z,
                                transform.transform.rotation.w);

        tf::Matrix3x3 my_mat(my_quat);
        my_mat.getRPY(roll_, pitch_, yaw_);

        if(yaw_ < 0)
            yaw_ += 2 * M_PI;

        yaw_ = abs(yaw_);

        pose_map_x_ = transform.transform.translation.x / mapROS_.info.resolution - mapROS_.info.origin.position.x / mapROS_.info.resolution;
        pose_map_y_ = transform.transform.translation.y / mapROS_.info.resolution - mapROS_.info.origin.position.y / mapROS_.info.resolution;

        robot_pose_ = true;

        if(past_robots_yaw_.size() == amount_yaw_saved_)
            past_robots_yaw_.pop_back();
        past_robots_yaw_.emplace(past_robots_yaw_.begin(), yaw_); 
        if(computeStandardDeviation(past_robots_yaw_) == 0)
            current_robots_mode_ = IDLE;
        else
            current_robots_mode_ = MOVING;

        current_pose_robot_.robot_map_x = pose_map_x_;
        current_pose_robot_.robot_map_y = pose_map_y_;
        current_pose_robot_.robot_odom_x = transform.transform.translation.x;
        current_pose_robot_.robot_odom_y = transform.transform.translation.y;
        current_pose_robot_.robot_yaw = yaw_;    

    }catch(tf2::TransformException &ex){
        ROS_WARN("THE TRANSFORMATION HAS FAILED");
        ros::Duration(0.5).sleep();
    }
}

float Robot_ROS::computeStandardDeviation(std::vector<float> past_robots_yaw_){
    float sum = 0, mean, std_dev = 0; 
    for(int i = 0; i < past_robots_yaw_.size(); i++)
        sum += past_robots_yaw_[i] * (180/M_PI);
        
    mean = sum / past_robots_yaw_.size(); 

    for(int i = 0; i < past_robots_yaw_.size(); i++){
        std_dev += pow((past_robots_yaw_[i] * (180/M_PI)) - mean, 2);
    }
    
    std_dev = sqrt(std_dev / past_robots_yaw_.size());
    
    if(std_dev <= 0.01)
        std_dev = 0;

    return std_dev;
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
    darknet_bounding_box_ = true;
} 

//#########################################
//              GET FUNCTIONS
//#########################################

RobotPose Robot_ROS::getRobotsPose(){
    return current_pose_robot_;
}

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

nav_msgs::OccupancyGrid Robot_ROS::getOccupancyGrid(){
    return mapROS_;
}
 
//#########################################
//              OTHER FUNCTIONS
//#########################################
void Robot_ROS::combineAllInformation(){
    if(image_is_converted_ && robot_pose_ && grid_map_ && darknet_bounding_box_){
        for(int i = 0 ; i < darknet_objects_.bounding_boxes.size(); i++){    
            if(checkObjectClass(darknet_objects_.bounding_boxes[i].Class) && current_robots_mode_ == IDLE){
                int xcenter, ycenter; 
                xcenter = ((darknet_objects_.bounding_boxes[i].xmax - darknet_objects_.bounding_boxes[i].xmin)/2 + darknet_objects_.bounding_boxes[i].xmin);
                ycenter = ((darknet_objects_.bounding_boxes[i].ymax - darknet_objects_.bounding_boxes[i].ymin)/2 + darknet_objects_.bounding_boxes[i].ymin);        
                float distance = bridged_image_.at<float>(xcenter, ycenter); 
                int obj_odom_x = husky_pose_.position.x + distance * cos(yaw_);
                int obj_odom_y = husky_pose_.position.y + distance * sin(yaw_);

                ObjectInfo current_object; 
                current_object.robot_map_x = husky_pose_.position.x; 
                current_object.robot_map_y = husky_pose_.position.y; 
                current_object.obj_map_x = obj_odom_x; 
                current_object.obj_map_y = obj_odom_y; 
                current_object.obj_class = darknet_objects_.bounding_boxes[i].Class;
                current_object.hours_detection = calendar_time_.tm_hour;  
                std::cout << "OBJECT INCLUDED: " << darknet_objects_.bounding_boxes[i].Class << " | Distance: " << distance << std::endl;
                objects_list_.push_back(current_object);
            }
        }
        darknet_objects_.bounding_boxes.clear();
    }
    pub_map_output_.publish(map_output_);
    pub_obj_map_.publish(map_objects_);
    map_published_ = true;
}

bool Robot_ROS::checkObjectClass(std::string objects_class){
    if(objects_class == "mug" || objects_class == "cup" || objects_class == "monitor" || objects_class == "book" || objects_class == "Book" || objects_class == "Mug" || objects_class == "Mobile phone" || objects_class == "Computer mouse" || objects_class == "Pen" || objects_class == "Computer monitor")
        return true;
    else
        return false;
}
void Robot_ROS::justPrint(){
    for(int i = 0; i < all_robot_poses_.size(); i++){
        int pose_x = all_robot_poses_[i].position.x / mapROS_.info.resolution - mapROS_.info.origin.position.x / mapROS_.info.resolution;
        int pose_y = all_robot_poses_[i].position.y / mapROS_.info.resolution - mapROS_.info.origin.position.y / mapROS_.info.resolution;  
        if(pose_x >= 0 && pose_x <= mapROS_.info.width && pose_y >= 0 && pose_y <= mapROS_.info.height)      
            plotCircleWithinMap(pose_x, pose_y, 1);
    }
    for(int i = 0; i < objects_list_.size(); i++){
        int robot_x = objects_list_[i].robot_map_x / mapROS_.info.resolution - mapROS_.info.origin.position.x / mapROS_.info.resolution;
        int robot_y = objects_list_[i].robot_map_y / mapROS_.info.resolution - mapROS_.info.origin.position.y / mapROS_.info.resolution;

        int obj_x = objects_list_[i].obj_map_x / mapROS_.info.resolution - mapROS_.info.origin.position.x / mapROS_.info.resolution;
        int obj_y = objects_list_[i].obj_map_y / mapROS_.info.resolution - mapROS_.info.origin.position.y / mapROS_.info.resolution;

        if(robot_x >= 0 && robot_x <= mapROS_.info.width && robot_y >= 0 && robot_y <= mapROS_.info.height)      
            plotCircleWithinMap(robot_x, robot_y, 2);
        if(obj_x >= 0 && obj_x <= mapROS_.info.width && obj_y >= 0 && obj_y <= mapROS_.info.height)      
            plotSquareWithinMap(obj_x, obj_y, 2);
    }
}

void Robot_ROS::plotSquareWithinMap(int x, int y, int which_map){
    int size = 3; 
    for(int l = y - size; l <= y + size; ++l){
        for(int k = x - size; k <= x + size; ++k){
            if(which_map == 1)
                map_output_.data[k + l * map_output_.info.width] = 100;
            else
                map_objects_.data[k + l * map_output_.info.width] = 100;            
        }
    }
}

void Robot_ROS::plotCircleWithinMap(int x, int y, int which_map){
    int size = 3; 
    int radius = 2;
    for(int l = y - size; l <= y + size; ++l){
        for(int k = x - size; k <= x + size; ++k){
            if(pow(l - y, 2) + pow(k - x, 2) <= pow(radius, 2)){
                if(which_map == 1)
                    map_output_.data[k + l * map_output_.info.width] = 100;
                else
                    map_objects_.data[k + l * map_output_.info.width] = 100;
            }
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

void Robot_ROS::saveOccupancyGrid(std::string map_name){
//        system("rosrun map_server map_saver -f map_of_objects map:=/map");
}

void Robot_ROS::setGrid(Grid* g){
    grid_ = g;
}
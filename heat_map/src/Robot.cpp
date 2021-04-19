#include "../include/Robot.h"
#include "Misc.h"
#include <GL/gl.h>

Robot::Robot(){
    ready_ = false;
    running_ = true;
    
    grid_map = new Grid(); 
    
    plan = new Planning("cup");
    plan->setGrid(grid_map);

    robotRos.setGrid(grid_map);
}
Robot::~Robot(){
    if(grid_map!=NULL)
        delete grid_map;    
}

void Robot::initialize(LogMode logMode, std::string filename){

    logMode_ = logMode;
    plan->setLogMode(logMode_);
    input_objects_list = filename;
    first_goal_published = false;
    if(logMode == QUERYING)
        plan->objs.readObjectListFromFile(input_objects_list);
    //else{
        bool success = robotRos.initialize();
        if(!success){
            std::cout << "Error while initializing the Robot ROS class!" << std::endl;
            exit(0);
        }
    //}
}

void Robot::run(){
  
    if(logMode_ == RECORDING){
        std::cout << "Map saved" << std::endl;
        robotRos.saveOccupancyGrid();
        robotRos.combineAllInformation();
        current_object_list = robotRos.getObjectList();
        bool obj_update;
        if(!current_object_list.empty())
            obj_update = plan->objs.updateObjects(current_object_list);        
    }else{ //QUERYING or NONE
        robotRos.plotRobotPathOnGrid();   
        if(plan->searchingMode == BRUTE_FORCE){
            if(plan->current_goal.robot_odom_x != -1 && plan->current_goal.robot_odom_y != -1){
                if(robotRos.distanceGoalAndRobotsPosition(plan->current_goal) < 0.3 || !first_goal_published){
                    std::cout << "ROBOT RUN - DISTANCE: " << robotRos.distanceGoalAndRobotsPosition(plan->current_goal) << " - INCREMENTING THE COUNTER." << std::endl;
                    plan->increaseBruteForceGoalCounter();
                    std::cout << "ROBOT RUN - BRUTE FORCE - goal:[" << plan->current_goal.robot_odom_x << ", " << plan->current_goal.robot_odom_y << ", " << plan->current_goal.robot_yaw << "]" << std::endl;
                    robotRos.publishGoalPositionBruteForce(plan->current_goal);   
                    first_goal_published = true;             
                }
            }
        }else 
            robotRos.publishGoalPosition(grid_map->goal_cell);
        //plan computes the position to go based on the query object 
        //robotRos receives the goal pose to navigate the robot towards it
    }
    
    robot_pose_ = robotRos.getRobotsPose();

    robotRos.resumeMovement();
    if(robotRos.getRobotPoseReceived())
        ready_ = true;

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

void Robot::drawRobot(const float robot_x, const float robot_y, const float robot_yaw){
    float scale = 3.8; //grid_map->getMapScale();
    glTranslatef(robot_x, robot_y, 0.0);
    glRotatef(robot_yaw, 0, 0, 1);

    glScalef(1.0/scale, 1.0/scale, 1.0/scale);

    glColor3f(1.0,1.0,0.0);
    glBegin( GL_POLYGON );
    {
        glVertex2f(-20, -8);
        glVertex2f(-13, -15);
        glVertex2f(8, -15);
        glVertex2f(15, -8);
        glVertex2f(15, 8);
        glVertex2f(8, 15);
        glVertex2f(-13, 15);
        glVertex2f(-20, 8);
    }
    glEnd();
    glColor3f(0.0,0.0,0.0);
    glBegin( GL_LINE_STRIP );
    {
        glVertex2f(0, 0);
        glVertex2f(30, 0);
    }
    glEnd();

    glScalef(scale, scale, scale);
    glRotatef(-robot_yaw, 0, 0, 1.0);
    glTranslatef(-robot_x, -robot_y, 0);    

}

float Robot::computePathSize(){
    float total_distance = 0; 
    std::vector<geometry_msgs::Pose> all_poses = robotRos.getRobotsPath();
    for(int i = 1; i < all_poses.size(); i++){
        total_distance += sqrt(pow(all_poses[i].position.x - all_poses[i-1].position.x, 2) + pow(all_poses[i].position.y - all_poses[i-1].position.y, 2));
    }
    return total_distance;
}

float Robot::measureDistanceGoalAndRobotsPosition(){
    return robotRos.distanceGoalAndRobotsPosition(plan->current_goal);
}
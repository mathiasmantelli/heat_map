#include "../include/Robot.h"
#include <GL/gl.h>

Robot::Robot(){
    ready_ = false;
    running_ = true;
    
    grid_map = new Grid(); 
    
    plan = new Planning();
    plan->setGrid(grid_map);

    robotRos.setGrid(grid_map);

}
Robot::~Robot(){
    if(grid_map!=NULL)
        delete grid_map;    
}

void Robot::initialize(LogMode logMode, std::string filename){
    logMode_ = logMode;
    
    input_objects_list = filename;
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
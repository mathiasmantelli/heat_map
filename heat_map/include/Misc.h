#ifndef MISC_H
#define MISC_H

enum LogMode {NONE, RECORDING, QUERYING};

enum RobotMode {IDLE, MOVING};    

enum SearchingMode {SEMANTIC, BRUTE_FORCE};

struct GoalCell{
        int cell_x, cell_y; 
        float yaw;
};

struct RobotPose{
        int robot_map_x, robot_map_y;
        int robot_odom_x, robot_odom_y;
        int robot_yaw;
};

#endif // MISC_H
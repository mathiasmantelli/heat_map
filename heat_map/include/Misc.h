#ifndef MISC_H
#define MISC_H

#include <vector>
#include <string>
#include <iostream>
#include <fstream>

enum LogMode {NONE, RECORDING, QUERYING};

enum RobotMode {IDLE, MOVING};    

enum SearchingMode {SEMANTIC, BRUTE_FORCE};

struct GoalCell{
        int cell_x, cell_y; 
        float yaw;
};

struct RobotPose{
        int robot_map_x, robot_map_y;
        float robot_odom_x, robot_odom_y;
        float robot_yaw;
};

class Misc{
public:
        Misc();
        std::vector<RobotPose> poses_brute_force_search;
        std::ifstream robot_pose_file;
        RobotPose getNextGoal(int index);
};
#endif // MISC_H
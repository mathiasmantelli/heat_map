#include "../include/GlutClass.h"

GlutClass::GlutClass(){
}

GlutClass::~GlutClass(){
}

GlutClass* GlutClass::instance_ = 0; 

GlutClass* GlutClass::getInstance (){
    if(instance_ == 0)
        instance_ = new GlutClass;

    return instance_;
}

void GlutClass::initialize(){
    std::cout << "INITIALIZE" << std::endl;
}

void GlutClass::process(){
    std::cout << "PROCESS" << std::endl;
}

void GlutClass::setRobot(Robot *r){
    robot_=r;
}

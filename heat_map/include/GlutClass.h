#ifndef __GLUTCLASS_H__
#define __GLUTCLASS_H__

#include "Robot.h"

class GlutClass{

public:
    static GlutClass* getInstance();
    void initialize();    
    void process();

    void setRobot(Robot* r);

private:
    GlutClass ();
    ~GlutClass ();
    static GlutClass* instance_;    
    Robot* robot_;
};

#endif /* __GLUT_H__ */
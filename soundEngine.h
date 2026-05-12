//soundEngine.h 
#pragma once
#include <synthizer.h>
#include <synthizer_constants.h>

class CSoundEngine
{
//Constructor
public:
    CSoundEngine();
    ~CSoundEngine();

//Methods
public:
    int initialise();
    bool isOk();
    syz_Handle Context();
    int setPosition(double x, double y, double z);
        int setOrientation(double atX, double atY, double atZ, double upX, double upY, double upZ);
    void destroy();
    void mute();

    //Variables
private:
    syz_Handle m_context;
    bool m_Muted;
};
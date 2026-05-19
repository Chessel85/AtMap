//SoundEngine.cpp
#include "soundEngine.h"
#include <stdio.h>

const double DEFAULT_VOLUME = 1.0;

CSoundEngine::CSoundEngine()
{
    m_Muted = false;
    m_context = 0;
}

CSoundEngine::~CSoundEngine()
{
    destroy();
}

int CSoundEngine::initialise()
{
    //Initialise synthizer 
    if (syz_initialize() != 0)
    {
        printf("Failed to initialize Synthizer.\n");
        return -1;
    }

    //Create context 
    if (syz_createContext(&m_context, nullptr, nullptr) != 0)
    {
        printf("Failed to create context.\n");
        syz_shutdown();
        return -2;
    }

    //set volume to default value 
    if (syz_setD(m_context, SYZ_P_GAIN, DEFAULT_VOLUME) != 0)
    {
        printf("Couldn't turn the volume down.\n");
    }

    return 0;
}

bool CSoundEngine::isOk()
{
    //Fail if no context handle 
    if (m_context == 0)
        return false;

    // syz_handleGetObjectType returns 0 on success
    int type;
    if (syz_handleGetObjectType(&type, m_context) != 0)
        return false;

    if (type != SYZ_OTYPE_CONTEXT)
        return false;

    return true;
}

syz_Handle CSoundEngine::Context()
{
    return m_context;
}

int CSoundEngine::setPosition(double x, double y, double z)
{
    //Check okay first
    if (!isOk())
        return -1;

    //Set position 
    if (syz_setD3(m_context, SYZ_P_POSITION, x, y, z) != 0)
    {
        printf("Failed to set context position.\n");
        return  -1;
    }

    return 0;
}

int CSoundEngine::setOrientation(double atX, double atY, double atZ, double upX, double upY, double upZ)
{
    //Check okay first
    if (!isOk())
        return -1;

    //Set orientation 
    if (syz_setD6(m_context, SYZ_P_ORIENTATION, atX, atY, atZ, upX, upY, upZ) != 0)
    {
        printf("Failed to set orientation.\n");
        return -1;
    }

    return 0;
}

void CSoundEngine::destroy()
{
    if (m_context != 0) 
    {
        syz_handleDecRef(m_context);
        m_context = 0; 
    }
    syz_shutdown();
}

void CSoundEngine::mute()
{
    //Toggle the sound on and off 
    if (m_Muted == false)
    {
        syz_setD(m_context, SYZ_P_GAIN, 0.0);
        m_Muted = true;
    }
    else
    {
        syz_setD(m_context, SYZ_P_GAIN, DEFAULT_VOLUME);
        m_Muted = false;
    }
}
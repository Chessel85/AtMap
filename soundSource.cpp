//soundSource.cpp
#include "soundSource.h"
#include <iostream>

CSoundSource::CSoundSource(syz_Handle context)
    : m_context(context), m_source(0), m_generator(0)
{
    //the context is initialised just above 
    // m_source and m_generator are initialized to 0 
    // The inheriting classes will populate these handles in their own constructors
}

CSoundSource::~CSoundSource()
{
    destroy();
}

bool CSoundSource::isReady() const
{
    //only check the context since the source is checked by the derived class 
    if (m_context == 0)
        return false;

    //get the object type 
    int contextType;
    if (syz_handleGetObjectType(&contextType, m_context) != 0)
        return false;
    if (contextType != SYZ_OTYPE_CONTEXT)
        return false;

    return true;
}

int CSoundSource::setPitch(double pitch)
{
    if (!isReady())
        return -1;

    // Pitch bend is a property of the Generator 
    if (m_generator == 0) 
        return -1;

    if (syz_setD(m_generator, SYZ_P_PITCH_BEND, pitch) != 0)
    {
        return -1;
    }

    return 0;
}

int CSoundSource::setGain(double gain)
{
    // Gain (volume) is usually best set on the Source (the speaker)
    if (!isReady() )
        return -1;

    if (syz_setD(m_source, SYZ_P_GAIN, gain) != 0)
    {
        return -1;
    }

    return 0;
}

int CSoundSource::setLooping(bool looping)
{
    if (m_generator == 0) 
        return -1;

    return syz_setI(m_generator, SYZ_P_LOOPING, looping ? 1 : 0);
}

syz_Handle CSoundSource::getSourceHandle() const
{
    return m_source;
}

syz_Handle CSoundSource::getGeneratorHandle() const
{
    return m_generator;
}

void CSoundSource::releaseHandle(syz_Handle& handle)
{
    if (handle != 0)
    {
        syz_handleDecRef(handle);
        handle = 0;
    }
}

void CSoundSource::destroy()
{
    // generators should be released before sources 
    releaseHandle(m_generator);
    releaseHandle(m_source);
    //context can be set to zero as it is not the owner of the handle
    m_context = 0;
}
//spatialSource.cpp
#include "spatialSource.h"
#include <iostream>

CSpatialSource::CSpatialSource(syz_Handle context)
    : CSoundSource(context)
{
    syz_createSource3D(&m_source, m_context, SYZ_PANNER_STRATEGY_HRTF, 0, 0, 0,  nullptr, nullptr, nullptr);

    //Check successful before setting other parameters
    if (!isReady())
        return;

    //Set the distance model 
    syz_setI(m_source, SYZ_P_DISTANCE_MODEL, SYZ_DISTANCE_MODEL_LINEAR);
}

CSpatialSource::~CSpatialSource()
{
    // Base class handles destroy 
}

bool CSpatialSource::isReady() const 
{
    //Check the context in the base class
    if (!CSoundSource::isReady())
        return false;

    //Check source is initialised 
    if (m_source == 0)
        return false;

    //Check the source type 
    int sourceType;
    if (syz_handleGetObjectType(&sourceType, m_source) != 0)
        return false;

    if (sourceType != SYZ_OTYPE_SOURCE_3D)
        return false;

    return true;
}

int CSpatialSource::attachBuffer(syz_Handle bufferHandle)
{
    //Check this object has a valid context and source
    if (!isReady())
        return -1;

    //Remove any existing generator handle 
    releaseHandle(m_generator);

    // Create a Buffer Generator for the spatial sound
    if (syz_createBufferGenerator(&m_generator, m_context, nullptr, nullptr, nullptr) != 0)
    {
        printf("Failed to create generator buffer.\n");
        return -1;
    }

    //Check generator type
    int generatorType = 0;
    syz_handleGetObjectType(&generatorType, m_generator);


    

    //Check the buffer is a valid buffer handle
    if (bufferHandle == 0)
    {
        printf("Zero buffer handle added to spatial source.\n");
        return -1;
    }

    int type  = 0;
    if (syz_handleGetObjectType(&type, bufferHandle) != 0)
    {
        printf("Failed to get handle type when testing buffer handle.\n");
        return -1;
    }

    if (type != SYZ_OTYPE_BUFFER)
    {
        printf("Buffer handle is not really a buffer handle passed into spatial source.\n");
        return -1;
    }


    //Attach the buffer to the generator 
    if (syz_setO(m_generator, SYZ_P_BUFFER, bufferHandle) != 0)
    {
        printf("Failed to attach buffer to generator.\n");
        return -1;
    }

    // Link the generator to our 3D source
    if (syz_sourceAddGenerator(m_source, m_generator) != 0)
    {
        printf("Failed to attach generator to spatial source.\n");
        return -1;
    }

    return 0;
}

int CSpatialSource::setPosition(double x, double y, double z)
{
    if (!isReady())
        return -1;

    // Use setD3 for 3D properties (Double x 3)
    return syz_setD3(m_source, SYZ_P_POSITION, x, y, z);
}

int CSpatialSource::setReferenceDistance(double distance)
{
    if (!isReady())
        return -1;

    // The distance at which the sound is at full volume
    return syz_setD(m_source, SYZ_P_DISTANCE_REF, distance);
}

int CSpatialSource::setMaxDistance(double distance)
{
    if (!isReady())
        return -1;

    // The distance beyond which the sound is either silent or at minimum volume
    return syz_setD(m_source, SYZ_P_DISTANCE_MAX, distance);
}

int CSpatialSource::playBuffer()
{
    if (m_generator == 0)
        return -1;

    

    if (syz_setD(m_generator, SYZ_P_PLAYBACK_POSITION, 0) != 0)
    {
        printf("CSpatialSource: Unable to set generator back to start.\n");
        return -1;
    }

    if (syz_play(m_generator) != 0)
    {
        printf("CSpatialSource: Unable to play buffer generator.\n");
        return -1;
    }
    return 0;
}
//spatialSource.cpp
#include "spatialSource.h"

CSpatialSource::CSpatialSource(syz_Handle context)
    : CSoundSource(context)
{
    syz_createSource3D(&m_source, m_context, SYZ_PANNER_STRATEGY_HRTF, 0, 0, 0,  nullptr, nullptr, nullptr);
}

CSpatialSource::~CSpatialSource()
{
    // Base class handles destroy 
}

int CSpatialSource::attachBuffer(syz_Handle bufferHandle)
{
    if (m_generator != 0)
    {
        syz_handleDecRef(m_generator);
        m_generator = 0;
    }

    // Create a Buffer Generator for the spatial sound
    int result = syz_createBufferGenerator(&m_generator, m_context, nullptr, nullptr, nullptr);
    if (result != 0) return result;

    syz_setI(m_generator, SYZ_P_BUFFER, bufferHandle);

    // Link the generator to our 3D source
    return syz_sourceAddGenerator(m_source, m_generator);
}

int CSpatialSource::setPosition(double x, double y, double z)
{
    if (m_source == 0) return -1;

    // Use setD3 for 3D properties (Double x 3)
    return syz_setD3(m_source, SYZ_P_POSITION, x, y, z);
}

int CSpatialSource::setDistanceModel(const std::string& model)
{
    if (m_source == 0) return -1;

    int syzModel = SYZ_DISTANCE_MODEL_LINEAR; // Default

    if (model == "none") syzModel = SYZ_DISTANCE_MODEL_NONE;
    else if (model == "inverse") syzModel = SYZ_DISTANCE_MODEL_INVERSE;
    else if (model == "exponent") syzModel = SYZ_DISTANCE_MODEL_EXPONENTIAL;

    return syz_setI(m_source, SYZ_P_DISTANCE_MODEL, syzModel);
}

int CSpatialSource::setReferenceDistance(double distance)
{
    if (m_source == 0) return -1;
    // The distance at which the sound is at full volume
    return syz_setD(m_source, SYZ_P_DISTANCE_REF, distance);
}

int CSpatialSource::setMaxDistance(double distance)
{
    if (m_source == 0) return -1;
    // The distance beyond which the sound is either silent or at minimum volume
    return syz_setD(m_source, SYZ_P_DISTANCE_MAX, distance);
}

int CSpatialSource::setLooping(bool looping)
{
    if (m_generator == 0) return -1;
    return syz_setI(m_generator, SYZ_P_LOOPING, looping ? 1 : 0);
}
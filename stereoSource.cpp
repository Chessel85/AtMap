//stereoSource.cpp
#include "stereoSource.h"

CStereoSource::CStereoSource(syz_Handle context)
    : CSoundSource(context)
{
    // Create the Scalar Panned Source
    // This source type allows for volume and panning controls
    syz_createScalarPannedSource(&m_source, m_context, SYZ_PANNER_STRATEGY_STEREO, 0, nullptr, nullptr, nullptr);
}

CStereoSource::~CStereoSource()
{
    // The base class CSoundSource::destroy() handles handle cleanup
}

int CStereoSource::attachBuffer(syz_Handle bufferHandle)
{
    // If a generator already exists, we replace it
    if (m_generator != 0)
    {
        syz_handleDecRef(m_generator);
        m_generator = 0;
    }

    // Create a Buffer Generator (used for short sound files in memory)
    int result = syz_createBufferGenerator(&m_generator, m_context, nullptr, nullptr, nullptr);
    if (result != 0) return result;

    // Assign the buffer data to the generator
    result = syz_setI(m_generator, SYZ_P_BUFFER, bufferHandle);
    if (result != 0) return result;

    // Route the generator through the source
    return syz_sourceAddGenerator(m_source, m_generator);
}

int CStereoSource::setPan(double pan)
{
    if (m_source == 0) return -1;

    // Clamp pan between -1.0 (Left) and 1.0 (Right)
    if (pan < -1.0) pan = -1.0;
    if (pan > 1.0) pan = 1.0;

    return syz_setD(m_source, SYZ_P_PANNING_SCALAR, pan);
}

int CStereoSource::setLooping(bool looping)
{
    if (m_generator == 0) return -1;

    // SYZ_P_LOOPING is an integer property (bool 0 or 1)
    return syz_setI(m_generator, SYZ_P_LOOPING, looping ? 1 : 0);
}
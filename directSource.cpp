//directSource.cpp
#include "directSource.h"
#include <iostream>

CDirectSource::CDirectSource(syz_Handle context) 
    : CSoundSource(context)
{
    //context member variable is set in base class constructor so can attempt to create a direct sound source with it 
    if (syz_createDirectSource(&m_source, context, nullptr, nullptr, nullptr) != 0)
    {
        printf("CDirectSource: Failed to create direct source.\n");
    }
}

CDirectSource::~CDirectSource()
{
    // Base class destructor handles cleanup of m_source and m_generator
}

int CDirectSource::addStream(const std::string& filePath)
{
    //check context and source are valid from base class 
    if (!isReady() )
        return -1;

    // Release existing generator before creating a new one
    releaseHandle(m_generator);

    if (syz_createStreamingGeneratorFromFile(&m_generator, m_context, filePath.c_str(), nullptr, nullptr, nullptr) != 0)
    {
        printf("CDirectSource: Failed to create streaming generator for: %s\n", filePath.c_str());
        return -1;
    }

    // Pause so it does not play until playStream() is called
    syz_pause(m_generator);

    //Add generator to the source 
    if (syz_sourceAddGenerator(m_source, m_generator) != 0)
    {
        printf("CDirectSource: Failed to add generator to direct source.\n");
        return -1;
    }

    return 0;
}

int CDirectSource::playStream()
{
    if (m_generator == 0) 
        return -1;

    if (syz_play(m_generator) != 0)
    {
        printf("CDirectSource: Unable to play streaming generator.\n");
        return -1;
    }
    return 0;
}

int CDirectSource::pauseStream()
{
    if (m_generator == 0) 
        return -1;

    if (syz_pause(m_generator) != 0)
    {
        printf("CDirectSource: Unable to pause streaming generator.\n");
        return -1;
    }
    return 0;
}

int CDirectSource::setPan(double pan)
{
    // Note: SYZ_OTYPE_DIRECT_SOURCE does not support SYZ_P_PANNING_SCALAR.
    // This method is now a no-op or could return an error.
    return -1;
}

int CDirectSource::setLooping(bool looping)
{
    if (m_generator == 0) 
        return -1;

    if (syz_setI(m_generator, SYZ_P_LOOPING, looping ? 1 : 0) != 0)
    {
        return -1;
    }
    return 0;
}
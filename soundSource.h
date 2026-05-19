//soundSource.h
#pragma once

#include <synthizer.h>
#include <synthizer_constants.h>

class CSoundSource 
{
//Constructor
public:
    virtual ~CSoundSource();

// Common functionality for all sound source objects
public:
    virtual bool isReady() const;
    void destroy();

    // Shared parameters
public:
    int setPitch(double pitch);
    int setGain(double gain);
    int setLooping(bool looping);

    // Handles 
public:
    syz_Handle getSourceHandle() const;
    syz_Handle getGeneratorHandle() const;
    void releaseHandle(syz_Handle& handle);

    protected:
    // Protected constructor: ensures must use a subclass
    CSoundSource(syz_Handle context);

//Attributes
    // protected:
    syz_Handle m_context;
    syz_Handle m_source;
    syz_Handle m_generator;
};

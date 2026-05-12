//stereoSource.h
#pragma once

#include "soundSource.h"
#include <string>

class CStereoSource : public CSoundSource 
{
//Constructor
public:
    explicit CStereoSource(syz_Handle context);
    virtual ~CStereoSource();

//Methods
public:
    int attachBuffer(syz_Handle bufferHandle);
    int setPan(double pan);
    int setLooping(bool looping);
};
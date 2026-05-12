//spatialSource.h
#pragma once

#include "soundSource.h"
#include <string>

class CSpatialSource : public CSoundSource
{
// Constructor
public:
    explicit CSpatialSource(syz_Handle context);
    virtual ~CSpatialSource();

// Methods
public:
    int attachBuffer(syz_Handle bufferHandle);
    int setPosition(double x, double y, double z);
    int setDistanceModel(const std::string& model);
    int setReferenceDistance(double distance);
    int setMaxDistance(double distance);
    int setLooping(bool looping);
};
//spatialSource.h
#pragma once

#include "soundSource.h"

class CSpatialSource : public CSoundSource
{
// Constructor
public:
    explicit CSpatialSource(syz_Handle context);
    ~CSpatialSource() override;

//Overrides
public:
    bool isReady() const override;

// Methods
public:
    int attachBuffer(syz_Handle bufferHandle);
    int setPosition(double x, double y, double z);
    int setReferenceDistance(double distance);
    int setMaxDistance(double distance);
    int playBuffer();
};
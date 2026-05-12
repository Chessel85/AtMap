//directSource.h
#pragma once
#include "soundSource.h"
#include <string>

class CDirectSource : public CSoundSource 
{
//Constructor
public:
    CDirectSource(syz_Handle context);
    virtual ~CDirectSource();

//Methods
public:
    int addStream(const std::string& filePath);
    int playStream();
    int pauseStream();
    int setPan(double pan);
    int setLooping(bool looping);
};
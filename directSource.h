//directSource.h
#pragma once
#include "soundSource.h"
#include <string>

class CDirectSource : public CSoundSource 
{
//Constructor
public:
    CDirectSource(syz_Handle context);
    ~CDirectSource() override;

//Overrides
public:
    bool isReady() const override;

//Methods
public:
    int addStream(const std::string& filePath);
    int playStream();
    int pauseStream();
};
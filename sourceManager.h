//sourceManager.h
#pragma once

#include "synthizer.h"
#include <string>
#include <QMap>
#include "soundType.h"

//Predefines
class CDirectSource;
class CSoundBuffer;

class CSourceManager 
{
//Constructor
public:
    CSourceManager();
    ~CSourceManager();

//Methods
public:
    bool initialize( syz_Handle context );
    void destroy();
    int playMusic();
    std::string getLastError() const;
    int loadBuffers();

private:
    SoundType stringToType(const QString& str) const;
    CSoundBuffer* getBuffer(SoundType type) const;

//Attributes
private:
    syz_Handle m_context;
    CDirectSource* m_musicSource;
    QMap<SoundType, CSoundBuffer*> m_buffers;
};
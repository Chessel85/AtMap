//SoundBuffer.h 
#pragma once

#include <synthizer.h>
#include <synthizer_constants.h>
#include <QString>


class CSoundBuffer
{
    //Constructors
public:
    CSoundBuffer();
    ~CSoundBuffer();

    //Methods
public:
    bool isOk();
    int loadBuffers(const QString& path, const QString& prefix);
    void destroy();
    syz_Handle getNextBuffer();

private:
    int addBuffer(int index, const QString& fullPath);


    //Attributes
private:
    syz_Handle* m_buffer;
    QString m_rootFilename;
    int m_numBuffers;
    int m_CurrentIndex;
};

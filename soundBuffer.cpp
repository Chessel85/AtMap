//SoundBuffer.cpp
#include "SoundBuffer.h"
#include <QDir>
#include <QFileInfo>
#include <QString>
#include <QStringList>
#include <iostream>

CSoundBuffer::CSoundBuffer()
{
    m_buffer = nullptr;
    m_rootFilename = "";
    m_numBuffers = 0;
    m_CurrentIndex = 0;
}

CSoundBuffer::~CSoundBuffer()
{
    destroy();
}


void CSoundBuffer::destroy()
{
    int i = 0;
    for (i = 0; i < m_numBuffers; i++)
    {
        if (m_buffer[i] != 0)
        {
            syz_handleDecRef(m_buffer[i]);
            m_buffer[i] = 0;
        }
    }
    delete[] m_buffer;
    m_buffer = nullptr;
    m_numBuffers = 0;
}

bool CSoundBuffer::isOk()
{
    // Must have buffers to be ok 
    if (m_numBuffers <= 0 || m_buffer == nullptr)
        return false;

    // Check if handles are valid Synthizer Buffer handles 
    int type;
    for (int i = 0; i < m_numBuffers; i++)
    {
        if (syz_handleGetObjectType(&type, m_buffer[i]) != 0)
            return false;
        if (type != SYZ_OTYPE_BUFFER)
            return false;
    }

    return true;
}

syz_Handle CSoundBuffer::getNextBuffer()
{
    if (!isOk())
        return 0;

    syz_Handle h = m_buffer[m_CurrentIndex];

    // Advance and wrap around for the next request
    m_CurrentIndex = (m_CurrentIndex + 1) % m_numBuffers;

    return h;
}

int CSoundBuffer::loadBuffers(const QString& path, const QString& prefix)
{
    //Clear out any existing buffers
    destroy();

    QDir directory(path);
    if (!directory.exists()) 
    {
        qDebug() << "Directory does not exist for loading sound files:" << path;
        return -1;
    }

    // Filter for files starting with the prefix 
    QStringList filters;
    filters << prefix + "*.wav" << prefix + "*.mp3" ;

    // Get the list of matching files, sorted by name
    QFileInfoList fileList = directory.entryInfoList(filters, QDir::Files, QDir::Name);

    m_numBuffers = fileList.size();
    if (m_numBuffers == 0) 
    {
        qDebug() << "No matching sound files found for prefix:" << prefix;
        return -1;
    }

    // Allocate memory for the handles
    m_buffer = new syz_Handle[m_numBuffers];
    for (int i = 0; i < m_numBuffers; i++) 
    {
        m_buffer[i] = 0;
    }

    // Now load each found file into the array
    int successCount = 0;
    for (int i = 0; i < m_numBuffers; i++) 
    {
        QString fullPath = fileList.at(i).absoluteFilePath();

        if (addBuffer(i, fullPath) == 0)
            successCount++;
    }

    return successCount;
}

int CSoundBuffer::addBuffer(int index, const QString& fullPath)
{
    if (syz_createBufferFromFile(&m_buffer[index], fullPath.toStdString().c_str(), nullptr, nullptr ) != 0 )
    {
        const char* err = syz_getLastErrorMessage();
        qDebug() << "Synthizer Error:" << QString::fromStdString(err) << ". File:" << fullPath;
        return -1;
    }
    return 0;
}
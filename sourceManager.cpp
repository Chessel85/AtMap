//sourceManager.cpp
#include "sourceManager.h"
#include "directSource.h"
#include <iostream>
#include <QSettings>
#include <QFileInfo>
#include "initManager.h"
#include "soundBuffer.h"

CSourceManager::CSourceManager()
    : m_context(0), m_musicSource(nullptr)
{
}

CSourceManager::~CSourceManager()
{
    destroy();
}

bool CSourceManager::initialize(syz_Handle context)
{
    if (context == 0)
    {
        return false;
    }

    m_context = context;

    //Create a direct source with this context 
    m_musicSource = new CDirectSource(m_context);

    if (!m_musicSource->isReady())
    {
        delete m_musicSource;
        m_musicSource = nullptr;
        return false;
    }

    return true;
}

void CSourceManager::destroy()
{
    // Clean up the heap-allocated source
    if (m_musicSource != nullptr)
    {
        delete m_musicSource;
        m_musicSource = nullptr;
    }

    //Destroy buffers
    for (CSoundBuffer* buffer : m_buffers)
    {
        delete buffer;
    }
    m_buffers.clear();

    //Can just reset context variable as this class is not the owner 
    m_context = 0;
}

int CSourceManager::playMusic()
{
    if (m_musicSource == nullptr)
        return -1;

    //Set file up hard-coded for now
    std::string fullPath = SOUNDS_PATH + "watermark.mp3";
    m_musicSource->addStream(fullPath);
    m_musicSource->setLooping(true );
    m_musicSource->setGain( 0.5 );

m_musicSource->playStream();
}

std::string CSourceManager::getLastError() const
{
    int  errorCode = syz_getLastErrorCode();
    const char* err_msg = syz_getLastErrorMessage();
    if (err_msg != nullptr)
    {
        return std::to_string( errorCode ) + ": " + std::string(err_msg);
    }

    return "No error reported.";
}

int CSourceManager::loadBuffers()
{
    // Use QSettings to read INI file 
    QString filePath = QString::fromStdString( SOUNDS_PATH )  +  "sounds.ini";
    QSettings settings(filePath, QSettings::IniFormat);

    settings.beginGroup("soundTypes");
    QStringList keys = settings.allKeys();

    for (const QString& key : keys)
    {
        SoundType type = stringToType(key);
        QString fileName = settings.value(key).toString();

        //Create a buffer 
        CSoundBuffer* buffer = new CSoundBuffer;
        int numBuffers = buffer->loadBuffers(QString::fromStdString(SOUNDS_PATH) , fileName);
        if (numBuffers == 0 )
        {
            delete buffer;
            qDebug() << "Failed to load buffer for" << key << " =" << fileName;
            continue;
        }

        //Put the buffer in the map 
        m_buffers[type] = buffer;
    }
    settings.endGroup();

    return m_buffers.size();
}

CSoundBuffer* CSourceManager::getBuffer(SoundType type) const
{
    auto it = m_buffers.find(type);
    if (it != m_buffers.end())
    {
        return it.value();
    }
    return nullptr;
}

SoundType CSourceManager::stringToType(const QString& str) const
{
    if (str == "standard")       return SoundType::standard;
    if (str == "city")           return SoundType::city;
    if (str == "capitalCity")    return SoundType::capitalCity;
    if (str == "town")           return SoundType::town;
    if (str == "village")        return SoundType::village;
    if (str == "hamlet")         return SoundType::hamlet;
    if (str == "railwayStation") return SoundType::railwayStation;
    if (str == "latitudeLine")   return SoundType::latitudeLine;
    if (str == "longitudeLine")  return SoundType::longitudeLine;

    return SoundType::standard; // Default fallback
}
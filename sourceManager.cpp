//sourceManager.cpp
#include "sourceManager.h"
#include "directSource.h"
#include <iostream>
#include <QSettings>
#include <QFileInfo>
#include <QDir>
#include "initManager.h"
#include "soundBuffer.h"
#include "beacon.h"

CSourceManager::CSourceManager()
    : m_context(0),
    m_musicSource(nullptr),
    m_spatialSource(nullptr)
{
    m_userGeolocation = QGeoCoordinate(0, 0);
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

    //Create spatial source 
    m_spatialSource = new CSpatialSource(m_context);

    if (!m_spatialSource->isReady())
    {
        delete m_spatialSource;
        m_spatialSource = nullptr;
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
    m_musicSource->setLooping(true);
    m_musicSource->setGain(0.1);

    m_musicSource->playStream();
}

void CSourceManager::playBeacon(CBeacon* beacon, int maxExtentMetres)
{
    if (beacon == nullptr)
        return;

    //Get info from beacon to retrieve the buffer 
    QString type = beacon->getSoundType();
    SourceCategory cat = beacon->getSourceCategory();
    CSoundBuffer* buffer = m_buffers.value(type, m_buffers.value("standard"));
    m_spatialSource->attachBuffer(buffer->getNextBuffer());

    //Work out where to place the spatial sound 
    double beaconX, beaconY, beaconZ;
    beacon->getPosition(beaconX, beaconY, beaconZ);
    double relX, relY;
    calculateSpatialLocation(relX, relY, beaconX, beaconY, maxExtentMetres);
    m_spatialSource->setPosition(relX, relY, 0);

    m_spatialSource->setLooping(false);
    //m_spatialSource->setGain(1.0);
    m_spatialSource->setReferenceDistance(1.0);
    m_spatialSource->setMaxDistance(1000);
    m_spatialSource->playBuffer();
}

void CSourceManager::setUserLocation(const double x, const double y)
{
    m_userGeolocation = QGeoCoordinate(y, x);
}

std::string CSourceManager::getLastError() const
{
    int  errorCode = syz_getLastErrorCode();
    const char* err_msg = syz_getLastErrorMessage();
    if (err_msg != nullptr)
    {
        return std::to_string(errorCode) + ": " + std::string(err_msg);
    }

    return "No error reported.";
}

int CSourceManager::loadFromINI()
{
    //Clear out any existing buffers
    qDeleteAll(m_buffers);
    m_buffers.clear();
    _layersToSoundType.clear();

    // Use QSettings to read INI file 
    QString baseDir = QString::fromStdString(SOUNDS_PATH);
    QString filePath = QDir(baseDir).filePath("sounds.ini");
    QSettings settings(filePath, QSettings::IniFormat);

    //Load source types and sound files 
    loadBuffers(baseDir, settings);

    //Load layer to source type mapping
    loadLayers(settings);

    return m_buffers.size();
}

void CSourceManager::loadBuffers(const QString& baseDir, QSettings& settings)
{
    //Get source types from settings object 
    settings.beginGroup("soundTypes");
    QStringList keys = settings.allKeys();

    for (const QString& key : keys)
    {
        QString type = key.trimmed();
        const QString filename = settings.value(type).toString().trimmed();

        //Check this sourceType does not already exist
        if (m_buffers.contains(type))
        {
            qDebug() << "SourceType" << type << "already exists in map. Skipping.";
            continue;
        }

        //Create a buffer 
        CSoundBuffer* buffer = new CSoundBuffer;
        int numBuffers = buffer->loadBuffers(baseDir, filename);
        if (numBuffers == 0)
        {
            delete buffer;
            qDebug() << "Failed to load buffer for" << type << " =" << filename;
            continue;
        }

        //Put the buffer in the map 
        m_buffers.insert(type, buffer);
    }
    settings.endGroup();
}

void CSourceManager::loadLayers(QSettings& settings)
{
    //Get layer to source type mapping from settings object 
    settings.beginGroup("layers");
    QStringList keys = settings.allKeys();

    for (const QString& key : keys)
    {
        const QString layer = key.trimmed();
        QString type = settings.value(layer).toString().trimmed();

        //Check this layer name does not already exist
        if (_layersToSoundType.contains(layer))
        {
            qDebug() << "Duplicate layer " << layer << "in config file. Skipping." << settings.fileName();
            continue;
        }

        // Verify the sound type actually exists
        if (!m_buffers.contains(type))
        {
            qDebug() << "Warning: Layer" << layer << "points to undefined sound type" << type;
        }

        //Put the layer and type in the map 
        _layersToSoundType.insert(layer, type);
    }
    settings.endGroup();
}


CSoundBuffer* CSourceManager::getBuffer(const QString& type) const
{
    auto it = m_buffers.find(type);
    if (it != m_buffers.end())
    {
        return it.value();
    }
    return nullptr;
}

void CSourceManager::calculateSpatialLocation(double& relX, double& relY, double beaconLong, double beaconLat, int maxExtent)
{
    // Centre projection on user coordinates 
        if (m_audioProjection.CreateProjection(m_userGeolocation, "aeqd") != 0)
            return;

    // Package the beacon's raw longitude and latitude into a QGeoCoordinate
    QGeoCoordinate beaconGeo(beaconLat, beaconLong);

    // Transform the beacon to absolute projection plane metres
    double beaconMetreX = 0.0;
    double beaconMetreY = 0.0;

    if (m_audioProjection.ForwardTransform(beaconGeo, beaconMetreX, beaconMetreY) == 0)
    {
        // scale to be a proportion of 1000 which is the max range of the source 
        relX = ( beaconMetreX / maxExtent ) * 1000.0; 
        relY = ( beaconMetreY / maxExtent) * 1000.0; // -m_userMetreY;

        //Check inside max distance
        double actualDistance = sqrt(relX * relX + relY * relY);
        if (actualDistance > 950)
        {
            //Need to scale back relX and relY so actual distance is 950 
            double scaleRatio = 950 / actualDistance;
            relX *= scaleRatio;
            relY *= scaleRatio;
        }
    }
}


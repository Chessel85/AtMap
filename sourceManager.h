//sourceManager.h
#pragma once

#include "synthizer.h"
#include <QObject>
#include <string>
#include <QMap>
#include "spatialSource.h"
#include "soundType.h"

//Predefines
class CDirectSource;
class CSoundBuffer;
class QSettings;
class CBeacon;


class CSourceManager  : public QObject 
{
    Q_OBJECT

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
    int loadFromINI();

public slots:
    void playBeacon(CBeacon* beacon);

private:
    void loadBuffers(const QString& baseDir, QSettings& settings);
    void  loadLayers(QSettings& settings);
    CSoundBuffer* getBuffer(const QString& type) const;

//Attributes
private:
    syz_Handle m_context;
    CDirectSource* m_musicSource;
    CSpatialSource* m_spatialSource;
    QMap<QString, CSoundBuffer*> m_buffers;
    QMap<QString, QString> _layersToSoundType;
};
//outlineDialog.h
#ifndef OUTLINEDIALOG_H
#define OUTLINEDIALOG_H

#include <QDialog>
#include <QPolygonF>
#include <QPainter>
#include <QKeyEvent>
#include "GeoResult.h"

class COutlineDialog : public QDialog 
{
    Q_OBJECT

//Constructor
public:
    explicit COutlineDialog(QWidget* parent = nullptr);

//Methods
public:
    void setCountryGeometry(const  QList<CGeoResult>& geoResults );

protected:
    void paintEvent(QPaintEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;

private:

//Member variables
private:
    //Polygons 
    QList<QPolygonF> m_polygons;

    //Colours and brightness
    bool m_inverted;
    int m_fgBrightness; //ranges from 0 to 255 
    int m_bgBrightness; //ranges from 0 to 255 
    QList<CGeoResult> m_geoPolygons;
};

#endif
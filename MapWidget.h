//MapWidget.h

#pragma once

#include <qwidget.h>
#include <QList.h>
#include <qvector.h>
#include <QPolygonF>
#include <qgeocoordinate.h>
#include <qgeopolygon.h>
#include <qfont.h>
#include "RedrawReason.h"
#include "GeoResult.h"
#include "aliases.h"
#include "GeoToScreen.h"

//Structure for labels
struct labelData 
{
    QString text;
    QPointF position; //Pixel position 
    QFont font;
};

class CMapWidget : public QWidget 
{
    Q_OBJECT

//Constructor
public:
    explicit CMapWidget(QWidget* parent = nullptr);

//Methods
public:
    void receiveMapData(const QList<CGeoResult>& geoResults, const NRList& pointResults, const QGeoCoordinate& bottomLeft, const QGeoCoordinate& topright, CGeoToScreen& transformer );

    //Slots

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;

signals:
    void clickedAtPixel(const QPoint& pixelPos, const int width, const int height );
    void newLocationAndOrScale( enumRedrawReason redrawReason );


private:
    QPointF ConvertGeoToScreen(QGeoCoordinate geoCoordinate );
    void DefineLabelsToDraw(const QList<CGeoResult>& geoResults, CGeoToScreen& transformer );
    void DefinePointsToDraw(const NRList& pointResults, CGeoToScreen& transformer );
    void PaintLabels(QPainter* painter);
    void PaintPointLabels(QPainter* painter);

//Member variables
private:
    // Stores the polygons to draw and their colour index
    QList<QPolygonF> m_polygons; 
    QList<int> m_colourIndices;

    //An array of brushes set to 5 colours for filling in the countries
    QVector<QBrush>  m_brushes;
    QList<labelData> m_labels;
    QList<labelData> m_pointLabels;

    //Viewport and screen dimensions for converting between the domains
    QGeoCoordinate m_geoBottomLeft;
    QGeoCoordinate m_geoTopRight;
    double m_geoWidth;
    double m_geoHeight;
    double m_screenWidth;
    double m_screenHeight;
};

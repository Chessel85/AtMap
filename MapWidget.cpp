//MapWidget.cpp
#include "MapWidget.h"
#include <qpoint.h>
#include <QGeoRectangle>  
#include <qpainter.h>
#include <qpen.h>
#include <qbrush.h>
#include <QColor>
#include <QPalette>
#include <qfontmetrics.h>
#include <QMouseEvent> // Required for QMouseEvent
#include <qelapsedtimer.h>
#include <qSet.h>
#include <QDebug> // For debugging output


CMapWidget::CMapWidget(QWidget* parent)
: QWidget(parent)
{
    //Set default background
    setAutoFillBackground(true);
    QPalette palette = this->palette(); // Get the current palette of the widget
    palette.setColor(QPalette::Window, Qt::darkBlue); // Set the window background color
    setPalette(palette); // Apply the modified palette to the widget

    //Create an array of 7 colours to colour the countries in
    // Though first build a protective colour 0 which is middle grey for when no colour index is found 
    QColor c0(100, 100, 100);
    QBrush b0(c0);
    m_brushes.append(b0);


    // Colour 1 soft turquoise 
    QColor c1(100, 200, 190);
    QBrush b1(c1);
    m_brushes.append(b1);

    //Soft gold 
    QColor c2(240, 210, 100 );
    QBrush b2(c2);
    m_brushes.append(b2);

    //Dusty lavender 
    QColor c3(180, 160, 210 );
    QBrush b3(c3);
    m_brushes.append(b3);

    //Rose quartz 
    QColor c4(230, 160, 180 );
    QBrush b4(c4);
    m_brushes.append(b4);

    //Sage green 
    QColor c5(160, 210, 130);
    QBrush b5(c5);
    m_brushes.append(b5);

    //Terracotta
    QColor c6(220, 150, 110);
    QBrush b6(c6);
    m_brushes.append(b6);

    //Stone slate 
    QColor c7(170, 170, 170);
    QBrush b7(c7);
    m_brushes.append(b7);


    //Random other colour 
    QColor c8(100, 70, 200);
    QBrush b8(c8);
    m_brushes.append(b8);



    //Another random color 
    QColor c9(128, 210, 110);
    QBrush b9(c9);
    m_brushes.append(b9);


    //Lake blue 
    QColor c10(160, 230, 255);
    QBrush b10(c10);
    m_brushes.append(b10);


    //Default flags
}

void CMapWidget::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event); //Prevents error when parameter not used

    //Going to measure how long the draw takes
    QElapsedTimer timer;
    timer.start();

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Set up outline color and width with a pen
    QPen pen(Qt::white);
    pen.setWidth(0);
    //painter.setPen(pen );
    painter.setPen(Qt::NoPen);

    //Check lists of polygons and colours are the same length
    if (m_polygons.size() != m_colourIndices.size())
    {
        qWarning() << "MapArea::Paint: Polygon and colour indices lists are not the same length.";
        return;
    }

    // Iterate through the screen polygons and draw them
    for( int i = 0; i < m_polygons.size(); i++ )
    {
        QPolygonF polygon = m_polygons[i];
        int colourIndex = m_colourIndices[i];
        //If colour index less than zero than probably set in natural earth data migration on purpose so skip
        if (colourIndex < 0)
            continue;
        if (colourIndex >= m_brushes.size() )
            colourIndex = 0;
        if (colourIndex == 0)
            qWarning() << "Polygon has invalid colour:" << m_colourIndices[i] << " (result index" << i << ")";
        QBrush brush = m_brushes[colourIndex];  //A safety valve of brush 0 exists in case i == 0
        painter.setBrush(brush);

        if (polygon.isEmpty())
            continue;

        // Can just draw the QPolygon 
        painter.drawPolygon(polygon);
    }

    //Draw the polygon labels
    PaintLabels( &painter );

    //Draw point result labels 
    PaintPointLabels(&painter);

        //Output time to draw
    qDebug() << "Redraw took " << timer.elapsed() << "milliseconds";
    int k = 0;
    k++;
}


void CMapWidget::receiveMapData(const QList<CGeoResult>& geoResults, const NRList& pointResults, const QGeoCoordinate& bottomLeft, const QGeoCoordinate& topRight , CGeoToScreen& transformer ) 
{
    //Create a timer to measure how long this method takes to execute 
    QElapsedTimer timer;
    timer.start();

    // Clear previous data
    m_polygons.clear(); 
    m_colourIndices.clear();

    //Set viewport and screen variables for conversion between the two domains 
    m_geoBottomLeft = bottomLeft;
    m_geoTopRight = topRight;
    m_geoWidth = m_geoTopRight.longitude() - m_geoBottomLeft.longitude();
    m_geoHeight = m_geoTopRight.latitude() - m_geoBottomLeft.latitude();
    m_screenWidth = (double)(this->width());
m_screenHeight = (double)(this->height());

    //Convert geo polygons to screen polygons 
    //Iterate through the geoResult list 
    for (const CGeoResult& geoResult : geoResults )
    {
        // A geoResult contains a list of polygons  but all about one relation 
        for (const QGeoPolygon& geoPolygon : geoResult.m_geoPolygons)
        {
            //Now process this polygon 
            //Create a screen polygon to populate
            QPolygonF screenPolygon;
            
            //Pass the geopolygon through the transformer to get screen coordinates 
            transformer.transform(geoPolygon, screenPolygon);
            m_polygons.append(screenPolygon);

            //Extract the colour index for this result
            m_colourIndices.append(geoResult.m_colourIndex);
        }
    }

    //Set up the country labels
    DefineLabelsToDraw(geoResults, transformer);

    //Define points to draw 
    DefinePointsToDraw(pointResults, transformer);

    qDebug() << "Redrawing with south west: " << bottomLeft.toString() << " and north east " << topRight.toString();

    // Request a repaint
    update(); 

    //Output how long converting all the data for rendering took 
    qDebug() << "Conversion to pixels took" << timer.elapsed() << "milliseconds";
}

void CMapWidget::DefineLabelsToDraw(const QList<CGeoResult>& geoResults, CGeoToScreen& transformer)
{
    //Clear previous results
    m_labels.clear();

    //Iterate through the results and create a label for each but only one per country
    QSet<QString> processedNames;

    for (const CGeoResult& geoResult : geoResults)
    {
        QString labelText = geoResult.m_name.c_str();
        if (processedNames.contains(labelText))
            continue;
        processedNames.insert(labelText);

        //Set up a label data structure to be drawn later
        labelData labelInfo;
        labelInfo.text = labelText;
        QGeoCoordinate geoCoordinate(geoResult.m_labelY, geoResult.m_labelX);
        //labelInfo.position = ConvertGeoToScreen(geoCoordinate);
        transformer.transformPoint(geoCoordinate, labelInfo.position);
        QFont font("Arial");
        font.setWeight(QFont::Bold);
        font.setPointSize(12);
        labelInfo.font = font;

        //Add to list
        m_labels.append(labelInfo);
    }
}

void CMapWidget::DefinePointsToDraw(const NRList& pointResults, CGeoToScreen& transformer )
{
    //Clear previous results
    m_pointLabels.clear();


    QFont cityFont("Arial", 10, QFont::Normal); 

    //Iterate through the point results and create a label 
        for (const nearbyResult& pointResult : pointResults )
    {
        QString labelText = pointResult.name.c_str();

        //Set up a label data structure to be drawn later
        labelData labelInfo;
        labelInfo.text = labelText;
        QGeoCoordinate geoCoordinate( pointResult.latitude, pointResult.longitude);
        //labelInfo.position = ConvertGeoToScreen(geoCoordinate);
        transformer.transformPoint(geoCoordinate, labelInfo.position);
        labelInfo.font = cityFont;

        //Add to list
        m_pointLabels.append(labelInfo);
    }
}

void CMapWidget::PaintLabels(QPainter* painter)
{
    //Go through the label list and draw
    for (labelData& ld : m_labels)
    {
        painter->setFont(ld.font);
        //Work out where to draw
        QFontMetricsF fm(ld.font);
        QRectF textBounds = fm.boundingRect(ld.text);
        //Calculate top - left for drawing relative to ld.position(the center)
        QPointF topLeftToDraw;
        topLeftToDraw.setX(ld.position.x() - textBounds.width() / 2.0);
        topLeftToDraw.setY(ld.position.y() - textBounds.height() / 2.0); // For baseline drawing with drawText, you might adjust this for descent

        // Draw a light shadow for better visibility
        painter->setPen(QPen(Qt::black, 2)); 
        painter->drawText(topLeftToDraw.x() + 1, topLeftToDraw.y() + 1, ld.text);
        painter->setPen(Qt::white); // Main text
painter->drawText(topLeftToDraw, ld.text);
    }
}

void CMapWidget::PaintPointLabels(QPainter* painter)
{
    //Define the spot marker 
    int spotRadius = 4;
    int rimThickness = 2;

    // Set up the painter's pen for the black rim
    QPen rimPen(Qt::black);
    rimPen.setWidth(rimThickness);
    painter->setPen(rimPen);

    // Set up the painter brush for the white fill
    QBrush dotBrush(Qt::white);
    painter->setBrush(dotBrush);

    //Go through the point label list and draw
    for (labelData& pld : m_pointLabels)
    {
        // Draw the circle
        painter->setPen(rimPen);
        painter->drawEllipse( pld.position, spotRadius, spotRadius);

        //Work out where to draw the label just to the right 
        painter->setFont(pld.font);
        QFontMetricsF fm(pld.font);
        QRectF textBounds = fm.boundingRect(pld.text);
        //Calculate top - left for drawing relative to ld.position(the center)
        QPointF topLeftToDraw;
        topLeftToDraw.setX(pld.position.x() + spotRadius + rimThickness+ 2);
        topLeftToDraw.setY(pld.position.y() - textBounds.height() / 2.0); // For baseline drawing with drawText, you might adjust this for descent

        // Draw a light shadow for better visibility
        painter->setPen(QPen(Qt::black, 2));
        painter->drawText(topLeftToDraw.x() + 1, topLeftToDraw.y() + 1, pld.text);
        painter->setPen(Qt::white); // Main text
        painter->drawText(topLeftToDraw, pld.text);
    }
}

void CMapWidget::mousePressEvent(QMouseEvent* event)
{
    // Check if the left mouse button was pressed.
    // You could also check for Qt::RightButton, Qt::MiddleButton, etc.
    if (event->button() == Qt::LeftButton)
    {
        // Get the position of the mouse click, relative to the widget itself.
        QPoint clickPosition = event->pos();
        int width = this->width();
        int height = this->height();

        qDebug() << "CMapWidget: Mouse clicked at pixel coordinates:" << clickPosition;

        // Emit the signal, passing the pixel coordinates of the click.
        // MainWindow will listen for this signal and initiate a relocate and map update 
        emit clickedAtPixel(clickPosition, width, height );
    }

    // IMPORTANT: Always call the base class implementation.
    // This ensures that default mouse event handling (if any) is performed,
    // and that the event continues to propagate up the widget hierarchy.
    QWidget::mousePressEvent(event);
}

QPointF CMapWidget::ConvertGeoToScreen(QGeoCoordinate geoCoordinate)
{
        double x = m_screenWidth * (geoCoordinate.longitude() - m_geoBottomLeft.longitude()) / m_geoWidth;
    double y = m_screenHeight * (1.0 - (geoCoordinate.latitude() - m_geoBottomLeft.latitude()) / m_geoHeight);

    return QPointF(x, y);
    }
//outlineDialog.cpp
#include "OutlineDialog.h"
#include <QGeoCoordinate>
#include "GeoToScreen.h"


COutlineDialog::COutlineDialog(QWidget* parent) : QDialog(parent) 
{
    // Keep the Title Bar and System Menu, but remove Min/Max buttons
    setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint);

    //Set drawing defaults
    m_inverted = false;
    m_fgBrightness = 128;
    m_bgBrightness = 20;
}

void COutlineDialog::paintEvent(QPaintEvent* event)
{
    //Check there is data to paint  but should be handled more gracefully where dialog instantiated 
    if (m_geoPolygons.size() == 0)
        return;
    CGeoResult geoResult = m_geoPolygons.at(0);
    if (geoResult.m_geoPolygons.size() == 0)
        return;

    //Set viewport and screen variables for conversion between the two domains 
    QGeoCoordinate geoBottomLeft(geoResult.m_minY, geoResult.m_minX);
    QGeoCoordinate geoTopRight(geoResult.m_maxY, geoResult.m_maxX);
    double geoWidth = geoTopRight.longitude() - geoBottomLeft.longitude();
    double geoHeight = geoTopRight.latitude() - geoBottomLeft.latitude();
    double screenWidth = (double)(this->width());
    double screenHeight = (double)(this->height());

    //Prepare to transform geocoordinates to screen pixels
    CGeoToScreen transformer;
    QGeoCoordinate geoCentre( geoBottomLeft.latitude() + geoHeight/ 2, geoBottomLeft.longitude() + geoWidth/ 2 );
    transformer.SetScreenSize(QSizeF(screenWidth, screenHeight));
    transformer.CreateProjection(geoCentre);
    transformer.setTransformedBoundingBox(geoBottomLeft, geoTopRight);

    //Prepare to draw 
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);


    //Set colours accounting for any inversion
    int fg = m_fgBrightness;
    int bg = m_bgBrightness;
    if (m_inverted)
        std::swap(fg, bg);

    // Define base colors: Yellow (60 degrees) and Blue (240 degrees)
    QColor countryColor = QColor::fromHsl(60, 255, fg);
    QColor backgroundColor = QColor::fromHsl(240, 255, bg);

    // Fill background
    painter.fillRect(rect(), backgroundColor);

    // Setup brush for the country
    painter.setBrush(countryColor);
    painter.setPen(Qt::NoPen);

    for (QGeoPolygon geoPolygon : geoResult.m_geoPolygons)
    {
        QPolygonF screenPolygon;

        //Transform
        transformer.transform(geoPolygon, screenPolygon);

        if (screenPolygon.isEmpty())
            continue;

        //Draw
        painter.drawPolygon(screenPolygon);
    }
}


void COutlineDialog::keyPressEvent(QKeyEvent* event) 
{
    //Handle arrow keys for changing brightness, i for inverting and escape to close
    int step = 5; //Change brightness in steps of 10 

    switch (event->key()) {
    case Qt::Key_Up:
        m_fgBrightness = qMin(255, m_fgBrightness + step);
        break;
    case Qt::Key_Down:
        m_fgBrightness = qMax(0, m_fgBrightness - step);
        break;
    case Qt::Key_Right:
        m_bgBrightness = qMin(255, m_bgBrightness + step);
        break;
    case Qt::Key_Left:
        m_bgBrightness = qMax(0, m_bgBrightness - step);
        break;
    case Qt::Key_I:
        m_inverted = !m_inverted;
        break;
    case Qt::Key_Escape:
        accept(); // Closes the dialog
        return;
    default:
        QDialog::keyPressEvent(event);
        return;
    }

    //Redraw with new colours 
    update(); 
}

void COutlineDialog::setCountryGeometry(const  QList<CGeoResult>& geoResults)
{
    //Clear anything already present
    m_geoPolygons.clear();

    //Copy pointers
    m_geoPolygons.append( geoResults );
    }
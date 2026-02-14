//CompassPane.h
#pragma once

#include <QWidget.h>
#include <QPushbutton.h>
#include <QLabel.h>
#include <QKeyEvent> 
#include <QMouseEvent>   
#include <QShowEvent>
#include <QGeoCoordinate.h>
#include <QString.h>
#include "RedrawReason.h"

class CCompassPane : public QWidget
{
    Q_OBJECT

//Constructor
public:
    explicit CCompassPane(QWidget* parent = nullptr);

//Methods
public:
    void setEnabledAllButtons(bool bState);
    void disableStepExtreme(int stepExtreme);
    void SaveFocus();
    void RestoreFocus();

private:
    void SetupCompass();
    void UpdateLocationLabel( QString locationText, QGeoCoordinate& GeoCoordinate );
    void UpdateStepLabel( double stepSize );
    int getKeyFromFocusedButton();
    bool EmitDirections(int key);

    //Slots
public:
    void UpdateLabels(enumRedrawReason redrawReason, QString locationText, QGeoCoordinate& GeoCoordinate, double stepSize, int stepExtreme );

protected:
    bool eventFilter(QObject* obj, QEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    bool focusNextPrevChild(bool next) override;
    void mousePressEvent(QMouseEvent* event) override;
    void showEvent(QShowEvent* event) override;

signals:
    void northRequested();
    void eastRequested();
    void southRequested();
    void westRequested();
    void stepMoreRequested();
    void stepLessRequested();


//Member variables
private:
    QPushButton* m_pNorthButton;
    QPushButton* m_pEastButton;
    QPushButton* m_pSouthButton;
    QPushButton* m_pWestButton;

    //Location and step size labels and buttons 
    QLabel* m_pLocationLabel;
    QLabel* m_pStepLabel;
    QPushButton* m_pStepMoreButton;
    QPushButton* m_pStepLessButton;
    QWidget* m_pLastEnabledFocusedWidget;
    QWidget* m_pLastF6FocusedWidget;
    bool m_initialShow;
};

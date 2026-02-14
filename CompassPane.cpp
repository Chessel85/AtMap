//CompassPane.cpp
#include "CompassPane.h"
#include <QApplication>
#include <QGridLayout>
#include <QKeyEvent>
#include <QAccessible.h>
#include <QAccessibleValueChangeEvent> 
#include <QAccessibleAnnouncementEvent>
#include <QDebug> // For debugging output

CCompassPane::CCompassPane(QWidget* parent)
: QWidget(parent)
{
    //Give this pane strong focus so it gets keystrokes
    setFocusPolicy(Qt::TabFocus);

    //Set background colour 
    setStyleSheet("background-color: yellow; border: 1px solid orange;");

    //And minimum size
    setMinimumSize(50, 50);


    //Set accessible name and description for accessibility tree
    setAccessibleName("Compass control area");
    //m_pCompassWidget->setAccessibleDescription("Contains buttons and information for moving.");


    //Set up the child widgets
    SetupCompass();

    //Default variable values
    m_pLastEnabledFocusedWidget = nullptr;
    m_pLastF6FocusedWidget = nullptr;
    m_initialShow = true;
}

void CCompassPane::SetupCompass()
{
    //Create a grid for the compass buttons and other labels to go in
    QGridLayout* pCompassLayout = new QGridLayout(this );

    //Create the buttons
    m_pNorthButton = new QPushButton("N", this);
    m_pEastButton = new QPushButton("E", this);
    m_pSouthButton = new QPushButton("S", this);
    m_pWestButton = new QPushButton("W", this);

    //Create information labels 
    m_pLocationLabel = new QLabel("Location: ", this);
    m_pLocationLabel->setObjectName("locationLabel");
    m_pLocationLabel->setStyleSheet("font-weight: bold;");

    //Step label and buttons 
    m_pStepLabel = new QLabel("Step: ", this);
    m_pStepLabel->setObjectName("stepLabel");
    m_pStepLabel->setStyleSheet("font-weight: bold;");
    m_pStepMoreButton = new QPushButton("+", this);
    m_pStepLessButton = new QPushButton("-", this);

    //Set accessible names so screen readers read this rather than default text 
    m_pNorthButton->setAccessibleName("North");
    m_pEastButton->setAccessibleName("East");
    m_pSouthButton->setAccessibleName("South");
    m_pWestButton->setAccessibleName("West");
    m_pStepMoreButton->setAccessibleName("plus");
    m_pStepLessButton->setAccessibleName("minus");

    //Place the buttons and labels in the layout grid
    pCompassLayout->addWidget(m_pNorthButton, 0, 1);
    pCompassLayout->addWidget(m_pEastButton, 1, 2);
    pCompassLayout->addWidget(m_pSouthButton, 2, 1);
    pCompassLayout->addWidget(m_pWestButton, 1, 0);
    pCompassLayout->addWidget(m_pStepLabel, 3, 0);

    // Create a horizontal layout for the step buttons
    QHBoxLayout* pStepButtonsLayout = new QHBoxLayout();
    pStepButtonsLayout->addWidget(m_pStepMoreButton);
    pStepButtonsLayout->addWidget(m_pStepLessButton);    pCompassLayout->addWidget(m_pLocationLabel, 4, 0, 1, 3); //Bottom row across all columns 

    // You can add spacing or stretch here if needed, e.g., pStepButtonsLayout->addStretch();
    pStepButtonsLayout->setContentsMargins(0, 0, 0, 0);

    // Add this new horizontal layout to the grid, spanning columns 1 and 2
    pCompassLayout->addLayout(pStepButtonsLayout, 3, 1, 1, 2); // Row 3, starting column 1, spanning 1 row and 2 columns

    //Layout the buttons neatly 
    pCompassLayout->setAlignment(Qt::AlignCenter);
    pCompassLayout->setColumnStretch(0, 1);
    pCompassLayout->setColumnStretch(1, 1);
    pCompassLayout->setColumnStretch(2, 1);
    pCompassLayout->setRowStretch(0, 1);
    pCompassLayout->setRowStretch(1, 0);
    pCompassLayout->setRowStretch(2, 0);
    pCompassLayout->setRowStretch(3, 0);
    pCompassLayout->setRowStretch(4, 1);

    //Allow the buttons and labels to receive the tab order
    m_pNorthButton->setFocusPolicy(Qt::StrongFocus);
    m_pEastButton->setFocusPolicy(Qt::StrongFocus);
    m_pSouthButton->setFocusPolicy(Qt::StrongFocus);
    m_pWestButton->setFocusPolicy(Qt::StrongFocus);
    m_pStepLabel->setFocusPolicy(Qt::StrongFocus);
    m_pStepMoreButton->setFocusPolicy(Qt::StrongFocus);
    m_pStepLessButton->setFocusPolicy(Qt::StrongFocus);
    m_pLocationLabel->setFocusPolicy(Qt::TabFocus);

    //Install event filters so compass button keystrokes  are handled by this class  
    m_pNorthButton->installEventFilter(this);
    m_pEastButton->installEventFilter(this);
    m_pSouthButton->installEventFilter(this);
    m_pWestButton->installEventFilter(this);
    m_pStepLabel->installEventFilter(this);
    m_pStepMoreButton->installEventFilter(this);
    m_pStepLessButton->installEventFilter(this);
    m_pLocationLabel->installEventFilter(this);

}

void CCompassPane::showEvent(QShowEvent* event)
{
    QWidget::showEvent(event);

    if (m_initialShow)
    {
        m_initialShow = false;
        m_pNorthButton->setFocus();
        QAccessibleAnnouncementEvent event(m_pNorthButton, m_pNorthButton->accessibleName());
        event.setPoliteness(QAccessible::AnnouncementPoliteness::Polite);
        QAccessible::updateAccessibility(&event);
            }
}


bool CCompassPane::eventFilter(QObject* obj, QEvent* event)
{
    if (event->type() == QEvent::KeyPress) 
    {
        QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
        int key = keyEvent->key();

        // Handle the arrow keys and the plus/minus keys here
        if (key == Qt::Key_Space  || key == Qt::Key_Up || key == Qt::Key_Down || key == Qt::Key_Left || key == Qt::Key_Right || key == Qt::Key_Plus || key == Qt::Key_Minus || key == Qt::Key_Equal)
        {
            CCompassPane::keyPressEvent(keyEvent);

            // Accept the event to stop further propagation to the button itself
            return true;
        }
    }
    else if (event->type() == QEvent::MouseButtonPress)
    {
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
        mousePressEvent(mouseEvent);
        return true;
    }

    // For all other events, let the base class handle them
    return QWidget::eventFilter(obj, event);
}


void CCompassPane::keyPressEvent(QKeyEvent* event)
{
    //Get key and update it if space or enter pressed 
    int key = event->key();

    //Capture if space, enter or return pressed and work out which button has the focus 
    if (key == Qt::Key_Space || key == Qt::Key_Enter  || key == Qt::Key_Return )
        key = getKeyFromFocusedButton();

    bool handled = EmitDirections(key);
    
    if (handled) 
    {
        event->accept(); // Indicate the event was handled
    }
    else 
    {
        event->ignore();
        QWidget::keyPressEvent(event); // Pass unhandled events to the base class
    }
    }

bool CCompassPane::focusNextPrevChild(bool next) 
{
    QWidget* focusedWidget = QApplication::focusWidget();

    //Check if current focus is on the last button and that tab pressed
    if (next && focusedWidget == m_pStepLessButton)
    {
        m_pNorthButton->setFocus();
        return true;
    }
    //Or going backwards and on first button 
    else if (!next && focusedWidget == m_pNorthButton)
    {
        //If minus key disabled go to the plus key 
        if (m_pStepLessButton->isEnabled())
            m_pStepLessButton->setFocus();
        else
            m_pStepMoreButton->setFocus();
        return true;
    }
    //Can be on plus button with minus button disabled 
    else if (next && !m_pStepLessButton->isEnabled() && focusedWidget == m_pStepMoreButton)
    {
        m_pNorthButton->setFocus();
        return true;
    }


    return QWidget::focusNextPrevChild(next);
}

void CCompassPane::mousePressEvent(QMouseEvent* event) 
{
    bool handled = false;

    // Check if the left mouse button was pressed
    if (event->button() == Qt::LeftButton)
    {
        // Get the key from the focused button 
        int key = getKeyFromFocusedButton();

        handled = EmitDirections(key);
    }

    // Accept if handled so processing stops 
    if( handled )
    {
        event->accept();
    }
    else 
    {
        // For any other mouse button or event, let the base class handle it
        QWidget::mousePressEvent(event);
    }
}



int CCompassPane::getKeyFromFocusedButton()
{
    int key = 0;
    QWidget* focus = QApplication::focusWidget();

    if (focus == m_pNorthButton)
        key = Qt::Key_Up;
    else if (focus == m_pEastButton)
        key = Qt::Key_Right;
    else if (focus == m_pSouthButton)
        key = Qt::Key_Down;
    else if (focus == m_pWestButton)
        key = Qt::Key_Left;
    else if (focus == m_pStepLessButton)
        key = Qt::Key_Minus;
    else if (focus == m_pStepMoreButton)
        key = Qt::Key_Plus;
        
    return key;
}

bool CCompassPane::EmitDirections(int key)
{
    bool handled = false;

    switch (key)
    {
    case Qt::Key_Up:
        emit northRequested();
        handled = true;
        break;
    case Qt::Key_Down:
        emit southRequested();
        handled = true;
        break;
    case Qt::Key_Left:
        emit westRequested();
        handled = true;
        break;
    case Qt::Key_Right:
        emit eastRequested();
        handled = true;
        break;
    case Qt::Key_Plus:
    case Qt::Key_Equal:
        emit stepMoreRequested();
        handled = true;
        break;
    case Qt::Key_Minus:
        emit stepLessRequested();
        handled = true;
        break;
    case Qt::Key_F6:
        handled = false;
        break;
    default:
        handled = false;
        break;
    }

    return handled;
}

void CCompassPane::setEnabledAllButtons(bool bState)
{
    //Capture current focus if disabling 
    if (!bState)
        m_pLastEnabledFocusedWidget = QApplication::focusWidget();

    //Set state of compass buttons 
    m_pNorthButton->setEnabled(bState);
    m_pEastButton->setEnabled(bState);
    m_pSouthButton->setEnabled(bState);
    m_pWestButton->setEnabled(bState);
    m_pStepLessButton->setEnabled(bState);
    m_pStepMoreButton->setEnabled(bState);

    //Return focus if enabling
    if (bState && m_pLastEnabledFocusedWidget )
    {
        m_pLastEnabledFocusedWidget->setFocus();
        m_pLastEnabledFocusedWidget = nullptr;
    }
}

void CCompassPane::disableStepExtreme(int stepExtreme)
{
    //stepExtreme of 1 means at largest step size so disable the step more button and value of minus 1 means at low end of range
    if (stepExtreme == 1)
        m_pStepMoreButton->setEnabled(false);
    else if (stepExtreme == -1)
        m_pStepLessButton->setEnabled(false);
}

void CCompassPane::SaveFocus()
{
    m_pLastF6FocusedWidget = QApplication::focusWidget();
    }

void CCompassPane::RestoreFocus()
{
    if (m_pLastF6FocusedWidget)
        m_pLastF6FocusedWidget->setFocus();
    else
        m_pNorthButton->setFocus();

    //Set variable back to null 
    m_pLastF6FocusedWidget = nullptr;
}

void CCompassPane::UpdateLocationLabel(QString locationText, QGeoCoordinate& geoCoordinate )
{
    // Ensure the label exists before trying to update it
    if (!m_pLocationLabel)
    {
        qDebug() << "Error: Location label not created.";
        return;
    }

    
    //And also get the longitude and latitude and Format the strings to 2 decimal places and add N/S or E/W
    double longitude = geoCoordinate.longitude();
    double latitude = geoCoordinate.latitude();

    QString latStr = QString("%1").arg(latitude, 0, 'f', 2);
    QString lonStr = QString("%1").arg(longitude, 0, 'f', 2);

    //Have notation in decimal format for now but should make it an option how it is displayed 
    //if (lon >= 0) lonStr += "East";   else   lonStr += "West";
    //if (lat >= 0) latStr += "North"; else latStr += "South";

    //Construct the label
    QString newLocationLabel = QString("Location: %1 %2 latitude %3 longitude").arg(locationText).arg(latStr).arg(lonStr);
    
    QString newAnnounceLocationLabel = QString("%1 %2 latitude %3 longitude").arg(locationText).arg(latStr).arg(lonStr);

    //Update the label
    m_pLocationLabel->setText(newLocationLabel);

    //Poke screen readers to announce the label text change 
    if (QAccessible::isActive())
    {
        QAccessibleAnnouncementEvent event(m_pLocationLabel, newAnnounceLocationLabel);
        event.setPoliteness(QAccessible::AnnouncementPoliteness::Polite);
        QAccessible::updateAccessibility(&event);
    }
}

void CCompassPane::UpdateStepLabel( double stepSize )
{
    // Ensure the label exists before trying to update it
    if (!m_pStepLabel)
    {
        qDebug() << "Error: Step label not created.";
        return;
    }

    //Construct the label
    QString newStepLabel;

    //How step size is displayed depends on if it is more or less than 1km 
    if (stepSize > 1.0)
        newStepLabel = QString("Step: %1km").arg(stepSize);
    else if (stepSize < 1.0)
        newStepLabel = QString("Step: %1m").arg(stepSize * 1000);

    //Similar logic  for announcement to screen reader but also make 1km singular 
    QString newAnnounceStepLabel;
    if (stepSize * 1000 == 1.0)
        newAnnounceStepLabel = QString("%1metre").arg(stepSize * 1000);
    else if (stepSize > 1.0)
        newAnnounceStepLabel = QString("%1kilometres").arg(stepSize);
    else if (stepSize < 1.0)
        newAnnounceStepLabel = QString("%1metres").arg(stepSize * 1000);
    else
        newAnnounceStepLabel = QString("%1kilometre").arg(stepSize);

    //Update the label
    m_pStepLabel->setText(newStepLabel);

    //Poke screen readers to announce the label text change 
    if (QAccessible::isActive())
    {
        QAccessibleAnnouncementEvent event(m_pStepLabel, newAnnounceStepLabel);
        event.setPoliteness(QAccessible::AnnouncementPoliteness::Polite);
        QAccessible::updateAccessibility(&event);
    }
}

void CCompassPane::UpdateLabels(enumRedrawReason redrawReason, QString locationText, QGeoCoordinate& geoCoordinate, double stepSize , int stepExtreme )
{
    //Can now ensure all buttons are enabled ready to be used again 
    setEnabledAllButtons(true);

    //Disable step buttons if at extreme of range
    disableStepExtreme(stepExtreme);

    //The cause of the redraw of the map is either a location change or a step size change and need to update the labels accordingly 
    if (redrawReason == rrLocationChange)
        UpdateLocationLabel( locationText, geoCoordinate);
    else if (redrawReason == rrStepSizeChange)
        UpdateStepLabel( stepSize);
    else
    {
        UpdateLocationLabel(locationText, geoCoordinate);
        UpdateStepLabel(stepSize);
    }
}
//GotoLocationDialog.cpp
#include "GotoLocationDialog.h"
#include <QGridlayout.h>
#include <QDoubleValidator  >
#include <QDebug> // For debugging output

CGotoLocationDialog::CGotoLocationDialog(QWidget* parent)
: QDialog(parent)
{
    //Initialise variables as needed 
    m_showFirstTime = true;

    //Title for dialog
    setWindowTitle( "Go to Coordinates" );

    //Set fixed dimensions for consistency
    setFixedSize(300, 150);

    //Create the controls
    lblLatitude = new QLabel("Latitude: ", this);
    lblLongitude = new QLabel("Longitude: ", this);
    editLatitude = new QLineEdit(this);
    editLongitude = new QLineEdit(this);

    //Validators to limit what can be entered
    QDoubleValidator* latitudeValidator = new QDoubleValidator(-90.0, 90.0, 6, this);
    latitudeValidator->setNotation(QDoubleValidator::StandardNotation); // Allow standard decimal notation
    editLatitude->setValidator(latitudeValidator);

    QDoubleValidator* longitudeValidator = new QDoubleValidator(-180.0, 180.0, 6, this);
    longitudeValidator->setNotation(QDoubleValidator::StandardNotation); // Allow standard decimal notation
    editLongitude->setValidator(longitudeValidator);

    //Ok and cancel buttons
    buttonOK = new QPushButton(this);
    buttonOK->setText("&OK");
    buttonCancel = new QPushButton(this);
    buttonCancel->setText("&Cancel");


    // When OK is clicked, the dialog is accepted() signal is emitted and it closes with QDialog::Accepted
    connect(buttonOK, &QPushButton::clicked, this, &CGotoLocationDialog::accept);
    // When Cancel is clicked, the dialog is rejected() signal is emitted and it closes with QDialog::Rejected
    connect(buttonCancel, &QPushButton::clicked, this, &CGotoLocationDialog::reject);

    //Improve accessibility
    lblLongitude->setBuddy(editLongitude);
    lblLatitude->setBuddy(editLatitude);

    //Setup the user interface
    setupUI();
}

void CGotoLocationDialog::setupUI()
{
    //Use a grid layout to  space the widgets 
    QGridLayout* mainLayout = new QGridLayout(this);

    //Add widgets into the grid 
    mainLayout->addWidget(lblLatitude, 0, 0);  //Row 0 column 0
    mainLayout->addWidget(editLatitude, 0, 1); //Row 0 column 1
    mainLayout->addWidget(lblLongitude, 1, 0); //Row 1 column 0
    mainLayout->addWidget(editLongitude, 1, 1 ); //Row 1 column 1

    //Buttons get added  spanning columns
    mainLayout->addWidget(buttonOK, 2, 0, 1, 2, Qt::AlignRight);  //Row 2 column 0 spanning 2 columns aligned right
    mainLayout->addWidget(buttonCancel, 2, 1, 1, 1, Qt::AlignRight);  //Next column along

    // Adjust column stretch to make the input fields expand
    mainLayout->setColumnStretch(1, 1);

    //Apply this layout to the dialog
    setLayout(mainLayout);
}


double CGotoLocationDialog::getLatitude() const
{
    // Convert the text from the QLineEdit to a double
    // toDouble() returns 0.0 if conversion fails, which is acceptable for invalid input
    return editLatitude->text().toDouble();
}

double CGotoLocationDialog::getLongitude() const
{
    // Convert the text from the QLineEdit to a double
    return editLongitude->text().toDouble();
}

void CGotoLocationDialog::setLatitude(double latitude )
{
    editLatitude->setText(QString::number(latitude));
    }

void CGotoLocationDialog::setLongitude(double longitude)
{
    editLongitude->setText(QString::number(longitude));
}

void CGotoLocationDialog::showEvent(QShowEvent* event)
{
    QDialog::showEvent(event);

    //Set focus first time this dialog is shown
    if (m_showFirstTime)
    {
        m_showFirstTime = false;
        editLatitude->setFocus();
    }
}
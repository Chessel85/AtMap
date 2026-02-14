//SearchLocationDialog.cpp
#include "SearchLocationDialog.h"
#include <QGridlayout.h>
#include <QDebug> // For debugging output

enum CustomItemDataRole 
{
    CoordinatesRole = Qt::UserRole + 1 // Custom role to store coordinates
};


CSearchLocationDialog::CSearchLocationDialog(QWidget* parent)
: QDialog(parent)
{
    //Initialise variables as needed 
    m_bInitialShow  = true;

    //Title for dialog
    setWindowTitle( "Search For Location" );
    setModal(true);

    //Set fixed dimensions for consistency
    setFixedSize(400, 300);

    //Create the controls
    lblSearchText= new QLabel("Search text: ", this);
    editSearchText= new QLineEdit(this);
    lblSearchText->setBuddy(editSearchText);
    buttonSearch = new QPushButton(this);
    buttonSearch->setText("&Search");
    connect(buttonSearch, &QPushButton::clicked, this, &CSearchLocationDialog::onSearchButtonClicked);
    
    //Search results list widget 
    listSearchResults = new QListWidget(this);
    listSearchResults->setAccessibleName("Search Results"); // Accessible name for screen readers

    //Ok and cancel buttons
    buttonOK = new QPushButton( this);
    buttonOK->setText("&OK");
    connect(buttonOK, &QPushButton::clicked, this, &CSearchLocationDialog::onOkButtonClicked);
    buttonCancel = new QPushButton(this);
    buttonCancel->setText("&Cancel");

    // When OK is clicked, the dialog is accepted() signal is emitted and it closes with QDialog::Accepted
    connect(buttonOK, &QPushButton::clicked, this, &CSearchLocationDialog::accept);
    // When Cancel is clicked, the dialog is rejected() signal is emitted and it closes with QDialog::Rejected
    connect(buttonCancel, &QPushButton::clicked, this, &CSearchLocationDialog::reject);

    connect(editSearchText, &QLineEdit::textChanged, this, &CSearchLocationDialog::onSearchTextChanged);
    connect(listSearchResults, &QListWidget::currentItemChanged, this, &CSearchLocationDialog::onResultsListSelectionChanged);
    connect(listSearchResults, &QListWidget::itemDoubleClicked, this, &CSearchLocationDialog::onOkButtonClicked); 

    //Setup the user interface
    setupUI();

    //Initially not everything is enabled
    buttonSearch->setEnabled(false);
    buttonOK->setEnabled(false);
}

void CSearchLocationDialog::setupUI()
{
    //Use a grid layout to  space the widgets 
    QGridLayout* mainLayout = new QGridLayout(this);

    //Add widgets into the grid 
    mainLayout->addWidget(lblSearchText, 0, 0);  //Row 0 column 0
    mainLayout->addWidget(editSearchText, 0, 1); //Row 0 column 1
    mainLayout->addWidget(buttonSearch, 0, 2); //Row 0 column 2 
    mainLayout->addWidget(listSearchResults, 1, 0,1, 3 ); //Row 1 spanning 3 columns

    //Buttons get added  spanning columns
    mainLayout->addWidget(buttonOK, 2, 0, 1, 2, Qt::AlignRight);  //Row 2 column 0 spanning 2 columns aligned right
    mainLayout->addWidget(buttonCancel, 2, 1, 1, 1, Qt::AlignRight);  //Next column along

    // Adjust column stretch to make the input fields expand
    mainLayout->setColumnStretch(1, 1);

    //Row stretch lets results expand
    mainLayout->setRowStretch(1, 1);

    //Apply this layout to the dialog
    setLayout(mainLayout);
}


void CSearchLocationDialog::showEvent(QShowEvent* event)
{
    //Call base class implementation first 
    QDialog::showEvent(event);

    //Carry out following only on initial show
    if (m_bInitialShow)
    {
        m_bInitialShow  = false;
        editSearchText->setFocus();
    }
}

void CSearchLocationDialog::keyPressEvent(QKeyEvent* event)
{
    // Check if the Enter key (Return or Enter on numeric keypad) was pressed
    if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter)
    {
        // Check if the current focus is on the search results list widget
        // or if an item is selected in the list
        if (listSearchResults->hasFocus() && listSearchResults->currentItem() != nullptr)
        {
            // Call the same method that handles the OK button click
            onOkButtonClicked();
            // Indicate that the event was handled
            event->accept();
            return; // Stop further processing of this event
        }
    }
    // For all other keys or if conditions are not met, pass the event to the base class
    QDialog::keyPressEvent(event);
}

void CSearchLocationDialog::handleSearchResults(const QMap<QString, QGeoCoordinate >& results)
{
    listSearchResults->clear();
    m_currentResults.clear();

    if (results.isEmpty() )
    {
        listSearchResults->addItem("No results");
        buttonOK->setEnabled(false);
    }
    else
    {
        //Add results to list
        m_currentResults = results;
        for (auto it = results.constBegin(); it != results.constEnd(); it++)
        {
            QListWidgetItem* item = new QListWidgetItem(it.key());
            item->setData(CoordinatesRole, QVariant::fromValue(it.value()));
            listSearchResults->addItem(item);
        }
    }

    //Select the first item
    if (listSearchResults->count() > 0)
    {
        listSearchResults->setCurrentRow(0);
        listSearchResults->setFocus();
    }
    }

void CSearchLocationDialog::onSearchButtonClicked()
{
    QString searchTerm = editSearchText->text().trimmed();
    if (!searchTerm.isEmpty())
    {
        qDebug() << "Searching with term" << searchTerm;
        emit searchRequested(searchTerm);
    }
   
}

void CSearchLocationDialog::onOkButtonClicked()
{
    QListWidgetItem* item = listSearchResults->currentItem();
    if (item)
    {
        QGeoCoordinate coordinates = item->data(CoordinatesRole).value<QGeoCoordinate>();
        qDebug() << "Location selected:" << item->text() << "at" << coordinates;
        emit locationSelected(coordinates);
        accept();
    }
}

void CSearchLocationDialog::onSearchTextChanged(const QString& text)
{
    QString searchText = editSearchText->text().trimmed();
    buttonSearch->setEnabled(!searchText.isEmpty());
}

void CSearchLocationDialog::onResultsListSelectionChanged()
{
    //Enable the okay button if there is a selected item in the list of results 
    QListWidgetItem* item = listSearchResults->currentItem();

    if (item != nullptr)
    {
        buttonOK->setEnabled(true);
    }
    else
    {
        buttonOK->setEnabled(false);
    }
}

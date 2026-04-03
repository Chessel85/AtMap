//layerDialog.cpp
#include "layersDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QGroupBox>
#include <QCheckBox>
#include <QRadioButton>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QKeyEvent>
#include <QAccessible>


CLayersDialog::CLayersDialog(QWidget* parent)
    : QDialog(parent)
{
    // Initial UI setup (Window title, main layout, scroll area)
    setupUi();
}

CLayersDialog::~CLayersDialog()
{
    // Qt handles child widget deletion, but clean up 
    // any manual resources if necessary.
}

void CLayersDialog::setupUi()
{
    // Basic Window Properties
    setWindowTitle(tr("Layer Settings"));
    resize(400, 600); // A standard, vertical-friendly default size

    //The Root Layout
    m_pMainLayout = new QVBoxLayout(this);

    // The Scroll Area which is The container for all the GroupBoxes
    m_pScrollArea = new QScrollArea(this);
    m_pScrollArea->setWidgetResizable(true);
    m_pScrollArea->setFrameShape(QFrame::NoFrame);
    m_pScrollArea->setFocusPolicy(Qt::NoFocus);

    // The Content Widget goes inside the scroll area 
    QWidget* scrollContent = new QWidget();
    m_pScrollLayout = new QVBoxLayout(scrollContent);
    m_pScrollLayout->setAlignment(Qt::AlignTop); // Keep groups at the top

    m_pScrollArea->setWidget(scrollContent);
    m_pMainLayout->addWidget(m_pScrollArea);

    // Standard Windows Button Box for OK and cancel
    m_pButtonBox = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this );

    // Connect the built-in signals to our slots
    connect(m_pButtonBox, &QDialogButtonBox::accepted, this, &CLayersDialog::onAccept);
    connect(m_pButtonBox, &QDialogButtonBox::rejected, this, &CLayersDialog::onCancel);

    m_pMainLayout->addWidget(m_pButtonBox);
}

void CLayersDialog::setLayers(const QVector<LayerData>& data )
{
    //Clear previous state
    qDeleteAll(m_layerButtons);
    m_layerButtons.clear();

    // Clear the scroll layout (removes old GroupBoxes)
    QLayoutItem* item;
    while ((item = m_pScrollLayout->takeAt(0)) != nullptr) 
    {
        if (item->widget()) 
        {
            item->widget()->deleteLater();
        }
        delete item;
    }
    m_groupLayouts.clear();

    // 2. Iterate through the flat list
    int lastGroupId = -1;
    for (const LayerData& ld : data) 
    {
        // Check if need to start a new GroupBox
        if (ld.groupId != lastGroupId) 
        {
            createLayerGroup(ld.groupId, ld.groupName, ld.isBaseGroup != 0);
            lastGroupId = ld.groupId;
        }

        // Get the layout for the current group
        QVBoxLayout* currentBoxLayout = m_groupLayouts.value(ld.groupId);
        if (!currentBoxLayout) 
            continue;

        // Create the appropriate button type
        QAbstractButton* pButton = nullptr;
        if (ld.isBaseGroup) 
        {
            pButton = new QRadioButton(ld.layerName);
            pButton->setAutoExclusive(true);
            pButton->setProperty("groupId", ld.groupId);
            pButton->installEventFilter(this);
            connect(pButton, &QRadioButton::clicked, this, &CLayersDialog::manuallyKeepExclusive);
        }
        else 
        {
            pButton = new QCheckBox(ld.layerName);
            pButton->installEventFilter(this);
        }

        // 6. Configure the button
        pButton->setChecked(ld.isSelected);

        // Stash the Layer ID so we can find it in getPendingChanges
        pButton->setProperty("layerId", ld.layerId);

        // Add to the UI and our internal tracking map
        currentBoxLayout->addWidget(pButton);
        m_layerButtons.insert(ld.layerId, pButton);
    }

    //Set focus 
    if( !m_layerButtons.isEmpty() )
    {
        // Get the first button added to the map 
        // Set focus to first item 
       if( m_pScrollLayout->count() > 0 ) 
        {
            // Get the first GroupBox
            QGroupBox* firstBox = qobject_cast<QGroupBox*>(m_pScrollLayout->itemAt(0)->widget());
            if( firstBox && firstBox->layout() && firstBox->layout()->count() > 0) 
            {
                // Get the first checkbox/radio inside that box
                QWidget* firstInner = firstBox->layout()->itemAt(0)->widget();
                if (firstInner) 
                {
                    firstInner->setFocus();
                }
            }
        }
    }
    else 
    {
        // Fallback: If no layers exist, focus the OK button
        m_pButtonBox->button(QDialogButtonBox::Ok)->setFocus();
    }
}

void CLayersDialog::createLayerGroup(int groupId, const QString& groupName, bool isBase)
{
    // Create the GroupBox
    QGroupBox* pGroupBox = new QGroupBox(groupName, m_pScrollArea->widget());

    //Create the Layout for the buttons inside this box
    QVBoxLayout* pBoxLayout = new QVBoxLayout(pGroupBox);

    // Standard Windows margins for a clean look
    pBoxLayout->setContentsMargins(10, 15, 10, 10);
    pBoxLayout->setSpacing(5);

    //Store the layout in our map so setLayers can add buttons to it
    m_groupLayouts.insert(groupId, pBoxLayout);

    //Add the GroupBox to the main scrollable list
    m_pScrollLayout->addWidget(pGroupBox);
}

QMap<int, bool> CLayersDialog::getPendingChanges() const
{
    //Data structure based on layer id and its state 
    QMap<int, bool> changes;

    // Iterate through every button
    QMap<int, QAbstractButton*>::const_iterator i = m_layerButtons.constBegin();
    while( i != m_layerButtons.constEnd() ) 
    {
        int layerId = i.key();                 
        bool isChecked = i.value()->isChecked(); 

        changes.insert(layerId, isChecked);
        ++i;
    }

    return changes;
}

void CLayersDialog::onAccept()
{
    accept(); // Closes dialog with QDialog::Accepted
}

void CLayersDialog::onCancel()
{
    reject(); // Closes dialog with QDialog::Rejected
}

bool CLayersDialog::eventFilter(QObject* obj, QEvent* event)
{
    if (event->type() == QEvent::KeyPress) 
    {
        QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
        if (keyEvent->key() == Qt::Key_Up || keyEvent->key() == Qt::Key_Down) 
        {
            QWidget* currentWidget = qobject_cast<QWidget*>(obj);
            if (currentWidget && currentWidget->parentWidget()) 
            {
                QLayout* layout = currentWidget->parentWidget()->layout();
                if (layout) 
                {
                    int index = layout->indexOf(currentWidget);
                    int nextIndex = (keyEvent->key() == Qt::Key_Down) ? index + 1 : index - 1;

                    // If within bounds, move focus
                    if (nextIndex >= 0 && nextIndex < layout->count()) 
                    {
                        QWidget* nextWidget = layout->itemAt(nextIndex)->widget();
                        if (nextWidget) {
                            nextWidget->setFocus();
                        }
                    }
                    // If OUT of bounds, we do nothing (trapping the focus)
                    
                    return true; // Always swallow the key to prevent default jumping
                }
            }
        }
    }
    return QDialog::eventFilter(obj, event);
}

void CLayersDialog::manuallyKeepExclusive()
{
    // Identify which button was clicked
    QRadioButton* pClickedRadio = qobject_cast<QRadioButton*>(sender());
    if (!pClickedRadio) 
        return;

    // Retrieve the Group ID 
    int groupId = pClickedRadio->property("groupId").toInt();

    // Find the layout for this specific group
    QVBoxLayout* layout = m_groupLayouts.value(groupId);
    if (!layout) 
        return;

    // Manual Exclusivity Logic
    for (int i = 0; i < layout->count(); ++i)
    {
        QRadioButton* pOther = qobject_cast<QRadioButton*>(layout->itemAt(i)->widget());
        if (pOther && 
            pOther != pClickedRadio)
        {
            pOther->blockSignals(true);
            pOther->setChecked(false);
            pOther->blockSignals(false);
        }
    }

    // Ensure the clicked one stays checked
    pClickedRadio->setChecked(true);
    QAccessibleAnnouncementEvent event(pClickedRadio, pClickedRadio->text() + " checked");
    event.setPoliteness(QAccessible::AnnouncementPoliteness::Assertive);
    QAccessible::updateAccessibility(&event);
    }
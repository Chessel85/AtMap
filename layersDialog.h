//layerDialog.h
#pragma once

#include <QDialog>
#include <QVector>
#include <QMap>
#include "mapViewType.h"

// Forward declarations 
class QVBoxLayout;
class QAbstractButton;
class QDialogButtonBox;
class QScrollArea;

class CLayersDialog : public QDialog
{
    Q_OBJECT

//Constructor
public:
    explicit CLayersDialog(QWidget *parent = nullptr);
    ~CLayersDialog();

//Methods
public:
    void setLayers(const QVector<LayerData>& data);
    QMap<int, bool> getPendingChanges() const;

protected:
    bool eventFilter(QObject* obj, QEvent* event) override;

private slots:
    void onAccept();
    void onCancel();
    void manuallyKeepExclusive();

private:
    // UI Construction helpers
    void setupUi();
    void createLayerGroup(int groupId, const QString& groupName, bool isBase);


    // Main layouts
    QVBoxLayout* m_pMainLayout;
    QVBoxLayout* m_pScrollLayout;
    QScrollArea* m_pScrollArea;
    QDialogButtonBox* m_pButtonBox;

    // Data tracking
    // Maps Layer ID to the actual Button (Checkbox or Radio) in the UI
    QMap<int, QAbstractButton*> m_layerButtons;
    
    // Tracks which GroupBox belongs to which ID to avoid duplicates
    QMap<int, QVBoxLayout*> m_groupLayouts; 
};
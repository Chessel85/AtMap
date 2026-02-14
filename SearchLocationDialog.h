//SearchLocationDialog.h
#pragma once

#include <QDialog.h>
#include <QLineEdit>
#include <QLabel.h>
#include <QPushButton>
#include <QListWidget.h>
#include <QString.h>
#include <QMap.h>
#include <QGeoCoordinate.h>
#include <QKeyEvent> 

class CSearchLocationDialog : public QDialog
{
    Q_OBJECT

//Constructor
public:
    explicit CSearchLocationDialog(QWidget* parent = nullptr);

//Methods
public slots:
    void handleSearchResults(const QMap<QString, QGeoCoordinate>& results);

signals:
    void searchRequested(const QString& searchTerm);
    void locationSelected(const QGeoCoordinate& coordinates);

protected:
    void showEvent(QShowEvent* event) override; // Declare the override
    void keyPressEvent(QKeyEvent* event) override;

private:
    void onSearchButtonClicked();
    void onOkButtonClicked();
    void onSearchTextChanged(const QString& text);
    void onResultsListSelectionChanged();
    void setupUI();

//Member variables
private:
    QLabel* lblSearchText;
    QLineEdit* editSearchText;
    QPushButton* buttonSearch;
    QListWidget* listSearchResults;
    QPushButton* buttonOK;
    QPushButton* buttonCancel;
    bool m_bInitialShow;
    QMap<QString, QGeoCoordinate> m_currentResults;
};

//GotoLocationDialog.h
#pragma once

#include <QDialog.h>
#include <QLineEdit>
#include <QLabel.h>
#include <QPushButton>

class CGotoLocationDialog : public QDialog
{
    Q_OBJECT

//Constructor
public:
    explicit CGotoLocationDialog(QWidget* parent = nullptr);

//Method
public:
    double getLatitude() const;
    double getLongitude() const;
    void setLatitude(double latitude );
    void setLongitude(double longitude );


protected:
    void showEvent(QShowEvent* event) override; // Declare the override

private:
    void setupUI();

//Member variables
private:
    QLineEdit* editLatitude;
    QLabel* lblLatitude;
    QLineEdit* editLongitude;
    QLabel* lblLongitude;
    QPushButton* buttonOK;
    QPushButton* buttonCancel;
    bool m_showFirstTime;
};

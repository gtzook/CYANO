#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QDateTime>
#include <QCheckBox>
#include <QMainWindow>
#include <QPushButton>
#include <QLabel>
#include <QSlider>
#include <QTimer>
#include <QChartView>
#include <QLineSeries>
#include <QValueAxis>
#include <QtCharts>
#include <QTcpSocket>
#include <QJsonDocument>
#include <QJsonObject>
#include <QFont>
#include <map>
#include "settings_window/settings.h"
#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTimer>
#include <QString>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDialog>
#include <QMessageBox>

QT_CHARTS_USE_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

private slots:
    void updateGUI();
    void agitationDecrease();
    void agitationIncrease();
    void agitationManual();
    void brightnessMoved();
    void brightnessReleased();
    void demo();
    void blank();
    void solOn();
    void solOff();
    void sendSettings();

private:
    void makeAgitation(QWidget *parent, QBoxLayout *layout);
    void makeBrightness(QWidget *parent, QBoxLayout *layout);
    void makeText(QWidget *parent, QBoxLayout *layout);
    void makePH(QWidget *parent, QBoxLayout *layout);
    void makeOD(QWidget *parent, QBoxLayout *layout);
    void makeWarning(QWidget *parent, QBoxLayout *layout);
    void makeFonts();
    void updatePlots(double ph, double od);
    void updateText(bool isDay, int remaining);
    void updateWarning(double ph);
    void agitationSend(int val);
    std::map<QString, QJsonValue> parseJSON(QByteArray raw);
    SettingsWizard *wiz;
    QPushButton *button1;
    QPushButton *button2;
    QLineSeries *pHSeries;
    QLineSeries *pHUpper;
    QLineSeries *pHLower;
    QValueAxis *pHXAxis;
    QLineSeries *odSeries;
    QValueAxis *odXAxis;
    QTcpSocket socket;
    QLabel *brightnessLabel;
    QSlider *brightnessSlider;
    QLabel *agitationValue;
    QLabel *dayNightLabel;
    QLabel *timeSwitchLabel;
    QLabel *warningLabel;
    QCheckBox *manualCheck;
    // fonts
    QFont plotTitles;
    QFont plotLabels;
    QFont plotTicks;
    QFont buttons;
    QFont labels;
    QFont labels2;
    bool manual;
    int agi;
    double upper;
    double lower;
};

#endif // MAINWINDOW_H

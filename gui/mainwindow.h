#ifndef MAINWINDOW_H
#define MAINWINDOW_H

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
#include <map>

QT_CHARTS_USE_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

private slots:
    std::map<QString, QJsonValue> parseJSON(QByteArray raw);
    void updateGUI();
    void agitationDecrease();
    void agitationIncrease();
    void agitationSend();
    void brightnessClicked();


private:
    QPushButton *button1;
    QPushButton *button2;
    QLabel *agitationLabel;
    QLabel *agitationValue;
    QPushButton *increaseButton;
    QPushButton *decreaseButton;
    QLabel *brightnessLabel;
    QSlider *brightnessSlider;
    QPushButton *okButton;
    QLabel *dayNightLabel;
    QLabel *timeSwitchLabel;
    QLabel *timeSwitchValue;
    QLineSeries *pHSeries;
    QValueAxis *pHXAxis;
    QLineSeries *odSeries;
    QValueAxis *odXAxis;
    QTcpSocket socket;
    int agi;
};

#endif // MAINWINDOW_H

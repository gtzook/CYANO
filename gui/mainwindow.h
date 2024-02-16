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

QT_CHARTS_USE_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void updateGUI(const QString &data);

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
    QLineSeries *odSeries;
};

#endif // MAINWINDOW_H

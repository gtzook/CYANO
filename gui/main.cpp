#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QSlider>
#include <QPushButton>
#include <QChartView>
#include <QLineSeries>
#include <QValueAxis>
#include <QtCharts>

QT_CHARTS_USE_NAMESPACE

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    app.setStyle("fusion");

    // Create the main window
    QWidget mainWindow;
    mainWindow.setWindowTitle("CYANO GUI");
    mainWindow.resize(800, 600);

    // Layout for the main window
    QVBoxLayout *mainLayout = new QVBoxLayout(&mainWindow);

    // Slider section
    QLabel *sliderLabel = new QLabel("Slider:");
    mainLayout->addWidget(sliderLabel);

    QSlider *slider = new QSlider(Qt::Horizontal);
    slider->setRange(0, 100);
    slider->setValue(50);
    mainLayout->addWidget(slider);

    // Agitation Display section
    QLabel *agitationLabel = new QLabel("Agitation:");
    mainLayout->addWidget(agitationLabel);

    QLabel *agitationValue = new QLabel("0%");
    mainLayout->addWidget(agitationValue);

    QPushButton *increaseButton = new QPushButton("Increase");
    mainLayout->addWidget(increaseButton);

    QPushButton *decreaseButton = new QPushButton("Decrease");
    mainLayout->addWidget(decreaseButton);

    // Day Duration Slider
    QLabel *brightnessLabel = new QLabel("Brightness:");
    mainLayout->addWidget(brightnessLabel);

    QSlider *brightnessSlider = new QSlider(Qt::Horizontal);
    brightnessSlider->setRange(0, 100);
    brightnessSlider->setValue(50);
    mainLayout->addWidget(brightnessSlider);

    QPushButton *okButton = new QPushButton("OK");
    mainLayout->addWidget(okButton);

    // Light Timer
    QLabel *dayNightLabel = new QLabel("DAY");
    dayNightLabel->setStyleSheet("QLabel { color: yellow; }");
    mainLayout->addWidget(dayNightLabel);

    QLabel *timeSwitchLabel = new QLabel("Time to switch:");
    mainLayout->addWidget(timeSwitchLabel);

    QLabel *timeSwitchValue = new QLabel("00:00");
    mainLayout->addWidget(timeSwitchValue);

    // Create a horizontal layout for the bottom graphs
    QHBoxLayout *bottomLayout = new QHBoxLayout();

    // pH section
    QVBoxLayout *pHLayout = new QVBoxLayout();
    QLabel *phLabel = new QLabel("pH Sensor");
    pHLayout->addWidget(phLabel);

    // Chart for pH Display
    QChart *pHChart = new QChart;
    QChartView *pHChartView = new QChartView(pHChart);
    pHChartView->setRenderHint(QPainter::Antialiasing);
    pHLayout->addWidget(pHChartView);
    bottomLayout->addLayout(pHLayout);

    QLineSeries *pHSeries = new QLineSeries;
    pHChart->addSeries(pHSeries);
    pHChart->createDefaultAxes();

    QValueAxis *pHXAxis = qobject_cast<QValueAxis *>(pHChart->axes(Qt::Horizontal).at(0));
    QValueAxis *pHYAxis = qobject_cast<QValueAxis *>(pHChart->axes(Qt::Vertical).at(0));
    pHXAxis->setRange(0, 100); // Adjust range as needed
    pHYAxis->setRange(0, 14); // pH range
    pHXAxis->setTitleText("Time");
    pHYAxis->setTitleText("pH");

    // OD section
    QVBoxLayout *odLayout = new QVBoxLayout();
    QLabel *odLabel = new QLabel("Optical Density Sensor");
    odLayout->addWidget(odLabel);

    // Chart for OD Display
    QChart *odChart = new QChart;
    QChartView *odChartView = new QChartView(odChart);
    odChartView->setRenderHint(QPainter::Antialiasing);
    odLayout->addWidget(odChartView);
    bottomLayout->addLayout(odLayout);

    QLineSeries *odSeries = new QLineSeries;
    odChart->addSeries(odSeries);
    odChart->createDefaultAxes();

    QValueAxis *odXAxis = qobject_cast<QValueAxis *>(odChart->axes(Qt::Horizontal).at(0));
    QValueAxis *odYAxis = qobject_cast<QValueAxis *>(odChart->axes(Qt::Vertical).at(0));
    odXAxis->setRange(0, 100); // Adjust range as needed
    odYAxis->setRange(0, 100); // OD range
    odXAxis->setTitleText("Time");
    odYAxis->setTitleText("OD");

    mainLayout->addLayout(bottomLayout);

    mainWindow.showFullScreen();

    // Simulating pH and OD sensor data update
    QTimer *pHTimer = new QTimer();
    QObject::connect(pHTimer, &QTimer::timeout, [&]() {
        static int timeElapsed = 0;
        qreal pHValue = qrand() % 14; // Simulated pH sensor reading
        pHSeries->append(timeElapsed, pHValue);
        timeElapsed++;
    });
    pHTimer->start(1000); // Update interval in milliseconds

    QTimer *odTimer = new QTimer();
    QObject::connect(odTimer, &QTimer::timeout, [&]() {
        static int timeElapsed = 0;
        qreal odValue = qrand() % 100; // Simulated OD sensor reading
        odSeries->append(timeElapsed, odValue);
        timeElapsed++;
    });
    odTimer->start(1000); // Update interval in milliseconds

    return app.exec();
}

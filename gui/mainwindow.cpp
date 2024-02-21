#include "mainwindow.h"
#include "networking/ip_finder.cpp"
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

#define GRAPH_WIDTH 20
#define SCROLL_THRESH 15
#define DATA_INTERVAL 0.5

QT_CHARTS_USE_NAMESPACE
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{

    qApp->setStyle(QStyleFactory::create("Fusion"));

    // Create the central widget
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    // Layout for the central widget
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

    // Menu Buttons
    QPushButton *button1 = new QPushButton("Button 1", centralWidget);
    QPushButton *button2 = new QPushButton("Button 2", centralWidget);

    // Agitation Display section
    QLabel *agitationLabel = new QLabel("Agitation:", centralWidget);
    agitationValue = new QLabel("100%", centralWidget);
    agi = 100;
    QPushButton *increaseButton = new QPushButton("Increase", centralWidget);
    QPushButton *decreaseButton = new QPushButton("Decrease", centralWidget);
    connect(increaseButton, &QPushButton::pressed, this, &MainWindow::agitationIncrease);
    connect(decreaseButton, &QPushButton::pressed, this, &MainWindow::agitationDecrease);

    // Day Duration Slider
    QLabel *brightnessLabel = new QLabel("Brightness:", centralWidget);
    brightnessSlider = new QSlider(Qt::Horizontal, centralWidget);
    brightnessSlider->setRange(0, 100);
    brightnessSlider->setValue(50);
    brightnessSlider->setFixedHeight(200); // Adjust the height of the slider box
    brightnessSlider->setStyleSheet(
        "QSlider::groove:horizontal {"
        "    border: 1px solid #bbb;"
        "    background: white;"
        "    height: 10px;"
        "    border-radius: 4px;"
        "}"
        "QSlider::sub-page:horizontal {"
        "    background: qlineargradient(x1: 0, y1: 0,    x2: 0, y2: 1,"
        "        stop: 0 #66e, stop: 1 #bbf);"
        "    background: qlineargradient(x1: 0, y1: 0.2, x2: 1, y2: 1,"
        "        stop: 0 #bbf, stop: 1 #55f);"
        "    border: 1px solid #777;"
        "    height: 10px;"
        "    border-radius: 4px;"
        "}"
        "QSlider::add-page:horizontal {"
        "    background: #fff;"
        "    border: 1px solid #777;"
        "    height: 10px;"
        "    border-radius: 4px;"
        "}"
        "QSlider::handle:horizontal {"
        "    background: qlineargradient(x1:0, y1:0, x2:1, y2:1,"
        "        stop:0 #eee, stop:1 #ccc);"
        "    border: 1px solid #777;"
        "    width: 13px;"
        "    margin-top: -2px;"
        "    margin-bottom: -2px;"
        "    border-radius: 4px;"
        "}"
        "QSlider::handle:horizontal:hover {"
        "    background: qlineargradient(x1:0, y1:0, x2:1, y2:1,"
        "        stop:0 #fff, stop:1 #ddd);"
        "    border: 1px solid #444;"
        "    border-radius: 4px;"
        "}"
        "QSlider::sub-page:horizontal:disabled {"
        "    background: #bbb;"
        "    border-color: #999;"
        "}"
        "QSlider::add-page:horizontal:disabled {"
        "    background: #eee;"
        "    border-color: #999;"
        "}"
        "QSlider::handle:horizontal:disabled {"
        "    background: #eee;"
        "    border: 1px solid #aaa;"
        "    border-radius: 4px;"
        "}");

    QPushButton *okButton = new QPushButton("OK", centralWidget);
    connect(okButton, &QPushButton::pressed, this, &MainWindow::brightnessClicked);

    // Light Timer
    QLabel *dayNightLabel = new QLabel("DAY", centralWidget);
    dayNightLabel->setStyleSheet("QLabel { color: yellow; }");
    QLabel *timeSwitchLabel = new QLabel("Time to switch:", centralWidget);
    QLabel *timeSwitchValue = new QLabel("00:00", centralWidget);

    // IP Display
    QLabel *ipLabel = new QLabel("", centralWidget);
    ipLabel->setText(QString::fromStdString("IP: " + get_ip()));

    // Create layouts for the bottom graphs
    QHBoxLayout *bottomLayout = new QHBoxLayout();

    // pH section
    QVBoxLayout *pHLayout = new QVBoxLayout();
    QLabel *phLabel = new QLabel("pH Sensor", centralWidget);
    QChart *pHChart = new QChart;
    QChartView *pHChartView = new QChartView(pHChart, centralWidget);
    pHSeries = new QLineSeries;
    pHChart->addSeries(pHSeries);
    // graph cosmetics
    QPen phpen = pHSeries->pen();
    phpen.setWidth(7);
    phpen.setColor(Qt::red);
    pHSeries->setPen(phpen);

    pHChart->createDefaultAxes();
    pHXAxis = qobject_cast<QValueAxis *>(pHChart->axes(Qt::Horizontal).at(0));
    QValueAxis *pHYAxis = qobject_cast<QValueAxis *>(pHChart->axes(Qt::Vertical).at(0));
    pHXAxis->setRange(0, GRAPH_WIDTH);
    pHYAxis->setRange(0, 14);
    pHXAxis->setTitleText("Time");
    pHYAxis->setTitleText("pH");

    pHChartView->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    pHLayout->addWidget(phLabel);
    pHLayout->addWidget(pHChartView);
    bottomLayout->addLayout(pHLayout, .2);
    // OD section
    QVBoxLayout *odLayout = new QVBoxLayout();
    QLabel *odLabel = new QLabel("Optical Density Sensor", centralWidget);
    QChart *odChart = new QChart;
    QChartView *odChartView = new QChartView(odChart, centralWidget);
    odSeries = new QLineSeries;
    odChart->addSeries(odSeries);
    // graph cosmetics
    QPen odpen = odSeries->pen();
    odpen.setWidth(7);
    odpen.setColor(Qt::green);
    odSeries->setPen(odpen);

    odChart->createDefaultAxes();
    odXAxis = qobject_cast<QValueAxis *>(odChart->axes(Qt::Horizontal).at(0));
    QValueAxis *odYAxis = qobject_cast<QValueAxis *>(odChart->axes(Qt::Vertical).at(0));
    odXAxis->setRange(0, GRAPH_WIDTH);
    odYAxis->setRange(0, 100);
    odXAxis->setTitleText("Time");
    odYAxis->setTitleText("OD");
    odChartView->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    odLayout->addWidget(odLabel);
    odLayout->addWidget(odChartView);
    bottomLayout->addLayout(odLayout, .2);

    // Create client interface
    socket.connectToHost("127.0.0.1", 12345);
    // Connect socket signals to slots
    connect(&socket, &QTcpSocket::readyRead, this, &MainWindow::updateGUI);

    // Add widgets to the main layout
    mainLayout->addWidget(button1);
    mainLayout->addWidget(button2);
    mainLayout->addWidget(agitationLabel);
    mainLayout->addWidget(agitationValue);
    mainLayout->addWidget(increaseButton);
    mainLayout->addWidget(decreaseButton);
    mainLayout->addWidget(brightnessLabel);
    mainLayout->addWidget(brightnessSlider);
    mainLayout->addWidget(okButton);
    mainLayout->addWidget(dayNightLabel);
    mainLayout->addWidget(ipLabel);
    mainLayout->addWidget(timeSwitchLabel);
    mainLayout->addWidget(timeSwitchValue);
    mainLayout->addLayout(bottomLayout);
}

void MainWindow::updateGUI()
{
    while (socket.bytesAvailable() > 0)
    {
        QByteArray rawData = socket.readAll();
        // Parse the JSON data
        std::map<QString, QJsonValue> data = parseJSON(rawData);
        if (data["success"].toBool())
        {
            auto count = pHSeries->count(); // number of existing points
            auto farthest_x = pHSeries->at(count - 1).x();

            // new values to append
            auto ph = data["ph"].toDouble();
            auto od = data["od"].toDouble();

            auto pos = 0.0;
            if (count > 0)
            {
                pos = farthest_x + DATA_INTERVAL; // one further than highest
            }
            // Add to line series
            pHSeries->append(pos, ph);
            odSeries->append(pos, od);

            // Scroll graphs
            if (farthest_x > SCROLL_THRESH)
            {
                odXAxis->setMax(odXAxis->max() + DATA_INTERVAL);
                odXAxis->setMin(odXAxis->min() + DATA_INTERVAL);
                odSeries->remove(0);

                pHXAxis->setMax(pHXAxis->max() + DATA_INTERVAL);
                pHXAxis->setMin(pHXAxis->min() + DATA_INTERVAL);
                pHSeries->remove(0);
            }
        }
    }
}

std::map<QString, QJsonValue> MainWindow::parseJSON(QByteArray raw)
{
    QJsonDocument doc = QJsonDocument::fromJson(raw);
    std::map<QString, QJsonValue> ret; // return map
    QJsonValue success = false;
    if (!doc.isNull())
    {
        if (doc.isObject())
        {
            QJsonObject jsonObj = doc.object();
            // Iterate through JSON
            foreach (QString key, jsonObj.keys())
            {
                QJsonValue value = jsonObj.value(key);
                // Add JSON things to map
                ret.insert({key, value});
            }
            success = true;
        }
        else
        {
            qWarning() << "JSON data is not an object.";
        }
    }
    else
    {
        qWarning() << "Failed to parse JSON data.";
    }
    ret.insert({"success", success});
    return ret;
}

void MainWindow::agitationDecrease()
{
    if (agi > 0)
    {
        agi -= 10;
    }
    agitationSend();
}
void MainWindow::agitationIncrease()
{
    if (agi < 100)
    {
        agi += 10;
    }
    agitationSend();
}
void MainWindow::agitationSend()
{
    QString s = QString("%1\%").arg(agi);
    agitationValue->setText(s);
    s = QString("a%1").arg(agi);
    socket.write(s.toUtf8());
}
void MainWindow::brightnessClicked()
{
    QString s = QString("b%1").arg(brightnessSlider->value());
    socket.write(s.toUtf8());
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MainWindow w;
    w.showFullScreen();
    return app.exec();
}

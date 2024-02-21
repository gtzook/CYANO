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

#define GRAPH_WIDTH 200
#define SCROLL_THRESH 15
#define DATA_INTERVAL 0.5

QT_CHARTS_USE_NAMESPACE
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    qApp->setStyle(QStyleFactory::create("Fusion"));
    makeFonts();
    // Create the central widget
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    // Layout for the central widget
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->addSpacing(20);

    // Create layouts for the bottom graphs
    const int plotSpacer = 30;
    QHBoxLayout *plotLayout = new QHBoxLayout();
    plotLayout->addSpacing(plotSpacer);
    makePH(centralWidget, plotLayout);
    plotLayout->addSpacing(plotSpacer);
    makeOD(centralWidget, plotLayout);
    plotLayout->addSpacing(plotSpacer);
    mainLayout->addLayout(plotLayout);

    QHBoxLayout *lineLayout = new QHBoxLayout();
    
    lineLayout->addSpacing(20);
    makeAgitation(centralWidget, lineLayout);
    lineLayout->addSpacing(290);
    makeText(centralWidget, lineLayout);
    makeBrightness(centralWidget, lineLayout);
    lineLayout->addSpacing(20);
    mainLayout->addLayout(lineLayout);

    // Menu Buttons
    QPushButton *button1 = new QPushButton("Parameters", centralWidget);
    mainLayout->addWidget(button1);

    mainLayout->addSpacing(150);

    // IP Display
    QLabel *ipLabel = new QLabel("", parent);
    ipLabel->setText(QString::fromStdString("IP: " + get_ip()));
    mainLayout->addWidget(ipLabel);
    mainLayout->addSpacing(10);
    // Create client interface
    socket.connectToHost("127.0.0.1", 12345);
    // Connect socket signals to slots
    connect(&socket, &QTcpSocket::readyRead, this, &MainWindow::updateGUI);

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
            updatePlots(data["ph"].toDouble(),data["od"].toDouble());
            updateText(data["state"].toBool(),(int)(data["remaining"].toDouble() + 0.5));
        }
    }
}
void MainWindow::updatePlots(double ph, double od){
    auto count = pHSeries->count(); // number of existing points
    auto farthest_x = pHSeries->at(count - 1).x();

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
void MainWindow::updateText(bool isDay, int remaining){
    if (isDay){
        dayNightLabel->setStyleSheet("QLabel { color: #edd000; }");
        dayNightLabel->setText("DAY");
    }
    else{
        dayNightLabel->setStyleSheet("QLabel { color: #054170; }");
        dayNightLabel->setText("NIGHT");        
    }
    auto minutes = remaining / 60;
    auto hours = minutes / 60;
    auto seconds = remaining - (minutes * 60);
    minutes -= 60 * hours;
    QString h = QString("%1").arg(hours), 
            m = QString("%1").arg(minutes), 
            s = QString("%1").arg(seconds);
    h = (hours < 10) ? '0' + h : h;
    m = (minutes < 10) ? '0' + m : m;
    s = (seconds < 10) ? '0' + s : s;
    QString ts = "Time to switch: " +
                    h + ':' + m + ':' + s;
    timeSwitchLabel->setText(ts);
}
void MainWindow::makeFonts(){
    plotTitles = QFont("Papyrus", 30, 50);
    plotLabels = QFont("Papyrus", 15, 100);
    plotTicks = QFont("Papyrus", 10, 20);
    labels = QFont("Papyrus", 20, 30);
}
void MainWindow::makeOD(QWidget *parent, QBoxLayout *layout){
    // OD section
    QChart *odChart = new QChart;
    odChart->legend()->hide();
    odChart->setTitle("Optical Density");
    odChart->setTitleFont(plotTitles);
    QChartView *odChartView = new QChartView(odChart, parent);
    odSeries = new QLineSeries;
    odChart->addSeries(odSeries);
    odChart->createDefaultAxes();

    // make green
    QPen odpen = odSeries->pen();
    odpen.setWidth(7);
    odpen.setColor(Qt::green);
    odSeries->setPen(odpen);

    odXAxis = qobject_cast<QValueAxis *>(odChart->axes(Qt::Horizontal).at(0));
    QValueAxis *odYAxis = qobject_cast<QValueAxis *>(odChart->axes(Qt::Vertical).at(0));
        
    odXAxis->setRange(0, GRAPH_WIDTH);
    odYAxis->setRange(0, 100);    
    odXAxis->setTitleText("Time");    
    odXAxis->setLabelsFont(plotTicks);
    odXAxis->setTitleFont(plotLabels);
    odYAxis->setTitleText("OD");
    odYAxis->setTitleFont(plotLabels);
    odYAxis->setLabelsFont(plotTicks);
    
    odChartView->setFixedHeight(600);

    layout->addWidget(odChartView);
}
void MainWindow::makePH(QWidget *parent, QBoxLayout *layout){
    // pH section
    QChart *pHChart = new QChart;
    pHChart->legend()->hide();
    pHChart->setTitle("pH");
    pHChart->setTitleFont(plotTitles);
    QChartView *pHChartView = new QChartView(pHChart, parent);
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
    pHXAxis->setTitleFont(plotLabels);
    pHXAxis->setLabelsFont(plotTicks);
    pHYAxis->setTitleText("pH");
    pHYAxis->setTitleFont(plotLabels);
    pHYAxis->setLabelsFont(plotTicks);

    pHChartView->setFixedHeight(600);

    layout->addWidget(pHChartView);
}
void MainWindow::makeText(QWidget *parent, QBoxLayout *layout){
    QVBoxLayout *timeText = new QVBoxLayout();
    timeText->setSpacing(12);
    // Light Timer
    dayNightLabel = new QLabel("DAY", parent);
    dayNightLabel->setStyleSheet("QLabel { color: #edd000; }");
    dayNightLabel->setFont(labels);
    timeSwitchLabel = new QLabel("Time to switch: 00:00", parent);
    timeSwitchLabel->setFont(labels);
    // Add widgets to the main layout
    timeText->setAlignment(Qt::AlignCenter);
    timeText->addWidget(dayNightLabel);
    timeText->addWidget(timeSwitchLabel);

    layout->addLayout(timeText);
}
void MainWindow::makeBrightness(QWidget *parent, QBoxLayout *layout){
    // Day Duration Slider
    brightnessLabel = new QLabel("Brightness: 50%", parent);
    brightnessLabel->setFont(labels);
    brightnessSlider = new QSlider(Qt::Horizontal, parent);
    brightnessSlider->setRange(0, 100);
    brightnessSlider->setValue(50);
    brightnessSlider->setFixedHeight(50); // Adjust the height of the slider box
    brightnessSlider->setStyleSheet(
        "QSlider::groove:horizontal {"
        "    border: 1px solid #bbb;"
        "    background: white;"
        "    height: 100px;"
        "    border-radius: 4px;"
        "}"
        "QSlider::sub-page:horizontal {"
        "    background: qlineargradient(x1: 0, y1: 0.2, x2: 1, y2: 1,"
        "        stop: 0 #d9ead3, stop: 1 #109210);"
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
        "    width: 50px;"
        "    margin-top: -2px;"
        "    margin-bottom: -2px;"
        "    border-radius: 200px;"
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
    connect(brightnessSlider, &QSlider::sliderMoved, this, &MainWindow::brightnessMoved);
    connect(brightnessSlider, &QSlider::sliderReleased, this, &MainWindow::brightnessReleased);
    QVBoxLayout *sLayout = new QVBoxLayout();
    sLayout->addWidget(brightnessLabel);
    sLayout->setAlignment(brightnessLabel, Qt::AlignHCenter);
    sLayout->addWidget(brightnessSlider);
    layout->addLayout(sLayout);
}
void MainWindow::makeAgitation(QWidget *parent, QBoxLayout *layout){
    // Agitation Display section
    QLabel *agitationLabel = new QLabel("Agitation: ", parent);
    agitationLabel->setFont(labels);
    agitationValue = new QLabel("100%", parent);
    agitationValue->setFont(labels);
    agitationValue->setFixedWidth(agitationValue->width());
    agi = 100;
    QPushButton *increaseButton = new QPushButton("Increase", parent);
    QPushButton *decreaseButton = new QPushButton("Decrease", parent);
    connect(increaseButton, &QPushButton::pressed, this, &MainWindow::agitationIncrease);
    connect(decreaseButton, &QPushButton::pressed, this, &MainWindow::agitationDecrease);
    
    increaseButton->setFont(labels);
    decreaseButton->setFont(labels);
    increaseButton->setFixedSize(200,100);
    decreaseButton->setFixedSize(200,100);

    QVBoxLayout *butLayout = new QVBoxLayout();
    QHBoxLayout *agiLayout = new QHBoxLayout();
    butLayout->addWidget(increaseButton);
    butLayout->addWidget(decreaseButton);

    agiLayout->setSpacing(0);
    agiLayout->addLayout(butLayout);
    agiLayout->addSpacing(30);
    agiLayout->addWidget(agitationLabel);
    agiLayout->addWidget(agitationValue);
    agiLayout->setAlignment(Qt::AlignLeft);
    // Add to layout
    layout->addLayout(agiLayout);
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
void MainWindow::brightnessMoved()
{
    int value = brightnessSlider->value();
    QString label = QString("Brightness: %1\%").arg(value);
    brightnessLabel->setText(label);
}
void MainWindow::brightnessReleased()
{
    int value = brightnessSlider->value();
    QString s = QString("b%1").arg(value);
    socket.write(s.toUtf8());
}
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MainWindow w;
    w.showFullScreen();
    return app.exec();
}

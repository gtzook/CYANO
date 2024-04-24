#include "mainwindow.h"
#include "networking/ip_finder.cpp"

#define GRAPH_WIDTH 200
#define SCROLL_THRESH 150
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
    // IP Display
    QLabel *ipLabel = new QLabel("", parent);
    ipLabel->setText(QString::fromStdString("IP: " + get_ip()));
    mainLayout->addWidget(ipLabel);
    mainLayout->addSpacing(10);

    // Create layouts for the bottom graphs
    const int plotSpacer = 30;
    QHBoxLayout *plotLayout = new QHBoxLayout();
    plotLayout->addSpacing(plotSpacer);
    makeOD(centralWidget, plotLayout);
    plotLayout->addSpacing(plotSpacer);
    makePH(centralWidget, plotLayout);
    plotLayout->addSpacing(plotSpacer);
    mainLayout->addLayout(plotLayout);
    mainLayout->addSpacing(10);

    QHBoxLayout *lineLayout = new QHBoxLayout();

    lineLayout->addSpacing(20);
    makeAgitation(centralWidget, lineLayout);
    lineLayout->addSpacing(290);
    makeText(centralWidget, lineLayout);
    lineLayout->addSpacing(150);
    makeBrightness(centralWidget, lineLayout);
    lineLayout->addSpacing(25);
    lineLayout->addStretch();
    mainLayout->addLayout(lineLayout);

    mainLayout->addSpacing(100);

    // Menu Buttons
    QHBoxLayout *menuButtons = new QHBoxLayout();

    QPushButton *demoButton = new QPushButton("Demo", centralWidget);
    demoButton->setFixedSize(200, 100);
    demoButton->setFont(labels);
    QPushButton *blankButton = new QPushButton("Blank OD", centralWidget);
    blankButton->setFixedSize(200, 100);
    blankButton->setFont(labels);
    QPushButton *solenoidOff = new QPushButton("Solenoid OFF", centralWidget);
    solenoidOff->setFixedSize(200, 100);
    solenoidOff->setFont(labels);
    QPushButton *solenoidOn = new QPushButton("Solenoid ON", centralWidget);
    solenoidOn->setFixedSize(200, 100);
    solenoidOn->setFont(labels);

    connect(demoButton, &QPushButton::pressed, this, &MainWindow::demo);

    connect(blankButton, &QPushButton::pressed, this, &MainWindow::blank);

    connect(solenoidOff, &QPushButton::pressed, this, &MainWindow::solOff);

    connect(solenoidOn, &QPushButton::pressed, this, &MainWindow::solOn);

    menuButtons->addWidget(demoButton);
    menuButtons->addWidget(blankButton);
    menuButtons->addWidget(solenoidOff);
    menuButtons->addWidget(solenoidOn);
    menuButtons->addSpacing(600);
    makeWarning(centralWidget, menuButtons);
    menuButtons->addStretch();
    mainLayout->addLayout(menuButtons);

    showFullScreen(); // make window fullscreen

    // Start settings wizard
    wiz = new SettingsWizard(this);
    wiz->show();
    connect(wiz, &QDialog::finished, this, &MainWindow::sendSettings);
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
            updatePlots(data["ph"].toDouble(), data["od"].toDouble());
            updateWarning(data["ph"].toDouble());
            updateText(data["state"].toBool(), (int)(data["remaining"].toDouble() + 0.5));
        }
    }
}
void MainWindow::updatePlots(double ph, double od)
{
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

    pHUpper->clear();
    pHUpper->append(0, upper);
    pHUpper->append(200 + farthest_x, upper);

    pHLower->clear();
    pHLower->append(0, lower);
    pHLower->append(200 + farthest_x, lower);

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

void MainWindow::updateWarning(double ph)
{
    // Get current date and time
    QDateTime currentDateTime = QDateTime::currentDateTime();
    QString currentTime = currentDateTime.toString("MM/dd hh:mm:ss");

    if (ph > upper)
    {
        warningLabel->setText("<font color='red'>pH exceeded upper threshold at " + currentTime + "!</font>");
    }
    else if (ph < lower)
    {
        warningLabel->setText("pH exceeded lower threshold at " + currentTime + "!");
    }
}


void MainWindow::updateText(bool isDay, int remaining)
{
    if (isDay)
    {
        dayNightLabel->setStyleSheet("QLabel { color: #edd000; }");
        dayNightLabel->setText("DAY");
    }
    else
    {
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
void MainWindow::makeFonts()
{
    plotTitles = QFont("Papyrus", 30, 50);
    plotLabels = QFont("Papyrus", 15, 100);
    plotTicks = QFont("Papyrus", 10, 20);
    labels = QFont("Papyrus", 20, 30);
    labels2 = QFont("Papyrus", 15, 50);
}
void MainWindow::makeWarning(QWidget *parent, QBoxLayout *layout)
{
    warningLabel = new QLabel("", parent);
    warningLabel->setStyleSheet("QLabel { color: #d50a0a; }");
    warningLabel->setFont(labels);
    layout->addWidget(warningLabel);
}
void MainWindow::makeOD(QWidget *parent, QBoxLayout *layout)
{
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
    odYAxis->setRange(0, 3);
    odXAxis->setTitleText("Time");
    odXAxis->setLabelsFont(plotTicks);
    odXAxis->setTitleFont(plotLabels);
    odYAxis->setTitleText("OD");
    odYAxis->setTitleFont(plotLabels);
    odYAxis->setLabelsFont(plotTicks);

    odChartView->setFixedHeight(600);

    layout->addWidget(odChartView);
}
void MainWindow::makePH(QWidget *parent, QBoxLayout *layout)
{
    // pH section
    QChart *pHChart = new QChart;
    pHChart->legend()->hide();
    pHChart->setTitle("pH");
    pHChart->setTitleFont(plotTitles);
    QChartView *pHChartView = new QChartView(pHChart, parent);
    pHSeries = new QLineSeries;
    pHUpper = new QLineSeries;
    pHLower = new QLineSeries;
    pHChart->addSeries(pHSeries);
    pHChart->addSeries(pHUpper);
    pHChart->addSeries(pHLower);

    // graph cosmetics
    QPen phpen = pHSeries->pen();
    phpen.setWidth(7);
    phpen.setColor(Qt::red);
    pHSeries->setPen(phpen);

    QPen limitpen = pHUpper->pen();
    limitpen.setWidth(2);
    limitpen.setColor(Qt::blue);
    limitpen.setStyle(Qt::DotLine);
    pHUpper->setPen(limitpen);
    pHLower->setPen(limitpen);

    pHChart->createDefaultAxes();
    pHXAxis = qobject_cast<QValueAxis *>(pHChart->axes(Qt::Horizontal).at(0));
    QValueAxis *pHYAxis = qobject_cast<QValueAxis *>(pHChart->axes(Qt::Vertical).at(0));
    pHXAxis->setRange(0, GRAPH_WIDTH);
    pHYAxis->setRange(5, 9);
    pHXAxis->setTitleText("Time");
    pHXAxis->setTitleFont(plotLabels);
    pHXAxis->setLabelsFont(plotTicks);
    pHYAxis->setTitleText("pH");
    pHYAxis->setTitleFont(plotLabels);
    pHYAxis->setLabelsFont(plotTicks);

    pHChartView->setFixedHeight(600);

    layout->addWidget(pHChartView);
}
void MainWindow::makeText(QWidget *parent, QBoxLayout *layout)
{
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
    timeText->addSpacing(50);
    timeText->addWidget(dayNightLabel);
    timeText->addWidget(timeSwitchLabel);

    timeText->addStretch();
    timeText->setAlignment(dayNightLabel, Qt::AlignCenter);
    timeText->setAlignment(timeSwitchLabel, Qt::AlignHCenter);
    layout->addLayout(timeText, Qt::AlignCenter);
}
void MainWindow::makeBrightness(QWidget *parent, QBoxLayout *layout)
{
    // Day Duration Slider
    brightnessLabel = new QLabel("Brightness: 50%", parent);
    brightnessLabel->setFont(labels);
    brightnessSlider = new QSlider(Qt::Horizontal, parent);
    brightnessSlider->setRange(0, 100);
    brightnessSlider->setValue(50);
    brightnessSlider->setFixedHeight(50);
    brightnessSlider->setFixedWidth(600);
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
    sLayout->addSpacing(50);
    sLayout->addWidget(brightnessLabel);
    sLayout->setAlignment(brightnessLabel, Qt::AlignHCenter);
    sLayout->addWidget(brightnessSlider);
    sLayout->setSpacing(30);
    sLayout->setContentsMargins(0, 0, 0, 0);
    sLayout->addStretch();
    layout->addLayout(sLayout);
}
void MainWindow::makeAgitation(QWidget *parent, QBoxLayout *layout)
{
    // Agitation Display section
    QLabel *agitationLabel = new QLabel("Pump: ", parent);
    agitationLabel->setFont(labels);
    agitationValue = new QLabel("100%", parent);
    agitationValue->setFont(labels);
    agitationValue->setFixedWidth(agitationValue->width());
    agi = 100;
    QLabel *switchLabel = new QLabel("Mode");
    switchLabel->setFont(labels2);
    manualCheck = new QCheckBox(parent);
    manualCheck->setTristate(true);
    manualCheck->setCheckState(Qt::Checked);
    manualCheck->setStyleSheet(
        "QCheckBox::indicator { width:150px; height: 100px;}"
        "QCheckBox::indicator::checked {image: url(/home/cyano/CYANO/gui/lib/auto.png);}"
        "QCheckBox::indicator::unchecked {image: url(/home/cyano/CYANO/gui/lib/on.png);}"
        "QCheckBox::indicator::indeterminate {image: url(/home/cyano/CYANO/gui/lib/off.png);}");
    manual = false;
    connect(manualCheck, &QCheckBox::stateChanged, this, &MainWindow::agitationManual);
    QPushButton *increaseButton = new QPushButton("Increase", parent);
    QPushButton *decreaseButton = new QPushButton("Decrease", parent);
    connect(increaseButton, &QPushButton::pressed, this, &MainWindow::agitationIncrease);
    connect(decreaseButton, &QPushButton::pressed, this, &MainWindow::agitationDecrease);

    increaseButton->setFont(labels);
    decreaseButton->setFont(labels);
    increaseButton->setFixedSize(200, 100);
    decreaseButton->setFixedSize(200, 100);

    QVBoxLayout *butLayout = new QVBoxLayout();
    QHBoxLayout *agiLayout = new QHBoxLayout();
    QVBoxLayout *labelSwitchLayout = new QVBoxLayout();
    QHBoxLayout *labelLayout = new QHBoxLayout();
    butLayout->addWidget(increaseButton);
    butLayout->addWidget(decreaseButton);
    butLayout->addStretch();

    labelLayout->addWidget(agitationLabel);
    labelLayout->addWidget(agitationValue);

    labelSwitchLayout->setSpacing(0);
    labelSwitchLayout->addLayout(labelLayout);
    labelSwitchLayout->addSpacing(30);
    labelSwitchLayout->addWidget(switchLabel);
    labelSwitchLayout->setAlignment(switchLabel, Qt::AlignHCenter);
    labelSwitchLayout->addWidget(manualCheck);
    labelSwitchLayout->setAlignment(manualCheck, Qt::AlignHCenter);
    labelSwitchLayout->addSpacing(20);
    labelSwitchLayout->addStretch(1);

    agiLayout->setSpacing(0);
    agiLayout->addLayout(butLayout);
    agiLayout->addSpacing(30);
    agiLayout->addLayout(labelSwitchLayout);
    agiLayout->setAlignment(Qt::AlignLeft);
    agiLayout->addStretch();
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
void MainWindow::agitationManual()
{
    auto state = manualCheck->checkState();
    int val = agi;
    manual = !(state == Qt::Checked);
    if (manual)
        val = state == Qt::Unchecked ? 100 : 0;
    agitationSend(val);
}
void MainWindow::agitationDecrease()
{
    if (!manual)
    {
        agi = agi > 0 ? agi - 10 : agi;
        agitationSend(agi);
    }
}
void MainWindow::agitationIncrease()
{
    if (!manual)
    {
        agi = agi < 100 ? agi + 10 : agi;
        agitationSend(agi);
    }
}
void MainWindow::agitationSend(int val)
{
    QString s = QString("%1\%").arg(val);
    agitationValue->setText(s);
    s = QString("Aa%1B").arg(val);
    socket.write(s.toUtf8());
}
void MainWindow::sendSettings()
{
    std::array<double, 2> pHLimits = wiz->getPhSettings();
    upper = pHLimits[0];
    lower = pHLimits[1];
    socket.write(wiz->getSettings());
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
    QString s = QString("Ab%1B").arg(value);
    socket.write(s.toUtf8());
}
void MainWindow::demo()
{
    QString s = QString("ApB\n");
    socket.write(s.toUtf8());
}
void MainWindow::solOn()
{
    QString s = QString("As1B\n");
    socket.write(s.toUtf8());
}
void MainWindow::solOff()
{
    QString s = QString("As0B\n");
    socket.write(s.toUtf8());
}
void MainWindow::blank()
{
    QMessageBox msgBox;
    msgBox.setWindowTitle("OD Blank");
    msgBox.setText("Really blank?");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);

    msgBox.setStyleSheet("QMessageBox {"
                         "width: 600px;"
                         "height: 600px;"
                         "}"
                         "QLabel {"
                         "font-size: 25pt;"
                         "}"
                         "QPushButton {"
                         "width: 300px;"
                         "height: 100px;"
                         "font-size: 25pt;"
                         "}");

    int ret = msgBox.exec();
    QMessageBox::StandardButton reply = static_cast<QMessageBox::StandardButton>(ret);

    if (reply == QMessageBox::Yes)
    {
        qDebug() << "Blank requested";
        QString s = QString("AxB\n");
        socket.write(s.toUtf8());
    }
}
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MainWindow w;
    return app.exec();
}

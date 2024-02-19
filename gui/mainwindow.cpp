#include "mainwindow.h"
#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTimer>
#include <QDebug>

QT_CHARTS_USE_NAMESPACE

int createSocket()
{
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
    {
        std::cerr << "Socket creation error" << std::endl;
    }
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(12345);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0)
    {
        std::cerr << "Invalid address/ Address not supported" << std::endl;
        return false;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        std::cerr << "Connection Failed" << std::endl;
        return false;
    }

    std::cerr << "Connected" << std::endl;
    return sock;
}

std::string receiveMessage(int sock)
{
    char buffer[1024] = {0};
    int valread;

    // Receive data from the server
    std::cout << "Waiting for message: " << std::endl;
    valread = read(sock, buffer, 1024);
    if (valread < 0)
    {
        std::cerr << "Read error" << std::endl;
        return "";
    }
    else if (valread == 0)
    {
        std::cerr << "Connection closed by server" << std::endl;
        return "";
    }
    else
    {
        std::cout << "Message received from server: " << buffer << std::endl;
        return std::string(buffer);
    }
}

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
    QLabel *agitationValue = new QLabel("0%", centralWidget);
    QPushButton *increaseButton = new QPushButton("Increase", centralWidget);
    QPushButton *decreaseButton = new QPushButton("Decrease", centralWidget);

    // Day Duration Slider
    QLabel *brightnessLabel = new QLabel("Brightness:", centralWidget);
    QSlider *brightnessSlider = new QSlider(Qt::Horizontal, centralWidget);
    brightnessSlider->setRange(0, 100);
    brightnessSlider->setValue(50);
    QPushButton *okButton = new QPushButton("OK", centralWidget);

    // Light Timer
    QLabel *dayNightLabel = new QLabel("DAY", centralWidget);
    dayNightLabel->setStyleSheet("QLabel { color: yellow; }");
    QLabel *timeSwitchLabel = new QLabel("Time to switch:", centralWidget);
    QLabel *timeSwitchValue = new QLabel("00:00", centralWidget);

    // Create layouts for the bottom graphs
    QVBoxLayout *bottomLayout = new QVBoxLayout();

    // pH section
    QVBoxLayout *pHLayout = new QVBoxLayout();
    QLabel *phLabel = new QLabel("pH Sensor", centralWidget);
    QChart *pHChart = new QChart;
    QChartView *pHChartView = new QChartView(pHChart, centralWidget);
    QLineSeries *pHSeries = new QLineSeries;
    pHChart->addSeries(pHSeries);
    pHChart->createDefaultAxes();
    QValueAxis *pHXAxis = qobject_cast<QValueAxis *>(pHChart->axes(Qt::Horizontal).at(0));
    QValueAxis *pHYAxis = qobject_cast<QValueAxis *>(pHChart->axes(Qt::Vertical).at(0));
    pHXAxis->setRange(0, 100);
    pHYAxis->setRange(0, 14);
    pHXAxis->setTitleText("Time");
    pHYAxis->setTitleText("pH");
    pHLayout->addWidget(phLabel);
    pHLayout->addWidget(pHChartView);
    bottomLayout->addLayout(pHLayout);

    // OD section
    QVBoxLayout *odLayout = new QVBoxLayout();
    QLabel *odLabel = new QLabel("Optical Density Sensor", centralWidget);
    QChart *odChart = new QChart;
    QChartView *odChartView = new QChartView(odChart, centralWidget);
    QLineSeries *odSeries = new QLineSeries;
    odChart->addSeries(odSeries);
    odChart->createDefaultAxes();
    QValueAxis *odXAxis = qobject_cast<QValueAxis *>(odChart->axes(Qt::Horizontal).at(0));
    QValueAxis *odYAxis = qobject_cast<QValueAxis *>(odChart->axes(Qt::Vertical).at(0));
    odXAxis->setRange(0, 100);
    odYAxis->setRange(0, 100);
    odXAxis->setTitleText("Time");
    odYAxis->setTitleText("OD");
    odLayout->addWidget(odLabel);
    odLayout->addWidget(odChartView);
    bottomLayout->addLayout(odLayout);

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
    mainLayout->addWidget(timeSwitchLabel);
    mainLayout->addWidget(timeSwitchValue);
    mainLayout->addLayout(bottomLayout);

    // Show the window
    showFullScreen();
}

void MainWindow::updateGUI(const QString &data)
{
    // Update GUI with received data
    // For demonstration, let's just print the received data
    //qDebug() << "Received data: " << data;
}


int main(int argc, char *argv[])
{
    //int sock = createSocket();
    QApplication app(argc, argv);
    MainWindow w;
    w.show();
    w.showFullScreen();
    std::cerr << "here4" << std::endl;
    return app.exec();
}

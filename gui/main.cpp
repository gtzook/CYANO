#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
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
#include <QThread>
#include <QObject>

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


class WorkerThread : public QObject {
    Q_OBJECT
public:
    explicit WorkerThread(QObject *parent = nullptr) : QObject(parent) {}

public slots:
    void run() {
        while(true) {
            // Receive data from the socket
            std::string data = receiveMessage(socket);
            
            // Emit signal with the received data
            emit dataReceived(QString::fromStdString(data));
        }
    }

signals:
    void dataReceived(QString data);

private:
    int socket;
};

int main(int argc, char *argv[])
{
    int sock = createSocket();
    
    QApplication app(argc, argv);

    app.setStyle("fusion");

    // Create the main window
    QWidget mainWindow;
    mainWindow.setWindowTitle("CYANO GUI");
    mainWindow.resize(800, 600);

    // Layout for the main window
    QVBoxLayout *mainLayout = new QVBoxLayout(&mainWindow);

    //top buttons
    QPushButton *button1 = new QPushButton("Button 1");
    button1->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    button1->setFont(QFont("Arial", 16));
    mainLayout->addWidget(button1);

    QPushButton *button2 = new QPushButton("Button 2");
    button2->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    button2->setFont(QFont("Arial", 16));
    mainLayout->addWidget(button2);

    // Agitation Display section
    QLabel *agitationLabel = new QLabel("Agitation:");
    agitationLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    agitationLabel->setFont(QFont("Arial", 16));
    agitationLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(agitationLabel);

    QLabel *agitationValue = new QLabel("0%");
    agitationValue->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    agitationValue->setFont(QFont("Arial", 16));
    agitationValue->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(agitationValue);

    QPushButton *increaseButton = new QPushButton("Increase");
    increaseButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    increaseButton->setFont(QFont("Arial", 16));
    mainLayout->addWidget(increaseButton);

    QPushButton *decreaseButton = new QPushButton("Decrease");
    decreaseButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    decreaseButton->setFont(QFont("Arial", 16));
    mainLayout->addWidget(decreaseButton);

    // Day Duration Slider
    QLabel *brightnessLabel = new QLabel("Brightness:");
    brightnessLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    brightnessLabel->setFont(QFont("Arial", 16));
    brightnessLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(brightnessLabel);

    QSlider *brightnessSlider = new QSlider(Qt::Horizontal);
    brightnessSlider->setRange(0, 100);
    brightnessSlider->setValue(50);
    brightnessSlider->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    mainLayout->addWidget(brightnessSlider);

    QPushButton *okButton = new QPushButton("OK");
    okButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    okButton->setFont(QFont("Arial", 16));
    mainLayout->addWidget(okButton);

    // Light Timer
    QLabel *dayNightLabel = new QLabel("DAY");
    dayNightLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    dayNightLabel->setFont(QFont("Arial", 16));
    dayNightLabel->setAlignment(Qt::AlignCenter);
    dayNightLabel->setStyleSheet("QLabel { color: yellow; }");
    mainLayout->addWidget(dayNightLabel);

    QLabel *timeSwitchLabel = new QLabel("Time to switch:");
    timeSwitchLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    timeSwitchLabel->setFont(QFont("Arial", 16));
    timeSwitchLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(timeSwitchLabel);

    QLabel *timeSwitchValue = new QLabel("00:00");
    timeSwitchValue->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    timeSwitchValue->setFont(QFont("Arial", 16));
    timeSwitchValue->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(timeSwitchValue);

    // Create a horizontal layout for the bottom graphs
    QVBoxLayout *bottomLayout = new QVBoxLayout();

    // pH section
    QVBoxLayout *pHLayout = new QVBoxLayout();
    QLabel *phLabel = new QLabel("pH Sensor");
    phLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    phLabel->setFont(QFont("Arial", 16));
    phLabel->setAlignment(Qt::AlignCenter);
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
    pHYAxis->setRange(0, 14);   // pH range
    pHXAxis->setTitleText("Time");
    pHYAxis->setTitleText("pH");

    // OD section
    QVBoxLayout *odLayout = new QVBoxLayout();
    QLabel *odLabel = new QLabel("Optical Density Sensor");
    odLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    odLabel->setFont(QFont("Arial", 16));
    odLabel->setAlignment(Qt::AlignCenter);
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
    odYAxis->setRange(0, 100);  // OD range
    odXAxis->setTitleText("Time");
    odYAxis->setTitleText("OD");

    mainLayout->addLayout(bottomLayout);

    mainWindow.showFullScreen();

    // Simulating pH and OD sensor data update
    QTimer *pHTimer = new QTimer();
    QObject::connect(pHTimer, &QTimer::timeout, [&]()
                     {
                         static int timeElapsed = 0;
                         qreal pHValue = qrand() % 14; // Simulated pH sensor reading
                         pHSeries->append(timeElapsed, pHValue);
                         timeElapsed++;
                     });
    pHTimer->start(1000); // Update interval in milliseconds

    QTimer *odTimer = new QTimer();
    QObject::connect(odTimer, &QTimer::timeout, [&]()
                     {
                         static int timeElapsed = 0;
                         qreal odValue = qrand() % 100; // Simulated OD sensor reading
                         odSeries->append(timeElapsed, odValue);
                         timeElapsed++;
                     });
    odTimer->start(1000); // Update interval in milliseconds

    app.exec();
}

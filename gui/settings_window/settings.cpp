#include "settings.h"

SettingsWizard::SettingsWizard(QWidget *parent) : QWizard(parent)
{
    setStyleSheet("QAbstractButton { height: 100px; width: 200px; font-size: 30pt}"
                  "QLabel{font-size: 35pt;}");
    // take out unneeded button
    setOption(QWizard::NoCancelButton, true);
    addPage(new IntroPage);
    showFullScreen();
}

IntroPage::IntroPage(QWidget *parent)
    : QWizardPage(parent)
{
    setTitle(tr("CYANO Bioreactor Growth Settings"));

    QHBoxLayout *main = new QHBoxLayout();  

    main->addSpacing(100);
    makeCycles(main);
    main->addSpacing(600);
    makeAgi(main);
    main->addStretch(); //5 - 9

    setLayout(main);
}

std::array<double, 2> SettingsWizard::getPhSettings(){
    double ph_upper = field("ph_upper").toDouble()/100.0;
    double ph_lower = field("ph_lower").toDouble()/100.0;

    std::array<double, 2> arr = {ph_upper, ph_lower};

    return arr;
}

QByteArray SettingsWizard::getSettings(){
    int daySel = field("day").toInt();
    int nightSel = field("night").toInt();
    int freqSel = field("agi_freq").toInt();
    double ph_upper = field("ph_upper").toDouble()/100.0;
    double ph_lower = field("ph_lower").toDouble()/100.0;
    
    QJsonObject settings;
    settings["day"] = getTimeStr(daySel);
    settings["night"] = getTimeStr(nightSel);
    settings["agi_freq"] = freqSel;
    settings["ph_upper"] = ph_upper;
    settings["ph_lower"] = ph_lower;

    return QJsonDocument(settings).toJson();
}

void IntroPage::makeCycles(QBoxLayout *layout){
    QVBoxLayout *cycleLayout = new QVBoxLayout();
    
    QList<QString> times = makeTimes();

    QFont comboBoxFont = QFont("Papyrus", 20, 30);

    // Init objects for day
    QLabel *dayBoxLabel = new QLabel("Daytime:");
    QComboBox *day = new QComboBox;
    day->setFont(comboBoxFont);
    day->addItems(times);
    day->setFixedWidth(400);
    day->view()->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    day->setCurrentIndex(8);

    // Init objects for night
    QLabel *nightBoxLabel = new QLabel("Nighttime:");
    QComboBox *night= new QComboBox;
    night->setFont(comboBoxFont);
    night->setFixedWidth(400);
    night->addItems(times);
    night->view()->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    night->setCurrentIndex(20);

    // Add to layout
    cycleLayout->addSpacing(200);
    cycleLayout->addWidget(dayBoxLabel);
    cycleLayout->addWidget(day, Qt::AlignHCenter);
    cycleLayout->addSpacing(200);
    cycleLayout->addWidget(nightBoxLabel);
    cycleLayout->addWidget(night, Qt::AlignHCenter);
    cycleLayout->addStretch();

    registerField("day", day);
    registerField("night", night);

    layout->addLayout(cycleLayout);
}
void IntroPage::makeAgi(QBoxLayout *layout){
    QVBoxLayout *agiLayout = new QVBoxLayout();
    
    QList<QString> agis = makeAgis();

    QFont comboBoxFont = QFont("Papyrus", 20, 30);

    // Init objects for day
    QLabel *agiLabel = new QLabel("Agitate every:");
    QComboBox *agiFreq = new QComboBox;
    agiFreq->setFont(comboBoxFont);
    agiFreq->addItems(agis);
    agiFreq->setFixedWidth(400);

    // Add to layout
    agiLayout->addSpacing(200);
    agiLayout->addWidget(agiLabel);
    agiLayout->addWidget(agiFreq, Qt::AlignHCenter);
    agiLayout->addStretch();
    makePh(agiLayout);
    registerField("agi_freq", agiFreq);

    layout->addLayout(agiLayout);
}

void IntroPage::makePh(QBoxLayout *layout){
    QVBoxLayout *phLayout = new QVBoxLayout();
    
    // Init objects for day
    upperLabel = new QLabel("Upper Bound:");
    upper = new QSlider(Qt::Horizontal);
    upper->setRange(700, 900); // Range from 5.0 to 9.0
    upper->setValue(800);
    upper->setSingleStep(1); // Single step
    upper->setPageStep(1); // Page step
    upper->setTickInterval(1); // Tick interval
    upper->setFixedHeight(50);
    upper->setFixedWidth(600);
    upper->setTickPosition(QSlider::TicksBelow);
    upper->setStyleSheet(
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

    connect(upper, &QSlider::valueChanged, this, &IntroPage::updateUpper);
    updateUpper();
    //Lower begins here
    lowerLabel = new QLabel("Lower Bound:");
    lower = new QSlider(Qt::Horizontal);
    lower->setRange(500, 700);
    lower->setValue(600);
    lower->setSingleStep(1); // Single step
    lower->setPageStep(1); // Page step
    lower->setTickInterval(1); // Tick interval
    lower->setFixedHeight(50);
    lower->setFixedWidth(600);
    lower->setTickPosition(QSlider::TicksBelow);
    lower->setStyleSheet(
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

    connect(lower, &QSlider::valueChanged, this, &IntroPage::updateLower);
    updateLower();

    // Add to layout
    phLayout->addWidget(upperLabel);
    phLayout->addWidget(upper);
    phLayout->addWidget(lowerLabel);
    phLayout->addWidget(lower);
    phLayout->addStretch();

    registerField("ph_lower", lower);
    registerField("ph_upper", upper);

    layout->addLayout(phLayout);
}

void IntroPage::updateUpper(){
     double value = upper->value()/100.0;
     QString s = QString("Upper limit: %1 pH").arg(value, 3, 'f', 2);
     upperLabel->setText(s);
}
void IntroPage::updateLower(){
     double value = lower->value()/100.0;
     QString s = QString("Lower limit: %1 pH").arg(value, 3, 'f', 2);
     lowerLabel->setText(s);
}

QList<QString> IntroPage::makeAgis(){
    QList<QString> agis;
    agis.append("Always");
    agis.append("1 minute");
    agis.append("5 minutes");
    agis.append("10 minutes");
    agis.append("30 minutes");
    agis.append("1 hour");
    return agis;
}

QList<QString> IntroPage::makeTimes(){
    QList<QString> times;
    times.append("1:00 AM");
    times.append("2:00 AM");
    times.append("3:00 AM");
    times.append("4:00 AM");
    times.append("5:00 AM");
    times.append("6:00 AM");
    times.append("7:00 AM");
    times.append("8:00 AM");
    times.append("9:00 AM");
    times.append("10:00 AM");
    times.append("11:00 AM");
    times.append("12:00 PM");
    times.append("1:00 PM");
    times.append("2:00 PM");
    times.append("3:00 PM");
    times.append("4:00 PM");
    times.append("5:00 PM");
    times.append("6:00 PM");
    times.append("7:00 PM");
    times.append("8:00 PM");
    times.append("9:00 PM");
    times.append("10:00 PM");
    times.append("11:00 PM");
    times.append("12:00 AM");
    return times;
}

QJsonValue SettingsWizard::getTimeStr(int i)
{
    QList<QString> times;
    times.append("01:00:00");
    times.append("02:00:00");
    times.append("03:00:00");
    times.append("04:00:00");
    times.append("05:00:00");
    times.append("06:00:00");
    times.append("07:00:00");
    times.append("08:00:00");
    times.append("09:00:00");
    times.append("10:00:00");
    times.append("11:00:00");
    times.append("12:00:00");
    times.append("13:00:00");
    times.append("14:00:00");
    times.append("15:00:00");
    times.append("16:00:00");
    times.append("17:00:00");
    times.append("18:00:00");
    times.append("19:00:00");
    times.append("20:00:00");
    times.append("21:00:00");
    times.append("22:00:00");
    times.append("23:00:00");
    times.append("00:00:00");

    return QJsonValue(times.at(i));
}
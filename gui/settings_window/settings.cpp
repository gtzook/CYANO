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
    main->addStretch();

    setLayout(main);
}

QByteArray SettingsWizard::getSettings(){
    int daySel = field("day").toInt();
    int nightSel = field("night").toInt();
    int freqSel = field("agi_freq").toInt();

    QJsonObject settings;
    settings["day"] = getTimeStr(daySel);
    settings["night"] = getTimeStr(nightSel);
    settings["agi_freq"] = freqSel;

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

    // Init objects for night
    QLabel *nightBoxLabel = new QLabel("Nighttime:");
    QComboBox *night= new QComboBox;
    night->setFont(comboBoxFont);
    night->setFixedWidth(400);
    night->addItems(times);
    night->view()->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

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

    registerField("agi_freq", agiFreq);

    layout->addLayout(agiLayout);
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
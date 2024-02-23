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
    QVBoxLayout *agiLayout = new QVBoxLayout(); 

    QLabel *test = new QLabel("test");
    agiLayout->addWidget(test);
    //cycleLayout->addWidget(night);
    main->addSpacing(100);
    makeCycles(main);
    main->addSpacing(600);
    main->addLayout(agiLayout);
    main->addStretch();

    setLayout(main);
}

QJsonArray SettingsWizard::getSettings(){
    int daySel = field("day").toInt();
    int nightSel = field("night").toInt();
    QJsonArray settings;
    settings.append(QJsonValue)
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


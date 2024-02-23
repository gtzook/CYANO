#ifndef SETTINGS
#define SETTINGS
#include <QWizard>
#include <QDebug>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QtCore>
#include <QComboBox>
#include <QStringList>
#include <QAbstractItemView>

class SettingsWizard : public QWizard
{
    Q_OBJECT
public:
    SettingsWizard(QWidget *parent = nullptr);
    QByteArray getSettings();
private:
    int SettingsWizard::getTime;
    QJsonValue getTimeStr(int i);
};

class IntroPage : public QWizardPage
{
    Q_OBJECT

public:
    IntroPage(QWidget *parent = nullptr);

private:
    QLabel *label;
    void makeCycles(QBoxLayout *layout);
    void makeAgi(QBoxLayout *layout);
    QList<QString> makeAgis()
    QList<QString> makeTimes();
};

#endif
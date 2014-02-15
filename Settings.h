#ifndef SETTINGS_H
#define SETTINGS_H

#include <QtGui/QDialog>
#include "ui_Settings.h"

#include "GameSettings.h"

class Settings : public QDialog
{
    Q_OBJECT

public:
    Settings(QWidget *parent = 0);
    ~Settings();

private:
    Ui::SettingsClass ui;

public slots:
	void okClicked();
	void radioButtonClicked(bool);
	void settingsResult(const QString &str);
signals:
	void setSettings(const QString &, int, const GameSettings &);

};

#endif // SETTINGS_H

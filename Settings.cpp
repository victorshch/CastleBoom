#include "Settings.h"
#include <QMessageBox>

Settings::Settings(QWidget *parent)
    : QDialog(parent)
{
	ui.setupUi(this);

	QObject::connect(ui.pushButton, SIGNAL(clicked(void)),
		this, SLOT(okClicked(void)));
	QObject::connect(ui.radioButton, SIGNAL(toggled(bool)), this, SLOT(radioButtonClicked(bool)));
}

Settings::~Settings()
{

}

void Settings::okClicked()
{
	GameSettings s;
	s.isServer = ui.radioButton->isChecked();
	s.wind = false;
	s.dmg = ui.spinBox_4->value();
	s.maxHp = ui.spinBox_3->value();
	emit setSettings(ui.lineEdit->text(), ui.spinBox->value(), s);
}

void Settings::radioButtonClicked(bool checked)
{
	ui.spinBox_3->setEnabled(checked);
	ui.spinBox_4->setEnabled(checked);
}

void Settings::settingsResult(const QString &str)
{
	if(str=="")
	{
		accept();
		return;
	}
	QMessageBox::critical(this, "Error", str, QMessageBox::Ok, QMessageBox::Ok);
}

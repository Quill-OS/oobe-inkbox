#include "oobewindow.h"
#include "ui_oobewindow.h"

#include <QFont>
#include <QFontDatabase>
#include <QScreen>
#include <QFile>
#include <QRect>
#include <QDebug>
#include <QProcess>
#include <QDir>
#include <iostream>
#include <fstream>
#include <QTextStream>
#include <QThread>

#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>

oobewindow::oobewindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::oobewindow)
{
    ui->setupUi(this);
    ui->checkBox->setFont(QFont("u001"));
    ui->checkBox_3->setFont(QFont("u001"));
    ui->darkModeWidget->hide();

    // Getting the screen's size
    float sW = QGuiApplication::screens()[0]->size().width();
    float sH = QGuiApplication::screens()[0]->size().height();
    // Defining what the default icon size will be
    float stdIconWidth = sW / 12.5;
    float stdIconHeight = sH / 12.5;
    float scale1_W = sW / 10;
    float scale1_H = sH / 10;
    float scale2_W = sW / 8;
    float scale2_H = sH / 8;
    float scale3_W = sW / 6;
    float scale3_H = sH / 6;
    float koboxScale_W = sW / 1.50;
    float koboxScale_H = sH / 1.50;

    // Stylesheet and general look
    QFile stylesheetFile(":/eink.qss");
    stylesheetFile.open(QFile::ReadOnly);
    this->setStyleSheet(stylesheetFile.readAll());
    stylesheetFile.close();

    int id = QFontDatabase::addApplicationFont(":/fraunces.ttf");
    QString family = QFontDatabase::applicationFontFamilies(id).at(0);
    QFont fraunces(family);
    int id_1 = QFontDatabase::addApplicationFont(":/NotoMono-Regular.ttf");
    QString family_1 = QFontDatabase::applicationFontFamilies(id_1).at(0);
    QFont notomono(family_1);
    int id_2 = QFontDatabase::addApplicationFont(":/crimson.ttf");
    QString family_2 = QFontDatabase::applicationFontFamilies(id_2).at(0);
    QFont crimson(family_2);
    int id_3 = QFontDatabase::addApplicationFont(":/Bitter-Medium.ttf");
    QString family_3 = QFontDatabase::applicationFontFamilies(id_3).at(0);
    QFont bitter(family_3);
    int id_4 = QFontDatabase::addApplicationFont(":/IbarraRealNova-Medium.ttf");
    QString family_4 = QFontDatabase::applicationFontFamilies(id_4).at(0);
    QFont ibarra(family_4);


    deviceID = readFile("/opt/inkbox_device").trimmed();
    if(deviceID == "n705" or deviceID == "n905" or deviceID == "n613" or deviceID == "n236" or deviceID == "n437" or deviceID == "n306") {
        ui->darkModeWidget->show();
    }

    ui->logoLabel->setText("");
    QPixmap logoPixmap(":/quill.png");
    QPixmap scaledLogoPixmap = logoPixmap.scaled(sW / 2, sH / 2, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    ui->logoLabel->setPixmap(scaledLogoPixmap);

    ui->welcomeLabel->setStyleSheet("font-size: 15pt");
    ui->roboto->setFont(notomono);
    ui->crimsonPro->setFont(crimson);
    ui->bitter->setFont(bitter);
    ui->univers->setFont(QFont("u001"));
    ui->ibarra->setFont(ibarra);
    ui->chooseFontLabel->setStyleSheet("font-size: 12pt");
    ui->fontSizeLabel->setStyleSheet("font-size: 12pt");
    ui->uiScalingLabel->setStyleSheet("font-size: 12pt");
    ui->setupFinishedLabel->setStyleSheet("font-size: 20pt; padding: 40px");
    ui->darkModeLabel->setStyleSheet("font-size: 12pt");
    ui->koboxLabel->setStyleSheet("font-size: 12pt");
    ui->koboxInfoLabel->setStyleSheet("font-size: 9pt");

    QFont roboto("Roboto");
    ui->roboto->setFont(roboto);

    // Setting icons up
    ui->rightBtn->setProperty("type", "borderless");
    ui->rightBtn->setText("");
    ui->rightBtn->setIcon(QIcon(":/right-black.png"));
    ui->rightBtn->setIconSize(QSize(stdIconWidth, stdIconHeight));

    ui->leftBtn->setEnabled(false);
    ui->leftBtn->setProperty("type", "borderless");
    ui->leftBtn->setText("");
    ui->leftBtn->setIcon(QIcon(":/left-black.png"));
    ui->leftBtn->setIconSize(QSize(stdIconWidth, stdIconHeight));

    ui->startBtn->setStyleSheet("padding: 30px; background: lightGrey");

    QPixmap pixmap(":/list.png");
    QPixmap scaled1Pixmap = pixmap.scaled(scale1_W, scale1_H, Qt::KeepAspectRatio);
    ui->scale1->setPixmap(scaled1Pixmap);

    QPixmap scaled2Pixmap = pixmap.scaled(scale2_W, scale2_H, Qt::KeepAspectRatio);
    ui->scale2->setPixmap(scaled2Pixmap);

    QPixmap scaled3Pixmap = pixmap.scaled(scale3_W, scale3_H, Qt::KeepAspectRatio);
    ui->scale3->setPixmap(scaled3Pixmap);

    QPixmap koboxIconPixmap(":/kobox-icon.png");
    QPixmap scaledKoboxIconPixmap = koboxIconPixmap.scaled(koboxScale_W, koboxScale_H, Qt::KeepAspectRatio);
    ui->koboxIcon->setPixmap(scaledKoboxIconPixmap);

    // Font size chooser and misc. font sizes
    ui->font1->setStyleSheet("font-size: 6pt");
    ui->font2->setStyleSheet("font-size: 10pt");
    ui->font3->setStyleSheet("font-size: 14pt");
    ui->font4->setStyleSheet("font-size: 18pt");
    ui->font5->setStyleSheet("font-size: 22pt");

    ui->horizontalLayout->setAlignment(Qt::AlignCenter);
    ui->horizontalLayout->setContentsMargins(0,0,0,0);
    ui->horizontalLayout_4->setAlignment(Qt::AlignCenter);
    ui->horizontalLayout_4->setContentsMargins(0,0,0,0);
    ui->horizontalLayout_5->setAlignment(Qt::AlignCenter);
    ui->horizontalLayout_5->setContentsMargins(0,0,0,0);

    // Default selected
    dpi_not_user = true;
    ui->chooseScale1->setChecked(true);

    if(checkconfig("/external_root/boot/flags/X11_START") == true) {
        x11_not_user = true;
        ui->checkBox_3->setChecked(true);
    }

    // Brightness
    int brightnessVal = 50;
    std::string brightnessValStr = std::to_string(brightnessVal);
    cinematicBrightness(100, 5);
    setDefaultWorkDir();
    string_writeconfig(".config/03-brightness/config", brightnessValStr);
}

oobewindow::~oobewindow()
{
    delete ui;
}

void oobewindow::on_rightBtn_clicked()
{
    pageNumber = pageNumber + 1;
    ui->stackedWidget->setCurrentIndex(pageNumber);
    ui->leftBtn->setEnabled(true);

    if(pageNumber == 0) {
        ui->statusLabel->setText("Press Next to setup");
    }
    if(pageNumber == 1) {
        ui->statusLabel->setText("1 of 4");
    }
    if(pageNumber == 2) {
        if(dpi_not_user == true) {
            // Writing default as user didn't explicitly select any option
            if(deviceID == "n705" or deviceID == "n905" or deviceID == "kt") {
                string_writeconfig(".config/09-dpi/config", "160");
            }
            else if(deviceID == "n613" or deviceID == "n236") {
                string_writeconfig(".config/09-dpi/config", "195");
            }
            else if(deviceID == "n306") {
                string_writeconfig(".config/09-dpi/config", "212");
            }
            else if(deviceID == "n437" or deviceID == "n249") {
                string_writeconfig(".config/09-dpi/config", "275");
            }
            else if(deviceID == "n873") {
                string_writeconfig(".config/09-dpi/config", "285");
            }
            else {
                string_writeconfig(".config/09-dpi/config", "160");
            }
            string_writeconfig(".config/09-dpi/config-enabled", "true");
            dpi_not_user = false;
        }
        ui->statusLabel->setText("2 of 4");
    }
    if(pageNumber == 3) {
        ui->statusLabel->setText("3 of 4");
    }
    if(pageNumber == 4) {
        ui->statusLabel->setText("4 of 4");
    }
    if(pageNumber == 5) {
        ui->statusLabel->setText("Welcome to Quill");
        ui->rightBtn->setEnabled(false);
    }
}

void oobewindow::on_leftBtn_clicked()
{
    pageNumber = pageNumber - 1;
    if(pageNumber == 0) {
        ui->stackedWidget->setCurrentIndex(pageNumber);
        ui->leftBtn->setEnabled(false);
    }
    else {
        ui->stackedWidget->setCurrentIndex(pageNumber);
    }

    if(pageNumber == 0) {
        ui->statusLabel->setText("Press Next to setup");
    }
    if(pageNumber == 1) {
        ui->statusLabel->setText("1 of 4");
    }
    if(pageNumber == 2) {
        ui->statusLabel->setText("2 of 4");
    }
    if(pageNumber == 3) {
        ui->statusLabel->setText("3 of 4");
    }
    if(pageNumber == 4) {
        ui->statusLabel->setText("4 of 4");
        ui->rightBtn->setEnabled(true);
    }
    if(pageNumber == 5) {
        ui->statusLabel->setText("Welcome to Quill");
        ui->rightBtn->setEnabled(false);
    }
}

void oobewindow::on_chooseScale1_toggled(bool checked)
{
    if(checked == true) {
        if(dpi_not_user == true) {
            ;
        }
        else {
            if(deviceID == "n705" or deviceID == "n905" or deviceID == "kt") {
                string_writeconfig(".config/09-dpi/config", "160");
            }
            else if(deviceID == "n613" or deviceID == "n236") {
                string_writeconfig(".config/09-dpi/config", "195");
            }
            else if(deviceID == "n306") {
                string_writeconfig(".config/09-dpi/config", "212");
            }
            else if(deviceID == "n437" or deviceID == "n249") {
                string_writeconfig(".config/09-dpi/config", "275");
            }
            else if(deviceID == "n873") {
                string_writeconfig(".config/09-dpi/config", "285");
            }
            else {
                string_writeconfig(".config/09-dpi/config", "160");
            }
            dpi_not_user = false;
        }
    }
    else {
        ;
    }
}

void oobewindow::on_chooseScale2_toggled(bool checked)
{
    if(checked == true) {
        if(deviceID == "n705" or deviceID == "n905" or deviceID == "kt") {
            string_writeconfig(".config/09-dpi/config", "187");
        }
        else if(deviceID == "n613" or deviceID == "n236") {
            string_writeconfig(".config/09-dpi/config", "210");
        }
        else if(deviceID == "n306") {
            string_writeconfig(".config/09-dpi/config", "227");
        }
        else if(deviceID == "n437" or deviceID == "n249") {
            string_writeconfig(".config/09-dpi/config", "290");
        }
        else if(deviceID == "n873") {
            string_writeconfig(".config/09-dpi/config", "300");
        }
        else {
            string_writeconfig(".config/09-dpi/config", "214");
        }
    }
    else {
        ;
    }
}

void oobewindow::on_chooseScale3_toggled(bool checked)
{
    if(checked == true) {
        if(deviceID == "n705" or deviceID == "n905" or deviceID == "kt") {
            string_writeconfig(".config/09-dpi/config", "200");
        }
        else if(deviceID == "n613" or deviceID == "n236") {
            string_writeconfig(".config/09-dpi/config", "225");
        }
        else if(deviceID == "n306") {
            string_writeconfig(".config/09-dpi/config", "242");
        }
        else if(deviceID == "n437" or deviceID == "n249") {
            string_writeconfig(".config/09-dpi/config", "305");
        }
        else if(deviceID == "n873") {
            string_writeconfig(".config/09-dpi/config", "315");
        }
        else {
            string_writeconfig(".config/09-dpi/config", "227");
        }
    }
    else {
        ;
    }
}

void oobewindow::on_checkBox_toggled(bool checked)
{
    if(checked == true) {
        string_writeconfig(".config/10-dark_mode/config", "true");
        string_writeconfig("/tmp/invertScreen", "y");
    }
    else {
        string_writeconfig(".config/10-dark_mode/config", "false");
        string_writeconfig("/tmp/invertScreen", "n");
    }
}

void oobewindow::on_sourceSerif_toggled(bool checked)
{
    if(checked == true) {
        string_writeconfig(".config/04-book/font", "Source Serif Pro");
    }
    else {
        ;
    }
}

void oobewindow::on_libreBaskerville_toggled(bool checked)
{
    if(checked == true) {
        string_writeconfig(".config/04-book/font", "Libre Baskerville");
    }
    else {
        ;
    }
}

void oobewindow::on_radioButton_toggled(bool checked)
{
    if(checked == true) {
        string_writeconfig(".config/04-book/font", "Libertinus Serif");
    }
    else {
        ;
    }
}

void oobewindow::on_inter_toggled(bool checked)
{
    if(checked == true) {
        string_writeconfig(".config/04-book/font", "Inter");
    }
    else {
        ;
    }
}

void oobewindow::on_roboto_toggled(bool checked)
{
    if(checked == true) {
        string_writeconfig(".config/04-book/font", "Roboto");
    }
    else {
        ;
    }
}

void oobewindow::on_notoMono_toggled(bool checked)
{
    if(checked == true) {
        string_writeconfig(".config/04-book/font", "Noto Mono");
    }
    else {
        ;
    }
}

void oobewindow::on_font1_toggled(bool checked)
{
    if(checked == true) {
        string_writeconfig(".config/04-book/size", "0");
    }
    else {
        ;
    }
}

void oobewindow::on_font2_toggled(bool checked)
{
    if(checked == true) {
        string_writeconfig(".config/04-book/size", "3");
    }
    else {
        ;
    }
}

void oobewindow::on_font3_toggled(bool checked)
{
    if(checked == true) {
        string_writeconfig(".config/04-book/size", "6");
    }
    else {
        ;
    }
}

void oobewindow::on_font4_toggled(bool checked)
{
    if(checked == true) {
        string_writeconfig(".config/04-book/size", "9");
    }
    else {
        ;
    }
}

void oobewindow::on_font5_toggled(bool checked)
{
    if(checked == true) {
        string_writeconfig(".config/04-book/size", "12");
    }
    else {
        ;
    }
}

void oobewindow::on_startBtn_clicked()
{
    if(checkconfig("/opt/inkbox_genuine") == true) {
        // InkBox OS
        string_writeconfig("/external_root/boot/flags/FIRST_BOOT", "false");
        string_writeconfig("/tmp/oobe-inkbox_completed", "true");
    }
    else {
        // We're in Nickel land!
        string_writeconfig(".flags/FIRST_BOOT", "false");
        string_writeconfig("/tmp/oobe-inkbox_completed", "true");
    }
    QProcess process;
    process.startDetached("inkbox.sh", QStringList());
    qApp->quit();
}

void oobewindow::on_crimsonPro_toggled(bool checked)
{
    if(checked == true) {
        string_writeconfig(".config/04-book/font", "Crimson Pro");
    }
    else {
        ;
    }
}

void oobewindow::on_checkBox_3_toggled(bool checked)
{
    if(checked == true) {
        if(x11_not_user != true) {
            string_writeconfig("/external_root/boot/flags/X11_START", "true");
        }
        else {
            x11_not_user = false;
        }
    }
    else {
        string_writeconfig("/external_root/boot/flags/X11_START", "false");
    }
}

bool oobewindow::checkconfig(QString file) {
    QFile config(file);
    config.open(QIODevice::ReadOnly);
    QTextStream in (&config);
    const QString content = in.readAll();
    std::string contentstr = content.toStdString();
    if(contentstr.find("true") != std::string::npos) {
        return true;
    }
    else {
        return false;
    }
    config.close();
    return 0;
}

void oobewindow::string_writeconfig(string file, string config_option) {
    ofstream fhandler;
    fhandler.open(file);
    fhandler << config_option;
    fhandler.close();
}

QString oobewindow::readFile(QString file) {
    if(QFile::exists(file)) {
        QFile fileToRead(file);
        fileToRead.open(QIODevice::ReadOnly);
        QTextStream in (&fileToRead);
        QString content = in.readAll();
        return content;
    }
    else {
        return NULL;
    }
}

void oobewindow::setDefaultWorkDir() {
    QDir::setCurrent("/mnt/onboard/.adds/inkbox");
}

int oobewindow::get_brightness() {
    if(deviceID == "n613") {
        QString brightnessString = readFile(".config/03-brightness/config");
        int brightness;
        if(brightnessString.isEmpty()) {
            brightness = 0;
        }
        else {
            brightness = brightnessString.toInt();
        }
        return brightness;
    }
    else {
        QFile brightness("/var/run/brightness");
        brightness.open(QIODevice::ReadOnly);
        QString valuestr = brightness.readAll();
        int value = valuestr.toInt();
        brightness.close();
        return value;
    }
    return 0;
}

void oobewindow::cinematicBrightness(int brightness, int warmth) {
    QString cbPath("/external_root/lib/ld-musl-armhf.so.1");
    QStringList cbArgs;
    cbArgs << "/external_root/opt/bin/cinematic_brightness" << QString::number(brightness) << QString::number(10 * warmth) << "-1" << "-1" << "3000" << "1";
    QProcess *cbProc = new QProcess();
    cbProc->startDetached(cbPath, cbArgs);
    cbProc->deleteLater();
}

void oobewindow::on_bitter_toggled(bool checked)
{
    if(checked == true) {
        string_writeconfig(".config/04-book/font", "Bitter");
    }
    else {
        ;
    }
}


void oobewindow::on_ibarra_toggled(bool checked)
{
    if(checked == true) {
        string_writeconfig(".config/04-book/font", "Ibarra Real Nova");
    }
    else {
        ;
    }
}


void oobewindow::on_univers_toggled(bool checked)
{
    if(checked == true) {
        string_writeconfig(".config/04-book/font", "u001");
    }
    else {
        ;
    }
}


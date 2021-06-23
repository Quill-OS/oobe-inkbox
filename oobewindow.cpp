#include "oobewindow.h"
#include "ui_oobewindow.h"

#include <QFont>
#include <QFontDatabase>
#include <QScreen>
#include <QFile>
#include <QRect>
#include <QDebug>
#include <QProcess>

oobewindow::oobewindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::oobewindow)
{
    ui->setupUi(this);

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

    ui->logoLabel->setFont(QFont(fraunces));
    ui->logoLabel->setStyleSheet("font-size: 55pt");
    ui->welcomeLabel->setStyleSheet("font-size: 15pt");
    ui->roboto->setFont(notomono);
    ui->crimsonPro->setFont(crimson);
    ui->chooseFontLabel->setStyleSheet("font-size: 12pt");
    ui->fontSizeLabel->setStyleSheet("font-size: 12pt");
    ui->uiScalingLabel->setStyleSheet("font-size: 12pt");
    ui->setupFinishedLabel->setStyleSheet("font-size: 20pt; padding: 40px");
    ui->darkModeLabel->setStyleSheet("font-size: 12pt");
    ui->koboxLabel->setStyleSheet("font-size: 12pt");
    ui->koboxInfoLabel->setStyleSheet("font-size: 9pt");

    QFont roboto("Roboto");
    ui->roboto->setFont(roboto);

    // Getting the screen's size
    float sW = QGuiApplication::screens()[0]->size().width();
    float sH = QGuiApplication::screens()[0]->size().height();
    // Defining what the default icon size will be
    float stdIconWidth = sW / 12.5;
    float stdIconHeight = sH / 12.5;

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

    ui->startBtn->setStyleSheet("padding: 20px; background: lightGrey");

    float scale1_W = sW / 10;
    float scale1_H = sH / 10;
    float scale2_W = sW / 8;
    float scale2_H = sH / 8;
    float scale3_W = sW / 6;
    float scale3_H = sH / 6;
    float koboxScale_W = sW / 1.50;
    float koboxScale_H = sH / 1.50;

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
    if(checkconfig("/opt/inkbox_genuine") == true) {
        dpi_not_user = true;
        ui->chooseScale1->setChecked(true);
    }
    else {
        ui->checkBox_2->setChecked(true);
    }

    if(checkconfig("/external_root/boot/flags/X11_START") == true) {
        x11_not_user = true;
        ui->checkBox_3->setChecked(true);
    }
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
        if(ui->checkBox_2->isChecked() == true) {
            string_writeconfig(".config/09-dpi/config", "false");
            string_writeconfig(".config/09-dpi/config-enabled", "false");
        }
        else {
            if(dpi_not_user == true) {
                // Writing default as user didn't explicitly select any option
                string_checkconfig_ro("/opt/inkbox_device");
                if(checkconfig_str_val == "n705\n") {
                    string_writeconfig(".config/09-dpi/config", "187");
                }
                if(checkconfig_str_val == "n905\n") {
                    string_writeconfig(".config/09-dpi/config", "160");
                }
                else {
                    string_writeconfig(".config/09-dpi/config", "187");
                }
                string_writeconfig(".config/09-dpi/config-enabled", "true");
                dpi_not_user = false;
            }
            else {
                ;
            }
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
        ui->statusLabel->setText("Welcome to InkBox");
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
        ui->rightBtn->setEnabled(true);
    }
    if(pageNumber == 4) {
        ui->statusLabel->setText("4 of 4");
    }
    if(pageNumber == 5) {
        ui->statusLabel->setText("Welcome to InkBox");
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
            string_checkconfig_ro("/opt/inkbox_device");
            if(checkconfig_str_val == "n905\n") {
                string_writeconfig(".config/09-dpi/config", "160");
            }
            if(checkconfig_str_val == "n705\n") {
                string_writeconfig(".config/09-dpi/config", "187");
            }
            else {
                string_writeconfig(".config/09-dpi/config", "187");
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
        string_checkconfig_ro("/opt/inkbox_device");
        if(checkconfig_str_val == "n905\n") {
            string_writeconfig(".config/09-dpi/config", "187");
        }
        if(checkconfig_str_val == "n705\n") {
            string_writeconfig(".config/09-dpi/config", "214");
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
        string_checkconfig_ro("/opt/inkbox_device");
        if(checkconfig_str_val == "n905\n") {
            string_writeconfig(".config/09-dpi/config", "200");
        }
        if(checkconfig_str_val == "n705\n") {
            string_writeconfig(".config/09-dpi/config", "227");
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
        string_writeconfig(".config/04-book/size", "1");
    }
    else {
        ;
    }
}

void oobewindow::on_font3_toggled(bool checked)
{
    if(checked == true) {
        string_writeconfig(".config/04-book/size", "2");
    }
    else {
        ;
    }
}

void oobewindow::on_font4_toggled(bool checked)
{
    if(checked == true) {
        string_writeconfig(".config/04-book/size", "3");
    }
    else {
        ;
    }
}

void oobewindow::on_font5_toggled(bool checked)
{
    if(checked == true) {
        string_writeconfig(".config/04-book/size", "4");
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
    }
    else {
        // We're in Nickel land!
        string_writeconfig(".flags/FIRST_BOOT", "false");
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

void oobewindow::on_checkBox_2_toggled(bool checked)
{
    // If you think something is missing, check out the on_rightBtn_clicked slot.
    if(checked == true) {
        ui->scalingWidget->setVisible(false);
        string_checkconfig(".config/09-dpi/config");
        dpiSetting = checkconfig_str_val.toStdString();
        string_writeconfig(".config/09-dpi/config", "false");
    }
    else {
        ui->scalingWidget->setVisible(true);

        // Write previously chosen value
        string_writeconfig(".config/09-dpi/config", dpiSetting);
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

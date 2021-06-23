#ifndef OOBEWINDOW_H
#define OOBEWINDOW_H

#include <QMainWindow>
#include <iostream>
#include <fstream>
#include <QFile>
#include <QTextStream>

using namespace std;

QT_BEGIN_NAMESPACE
namespace Ui { class oobewindow; }
QT_END_NAMESPACE

class oobewindow : public QMainWindow
{
    Q_OBJECT

public:
    oobewindow(QWidget *parent = nullptr);
    ~oobewindow();
    int pageNumber = 0;
    bool dpi_not_user = false;
    bool x11_not_user = false;
    QString checkconfig_str_val;
    string dpiSetting;
    void string_writeconfig(string file, string config_option) {
        ofstream fhandler;
        fhandler.open(file);
        fhandler << config_option;
        fhandler.close();
    }
    void string_checkconfig(QString file) {
        QFile config(file);
        config.open(QIODevice::ReadWrite);
        QTextStream in (&config);
        checkconfig_str_val = in.readAll();
        config.close();
    }
    void string_checkconfig_ro(QString file) {
        QFile config(file);
        config.open(QIODevice::ReadOnly);
        QTextStream in (&config);
        checkconfig_str_val = in.readAll();
        config.close();
    }
    bool checkconfig(QString file) {
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
    };

private slots:
    void on_rightBtn_clicked();

    void on_leftBtn_clicked();

    void on_chooseScale1_toggled(bool checked);

    void on_chooseScale2_toggled(bool checked);

    void on_chooseScale3_toggled(bool checked);

    void on_checkBox_toggled(bool checked);

    void on_sourceSerif_toggled(bool checked);

    void on_libreBaskerville_toggled(bool checked);

    void on_radioButton_toggled(bool checked);

    void on_inter_toggled(bool checked);

    void on_roboto_toggled(bool checked);

    void on_notoMono_toggled(bool checked);

    void on_font1_toggled(bool checked);

    void on_font2_toggled(bool checked);

    void on_font3_toggled(bool checked);

    void on_font4_toggled(bool checked);

    void on_font5_toggled(bool checked);

    void on_startBtn_clicked();

    void on_crimsonPro_toggled(bool checked);

    void on_checkBox_2_toggled(bool checked);

    void on_checkBox_3_toggled(bool checked);

private:
    Ui::oobewindow *ui;
};
#endif // OOBEWINDOW_H

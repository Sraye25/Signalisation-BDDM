#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtWidgets>
#include "affichage.h"

namespace Ui {class MainWindow;}

///RGB : value 0 to 255
typedef struct {
    double r;
    double g;
    double b;
} rgb;

///HSV
typedef struct {
    double h;       // angle in degrees
    double s;       // Percent
    double v;       // Percent
} hsv;

class MainWindow : public QMainWindow
{
    Q_OBJECT

    public:
        explicit MainWindow(QWidget *parent = 0);
        ~MainWindow();

    private slots:
        void on_pushButton_pressed();

        void on_pushButton_2_pressed();

    private:
        Ui::MainWindow *ui;

};

#endif // MAINWINDOW_H

#ifndef FENETRE_H
#define FENETRE_H

#include <QMainWindow>
#include <QImage>
#include <QString>
#include <QFileDialog>

namespace Ui
{
    class Fenetre;
}

class Fenetre : public QMainWindow
{
    Q_OBJECT
    public:
        explicit Fenetre(QWidget *parent = 0);
        ~Fenetre();

    public slots:
        //Charger une scéne
        void chargerScene();

    private:
        Ui::Fenetre *ui;

        QImage m_scene;
};

#endif // FENETRE_H

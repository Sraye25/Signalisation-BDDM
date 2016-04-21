#ifndef AFFICHAGE_H
#define AFFICHAGE_H

#include <QVector>
#include <QImage>
#include <QLabel>
#include <QVBoxLayout>
#include <iostream>

class MenuDeroulant
{
    public:
        MenuDeroulant(QVBoxLayout *layout, QWidget *widget);
        ~MenuDeroulant();

        void ajouterImage(QImage const& image);
        void effacer();

    private:

        int tailleImageCumule() const;

        QVector<QLabel*> m_listeImage;
        QVBoxLayout* m_layout;
        QWidget* m_widget;
};


#endif // AFFICHAGE_H

#ifndef TRIANGLEDETECTION_H
#define TRIANGLEDETECTION_H

#include <QImage>
#include <iostream>

#include "detection.h"

typedef struct Triangle Triangle;
struct Triangle
{
    QPoint p1;
    QPoint p2;
    QPoint p3;
};

class Triangledetection
{
    public:

        Triangledetection();

        //detecter les triangles
        QImage detect(const QImage &source);

        //avoir la liste des triangles
        QVector<Triangle> getListeTriangle() const;

        //avoir les images autour des triangle
        QVector<QImage> avoirImageTriangle(const QImage &source) const;

    private:

        //Supprimer les doublons dans le liste de triangle
        void supprimerDoublon();

        QVector<Triangle> liste_triangle;
};

//Dessiner un triangle dans une image
void dessiner(QImage &image, const Triangle& triangle, QRgb couleur);

//Verifier si deux triangles sont les mêmes ou pas
//ecart : taille de la fênetre autour de chaque point du trianle afin de voir si les triangle sont les mêmes
bool triangleEquivalent(const Triangle& t1, const Triangle& t2, int ecart=0);

//Verifier si deux points sont les mêmes ou pas
//ecart : taille de la fênetre autour de chaque point afin de voir si les points sont les mêmes
bool pointEquivalent(const QPoint& p1, const QPoint& p2, int ecart=0);

//Renvoie le triangle le plus grand
Triangle trianglePlusGrand(const Triangle& t1, const Triangle& t2);

//Avoir la boite englobante triangle
QRect boiteEnglobanteTriangle(const Triangle& t);

#endif // TRIANGLEDETECTION_H

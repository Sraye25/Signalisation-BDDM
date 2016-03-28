#include "indexationrecherche.h"
#include <QVector>
#include <QColor>
#include <cmath>
#include <cstring>
#include <iostream>
#include <stack>
#include <sstream>

#define SSTR( x ) static_cast< std::ostringstream & >( \
        ( std::ostringstream() << std::dec << x ) ).str()

/*
 * Recherche la meilleure ressemblance du panneau avec les images tests
 * et renvoie le dossier où chercher
 */
std::string IndexationRecherche::rechercherbondossierrecherche(QImage image) {

    //COMPOSANTES CONNEXES ROUGES
    int nbcomposantes8connexesrouges=0;
    QImage nbcomposanteshuitconnexesrouges=image;
    QImage nbcomposanteshuitconnexesnoires=image;
    QImage nbcomposanteshuitconnexesbleues=image;
    std::string chemin;

    //RECHERCHE DES COMPOSANTES CONNEXES BLEUES
    int nbcomposantes8connexesbleues=0;
    for ( int row = 0; row <image.height(); row++ ) {
        for ( int col = 0; col < image.width(); col++ ) {
            QColor clrCurrent( image.pixel( col, row ) );
            int red = clrCurrent.red();
            int green = clrCurrent.green();
            int blue = clrCurrent.blue();

            if (red <50 && green < 100 && blue >100) {
                nbcomposanteshuitconnexesbleues.setPixel(col, row, qRgb(0,0,0));
            }
            else nbcomposanteshuitconnexesbleues.setPixel(col, row, qRgb(255,255,255));
        }
    }

    nbcomposantes8connexesbleues=nombreComposante8ConnexeNoir(nbcomposanteshuitconnexesbleues);

    if (nbcomposantes8connexesbleues>0) {
        chemin+="bleu/";
        if (nbcomposantes8connexesbleues>=4) {
            chemin+="4b/";
        }
        else if (nbcomposantes8connexesbleues>=2) {
            chemin+="2b/";
        }
        else {
            chemin+="0/";
        }
        return chemin;
    }


    for ( int row = 0; row <image.height(); row++ ) {
        for ( int col = 0; col < image.width(); col++ ) {
            QColor clrCurrent( image.pixel( col, row ) );
            int red = clrCurrent.red();
            int green = clrCurrent.green();
            int blue = clrCurrent.blue();

            if (red >150 && green<100 && blue<100) {
                nbcomposanteshuitconnexesrouges.setPixel(col, row, qRgb(255,255,255));
            }
        else nbcomposanteshuitconnexesrouges.setPixel(col, row, qRgb(0,0,0));
        }
    }

    for ( int row = 0; row <nbcomposanteshuitconnexesrouges.height(); row++ ) {
        for ( int col = 0; col < nbcomposanteshuitconnexesrouges.width(); col++ ) {
            QColor clrCurrent( nbcomposanteshuitconnexesrouges.pixel( col, row ) );
            int red = clrCurrent.red();

            if (red == 0) {
                nbcomposanteshuitconnexesrouges=coloriage8connexe(nbcomposanteshuitconnexesrouges,row,col);
                nbcomposantes8connexesrouges++;
            }
        }
    }

    if (nbcomposantes8connexesrouges==2) { //se place dans le chemin 2 composantes connexes rouges trouvées
        chemin = SSTR(nbcomposantes8connexesrouges);
        chemin+="r/";
        std::cout << "Nombre de composantes 8-connexes rouges ";
        std::cout << chemin << std::endl;
        return chemin;
    }
    else if (nbcomposantes8connexesrouges==1) { //se place dans le chemin 1 composante connexe rouge trouvée
        chemin = SSTR(nbcomposantes8connexesrouges);
        chemin+="r/";
        std::cout << "Nombre de composantes 8-connexes rouges ";
        std::cout << chemin << std::endl;

        // RECHERCHE COMPOSANTES CONNEXES NOIRES
        int nbcomposantes8connexesnoires=0;
        for ( int row = 0; row <image.height(); row++ ) {
            for ( int col = 0; col < image.width(); col++ ) {
                QColor clrCurrent( image.pixel( col, row ) );
                int red = clrCurrent.red();
                int green = clrCurrent.green();
                int blue = clrCurrent.blue();

                if (red <50 && green < 50 && blue <50) {
                    nbcomposanteshuitconnexesnoires.setPixel(col, row, qRgb(0,0,0));
                }
                else nbcomposanteshuitconnexesnoires.setPixel(col, row, qRgb(255,255,255));
            }
        }

        nbcomposantes8connexesnoires=nombreComposante8ConnexeNoir(nbcomposanteshuitconnexesnoires);

        if (nbcomposantes8connexesnoires==1 || nbcomposantes8connexesnoires==2 ||nbcomposantes8connexesnoires==3) { //se place dans le chemin 2 composantes connexes noires trouvées
            chemin += SSTR(nbcomposantes8connexesnoires);
            chemin+="b/";
            std::cout << "Nombre de composantes 8-connexes noires ";
            std::cout << chemin << std::endl;
            return chemin;
        }
        else if (nbcomposantes8connexesnoires==0) { //se place dans le chemin 2 composantes connexes noires trouvées
            chemin += SSTR(nbcomposantes8connexesnoires);
            chemin+="b/";
            std::cout << "Nombre de composantes 8-connexes noires ";
            std::cout << chemin << std::endl;

            //REGARDE POURCENTAGE ROUGE DANS LA PHOTO
            int pourrouge=0;
            for ( int row = 0; row <image.height(); row++ ) {
                for ( int col = 0; col < image.width(); col++ ) {
                    QColor clrCurrent( image.pixel( col, row ) );
                    int red = clrCurrent.red();
                    int green = clrCurrent.green();
                    int blue = clrCurrent.blue();

                    if (red >150 && green<125 && blue<125) {
                        pourrouge++;
                    }
                }
            }

            if (pourrouge>(image.height()*image.width()/2)) {
                chemin+="fr/";
            }
            else {
                chemin+="nfr/";
            }

            return chemin;
        }
        else {
            chemin+="hard/";
            std::cout << "Nombre de composantes 8-connexes noires ";
            std::cout << chemin << std::endl;
            return chemin;
        }


    }
    else { //0 ou plus de 2 composantes connexes rouges -> panneau non trouvé
        chemin+="0/";
        std::cout << "Nombre de composantes 8-connexes rouges ";
        std::cout << chemin << std::endl;
        return chemin;
    }
}


QImage IndexationRecherche::coloriage8connexe(QImage image, int x, int y) {

    QPoint germe;
    germe.setX(x);
    germe.setY(y);


    std::stack<QPoint> pile;
    QPoint point;


    pile.push(germe);

    while(!pile.empty())
    {
        point = pile.top(); pile.pop();
        image.setPixel(point.x(), point.y(),qRgb(255,255,255));

        if(point.x()>0 && qRgb(0,0,0) == image.pixel(point.x()-1,point.y())) pile.push(QPoint(point.x()-1,point.y()));
        if(point.x()<image.width()-1 && qRgb(0,0,0) == image.pixel(point.x()+1,point.y())) pile.push(QPoint(point.x()+1,point.y()));
        if(point.y()>0 && qRgb(0,0,0) == image.pixel(point.x(),point.y()-1)) pile.push(QPoint(point.x(),point.y()-1));
        if(point.y()<image.height()-1 && qRgb(0,0,0) == image.pixel(point.x(),point.y()+1)) pile.push(QPoint(point.x(),point.y()+1));
        if(point.x()>0 && point.y()>0 && qRgb(0,0,0) == image.pixel(point.x()-1,point.y()-1)) pile.push(QPoint(point.x()-1,point.y()-1));
        if(point.x()>0 && point.y()<image.height()-1 && qRgb(0,0,0) == image.pixel(point.x()-1,point.y()+1)) pile.push(QPoint(point.x()-1,point.y()+1));
        if(point.x()<image.width()-1 && point.y()>0 && qRgb(0,0,0) == image.pixel(point.x()+1,point.y()-1)) pile.push(QPoint(point.x()+1,point.y()-1));
        if(point.x()<image.width()-1 && point.y()<image.height()-1 && qRgb(0,0,0) == image.pixel(point.x()+1,point.y()+1)) pile.push(QPoint(point.x()+1,point.y()+1));

    }


    return image;
}



int IndexationRecherche::nombreComposante8ConnexeNoir(QImage image)
{
    std::stack<QPoint> pile;
    QPoint point;
    QRgb coul_courante;
    int nbCompo=0;

    int** matrice = new int* [image.width()];
    for(int i=0; i<image.width();i++)
    {
        matrice[i] = new int[image.height()];
        for(int j=0; j<image.height();j++) matrice[i][j] = 0; //Initialisation
    }

    //Pour chaque pixel
    for(int i=0; i<image.width();i++)
    {
        for(int j=0; j<image.height();j++)
        {
            if(matrice[i][j]==0) //Si le pixel n'est pas encore dans un composante connexe
            {
                coul_courante = image.pixel(i,j);
                pile.push(QPoint(i,j));
                if (coul_courante==qRgb(0,0,0)){
                    nbCompo++; //On a une compo connexe en plus
                }

                while(!pile.empty()) //Ici 8-connexité
                {
                    point = pile.top(); pile.pop();
                    image.setPixel(point,coul_courante);
                    matrice[point.x()][point.y()]=nbCompo;

                    if(point.x()>0 && coul_courante == image.pixel(point.x()-1,point.y()) && matrice[point.x()-1][point.y()]!=nbCompo) pile.push(QPoint(point.x()-1,point.y()));
                    if(point.x()<image.width()-1 && coul_courante == image.pixel(point.x()+1,point.y()) && matrice[point.x()+1][point.y()]!=nbCompo) pile.push(QPoint(point.x()+1,point.y()));
                    if(point.y()>0 && coul_courante == image.pixel(point.x(),point.y()-1) && matrice[point.x()][point.y()-1]!=nbCompo) pile.push(QPoint(point.x(),point.y()-1));
                    if(point.y()<image.height()-1 && coul_courante == image.pixel(point.x(),point.y()+1) && matrice[point.x()][point.y()+1]!=nbCompo) pile.push(QPoint(point.x(),point.y()+1));

                    if(point.x()>0 && point.y()>0 && coul_courante == image.pixel(point.x()-1,point.y()-1) && matrice[point.x()-1][point.y()-1]!=nbCompo) pile.push(QPoint(point.x()-1,point.y()-1));
                    if(point.x()<image.width()-1 && point.y()>0 && coul_courante == image.pixel(point.x()+1,point.y()-1) && matrice[point.x()+1][point.y()-1]!=nbCompo) pile.push(QPoint(point.x()+1,point.y()-1));
                    if(point.x()>0 && point.y()<image.height()-1 && coul_courante == image.pixel(point.x()-1,point.y()+1) && matrice[point.x()-1][point.y()+1]!=nbCompo) pile.push(QPoint(point.x()-1,point.y()+1));
                    if(point.x()<image.width()-1 && point.y()<image.height()-1 && coul_courante == image.pixel(point.x()+1,point.y()+1) && matrice[point.x()+1][point.y()+1]!=nbCompo) pile.push(QPoint(point.x()+1,point.y()+1));
                }
            }
        }
    }

    //Supression de la matrice
    for(int i=0; i<image.width();i++) delete[] matrice[i];
    delete[] matrice;

    return nbCompo;
}

#include "triangledetection.h"

Triangledetection::Triangledetection() : liste_triangle()
{

}


QImage Triangledetection::detect(const QImage &source, const QImage &imageBase)
{
    //QImage binary = detectionContour(extraireRouge(source));
    QImage binary = detectionContour(source);
    QImage detection = binary.convertToFormat(QImage::Format_RGB888);

    QVector<QPoint> ligne = hough(detection);
    std::cout << "-> Nombre de ligne detecté : " << ligne.size() << std::endl;

    QVector<QPoint> ligne_angle0,ligne_angle60,ligne_angle120;
    QPoint inter1,inter2,inter3;
    int l1,l2,l3;



    //Avoir les lignes avec des angles pouvant appartenir à un panneau (+ ou - 1°)
    avoirLigneAngle(ligne,ligne_angle0,90,1);
    avoirLigneAngle(ligne,ligne_angle0,270,1);

    avoirLigneAngle(ligne,ligne_angle60,150,1);
    avoirLigneAngle(ligne,ligne_angle60,330,1);

    avoirLigneAngle(ligne,ligne_angle120,210,1);
    avoirLigneAngle(ligne,ligne_angle120,30,1);

    //On determine les intersections et les longueurs des segments
    for(int i=0;i<ligne_angle0.size();i++)
    {
        for(int j=0;j<ligne_angle60.size();j++)
        {
            for(int k=0;k<ligne_angle120.size();k++)
            {

                inter1 = intersection(ligne_angle0[i],ligne_angle60[j]);
                inter2 = intersection(ligne_angle60[j],ligne_angle120[k]);
                inter3 = intersection(ligne_angle120[k],ligne_angle0[i]);

                l1 = distance(inter1,inter2);
                l2 = distance(inter2,inter3);
                l3 = distance(inter3,inter1);

                //Si les distance sont les mêmes et que tous les points sont dans l'image => on a un triangle
                if(l1 == l2 && l2 == l3 && l1 > 30 && l1 < 100 && estPointImage(detection,inter1) && estPointImage(detection,inter2) && estPointImage(detection,inter3))
                {
                    Triangle a;
                    a.p1 = inter1;
                    a.p2 = inter2;
                    a.p3 = inter3;
                    liste_triangle.push_back(a);
                }
            }
        }
    }

    std::cout<<"-> Nombre de triangle detectés avant élimination des doublons : " << liste_triangle.size() << std::endl;

    //On supprime les triangle doublons
    supprimerDoublon();

    //Dessiner les triangles à l'écran
    for(int i=0;i<liste_triangle.size();i++)
        dessiner(detection,liste_triangle[i],qRgb(0,255,127));


    //Generer les images avec les cercles reconnus
    for(int i=0;i<liste_triangle.size();i++)
    {
        int minX = liste_triangle[i].p1.x();
        int minY = liste_triangle[i].p1.y();
        int maxX = liste_triangle[i].p1.x();
        int maxY = liste_triangle[i].p1.y();
        if (liste_triangle[i].p2.x()<minX) minX = liste_triangle[i].p2.x();
        if (liste_triangle[i].p2.y()<minY) minY = liste_triangle[i].p2.y();
        if (liste_triangle[i].p2.x()>maxX) maxX = liste_triangle[i].p2.x();
        if (liste_triangle[i].p2.y()>maxY) maxY = liste_triangle[i].p2.y();
        if (liste_triangle[i].p3.x()<minX) minX = liste_triangle[i].p3.x();
        if (liste_triangle[i].p3.y()<minY) minY = liste_triangle[i].p3.y();
        if (liste_triangle[i].p3.x()>maxX) maxX = liste_triangle[i].p3.x();
        if (liste_triangle[i].p3.y()>maxY) maxY = liste_triangle[i].p3.y();


        QImage BlueRoadSigns = QImage(maxX-minX, maxY-minY, QImage::Format_RGB32);

        for(int row = 0;row<maxY-minY;row++)
        {
            for (int col=0;col<maxX-minX;col++)
            {
                QColor clrCurrent(imageBase.pixel(col+minX,row+minY));

                int red = clrCurrent.red();
                int green = clrCurrent.green();
                int blue = clrCurrent.blue();

                BlueRoadSigns.setPixel(col, row, qRgb(red,green,blue));
            }
        }

        liste_TrianglesReconnu.push_back(BlueRoadSigns);
    }

    std::cout<<"-> Nombre de triangle detectés : " << liste_triangle.size() << std::endl;

    return detection;
}

QVector<Triangle> Triangledetection::getListeTriangle() const
{
    return liste_triangle;
}

QVector<QImage> Triangledetection::avoirImageTriangle(const QImage &source) const
{
    QVector<QImage> res;
    QImage img;

    for(int i=0;i<liste_triangle.size();i++)
    {
        img = source.copy(boiteEnglobanteTriangle(liste_triangle[i]));
        res.push_back(img);
    }

    return res;
}

void Triangledetection::supprimerDoublon()
{
    QVector<Triangle> copie;
    Triangle temp;

    for(int i=0;i<liste_triangle.size();i++)
    {
        int j=0; bool present=false;
        //Si le triangle n'est pas present, on continue la recherche
        while(j<copie.size() && !present)
        {
            //Si le triangle est present dans la liste
            if(triangleEquivalent(liste_triangle[i],copie[j],20))
            {
                present=true;
                //On garde le triangle le plus grand
                copie[j] = trianglePlusGrand(liste_triangle[i],copie[j]);
            }
            j++;
        }

        //Si le triangle n'est pas présent, on ajoute
        if(!present) copie.push_back(liste_triangle[i]);
    }

    liste_triangle = copie;
}

void dessiner(QImage& image, const Triangle& triangle, QRgb couleur)
{
    ligneBresenham(image,triangle.p1,triangle.p2,couleur);
    ligneBresenham(image,triangle.p2,triangle.p3,couleur);
    ligneBresenham(image,triangle.p3,triangle.p1,couleur);
}

bool triangleEquivalent(const Triangle& t1, const Triangle& t2, int ecart)
{
    if(pointEquivalent(t1.p1,t2.p1,ecart) && pointEquivalent(t1.p2,t2.p2,ecart) && pointEquivalent(t1.p3,t2.p3,ecart))
        return true;
    else
        return false;
}

bool pointEquivalent(const QPoint& p1, const QPoint& p2, int ecart)
{
    QPoint p = p1-p2;

    if(p.x()*p.x()+p.y()*p.y() <= ecart*ecart)
        return true;
    else
        return false;
}

Triangle trianglePlusGrand(const Triangle& t1, const Triangle& t2)
{
    QPoint l1 = t1.p1 - t1.p2;
    QPoint l2 = t2.p1 - t2.p2;

    //Si triangle1 < triangle2
    if(l1.x()*l1.x()+l1.y()*l1.y() < l2.x()*l2.x()+l2.y()*l2.y())
        return t2;
    else
        return t1;
}

QRect boiteEnglobanteTriangle(const Triangle& t)
{
    int lmax=t.p1.x() , lmin=t.p1.x() , hmax=t.p1.y() , hmin=t.p1.y();

    if(t.p2.x() > lmax) lmax = t.p2.x();
    if(t.p3.x() > lmax) lmax = t.p3.x();

    if(t.p2.x() < lmin) lmin = t.p2.x();
    if(t.p3.x() < lmin) lmin = t.p3.x();

    if(t.p2.y() > hmax) hmax = t.p2.y();
    if(t.p3.y() > hmax) hmax = t.p3.y();

    if(t.p2.y() < hmin) hmin = t.p2.y();
    if(t.p3.y() < hmin) hmin = t.p3.y();

    return QRect(QPoint(lmin,hmin),QPoint(lmax,hmax));
}



QVector<QImage> Triangledetection::panneauxReconnu() const
{
    QVector<QImage> res;



    //Count the number of Red Circle Road Signs In Database
    DIR *pdir = NULL;
    pdir = opendir ("./data/TR/");
    FilesBDDM fbddm;
    int nbRedRoadSignsInDatabase = fbddm.compterFichier(pdir);
    std::cout << nbRedRoadSignsInDatabase << " Red Circles Road Signs In Database" << std::endl;
    closedir (pdir);

    long TRessemblances[nbRedRoadSignsInDatabase+2][2]; //Tableau des ressemblances avec la base de données

    for(int k=0; k<nbRedRoadSignsInDatabase+2; k++)
    {
        TRessemblances[k][0] = -1;
        TRessemblances[k][1] = -1;
    }


    for(int i=0; i< liste_triangle.size(); i++)
    {
        QImage BlueRoadSigns = liste_TrianglesReconnu[i];

        IndexationRecherche Indexationrech;
        std::string adddossierrecherche = Indexationrech.rechercherbondossierrechercheTriangles(BlueRoadSigns);

        //Binairisation du panneau
        BlueRoadSigns = binarisationPanneau(BlueRoadSigns);

        // Compare the sign with all in the database to find the most-look-loke sign
        std::cout << "Recherche des ressemblances" << std::endl;
        int maxressemblance=0;

        std::string chemin2 = "./data/TR/" + adddossierrecherche;
        pdir = opendir (chemin2.c_str());

        struct dirent *pent = NULL;
        if (pdir == NULL)
        {
            std::cout << "\nERROR! pdir could not be initialised correctly";
            exit (3);
        }
        int fichiersparcourus = 0;

        while (pent = readdir (pdir))
        {
            if (pent == NULL)
            {
                std::cout << "\nERROR! pent could not be initialised correctly";
                exit (3);
            }
            if (strcmp(pent->d_name, ".") != 0 && strcmp(pent->d_name, "..") != 0)
            {
                std::string chemin3 = "./data/TR/" + adddossierrecherche + pent->d_name;
                QImage CurrentImageDatabase((char*)chemin3.c_str());

                std::cout << "Le fichier a bien été ouvert : " << pent->d_name << std::endl;

                // Scale the image to compare it to database
                CurrentImageDatabase = CurrentImageDatabase.scaled(100,100,Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
                //Binarise the image to apply skeletonisation
                CurrentImageDatabase = binarisationautre(CurrentImageDatabase);

                //Comparaison des ressemblances
                int ratio=0; //nb de pixels noirs squeletisés à comparer
                int ressemblances=0; //nb de pixels noirs squeletisés correspondants


                for ( int row = 0; row < BlueRoadSigns.width(); row++ )
                {
                    for ( int col = 0; col < BlueRoadSigns.height(); col++ )
                    {
                        QColor c( BlueRoadSigns.pixel( col, row ) );
                        if (c.red()==0)
                        {
                            // Si le pixel est noir
                            ratio++;
                            QColor c2( CurrentImageDatabase.pixel( col, row ) );
                            if (c2.red()==0) ressemblances++;
                        }
                    }
                }

                TRessemblances[fichiersparcourus][0]=telldir(pdir);
                TRessemblances[fichiersparcourus][1]=ressemblances*1000/ratio;

                if (TRessemblances[fichiersparcourus][1]>TRessemblances[maxressemblance][1])
                    maxressemblance = fichiersparcourus;
            }
            fichiersparcourus++;
        }

        std::cout << adddossierrecherche << std::endl;

        DIR *pdir2 = NULL;
        std::string chemin4 = "./data/TR/" + adddossierrecherche;
        pdir2 = opendir ((char*)chemin4.c_str());

        rewinddir(pdir2);
        seekdir(pdir2, TRessemblances[maxressemblance-1][0]);

        std::string chemin = "./data/TR/"+adddossierrecherche;
        chemin += readdir(pdir2)->d_name;
        res.push_back(QImage((char*)chemin.c_str()));

        closedir(pdir2);
        closedir(pdir);

    }

    return res;
}

#include "triangledetection.h"

Triangledetection::Triangledetection()
{

}

QImage Triangledetection::detect(const QImage &source)
{
    QImage binary = edges(source);
    QImage detection = binary.convertToFormat(QImage::Format_RGB888);

    QVector<QPoint> ligne = hough(source);
    QVector<QPoint> ligne_angle0,ligne_angle60,ligne_angle120;
    QVector<Triangle> liste_triangle;
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
                if(l1 == l2 && l2 == l3 && estPointImage(detection,inter1) && estPointImage(detection,inter2) && estPointImage(detection,inter3))
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

    for(int i=0;i<liste_triangle.size();i++)
        dessiner(detection,liste_triangle[i],qRgb(0,255,127));

    std::cout<<"NB triangle : " << liste_triangle.size() << std::endl;

    return detection;
}

void dessiner(QImage& image, const Triangle& triangle, QRgb couleur)
{
    ligneBresenham(image,triangle.p1,triangle.p2,couleur);
    ligneBresenham(image,triangle.p2,triangle.p3,couleur);
    ligneBresenham(image,triangle.p3,triangle.p1,couleur);
}



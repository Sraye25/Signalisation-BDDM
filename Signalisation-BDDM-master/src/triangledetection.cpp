#include "triangledetection.h"

Triangledetection::Triangledetection()
{

}

QImage Triangledetection::detect(const QImage &source)
{
    QImage binary = edges(source);
    //QImage detection = source.convertToFormat(QImage::Format_RGB888);

    QVector<QPoint> ligne = hough(source);
    QVector<QPoint> ligne_angle0,ligne_angle60,ligne_angle120;
    QVector<int> longueur;

    //Avoir les lignes avec des angles pouvant appartenir à un panneau
    avoirLigneAngle(ligne,ligne_angle0,0,3);
    avoirLigneAngle(ligne,ligne_angle0,180,3);

    avoirLigneAngle(ligne,ligne_angle60,60,3);
    avoirLigneAngle(ligne,ligne_angle60,240,3);

    avoirLigneAngle(ligne,ligne_angle120,120,3);
    avoirLigneAngle(ligne,ligne_angle120,300,3);

    //On ne garde que les segments de même longueur
    for(int i=0;i<ligne_angle0.size();i++)
    {
        for(int j=0;j<ligne_angle60.size();j++)
        {
            for(int k=0;k<ligne_angle120.size();k++)
            {
            }
        }
    }

    std::cout<<"NB ligne 0° : " << ligne_angle0.size() << std::endl;
    std::cout<<"NB ligne 60° : " << ligne_angle60.size() << std::endl;
    std::cout<<"NB ligne 120° : " << ligne_angle120.size() << std::endl;


    return binary;
}




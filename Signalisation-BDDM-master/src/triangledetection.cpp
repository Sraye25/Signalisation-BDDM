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
    QVector<int> longueur;

    //Avoir les lignes avec des angles pouvant appartenir à un panneau
    avoirLigneAngle(ligne,ligne_angle0,90,2);
    avoirLigneAngle(ligne,ligne_angle0,270,2);

    avoirLigneAngle(ligne,ligne_angle60,150,2);
    avoirLigneAngle(ligne,ligne_angle60,330,2);

    avoirLigneAngle(ligne,ligne_angle120,210,2);
    avoirLigneAngle(ligne,ligne_angle120,30,2);

    //Tracer
    for(int i=0;i<ligne_angle0.size();i++)
    {
        tracerLigne(detection,ligne_angle0[i].x(),ligne_angle0[i].y(),qRgb(0,255,127));
    }

    for(int i=0;i<ligne_angle60.size();i++)
    {
        tracerLigne(detection,ligne_angle60[i].x(),ligne_angle60[i].y(),qRgb(0,255,127));
    }

    for(int i=0;i<ligne_angle120.size();i++)
    {
        tracerLigne(detection,ligne_angle120[i].x(),ligne_angle120[i].y(),qRgb(0,255,127));
    }



    std::cout<<"NB ligne 0° : " << ligne_angle0.size() << std::endl;
    std::cout<<"NB ligne 60° : " << ligne_angle60.size() << std::endl;
    std::cout<<"NB ligne 120° : " << ligne_angle120.size() << std::endl;


    return detection;
}




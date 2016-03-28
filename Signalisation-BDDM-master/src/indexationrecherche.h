#include <cstring>
#include <QImage>
#include <QVector>

#ifndef INDEXATIONRECHERCHE_H
#define INDEXATIONRECHERCHE_H

class IndexationRecherche
{
public:
    QImage coloriage8connexe(QImage image, int x, int y);
    std::string rechercherbondossierrecherche(QImage image);
    int nombreComposante8ConnexeNoir(QImage image);
};

#endif // INDEXATIONRECHERCHE_H

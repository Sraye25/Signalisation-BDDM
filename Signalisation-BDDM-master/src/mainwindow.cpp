#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QObject::connect(ui->actionOuvrir,SIGNAL(triggered()),this,SLOT(ouvrirFichier()));
    QObject::connect(ui->actionPanneaux_Rouges,SIGNAL(triggered()),this,SLOT(extrairePanneauxRouges()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

/*****************************************************************************
 ** load Image
 ****************************************************************************/
void MainWindow::ouvrirFichier()
{
    QString fname = QFileDialog::getOpenFileName(this, "Open", qApp->applicationDirPath(), "Images (*.jpg *.bmp *.tif *.png)");
    if(!QFile(fname).exists())
        return;

    QImage image(fname);
    ui->image_base->setPixmap(QPixmap::fromImage(image));
    ui->image_base->setScaledContents(true);
}

/*****************************************************************************
 ** Convert image into HSV space to egalize hitograms of V-value to increase contrast to have a better recognition
 ** On pressed :
 ******************************************************************************/
void MainWindow::extrairePanneauxRouges()
{
    QImage image = ui->image_base->pixmap()->toImage(); if(image.isNull()) return;
    QImage RedRoadSigns = image;
    MenuDeroulant menuImage(ui->verticalLayout_cercle,ui->scrollAreaWidgetContents_cercle);
    MenuDeroulant menuResultat(ui->verticalLayout_cercle_resulat,ui->scrollAreaWidgetContents_cercle_resultat);
    MenuDeroulant menuTriangle(ui->verticalLayout_triangle,ui->scrollAreaWidgetContents_triangle);

    //Egalisation d'histogramme
    RedRoadSigns = egalisationHistogramme(RedRoadSigns);

    //Extraire couleur rouge
    RedRoadSigns = extraireRouge(RedRoadSigns);

    //Affichage de l'image où le rouge est extrait
    ui->image_rouge->setPixmap(QPixmap::fromImage(RedRoadSigns));
    ui->image_rouge->setScaledContents(true);

    /// --------------- DETECTION TRIANGLES

    Triangledetection tri;
    QImage ligneImage = tri.detect(RedRoadSigns);
    QVector<QImage> liste_triangle = tri.avoirImageTriangle(image);

    for(int i=0;i<liste_triangle.size();i++)
        menuTriangle.ajouterImage(liste_triangle[i].scaled(100,100,Qt::KeepAspectRatio));

    ui->image_triangle->setPixmap(QPixmap::fromImage(ligneImage));
    ui->image_triangle->setScaledContents(true);

    /// --------------- DETECTION CERCLES

    unsigned int min_r = 0, max_r = 0;

    min_r = 10;
    max_r = 40;

    HoughCircleDetector hcd;
    QImage resultRedRoadSigns = hcd.detect(RedRoadSigns, image, min_r, max_r);

    ui->image_cercle->setPixmap(QPixmap::fromImage(resultRedRoadSigns));
    ui->image_cercle->setScaledContents(true);

    QVector<QImage> liste_cercle = hcd.avoirCercleReconnu();
    for(int i=0;i<liste_cercle.size();i++)
        menuImage.ajouterImage(liste_cercle[i].scaled(100,100,Qt::KeepAspectRatio));

    QVector<QImage> liste_panneauTrouvee = hcd.panneauxReconnu();
    for(int i=0;i<liste_panneauTrouvee.size();i++)
        menuResultat.ajouterImage(liste_panneauTrouvee[i].scaled(100,100,Qt::KeepAspectRatio));
}

/******************************************************************************
 ** RGV to HSV
 ******************************************************************************/
hsv rgb2hsv(rgb in)
{
    hsv         out;
    double      min, max, delta;

    min = in.r < in.g ? in.r : in.g;
    min = min  < in.b ? min  : in.b;

    max = in.r > in.g ? in.r : in.g;
    max = max  > in.b ? max  : in.b;

    out.v = max;                                // v
    delta = max - min;
    if (delta < 0.00001)
    {
        out.s = 0;
        out.h = 0; // undefined, maybe nan?
        return out;
    }
    if( max > 0.0 ) { // NOTE: if Max is == 0, this divide would cause a crash
        out.s = (delta / max);                  // s
    } else {
        // if max is 0, then r = g = b = 0
            // s = 0, v is undefined
        out.s = 0.0;
        out.h = NAN;                            // its now undefined
        return out;
    }
    if( in.r >= max )                           // > is bogus, just keeps compilor happy
        out.h = ( in.g - in.b ) / delta;        // between yellow & magenta
    else
    if( in.g >= max )
        out.h = 2.0 + ( in.b - in.r ) / delta;  // between cyan & yellow
    else
        out.h = 4.0 + ( in.r - in.g ) / delta;  // between magenta & cyan

    out.h *= 60.0;                              // degrees

    if( out.h < 0.0 )
        out.h += 360.0;

    return out;
}

/******************************************************************************
 ** HSV to RGB
 ******************************************************************************/
rgb hsv2rgb(hsv in)
{
    double      hh, p, q, t, ff;
    long        i;
    rgb         out;

    if(in.s <= 0.0) {       // < is bogus, just shuts up warnings
        out.r = in.v;
        out.g = in.v;
        out.b = in.v;
        return out;
    }
    hh = in.h;
    if(hh >= 360.0) hh = 0.0;
    hh /= 60.0;
    i = (long)hh;
    ff = hh - i;
    p = in.v * (1.0 - in.s);
    q = in.v * (1.0 - (in.s * ff));
    t = in.v * (1.0 - (in.s * (1.0 - ff)));

    switch(i) {
    case 0:
        out.r = in.v;
        out.g = t;
        out.b = p;
        break;
    case 1:
        out.r = q;
        out.g = in.v;
        out.b = p;
        break;
    case 2:
        out.r = p;
        out.g = in.v;
        out.b = t;
        break;

    case 3:
        out.r = p;
        out.g = q;
        out.b = in.v;
        break;
    case 4:
        out.r = t;
        out.g = p;
        out.b = in.v;
        break;
    case 5:
    default:
        out.r = in.v;
        out.g = p;
        out.b = q;
        break;
    }
    return out;
}

/******************************************************************************
 ** Invert Black and White color to correspond to our skeletisation implementation
 ******************************************************************************/
QImage InvertBlackAndWhite(QImage img) {
    QColor noirC(0,0,0);
    QRgb noir = noirC.rgb();
    QColor blancC(255,255,255);
    QRgb blanc = blancC.rgb();

    for (int i=1; i<img.width()-1;i++) {
        for (int j=1; j<img.height()-1;j++) {
            if (img.pixel(i,j)==noir) {
                img.setPixel(i,j,blanc);
            } else if (img.pixel(i,j)==blanc) {
                img.setPixel(i,j,noir);
            }
        }
    }
    return img;
}

/******************************************************************************
 ** Erosion
 ******************************************************************************/
QImage Erosion(QImage RedRoadSigns) {

    QImage temoin = RedRoadSigns;

    QColor noirC(0,0,0);
    QRgb noir = noirC.rgb();

    for(int i=0; i<temoin.width(); i++)
    {
        for(int j=0; j<temoin.height();j++)
        {
            if(temoin.pixel(i,j)==noir) //Si le pixel est noir
            {
                if(i < temoin.width()-1 && temoin.pixel(i+1,j)!=noir) RedRoadSigns.setPixel(i+1,j,noir);
                if(i > 0 && temoin.pixel(i-1,j)!=noir) RedRoadSigns.setPixel(i-1,j,noir);
                if(j < temoin.height()-1 && temoin.pixel(i,j+1)!=noir) RedRoadSigns.setPixel(i,j+1,noir);
                if(j > 0 && temoin.pixel(i,j-1)!=noir) RedRoadSigns.setPixel(i,j-1,noir);
            }
        }
    }

    return RedRoadSigns;
}


/******************************************************************************
 ** Dilatation
 ******************************************************************************/
QImage Dilatation(QImage RedRoadSigns) {

    QImage temoin = RedRoadSigns;

    QColor blancC(255,255,255);
    QRgb blanc = blancC.rgb();

    for(int i=0; i<temoin.width(); i++)
    {
        for(int j=0; j<temoin.height();j++)
        {
            if(temoin.pixel(i,j)==blanc) //Si le pixel est noir
            {
                if(i < temoin.width()-1 && temoin.pixel(i+1,j)!=blanc) RedRoadSigns.setPixel(i+1,j,blanc);
                if(i > 0 && temoin.pixel(i-1,j)!=blanc) RedRoadSigns.setPixel(i-1,j,blanc);
                if(j < temoin.height()-1 && temoin.pixel(i,j+1)!=blanc) RedRoadSigns.setPixel(i,j+1,blanc);
                if(j > 0 && temoin.pixel(i,j-1)!=blanc) RedRoadSigns.setPixel(i,j-1,blanc);
            }
        }
    }

    return RedRoadSigns;
}



/******************************************************************************
 ** Squeletisation
 ******************************************************************************/
QImage Squeletisation(QImage img) {
        bool modif1 = true, modif2 = true;
        QColor noirC(0,0,0);
        QRgb noir = noirC.rgb();
        bool res = false;


        while(modif1 || modif2)
        {

            res = false;

            for(int i=1; i<img.width()-1;i++)
            {
                for(int j=1; j<img.height()-1;j++)
                {
                    if(img.pixel(i,j)==noir)
                    {
                        if(nbPixelVoisins8Noir(img,i,j) <= 6 && nbPixelVoisins8Noir(img,i,j) >= 2)
                        {
                            if(nbTransitionVoisinBlancNoir(img,i,j) == 1)
                            {
                                if(img.pixel(i,j+1)!=noir || img.pixel(i+1,j)!=noir || img.pixel(i,j-1)!=noir)
                                {
                                    if(img.pixel(i+1,j)!=noir || img.pixel(i,j-1)!=noir || img.pixel(i-1,j)!=noir)
                                    {
                                        img.setPixel(i,j,qRgb(255,255,255));
                                        res = true;
                                    }
                                }
                            }
                        }
                    }
                }
            }
            modif1 = res;


            res = false;

            for(int i=1; i<img.width()-1;i++)
            {
                for(int j=1; j<img.height()-1;j++)
                {
                    if(img.pixel(i,j)==noir)
                    {
                        if(nbPixelVoisins8Noir(img,i,j) <= 6 && nbPixelVoisins8Noir(img,i,j) >= 2)
                        {
                            if(nbTransitionVoisinBlancNoir(img,i,j) == 1)
                            {
                                if(img.pixel(i,j+1)!=noir || img.pixel(i+1,j)!=noir || img.pixel(i-1,j)!=noir)
                                {
                                    if(img.pixel(i,j+1)!=noir || img.pixel(i,j-1)!=noir || img.pixel(i-1,j)!=noir)
                                    {
                                        img.setPixel(i,j,qRgb(255,255,255));
                                        res = false;
                                    }
                                }
                            }
                        }
                    }
                }
            }
            modif2 = res;
        }

        return img;

    }

//==========================================================================================================
int nbTransitionVoisinBlancNoir(QImage temoin, int x, int y)
{
    QColor noirC(0,0,0);
    QRgb noir = noirC.rgb();

    int res=0;

    if(temoin.pixel(x,y+1)!=noir && temoin.pixel(x+1,y+1)==noir) res++;
    if(temoin.pixel(x+1,y+1)!=noir && temoin.pixel(x+1,y)==noir) res++;
    if(temoin.pixel(x+1,y)!=noir && temoin.pixel(x+1,y-1)==noir) res++;
    if(temoin.pixel(x+1,y-1)!=noir && temoin.pixel(x,y-1)==noir) res++;
    if(temoin.pixel(x,y-1)!=noir && temoin.pixel(x-1,y-1)==noir) res++;
    if(temoin.pixel(x-1,y-1)!=noir && temoin.pixel(x-1,y)==noir) res++;
    if(temoin.pixel(x-1,y)!=noir && temoin.pixel(x-1,y+1)==noir) res++;
    if(temoin.pixel(x-1,y+1)!=noir && temoin.pixel(x,y+1)==noir) res++;

    return res;
}

//==========================================================================================================
int nbPixelVoisins8Noir(QImage const& temoin, int x, int y)
{
    QColor noirC(0,0,0);
    QRgb noir = noirC.rgb();

    int res=0;
    if(x<temoin.width()-1 && temoin.pixel(x+1,y)==noir) res++;
    if(x<temoin.width()-1 && y<temoin.height()-1 && temoin.pixel(x+1,y+1)==noir) res++;
    if(x<temoin.width()-1 && y>0 && temoin.pixel(x+1,y-1)==noir) res++;
    if(x>0 && temoin.pixel(x-1,y)==noir) res++;
    if(x>0 && y<temoin.height()-1 && temoin.pixel(x-1,y+1)==noir) res++;
    if(x>0 && y>0 && temoin.pixel(x-1,y-1)==noir) res++;
    if(y<temoin.height()-1 && temoin.pixel(x,y+1)==noir) res++;
    if(y>0 && temoin.pixel(x,y-1)==noir) res++;

    return res;
}

//==========================================================================================================
/* Bianrisation seuillage automatique: Otsu */
QImage binarisation_otsu(QImage image) {
    int hist[255];
    float prob[255], omega[255];
    float myu[255]; /* Valeur moyenne pour la sÃ©paration */
    float max_sigma, sigma[255];
    int i, x, y;
    int threshold; /* Seuil de binarisation */
    int moy;
    int taille=0;

    /* GÃ©nÃ©ration de l'histogramme */
    for (i = 0; i < 255; i++) hist[i] = 0;
    for (x=0; x<image.width(); x++) {
        for (y=0; y<image.height(); y++) {
            QColor c( image.pixel( x, y ) );
            if ((c.red()!=255 + c.green()!=255 + c.blue()!=255) || (c.red()!=0 + c.green()!=0 + c.blue()!=0)) {
                moy = (int)((c.red() + c.green() + c.blue())/3);
                hist[moy]++;
                taille++;
            }
        }
    }

    /* calcul de la densitÃ© de probabilitÃ© */
    for ( i = 0; i < 255; i ++ ) {
        prob[i] = (double)hist[i] / (taille);
    }

    /* GÃ©nÃ©ration de omega & myu */
    omega[0] = prob[0];
    myu[0] = 0.0;
    for (i = 1; i < 255; i++) {
        omega[i] = omega[i-1] + prob[i];
        myu[i] = i*prob[i];
    }

    /* calcul de la valeur optimale du seuil */
    threshold = 0;
    max_sigma = 0.0;
    for (i = 0; i < 255; i++) {
        if (omega[i] != 0.0 && omega[i] != 1.0) { sigma[i] = pow(myu[i]*omega[i] - myu[i], 2) / (omega[i]*(1.0 - omega[i])); }
        else {sigma[i] = 0.0;}
        if (sigma[i] > max_sigma) {
            max_sigma = sigma[i];
            threshold = i;
        }
    }
    if (threshold==0) threshold=123; /* Cas des images binaires */

    int largeur = image.width();
    int hauteur = image.height();
    QColor noirC(0,0,0);
    QRgb noir = noirC.rgb();
    QColor blancC(255,255,255);
    QRgb blanc = blancC.rgb();

    std::cout << "Seuil optimal : ";
    std::cout << threshold << std::endl;

    QImage image2 = image;

    for (x=0; x<largeur; x++) {
        for (y=0; y<hauteur; y++) {
            if ((x>=0) && (y>=0)) {
                QColor c( image.pixel( x, y ) );
                moy = (int)((c.red() + c.green() + c.blue())/3);

                if (moy<threshold) { image2.setPixel(x,y,noir); }
                if (moy>=threshold) { image2.setPixel(x,y,blanc); }
            }
        }
    }

    return image2;
}

//==========================================================================================================
/* Bianrisation seuillage automatique: Otsu */
QImage binarisationautre(QImage image) {

    for ( int row = 0; row < image.height(); row++ ) {
        for ( int col = 0; col < image.width(); col++ )
        {
            QColor clrCurrent( image.pixel( col, row ) );

            int red = clrCurrent.red();
            int green = clrCurrent.green();
            int blue = clrCurrent.blue();

            if (red >100 && green>100 && blue>100) {
                image.setPixel(col, row, qRgb(255,255,255));
            }
            else image.setPixel(col, row, qRgb(0,0,0));
        }
    }

    return image;
}

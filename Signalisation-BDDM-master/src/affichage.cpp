#include "affichage.h"

MenuDeroulant::MenuDeroulant(QVBoxLayout* layout, QWidget* widget) :
    m_layout(layout)
  , m_widget(widget)
{
    QLayoutItem *child;

    m_listeImage.clear();
    while((child = m_layout->takeAt(0)) != 0) delete child;
    m_widget->repaint();
}

void MenuDeroulant::ajouterImage(QImage const& image)
{
    m_listeImage.push_back(new QLabel());
    m_listeImage.last()->setPixmap(QPixmap::fromImage(image));
    m_layout->addWidget(m_listeImage.last());

    int taille = tailleImageCumule();

    m_widget->setMinimumHeight(taille);
    m_widget->setMaximumHeight(taille);
}

void MenuDeroulant::effacer()
{
    QLayoutItem *child;

    for(int i=0;i<m_listeImage.size();i++)
    {
        m_listeImage[i]->clear();
        delete m_listeImage[i];
    }
    while((child = m_layout->takeAt(0)) != 0) delete child;

    m_listeImage.clear();
    m_widget->repaint();
}

MenuDeroulant::~MenuDeroulant()
{

}

int MenuDeroulant::tailleImageCumule() const
{
    int taille = 0;
    for(int i=0; i<m_listeImage.size();i++) taille += (m_listeImage[i]->pixmap()->height()+5);
    std::cout << "Taaaaaaaaaaaaaille : " << taille << std::endl;
    return taille;
}

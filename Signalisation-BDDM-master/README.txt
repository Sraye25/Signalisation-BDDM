DONE:
-charger une image
-faire ressortir les couleurs succeptibles d'appartenir aux panneaux rouges (Pour les images couleurs, il faut �tendre la m�thode d'�galisation en niveau de gris aux 3 composantes de couleur (ex. RVB). Il est possible de faire une �galisation ind�pendamment sur chaque composante, toutefois ceci d�grade les couleurs, et n'est donc pas utilis� en pratique2. La m�thode utilis�e est de r�aliser l'�galisation uniquement sur les intensit�s, sans toucher aux couleurs. Ceci est fait en g�n�ral dans l'espace colorim�trique HSV, o� l'�galisation est r�alis�e sur la composante V uniquement)
-binarisation otsu seuil automatique
-transformations de morphologie math�matique (au choix):
	- soit: ouverture (erosion puis dilatation) suivi d'une squeletisation
	- soit: detection de contours (plus long que le premier et plus de r�sultats donc beaucoup plus de calculs)
-reconnaissance de cercles
-afficher les cercles d�tect�s (en supprimant ceux dans la m�me zone)
-remplissage de la base de donn�es
-ajout d'images test
-correction d'un bug faisant crasher l'app du au tableau non initialis� (et du coup tableau d'int �tait assign� � 'nan' oO??)
-nouvelle binarisation pour les panneaux ce qui am�liore grandement la reconnaissance
cr�ation de fonctions afin d'ajouter les images dans la liste
-indexation termin�e pour les panneaux circulaires rouges

NEXT:
->reconnaitre les formes (triangles, carr�s)
->afficher multiples d�tection

BUGS:
->Si on clique sur un bouton sans avoir charg� une image, fait planter le programme
->Bug fonctions ajout image : plus d'une image fait planter le programme

AUTRES:
Osef les panneaux bleus pour le moment

DONE:
-charger une image
-faire ressortir les couleurs succeptibles d'appartenir aux panneaux rouges (Pour les images couleurs, il faut étendre la méthode d'égalisation en niveau de gris aux 3 composantes de couleur (ex. RVB). Il est possible de faire une égalisation indépendamment sur chaque composante, toutefois ceci dégrade les couleurs, et n'est donc pas utilisé en pratique2. La méthode utilisée est de réaliser l'égalisation uniquement sur les intensités, sans toucher aux couleurs. Ceci est fait en général dans l'espace colorimétrique HSV, où l'égalisation est réalisée sur la composante V uniquement)
-binarisation otsu seuil automatique
-transformations de morphologie mathématique (au choix):
	- soit: ouverture (erosion puis dilatation) suivi d'une squeletisation
	- soit: detection de contours (plus long que le premier et plus de résultats donc beaucoup plus de calculs)
-reconnaissance de cercles
-afficher les cercles détectés (en supprimant ceux dans la même zone)

NEXT:
->reconnaitre les formes (triangles, carrés)

BUGS:
->Si on clique sur un bouton sans avoir chargé une image, fait planter le programme
->Afficher plusieurs panneaux détectés
->Bug make install

AUTRES:
Osef les panneaux bleus pour le moment
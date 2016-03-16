#include "filesbddm.h"
#include <stdlib.h>
#include <stdio.h>
#include <cstring>
#include <dirent.h>

#ifndef WIN32
    #include <sys/types.h>
#endif


int FilesBDDM::compterFichier(DIR* dir)
{
    int nbr = 0;
    struct dirent* ent = NULL;

    while ((ent = readdir(dir)) != NULL)
    {
        if (strcmp(ent->d_name, ".") != 0 && /* Si le fichier lu n'est pas . */
            strcmp(ent->d_name, "..") != 0) /*  Et n'est pas .. non plus */
        nbr++; /* Alors on incrémente le compteur */
    }

    return nbr;
}

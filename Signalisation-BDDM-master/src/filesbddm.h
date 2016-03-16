#ifndef FILESBDDM_H
#define FILESBDDM_H

#include <dirent.h>

#ifndef WIN32
    #include <sys/types.h>
#endif


class FilesBDDM
{
public:
    int compterFichier(DIR* dir);
};

#endif // FILESBDDM_H

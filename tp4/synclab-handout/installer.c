/*
 * Sync Lab - installer.c
 * 
 * Ecole polytechnique de Montreal, 2018
 */

#include "libsynclab.h"
#include "installer/libinstaller.h"

// TODO
// Si besoin, ajouter ici les directives d'inclusion
// -------------------------------------------------
#include <stdlib.h>
// -------------------------------------------------

// TODO
// Si besoin, définissez ici des types de structures et/ou
// des variables
// -------------------------------------------------------
struct installAttri 
{
    int packageNum;
    struct management_data* md;
};


char* namePackage [10] = {
    "good-1.17", "swacheb-8.16", "libslenkopt-3.8", "rub-2.3","libkut-5.9",
    "bulik-2.19", "libvank-7.2",
    "liblog-7.14","libgleg-2.5",
    "flissect-0.0"
};

// -------------------------------------------------------

// TODO
// Si besoin, définissez ici des fonctions supplémentaires
// -------------------------------------------------------
void* pthreadErrorManagement(void* arg)
{
    struct installAttri * cleanUpPar = (struct installAttri *) arg; 
    doPackageCleanup(cleanUpPar->packageNum, cleanUpPar->md);
    switch(cleanUpPar->packageNum)
    {
        case 0 :
            break;
        case 1 :
            pthread_cancel(cleanUpPar->md->tids[0]);
            break;
        case 2 :
            pthread_cancel(cleanUpPar->md->tids[0]);
            break;
        case 3 :
            pthread_cancel(cleanUpPar->md->tids[0]);
            break;
        case 4 :
            pthread_cancel(cleanUpPar->md->tids[1]);
            pthread_cancel(cleanUpPar->md->tids[2]);
            pthread_cancel(cleanUpPar->md->tids[3]);
            break;
        case 5 :
            pthread_cancel(cleanUpPar->md->tids[0]);
            pthread_cancel(cleanUpPar->md->tids[1]);
            pthread_cancel(cleanUpPar->md->tids[3]);
            break;
        case 6 :
            pthread_cancel(cleanUpPar->md->tids[1]);
            pthread_cancel(cleanUpPar->md->tids[2]);
            pthread_cancel(cleanUpPar->md->tids[3]);
            break;
        case 7 :
            pthread_cancel(cleanUpPar->md->tids[0]);
            pthread_cancel(cleanUpPar->md->tids[5]);
            break;
        case 8 :
            pthread_cancel(cleanUpPar->md->tids[4]);
            pthread_cancel(cleanUpPar->md->tids[5]);
            break;
        case 9 :
            pthread_cancel(cleanUpPar->md->tids[5]);
            pthread_cancel(cleanUpPar->md->tids[1]);
            break;
        default :
            break;

    }
    return NULL;
}

void* install (void* arg)
{
    
    
    struct installAttri* att = ( struct installAttri *) arg;
    threadedPackageInstaller(att->packageNum, att->md);
   
    return NULL;
}


// -------------------------------------------------------

/*
 * Cette fonction alloue et initialise certains champs de la structure md qui
 * sont utilisés dans le reste des fonctions.
 */
void initializeManagementData(struct management_data *md) {
    // TODO (Q2, Q3)
    md->downloadSemaphore = malloc(sizeof(sem_t));
   // md->canInstallPackage = malloc(10*sizeof(sem_t));
    sem_init(md->downloadSemaphore, 0, 3);
    for (int i = 0 ; i <10 ; i++)
    {
        md->canInstallPackage[i] = malloc(sizeof(sem_t));
        if((i == 9) || (i == 8) || (i == 7) || ((i == 6)) )
        {
            sem_init(md->canInstallPackage[i], 0, 1);
        }
        else 
        {
            sem_init(md->canInstallPackage[i], 0, 0);
        }
    }
}

/*
 * Cette fonction nettoie les champs de la structure md qui ont été initialisés
 * par la fonction initializeManagementData.
 */
void cleanupManagementData(struct management_data *md) {
    // TODO (Q2, Q3)
    sem_destroy(md->downloadSemaphore);
    free(md->downloadSemaphore );
    for (int i = 0 ; i < 10 ; i++)
    {
        sem_destroy(md->canInstallPackage[i]);
        free(md->canInstallPackage[i]);
    }
}

/*
 * Cette fonction télécharge et installe le paquet logiciel dont le numéro est
 * passé en argument.
 */
void installPackage(int packageNum, struct management_data *md) {
    // TODO (Q2, Q3)
    struct installAttri* att = malloc(sizeof(struct installAttri));
    att->packageNum = packageNum;
    att->md = md;
    pthread_cleanup_push( (void*)pthreadErrorManagement, (void*)att);
    

    sem_wait(md->downloadSemaphore);
    doPackageDownload(namePackage[packageNum], md);
    sem_post(md->downloadSemaphore);

    sem_wait(md->canInstallPackage[packageNum]);
    doPackageInstall(namePackage[packageNum], md);
   pthread_cleanup_pop (0);
}

/*
 * Cette fonction vérifie si le paquet logiciel dont le numéro est passé en
 * argument est prêt pour l'installation. Si c'est le cas, la fonction débloque
 * le paquet pour que son installation puisse se lancer.
 * 
 * NOTE: Cette fonction vous aidera à clarifier votre code pour la fonction
 * postPackageInstall. Il est fortement recommandée de l'utiliser, mais ce
 * n'est pas obligatoire.
 */
void wakePackage(int wokenPackageNum, struct management_data *md) {
    // TODO (Q3)


}

/*
 * Cette fonction est exécutée après l'installation du paquet logiciel dont
 * le numéro est passé en argument. Son rôleF est de marquer le paquet comme
 * installé dans la structure md, et également de débloquer l'installation des
 * paquets logiciels qui pourraient désormais être prêts pour installation.
 * 
 * Afin de clarifier votre code, il est conseillé de compléter et d'utiliser la
 * fonction wakePackage définie juste au-dessus.
 * 
 * ATTENTION: Cette fonction est testée de manière unitaire par le script de
 * notation. Vous devez vous assurer qu'elle a bien le comportement décrit plus
 * haut et qu'elle ne modifie pas d'autres variables que celles stockées dans
 * la structure md.
 */
void postPackageInstall(int packageNum, struct management_data *md) {
    // TODO (Q3)
    md->isPackageInstalled[ packageNum] = 1;
     if ((md->isPackageInstalled[9] == 1) && 
        (md->isPackageInstalled[8] == 1) &&
        (md->isPackageInstalled[7] == 1) ) 
        sem_post(md->canInstallPackage[5]);

    if(md->isPackageInstalled[8]== 1)
        sem_post(md->canInstallPackage[4]);

    if((md->isPackageInstalled[6]==1)  &&  
       (md->isPackageInstalled[4]) 
       )
        sem_post(md->canInstallPackage[2]);

    if((md->isPackageInstalled[6]==1) && 
        (md->isPackageInstalled[4]==1)  &&  
        (md->isPackageInstalled[5] == 1) 
        )
        sem_post(md->canInstallPackage[3]);

    if((md->isPackageInstalled[9] == 1)  &&  
       (md->isPackageInstalled[5] == 1)  &&
        (md->isPackageInstalled[4]==1)  && 
        (md->isPackageInstalled[6]==1) 
        )
        sem_post(md->canInstallPackage[1]); 

    if((md->isPackageInstalled[1] == 1)  &&  
        (md->isPackageInstalled[3] == 1)  &&
        (md->isPackageInstalled[2] == 1) && 
        (md->isPackageInstalled[5] == 1) && 
        (md->isPackageInstalled[7] == 1)
       )
        sem_post(md->canInstallPackage[0]); 

    
    
}

/*
 * Cette fonction crée les dix fils d'exécution qui seront chargés de
 * l'installation des paquets logiciels.
 * Chaque fil d'exécution doit exécuter la fonction threadedPackageInstaller
 * en lui passant comme argument:
 * - le numéro du paquet logiciel à installer (compris entre 0 et 9)
 * - le pointeur md vers la structure passée en argument de la fonction
 *   installer
 */

void installer(struct management_data *md) {
    // NE PAS MODIFIER
    // --------------------------
    initializeManagementData(md);
    installerTestInit(md);
    // --------------------------

    // TODO (Q1)
    // Lancer les fils d'exécution
    // ---------------------------
    struct installAttri * att[10] ;
    for (int i= 0 ; i < 10; i++)
    {
        att[i] = calloc(1,  sizeof(struct installAttri ));
        att[i]->packageNum = i;
        att[i]->md = md;
        pthread_create(&md->tids[i], NULL, (void*)install , (void*) (att[i]));
    }
    for (int i = 0 ; i <10 ; i++)
    {
        pthread_join(md->tids[i], NULL);
    }
    for (int i = 0 ; i<10 ; i++)
    {
        free(att[i]);
    }
    // ---------------------------

    // NE PAS MODIFIER
    // -----------------------
    cleanupManagementData(md);
    // -----------------------
}
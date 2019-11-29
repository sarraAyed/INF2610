/*
 * Mem Lab - memsim.c
 * 
 * Ecole polytechnique de Montreal, 2018
 */

#include "memsim.h"
#include "memsim/libmemsim.h"
#include "libmemlab.h"
#include <stdbool.h>
// TODO
// Si besoin, ajoutez ici les directives d'inclusion
// -------------------------------------------------
#include <stdlib.h>
// -------------------------------------------------

// TODO
// Si besoin, définissez ici des fonctions supplémentaires
// -------------------------------------------------------

// -------------------------------------------------------

/*
 * Calcule et renvoie le déplacement dans la page correspondant à l'adresse
 * fournie en argument.
 */
unsigned long getPageOffset(unsigned long addr) {
    // TODO
    return addr%2048;
}

/*
 * Calcule et renvoie le numéro de page correspondant à l'adresse virtuelle
 * fournie en argument.
 */
unsigned long getPageNumber(unsigned long addr) {
    // TODO
    return addr>>11;
}

/*
 * Calcule et renvoie l'adresse de début de page (ou cadre) correspondant au
 * numéro de page (ou de cadre) fourni en argument.
 */
unsigned long getStartPageAddress(unsigned long pageNumber) {
    // TODO
    return pageNumber<<11;
}

/*
 * Initialise votre structure permettant de stocker l'état de la mémoire
 * 
 * Retourne:
 * - un pointeur vers une structure initialisée stockant l'état de la mémoire
 */
struct paging_system_state *initMemoryState() {
    // TODO
     struct paging_system_state* pss = malloc (sizeof(struct paging_system_state));

    struct tlb* TLB = malloc (sizeof(struct tlb));
    TLB->pageNumbers = calloc(16, sizeof(unsigned long));
    TLB->frameNumbers =  calloc (16,sizeof(unsigned long));
    TLB->lastAccessTimestamps = calloc (16,sizeof(int));
    TLB->entryCreationTimestamps = calloc (16,sizeof(unsigned int));
    TLB->isUsed = calloc(16,sizeof(char));

    for (int i = 0 ; i< 16 ; i++){
       TLB->isUsed[i] =0;
    }

    struct pt* PT = malloc(sizeof(struct pt));
    PT->frameNumbers = calloc(512,sizeof(unsigned long));
    PT->isValid = calloc(512,sizeof(char));

    for (int i = 0 ; i< 512 ; i++){
       PT->isValid[i] =0;
    }

    struct memory* memoire = malloc (sizeof(struct memory));
    memoire->pageNumbers=calloc(207,sizeof(unsigned long));
    memoire->lastAccessTimestamps= calloc(207,sizeof(unsigned int));
    memoire->entryCreationTimestamps = calloc (207,sizeof(unsigned int));
    memoire->isUsed = calloc(207,sizeof(char));
    for (int i = 0 ; i< 207 ; i++){
        memoire->isUsed[i] =0;
    }

    pss->tlb = TLB;
    pss->pt = PT;
    pss->mem = memoire;
    return (pss);
    
}

/*
 * Cherche la traduction de l'adresse virtuelle dans le TLB.
 * 
 * Si la traduction est trouvée dans le TLB, modifier les champs:
 * - mr->wasFoundInTLB
 * - mr->physicalAddress
 * 
 * Vous devez également mettre à jour les timestamps dans le TLB et la table
 * de pages.
 */
void lookupInTLB(struct paging_system_state *ms,
                 struct memory_request *mr) {
    // TODO
    unsigned long pageNumber = getPageNumber(mr->virtualAddr);
    for (int i = 0 ; i < 16 ; i++)
    {
        if (ms->tlb->pageNumbers[i] == pageNumber && ms->tlb->isUsed[i] == 1) 
        {
            mr->wasFoundInTLB = 1;
            mr->physicalAddr =  getStartPageAddress(ms->tlb->frameNumbers[i]) + getPageOffset(mr->virtualAddr);
            ms->tlb->lastAccessTimestamps[i] = mr->timestamp;
            ms->mem->lastAccessTimestamps[ms->tlb->frameNumbers[i]-455] = mr->timestamp;
            break;
        }
    }
}

/*
 * Cherche la traduction de l'adresse virtuelle dans la table de pages.
 * 
 * Si la traduction est trouvée dans la table de pages, modifier le champ:
 * - mr->physicalAddress
 * 
 * Sinon, modifier le champ:
 * - mr->wasPageFault
 * 
 * Vous devez également mettre à jour les timestamps dans la mémoire centrale.
 */
void lookupInPT(struct paging_system_state *ms,
                struct memory_request *mr) {
    // TODO
    unsigned long pageNumber = getPageNumber(mr->virtualAddr);
    if(ms->pt->isValid[pageNumber] == 1)
    {
        mr->physicalAddr =  getStartPageAddress(ms->pt->frameNumbers[pageNumber])+getPageOffset(mr->virtualAddr);
        ms->mem->lastAccessTimestamps[ms->pt->frameNumbers[pageNumber]-455] = mr->timestamp;
    }
    else 
        mr->wasPageFault = -1 ;
}

/*
 * Ajoute la traduction de l'adresse virtuelle dans le TLB.
 * 
 * Si le TLB est plein, vous devez prendre en compte la politique de
 * remplacement du TLB pour modifier les champs:
 * - mr->wasEvictionInTLB
 * - mr->virtualAddrEvictedFromTLB
 * 
 * N'oubliez pas d'initialiser correctement le timestamp de votre nouvelle
 * entrée dans le TLB.
 * 
 * Attention: Si une page X est retirée de la mémoire où elle est remplacée par
 * une page Y, alors le TLB est mis à jour pour remplacer X par Y.
 */
void addToTLB(struct paging_system_state *ms,
              struct memory_request *mr) {
    // TODO
    unsigned int frame = -1; 
    for (int i = 0 ; i < 16 ; i++)
    {
        if(ms->tlb->isUsed[i] == 0)
        {
            frame =  i+455;
            ms->pt->frameNumbers[getPageNumber(mr->virtualAddr)]=frame;
            
            ms->tlb->pageNumbers[i] = getPageNumber(mr->virtualAddr);
            ms->tlb->frameNumbers[i] = frame;
            ms->tlb->lastAccessTimestamps[i] = mr->timestamp;
            ms->tlb->entryCreationTimestamps[i] = mr->timestamp;
            ms->tlb->isUsed[i] = 1;
            ms->mem->lastAccessTimestamps[frame] = mr->timestamp;
            break;
        }
    }
    if(frame == -1)
    {
        unsigned int leastRecentlyUsed = ms->tlb->lastAccessTimestamps[0]; 
        int indexLRU = 0 ;
        for (int i = 1 ; i < 16 ; i++)
        {
            if (ms->tlb->lastAccessTimestamps[i] < leastRecentlyUsed )
            {
                leastRecentlyUsed = ms->tlb->lastAccessTimestamps[i];
                indexLRU = i;
            }
                
        }

        frame =  indexLRU+455;
        ms->pt->frameNumbers[getPageNumber(mr->virtualAddr)]=frame;
        ms->pt->isValid[getPageNumber(mr->virtualAddr)]=1;
        
        ms->tlb->pageNumbers[indexLRU] = getPageNumber(mr->virtualAddr);
        ms->tlb->frameNumbers[indexLRU] = frame;
        ms->tlb->lastAccessTimestamps[indexLRU] = mr->timestamp;
        ms->tlb->entryCreationTimestamps[indexLRU] = mr->timestamp;
        ms->tlb->isUsed[indexLRU] = 1;
        ms->mem->lastAccessTimestamps[frame] = mr->timestamp;

    }
    
    
}

/*
 * Si cette fonction est appelée en dernier recours, cela signifie que la page
 * demandée n'est même pas présente en mémoire. Il faut donc l'amener en
 * mémoire puis ajouter la traduction dans la table de pages.
 * 
 * Si la mémoire est pleine, vous devez prendre en compte la politique de
 * remplacement de la mémoire pour modifier les champs:
 * - mr->wasEvictionInMemory
 * - mr->virtualAddrEvictedFromMemory
 * 
 * Dans tous les cas, vous devez modifier le champ:
 * - mr->physicalAddress
 */
void getPageIntoMemory(struct paging_system_state *ms,
                       struct memory_request *mr) {
    // TODO
    // bool fullMem = true;
    // int indexEmpty = 0 ; 
    // int indexReplace = -1;
    
    // unsigned long oldPage;
    // unsigned long newPage;

    // unsigned int leastRecentlyUsed = ms->mem->lastAccessTimestamps[0];
    // int indexLRU = 0 ; 
    // for (int i = 0 ; i < 207 ; i ++)
    // {
    //     if (ms->mem->isUsed[i] == '0')
    //     {
    //         fullMem = false;
    //         indexEmpty = i;
    //         indexReplace = indexEmpty
    //         break;
    //     }
    //     if (leastRecentlyUsed > ms->mem->lastAccessTimestamps[i])
    //     {
    //         leastRecentlyUsed = ms->mem->lastAccessTimestamps[i];
    //         indexLRU = -1;
    //     }
    // }

    // if(fullMem)
    // {
    //     indexReplace  = indexLRU;
    //     oldPage = ms->mem->pageNumbers[indexLRU]; 
    //     ms->pt->isValid[oldPage] ='0';
    //     mr->wasEvictionInMemory = 1;
    //     mr->virtualAddrEvictedFromMemory = getStartPageAddress(oldPage)+getPageOffset(mr->virtualAddr);
    //     ms->mem->entryCreationTimestamps[oldPage] = mr->timestamp;
    //     //ajout dans TP
    //     //Remplir les attrib de mr

    // }
        
    //     ms->mem->pageNumbers[indexReplace] = getPageNumber(mr->virtualAddr);
    //     ms->mem->lastAccessTimestamps[indexReplace] = mr->timestamp;
    //     ms->mem->entryCreationTimestamps[indexReplace] = mr->timestamp;
    //     ms->mem->isUsed[indexReplace] = '1';
    //     newPage = ms->mem->pageNumbers[indexReplace];

    //     ms->pt->frameNumbers[newPage] = indexReplace; //if it doesn't work try to remove the 455
    //     ms->pt->isValid[newPage] = '1' ; 
        
    //     unsigned long adressePhysique = getStartPageAddress(0xe3800+newPage)|getPageOffset(mr->virtualAddr);
    //     mr->physicalAddr = adressePhysique;
    bool isfull = true;
    int i;
    for ( i = 0 ; i< 207 ; i++) {
        if (ms->mem->isUsed[i] != 1){
            isfull = false;
            break ; 
        } 
    }

    if(!isfull){
        mr->physicalAddr = (getStartPageAddress(i+455)) | getPageOffset(mr->virtualAddr);
        ms-> mem->entryCreationTimestamps[i] = mr->timestamp;
        ms-> mem->lastAccessTimestamps[i] = mr->timestamp;
        ms->mem->pageNumbers[i] = getPageNumber(mr->virtualAddr);
        ms->pt->frameNumbers[getPageNumber(mr->virtualAddr)] = i+455;
        ms->pt->isValid[getPageNumber(mr->virtualAddr)]=1;
        ms->mem->isUsed[i] = 1;
    }else{
        
       
        int size = 207;
        int lastAccessElem = 0 ;
        int valueAcess = ms->mem->lastAccessTimestamps[0];
        for (int j = 0 ; j < size ; j++){
            if (ms->mem->lastAccessTimestamps[j]<valueAcess){
                lastAccessElem = j ;
                valueAcess = ms->mem->lastAccessTimestamps[j];
            }
        }

        mr->wasEvictionInMemory = 1;
        ms->pt->isValid[ms->mem->pageNumbers[lastAccessElem]]=0;

        mr->virtualAddrEvictedFromMemory = (getStartPageAddress(ms->mem->pageNumbers[lastAccessElem])|getPageOffset(mr->virtualAddr));


        ms->mem->pageNumbers[lastAccessElem] = getPageNumber(mr->virtualAddr);

        ms->mem->lastAccessTimestamps[lastAccessElem] = mr->timestamp;
        ms->mem->entryCreationTimestamps[lastAccessElem] =  mr->timestamp;

        unsigned long elementToReplace = ms->mem->pageNumbers[lastAccessElem];

        mr->physicalAddr = getStartPageAddress(455+lastAccessElem) | getPageOffset(mr->virtualAddr);
        ms->pt->frameNumbers[getPageNumber(mr->virtualAddr)] = lastAccessElem;
        ms->pt->isValid[getPageNumber(mr->virtualAddr)]=1;
        // ms-> mem->entryCreationTimestamps[i] = mr->timestamp;
        // ms-> mem->lastAccessTimestamps[i] = mr->timestamp;
   }

}

/*
 * Traite une demande d'accès à la mémoire.
 * 
 * Cette fonction mute les structures fournies en arguments pour modifier
 * l'état de la mémoire et donner des informations sur la demande d'accès en
 * argument (traduction en adresse physique, présence ou non de défaut de page,
 * présence ou non de la traduction dans le TLB...)
 * 
 * Arguments:
 * - un pointeur vers votre structure représentant l'état de la mémoire
 * - un pointeur vers une structure représentant la demande d'accès
 */
void processMemoryRequest(struct paging_system_state *ms,
                          struct memory_request *mr) {
    lookupInTLB(ms, mr);
    if (mr->wasFoundInTLB == 1)
    {
        return;
    }
    /* Lookup in page table */
    lookupInPT(ms, mr);
    if (mr->wasPageFault == 0)
    {
        addToTLB(ms, mr);
        return;
    }
    /* Get frame in memory */
    getPageIntoMemory(ms, mr);
    addToTLB(ms, mr);
}

/*
 * Désalloue votre structure permettant de stocker l'état de la mémoire
 * 
 * Arguments:
 * - un pointeur vers votre structure stockant l'état de la mémoire
 */
void cleanMemoryState(struct paging_system_state *ms) {
    // TODO
    free(ms->tlb->entryCreationTimestamps);
    free(ms->tlb->isUsed);
    free(ms->tlb->frameNumbers);
    free(ms->tlb->lastAccessTimestamps);
    free(ms->tlb);
    
    free(ms->pt->frameNumbers);
    free(ms->pt->isValid);
    free(ms->pt);

    free(ms->mem->pageNumbers);
    free(ms->mem->lastAccessTimestamps);
    free(ms->mem->entryCreationTimestamps);
    free(ms->mem->isUsed);
    free(ms->mem);

    free(ms);
}
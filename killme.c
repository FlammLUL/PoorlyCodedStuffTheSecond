#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

#include "cityObjects.h"

void* listCreate(char type){
    switch(type){
        case 'b':
            cbList *cbHead = (cbList*) malloc(sizeof(cbList));
            cbHead->next = NULL;
            cbHead->prev = NULL;
            return cbHead;
        case 'h':
            hList *hHead = (hList*) malloc(sizeof(hList));
            hHead->next = NULL;
            hHead->prev = NULL;
            return hHead;
        case 't':
            tlList *tlHead = (tlList*) malloc(sizeof(tlList));
            tlHead->next = NULL;
            tlHead->prev = NULL;
            return tlHead;
        case 's':
            csList *csHead = (csList*) malloc(sizeof(csList));
            csHead->next = NULL;
            csHead->prev = NULL;
            return csHead;
    }
}

int listLength(void* head, char type){
    int nodeAmount = 0;

    switch(type){
        case 'b':
            cbList *aux = (cbList*) head;
            break;
        case 'h':
            hList *aux = (hList*) head;
            break;
        case 't':
            tlList *aux = (tlList*) head;
            break;
        case 's':
            csList *aux = (csList*) head;
            break;
    }

    while(aux->next != NULL){
        aux = aux->next;
        nodeAmount++;
    }

    return nodeAmount;
}

void listInsertFinal(FILE* input, void** head, char type){
    char idDummy[30];
    float x, y;

    // Ainda falta adicionar o negócio para setar a cor dessa porra

    fscanf(input, "%s %f %f", idDummy, &x, &y);

    if(type == 'b'){
        float width, height;
        fscanf(input, "%f %f", &width, &height);
    }

    size_t q = sizeof(idDummy);
    q += 1;

    switch(type){
        case 'b':
            CityBlock cb;
            cb.x = x;
            cb.y = y;
            cb.zip = (char*) malloc(q * sizeof(char))
            strcpy(idDummy, cb.zip);
            cb.width = width;
            cb.height = height;

            cbList *findTail = (cbList*) *head;
            cbList *aux = (cbList*) malloc(sizeof(cbList));
            
            aux->cb = cb
            break;
        case 'h':
            Hydrant h;
            h.x = x;
            h.y = y;
            h.id = (char*) malloc(q * sizeof(char));
            strcpy(idDummy, h.id);

            hList *findTail = (hList*) *head;
            hList *aux = (hList*) malloc(sizeof(hList));

            aux->h = h;
            break;   
        case 't':
            TrafficLight tl;
            tl.x = x;
            tl.y = y;
            tl.id = (char*) malloc(q * sizeof(char));
            strcpy(idDummy, tl.id);

            tlList *findTail = (tlList*) *head;
            tlList *aux = (tlList*) malloc(sizeof(hList));

            aux->tl = tl;
            break;
        case 's':
            CellSite cs;
            cs.x = x;
            cs.y = y;
            cs.id = (char*) malloc(q * sizeof(char));
            strcpy(idDummy, cs.id);

            csList *findTail = (csList*) *head;
            csList *aux = (csList*) malloc(sizeof(csList));

            aux->cs = cs;
            break;
    }

    if(*head == NULL){
        *head = aux;
        return;
    }

    while(findTail->next != NULL){
        findTail = findTail->next;
    }
    
    aux->prev = findTail;
    findTail->next = aux;
    aux->next = NULL;
}

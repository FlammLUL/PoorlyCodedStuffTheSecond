#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "environment.h"

/* Função usada para definir o "ambiente" de trabalho, como, por exemplo, os paths */
int setEnvironmentExtensions(Environment *e){
    int fSize;
    int nSize;
    int oSize;
    int pSize;

    char *pOut;
    char *p;

    fSize = strlen(e->inputFolderPath);
    nSize = strlen(e->inputFileName);
    oSize = strlen(e->outputFilePath);

    e->inputActualFilePath = (char*) malloc((fSize + nSize + 3) * sizeof(char));

    strcpy(e->inputActualFilePath, e->inputFolderPath);
    if(*(e->inputActualFilePath + fSize - 1) != '/'){
        strcat(e->inputActualFilePath, "/");
    }    
    if(*(e->outputFilePath + oSize - 1) != '/'){
        strcat(e->outputFilePath, "/");
    }
    strcat(e->inputActualFilePath, e->inputFileName);

    pOut = strrchr(e->inputActualFilePath, '/');
    pOut++;
    oSize = 1;
    while(*(pOut + oSize) != '.'){
        oSize += 1;
    }
    e->outputFileName = (char*) malloc((oSize + 2) * sizeof(char));
    oSize = 1;

    p = strstr(pOut, ".geo");
    strncpy(e->outputFileName, pOut, strlen(pOut) - strlen(p) + 1);

    oSize = strlen(e->outputFilePath);
    oSize += strlen(e->outputFileName);
    oSize += 5;

    e->outputActualFilePathSvg = (char*) malloc(oSize * sizeof(char));
    e->outputActualFilePathTxt = (char*) malloc(oSize * sizeof(char));

    strcpy(e->outputActualFilePathSvg, e->outputFilePath);
    strcat(e->outputActualFilePathSvg, e->outputFileName);
    strcpy(e->outputActualFilePathTxt, e->outputActualFilePathSvg);
    strcpy(e->outputFilePath, e->outputActualFilePathSvg);
    strcat(e->outputActualFilePathSvg, "svg");
    strcat(e->outputActualFilePathTxt, "txt");

    return 0;
}
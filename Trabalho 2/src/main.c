#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "environment.h"
#include "flammQueue.h"
#include "svgUtils.h"

int main(int argc, char **argv){
    /*
        Definição de algumas variáveis pertinentes
    */
    int maximumFigureAmount;
    int nowFigureAmount;

    Environment *e; /* Variável de ambiente, feito para guardar coisas úteis */
    Queue *qHead;

    int i;  /* Iterador */

    /* Ponteiros para os arquivos de texto/SVG */
	FILE *input;
	FILE *output;
	FILE *commandOutput;

    /* Variáveis que auxiliam no parseamento dos comandos */
    char command[3];

    int iId;
    double iPx, iPy;

    int dId1, dId2;
    double dOutput;

    int oId1, oId2;

    /* Variáveis que controlam a quantidade máxima de objetos */
    maximumFigureAmount = 1000;
    nowFigureAmount = 0;

    /* Inicializando a fila e as variáveis de ambiente */
    e = (Environment*) malloc(sizeof(Environment));
    qHead = (Queue*) malloc(sizeof(Queue));
    qHead = NULL;

    /* Retirando os argumentos da linha de comando, espero não dar ruim para o "-o" */
    for(i = 1; i < argc; i++){
        if(strcmp(argv[i], "-e") == 0){
            e->inputFolderPath = malloc((strlen(argv[i + 1]) + 1) * sizeof(char));
            strcpy(e->inputFolderPath, argv[i + 1]);
        } else if(strcmp(argv[i], "-f") == 0){
            e->inputFileName = malloc((strlen(argv[i + 1]) + 1) * sizeof(char));
            strcpy(e->inputFileName, argv[i + 1]);
        } else if(strcmp(argv[i], "-o") == 0) {
            e->outputFilePath = malloc((strlen(argv[i + 1]) + 1) * sizeof(char));
            strcpy(e->outputFilePath, argv[i + 1]);
        }
    }
    setEnvironmentExtensions(e);

    /* Abrindo o arquivo de entrada para ver os comandos a serem realizados */
    input = fopen(e->inputActualFilePath, "r");

    while(1){
        fscanf(input, "%s", command);
        if(strcmp(command, "nx") == 0){
            fscanf(input, "%d", &maximumFigureAmount);
        } else if(strcmp(command, "c") == 0){
            if(nowFigureAmount <= maximumFigureAmount){
                fQueueInsert(&qHead, createCircle(input));
                nowFigureAmount += 1;
            } else {
                break;
            }
        } else if(strcmp(command, "r") == 0){
            if(nowFigureAmount <= maximumFigureAmount){
                fQueueInsert(&qHead, createRectangle(input));
                nowFigureAmount += 1;
            } else {
                break;
            }
        } else if(strcmp(command, "o") == 0){
            fscanf(input, "%d %d", &oId1, &oId2);
            commandOutput = fopen(e->outputActualFilePathTxt, "a");
            fprintf(commandOutput, "o %d %d\n", oId1, oId2);
            if(checkOverlay(qHead, oId1, oId2)){
                fprintf(commandOutput, "SIM\n");
            } else {
                fprintf(commandOutput, "NAO\n");
            }
        } else if(strcmp(command, "i") == 0){
            fscanf(input, "%d %lf %lf", &iId, &iPx, &iPy);
            commandOutput = fopen(e->outputActualFilePathTxt, "a");
            fprintf(commandOutput, "i %d %lf %lf\n", iId, iPx, iPy);            
            if(isInternal(searchForID(qHead, iId), iPx, iPy)){
                fprintf(commandOutput, "SIM\n");
            } else {
                fprintf(commandOutput, "NAO\n");
            }
            fclose(commandOutput);
        } else if(strcmp(command, "d") == 0){
            fscanf(input, "%d %d", &dId1, &dId2);
            dOutput = getDistanceWithOutput(qHead, dId1, dId2);
            commandOutput = fopen(e->outputActualFilePathTxt, "a");
            fprintf(commandOutput, "d %d %d\n%lf\n", dId1, dId2, dOutput);
            fclose(commandOutput);
        } else if(strcmp(command, "a") == 0){
            makeLinesSVG(input, e, qHead);
        } else if(strcmp(command, "#") == 0){
            break;
        }
    }

    /* Cria um arquivo .SVG contendo apenas as figuras geométricas geradas */
    makeSVG(e, qHead);

    /* Libera a memória utilizada para a fila apenas por precaução */
    for(i = 0; i < nowFigureAmount; i++){
        fQueuePop(&qHead);
    }

    fclose(input);
}

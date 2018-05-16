#ifndef ENVIRONMENT_H_
#define ENVIRONMENT_H_

/*  Ambiente:
        - Armazena algumas variáveis importantes que poderão ser utilizadas em vários momentos no programa
        - Fácil expansão, não requer mudar as funções para colocar aquelas aberrações com 10 argumentos (sem saudades do T1)
*/

typedef struct environment{
    char *inputFolderPath;
    char *inputFileName;
    char *outputFileName;
    char *outputFilePath;

    char *inputActualFilePath;
    char *outputActualFilePathSvg;
    char *outputActualFilePathTxt;
    char *outputPathForCommandA;
} Environment;

int setEnvironmentExtensions(Environment*);
#endif 
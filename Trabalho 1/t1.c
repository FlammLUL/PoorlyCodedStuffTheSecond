#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define STRING_MAX_SIZE 100
// Tamanho máximo de uma string

// Prototipagem das funções
void doCommands();

char inputFile[STRING_MAX_SIZE];
char dirPath[STRING_MAX_SIZE];
char commandLineParameter[3][STRING_MAX_SIZE]; // @1 = [0], @2 = [1]; @3 = [2]

int main(int argc, char **argv){
    /*
            Parseando a command line
    */
    for(int i = 0; i < argc; ++i){
        int parameterIndex = 0;
        if(strcmp(argv[i], "-f") == 0){ // Diretório do inputFile
            i++;
            strcpy(inputFile, argv[i]);
        } else if(strcmp(argv[i], "-o") == 0){ // Especificando um valor default para d
            i++;
            strcpy(dirPath, argv[i]);
        } else { // Leitura dos parâmetros @1, @2 e @3
            if(commandLineParameter[parameterIndex][0] != '\0'){
                // Caso o parâmetro @1 não estiver vazio, passar para @2, ...
                parameterIndex++;
            }
            strcpy(argv[i], commandLineParameter[parameterIndex]);
        }
    }
    doCommands();
    return 0;
}

void doCommands(){
    // FilePath
}

/*
    Pensar em como fazer a leitura
*/

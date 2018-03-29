#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define STRING_MAX_SIZE 500
// Tamanho máximo de uma string

// Prototipagem das funções
void removeLineAtBufferEnd(char*);

void doCommands(FILE*, char*, char*, char*);
void concatenateVariableNormal(char*, FILE*, char*, char*, char*);
void concatenateVariableSpecial(char*, FILE*, char*, char*, char*);


int main(int argc, char **argv){
    FILE *input;
    char dirPath[STRING_MAX_SIZE];
    char commandLineParameter[3][STRING_MAX_SIZE]; // @1 = [0], @2 = [1]; @3 = [2]

    // Inicializando os parâmetros @1, @2 e @3 como \0
    for(int i = 0; i < 3; i++){
        commandLineParameter[i][0] = '\0';
    }

    for(int i = 0; i < argc; i++){
        int parameterIndex = 0;
        if(strcmp(argv[i], "-f") == 0){ // Diretório do inputFile
            i++;
            input = fopen(argv[i], "r");
        } else if(strcmp(argv[i], "-o") == 0){ // Especificando um valor default para d
            i++;
            strcpy(dirPath, argv[i]);
        } else { // Leitura dos parâmetros @1, @2 e @3
            if(commandLineParameter[parameterIndex][0] != '\0'){
                // Caso o parâmetro @1 não estiver vazio, passar para @2, ...
                parameterIndex++;
            }
            strcpy(commandLineParameter[parameterIndex], argv[i]);
        }
    }
    doCommands(input, commandLineParameter[0], commandLineParameter[1], commandLineParameter[2]);
    return 0;
}

void doCommands(FILE *input, char *p1, char *p2, char *p3){
    char *xVar = malloc(sizeof(char)), *yVar = malloc(sizeof(char)), *zVar = malloc(sizeof(char));
    char command[3];
    //printf("Parou antes do while\n");
    while(fscanf(input, "%s", command) != EOF){
        printf("%s", command);
        if(strcmp(command, "+x") == 0){
            concatenateVariableNormal(xVar, input, p1, p2, p3);
        } else if(strcmp(command, "+y") == 0){
            concatenateVariableNormal(yVar, input, p1, p2, p3);
        } else if(strcmp(command, "+z") == 0){
            concatenateVariableSpecial(zVar, input, p1, p2, p3);
        } else {
            //switch(command[0]){}
        }
    }
    printf("%s\n", xVar); // Teste
}

// Para casos x, y
void concatenateVariableNormal(char *var, FILE *input, char *p1, char *p2, char *p3){
    char removeLeadingWhitespace = fgetc(input);
    printf("%c", removeLeadingWhitespace);

    char toBeConcatenated[STRING_MAX_SIZE];
    fgets(toBeConcatenated, STRING_MAX_SIZE, input);
    toBeConcatenated[strcspn(toBeConcatenated, "\n")] = 0; // Limpar o caracter \n

    printf("%s\n", toBeConcatenated);

    if(strcmp(toBeConcatenated, "@1") == 0){
        var = realloc(var, (strlen(var) + strlen(p1) + 1) * sizeof(char));
        printf("strcat p1!\n");
        printf("Test: %s %s\n", var, p1);
        strcat(var, p1);
        return;
    } else if(strcmp(toBeConcatenated, "@2") == 0){
        var = realloc(var, (strlen(var) + strlen(p2) + 1) * sizeof(char));
        printf("strcat p2!\n");
        printf("Test: %s %s\n", var, p2);
        strcat(var, p2);
        return;
    } else if(strcmp(toBeConcatenated, "@3") == 0){
        var = realloc(var, (strlen(var) + strlen(p3) + 1) * sizeof(char));
        printf("strcat p3!\n");
        printf("Test: %s %s\n", var, p3);
        strcat(var, p3);
        return;
    }
}

void concatenateVariableSpecial(char *var, FILE *input, char *p1, char *p2, char *p3){
    // Para caso z
    char toBeConcatenated[STRING_MAX_SIZE];
    fgets(toBeConcatenated, STRING_MAX_SIZE, input);
}
/*
    Pensar em como fazer a leitura
*/

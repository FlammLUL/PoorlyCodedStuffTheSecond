#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define STRING_MAX_SIZE 500
// Tamanho máximo de uma string

/*
    Pensando um pouco melhor,
    acho que dava pra ter feito os parâmetros tudo dentro de uma struct
    pra não fazer funções com 20 parâmetros, mas ok.
    Muito tarde para mudar.
*/

struct FILES{
    FILE* filePointer;
    char filePath[STRING_MAX_SIZE];
    int id;
};

typedef struct FILES Files;

// Funções: utilidades
void removeLineAtBufferEnd(char*);
void removeCharFromString(char**, char);
int ctoi(char);

// Funções: comandos (é muito bom não saber que parâmetro é o que aqui)
void doCommands(FILE*, char*, char*, char*, char*, char*);
void concatenateVariableNormal(char**, FILE*, char*, char*, char*);
void concatenateVariableSpecial(char**, FILE*, char*);
void clearVariable(char**);
void setCurrentDirectory(FILE*, char**, char**, char*, char*, char*);
void setFileName(char**, FILE*, char*, char*, char*);
void setExtension(char**, FILE*, char*, char*, char*);
void createFile(Files[10], int, char**, char**, char**);
void openFile(Files[10], int, char**, char**, char**);
void readFile(Files[10], int);
void writeFile(FILE*, Files[10], int, char**, char**, char**);
void readLineFromFile(FILE*, Files[10], int, char**, char**, char**);
void closeFile(Files[10], int);


int main(int argc, char **argv){
    FILE *input; // Arquivo de entrada ditado pelo argumento da linha de comando -f [inputDir]
    char dirPath[STRING_MAX_SIZE];
    char commandLineParameter[3][STRING_MAX_SIZE]; // @1 = [0], @2 = [1]; @3 = [2]

    // Inicializando os parâmetros @1, @2 e @3 como \0
    for(int i = 0; i < 3; i++){
        commandLineParameter[i][0] = '\0';
    }

    input = fopen(argv[2], "r"); // Abrindo o arquivo de entrada
    int parameterIndex = 0; // Parâmetros para pegar @1, @2 e @3

    // Verificando se a opção "-o" existe e, posteriormente, pegando os parâmetros
    if(strcmp(argv[2], "-o") == 0){
        strcpy(dirPath, argv[3]);
        for(int i = 5; i < argc; i++){
            if(commandLineParameter[parameterIndex][0] == '\0'){
                strcpy(commandLineParameter[parameterIndex], argv[i]);
                parameterIndex++;
            }
        }
    } else {
        for(int i = 3; i < argc; i++){
            if(commandLineParameter[parameterIndex][0] == '\0'){
                strcpy(commandLineParameter[parameterIndex], argv[i]);
                parameterIndex++;
            }
        }
    }

    // Ponteiros para potencial modificação posterior
    char *dirPathPointer = dirPath;
    char *defaultDirPathPointer = argv[3];

    // Remover o ponto inicial da string
    removeCharFromString(&dirPathPointer, '.');

    // Realização dos comandos...
    doCommands(input, commandLineParameter[0], commandLineParameter[1], commandLineParameter[2], dirPathPointer, defaultDirPathPointer);
    return 0;
}

// Command parser: análise dos comandos e leitura de argumentos, caso necessário
void doCommands(FILE *input, char *p1, char *p2, char *p3, char *dirPointer, char *defaultDirPointer){
    // Inicialização de variáveis x, y, z
    char *xVar = "", *yVar = "", *zVar = "";

    // Arrays para nome de arquivo extensão e seus respectivos ponteiros para potencial modificação posterior
    char fileName[STRING_MAX_SIZE]; char *namePointer = fileName;
    char fileExt[STRING_MAX_SIZE]; char *extPointer = fileExt;

    // Array de files
    Files files[10];

    // Sim, isso é o cúmulo da preguiça
    // Um char que armazena o whitespace " " após um comando/argumento (no caso de existir um segundo argumento)
    // para evitar criar uma função que retirasse o 'leading whitespace' de uma string
    char removeLeadingWhitespace;

    // ID de um arquivo
    int id;

    // Armazenador do comando a ser lido toda linha
    char command[3];

    // Loop para a seleção e realização dos comandos
    while(fscanf(input, "%s", command) != EOF){
        // Apenas com função de debug
        printf("%s ", command);

        // Funções de concatenação...
        if(strcmp(command, "+x") == 0){
            concatenateVariableNormal(&xVar, input, p1, p2, p3);
        } else if(strcmp(command, "+y") == 0){
            concatenateVariableNormal(&yVar, input, p1, p2, p3);
        } else if(strcmp(command, "+z") == 0){
            concatenateVariableSpecial(&zVar, input, p1);
        } else {
            switch(command[0]){
            // Funções de limpeza de variável
            case 'x':
                clearVariable(&xVar);
                break;
            case 'y':
                clearVariable(&yVar);
                break;
            case 'z':
                clearVariable(&zVar);
                break;
            // Configurar o diretório padrão
            case 'd':
                setCurrentDirectory(input, &dirPointer, &defaultDirPointer, xVar, yVar, zVar);
                break;
            // Configurar o nome do arquivo
            case 'a':
                setFileName(&namePointer, input, xVar, yVar, zVar);
                break;
            // Configurar a extensão do arquivo
            case 'e':
                setExtension(&extPointer, input, xVar, yVar, zVar);
                break;
            // Criar um arquivo com um ID no diretório padrão
            case 'c':
                fscanf(input, "%d", &id);
                createFile(files, id, &dirPointer, &namePointer, &extPointer);
                break;
            // Abrir um arquivo para inclusão (append)
            case 'o':
                fscanf(input, "%d", &id);
                openFile(files, id, &dirPointer, &namePointer, &extPointer);
                break;
            // Abrir um arquivo para leitura (read)
            case 'l':
                fscanf(input, "%d", &id);
                readFile(files, id);
                break;
            // Abrir um arquivo para escrita (write) de uma string sem aspas duplas a delimitando e terminada pelo caracter '$'
            case 'w':
                fscanf(input, "%d", &id);
                writeFile(input, files, id, &xVar, &yVar, &zVar);
                break;
            // Abrir um arquivo para leitura e armazenamento de uma linha em uma variável
            case 'r':
                fscanf(input, "%d", &id);
                readLineFromFile(input, files, id, &xVar, &yVar, &zVar);
                break;
            // Por fim, fechar um arquivo que está aberto
            case 'f':
                fscanf(input, "%d", &id);
                closeFile(files, id);
                break;
            }
        }
    }
}

////////////////////////
//      Comandos      //
////////////////////////

// Para casos x, y
void concatenateVariableNormal(char **var, FILE *input, char *p1, char *p2, char *p3){
    char removeLeadingWhitespace = fgetc(input);

    // A função que retira o argumento dessa função
    char toBeConcatenated[STRING_MAX_SIZE];
    fgets(toBeConcatenated, STRING_MAX_SIZE, input);

    // Retirar o caracter '\n' (new line) do final da string
    toBeConcatenated[strcspn(toBeConcatenated, "\n")] = 0;

    // Apenas para testes, será removida posteriormente
    printf("%s\n", toBeConcatenated);

    // Tamanho da variável (x, y ou z) para posterior utilização na alocação de memória
    int varSize = strlen(*var);

    // String auxiliar para cópia e posterior concatenação.
    char dummyString[STRING_MAX_SIZE];

    // Comparação dos argumentos da função
    if(strcmp(toBeConcatenated, "@1") == 0){
        // Tamanho para alocação de memória posterior
        int pSize = strlen(p1);

        // Temporário, será retirado mais tarde
        printf("varSize: %d \tpSize: %d\n", varSize, pSize);

        // Faz uma cópia da variável
        strcpy(dummyString, *var);

        // Libera toda a memória alocada para a variável e a realoca, incluindo um espaço extra para o caracter de finalização de uma string '\0'
        free(*var);
        *var = malloc((pSize + varSize + 1) * sizeof(char));

        // O processo inverso ocorre: a string auxiliar é copiada para o endereço da variável, que é concatenada com o parâmetro correspondente
        strcpy(*var, dummyString);
        strcat(*var, p1);

        // Mais uma vez, um testezinho
        printf("Test: %s\n", *var);

        // Por fim, retorna
        return;
    // E o mesmo processo é repetido duas vezes
    } else if(strcmp(toBeConcatenated, "@2") == 0){
        int pSize = strlen(p2);
        printf("varSize: %d \tpSize: %d\n", varSize, pSize);
        strcpy(dummyString, *var);
        free(*var);
        *var = malloc((pSize + varSize + 1) * sizeof(char));
        strcpy(*var, dummyString);
        strcat(*var, p2);
        printf("Test: %s\n", *var);
        return;
    } else if(strcmp(toBeConcatenated, "@3") == 0){
        int pSize = strlen(p3);
        printf("varSize: %d \tpSize: %d\n", varSize, pSize);
        strcpy(dummyString, *var);
        free(*var);
        *var = malloc((pSize + varSize + 1) * sizeof(char));
        strcpy(*var, dummyString);
        strcat(*var, p3);
        printf("Test: %s\n", *var);
        return;
    // Se passar por aqui, quer dizer que caímos em uma string delimitada por aspas duplas
    } else {
        // Criando um ponteiro que aponta para a string e removendo as suas aspas duplas
        char *p = toBeConcatenated;
        removeCharFromString(&p, '\"');

        // E repetimos o mesmo processo
        int pSize = strlen(toBeConcatenated);
        strcpy(dummyString, *var);
        free(*var);
        *var = malloc((pSize + varSize + 1) * sizeof(char));
        strcpy(*var, dummyString);
        strcat(*var, toBeConcatenated);
        printf("Test: %s\n", *var);
        return;
    }
}

// Para caso z
void concatenateVariableSpecial(char **var, FILE *input, char *p1){
    char removeLeadingWhitespace = fgetc(input);

    char toBeConcatenated[STRING_MAX_SIZE];
    fgets(toBeConcatenated, STRING_MAX_SIZE, input);
    toBeConcatenated[strcspn(toBeConcatenated, "\n")] = 0; // Limpar o caracter \n

    printf("%s\n", toBeConcatenated);

    int varSize = strlen(*var);
    char dummyString[STRING_MAX_SIZE];

    if(strcmp(toBeConcatenated, "**") == 0){
        strcpy(dummyString, *var);
        int n = atoi(p1);
        free(var);
        *var = malloc(((n * varSize) + 1) * sizeof(char));
        for(int i = 0; i < n; i++){
            if(i == 0){
                strcpy(*var, dummyString);
            } else {
                strcat(*var, dummyString);
            }
        }
    } else if(toBeConcatenated[0] == '*'){
        strcpy(dummyString, *var);
        int n = ctoi(toBeConcatenated[1]);
        free(var);
        *var = malloc(((n * varSize) + 1) * sizeof(char));
        for(int i = 0; i < n; i++){
            if(i == 0){
                strcpy(*var, dummyString);
            } else {
                strcat(*var, dummyString);
            }
        }
    } else {
        // Função para remover as aspas de uma string
        char *p = toBeConcatenated;
        removeCharFromString(&p, '\"');
        int pSize = strlen(toBeConcatenated);
        strcpy(dummyString, *var);
        free(*var);
        *var = malloc((pSize + varSize + 1) * sizeof(char));
        strcpy(*var, dummyString);
        strcat(*var, toBeConcatenated);
        printf("Test: %s\n", *var);
        return;
    }
}

void clearVariable(char **var){
    free(*var);
    *var = malloc(sizeof(char));
    strcpy(*var, "");
}

void setCurrentDirectory(FILE* input, char **path, char **defaultPath, char *x, char *y, char *z){
    char removeLeadingWhitespace = fgetc(input);

    char dirArg[STRING_MAX_SIZE];
    fgets(dirArg, STRING_MAX_SIZE, input);
    dirArg[strcspn(dirArg, "\n")] = 0;

    if(strcmp(dirArg, "#x") == 0){
        strcpy(*path, x);
    } else if(strcmp(dirArg, "#y") == 0){
        strcpy(*path, y);
    } else if(strcmp(dirArg, "#z") == 0){
        strcpy(*path, z);
    } else {
        strcpy(*path, dirArg);
    }
    // Função para remover ocorrências de barras duplas ("//")
    removeCharFromString(path, '.');
    printf("%s\n", *path);
}

void setFileName(char **fileName, FILE* input, char *x, char *y, char *z){
    char removeLeadingWhitespace = fgetc(input);

    char nameArg[STRING_MAX_SIZE];
    fgets(nameArg, STRING_MAX_SIZE, input);
    nameArg[strcspn(nameArg, "\n")] = 0;

    if(strcmp(nameArg, "#x") == 0){
        strcpy(*fileName, x);
    } else if(strcmp(nameArg, "#y") == 0){
        strcpy(*fileName, y);
    } else if(strcmp(nameArg, "#z") == 0){
        strcpy(*fileName, z);
    } else {
        char *argPointer = nameArg;
        removeCharFromString(&argPointer, '\"');
        strcpy(*fileName, argPointer);
    }
    printf("%s\n", *fileName);
}

void setExtension(char **fileExt, FILE* input, char *x, char *y, char *z){
    char removeLeadingWhitespace = fgetc(input);

    char extArg[STRING_MAX_SIZE];
    fgets(extArg, STRING_MAX_SIZE, input);
    extArg[strcspn(extArg, "\n")] = 0;

    if(strcmp(extArg, "#x") == 0){
        strcpy(*fileExt, x);
    } else if(strcmp(extArg, "#y") == 0){
        strcpy(*fileExt, y);
    } else if(strcmp(extArg, "#z") == 0){
        strcpy(*fileExt, z);
    } else {
        char *extPointer = extArg;
        removeCharFromString(&extPointer, '\"');
        strcpy(*fileExt, extPointer);
    }
    printf("%s\n", *fileExt);
}

void createFile(Files f[10], int id, char **path, char **fileName, char **extName){
    char dummyString[STRING_MAX_SIZE];

    strcpy(dummyString, *path);
    strcat(dummyString, *fileName);
    strcat(dummyString, ".");
    strcat(dummyString, *extName);

    f[id].filePointer = fopen(dummyString, "w");
    if(!f[id].filePointer){
        perror("fopen");
    }
    strcpy(f[id].filePath, dummyString);
    printf("%d (Path: \"%s\")\n", id, f[id].filePath);
}

void openFile(Files f[10], int id, char **path, char **fileName, char **extName){
    char dummyString[STRING_MAX_SIZE];
    strcpy(dummyString, *path);
    strcat(dummyString, *fileName);
    strcat(dummyString, ".");
    strcat(dummyString, *extName);

    f[id].filePointer = fopen(dummyString, "a");
    strcpy(f[id].filePath, dummyString);
    printf("%d %s\n", id, f[id].filePath);
}

void readFile(Files f[10], int id){
    f[id].filePointer = fopen(f[id].filePath, "r");
    printf("%d\n", id);
}

void writeFile(FILE* input, Files f[10], int id, char **x, char **y, char **z){
    if(fopen(f[id].filePath, "w") == NULL){
        printf("Erro: nao existe um arquivo aberto no id %d!\n", id);
        exit(EXIT_FAILURE);
    } else {
        char removeLeadingWhiteSpace = fgetc(input);

        char toBeWritten[STRING_MAX_SIZE];
        fgets(toBeWritten, STRING_MAX_SIZE, input);
        toBeWritten[strcspn(toBeWritten, "\n")] = 0; // Limpar o caracter \n
        printf("s\n", toBeWritten);

        if(strcmp(toBeWritten, "#x") == 0){
            fprintf(f[id].filePointer, "%s", *x);
        } else if(strcmp(toBeWritten, "#y") == 0){
            fprintf(f[id].filePointer, "%s", *y);
        } else if(strcmp(toBeWritten, "#z") == 0){
            fprintf(f[id].filePointer, "%s", *z);
        } else {
            toBeWritten[strcspn(toBeWritten, "$")] = 0; // Limpar o caracter $
            fprintf(f[id].filePointer, "%s", toBeWritten);
        }
    }
}

void readLineFromFile(FILE* input, Files f[10], int id, char **x, char **y, char **z){
    char removeLeadingWhiteSpace = fgetc(input);

    char toBeWritten[STRING_MAX_SIZE];
    fgets(toBeWritten, STRING_MAX_SIZE, input);
    toBeWritten[strcspn(toBeWritten, "\n")] = 0; // Limpar o caracter \n

    if(strcmp(toBeWritten, "#x") == 0){
        strcpy(*x, toBeWritten);
    } else if(strcmp(toBeWritten, "#y") == 0){
        strcpy(*y, toBeWritten);
    } else if(strcmp(toBeWritten, "#z") == 0){
        strcpy(*z, toBeWritten);
    }
}

void closeFile(Files f[10], int id){
    fclose(f[id].filePointer);
    printf("%d\n", id);
}

///////////////////////////////////
//          Utilidades           //
///////////////////////////////////

void removeCharFromString(char **str, char c){
    char *pRead = *str, *pWrite = *str;
    while(*pRead){
        *pWrite = *pRead++;
        pWrite += (*pWrite != c);
    }
    *pWrite = '\0';
}

int ctoi(char c){
    return c - '0';
}

void mkpath(char *path){
    char p[STRING_MAX_SIZE];
    char pFull[STRING_MAX_SIZE];

    strcpy(p, path);
    char *pPointer = strtok(p, "/")

    int c = 0;

    while(pPointer != NULL){
        if(c != 1){
            c == 1;
            strcpy(pFull, p);
        } else {
            strcat(pFull, "/");
            strcat(pFull, p);
        }
        //
    }
    strcpy(p, path);
}

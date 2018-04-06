#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// Definindo um tamanho máximo para uma string
// Isso tem nome: preguiça, fiz isso para evitar usar malloc e free inúmeras vezes
// Achei mais tranquilo trabalhar com ponteiros que apontam para arrays de char do que com alocação dinâmica
// Isso *potencialmente* vai custar alguma nota
#define STRING_MAX_SIZE 1000

/*
    Pensando um pouco melhor,
    acho que dava pra ter feito os parâmetros tudo dentro de uma struct
    pra não fazer funções com 20 parâmetros, mas ok.
    Muito tarde para mudar.
*/

// Estrutura para lidar com os arquivos de até 10 IDs
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
    // Arquivo de entrada ditado pelo argumento -f da linha de comando
    FILE *input;
    char dirPath[STRING_MAX_SIZE];

    // Criando as variáveis @1, @2 e @3
    char commandLineParameter[3][STRING_MAX_SIZE];

    // Inicializando os parâmetros @1, @2 e @3 como \0
    for(int i = 0; i < 3; i++){
        commandLineParameter[i][0] = '\0';
    }

    // Abrindo o arquivo definido pelo argumento -o da minha de comando
    input = fopen(argv[2], "r");

    // Variável utilitária para pegar os @1, @2 e @3
    int parameterIndex = 0;

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

    // Tamanho da variável (x, y ou z) para posterior utilização na alocação de memória
    int varSize = strlen(*var);

    // String auxiliar para cópia e posterior concatenação.
    char dummyString[STRING_MAX_SIZE];

    // Comparação dos argumentos da função
    if(strcmp(toBeConcatenated, "@1") == 0){
        // Tamanho para alocação de memória posterior
        int pSize = strlen(p1);

        // Faz uma cópia da variável
        strcpy(dummyString, *var);

        // Libera toda a memória alocada para a variável e a realoca, incluindo um espaço extra para o caracter de finalização de uma string '\0'
        free(*var);
        *var = malloc((pSize + varSize + 1) * sizeof(char));

        // O processo inverso ocorre: a string auxiliar é copiada para o endereço da variável, que é concatenada com o parâmetro correspondente
        strcpy(*var, dummyString);
        strcat(*var, p1);

        // Mais uma vez, um testezinho

        // Por fim, retorna
        return;
    // E o mesmo processo é repetido duas vezes
    } else if(strcmp(toBeConcatenated, "@2") == 0){
        int pSize = strlen(p2);
        strcpy(dummyString, *var);
        free(*var);
        *var = malloc((pSize + varSize + 1) * sizeof(char));
        strcpy(*var, dummyString);
        strcat(*var, p2);
        return;
    } else if(strcmp(toBeConcatenated, "@3") == 0){
        int pSize = strlen(p3);
        strcpy(dummyString, *var);
        free(*var);
        *var = malloc((pSize + varSize + 1) * sizeof(char));
        strcpy(*var, dummyString);
        strcat(*var, p3);
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
        return;
    }
}

// Para caso z
void concatenateVariableSpecial(char **var, FILE *input, char *p1){
    char removeLeadingWhitespace = fgetc(input);

    // Limpar o caracter \n
    char toBeConcatenated[STRING_MAX_SIZE];
    fgets(toBeConcatenated, STRING_MAX_SIZE, input);
    toBeConcatenated[strcspn(toBeConcatenated, "\n")] = 0;

    // Mais ou menos o mesmo processo de antes aqui se repete
    int varSize = strlen(*var);
    char dummyString[STRING_MAX_SIZE];

    if(strcmp(toBeConcatenated, "**") == 0){
        // Concatenar @1 vezes
        strcpy(dummyString, *var);
        int n = atoi(p1);
        free(*var);
        *var = malloc(((n * varSize) + 1) * sizeof(char));
        for(int i = 0; i < n; i++){
            if(i == 0){
                strcpy(*var, dummyString);
            } else {
                strcat(*var, dummyString);
            }
        }
    } else if(toBeConcatenated[0] == '*'){
        // Concatenar *n vezes
        strcpy(dummyString, *var);

        // Ponteiro para modificar um array de chars e coletar a quantidade de vezes
        char *catPointer = toBeConcatenated;
        removeCharFromString(&catPointer, '*');
        int n = atoi(catPointer);

        free(*var);
        *var = malloc(((n * varSize) + 1) * sizeof(char));
        for(int i = 0; i < n; i++){
            if(i == 0){
                strcpy(*var, dummyString);
            } else {
                strcat(*var, dummyString);
            }
        }
    } else {
        // Concatenar uma string delimitada por duas aspas
        char *p = toBeConcatenated;
        removeCharFromString(&p, '\"');

        int pSize = strlen(toBeConcatenated);
        strcpy(dummyString, *var);
        free(*var);
        *var = malloc((pSize + varSize + 1) * sizeof(char));
        strcpy(*var, dummyString);
        strcat(*var, toBeConcatenated);
        return;
    }
}

void clearVariable(char **var){
    // Limpando a variável
    free(*var);

    // Alocando espaço para o caracter '\0'
    *var = malloc(sizeof(char));
    strcpy(*var, "");
}

void setCurrentDirectory(FILE* input, char **path, char **defaultPath, char *x, char *y, char *z){
    char removeLeadingWhitespace = fgetc(input);

    char dirArg[STRING_MAX_SIZE];
    fgets(dirArg, STRING_MAX_SIZE, input);
    dirArg[strcspn(dirArg, "\n")] = 0;

    // Seleciona o path a partir de uma variável ou de uma string com um path
    if(strcmp(dirArg, "#x") == 0){
        strcpy(*path, x);
    } else if(strcmp(dirArg, "#y") == 0){
        strcpy(*path, y);
    } else if(strcmp(dirArg, "#z") == 0){
        strcpy(*path, z);
    } else {
        strcpy(*path, dirArg);
    }
    // Adicionando um (potencial) segundo '/' ao final do path
    strcat(*path, "/");
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
        // Mais uma vez, um ponteiro para modificar um array de char
        char *argPointer = nameArg;
        removeCharFromString(&argPointer, '\"');
        strcpy(*fileName, argPointer);
    }
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
        // E de novo...
        char *extPointer = extArg;
        removeCharFromString(&extPointer, '\"');
        strcpy(*fileExt, extPointer);
    }
}

void createFile(Files f[10], int id, char **path, char **fileName, char **extName){
    char dummyString[STRING_MAX_SIZE];

    // Concatenando TUDO para ter o path real desse arquivo
    // Detalhe: ele não cria os diretórios, ou seja, O DIRETÓRIO JÁ DEVE EXISTIR no momento em que essa função for rodada
    strcpy(dummyString, *path);
    strcat(dummyString, *fileName);
    strcat(dummyString, ".");
    strcat(dummyString, *extName);

    // Abrindo para escrita para conseguir criar o arquivo...
    f[id].filePointer = fopen(dummyString, "w");
    /*  // Bloco de código para informar um erro, caso existir. O único erro que deve poder surgir é o de arquivo não encontrado devido aos diretórios ainda não existirem
    if(!f[id].filePointer){
        perror("fopen");
    } */
    strcpy(f[id].filePath, dummyString);
}

void openFile(Files f[10], int id, char **path, char **fileName, char **extName){
    char dummyString[STRING_MAX_SIZE];

    strcpy(dummyString, *path);
    strcat(dummyString, *fileName);
    strcat(dummyString, ".");
    strcat(dummyString, *extName);

    // Abre para inclusão - adição de strings ao final do arquivo
    f[id].filePointer = fopen(dummyString, "a");
    strcpy(f[id].filePath, dummyString);
}

void readFile(Files f[10], int id){
    // Apenas abre para leitura
    f[id].filePointer = fopen(f[id].filePath, "r");
}

void writeFile(FILE* input, Files f[10], int id, char **x, char **y, char **z){
    // Conforme consta no arquivo do trabalho, jogando um erro no console no caso de um arquivo a ser lido não existir.
    if(fopen(f[id].filePath, "w") == NULL){
        printf("Erro: nao existe um arquivo aberto no id %d!\n", id);
        exit(EXIT_FAILURE);
    } else {
        char removeLeadingWhiteSpace = fgetc(input);

        char toBeWritten[STRING_MAX_SIZE];
        fgets(toBeWritten, STRING_MAX_SIZE, input);
        toBeWritten[strcspn(toBeWritten, "\n")] = 0;

        // Ponteiros duplos desnecessários
        if(strcmp(toBeWritten, "#x") == 0){
            fprintf(f[id].filePointer, "%s", *x);
        } else if(strcmp(toBeWritten, "#y") == 0){
            fprintf(f[id].filePointer, "%s", *y);
        } else if(strcmp(toBeWritten, "#z") == 0){
            fprintf(f[id].filePointer, "%s", *z);
        } else {
            // Limpando o $ ao final da string
            toBeWritten[strcspn(toBeWritten, "$")] = 0;
            fprintf(f[id].filePointer, "%s", toBeWritten);
        }
    }
}

void readLineFromFile(FILE* input, Files f[10], int id, char **x, char **y, char **z){
    char removeLeadingWhiteSpace = fgetc(input);

    char toBeWritten[STRING_MAX_SIZE];
    fgets(toBeWritten, STRING_MAX_SIZE, input);
    toBeWritten[strcspn(toBeWritten, "\n")] = 0;

    if(strcmp(toBeWritten, "#x") == 0){
        strcpy(*x, toBeWritten);
    } else if(strcmp(toBeWritten, "#y") == 0){
        strcpy(*y, toBeWritten);
    } else if(strcmp(toBeWritten, "#z") == 0){
        strcpy(*z, toBeWritten);
    }
}

void closeFile(Files f[10], int id){
    // Fecha o arquivo
    fclose(f[id].filePointer);

    // Faz o filePointer desse ID apontar para NULL até que seja criado/aberto outro arquivo nesse mesmo ID
    f[id].filePointer = NULL;
}

///////////////////////////////////
//          Utilidades           //
///////////////////////////////////

void removeCharFromString(char **str, char c){
    // Remover um determinado char de alguma string
    char *pRead = *str, *pWrite = *str;
    while(*pRead){
        *pWrite = *pRead++;
        // Condicional que faz essa função funcionar
        pWrite += (*pWrite != c);
    }
    // Força a inclusão de um null para que a string termine n ocorrências antes
    *pWrite = '\0';
}

int ctoi(char c){
    // Função que acabou não possuindo uso
    return c - '0';
}

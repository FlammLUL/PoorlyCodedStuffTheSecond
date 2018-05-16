#ifndef FLAMMQUEUE_H_
#define FLAMMQUEUE_H_

/*  Figura Geométrica:
        - Representa um objeto - um retângulo ou um círculo - a posteriormente ser representado no arquivo .svg
        - Não há necessidade de separá-los, basta colocar o valor -1 em cRadius no caso da figura geométrica ser um retângulo, por exemplo
*/

typedef struct geometricFigure{
    int id;
    float x, y;
    float rWidth, rHeight;
    float cRadius;
    char *fColor, *bColor;
} GeometricFigure;

/* ===================================================================== */

/*  Fila:
        - Estrutura de dados que segue o esquema FIFO - First In, First Out -, isto é, o primeiro elemento a entrar também será o primeiro a sair
        - É, afinal, uma espécie de lista ligada na qual os itens são inseridos ao final e retirados do começo
*/

typedef struct queue{
    GeometricFigure g;
    struct queue *next;
} Queue;

/* ===================================================================== */

/*  Resumo super rápido das funções de filas:
        - fQueueInsert: adiciona um elemento ao final da fila
        - fQueuePop: retira o primeiro elemento da fila
        - fQueuePrint: função para testar se a fila foi feita de forma correta, não possui uso para o código

*/

int fQueueInsert(Queue**, GeometricFigure);
int fQueuePop(Queue**);
int fQueuePrint(Queue*);

/*  Resumo super rápido das funções envolvendo as figuras:
        - createCircle: cria um círculo
        - createRectangle: cria um retângulo
        - searchForID: procura por uma figura com um determinado ID
        - getDistance: retorna a distância entre duas figuras, pertinente aos comandos "i" e "o"
        - getDistanceWithOutput: mexe com o arquivo de saída, pelo menos supostamente
        - calcTriangleArea: calcula a área de um triângulo dada suas coordenadas
        - isInternal: checa se um par de coordenadas (x, y) é interno a uma figura geométrica
        - checkOverlay: checa se duas figuras geométricas estão sobrepostas
*/
GeometricFigure createCircle(FILE*);
GeometricFigure createRectangle(FILE*);
GeometricFigure searchForID(Queue*, int);
double getDistance(GeometricFigure, GeometricFigure);
double getDistanceWithOutput(Queue*, int, int);
double calcTriangleArea(double, double, double, double, double, double);
int isInternal(GeometricFigure, double, double);
int checkOverlay(Queue*, int, int);

#endif

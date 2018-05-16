#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include "flammQueue.h"

/* Alocando um item ao final da fila */
int fQueueInsert(Queue **qHead, GeometricFigure g){
    Queue *aux;
    Queue *qTail;

    aux = (Queue*) malloc(sizeof(Queue));
    aux->next = NULL;
    aux->g = g;
    if(*qHead == NULL){
        *qHead = aux;
    } else {
        qTail = *qHead;
        while(qTail->next != NULL){
            qTail = qTail->next;
        }
        qTail->next = aux;
    }
    return 0;
}

/* Retirando um item do começo da fila */
int fQueuePop(Queue **qHead){
    Queue *aux;

    aux = *qHead;
    *qHead = (*qHead)->next;

    free(aux);
    return 0;
}

/* Apenas para ajudar a debugar */
int fQueuePrint(Queue *qHead){
    Queue *aux;
    aux = qHead;
    while(aux != NULL){
        printf("%d > ", aux->g.x);
        aux = aux->next;
    }
    printf("NULL");
    printf("\n");
    return 0;
}

GeometricFigure createCircle(FILE* input){
    GeometricFigure g;

    char bDummy[40], fDummy[40];
    int bSize, fSize;

    fscanf(input, "%d %s %s %f %f %f", &(g.id), bDummy, fDummy, &(g.cRadius), &(g.x), &(g.y));

    bSize = strlen(bDummy);
    fSize = strlen(fDummy);

    g.bColor = (char*) malloc((bSize + 1) * sizeof(char));
    g.fColor = (char*) malloc((fSize + 1) * sizeof(char));

    strcpy(g.bColor, bDummy);
    strcpy(g.fColor, fDummy);

    return g;
}

GeometricFigure createRectangle(FILE* input){
    GeometricFigure g;

    char bDummy[40], fDummy[40];
    int bSize, fSize;

    fscanf(input, "%d %s %s %f %f %f %f", &(g.id), bDummy, fDummy, &(g.rWidth), &(g.rHeight), &(g.x), &(g.y));

    g.cRadius = -1;

    bSize = strlen(bDummy);
    fSize = strlen(fDummy);

    g.bColor = (char*) malloc((bSize + 1) * sizeof(char));
    g.fColor = (char*) malloc((fSize + 1) * sizeof(char));

    strcpy(g.bColor, bDummy);
    strcpy(g.fColor, fDummy);

    return g;
}

GeometricFigure searchForID(Queue *qHead, int id){
    GeometricFigure g;
    Queue *aux;

    aux = qHead;

    while(aux->g.id != id || aux == NULL){
        aux = aux->next;
    }
    g = aux->g;

    return g;
}

/* Retorna a distância entre o centro de massa de duas figuras
        - Para o círculo, basta calcular com base no seu centro (cx, cy)
        - Para o retângulo, é necessário calcular o seu centro de massa (x + width/2, y + height/2)
    Assim que os pontos (x1, y1) e (x2, y2), que representam os centros de massa das figuras geométricas,
estão determinados, basta retirar a sua distância usando a fórmula para a distância em um plano 2d */
double getDistance(GeometricFigure g1, GeometricFigure g2){
    double x1, x2, y1, y2, d;

    if(g1.cRadius != -1){
        x1 = (double) g1.x;
        y1 = (double) g1.y;
    } else {
        x1 = (double) (g1.x + (g1.rWidth/2));
        y1 = (double) (g1.x + (g1.rHeight/2));
    }

    if(g2.cRadius != -1){
        x2 = (double) g2.x;
        y2 = (double) g2.y;
    } else {
        x2 = (double) (g2.x + (g2.rWidth/2));
        y2 = (double) (g2.x + (g2.rHeight/2));
    }

    d = sqrt(pow((x1 - x2), 2) + pow((y1 - y2), 2));
    return d;
}

double getDistanceWithOutput(Queue *qHead, int idDummy1, int idDummy2){
    GeometricFigure g1, g2;
    double d;

    g1 = searchForID(qHead, idDummy1);
    g2 = searchForID(qHead, idDummy2);

    d = getDistance(g1, g2);
    return d;
}

/*  Calcula a área de um triângulo desenvolvendo uma fórmula para a área que utiliza uma matriz
cujas linhas são [1, 1, 1], [x1, x2, x3], [y1, y2, y3]. Ao retirar-se o determinante dessa matriz
e dividindo-o por 2, obtém-se a área de um triângulo determinado pelos pontos (x1, y1), (x2, y2), (x3, y3):
    A = 0.5 * (x1y2 - x1y3 + x2y3 - x2y1 + x3y1 - x3y2)
    = A = 0.5 * (x1(y2 - y3) + x2(y3 - y1) + x3(y1 - y2)) */
double calcTriangleArea(double x1, double y1, double x2, double y2, double x3, double y3){
    double a;
    a = (double) abs((x1 * (y2 - y3)) + (x2 * (y3 - y1)) + (x3 * (y1 - y2)));
    a /= 2;
    return a;
}

/* Para verificar se um ponto é interno a uma figura, deve-se verificar se a figura oferecida como parâmetro é um círculo ou um retângulo
    - Caso for um círculo, basta verificar se a distância até o centro (cx, cy) é menor que o raio de G
    - Caso for um retângulo, deve-se dividir um retângulo ABCD em triângulos. Um dos vértices do triângulo deve ser o ponto P
        - Com isso, tem-se quatro triângulos: ABP, BCP, CDP, DAP
        - A partir disso, a soma das áreas dos quatro triângulos deve ser igual à área do retângulo original para que o ponto P seja interno à figura
        - Caso uma das áreas seja igual a zero, isso quer dizer que a figura contém o ponto, mas ele está em sua borda */
int isInternal(GeometricFigure g, double px, double py){
    double cDistance;
    double sArea, gArea;
    int nZero;

    if(g.cRadius != -1){
        cDistance = sqrt(pow(px - (double) g.x, 2) + pow(py - (double) g.y, 2));
        if(cDistance < g.cRadius){
            return 1;
        } else {
            return 0;
        }
    } else {
        gArea = g.rHeight * g.rWidth;
        sArea = 0;
        nZero = 0;
        sArea += calcTriangleArea(px, py, (double)  g.x, (double)  g.y, (double) (g.x + g.rWidth), (double)  g.y);
        sArea += calcTriangleArea(px, py, (double)  (g.x + g.rWidth), (double) g.y, (double) (g.x + g.rWidth), (double) (g.y + g.rHeight));
        sArea += calcTriangleArea(px, py, (double)  (g.x + g.rWidth), (double) (g.y + g.rHeight), (double) g.x, (double) (g.y + g.rHeight));
        sArea += calcTriangleArea(px, py, (double)  g.x, (double) (g.y + g.rHeight), (double) g.x, (double) g.y);

        /* Isso aqui apenas existe para verificar se a área de algum é zero. Caso o for, é porque o ponto pertence à figura, mas está na borda, ou seja, não é interno. */
        if(calcTriangleArea(px, py, (double)  g.x, (double)  g.y, (double) (g.x + g.rWidth), (double)  g.y) == 0){
            nZero += 1;
        }
        if(calcTriangleArea(px, py, (double)  (g.x + g.rWidth), (double) g.y, (double) (g.x + g.rWidth), (double) (g.y + g.rHeight)) == 0){
            nZero += 1;
        }
        if(calcTriangleArea(px, py, (double)  (g.x + g.rWidth), (double) (g.y + g.rHeight), (double) g.x, (double) (g.y + g.rHeight)) == 0){
            nZero += 1;
        }
        if(calcTriangleArea(px, py, (double)  g.x, (double) (g.y + g.rHeight), (double) g.x, (double) g.y) == 0){
            nZero += 1;
        }

        if(gArea == sArea && nZero == 0){
            return 1;
        } else {
            return 0;
        }
    }
}

/*  Para a verificação de sobreposição (comando "o"), deve, primeiro, verificar quais formas são retângulos e quais são círculos e agir de acordo
        - Caso hajam dois círculos, verificar a distância entre seus centros e ver se, para pelo menos um, essa distância é menor que a soma de seus raios
        - Caso hajam um retângulo e um círculo, verificar se o centro do círculo é interno ao retângulo ou se cada um dos quatro vértices do retângulo é interno ao círculo
        - Caso hajam dois retângulos, fazer verificações em relação às coordenadas de dois vértices opostos */
int checkOverlay(Queue *qHead, int id1, int id2){
    GeometricFigure g1, g2;

    g1 = searchForID(qHead, id1);
    g2 = searchForID(qHead, id2);

    if(g1.cRadius != -1 && g2.cRadius != -1){
        if(getDistance(g1, g2) <= g1.cRadius + g2.cRadius){
            return 1;
        } else {
            return 0;
        }
    } else if(g1.cRadius != -1 && g2.cRadius == -1){
        if(isInternal(g2, g1.x, g1.y) || isInternal(g1, g2.x, g2.y) || isInternal(g1, g2.x + g2.rWidth, g2.y) || isInternal(g1, g2.x, g2.y + g2.rHeight) || isInternal(g1, g2.x + g2.rWidth, g2.y + g2.rHeight)){
            return 1;
        } else {
            return 0;
        }
    } else if(g1.cRadius == -1 && g2.cRadius != -1){
        if(isInternal(g1, g2.x, g2.y) || isInternal(g2, g1.x, g1.y) || isInternal(g2, g1.x + g1.rWidth, g1.y) || isInternal(g2, g1.x, g1.y + g1.rHeight) || isInternal(g2, g1.x + g1.rWidth, g1.y + g1.rHeight)){
            return 1;
        } else {
            return 0;
        }
    } else {
        if(g1.x < g2.x + g2.rWidth && g1.x + g1.rWidth > g2.x && g1.y > g2.y + g2.rHeight && g1.y + g1.rHeight < g2.y){
            return 1;
        } else {
            return 0;
        }
    }
    return 0;
}


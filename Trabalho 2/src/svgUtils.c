#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "flammQueue.h"
#include "environment.h"
#include "svgUtils.h"

/* Função que cria o arquivo .SVG contendo apenas as figuras: 
    - Ideia: iterar pela fila, adicionando elementos de seu começo ao final 
*/
void makeSVG(Environment *e, Queue *qHead){
    FILE *outputFile;
    Queue *aux;
    
    outputFile = fopen(e->outputActualFilePathSvg, "w");

    fprintf(outputFile, "<svg version=\"1.1\" baseProfile=\"full\" width=\"3840\" height=\"2160\" xmlns=\"http://www.w3.org/2000/svg\">\n");

    aux = qHead;

    while(aux != NULL){
        if(aux->g.cRadius == -1){
            fprintf(outputFile, "\t<rect x=\"%f\" y=\"%f\" width=\"%f\" height=\"%f\" stroke=\"%s\" stroke-width=\"%d\" fill=\"%s\" opacity=\"0.5\" />\n", aux->g.x, aux->g.y, aux->g.rWidth, aux->g.rHeight, aux->g.bColor, 3, aux->g.fColor);
        } else {
            fprintf(outputFile, "\t<circle cx=\"%f\" cy=\"%f\" r=\"%f\" stroke=\"%s\" stroke-width=\"%d\" fill=\"%s\" opacity=\"0.5\" />\n", aux->g.x, aux->g.y, aux->g.cRadius, aux->g.bColor, 3, aux->g.fColor);
        }
        aux = aux->next;
    }
    fprintf(outputFile, "</svg>");
}

/* Função que cria o arquivo .SVG contendo figuras, linhas e distâncias:
    - A ideia segue a mesma, excetuando-se pelo fato de que as distâncias são calculadas
e colocadas no momento em que são calculadas para minimizar o custo de desempenho, embora
isso afete a sua visibilidade.
*/
void makeLinesSVG(FILE* input, Environment *e, Queue *qHead){
    FILE *outputFile;
    char dummyString[100];
    char *lColor;
    int newSize;
    int id;
    GeometricFigure g;
    double sx, sy, ex, ey;
    Queue *aux;

    fscanf(input, "%d %s", &id, dummyString);

    newSize = strlen(e->outputFilePath);
    newSize += strlen(dummyString);
    newSize += 5;

    e->outputPathForCommandA = (char*) malloc(newSize * sizeof(char));
    strcpy(e->outputPathForCommandA, e->outputFilePath);
    strcat(e->outputPathForCommandA, "-");
    strcat(e->outputPathForCommandA, dummyString);
    strcat(e->outputPathForCommandA, ".svg");

    outputFile = fopen(e->outputPathForCommandA, "w");
    fprintf(outputFile, "<svg version=\"1.1\" baseProfile=\"full\" width=\"3840\" height=\"2160\" xmlns=\"http://www.w3.org/2000/svg\">\n");

    aux = qHead;

    while(aux != NULL){
        if(aux->g.cRadius == -1){
            fprintf(outputFile, "\t<rect x=\"%f\" y=\"%f\" width=\"%f\" height=\"%f\" stroke=\"%s\" stroke-width=\"%d\" fill=\"%s\" opacity=\"0.5\"/>\n", aux->g.x, aux->g.y, aux->g.rWidth, aux->g.rHeight, aux->g.bColor, 3, aux->g.fColor);
        } else {
            fprintf(outputFile, "\t<circle cx=\"%f\" cy=\"%f\" r=\"%f\" stroke=\"%s\" stroke-width=\"%d\" fill=\"%s\" opacity=\"0.5\"/>\n", aux->g.x, aux->g.y, aux->g.cRadius, aux->g.bColor, 3, aux->g.fColor);
        }
        aux = aux->next;
    }

    aux = qHead;

    /* Tomando diferentes rumos a depender da forma geométrica */
    g = searchForID(qHead, id);
    if(g.cRadius == -1){
        sx = g.x + (g.rWidth/2);
        sy = g.y + (g.rHeight/2);
    } else {
        sx = g.x;
        sy = g.y;
    }
    lColor = malloc(strlen(g.bColor) * sizeof(char));
    strcpy(lColor, g.bColor);

    while(aux != NULL){
        if(aux->g.cRadius == -1){
            ex = aux->g.x + (aux->g.rWidth / 2);
            ey = aux->g.y + (aux->g.rHeight / 2);
        } else {
            ex = aux->g.x;
            ey = aux->g.y;
        }
        fprintf(outputFile, "\t<line x1=\"%lf\" y1=\"%lf\" x2=\"%lf\" y2=\"%lf\" stroke=\"%s\" stroke-width=\"2\" opacity=\"0.75\" />\n", sx, sy, ex, ey, lColor);
        fprintf(outputFile, "\t<text x=\"%lf\" y=\"%lf\" fill=\"%s\" stroke=\"black\" stroke-width=\"1\"> %.2lf </text>\n", ex, ey - 10, lColor, getDistance(g, aux->g));
        aux = aux->next;
    }
    fprintf(outputFile, "</svg>");
}

void removeCharFromString(char **str, char c){
    char *pRead = *str, *pWrite = *str;
    while(*pRead){
        *pWrite = *pRead++;
        pWrite += (*pWrite != c);
    }
    *pWrite = '\0';
}

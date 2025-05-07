#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define _LIN 2
#define _COL 2

double* calculaReta(double x1, double y1, double x2, double y2){
    double a = (y2-y1)/(x2-x1);
    double b = -(a * x1) + y1;
    double* equacao = malloc(2* sizeof(double));
    equacao[0] = a;
    equacao[1] = b;
    return equacao;
}
void leiaPontos(double pontos[][_COL]){
    for(int i = 0; i < _LIN; i++){
        printf("\nDigite os valores do ponto%i: ", (i + 1));
        for(int j = 0; j < _COL; j++){
            printf("\n%c: ", !j?'x':'y');
            scanf("%lf", &pontos[i][j]);
        }
    }
}
void printEquacao(int grau, double* equacao){
    printf("f(x) = ");
    int index = 0;
    for(int i = grau; i >= 2; i--){
        printf("%fx^%d + ", equacao[index], i);
        index++;
    }
    printf("%.2fx + %.2f", equacao[index], equacao[index + 1]);
}

int main()
{
    double pontos[_LIN][_COL];
    leiaPontos(pontos);
    double* reta = calculaReta(pontos[0][0], pontos[0][1], pontos[1][0], pontos[1][1]);
    printEquacao(1, reta);
    free(reta);
    return 0;
}

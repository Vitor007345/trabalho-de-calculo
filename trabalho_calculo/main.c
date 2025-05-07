#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>

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


double execEquacao(int grau, double* equacao, double x){
    int expo = grau;
    double resultado = 0;
    for(int i = 0; i <= grau; i++){
        resultado +=  equacao[i] * pow(x, expo);
        expo--;
    }
    return resultado;
}


double* retaSec(int grau, double* equacao, double x1, double x2){
    return calculaReta(x1, execEquacao(grau, equacao, x1), x2, execEquacao(grau, equacao, x2));
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


void printaSecsAproxDe(double x, int grau, double* equacao, int max){
    double aprox = 0.5;
    while(aprox >= 0.2){
        double* sec = retaSec(grau, equacao, x, (x + aprox));
        printEquacao(1, sec);
        free(sec);
        printf("\n");
        aprox -= 0.1;
    }
    double aproxAdaptativo = sqrt(DBL_EPSILON) * fmax(fabs(x), 1.0);
    for(int i = 0; i < max && aprox >= aproxAdaptativo; i++){
        //printf("Aprox: %.20f\n", (double)aprox);
        //printf("aprox > DBL_MIN = %s\n", (aprox > DBL_MIN? "true" : "false"));
        //printf("DBL_MIN: %.20f\n", (double)DBL_MIN);
        double* sec = retaSec(grau, equacao, x, (x + aprox));
        printEquacao(1, sec);
        free(sec);
        printf("\n");
        aprox /= 10;
    }
}

double* calculaRetaTangente(double x, int grau, double* equacao){
    double aprox = sqrt(DBL_EPSILON) * fmax(fabs(x), 1.0);
    double x2 = x + aprox;
    return retaSec(grau, equacao, x, x2);
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

int main()
{

    double pontos[_LIN][_COL];
    leiaPontos(pontos);
    double* reta = calculaReta(pontos[0][0], pontos[0][1], pontos[1][0], pontos[1][1]);
    printEquacao(1, reta);
    free(reta);


    return 0;
}

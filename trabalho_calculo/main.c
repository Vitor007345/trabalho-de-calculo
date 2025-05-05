#include <stdio.h>
#include <stdlib.h>
#include <math.h>


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
    printf("%.20fx + %.20f", equacao[index], equacao[index + 1]);
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
    for(int i = 0; i < max; i++){
        double* sec = retaSec(grau, equacao, x, (x + aprox));
        printEquacao(1, sec);
        free(sec);
        printf("\n");
        aprox /= 10;
    }
}


int main()
{
    double equacao[] = {1, 0, 0};
    printaSecsAproxDe(3.766, 2, equacao, 20);

    return 0;
}

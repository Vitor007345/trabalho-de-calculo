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


void printaSecsAproxDe(double x, int grau, double* equacao){

}


int main()
{
    //double* secante = retaSec()
    printf("y = %fx + %f\n", equacao[0], equacao[1]);
    return 0;
}

#include <iostream>
#include <functional>
#include <vector>
#include <math.h>
#include <float.h>

using namespace std;



std::function<double(double)> calculaReta(double x1, double y1, double x2, double y2){
    double a = (y2-y1)/(x2-x1);
    double b = -(a * x1) + y1;

    return [a, b](double x) {return a*x + b;};
}

std::function<double(double)> retaSec(std::function<double(double)> func, double x1, double x2){
    return calculaReta(x1, func(x1), x2, func(x2));
}

std::function<double(double)> calculaRetaTangente(double x, std::function<double(double)> func){
    double aprox = sqrt(DBL_EPSILON) * fmax(fabs(x), 1.0);
    double x2 = x + aprox;
    return retaSec(func, x, x2);
}

std::function<double(double)> generateFunction(std::initializer_list<double> args){
    std::vector<double> coeficientes = std::vector<double>(args);
    int grau = coeficientes.size() - 1;

    return [coeficientes, grau] (double x) {
        double retorno = 0;
        int expo = grau;
        for (double coef : coeficientes){
            retorno += coef * pow(x, expo);
            expo--;
        }
        return retorno;
    };
}



int main()
{
    auto reta = calculaReta(1.0, 1.0, 3.0, 5.0);

    std::cout << reta(2.5);

    return 0;
}

#include <iostream>
#include <functional>
#include <vector>
#include <math.h>
#include <float.h>
#include <string>
#include <sstream>
#include <iomanip>

//using namespace std;

struct FUNC{
    std::string expression;
    std::function<double(double)> f;
};




FUNC calculaReta(double x1, double y1, double x2, double y2){
    double a = (y2-y1)/(x2-x1);
    double b = -(a * x1) + y1;

    std::cout << "M: " << a << std::endl;
    FUNC reta;

    reta.expression = "f(x) = " + std::to_string(a) + "x + " + std::to_string(b);
    reta.f = [a, b](double x) {return a*x + b;};


    return reta;
}

FUNC retaSec(FUNC f, double x1, double x2){
    return calculaReta(x1, f.f(x1), x2, f.f(x2));
}


FUNC calculaRetaTangente(double x, FUNC func){
    double aprox = sqrt(DBL_EPSILON) * fmax(fabs(x), 1.0); //aprox de 0
    double x2 = x + aprox;
    return retaSec(func, x, x2);
}

void printFunc(FUNC func){
    std::cout << func.expression << std::endl;
}


std::vector<FUNC> calculaSecsAproxDeTan(double x, FUNC func){

    std::vector<FUNC> secs;

    secs.resize(5);

    double aprox = 0.5;

    for(FUNC& sec : secs){
        double x2 = x + aprox;

        sec = retaSec(func, x, x2);

        std::cout << "Aprox de " << x << ": " << x2 << std::endl;
        std::cout << func.expression + "\nf(" << x << ") = " << func.f(x2) << "\n" << std::endl;


        aprox -= 0.1;
    }
    return secs;

}

void printFuncs(std::vector<FUNC> funcoes){
    for(FUNC f : funcoes){
        printFunc(f);
    }
}

void printfRetaTan(double x, FUNC func){
    FUNC tan = calculaRetaTangente(x, func);
    printFunc(tan);
}


int main()
{
    const double valor = 2;
    FUNC f;
    f.expression = "f(x) = x^2";
    f.f = [](double x){return x*x;};

    printFuncs(calculaSecsAproxDeTan(valor, f));

    std::cout << "\nReta tangente a funcao " + f.expression << std::endl;

    printfRetaTan(valor, f);


    return 0;
}


/*
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
*/

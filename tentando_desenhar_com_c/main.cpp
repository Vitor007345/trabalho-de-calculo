#include <windows.h>
#include <math.h>
#include <vector>
#include <string>
#include <algorithm>
#include <functional>
#include <float.h>
#include <sstream>
#include <iomanip>
#include <iostream>

//Feito pelo deepseek-inicio
const int LARGURA = 800;
const int ALTURA = 800;

class MultiFunctionPlotter {
private:
    struct Function {
        std::string expression;
        COLORREF color;
        std::function<double(double)> func;
    };

    std::vector<Function> functions;
    double x_min = -5.0;
    double x_max = 5.0;
    double y_min = -10.0;
    double y_max = 10.0;

    void autoAdjustYRange() {
        y_min = INFINITY;
        y_max = -INFINITY;

        const int steps = 1000;
        double step = (x_max - x_min) / steps;

        for (const auto& f : functions) {
            for (double x = x_min; x <= x_max; x += step) {
                try {
                    double y = f.func(x);
                    if (!std::isinf(y) && !std::isnan(y)) {
                        y_min = std::min(y_min, y);
                        y_max = std::max(y_max, y);
                    }
                } catch (...) {
                    // Ignora erros de cálculo
                }
            }
        }

        // Adiciona margem
        double margin = (y_max - y_min) * 0.1;
        y_min -= margin;
        y_max += margin;
    }

    int toPixelX(double x) const {
        return static_cast<int>((x - x_min) / (x_max - x_min) * LARGURA);
    }

    int toPixelY(double y) const {
        return ALTURA - static_cast<int>((y - y_min) / (y_max - y_min) * ALTURA);
    }

    void drawGrid(HDC hdc) {
        // Configuração da grade
        HPEN gridPen = CreatePen(PS_DOT, 1, RGB(200, 200, 200));
        HPEN oldPen = (HPEN)SelectObject(hdc, gridPen);

        // Fonte para os rótulos
        HFONT hFont = CreateFont(14, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
                               OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                               VARIABLE_PITCH, TEXT("Arial"));
        HFONT oldFont = (HFONT)SelectObject(hdc, hFont);

        // Cor do texto
        SetTextColor(hdc, RGB(0, 0, 0));
        SetBkMode(hdc, TRANSPARENT);

        // Intervalos para a grade (ajuste conforme necessário)
        double x_interval = (x_max - x_min) / 10.0;
        double y_interval = (y_max - y_min) / 10.0;

        // Desenha linhas verticais e rótulos do eixo X
        for (double x = floor(x_min / x_interval) * x_interval; x <= x_max; x += x_interval) {
            if (x < x_min) continue;

            int px = toPixelX(x);
            MoveToEx(hdc, px, 0, NULL);
            LineTo(hdc, px, ALTURA);

            // Desenha o rótulo
            std::ostringstream oss;
            oss << std::fixed << std::setprecision(2) << x;
            std::string label = oss.str();
            RECT textRect = {px - 20, ALTURA - 20, px + 20, ALTURA};
            DrawTextA(hdc, label.c_str(), -1, &textRect, DT_CENTER);
        }

        // Desenha linhas horizontais e rótulos do eixo Y
        for (double y = floor(y_min / y_interval) * y_interval; y <= y_max; y += y_interval) {
            if (y < y_min) continue;

            int py = toPixelY(y);
            MoveToEx(hdc, 0, py, NULL);
            LineTo(hdc, LARGURA, py);

            // Desenha o rótulo
            std::ostringstream oss;
            oss << std::fixed << std::setprecision(2) << y;
            std::string label = oss.str();
            RECT textRect = {5, py - 10, 50, py + 10};
            DrawTextA(hdc, label.c_str(), -1, &textRect, DT_LEFT);
        }

        // Restaura objetos GDI
        SelectObject(hdc, oldPen);
        SelectObject(hdc, oldFont);
        DeleteObject(gridPen);
        DeleteObject(hFont);
    }

public:
    void addFunction(const std::string& expr, COLORREF color, std::function<double(double)> func) {
        functions.push_back({expr, color, func});
        autoAdjustYRange();
    }

    void drawGraph(HWND hwnd) {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        // Fundo branco
        RECT rect;
        GetClientRect(hwnd, &rect);
        FillRect(hdc, &rect, (HBRUSH)(COLOR_WINDOW + 1));

        // Desenha grade primeiro (para ficar atrás dos eixos)
        drawGrid(hdc);

        // Desenha eixos
        HPEN axisPen = CreatePen(PS_SOLID, 2, RGB(0, 0, 0));
        SelectObject(hdc, axisPen);

        int originX = toPixelX(0);
        int originY = toPixelY(0);

        MoveToEx(hdc, 0, originY, NULL);
        LineTo(hdc, LARGURA, originY);
        MoveToEx(hdc, originX, 0, NULL);
        LineTo(hdc, originX, ALTURA);

        DeleteObject(axisPen);

        // Desenha cada função
        for (const auto& f : functions) {
            HPEN graphPen = CreatePen(PS_SOLID, 2, f.color);
            SelectObject(hdc, graphPen);

            bool firstPoint = true;
            double step = (x_max - x_min) / LARGURA;

            for (double x = x_min; x <= x_max; x += step) {
                try {
                    double y = f.func(x);
                    if (!std::isinf(y) && !std::isnan(y)) {
                        int px = toPixelX(x);
                        int py = toPixelY(y);

                        if (firstPoint) {
                            MoveToEx(hdc, px, py, NULL);
                            firstPoint = false;
                        } else {
                            LineTo(hdc, px, py);
                        }
                    }
                } catch (...) {
                    firstPoint = true; // Reinicia o traço se houver erro
                }
            }

            DeleteObject(graphPen);
        }

        EndPaint(hwnd, &ps);
    }

    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
        static MultiFunctionPlotter* plotter = nullptr;

        if (msg == WM_CREATE) {
            plotter = reinterpret_cast<MultiFunctionPlotter*>(
                reinterpret_cast<LPCREATESTRUCT>(lParam)->lpCreateParams);
        }

        if (plotter) {
            switch (msg) {
                case WM_PAINT:
                    plotter->drawGraph(hwnd);
                    return 0;
                case WM_DESTROY:
                    PostQuitMessage(0);
                    return 0;
            }
        }
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }

    void show() {
        WNDCLASS wc = {0};
        wc.lpfnWndProc = WindowProc;
        wc.hInstance = GetModuleHandle(NULL);
        wc.lpszClassName = "MultiFunctionPlotter";
        wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

        RegisterClass(&wc);

        HWND hwnd = CreateWindow(
            "MultiFunctionPlotter", "Gráfico de Múltiplas Funções",
            WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT, CW_USEDEFAULT,
            LARGURA, ALTURA,
            NULL, NULL,
            GetModuleHandle(NULL),
            this
        );

        if (!hwnd) return;

        ShowWindow(hwnd, SW_SHOW);
        UpdateWindow(hwnd);

        MSG msg;
        while (GetMessage(&msg, NULL, 0, 0)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
};


//Feito pelo deepseek-fim

//resto feito por mim usando a funcionalidade de desenhar que o deepseek forneceu
std::function<double(double)> calculaReta(double x1, double y1, double x2, double y2){
    double a = (y2-y1)/(x2-x1);
    double b = -(a * x1) + y1;

    return [a, b](double x) {return a*x + b;};
}

std::function<double(double)> retaSec(std::function<double(double)> func, double x1, double x2){
    return calculaReta(x1, func(x1), x2, func(x2));
}

void desenhaSecantesAproxDe(MultiFunctionPlotter& plotter, double x, std::function<double(double)> func){
    double aprox = 0.5;
    int changeColor = 0;
    while(aprox >= 0.2){
        auto funcSec = retaSec(func, x, (x + aprox));
        plotter.addFunction("secAproxDeTan", RGB(128 + changeColor, 0, 128 - changeColor), funcSec);
        aprox -= 0.1;
        changeColor += 10;
    }
}

std::function<double(double)> calculaRetaTangente(double x, std::function<double(double)> func){
    double aprox = sqrt(DBL_EPSILON) * fmax(fabs(x), 1.0); //aprox de 0
    double x2 = x + aprox;
    return retaSec(func, x, x2);
}


std::function<double(double)> derivada(std::function<double(double)> f){
    return [f](double x){
        double h = sqrt(DBL_EPSILON) * fmax(fabs(x), 1.0); //aprox de 0
        return (f(x + h) - f(x - h))/(2*h);
    };
}



int main() {
    MultiFunctionPlotter plotter;

    const double valor = 1;
    const auto func = [](double x) {return x*x;};


    plotter.addFunction("func", RGB(255, 0, 0), func);
    desenhaSecantesAproxDe(plotter, valor, func);
    plotter.addFunction("retaTan", RGB(0, 255, 0), calculaRetaTangente(valor, func));
    plotter.addFunction("derivada", RGB(255, 128, 0), derivada(func));




    plotter.show();

    return 0;
}


/*
class MultiFunctionPlotter {
private:
    struct Function {
        std::string expression;
        COLORREF color;
        std::function<double(double)> func;
    };

    std::vector<Function> functions;
    double x_min = -5.0;
    double x_max = 5.0;
    double y_min = -10.0;
    double y_max = 10.0;

    void autoAdjustYRange() {
        y_min = INFINITY;
        y_max = -INFINITY;

        const int steps = 1000;
        double step = (x_max - x_min) / steps;

        for (const auto& f : functions) {
            for (double x = x_min; x <= x_max; x += step) {
                try {
                    double y = f.func(x);
                    if (!std::isinf(y) && !std::isnan(y)) {
                        y_min = std::min(y_min, y);
                        y_max = std::max(y_max, y);
                    }
                } catch (...) {
                    // Ignora erros de cálculo
                }
            }
        }

        // Adiciona margem
        double margin = (y_max - y_min) * 0.1;
        y_min -= margin;
        y_max += margin;
    }

    int toPixelX(double x) const {
        return static_cast<int>((x - x_min) / (x_max - x_min) * LARGURA);
    }

    int toPixelY(double y) const {
        return ALTURA - static_cast<int>((y - y_min) / (y_max - y_min) * ALTURA);
    }

public:
    void addFunction(const std::string& expr, COLORREF color, std::function<double(double)> func) {
        functions.push_back({expr, color, func});
        autoAdjustYRange();
    }

    void drawGraph(HWND hwnd) {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        // Fundo branco
        RECT rect;
        GetClientRect(hwnd, &rect);
        FillRect(hdc, &rect, (HBRUSH)(COLOR_WINDOW + 1));

        // Desenha eixos
        HPEN axisPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
        SelectObject(hdc, axisPen);

        int originX = toPixelX(0);
        int originY = toPixelY(0);

        MoveToEx(hdc, 0, originY, NULL);
        LineTo(hdc, LARGURA, originY);
        MoveToEx(hdc, originX, 0, NULL);
        LineTo(hdc, originX, ALTURA);

        DeleteObject(axisPen);

        // Desenha cada função
        for (const auto& f : functions) {
            HPEN graphPen = CreatePen(PS_SOLID, 2, f.color);
            SelectObject(hdc, graphPen);

            bool firstPoint = true;
            double step = (x_max - x_min) / LARGURA;

            for (double x = x_min; x <= x_max; x += step) {
                try {
                    double y = f.func(x);
                    if (!std::isinf(y) && !std::isnan(y)) {
                        int px = toPixelX(x);
                        int py = toPixelY(y);

                        if (firstPoint) {
                            MoveToEx(hdc, px, py, NULL);
                            firstPoint = false;
                        } else {
                            LineTo(hdc, px, py);
                        }
                    }
                } catch (...) {
                    firstPoint = true; // Reinicia o traço se houver erro
                }
            }

            DeleteObject(graphPen);
        }

        EndPaint(hwnd, &ps);
    }

    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
        static MultiFunctionPlotter* plotter = nullptr;

        if (msg == WM_CREATE) {
            plotter = reinterpret_cast<MultiFunctionPlotter*>(
                reinterpret_cast<LPCREATESTRUCT>(lParam)->lpCreateParams);
        }

        if (plotter) {
            switch (msg) {
                case WM_PAINT:
                    plotter->drawGraph(hwnd);
                    return 0;
                case WM_DESTROY:
                    PostQuitMessage(0);
                    return 0;
            }
        }
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }

    void show() {
        WNDCLASS wc = {0};
        wc.lpfnWndProc = WindowProc;
        wc.hInstance = GetModuleHandle(NULL);
        wc.lpszClassName = "MultiFunctionPlotter";
        wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

        RegisterClass(&wc);

        HWND hwnd = CreateWindow(
            "MultiFunctionPlotter", "Gráfico de Múltiplas Funções",
            WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT, CW_USEDEFAULT,
            LARGURA, ALTURA,
            NULL, NULL,
            GetModuleHandle(NULL),
            this
        );

        if (!hwnd) return;

        ShowWindow(hwnd, SW_SHOW);
        UpdateWindow(hwnd);

        MSG msg;
        while (GetMessage(&msg, NULL, 0, 0)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
};
*/

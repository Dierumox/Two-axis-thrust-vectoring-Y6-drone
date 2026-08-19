#include <vector>
#include <random>

//
// Objetos
//

std::random_device rd;
std::mt19937 generador(rd());

//
// Configuración global
//

int no_planos_intermedios = 2;

//
// Muestreo inicial
//

int lado_x_circulo = 20;
int lado_y_circulo = 20;
int lado_radio_circulo = 10;
int lado_rotar_x_plano = 10;
int lado_rotar_y_plano = 10;
int lado_separacion_plano = 10;
int generaciones_muestreo = 1000;

//
// Investigación normal
//

int poblacion_normal = 100;

struct Circulo {
    double x;
    double y;
    double radio;
    int rama;
    int booleano;
};

struct Plano {
    std::vector<Circulo> circulos;
    double rotar_x;
    double rotar_y;
    double separacion;
    int rama;
};

struct Individuo {
    std::vector<Plano> planos;
    double fitness;
};

std::vector<Individuo> poblacion_actual;

Plano inicio {};
Plano fin {};

void generar_individuo_muestreo() {

}

void generar_individuo_normal() {
    std::uniform_int_distribution<int> distribucion(0, 10);
    int numero = distribucion(generador);
}

void generar_poblacion_inicial() {
    for(int generacion = 0; generacion < generaciones_muestreo; generacion++) {
        for(int individuo = 0; individuo < poblacion_muestreo; individuo++) {
            poblacion_actual.push_back(generar_individuo_muestreo());
        }
    }
}

void main() {
    generar_poblacion_inicial();

    while(true) {

    }

    return(0);
}
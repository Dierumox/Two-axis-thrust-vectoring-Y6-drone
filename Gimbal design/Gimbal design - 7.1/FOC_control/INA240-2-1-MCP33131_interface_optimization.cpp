#include <vector>
#include <iostream>
#include <ngspice/sharedspice.h>
#include <cstring>
#include <sstream>
#include <iomanip>

//
// Circuito
//

std::string linea0;
std::string linea1;
std::string linea2;
std::string linea3;
std::string linea4;
std::string linea5;
std::string linea6;
std::string linea7;
std::string linea8;
std::string linea9;
std::string linea10;
std::string linea11;

char* circuito[9];

//
// Valores R y C
//

const double valoresR[] = {
    10, 11, 12, 13, 15, 16, 18, 20, 22, 24, 27, 30,
    33, 36, 39, 43, 47, 51, 56, 62, 68, 75, 82, 91,
    100, 110, 120, 130, 150, 160, 180, 200, 220, 240, 270, 300,
    330, 360, 390, 430, 470, 510, 560, 620, 680, 750, 820, 910,
    1000, 1100, 1200, 1300, 1500, 1600, 1800, 2000, 2200, 2400,
    2700, 3000, 3300, 3600, 3900, 4300, 4700, 5100, 5600, 6200,
    6800, 7500, 8200, 9100,
    10000, 11000, 12000, 13000, 15000, 16000, 18000, 20000, 22000,
    24000, 27000, 30000, 33000, 36000, 39000, 43000, 47000, 51000,
    56000, 62000, 68000, 75000, 82000, 91000,
    100000, 120000, 150000, 180000, 220000, 270000, 330000,
    390000, 470000, 560000, 680000, 820000, 1000000
};

const double valoresC[] = {
    1e-12, 2.2e-12, 3.3e-12, 4.7e-12, 6.8e-12, 10e-12,
    15e-12, 22e-12, 33e-12, 47e-12, 68e-12,
    100e-12, 150e-12, 220e-12, 330e-12, 470e-12, 680e-12,
    1e-9, 1.5e-9, 2.2e-9, 3.3e-9, 4.7e-9, 6.8e-9,
    10e-9, 15e-9, 22e-9, 33e-9, 47e-9, 68e-9,
    100e-9, 150e-9, 220e-9, 330e-9, 470e-9, 680e-9,
    1e-6, 1.5e-6, 2.2e-6, 3.3e-6, 4.7e-6, 6.8e-6,
    10e-6
};

//
// Programa
//

int miSendChar(char* output, int id, void* userdata) {

    if (output != nullptr) {
        std::cout << "[NGSPICE] " << output << std::endl;
    }

    return 0;
}

int miSendStat(char* status, int id, void* userdata) {
    return 0;
}

int miControlledExit(int status, bool immediate, bool quit, int id, void* userdata) {
    return 0;
}

struct Individuo {
    int indiceR;
    int indiceC;
    double fitness;
};

std::vector<Individuo> poblacion;

void generar_poblacion() {

    for (int no_R = 0; no_R < std::size(valoresR); no_R++) {

        for (int no_C = 0; no_C < std::size(valoresC); no_C++) {

            Individuo individuo;

            individuo.indiceR = no_R;
            individuo.indiceC = no_C;
            individuo.fitness = 0;

            poblacion.push_back(individuo);
        }
    }
}

std::string convertir_numero(double valor) {
    std::ostringstream flujo;
    flujo << std::scientific << std::setprecision(15) << valor;
    return flujo.str();
}

void evaluar(Individuo& individuo) {

    linea0 = "TEST INA240";
    linea1 = ".include INA240A1_test.lib";
    linea2 = "VCC VCC 0 5";
    linea3 = "VREF ref 0 2.5";
    linea4 = "V1 in 0 0";
    linea5 = "XINA out in 0 ref ref VCC 0 INA240A1";
    linea6 = ".op";
    linea7 = ".end";

    circuito[0] = linea0.data();
    circuito[1] = linea1.data();
    circuito[2] = linea2.data();
    circuito[3] = linea3.data();
    circuito[4] = linea4.data();
    circuito[5] = linea5.data();
    circuito[6] = linea6.data();
    circuito[7] = linea7.data();
    circuito[8] = nullptr;

    ngSpice_Circ(circuito);

    ngSpice_Command("run");

    char nombre_time[] = "time";
    char nombre_vout[] = "out";

    vector_info* info_time = ngGet_Vec_Info(nombre_time);
    vector_info* info_vout = ngGet_Vec_Info(nombre_vout);

    if (info_time == nullptr || info_vout == nullptr) {
        std::cout << "No se han encontrado los vectores" << std::endl;
        return;
    }

    std::cout << "Número de puntos: "
              << info_time->v_length
              << std::endl;

    for (int i = 0; i < info_time->v_length; i++) {

        double tiempo = info_time->v_realdata[i];
        double salida = info_vout->v_realdata[i];

        // Aquí posteriormente calcularemos el fitness.
    }
}

int main() {

    std::cout << "ENTRANDO EN MAIN" << std::endl;

    int Init = ngSpice_Init(
        miSendChar,
        miSendStat,
        miControlledExit,
        nullptr,
        nullptr,
        nullptr,
        nullptr
    );

    std::cout << "Init: " << Init << std::endl;

    generar_poblacion();

    int contador = 0;

    std::cout << "Progreso:" << std::endl;

    while (contador < 1) {

        contador++;

        evaluar(poblacion[contador - 1]);

    }

    return 0;
}
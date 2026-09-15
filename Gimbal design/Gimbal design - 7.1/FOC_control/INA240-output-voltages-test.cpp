#include <ngspice/sharedspice.h>
#include <iostream>
#include <vector>
#include <string>
#include <cmath>

std::string linea0;
std::string linea1;
std::string linea2;
std::string linea3;
std::string linea4;
std::string linea5;
std::string linea6;
std::string linea7;

char* circuito[9];

int miSendChar(char*, int, void*) {
    return 0;
}

int miSendStat(char*, int, void*) {
    return 0;
}

int miControlledExit(int, bool, bool, int, void*) {
    return 0;
}

int main() {

    _putenv_s(
        "SPICE_SCRIPTS",
        "C:\\Users\\usuario\\Desktop\\MSYS2\\ucrt64\\share\\ngspice\\scripts"
    );

    ngSpice_Init(
        miSendChar,
        miSendStat,
        miControlledExit,
        nullptr,
        nullptr,
        nullptr,
        nullptr
    );

    ngSpice_Command("set ngbehavior=ps");

    linea0 = "TEST INA240";
    linea1 = ".include INA240A1.lib";
    linea2 = "VCC VCC 0 5";
    linea3 = "VREF ref 0 2.5";

    linea4 =
        "V1 in 0 PWL("
        "0u 0 "
        "50u 0.025 "
        "100u 0 "
        "150u 0.05 "
        "200u 0 "
        "250u 0.075 "
        "300u 0 "
        "350u 0.1 "
        "400u 0 "
        "450u 0.05 "
        "500u 0.025 "
        "550u 0.075 "
        "600u 0.05 "
        "650u 0.1 "
        "700u 0.05 "
        "750u 0.075 "
        "800u 0.025 "
        "850u 0.05 "
        "900u 0"
        ")";

    linea5 = "XINA out in 0 ref ref VCC 0 INA240A1";

    linea6 = ".tran 1u 900u";
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
    char nombre_in[] = "in";
    char nombre_out[] = "out";

    vector_info* info_time = ngGet_Vec_Info(nombre_time);

    if (info_time == nullptr) {
        std::cout << "No se pudo obtener el vector time.\n";
        return 1;
    }

    double* datos_time = info_time->v_realdata;
    int longitud_time = info_time->v_length;

    vector_info* info_in = ngGet_Vec_Info(nombre_in);

    if (info_in == nullptr) {
        std::cout << "No se pudo obtener el vector in.\n";
        return 1;
    }

    double* datos_in = info_in->v_realdata;
    int longitud_in = info_in->v_length;

    vector_info* info_out = ngGet_Vec_Info(nombre_out);

    if (info_out == nullptr) {
        std::cout << "No se pudo obtener el vector out.\n";
        return 1;
    }

    double* datos_out = info_out->v_realdata;
    int longitud_out = info_out->v_length;

    int longitud = longitud_time;

    if (longitud_in < longitud) {
        longitud = longitud_in;
    }

    if (longitud_out < longitud) {
        longitud = longitud_out;
    }

    std::cout << "Numero de puntos de simulacion: "
              << longitud
              << "\n\n";

    std::cout << "Tiempo | Entrada IN | Salida OUT\n";
    std::cout << "--------------------------------\n";

    for (int punto = 0; punto <= 18; punto++) {

        double tiempo_objetivo = punto * 50.0e-6;

        int indice_mas_cercano = 0;

        double error_minimo =
            std::abs(datos_time[0] - tiempo_objetivo);

        for (int i = 1; i < longitud; i++) {

            double error =
                std::abs(datos_time[i] - tiempo_objetivo);

            if (error < error_minimo) {
                error_minimo = error;
                indice_mas_cercano = i;
            }
        }

        double tiempo_us =
            datos_time[indice_mas_cercano] * 1e6;

        double entrada =
            datos_in[indice_mas_cercano];

        double salida =
            datos_out[indice_mas_cercano];

        std::cout << tiempo_us
                  << " us | "
                  << entrada
                  << " V | "
                  << salida
                  << " V\n";
    }

    return 0;
}
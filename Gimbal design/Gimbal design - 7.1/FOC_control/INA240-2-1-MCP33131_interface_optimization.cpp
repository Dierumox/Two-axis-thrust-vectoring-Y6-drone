#include <ngspice/sharedspice.h>

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

struct Resultado {
    double R;
    double C;
    double fitness;
};

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
std::string linea12;

char* circuito[20];

int miSendChar(char*, int, void*) {
    return 0;
}

int miSendStat(char*, int, void*) {
    return 0;
}

int miControlledExit(int, bool, bool, int, void*) {
    return 0;
}

void crearCircuitoBase() {

    linea0 = "FOC RC OPTIMIZER";

    linea1 = ".include INA240A1.lib";

    linea2 = "VCC VCC 0 5";

    linea3 = "VREF ref 0 2.5";

    /*
        La secuencia se genera en main() y se coloca aquí
        directamente en la fuente V1.
    */

    linea5 = "XINA out in 0 ref ref VCC 0 INA240A1";

    /*
        Divisor 2:1:
        
        R1 = R
        R2 = R

        La tensión DC ideal después del divisor es
        exactamente VINA / 2.
    */

    /*
        La entrada del MCP33131:

        CPIN  = 2 pF permanente
        RSON  = 200 ohm durante adquisición
        CS    = 31 pF durante adquisición

        El condensador CS queda aislado durante conversión
        y conserva la carga adquirida.
    */

    linea7 =
        ".model MCP_SWITCH SW("
        "Ron=200 "
        "Roff=1e12 "
        "Vt=2.5 "
        "Vh=0"
        ")";

    /*
        Adquisición:
        300 ns de adquisición.
        El valor se evalúa 1 ns después, con el switch abierto.

        Periodo entre adquisiciones = 50 us.
    */

    linea8 =
        "VCTRL ctrl 0 "
        "PULSE(0 5 0 1n 1n 300n 50u)";

    linea9 =
        "SADC sample div ctrl 0 MCP_SWITCH";

    linea10 =
        "CSAMPLE sample 0 31p";

    linea11 =
        "CPIN div 0 2p";

    /*
        La simulación completa tendrá una duración determinada
        por la cantidad de puntos de la secuencia.
    */

    linea12 = ".options method=gear";
}

int ejecutarSimulacion(
    const std::string& pwl,
    double R,
    double C,
    double tiempo_final,
    const std::vector<double>& tiempos_muestreo,
    std::vector<double>& muestras
) {

    std::ostringstream r_string;
    std::ostringstream c_string;
    std::ostringstream tran_string;

    r_string << std::setprecision(12) << R;
    c_string << std::setprecision(12) << C;

    tran_string
        << ".tran 100n "
        << std::setprecision(12)
        << tiempo_final;

    linea4 = "V1 in 0 PWL(" + pwl + ")";

    linea6 =
        "R_TOP out div "
        + r_string.str();

    std::string linea6b =
        "R_BOTTOM div 0 "
        + r_string.str();

    std::string linea6c =
        "C_FILTER div 0 "
        + c_string.str();

    std::string linea13 = tran_string.str();

    circuito[0] = linea0.data();
    circuito[1] = linea1.data();
    circuito[2] = linea2.data();
    circuito[3] = linea3.data();
    circuito[4] = linea4.data();
    circuito[5] = linea5.data();
    circuito[6] = linea6.data();
    circuito[7] = linea6b.data();
    circuito[8] = linea6c.data();
    circuito[9] = linea7.data();
    circuito[10] = linea8.data();
    circuito[11] = linea9.data();
    circuito[12] = linea10.data();
    circuito[13] = linea12.data();
    circuito[14] = linea13.data();
    circuito[15] = ".end";
    circuito[16] = nullptr;

    /*
        Elimina los resultados de la simulación anterior
        antes de cargar el siguiente circuito.
    */

    ngSpice_Command("destroy all");
    ngSpice_Command("remcirc");

    int error = ngSpice_Circ(circuito);

    if (error != 0) {
        return 1;
    }

    ngSpice_Command("run");

    char nombre_time[] = "time";
    char nombre_sample[] = "sample";

    vector_info* info_time =
        ngGet_Vec_Info(nombre_time);

    vector_info* info_sample =
        ngGet_Vec_Info(nombre_sample);

    if (info_time == nullptr ||
        info_sample == nullptr) {

        return 2;
    }

    double* time_data =
        info_time->v_realdata;

    double* sample_data =
        info_sample->v_realdata;

    int n_time = info_time->v_length;
    int n_sample = info_sample->v_length;

    int n = std::min(n_time, n_sample);

    muestras.clear();

    for (double tiempo_objetivo : tiempos_muestreo) {

        int indice = 0;

        double error_minimo =
            std::abs(
                time_data[0] -
                tiempo_objetivo
            );

        for (int i = 1; i < n; i++) {

            double error =
                std::abs(
                    time_data[i] -
                    tiempo_objetivo
                );

            if (error < error_minimo) {
                error_minimo = error;
                indice = i;
            }
        }

        muestras.push_back(
            sample_data[indice]
        );
    }

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

    crearCircuitoBase();

    /*
        ========================================================
        SECUENCIA DE CORRIENTE
        ========================================================

        Cada valor representa la corriente objetivo en amperios.

        Se utiliza una secuencia determinista con:
        - niveles bajos
        - niveles medios
        - niveles altos
        - subidas
        - bajadas
        - cambios grandes
        - cambios pequeños
        - valores aleatorios reproducibles
    */

    std::vector<double> corriente;

    corriente = {
        0.00,
        0.25,
        0.00,
        0.50,
        0.00,
        0.75,
        0.00,
        1.00,
        0.00,
        0.50,
        0.25,
        0.75,
        0.50,
        1.00,
        0.50,
        0.75,
        0.25,
        0.50,
        0.00
    };

    /*
        Añadimos una secuencia determinista adicional.
        No usamos rand() para que todas las ejecuciones
        sean exactamente reproducibles.
    */

    const double secuencia_extra[] = {
        0.10, 0.20, 0.30, 0.40, 0.50,
        0.60, 0.70, 0.80, 0.90, 1.00,

        1.00, 0.90, 0.80, 0.70, 0.60,
        0.50, 0.40, 0.30, 0.20, 0.10,

        0.05, 0.95,
        0.10, 0.90,
        0.15, 0.85,
        0.20, 0.80,
        0.25, 0.75,
        0.30, 0.70,
        0.35, 0.65,
        0.40, 0.60,
        0.45, 0.55,

        0.55, 0.45,
        0.60, 0.40,
        0.65, 0.35,
        0.70, 0.30,
        0.75, 0.25,
        0.80, 0.20,
        0.85, 0.15,
        0.90, 0.10,
        0.95, 0.05,

        0.00,
        0.50,
        1.00,
        0.00,
        0.25,
        1.00,
        0.75,
        0.00,
        0.90,
        0.20,
        0.60,
        0.10,
        0.80,
        0.30,
        1.00,
        0.40,
        0.00
    };

    for (double valor : secuencia_extra) {
        corriente.push_back(valor);
    }

    /*
        ========================================================
        TIEMPOS
        ========================================================

        La primera adquisición empieza en t = 0.

        El ADC adquiere durante 300 ns y luego se evalúa
        la tensión retenida en el condensador de muestreo
        1 ns después.

        Por tanto:

        0 A inicial:
            301 ns

        siguiente muestra:
            50 us + 301 ns

        etc.
    */

    std::vector<double> tiempos_muestreo;

    for (int i = 0; i < corriente.size(); i++) {

        double tiempo =
            i * 50.0e-6 + 301.0e-9;

        tiempos_muestreo.push_back(tiempo);
    }

    /*
        ========================================================
        IDEALES
        ========================================================

        INA ideal:

            VINA = 2.5 + 2*I

        Divisor 2:1:

            VADC_ideal = (2.5 + 2*I) / 2

                       = 1.25 + I
    */

    std::vector<double> ideal;

    for (double I : corriente) {

        double valor_ideal =
            1.25 + I;

        ideal.push_back(valor_ideal);
    }

    /*
        ========================================================
        PWL DEL INA REAL
        ========================================================

        La corriente se convierte en tensión del shunt:

            Vshunt = I * 0.1

        La fuente V1 representa esa tensión.
    */

    std::ostringstream pwl_stream;

    pwl_stream << std::setprecision(12);

    for (int i = 0; i < corriente.size(); i++) {

        double tiempo =
            i * 50.0e-6;

        double v_shunt =
            corriente[i] * 0.1;

        pwl_stream
            << tiempo
            << " "
            << v_shunt
            << " ";
    }

    std::string pwl =
        pwl_stream.str();

    /*
        ========================================================
        PRIMERA SIMULACIÓN: INA240 REAL
        ========================================================

        Esto genera la señal real del INA240 que después
        alimentará cada simulación del filtro.

        En esta versión, la simulación del filtro contiene
        físicamente el INA240, por lo que la señal real queda
        determinada directamente por el modelo.
    */

    /*
        ========================================================
        RANGOS DE BÚSQUEDA
        ========================================================

        R es el valor de LAS DOS resistencias del divisor.

        C es el condensador del filtro.

        Estos rangos son deliberadamente amplios.
    */

    const std::vector<double> valores_R = {

        10.0,
        15.0,
        22.0,
        33.0,
        47.0,
        68.0,
        100.0,
        150.0,
        220.0,
        330.0,
        470.0,
        680.0,
        1000.0,
        1500.0,
        2200.0,
        3300.0,
        4700.0,
        6800.0,
        10000.0
    };

    const std::vector<double> valores_C = {

        1e-12,
        2.2e-12,
        4.7e-12,
        10e-12,
        22e-12,
        47e-12,
        100e-12,
        220e-12,
        470e-12,
        1e-9,
        2.2e-9,
        4.7e-9,
        10e-9
    };

    /*
        ========================================================
        OPTIMIZACIÓN
        ========================================================
    */

    std::vector<Resultado> resultados;

    int total =
        valores_R.size() *
        valores_C.size();

    int actual = 0;

    double tiempo_final =
        (corriente.size() - 1) * 50.0e-6
        + 1.0e-6;

    for (double R : valores_R) {

        for (double C : valores_C) {

            actual++;

            std::cout
                << "["
                << actual
                << "/"
                << total
                << "] "
                << "R = "
                << R
                << " ohm, C = "
                << C
                << " F"
                << std::endl;

            std::vector<double> muestras;

            int error =
                ejecutarSimulacion(
                    pwl,
                    R,
                    C,
                    tiempo_final,
                    tiempos_muestreo,
                    muestras
                );

            if (error != 0) {

                std::cout
                    << "  ERROR DE SIMULACION"
                    << std::endl;

                continue;
            }

            if (muestras.size() != ideal.size()) {

                std::cout
                    << "  Numero de muestras incorrecto"
                    << std::endl;

                continue;
            }

            /*
                FITNESS:
                
                MAE = error absoluto medio

                La referencia es SIEMPRE el valor ideal.
            */

            double suma_error = 0.0;

            for (int i = 0; i < ideal.size(); i++) {

                suma_error +=
                    std::abs(
                        muestras[i] -
                        ideal[i]
                    );
            }

            double fitness =
                suma_error /
                static_cast<double>(
                    ideal.size()
                );

            resultados.push_back({
                R,
                C,
                fitness
            });

            std::cout
                << "  FITNESS = "
                << std::scientific
                << fitness
                << " V"
                << std::endl;
        }
    }

    /*
        ========================================================
        ORDENAR
        ========================================================
    */

    std::sort(
        resultados.begin(),
        resultados.end(),
        [](const Resultado& a,
           const Resultado& b) {

            return a.fitness <
                   b.fitness;
        }
    );

    /*
        ========================================================
        RESULTADOS
        ========================================================
    */

    std::cout << "\n";
    std::cout << "========================================\n";
    std::cout << "RESULTADOS\n";
    std::cout << "========================================\n";

    int cantidad_mostrar =
        std::min(
            20,
            static_cast<int>(
                resultados.size()
            )
        );

    for (int i = 0; i < cantidad_mostrar; i++) {

        std::cout
            << std::setw(2)
            << i + 1
            << " | R = "
            << std::setw(8)
            << resultados[i].R
            << " ohm"
            << " | C = "
            << std::scientific
            << resultados[i].C
            << " F"
            << " | MAE = "
            << resultados[i].fitness
            << " V\n";
    }

    if (!resultados.empty()) {

        std::cout << "\n";
        std::cout << "MEJOR COMBINACION:\n";

        std::cout
            << "R = "
            << resultados[0].R
            << " ohm\n";

        std::cout
            << "C = "
            << std::scientific
            << resultados[0].C
            << " F\n";

        std::cout
            << "Fitness MAE = "
            << resultados[0].fitness
            << " V\n";
    }

    std::cout << "\n";

    return 0;
}
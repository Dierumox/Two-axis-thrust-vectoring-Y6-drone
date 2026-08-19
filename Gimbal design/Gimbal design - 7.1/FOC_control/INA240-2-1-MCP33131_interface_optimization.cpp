#include <vector>
#include <iterator>
#include <iostream>

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

struct Individuo {
    int indiceR;
    int indiceC;
    double fitness;
};

std::vector<Individuo> poblacion;

void generar_poblacion() {
    for(int no_R = 0; no_R < std::size(valoresR); no_R++) {
        for(int no_C = 0; no_C < std::size(valoresC); no_C++) {
            Individuo individuo;
            individuo.indiceR = no_R;
            individuo.indiceC = no_C;
            poblacion.push_back(individuo);
        }
    }
}

void evaluar(Individuo& individuo) {

}

int main() {
    generar_poblacion();
    double contador = 0;
    int contador_secundario = 0;
    int contador_terciario = 0;
    std::cout << "Progreso:" << std::endl;

    while(contador < poblacion.size()) {
        contador += 1;
        contador_secundario += 1;
        individuo_objetivo = contador - 1;
        evaluar(poblacion[individuo_objetivo])
        if(contador_secundario == 100) {
            contador_secundario = 0;
            contador_terciario +=1;
            std::cout << contador_terciario * 100 << "/" << poblacion.size() << std::endl;
        }
    }

    int mejor = 0;

    for(int i = 1; i < poblacion.size(); i++) {
        if(poblacion[i].fitness > poblacion[mejor].fitness) {
            mejor = i;
        }
    }

    std::cout << "Mejor individuo: " << mejor; << std::endl;
              << "Valor R: " << valoresR[poblacion[mejor].indiceR] << " ohm" << std::endl;
              << "Valor C: " << valoresC[poblacion[mejor].indiceC] << " F" << std::endl;

    return(0);
}
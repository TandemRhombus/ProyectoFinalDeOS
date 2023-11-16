#include <iostream>
#include "Config.h"
#include "Process.h"
#include "BuddySystem.h"
#include "RoundRobin.h"

using namespace std;

void showConfigurationMenu(Config& config) {
    int choice = 0;
    while (choice != 6) {
        cout << "\n=== Menu de Configuracion ===\n";
        cout << "1. Establecer Cuanto de Procesamiento (De cada proceso)\n";
        cout << "2. Establecer Tamano Maximo de Memoria de Proceso\n";
        cout << "3. Establecer Tamano Maximo del Cuanto del Sistema\n";
        cout << "4. Establecer Tamano de la Memoria\n";
        cout << "5. Establecer Tamano del SPLIT\n";
        cout << "6. Salir\n";
        cout << "Seleccione una opcion: ";
        cin >> choice;

        int value;
        switch (choice) {
            case 1:
                cout << "Ingrese el Cuánto de Procesamiento: ";
                cin >> value;
                config.processQuantum = value;
                break;
            case 2:
                cout << "Ingrese el Tamano Maximo de Memoria de Proceso (en Bytes): ";
                cin >> value;
                config.maxProcessMemorySize = value;
                break;
            case 3:
                cout << "Ingrese el Tamano Máximo del Cuanto del Sistema: ";
                cin >> value;
                config.maxSystemQuantum = value;
                break;
            case 4:
                // Validar que el valor ingresado sea 1 o 4 o 8
                while(value != 1 && value != 4 && value != 8) {
                    cout << "Ingrese el Tamano de la Memoria (en MB de 1, 4 o 8): ";
                    cin >> value;
                    // Validar que el valor ingresado sea 1 o 4 o 8
                    if (value != 1 && value != 4 && value != 8) {
                        cout << "Opcion no valida, intente de nuevo.\n";
                    }
                    config.memorySize = value;
                }
                break;
            case 5:
                while(value < 32) {
                    cout << "Ingrese el Tamano Minimo de Split (en KB y mayor o igual a 32): ";
                    cin >> value;
                    // Validar que el valor ingresado sea mayor o igual a 32
                    if (value < 32) {
                        cout << "Opcion no valida, intente de nuevo.\n";
                    }
                    config.minSplitSize = value;
                }
                break;
            case 6:
                cout << "Saliendo del menú de configuración...\n";
                break;
            default:
                cout << "Opción no válida, intente de nuevo.\n";
        }
    }
}


int main() {
    Config config;
    Process process;
    BuddySystem buddySystem(config.getMemorySize(), config.getMinSplitSize());
    RoundRobin roundRobin(config.getMaxSystemQuantum());
    showConfigurationMenu(config);
    //Asignaciones de configuraciónes
    process.setQuantum(config.getProcessQuantum());
    buddySystem.setMaxSize(config.getMaxProcessMemorySize());

    return 0;
}

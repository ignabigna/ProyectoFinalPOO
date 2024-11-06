#include <iostream>
#include <string>
#include "ClienteXMLRPC.h"

void mostrarMenuInicio() {
    std::cout << "\n--- ***Cliente*** ---" << std::endl;
    std::cout << "1. Ingresar usuario y contraseña" << std::endl;
    std::cout << "2. Registrar nuevo usuario" << std::endl;
    std::cout << "Seleccione una opción: ";
}

void mostrarMenuAutenticado() {
    std::cout << "\n--- ***Opciones disponibles*** ---" << std::endl;
    std::cout << "1. Solicitar saludo al servidor" << std::endl;
    std::cout << "2. Solicitar listado de GCodes" << std::endl;
    std::cout << "3. Ingresar comando GCode" << std::endl;
    std::cout << "4. Conectar al robot" << std::endl;
    std::cout << "5. Desconectar el robot" << std::endl;
    std::cout << "6. Activar motores" << std::endl;
    std::cout << "7. Desactivar motores" << std::endl;
    std::cout << "8. Iniciar modo automático" << std::endl;
    std::cout << "9. Salir" << std::endl;
    std::cout << "Seleccione una opción: ";
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Uso: " << argv[0] << " <IP> <puerto>" << std::endl;
        return 1;
    }

    std::string serverHost = argv[1];
    int serverPort = std::stoi(argv[2]);
    ClienteXMLRPC cliente(serverHost, serverPort);
    bool autenticado = false;
    std::string usuario;

    while (true) {
        if (!autenticado) {
            mostrarMenuInicio();
        } else {
            mostrarMenuAutenticado();
        }

        int opcion;
        std::cin >> opcion;

        if (!autenticado) {
            if (opcion == 1) {
                std::string contraseña;
                std::cout << "Ingrese su usuario: ";
                std::cin >> usuario;
                std::cout << "Ingrese su contraseña: ";
                std::cin >> contraseña;
                autenticado = cliente.autenticarUsuario(usuario, contraseña);
                if (autenticado) std::cout << "Autenticación exitosa." << std::endl;
                else std::cout << "Usuario o contraseña incorrectos." << std::endl;
            }
            else if (opcion == 2) {
                std::string nuevoUsuario, nuevaContraseña;
                std::cout << "Ingrese un nuevo usuario: ";
                std::cin >> nuevoUsuario;
                std::cout << "Ingrese una nueva contraseña: ";
                std::cin >> nuevaContraseña;
                cliente.registrarUsuario(nuevoUsuario, nuevaContraseña);
            }
        } else {
            switch (opcion) {
                case 1: cliente.solicitarSaludo(usuario); break;
                case 2: cliente.obtenerListadoGCodes(); break;
                case 3: {
                    std::string gcode;
                    std::cout << "Ingrese un código GCode: ";
                    std::cin >> gcode;
                    cliente.enviarGCode(usuario, gcode);
                    break;
                }
                case 4: cliente.conectarRobot(); break;
                case 5: cliente.desconectarRobot(); break;
                case 6: cliente.activarMotores(); break;
                case 7: cliente.desactivarMotores(); break;
                case 8: cliente.modoAutomatico(); break;
                case 9: std::cout << "Saliendo del programa..." << std::endl; return 0;
                default: std::cerr << "Opción inválida." << std::endl;
            }
        }
    }
}

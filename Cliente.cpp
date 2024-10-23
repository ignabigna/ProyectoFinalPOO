#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include "libreria/XmlRpc.h"

class ClienteXMLRPC {
public:
    ClienteXMLRPC(const std::string& serverHost, int serverPort)
        : client(serverHost.c_str(), serverPort) 
    {
        std::cout << "Cliente conectado a " << serverHost << " en el puerto " << serverPort << std::endl;
    }

    void solicitarSaludo(const std::string& nombre) {
        XmlRpc::XmlRpcValue args, result;
        args[0] = nombre;

        try {
            std::cout << "Enviando solicitud de saludo al servidor..." << std::endl;
            client.execute("saludar", args, result);

            std::string saludo = static_cast<std::string>(result);
            std::cout << "Respuesta del servidor: " << saludo << std::endl;

        } catch (XmlRpc::XmlRpcException& e) {
            std::cerr << "Error al contactar al servidor: " << e.getMessage() << std::endl;
        }
    }

    void enviarGCode(const std::string& usuario, const std::string& gcode) {
        XmlRpc::XmlRpcValue args, result;
        args[0] = usuario;
        args[1] = gcode;

        try {
            std::cout << "Enviando comando GCode al servidor..." << std::endl;
            client.execute("enviarGCode", args, result);

            std::string respuesta = static_cast<std::string>(result);
            std::cout << "Respuesta del servidor: " << respuesta << std::endl;

        } catch (XmlRpc::XmlRpcException& e) {
            std::cerr << "Error al contactar al servidor: " << e.getMessage() << std::endl;
        }
    }

private:
    XmlRpc::XmlRpcClient client; 
};

// Función para verificar credenciales en el archivo CSV
bool verificarCredenciales(const std::string& usuario, const std::string& contraseña) {
    std::ifstream archivo("usuarios.csv");
    std::string linea, u, p;
    
    if (!archivo.is_open()) {
        std::cerr << "No se pudo abrir el archivo CSV." << std::endl;
        return false;
    }

    // Leer el archivo línea por línea
    while (std::getline(archivo, linea)) {
        std::stringstream ss(linea);
        std::getline(ss, u, ',');
        std::getline(ss, p, ',');

        // Comprobar si el usuario y la contraseña coinciden
        if (u == usuario) {
            if (p == contraseña) {
                return true; // Usuario y contraseña correctos
            }
            return false; // Usuario correcto, pero contraseña incorrecta
        }
    }

    return false;
}

// Función para verificar si el código GCode está en el archivo CSV
bool verificarGCode(const std::string& gcode) {
    std::ifstream archivo("codigos_gcode.csv");
    std::string linea, codigo;

    if (!archivo.is_open()) {
        std::cerr << "No se pudo abrir el archivo de códigos GCode." << std::endl;
        return false;
    }

    // Leer el archivo línea por línea
    while (std::getline(archivo, linea)) {
        std::stringstream ss(linea);
        std::getline(ss, codigo, ',');
        
        // Comprobar si el código coincide
        if (codigo == gcode) {
            return true;
        }
    }
    
    return false;
}

// Función para registrar un nuevo usuario
void registrarNuevoUsuario(const std::string& usuario, const std::string& contraseña) {
    std::ofstream archivo("usuarios.csv", std::ios::app); // Abrir el archivo en modo de añadir
    if (archivo.is_open()) {
        archivo << usuario << "," << contraseña << std::endl;
        std::cout << "Usuario registrado correctamente." << std::endl;
    } else {
        std::cerr << "Error al abrir el archivo CSV para escritura." << std::endl;
    }
}

// Menú principal
void mostrarMenu() {
    std::cout << "\n--- ***Cliente*** ---" << std::endl;
    std::cout << "1. Ingresar usuario y contraseña" << std::endl;
    std::cout << "2. Registrar nuevo usuario" << std::endl;
    std::cout << "3. Solicitar saludo al servidor" << std::endl;
    std::cout << "4. Ingresar comando GCode" << std::endl;
    std::cout << "5. Salir" << std::endl;
    std::cout << "Seleccione una opcion: ";
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Uso: " << argv[0] << " <IP> <puerto>" << std::endl;
        return 1;
    }

    std::string serverHost = argv[1];
    int serverPort = std::stoi(argv[2]);

    ClienteXMLRPC cliente(serverHost, serverPort);
    std::string usuario, contraseña;
    bool autenticado = false; // Variable para controlar si está autenticado
    int opcion;

    do {
        mostrarMenu();
        std::cin >> opcion;

        switch (opcion) {
            case 1: {
                std::cout << "Ingrese su usuario: ";
                std::cin >> usuario;
                std::cout << "Ingrese su contraseña: ";
                std::cin >> contraseña;

                if (verificarCredenciales(usuario, contraseña)) {
                    std::cout << "Usuario autenticado correctamente." << std::endl;
                    autenticado = true;
                } else {
                    std::cerr << "Credenciales incorrectas." << std::endl;
                    autenticado = false; // Desautenticado si las credenciales son incorrectas
                }
                break;
            }
            case 2: {
                std::string nuevoUsuario, nuevaContraseña;
                std::cout << "Ingrese un nuevo usuario: ";
                std::cin >> nuevoUsuario;
                std::cout << "Ingrese una nueva contraseña: ";
                std::cin >> nuevaContraseña;
                registrarNuevoUsuario(nuevoUsuario, nuevaContraseña);
                break;
            }
            case 3: {
                if (!autenticado) {
                    std::cerr << "Debe ingresar primero con su usuario y contraseña." << std::endl;
                } else {
                    cliente.solicitarSaludo(usuario);
                }
                break;
            }
            case 4: {
                if (!autenticado) {
                    std::cerr << "Debe ingresar primero con su usuario y contraseña." << std::endl;
                } else {
                    std::string gcode;
                    std::cout << "Ingrese un código GCode: ";
                    std::cin >> gcode;

                    if (verificarGCode(gcode)) {
                        std::cout << "Código GCode válido. ¿Desea enviarlo al servidor? (s/n): ";
                        char confirmacion;
                        std::cin >> confirmacion;

                        if (confirmacion == 's' || confirmacion == 'S') {
                            cliente.enviarGCode(usuario, gcode);
                        } else {
                            std::cout << "Envío cancelado." << std::endl;
                        }
                    } else {
                        std::cerr << "Código GCode no válido." << std::endl;
                    }
                }
                break;
            }
            case 5:
                std::cout << "Saliendo..." << std::endl;
                break;
            default:
                std::cerr << "Opcion invalida. Intente nuevamente." << std::endl;
                break;
        }
    } while (opcion != 5);

    return 0;
}

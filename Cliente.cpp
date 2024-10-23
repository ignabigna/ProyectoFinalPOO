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

void obtenerListadoGCodes() {
    XmlRpc::XmlRpcValue params;  // Parámetro vacío
    XmlRpc::XmlRpcValue result;

    try {
        std::cout << "Solicitando listado de GCodes al servidor..." << std::endl;
        client.execute("obtenerListadoGCodes", params, result);  // Ahora pasa 3 parámetros

        std::cout << "Listado de GCodes disponibles:\n";
        for (int i = 0; i < result.size(); ++i) {
            std::cout << result[i] << std::endl;
        }

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
        if (u == usuario && p == contraseña) {
            return true;
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
    std::cout << "4. Solicitar listado de GCodes" << std::endl;
    std::cout << "5. Ingresar comando GCode" << std::endl;
    std::cout << "6. Salir" << std::endl;
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

    while (true) {
        mostrarMenu();
        int opcion;
        std::cin >> opcion;

        if (opcion == 1) {
            // Lógica para ingresar usuario y contraseña
            std::string usuario, contraseña;
            std::cout << "Ingrese usuario: ";
            std::cin >> usuario;
            std::cout << "Ingrese contraseña: ";
            std::cin >> contraseña;

            if (verificarCredenciales(usuario, contraseña)) {
                std::cout << "Autenticación exitosa." << std::endl;
            } else {
                std::cout << "Usuario o contraseña incorrectos." << std::endl;
            }

        } else if (opcion == 2) {
            // Lógica para registrar nuevo usuario
            std::string nuevoUsuario, nuevaContraseña;
            std::cout << "Ingrese nuevo usuario: ";
            std::cin >> nuevoUsuario;
            std::cout << "Ingrese nueva contraseña: ";
            std::cin >> nuevaContraseña;

            registrarNuevoUsuario(nuevoUsuario, nuevaContraseña);

        } else if (opcion == 3) {
            // Lógica para solicitar saludo al servidor
            std::string nombre;
            std::cout << "Ingrese su nombre: ";
            std::cin >> nombre;
            cliente.solicitarSaludo(nombre);

        } else if (opcion == 4) {
            // Lógica para solicitar el listado de GCodes
            cliente.obtenerListadoGCodes();

        } else if (opcion == 5) {
            // Lógica para enviar un comando GCode
            std::string usuario, gcode;
            std::cout << "Ingrese su nombre de usuario: ";
            std::cin >> usuario;
            std::cout << "Ingrese el comando GCode que desea enviar: ";
            std::cin >> gcode;

            if (verificarGCode(gcode)) {
                cliente.enviarGCode(usuario, gcode);
            } else {
                std::cout << "El código GCode no es válido." << std::endl;
            }

        } else if (opcion == 6) {
            std::cout << "Saliendo del programa..." << std::endl;
            break;
        } else {
            std::cout << "Opción no válida. Intente nuevamente." << std::endl;
        }
    }

    return 0;
}

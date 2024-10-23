#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include "libreria/XmlRpc.h"

// Declaración de funciones
bool verificarCredenciales(const std::string& usuario, const std::string& contraseña);
void registrarNuevoUsuario(const std::string& nuevoUsuario, const std::string& nuevaContraseña);
bool verificarGCode(const std::string& gcode);

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
        XmlRpc::XmlRpcValue params;
        XmlRpc::XmlRpcValue result;

        try {
            std::cout << "Solicitando listado de GCodes al servidor..." << std::endl;
            client.execute("obtenerListadoGCodes", params, result);

            std::cout << "Listado de GCodes disponibles:\n";
            for (int i = 0; i < result.size(); ++i) {
                std::cout << result[i] << std::endl;
            }

        } catch (XmlRpc::XmlRpcException& e) {
            std::cerr << "Error al contactar al servidor: " << e.getMessage() << std::endl;
        }
    }

    void gestionarConexionRobot(bool activar) {
        XmlRpc::XmlRpcValue args, result;
        args[0] = activar;

        try {
            std::string accion = activar ? "Activar" : "Desactivar";
            std::cout << accion << " conexión con el robot..." << std::endl;
            client.execute("gestionarConexionRobot", args, result);

            std::string respuesta = static_cast<std::string>(result);
            std::cout << "Respuesta del servidor: " << respuesta << std::endl;

        } catch (XmlRpc::XmlRpcException& e) {
            std::cerr << "Error al contactar al servidor: " << e.getMessage() << std::endl;
        }
    }

    void apagarServidor() {
        XmlRpc::XmlRpcValue params, result;

        try {
            std::cout << "Solicitando apagado del servidor..." << std::endl;
            client.execute("apagarServidor", params, result);

            std::string respuesta = static_cast<std::string>(result);
            std::cout << "Respuesta del servidor: " << respuesta << std::endl;

        } catch (XmlRpc::XmlRpcException& e) {
            std::cerr << "Error al contactar al servidor: " << e.getMessage() << std::endl;
        }
    }

private:
    XmlRpc::XmlRpcClient client;
};

void mostrarMenu() {
    std::cout << "\n--- ***Cliente*** ---" << std::endl;
    std::cout << "1. Ingresar usuario y contraseña" << std::endl;
    std::cout << "2. Registrar nuevo usuario" << std::endl;
    std::cout << "3. Solicitar saludo al servidor" << std::endl;
    std::cout << "4. Solicitar listado de GCodes" << std::endl;
    std::cout << "5. Ingresar comando GCode" << std::endl;
    std::cout << "6. Activar/Desactivar conexión con el robot" << std::endl;
    std::cout << "7. Apagar el servidor" << std::endl;
    std::cout << "8. Salir" << std::endl;
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
            // Registrar nuevo usuario
            std::string nuevoUsuario, nuevaContraseña;
            std::cout << "Ingrese nuevo usuario: ";
            std::cin >> nuevoUsuario;
            std::cout << "Ingrese nueva contraseña: ";
            std::cin >> nuevaContraseña;

            registrarNuevoUsuario(nuevoUsuario, nuevaContraseña);

        } else if (opcion == 3) {
            // Solicitar saludo
            std::string nombre;
            std::cout << "Ingrese su nombre: ";
            std::cin >> nombre;
            cliente.solicitarSaludo(nombre);

        } else if (opcion == 4) {
            // Solicitar listado de GCodes
            cliente.obtenerListadoGCodes();

        } else if (opcion == 5) {
            // Enviar un comando GCode
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
            // Activar/Desactivar conexión con el robot
            bool activar;
            std::cout << "¿Desea activar (1) o desactivar (0) la conexión con el robot?: ";
            std::cin >> activar;
            cliente.gestionarConexionRobot(activar);

        } else if (opcion == 7) {
            // Apagar el servidor
            cliente.apagarServidor();
            break;  // Salir después de apagar el servidor

        } else if (opcion == 8) {
            std::cout << "Saliendo del programa..." << std::endl;
            break;
        } else {
            std::cout << "Opción no válida. Intente nuevamente." << std::endl;
        }
    }

    return 0;
}

// Implementaciones de las funciones faltantes
bool verificarCredenciales(const std::string& usuario, const std::string& contraseña) {
    // Aquí va la lógica para verificar las credenciales
    // Esto es solo un ejemplo, debe ajustarse a tu lógica real
    return (usuario == "admin" && contraseña == "1234");
}

void registrarNuevoUsuario(const std::string& nuevoUsuario, const std::string& nuevaContraseña) {
    // Aquí va la lógica para registrar un nuevo usuario
    // Solo un ejemplo básico
    std::cout << "Usuario " << nuevoUsuario << " registrado exitosamente." << std::endl;
}

bool verificarGCode(const std::string& gcode) {
    // Aquí va la lógica para verificar si el GCode es válido
    // Esto es solo un ejemplo, debe ajustarse a tu lógica real
    return !gcode.empty();  // Verifica que no esté vacío
}

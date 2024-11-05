#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <filesystem>
#include <fstream>
#include <thread>
#include <chrono>
#include "libreria/XmlRpc.h"

namespace fs = std::filesystem;

class ClienteXMLRPC {
public:
    ClienteXMLRPC(const std::string& serverHost, int serverPort)
        : client(serverHost.c_str(), serverPort), robotConectado(false)
    {
        std::cout << "Cliente conectado a " << serverHost << " en el puerto " << serverPort << std::endl;
    }

    bool autenticarUsuario(const std::string& usuario, const std::string& contraseña) {
        XmlRpc::XmlRpcValue args, result;
        args[0] = usuario;
        args[1] = contraseña;

        try {
            client.execute("autenticar_usuario", args, result);
            return static_cast<bool>(result);
        } catch (XmlRpc::XmlRpcException& e) {
            std::cerr << "Error al autenticar con el servidor: " << e.getMessage() << std::endl;
            return false;
        }
    }

    void conectarRobot() {
        XmlRpc::XmlRpcValue args, result;
        try {
            std::cout << "Solicitando conexión al robot..." << std::endl;
            client.execute("conectar_robot", args, result);

            for (int i = 0; i < result.size(); ++i) {
                std::cout << "Mensaje de inicialización: " << result[i] << std::endl;
            }
            robotConectado = true;
        } catch (XmlRpc::XmlRpcException& e) {
            std::cerr << "Error al conectar con el robot: " << e.getMessage() << std::endl;
            robotConectado = false;
        }
    }

    void desconectarRobot() {
        XmlRpc::XmlRpcValue args, result;
        try {
            std::cout << "Solicitando desconexión del robot..." << std::endl;
            client.execute("desconectar_robot", args, result);
            std::string respuesta = static_cast<std::string>(result);
            std::cout << "Respuesta del servidor: " << respuesta << std::endl;
            robotConectado = false;
        } catch (XmlRpc::XmlRpcException& e) {
            std::cerr << "Error al desconectar el robot: " << e.getMessage() << std::endl;
        }
    }

    void activarMotores() {
        XmlRpc::XmlRpcValue args, result;
        try {
            std::cout << "Enviando solicitud de activación de motores al servidor..." << std::endl;
            client.execute("activar_motores", args, result);
            std::cout << "Respuesta del servidor: " << static_cast<std::string>(result) << std::endl;
        } catch (XmlRpc::XmlRpcException& e) {
            std::cerr << "Error al contactar al servidor: " << e.getMessage() << std::endl;
        }
    }

    void desactivarMotores() {
        XmlRpc::XmlRpcValue args, result;
        try {
            std::cout << "Enviando solicitud de desactivación de motores al servidor..." << std::endl;
            client.execute("desactivar_motores", args, result);
            std::cout << "Respuesta del servidor: " << static_cast<std::string>(result) << std::endl;
        } catch (XmlRpc::XmlRpcException& e) {
            std::cerr << "Error al contactar al servidor: " << e.getMessage() << std::endl;
        }
    }

    void registrarUsuario(const std::string& usuario, const std::string& contraseña) {
        XmlRpc::XmlRpcValue args, result;
        args[0] = usuario;
        args[1] = contraseña;

        try {
            client.execute("registrar_usuario", args, result);
            std::cout << static_cast<std::string>(result) << std::endl;
        } catch (XmlRpc::XmlRpcException& e) {
            std::cerr << "Error al registrar con el servidor: " << e.getMessage() << std::endl;
        }
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

    void enviarGCode(const std::string& usuario, const std::string& gcode) {
        if (!robotConectado) {
            std::cerr << "Error: Debe conectar al robot antes de enviar un G-Code." << std::endl;
            return;
        }

        XmlRpc::XmlRpcValue args, result;
        args[0] = usuario;
        args[1] = gcode;

        try {
            std::cout << "Enviando comando GCode al servidor..." << std::endl;
            client.execute("enviarGCode", args, result);
            std::string respuesta = static_cast<std::string>(result);
            std::cout << "Respuesta del servidor: " << respuesta << std::endl;
        } catch (XmlRpc::XmlRpcException& e) {
            std::cerr << "Error al enviar el comando GCode: " << e.getMessage() << std::endl;
        }
    }

    void modoAutomatico() {
        std::string carpeta = "M_Automatico";
        for (const auto& archivo : fs::directory_iterator(carpeta)) {
            if (archivo.path().extension() == ".csv") {
                std::cout << "Leyendo archivo: " << archivo.path().filename() << std::endl;
                
                std::ifstream archivoCSV(archivo.path());
                std::string linea;
                while (std::getline(archivoCSV, linea)) {
                    if (!robotConectado) {
                        std::cerr << "Error: Debe conectar al robot antes de iniciar el modo automático." << std::endl;
                        return;
                    }

                    std::cout << "Enviando comando GCode: " << linea << std::endl;
                    enviarGCode("En automatico", linea);

                    std::this_thread::sleep_for(std::chrono::seconds(5));
                }
            }
        }
    }

private:
    XmlRpc::XmlRpcClient client;
    bool robotConectado;
};

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

bool esOpcionValida(int opcion, int maxOpciones) {
    if (opcion < 1 || opcion > maxOpciones) {
        std::cerr << "Error: Opción no válida. Por favor, seleccione una opción entre 1 y " << maxOpciones << "." << std::endl;
        return false;
    }
    return true;
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
            if (!esOpcionValida(opcion, 2)) continue;

            switch (opcion) {
                case 1: {
                    std::string contraseña;
                    std::cout << "Ingrese su usuario: ";
                    std::cin >> usuario;
                    std::cout << "Ingrese su contraseña: ";
                    std::cin >> contraseña;
                    autenticado = cliente.autenticarUsuario(usuario, contraseña);
                    if (autenticado) {
                        std::cout << "Autenticación exitosa." << std::endl;
                    } else {
                        std::cout << "Usuario o contraseña incorrectos." << std::endl;
                    }
                    break;
                }
                case 2: {
                    std::string nuevoUsuario, nuevaContraseña;
                    std::cout << "Ingrese un nuevo usuario: ";
                    std::cin >> nuevoUsuario;
                    std::cout << "Ingrese una nueva contraseña: ";
                    std::cin >> nuevaContraseña;
                    cliente.registrarUsuario(nuevoUsuario, nuevaContraseña);
                    break;
                }
            }
        } else {
            if (!esOpcionValida(opcion, 9)) continue;

            switch (opcion) {
                case 1:
                    cliente.solicitarSaludo(usuario);
                    break;
                case 2:
                    cliente.obtenerListadoGCodes();
                    break;
                case 3: {
                    std::string gcode;
                    std::cout << "Ingrese un código GCode: ";
                    std::cin >> gcode;

                    if (gcode == "G1") {
                        // Solicitar valores específicos para G1
                        double x, y, z, velocidad;
                        std::cout << "Ingrese el valor de X: ";
                        std::cin >> x;
                        std::cout << "Ingrese el valor de Y: ";
                        std::cin >> y;
                        std::cout << "Ingrese el valor de Z: ";
                        std::cin >> z;
                        std::cout << "Ingrese la velocidad (F): ";
                        std::cin >> velocidad;

                        // Formatear la cadena G-Code con dos decimales
                        std::ostringstream gcodeFormatted;
                        gcodeFormatted << std::fixed << std::setprecision(2);
                        gcodeFormatted << "G1 X" << x << " Y" << y << " Z" << z << " F" << velocidad;

                        // Enviar el comando formateado al servidor
                        cliente.enviarGCode(usuario, gcodeFormatted.str());
                    } else {
                        // Enviar otros comandos directamente al servidor
                        cliente.enviarGCode(usuario, gcode);
                    }
                    break;
                }
                case 4:
                    cliente.conectarRobot();
                    break;
                case 5:
                    cliente.desconectarRobot();
                    break;
                case 6:
                    cliente.activarMotores();
                    break;
                case 7:
                    cliente.desactivarMotores();
                    break;
                case 8:
                    std::cout << "Iniciando modo automático..." << std::endl;
                    cliente.modoAutomatico();
                    break;
                case 9:
                    std::cout << "Saliendo del programa..." << std::endl;
                    return 0;
                default:
                    std::cerr << "Opción inválida. Intente nuevamente." << std::endl;
            }
        }
    }
}

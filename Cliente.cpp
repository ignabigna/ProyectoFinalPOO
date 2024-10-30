#include <iostream>
#include <string>
#include "libreria/XmlRpc.h"

class ClienteXMLRPC {
public:
    ClienteXMLRPC(const std::string& serverHost, int serverPort)
        : client(serverHost.c_str(), serverPort) 
    {
        std::cout << "Cliente conectado a " << serverHost << " en el puerto " << serverPort << std::endl;
    }

    // Autenticación con el servidor
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

    // Registro de usuario en el servidor
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

    // Solicitar saludo del servidor
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

    // Solicitar listado de GCodes al servidor
    void obtenerListadoGCodes() {
        XmlRpc::XmlRpcValue params;  // Parámetro vacío
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

    // Enviar GCode al servidor
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

// Función para mostrar el menú principal
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
    bool autenticado = false;
    std::string usuario;

    while (true) {
        mostrarMenu();
        int opcion;
        std::cin >> opcion;

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
                    cliente.obtenerListadoGCodes();
                }
                break;
            }
            case 5: {
                if (!autenticado) {
                    std::cerr << "Debe ingresar primero con su usuario y contraseña." << std::endl;
                } else {
                    std::string gcode;
                    std::cout << "Ingrese un código GCode: ";
                    std::cin >> gcode;

                    std::cout << "¿Desea enviarlo al servidor? (s/n): ";
                    char confirmacion;
                    std::cin >> confirmacion;

                    if (confirmacion == 's' || confirmacion == 'S') {
                        cliente.enviarGCode(usuario, gcode);
                    } else {
                        std::cout << "Envío cancelado." << std::endl;
                    }
                }
                break;
            }
            case 6:
                std::cout << "Saliendo del programa..." << std::endl;
                return 0;
            default:
                std::cerr << "Opción inválida. Intente nuevamente." << std::endl;
        }
    }
}

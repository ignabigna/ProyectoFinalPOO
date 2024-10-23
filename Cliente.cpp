#include <iostream>
#include <string>
#include <cstdlib>
#include "libreria/XmlRpc.h"  // Asegúrate de tener esta librería incluida correctamente

class ClienteXMLRPC {
public:
    ClienteXMLRPC(const std::string& serverHost, int serverPort)
        : client(serverHost.c_str(), serverPort) // Inicializamos el cliente con el servidor
    {
        std::cout << "Cliente conectado a " << serverHost << " en el puerto " << serverPort << std::endl;
    }

    void enviarSaludo(const std::string& nombre) {
        XmlRpc::XmlRpcValue args, result;
        args[0] = nombre;  // Enviar el nombre del usuario

        try {
            std::cout << "Enviando solicitud de saludo al servidor..." << std::endl;
            client.execute("saludar", args, result);  // Ejecutamos la función "saludar" en el servidor

            std::string respuesta = static_cast<std::string>(result);
            std::cout << "Respuesta del servidor: " << respuesta << std::endl;

        } catch (XmlRpc::XmlRpcException& e) {
            std::cerr << "Error al contactar al servidor: " << e.getMessage() << std::endl;
        }
    }

private:
    XmlRpc::XmlRpcClient client;  // Cliente XML-RPC para conectarse al servidor
};

void mostrarMenu() {
    std::cout << "\n--- ***Cliente*** ---" << std::endl;
    std::cout << "1. Ingresar nombre de usuario" << std::endl;
    std::cout << "2. Enviar saludo al servidor" << std::endl;
    std::cout << "3. Salir" << std::endl;
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
    std::string nombre;
    int opcion;

    do {
        mostrarMenu();
        std::cin >> opcion;

        switch (opcion) {
            case 1:
                std::cout << "Ingrese su nombre de usuario: ";
                std::cin >> nombre;
                break;
            case 2:
                if (nombre.empty()) {
                    std::cerr << "Debe ingresar su nombre de usuario primero." << std::endl;
                } else {
                    cliente.enviarSaludo(nombre);
                }
                break;
            case 3:
                std::cout << "Saliendo..." << std::endl;
                break;
            default:
                std::cerr << "Opcion invalida. Intente nuevamente." << std::endl;
                break;
        }
    } while (opcion != 3);

    return 0;
}

#include <iostream>
#include <string>
#include <cstdlib>
#include <vector>
#include <ctime>
#include "libreria/XmlRpc.h"  // Asegurate de tener esta libreria incluida correctamente

class ClienteXMLRPC {
public:
    ClienteXMLRPC(const std::string& serverHost, int serverPort)
        : client(serverHost.c_str(), serverPort) // Inicializamos el cliente con el servidor
    {
        std::cout << "Cliente conectado a " << serverHost << " en el puerto " << serverPort << std::endl;
    }

    void solicitarNumeroAleatorio(const std::string& usuario, const std::string& password, double ci, double cs) {
        XmlRpc::XmlRpcValue args, result;
        args[0] = usuario;  // Enviar usuario
        args[1] = password; // Enviar contraseña
        args[2] = ci;       // Enviar cota inferior
        args[3] = cs;       // Enviar cota superior

        try {
            std::cout << "Enviando solicitud al servidor para generar un numero aleatorio..." << std::endl;
            client.execute("generarNumero", args, result);  // Ejecutamos la función "generarNumero" en el servidor

            double numero = static_cast<double>(result[0]);
            std::cout << "Numero generado: " << numero << std::endl;

        } catch (XmlRpc::XmlRpcException& e) {
            std::cerr << "Error al contactar al servidor: " << e.getMessage() << std::endl;
        }
    }

private:
    XmlRpc::XmlRpcClient client;  // Cliente XML-RPC para conectarse al servidor
};

void mostrarMenu() {
    std::cout << "\n--- ***Cliente*** ---" << std::endl;
    std::cout << "1. Ingresar usuario y contraseña" << std::endl;
    std::cout << "2. Ingresar cotas (ci y cs)" << std::endl;
    std::cout << "3. Solicitar numero aleatorio al servidor" << std::endl;
    std::cout << "4. Salir" << std::endl;
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
    std::string usuario, password;
    double ci = 0.0, cs = 0.0;
    int opcion;

    do {
        mostrarMenu();
        std::cin >> opcion;

        switch (opcion) {
            case 1:
                std::cout << "Ingrese su usuario: ";
                std::cin >> usuario;
                std::cout << "Ingrese su contraseña: ";
                std::cin >> password;
                break;
            case 2:
                std::cout << "Ingrese cota inferior (ci): ";
                std::cin >> ci;
                std::cout << "Ingrese cota superior (cs): ";
                std::cin >> cs;
                break;
            case 3:
                if (usuario.empty() || password.empty()) {
                    std::cerr << "Debe ingresar usuario y contraseña primero." << std::endl;
                } else if (ci == 0 && cs == 0) {
                    std::cerr << "Debe ingresar las cotas primero." << std::endl;
                } else {
                    cliente.solicitarNumeroAleatorio(usuario, password, ci, cs);
                }
                break;
            case 4:
                std::cout << "Saliendo..." << std::endl;
                break;
            default:
                std::cerr << "Opcion invalida. Intente nuevamente." << std::endl;
                break;
        }
    } while (opcion != 4);

    return 0;
}

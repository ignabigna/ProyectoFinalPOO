#include "ClienteXMLRPC.h"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <thread>
#include <chrono>

namespace fs = std::filesystem;

ClienteXMLRPC::ClienteXMLRPC(const std::string& serverHost, int serverPort)
    : client(serverHost.c_str(), serverPort), robotConectado(false)
{
    std::cout << "Cliente conectado a " << serverHost << " en el puerto " << serverPort << std::endl;
}

bool ClienteXMLRPC::autenticarUsuario(const std::string& usuario, const std::string& contraseña) {
    XmlRpc::XmlRpcValue args, result;
    args[0] = usuario;
    args[1] = contraseña;

    try {
        client.execute("autenticar_usuario", args, result);
        if (result.getType() == XmlRpc::XmlRpcValue::TypeArray && result.size() == 2) {
            bool autenticado = static_cast<bool>(result[0]);
            bool esAdmin = static_cast<bool>(result[1]);
            if (autenticado) {
                std::cout << (esAdmin ? "Usuario autenticado como administrador." : "Usuario autenticado.") << std::endl;
            }
            return autenticado;
        } else {
            std::cerr << "Respuesta inesperada del servidor." << std::endl;
            return false;
        }
    } catch (XmlRpc::XmlRpcException& e) {
        std::cerr << "Error al autenticar con el servidor: " << e.getMessage() << std::endl;
        return false;
    }
}

void ClienteXMLRPC::conectarRobot() {
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

void ClienteXMLRPC::desconectarRobot() {
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

void ClienteXMLRPC::activarMotores() {
    XmlRpc::XmlRpcValue args, result;
    try {
        std::cout << "Enviando solicitud de activación de motores al servidor..." << std::endl;
        client.execute("activar_motores", args, result);
        std::cout << "Respuesta del servidor: " << static_cast<std::string>(result) << std::endl;
    } catch (XmlRpc::XmlRpcException& e) {
        std::cerr << "Error al contactar al servidor: " << e.getMessage() << std::endl;
    }
}

void ClienteXMLRPC::desactivarMotores() {
    XmlRpc::XmlRpcValue args, result;
    try {
        std::cout << "Enviando solicitud de desactivación de motores al servidor..." << std::endl;
        client.execute("desactivar_motores", args, result);
        std::cout << "Respuesta del servidor: " << static_cast<std::string>(result) << std::endl;
    } catch (XmlRpc::XmlRpcException& e) {
        std::cerr << "Error al contactar al servidor: " << e.getMessage() << std::endl;
    }
}

void ClienteXMLRPC::registrarUsuario(const std::string& usuario, const std::string& contraseña) {
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

void ClienteXMLRPC::solicitarSaludo(const std::string& nombre) {
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

void ClienteXMLRPC::obtenerListadoGCodes() {
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

void ClienteXMLRPC::enviarGCode(const std::string& usuario, const std::string& gcode) {
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

void ClienteXMLRPC::modoAutomatico() {
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

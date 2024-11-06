#ifndef CLIENTEXMLRPC_H
#define CLIENTEXMLRPC_H

#include <string>
#include "libreria/XmlRpc.h"

class ClienteXMLRPC {
public:
    ClienteXMLRPC(const std::string& serverHost, int serverPort);
    bool autenticarUsuario(const std::string& usuario, const std::string& contraseña);
    void conectarRobot();
    void desconectarRobot();
    void activarMotores();
    void desactivarMotores();
    void registrarUsuario(const std::string& usuario, const std::string& contraseña);
    void solicitarSaludo(const std::string& nombre);
    void obtenerListadoGCodes();
    void enviarGCode(const std::string& usuario, const std::string& gcode);
    void modoAutomatico();

private:
    XmlRpc::XmlRpcClient client;
    bool robotConectado;
};

#endif

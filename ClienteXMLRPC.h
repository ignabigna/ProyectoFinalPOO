#ifndef CLIENTEXMLRPC_H
#define CLIENTEXMLRPC_H

#include <vector>
#include <utility>
#include <iostream>

class ClienteXMLRPC {
private:
    std::vector<std::pair<int, double>> numerosRecibidos;  // Mantener como privado

public:
    // Método para obtener los números recibidos
    const std::vector<std::pair<int, double>>& getNumerosRecibidos() const {
        return numerosRecibidos;
    }

    // Método para agregar un número recibido
    void agregarNumeroRecibido(int ordinal, double numero) {
        numerosRecibidos.push_back(std::make_pair(ordinal, numero));
    }

    // Método para realizar la petición (simulación, debes adaptar a tu implementación)
    void realizarPeticion(const std::string& alias, bool incluirTiempo, bool nuevaSecuencia, double ci, double cs) {
        // Simulación: Agregando un número recibido aleatorio entre ci y cs
        double numeroGenerado = ci + (static_cast<double>(rand()) / RAND_MAX) * (cs - ci);
        agregarNumeroRecibido(nuevaSecuencia ? 1 : numerosRecibidos.size() + 1, numeroGenerado);

        // Mostrar el número generado
        std::cout << "Número generado: " << numeroGenerado;
        if (incluirTiempo) {
            std::cout << " (con marca de tiempo)";
        }
        std::cout << std::endl;
    }
};

#endif // CLIENTEXMLRPC_H

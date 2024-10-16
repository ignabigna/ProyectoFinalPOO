/* holaClient.cpp : ejemplo sencillo de cliente XMLRPC. 
   Uso: holaCliente Host Port
   Nota: sobre Windows puede faltar ws2_32.lib, para poder crear los sockets
*/
#include <iostream>
#include <stdlib.h>
using namespace std;
#include "XmlRpc.h"
using namespace XmlRpc;

int main(int argc, char* argv[])
{
  if (argc != 3) {
    std::cerr << "Uso: hola_Client IP_HOST N_PORT\n";
    return -1;
  }
  
  int port = atoi(argv[2]);
  //XmlRpc::setVerbosity(5);

  // Una mirada a los métodos soportados por la API
  XmlRpcClient c(argv[1], port);
  XmlRpcValue noArgs, result;
  if (c.execute("system.listMethods", noArgs, result))
    std::cout << "\nMetodos:\n " << result << "\n\n";
  else
    std::cout << "Error en la llamada a 'listMethods'\n\n";

  // Requerimiento a la API para recuperar una ayuda sobre el método Saludo
  XmlRpcValue oneArg;
  oneArg[0] = "Saludo";
  if (c.execute("system.methodHelp", oneArg, result))
    std::cout << "Ayuda para el metodo 'Saludo': " << result << "\n\n";
  else
    std::cout << "Error en la llamada a 'methodHelp'\n\n";

  // Llamada al metodo Saludo
  if (c.execute("Saludo", noArgs, result))
    std::cout << result << "\n\n";
  else
    std::cout << "Error en la llamada a 'Saludo'\n\n";

  // Llamada al metodo SaludoNombre
  oneArg[0] = "Programador";
  if (c.execute("SaludoNombre", oneArg, result))
    std::cout << result << "\n\n";
  else
    std::cout << "Error en la llamada a 'SaludoNombre'\n\n";

  // Llamada con un array de numeros
  XmlRpcValue numbers;
  numbers[0] = 33.33;
  numbers[1] = 112.57;
  numbers[2] = 76.1;
  std::cout << "numbers.size() is " << numbers.size() << std::endl;
  if (c.execute("Sumar", numbers, result))
    std::cout << "Suma = " << double(result) << "\n\n";
  else
    std::cout << "Error en la llamada a 'Sumar'\n\n";

  // Prueba de fallo por llamada a metodo inexistente
  if (c.execute("OtroMetodo", numbers, result))
    std::cout << "Llamada a OtroMetodo: fallo: " << c.isFault() << ", resultado = " << result << std::endl;
  else
    std::cout << "Error en la llamada a 'Sumar'\n";

  // Prueba de llamada a metodos multiples. 
  // En este caso se trata de argumento unico, un array de estructuras
  XmlRpcValue multicall;
  multicall[0][0]["methodName"] = "Sumar";
  multicall[0][0]["params"][0] = 5.0;
  multicall[0][0]["params"][1] = 9.0;

  multicall[0][1]["methodName"] = "SaludoNombre";
  multicall[0][1]["params"][0] = "Juan";
    
  multicall[0][2]["methodName"] = "Sumar";
  multicall[0][2]["params"][0] = 10.5;
  multicall[0][2]["params"][1] = 12.5;

  if (c.execute("system.multicall", multicall, result))
    std::cout << "\nResultado multicall = " << result << std::endl;
  else
    std::cout << "\nError en la llamada a 'system.multicall'\n";

  char salida;
  cout << "Ingrese cualquier caracter para salir...";
  cin >> salida;  
  return 0;
}

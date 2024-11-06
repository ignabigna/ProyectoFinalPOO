import subprocess
from InterfazServidor import InterfazServidor

def mostrar_menu():
    print("Seleccione el modo de operación:")
    print("1. Controlar robot de forma remota (Iniciar cliente)")
    print("2. Controlar robot de forma local (Iniciar servidor)")
    opcion = input("Ingrese el número de la opción: ")
    return opcion

def iniciar_servidor():
    interfaz = InterfazServidor()
    if interfaz.autenticar_administrador():
        interfaz.ejecutar()  # Despliega la CLI para el administrador
    else:
        print("Acceso denegado.")

def iniciar_cliente():
    try:
        # Ejecuta el cliente en una nueva ventana de consola
        subprocess.run(["start", "cmd", "/k", "Cliente", "127.0.0.1", "8080"], shell=True)
        
        # Ejecuta el servidor en una nueva ventana de consola
        subprocess.run(["start", "cmd", "/k", "python", "servidor.py"], shell=True)

    except subprocess.CalledProcessError as e:
        print(f"Error al iniciar el cliente o el servidor: {e}")


if __name__ == "__main__":
    opcion = mostrar_menu()
    if opcion == "1":
        iniciar_cliente()
    elif opcion == "2":
        iniciar_servidor()
    else:
        print("Opción no válida. Por favor, seleccione 1 o 2.")

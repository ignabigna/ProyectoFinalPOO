import subprocess
from InterfazServidor import InterfazServidor

def mostrar_menu_principal():
    print("Seleccione el modo de operación:")
    print("1. Controlar robot de forma remota (Iniciar servidor RPC)")
    print("2. Controlar robot de forma local (Consola de servidor)")
    opcion = input("Ingrese el número de la opción: ")
    return opcion

def iniciar_servidor_rpc():
    try:
        print("Iniciando servidor RPC...")
        subprocess.run(["python", "servidor.py"], check=True)
    except subprocess.CalledProcessError as e:
        print(f"Error al iniciar el servidor RPC: {e}")

def iniciar_interfaz_servidor():
    interfaz = InterfazServidor()
    interfaz.mostrar_menu()

def iniciar_cliente():
    try:
        # Ejecutar el cliente precompilado
        subprocess.run(["./Cliente", "127.0.0.1", "8080"], check=True)
    except subprocess.CalledProcessError as e:
        print(f"Error al iniciar el cliente: {e}")

if __name__ == "__main__":
    opcion = mostrar_menu_principal()
    if opcion == "1":
        iniciar_servidor_rpc()
    elif opcion == "2":
        iniciar_interfaz_servidor()
    else:
        print("Opción no válida. Por favor, seleccione 1 o 2.")

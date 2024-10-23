from xmlrpc.server import SimpleXMLRPCServer

# Clase principal del servidor
class Servidor:
    def __init__(self):
        pass

    def saludar(self, nombre):
        # Registrar y mostrar en la consola del servidor quién solicitó el saludo
        print(f"Solicitud de saludo recibida de: {nombre}")
        return f"Hola, {nombre}! Bienvenido al servidor."

# Configuración del servidor
def iniciar_servidor():
    servidor = Servidor()
    server = SimpleXMLRPCServer(("localhost", 8080), allow_none=True)
    print("Servidor escuchando en el puerto 8080...")

    # Registrar métodos en el servidor
    server.register_instance(servidor)

    # Correr el servidor
    server.serve_forever()

if __name__ == "__main__":
    iniciar_servidor()

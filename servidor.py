from xmlrpc.server import SimpleXMLRPCServer
from datetime import datetime

# Clase que representa el robot
class Robot:
    def __init__(self):
        self.posicion_actual = 0.0
        self.estado_motores = False
        self.conectado = False

    def activar_motores(self):
        if self.conectado:
            self.estado_motores = True
            return "Motores activados"
        return "Robot no conectado"

    def desactivar_motores(self):
        if self.conectado:
            self.estado_motores = False
            return "Motores desactivados"
        return "Robot no conectado"

    def conectar(self):
        self.conectado = True
        return "Robot conectado"

    def desconectar(self):
        self.conectado = False
        return "Robot desconectado"

# Clase para manejar la autenticación de usuarios y el registro
class Archivo:
    def __init__(self):
        self.usuarios = {"admin": "1234"}  # Ejemplo de usuarios con contraseñas
        self.log = []

    def autenticar(self, usuario, contraseña):
        if usuario in self.usuarios and self.usuarios[usuario] == contraseña:
            return True
        return False

    def agregar_registro(self, registro):
        timestamp = datetime.now().strftime('%Y-%m-%d %H:%M:%S')
        self.log.append(f"{timestamp}: {registro}")

    def obtener_log(self):
        return self.log

# Clase principal del servidor
class Servidor:
    def __init__(self):
        self.robot = Robot()
        self.archivo = Archivo()
        self.clientes_conectados = []

    def autenticar(self, usuario, contraseña):
        if self.archivo.autenticar(usuario, contraseña):
            self.clientes_conectados.append(usuario)
            self.archivo.agregar_registro(f"Usuario {usuario} autenticado y conectado.")
            return True
        return False

    def conectar_robot(self):
        resultado = self.robot.conectar()
        self.archivo.agregar_registro("Robot conectado.")
        return resultado

    def desconectar_robot(self):
        resultado = self.robot.desconectar()
        self.archivo.agregar_registro("Robot desconectado.")
        return resultado

    def activar_motores(self):
        resultado = self.robot.activar_motores()
        self.archivo.agregar_registro("Motores activados.")
        return resultado

    def desactivar_motores(self):
        resultado = self.robot.desactivar_motores()
        self.archivo.agregar_registro("Motores desactivados.")
        return resultado

    def solicitar_reporte(self):
        return {
            "estado_motores": self.robot.estado_motores,
            "posicion_actual": self.robot.posicion_actual,
            "conectado": self.robot.conectado,
            "clientes_conectados": self.clientes_conectados,
            "log": self.archivo.obtener_log()
        }

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

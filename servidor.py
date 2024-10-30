import csv
import serial
from xmlrpc.server import SimpleXMLRPCServer

# Clase principal del servidor
class Servidor:
    def __init__(self):
        self.usuarios = self.cargar_usuarios("usuarios.csv")
        self.gcode_dict = self.cargar_gcode_descripciones("codigos_gcode.csv")
        self.serial_port = self.iniciar_puerto_serial("COM3")

    # Cargar los usuarios y contraseñas desde el archivo CSV
    def cargar_usuarios(self, archivo_csv):
        usuarios = {}
        try:
            with open(archivo_csv, mode="r") as file:
                reader = csv.reader(file)
                for row in reader:
                    if len(row) >= 2:
                        usuarios[row[0]] = row[1]  # usuario: contraseña
            print("Usuarios cargados correctamente.")
        except Exception as e:
            print(f"Error al cargar el archivo de usuarios: {e}")
        return usuarios

    # Registrar un nuevo usuario en el archivo CSV
    def registrar_usuario(self, usuario, contraseña):
        if usuario in self.usuarios:
            return "El usuario ya existe."
        else:
            self.usuarios[usuario] = contraseña
            try:
                with open("usuarios.csv", mode="a", newline="") as file:
                    writer = csv.writer(file)
                    writer.writerow([usuario, contraseña])
                print(f"Usuario {usuario} registrado correctamente.")
                return "Usuario registrado correctamente."
            except Exception as e:
                print(f"Error al registrar el usuario: {e}")
                return f"Error al registrar el usuario: {e}"

    # Validar credenciales de usuario
    def autenticar_usuario(self, usuario, contraseña):
        if usuario in self.usuarios and self.usuarios[usuario] == contraseña:
            print(f"Usuario {usuario} autenticado correctamente.")
            return True
        else:
            print(f"Autenticación fallida para el usuario {usuario}.")
            return False

    # Cargar los códigos G-code y sus descripciones desde el archivo CSV
    def cargar_gcode_descripciones(self, archivo_csv):
        gcode_dict = {}
        try:
            with open(archivo_csv, mode="r") as file:
                reader = csv.reader(file)
                next(reader)  # Saltar el encabezado
                for row in reader:
                    if len(row) >= 2:
                        gcode_dict[row[0]] = row[1]  # GCode: descripción
            print("Códigos G-code cargados correctamente.")
        except Exception as e:
            print(f"Error al cargar el archivo de códigos G-code: {e}")
        return gcode_dict

    # Inicializar el puerto serial
    def iniciar_puerto_serial(self, puerto):
        try:
            ser = serial.Serial(puerto, 9600, timeout=1)  # Configurar el puerto COM3
            print(f"Puerto {puerto} abierto correctamente.")
            return ser
        except serial.SerialException as e:
            print(f"Error al abrir el puerto serial: {e}")
            return None

    # Enviar G-code al puerto COM y devolver la descripción al cliente
    def enviarGCode(self, usuario, gcode):
        print(f"Usuario {usuario} ha enviado el código G-code: {gcode}")
        
        if gcode in self.gcode_dict:
            descripcion = self.gcode_dict[gcode]

            # Enviar el G-code por el puerto serial
            if self.serial_port:
                try:
                    self.serial_port.write(f"{gcode}\n".encode())  # Enviar el código al puerto serial
                    print(f"Código G-code {gcode} enviado al puerto COM3.")
                except Exception as e:
                    return f"Error al enviar el código G-code: {e}"

            # Devolver la descripción del código al cliente
            return f"Código G-code válido: {descripcion}"
        else:
            return "Código G-code no reconocido."

    # Método para obtener el listado de G-codes y sus descripciones
    def obtenerListadoGCodes(self):
        listado = []
        for codigo, descripcion in self.gcode_dict.items():
            listado.append(f"{codigo}: {descripcion}")
        return listado

    # Método para saludar al usuario
    def saludar(self, nombre):
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

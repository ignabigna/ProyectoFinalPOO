# servidor.py
import csv
import serial
from xmlrpc.server import SimpleXMLRPCServer
import time
import os
import json
from Logger import Logger  # Importa la clase Logger
from ErrorManager import ErrorManager  # Importa la clase ErrorManager


# Clase principal del servidor
class Servidor:
    def __init__(self):
        # Inicializar el logger y el gestor de errores
        self.logger = Logger(__name__)
        self.error_manager = ErrorManager(self.logger)

        # Inicialización de atributos
        self.usuarios = self.cargar_usuarios("usuarios.csv")
        self.gcode_dict = self.cargar_gcode_descripciones("codigos_gcode.csv")
        self.serial_port = None  # Inicialización sin conexión al puerto COM
        self.motores_activados = False  # Estado de los motores
        self.homing_realizado = False  # Indica si se ha realizado homing

        # Límites de operación
        self.limites_espacio_trabajo = {
            'Z': (-88, 150),  # Valores de Z_MIN y Z_MAX según config.h
            'R': (100, 240)   # Valores de R_MIN y R_MAX calculados según config.h
        }
        self.velocidad_maxima = 100  # Velocidad máxima en mm/s
    
    def validar_comando_gcode(self, gcode):
        """
        Valida si un comando G-Code es viable en función de los parámetros geométricos y cinemáticos del robot.
        """
        if gcode.startswith("G1"):
            partes = gcode.split()
            coordenadas = {}
            for parte in partes[1:]:
                if parte[0] in 'XYZRE':
                    valor = float(parte[1:])
                    if parte[0] in self.limites_espacio_trabajo:
                        limite_min, limite_max = self.limites_espacio_trabajo[parte[0]]
                        if not (limite_min <= valor <= limite_max):
                            return False, f"ERROR: {parte[0]}={valor} fuera del rango permitido ({limite_min}, {limite_max})"
            return True, "Comando G1 válido"
        return True, "Comando válido"
    
    def cargar_usuarios(self, archivo_csv):
        usuarios = {}
        try:
            with open(archivo_csv, mode="r") as file:
                reader = csv.reader(file)
                for row in reader:
                    if len(row) >= 2:
                        usuarios[row[0]] = row[1]  # usuario: contraseña
            self.logger.info("Usuarios cargados correctamente.")
        except FileNotFoundError as e:
            raise FileNotFoundError(self.error_manager.handle_error(e, f"El archivo {archivo_csv} no se encontró"))
        except Exception as e:
            raise Exception(self.error_manager.handle_error(e, "Error al cargar el archivo de usuarios"))
        return usuarios

    # Registrar un nuevo usuario en el archivo CSV
    def registrar_usuario(self, usuario, contraseña):
        if usuario in self.usuarios:
            self.error_manager.handle_warning(f"Intento de registrar un usuario ya existente: {usuario}")
            return "El usuario ya existe."
        else:
            self.usuarios[usuario] = contraseña
            try:
                with open("usuarios.csv", mode="a", newline="") as file:
                    writer = csv.writer(file)
                    writer.writerow([usuario, contraseña])
                self.logger.info(f"Usuario {usuario} registrado correctamente.")
                return "Usuario registrado correctamente."
            except Exception as e:
                return self.error_manager.handle_error(e, "Error al registrar el usuario")

    # Validar credenciales de usuario
    def autenticar_usuario(self, usuario, contraseña):
        if usuario in self.usuarios and self.usuarios[usuario] == contraseña:
            self.logger.info(f"Usuario {usuario} autenticado correctamente.")
            return True
        else:
            self.error_manager.handle_warning(f"Autenticación fallida para el usuario {usuario}.")
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
            self.logger.info("Códigos G-code cargados correctamente.")
        except Exception as e:
            self.error_manager.handle_error(e, "Error al cargar el archivo de códigos G-code")
        return gcode_dict

    # Inicializar el puerto serial con el baud rate correcto
    def iniciar_puerto_serial(self, puerto):
        try:
            ser = serial.Serial(puerto, 115200, timeout=1)  # Configurar el puerto COM3 con baud rate 115200
            self.logger.info(f"Puerto {puerto} abierto correctamente.")
            return ser
        except serial.SerialException as e:
            return self.error_manager.handle_error(e, f"Error al abrir el puerto serial en {puerto}")

        
    # Método para conectar al robot cuando lo solicite el cliente
    def conectar_robot(self):
        if self.serial_port is None:
            try:
                self.serial_port = serial.Serial("COM3", 115200, timeout=1)
                self.logger.info("Puerto COM5 abierto correctamente.")
                inicializacion = []
                
                for _ in range(2):
                    linea = self.serial_port.readline().decode("ISO-8859-1").strip()
                    while not linea:
                        linea = self.serial_port.readline().decode("ISO-8859-1").strip()
                    inicializacion.append(linea)
                    self.logger.info(f"Línea recibida del Arduino: {linea}")
                
                return inicializacion
            except serial.SerialException as e:
                raise serial.SerialException(self.error_manager.handle_error(e, "Error al abrir el puerto serial"))
        else:
            self.error_manager.handle_warning("Intento de conectar al robot cuando ya estaba conectado.")
            return ["El robot ya está conectado."]

    def desconectar_robot(self):
        if self.serial_port is not None:
            try:
                self.serial_port.close()
                self.serial_port = None
                self.logger.info("El robot se ha desconectado correctamente.")
                return "El robot se ha desconectado correctamente."
            except Exception as e:
                return self.error_manager.handle_error(e, "Error al desconectar el robot")
        else:
            return self.error_manager.handle_warning("El robot ya está desconectado.")
        
    def activar_motores(self):
        try:
            if self.serial_port is None:
                raise ConnectionError("El robot no está conectado.")
            
            self.serial_port.write("M17\r\n".encode())
            self.logger.info("Comando M17 enviado: Motores activados.")
            self.motores_activados = True
            self.homing_realizado = False  # Resetear el estado de homing al activar motores
            return "Motores activados correctamente."
        except Exception as e:
            return self.error_manager.handle_error(e, "Error al activar los motores")

        
    def desactivar_motores(self):
        try:
            if self.serial_port is None:
                raise ConnectionError("El robot no está conectado.")
            
            self.serial_port.write("M18\r\n".encode())
            self.logger.info("Comando M18 enviado: Motores desactivados.")
            self.motores_activados = False
            return "Motores desactivados correctamente."
        except Exception as e:
            return self.error_manager.handle_error(e, "Error al desactivar los motores")


    def enviarGCode(self, usuario, gcode):
        try:
            if self.serial_port is None:
                raise ConnectionError("El robot no está conectado. Conéctese primero antes de enviar comandos G-Code.")
            
            if not self.motores_activados:
                raise RuntimeError("Los motores deben estar activados antes de enviar cualquier comando.")
            
            # Verificar si se ha realizado el homing antes de otros comandos
            if gcode != "G28" and not self.homing_realizado:
                raise RuntimeError("El homing (G28) debe realizarse como la primera acción después de activar los motores.")
            
            # Validar el formato del G-code
            valido, mensaje = self.validar_comando_gcode(gcode)
            if not valido:
                return mensaje

            self.logger.info(f"Usuario {usuario} ha enviado el código G-code: {gcode}")

            # Verificar si el comando G1 es válido sin importar los parámetros específicos
            comando_base = gcode.split()[0]
            if comando_base in self.gcode_dict:
                if comando_base == "G28":
                    self.homing_realizado = True  # Actualizar la variable al enviar el G28
                    descripcion = "Homing completado"  # Descripción opcional
                elif comando_base == "G1":
                    descripcion = "Movimiento lineal"
                else:
                    descripcion = self.gcode_dict[comando_base]

                self.serial_port.write(f"{gcode}\r\n".encode())
                self.logger.info(f"Código G-code {gcode} enviado al puerto COM3.")
                
                time.sleep(2)  # Ajustar el tiempo de espera si es necesario

                # Leer la respuesta completa del Arduino
                respuesta_arduino = self.leer_respuesta_arduino_completa()
                return f"{descripcion} - Respuesta del Arduino:\n{respuesta_arduino}"

            return self.error_manager.handle_warning("Código G-code no reconocido.")
        
        except (ConnectionError, TimeoutError, RuntimeError) as e:
            return self.error_manager.handle_error(e)
        except Exception as e:
            return self.error_manager.handle_error(e, "Error al enviar el código G-code")



        
    def leer_respuesta_arduino_completa(self):
        """
        Lee todas las líneas disponibles del Arduino en el puerto serial hasta que no haya más respuestas.
        """
        respuesta_completa = []
        try:
            # Leer todas las líneas disponibles en el buffer de la terminal serial
            while self.serial_port.in_waiting > 0:
                linea = self.serial_port.readline().decode("ISO-8859-1").strip()
                if linea:
                    respuesta_completa.append(linea)
                    self.logger.info(f"Línea recibida del Arduino: {linea}")
            
            # Retornar todas las líneas como una lista unida por saltos de línea
            return "\n".join(respuesta_completa) if respuesta_completa else "Sin respuesta del Arduino."
        except Exception as e:
            self.logger.error(f"Error al leer la respuesta del Arduino: {e}")
            return f"Error al leer la respuesta del Arduino: {e}"


    # Método para obtener el listado de G-codes y sus descripciones
    def obtenerListadoGCodes(self):
        listado = []
        for codigo, descripcion in self.gcode_dict.items():
            listado.append(f"{codigo}: {descripcion}")
        return listado

    # Método para saludar al usuario
    def saludar(self, nombre):
        self.logger.info(f"Solicitud de saludo recibida de: {nombre}")
        return f"Hola, {nombre}! Bienvenido al servidor."
    
    def iniciar_rpc(self):
        if not self.conectado_rpc:
            self.conectado_rpc = True
            self.logger.info("Servidor RPC iniciado.")
            return "Servidor RPC iniciado."
        else:
            self.logger.warning("El servidor RPC ya está activo.")
            return "El servidor RPC ya está activo."

    def apagar_rpc(self):
        if self.conectado_rpc:
            self.conectado_rpc = False
            self.logger.info("Servidor RPC apagado.")
            return "Servidor RPC apagado."
        else:
            self.logger.warning("El servidor RPC ya está inactivo.")
            return "El servidor RPC ya está inactivo."

    def mostrar_logs(self, num_lineas=100):
        log_path = 'LogsServer'
        logs = []
        try:
            archivos_log = sorted(
                [os.path.join(log_path, f) for f in os.listdir(log_path) if f.endswith('.log')],
                key=os.path.getmtime,
                reverse=True
            )
            if archivos_log:
                with open(archivos_log[0], 'r') as log_file:
                    logs = log_file.readlines()[-num_lineas:]
                return logs
            else:
                return ["No hay archivos de logs disponibles."]
        except Exception as e:
            return [f"Error al mostrar los logs: {str(e)}"]

    def mostrar_estado(self):
        estado = {
            "Estado de conexión": "Conectado" if self.serial_port else "Desconectado",
            "Motores activados": self.motores_activados,
            "Homing realizado": self.homing_realizado,
            "Servidor RPC": "Activo" if self.conectado_rpc else "Inactivo"
        }
        self.logger.info("Estado actual del servidor mostrado.")
        return json.dumps(estado, indent=4)

    def mostrar_ayuda_comando(self, comando):
        if comando in self.gcode_dict:
            ejemplo = f"Ejemplo de uso: {comando} X100 Y100 Z0 F50"
            ayuda = f"{comando}: {self.gcode_dict[comando]}\n{ejemplo}"
            self.logger.info(f"Se mostró la ayuda para el comando {comando}.")
            return ayuda
        else:
            self.logger.warning(f"Comando {comando} no reconocido al solicitar ayuda.")
            return "Comando no reconocido. Verifique el comando ingresado."

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

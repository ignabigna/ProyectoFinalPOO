# admin.py
import serial
import time
import os
import csv
import threading
from Logger import Logger  # Asegúrate de que Logger esté accesible también en modo local
from ErrorManager import ErrorManager

class Admin:
    def __init__(self):
        # Inicializar el logger y el gestor de errores
        self.logger = Logger(__name__)
        self.error_manager = ErrorManager(self.logger)
        
        # Conectar al puerto serial
        self.serial_port = None
        self.robot_conectado = False
        self.conectar_arduino()

    def conectar_arduino(self):
        """Conectar al Arduino en modo local."""
        try:
            self.serial_port = serial.Serial("COM5", 115200, timeout=1)
            self.logger.info("Conectado a Arduino en el puerto COM5.")
            
            # Leer las primeras líneas de inicialización del Arduino
            for _ in range(2):
                linea = self.serial_port.readline().decode("ISO-8859-1").strip()
                if linea:
                    self.logger.info(f"Inicialización del Arduino: {linea}")
            self.robot_conectado = True
        except serial.SerialException as e:
            self.error_manager.handle_error(e, "Error al conectar al Arduino en modo local")

    def desconectar_arduino(self):
        """Desconectar del Arduino."""
        if self.serial_port is not None:
            self.serial_port.close()
            self.serial_port = None
            self.logger.info("Desconectado del Arduino.")
            self.robot_conectado = False
        else:
            self.logger.warning("El Arduino ya está desconectado.")

    def activar_motores(self):
        """Activar los motores en modo local."""
        self.enviar_comando("M17", "Motores activados")

    def desactivar_motores(self):
        """Desactivar los motores en modo local."""
        self.enviar_comando("M18", "Motores desactivados")

    def enviar_comando(self, gcode, descripcion=None):
        """
        Envía un comando G-code al Arduino y muestra la respuesta.
        """
        if self.serial_port is None or not self.robot_conectado:
            print("Error: El Arduino no está conectado.")
            return
        
        try:
            self.serial_port.write(f"{gcode}\r\n".encode())
            self.logger.info(f"Enviando comando: {gcode}")
            time.sleep(2)  # Ajustar si es necesario

            # Leer y mostrar la respuesta del Arduino
            respuesta = self.leer_respuesta_completa()
            resultado = f"{descripcion} - Respuesta del Arduino:\n{respuesta}" if descripcion else respuesta
            print(resultado)
            return resultado
        except Exception as e:
            return self.error_manager.handle_error(e, f"Error al enviar el comando {gcode}")

    def leer_respuesta_completa(self):
        """
        Lee todas las líneas disponibles en la respuesta del Arduino.
        """
        respuesta_completa = []
        try:
            while self.serial_port.in_waiting > 0:
                linea = self.serial_port.readline().decode("ISO-8859-1").strip()
                if linea:
                    respuesta_completa.append(linea)
                    self.logger.info(f"Respuesta del Arduino: {linea}")
            return "\n".join(respuesta_completa) if respuesta_completa else "Sin respuesta del Arduino."
        except Exception as e:
            self.logger.error(f"Error al leer la respuesta del Arduino: {e}")
            return f"Error al leer la respuesta del Arduino: {e}"

    def enviar_gcode_personalizado(self):
        """
        Permite al usuario ingresar un comando G-code manualmente para enviarlo al Arduino.
        """
        gcode = input("Ingrese el comando G-code: ")
        descripcion = f"Comando personalizado: {gcode}"
        self.enviar_comando(gcode, descripcion)

    def modo_automatico(self):
        """Iniciar el modo automático para ejecutar comandos G-code desde archivos en una carpeta."""
        carpeta = "M_Automatico"
        if not os.path.exists(carpeta):
            print(f"La carpeta '{carpeta}' no existe. Crear la carpeta y colocar archivos CSV con comandos G-code.")
            return
        
        for archivo in os.listdir(carpeta):
            if archivo.endswith(".csv"):
                print(f"Leyendo archivo: {archivo}")
                with open(os.path.join(carpeta, archivo), 'r') as file:
                    reader = csv.reader(file)
                    for linea in reader:
                        if not self.robot_conectado:
                            print("Error: El robot debe estar conectado antes de iniciar el modo automático.")
                            return
                        gcode = linea[0]  # Supone que cada línea contiene un solo comando G-code
                        print(f"Enviando comando GCode: {gcode}")
                        self.enviar_comando(gcode, "Modo automático")
                        time.sleep(5)  # Pausa entre comandos

    def mostrar_log(self):
        """Mostrar las últimas 100 líneas del archivo de log."""
        log_path = "LogsServer/servidor_2024-11-05_23-59-23.log"  # Reemplaza con la ruta de tu archivo de log

        try:
            with open(log_path, 'r') as file:
                # Leer todas las líneas y seleccionar las últimas 100
                lineas = file.readlines()
                ultimas_lineas = lineas[-100:]
                print("\nÚltimas 100 líneas del log de trabajo:\n")
                for linea in ultimas_lineas:
                    print(linea.strip())
        except FileNotFoundError:
            print(f"Error: No se encontró el archivo de log en '{log_path}'.")
        except Exception as e:
            print(f"Error al leer el archivo de log: {e}")

    def mostrar_ayuda_comandos(self):
        """Mostrar el listado de GCodes y sus descripciones desde el archivo CSV."""
        gcode_csv = "codigos_gcode.csv"  # Reemplaza con la ruta de tu archivo CSV

        try:
            with open(gcode_csv, 'r') as file:
                reader = csv.reader(file)
                print("\nListado de comandos GCode y sus descripciones:\n")
                for row in reader:
                    if len(row) >= 2:
                        codigo, descripcion = row
                        print(f"{codigo}: {descripcion}")
        except FileNotFoundError:
            print(f"Error: No se encontró el archivo CSV en '{gcode_csv}'.")
        except Exception as e:
            print(f"Error al leer el archivo CSV: {e}")

    def ejecutar_modo_local(self):
        """
        Muestra un menú para ejecutar comandos locales en el Arduino.
        """
        opciones = {
            "1": ("Conectar al robot", self.conectar_arduino),
            "2": ("Desconectar el robot", self.desconectar_arduino),
            "3": ("Activar motores", self.activar_motores),
            "4": ("Desactivar motores", self.desactivar_motores),
            "5": ("Enviar comando GCode personalizado", self.enviar_gcode_personalizado),
            "6": ("Iniciar modo automático", self.modo_automatico),
            "7": ("Mostrar últimas 100 líneas del log", self.mostrar_log),
            "8": ("Mostrar ayuda de comandos", self.mostrar_ayuda_comandos),
            "9": ("Salir", self.desconectar_arduino)
        }
        
        while True:
            print("\nSeleccione una opción:")
            for key, (desc, _) in opciones.items():
                print(f"{key}. {desc}")
            
            opcion = input("Ingrese su opción: ")
            if opcion in opciones:
                _, accion = opciones[opcion]
                accion()
                if opcion == "9":  # Salir
                    break
            else:
                print("Opción no válida. Intente de nuevo.")

# Logger.py
import logging
import os
from datetime import datetime

class Logger:
    def __init__(self, nombre_logger, nivel=logging.INFO):
        # Crear la carpeta "LogsServer" si no existe
        log_dir = 'LogsServer'
        if not os.path.exists(log_dir):
            os.makedirs(log_dir)
        
        # Generar un nombre de archivo único con la fecha y hora actuales
        fecha_hora = datetime.now().strftime('%Y-%m-%d_%H-%M-%S')
        archivo_log = os.path.join(log_dir, f'servidor_{fecha_hora}.log')
        
        # Configuración del logger
        self.logger = logging.getLogger(nombre_logger)
        self.logger.setLevel(nivel)
        
        # Configuración para archivo de logs
        file_handler = logging.FileHandler(archivo_log)
        file_formatter = logging.Formatter('%(asctime)s - %(levelname)s - %(message)s')
        file_handler.setFormatter(file_formatter)
        self.logger.addHandler(file_handler)
        
        # Configuración para la consola
        console_handler = logging.StreamHandler()
        console_formatter = logging.Formatter('%(message)s')  # Solo muestra el mensaje en la consola
        console_handler.setFormatter(console_formatter)
        self.logger.addHandler(console_handler)

    # Métodos para registrar mensajes de distintos niveles
    def info(self, mensaje):
        self.logger.info(mensaje)

    def warning(self, mensaje):
        self.logger.warning(mensaje)

    def error(self, mensaje):
        self.logger.error(mensaje)

    def debug(self, mensaje):
        self.logger.debug(mensaje)

# Logger.py
import logging

class Logger:
    def __init__(self, nombre_logger, archivo_log='servidor.log', nivel=logging.INFO):
        self.logger = logging.getLogger(nombre_logger)
        self.logger.setLevel(nivel)
        
        # Configuración para archivo de logs
        file_handler = logging.FileHandler(archivo_log)
        file_formatter = logging.Formatter('%(asctime)s - %(message)s')
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

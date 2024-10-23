import csv
import serial
from xmlrpc.server import SimpleXMLRPCServer
import os
import sys

class Servidor:
    def __init__(self):
        self.gcode_dict = self.cargar_gcode_descripciones('codigos_gcode.csv')
        self.serial_port = None

    # Cargar los códigos G-code y sus descripciones desde el archivo CSV
    def cargar_gcode_descripciones(self, archivo_csv):
        gcode_dict = {}
        try:
            with open(archivo_csv, mode='r') as file:
                reader = csv.reader(file)
                next(reader)  # Saltar el encabezado
                for row in reader:
                    gcode = row[0].strip()
                    descripcion = row[1].strip()
                    gcode_dict[gcode] = descripcion
        except FileNotFoundError:
            print(f"Error: No se encontró el archivo {archivo_csv}.")
        return gcode_dict

    # Manejar el comando G-code y enviar la descripción al cliente
    def enviarGCode(self, usuario, gcode):
        if gcode in self.gcode_dict:
            descripcion = self.gcode_dict[gcode]
            if self.serial_port is not None and self.serial_port.is_open:
                self.serial_port.write(gcode.encode())  # Enviar al puerto serial
                print(f"Comando {gcode} enviado al robot.")
            return f"Acción ejecutada para el código {gcode}: {descripcion}"
        else:
            return f"Código G-code {gcode} no reconocido."

    # Listar los G-codes disponibles
    def obtenerListadoGCodes(self):
        return list(self.gcode_dict.keys())

    # Conectar/Desconectar el puerto COM3
    def gestionarConexionRobot(self, activar):
        if activar:
            if self.serial_port is None or not self.serial_port.is_open:
                try:
                    self.serial_port = serial.Serial('COM3', 9600, timeout=1)
                    return "Robot activado. Conexión abierta en COM3."
                except serial.SerialException as e:
                    return f"Error al abrir el puerto COM3: {e}"
            else:
                return "Robot ya está activado."
        else:
            if self.serial_port is not None and self.serial_port.is_open:
                self.serial_port.close()
                return "Robot desactivado. Conexión cerrada en COM3."
            else:
                return "Robot ya está desactivado."

    # Apagar el servidor
    def apagarServidor(self):
        print("Servidor apagándose...")
        os._exit(0)  # Apagar el servidor inmediatamente

# Iniciar el servidor
def iniciar_servidor():
    servidor = Servidor()
    server = SimpleXMLRPCServer(("localhost", 8000))
    print("Servidor escuchando en el puerto 8000...")

    # Registrar las funciones del servidor
    server.register_instance(servidor)

    # Ejecutar el servidor
    try:
        server.serve_forever()
    except KeyboardInterrupt:
        print("Servidor detenido.")

if __name__ == "__main__":
    iniciar_servidor()

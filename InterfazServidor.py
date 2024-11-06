from servidor import Servidor
from Logger import Logger

class InterfazServidor:
    def __init__(self):
        self.servidor = Servidor()
        self.logger = Logger("InterfazServidor")
        self.usuario_actual = None

    def autenticar_administrador(self):
        usuario = input("Ingrese el usuario: ")
        contraseña = input("Ingrese la contraseña: ")
        if self.servidor.autenticar_usuario(usuario, contraseña):
            if usuario == "admin" and contraseña == "admin123":  # Verifica que el usuario tenga privilegios de administrador
                self.usuario_actual = usuario
                return True
            else:
                return False
        else:
            return False

    def mostrar_menu(self):
        print("\n--- ***Opciones disponibles*** ---")
        print("1. Solicitar listado de GCodes")
        print("2. Ingresar comando GCode")
        print("3. Conectar al robot")
        print("4. Desconectar el robot")
        print("5. Activar motores")
        print("6. Desactivar motores")
        print("7. Iniciar modo automático")
        print("8. [Administrador] Mostrar parámetros de conexión")
        print("9. [Administrador] Editar parámetros de conexión")
        print("10. [Administrador] Encender servidor RPC")
        print("11. [Administrador] Apagar servidor RPC")
        print("12. [Administrador] Mostrar últimas 100 líneas del log")
        opcion = input("Seleccione una opción: ")
        return opcion

    def ejecutar(self):
        while True:
            opcion = self.mostrar_menu()
            if opcion == "1":
                print("Solicitando listado de GCodes...")
                listado = self.servidor.obtenerListadoGCodes()
                for item in listado:
                    print(item)
            elif opcion == "2":
                gcode = input("Ingrese un comando GCode: ")
                if gcode == "G1":
                    try:
                        # Solicitar valores específicos para el comando G1
                        x = float(input("Ingrese el valor de X: "))
                        y = float(input("Ingrese el valor de Y: "))
                        z = float(input("Ingrese el valor de Z: "))
                        velocidad = float(input("Ingrese la velocidad (E): "))

                        # Formatear la cadena G-Code con dos decimales
                        gcode_formateado = f"G1 X{x:.2f} Y{y:.2f} Z{z:.2f} E{velocidad:.2f}"

                        # Enviar el comando formateado al servidor
                        print("Enviando comando GCode al servidor...")
                        respuesta = self.servidor.enviarGCode(self.usuario_actual, gcode_formateado)
                    except ValueError:
                        print("Error: Por favor, ingrese valores numéricos válidos.")
                else:
                    # Enviar otros comandos directamente al servidor
                    respuesta = self.servidor.enviarGCode(self.usuario_actual, gcode)
                    print("Respuesta del servidor:", respuesta)
                self.servidor.enviarGCode(self.usuario_actual, gcode)
            elif opcion == "3":
                print("Conectando al robot...")
                self.servidor.conectar_robot()
            elif opcion == "4":
                print("Desconectando el robot...")
                self.servidor.desconectar_robot()
            elif opcion == "5":
                print("Activando motores...")
                self.servidor.activar_motores()
            elif opcion == "6":
                print("Desactivando motores...")
                self.servidor.desactivar_motores()
            elif opcion == "7":
                print("Iniciando modo automático...")
                respuesta = self.servidor.modo_automatico()
            elif opcion == "8":
                print("Mostrando estado de conexión actual...")
                estado = self.servidor.mostrar_estado()
                print(estado)
            elif opcion == "9":
                print("Mostrando/actualizando parámetros de conexión...")
                self.servidor.mostrar_editar_parametros_conexion()
            elif opcion == "10":
                print("Encendiendo servidor RPC...")
                respuesta = self.servidor.iniciar_rpc()
            elif opcion == "11":
                print("Apagando servidor RPC...")
                respuesta = self.servidor.apagar_rpc()
            elif opcion == "12":
                print("Mostrando últimas 100 líneas del log...")
                logs = self.servidor.mostrar_logs()  # Llama al método en `servidor.py`
                for linea in logs:
                    print(linea.strip())
            else:
                print("Opción no válida. Intente nuevamente.")

    # Métodos de ejemplo para algunas opciones
    

# Para probar la clase
if __name__ == "__main__":
    interfaz = InterfazServidor()
    interfaz.ejecutar()

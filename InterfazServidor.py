# interfaz_servidor.py
from servidor import Servidor
from Logger import Logger

class InterfazServidor:
    def __init__(self):
        self.servidor = Servidor()
        self.logger = Logger("InterfazServidor")
        self.usuario_actual = None

    def mostrar_menu(self):
        print("\n--- Menú de Control del Servidor ---")
        print("1. Conectar al robot")
        print("2. Desconectar el robot")
        print("3. Activar motores")
        print("4. Desactivar motores")
        print("5. Listar comandos disponibles")
        print("6. Mostrar ayuda de comandos")
        print("7. Reporte general de la conexión y estado")
        print("8. Reporte del log de trabajo")
        print("9. Selección de modo de trabajo (manual/automático, absoluto/relativo)")
        print("10. [Administrador] Mostrar/editar parámetros de conexión")
        print("11. [Administrador] Encender/apagar servidor RPC")
        print("12. [Administrador] Mostrar últimas 100 líneas del log")
        print("13. Salir")
        opcion = input("Seleccione una opción: ")
        return opcion

    def autenticar_administrador(self):
        usuario = input("Ingrese el usuario: ")
        contraseña = input("Ingrese la contraseña: ")
        if self.servidor.autenticar_usuario(usuario, contraseña):
            if usuario == "admin":  # Reemplazar "admin" con el usuario administrador específico
                self.usuario_actual = usuario
                self.logger.info(f"Acceso concedido al administrador {usuario}")
                return True
            else:
                print("Acceso denegado. Solo el administrador puede ingresar.")
                self.logger.warning(f"Intento fallido de acceso como administrador por {usuario}")
                return False
        else:
            print("Usuario o contraseña incorrectos.")
            return False

    def ejecutar(self):
        if not self.autenticar_administrador():
            return

        while True:
            opcion = self.mostrar_menu()
            if opcion == "1":
                self.conectar_robot()
            elif opcion == "2":
                self.desconectar_robot()
            elif opcion == "3":
                self.activar_motores()
            elif opcion == "4":
                self.desactivar_motores()
            elif opcion == "5":
                self.listar_comandos_disponibles()
            elif opcion == "6":
                self.mostrar_ayuda_comandos()
            elif opcion == "7":
                self.reporte_general()
            elif opcion == "8":
                self.reporte_log_trabajo()
            elif opcion == "9":
                self.seleccionar_modo_trabajo()
            elif opcion == "10":
                self.mostrar_editar_parametros()
            elif opcion == "11":
                self.encender_apagar_rpc()
            elif opcion == "12":
                self.mostrar_ultimas_lineas_log()
            elif opcion == "13":
                print("Saliendo del programa...")
                break
            else:
                print("Opción no válida. Intente nuevamente.")

    # Métodos de ejemplo para algunas opciones
    def conectar_robot(self):
        print("Conectando al robot...")
        respuesta = self.servidor.conectar_robot()
        print("\n".join(respuesta))

    def desconectar_robot(self):
        print("Desconectando el robot...")
        respuesta = self.servidor.desconectar_robot()
        print(respuesta)

    def activar_motores(self):
        print("Activando motores...")
        respuesta = self.servidor.activar_motores()
        print(respuesta)

    def desactivar_motores(self):
        print("Desactivando motores...")
        respuesta = self.servidor.desactivar_motores()
        print(respuesta)

    # Métodos adicionales se implementarán más adelante...

# Para probar la clase
if __name__ == "__main__":
    interfaz = InterfazServidor()
    interfaz.ejecutar()

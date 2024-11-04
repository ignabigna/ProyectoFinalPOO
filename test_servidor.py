import unittest
from servidor import Servidor

class TestServidor(unittest.TestCase):
    def setUp(self):
        # Inicializar una instancia de Servidor
        self.servidor = Servidor()
        self.servidor.serial_port = True  # Simulación: el puerto serial está conectado

    def test_enviar_comando_sin_motores_activados(self):
        resultado = self.servidor.enviarGCode("usuario_test", "G1 X100")
        self.assertIn("Los motores deben estar activados", resultado)

    def test_enviar_comando_sin_homing(self):
        # Simular que los motores están activados pero no se ha hecho homing
        self.servidor.motores_activados = True
        resultado = self.servidor.enviarGCode("usuario_test", "G1 X100")
        self.assertIn("El homing (G28) debe realizarse", resultado)

    def test_enviar_comando_con_motores_y_homing(self):
        # Simular que los motores están activados y se ha hecho homing
        self.servidor.motores_activados = True
        self.servidor.homing_realizado = True
        resultado = self.servidor.enviarGCode("usuario_test", "G1 X100")
        self.assertNotIn("Error", resultado)  # Verificar que no se devuelve un error

    # Nuevas pruebas de validación de rangos
    def test_enviar_comando_fuera_de_rango_z(self):
        self.servidor.motores_activados = True
        self.servidor.homing_realizado = True
        resultado = self.servidor.enviarGCode("usuario_test", "G1 Z200")  # Z fuera del rango permitido
        self.assertIn("ERROR: Z=200 fuera del rango permitido", resultado)

    def test_enviar_comando_fuera_de_rango_r(self):
        self.servidor.motores_activados = True
        self.servidor.homing_realizado = True
        resultado = self.servidor.enviarGCode("usuario_test", "G1 R50")  # R fuera del rango permitido
        self.assertIn("ERROR: R=50 fuera del rango permitido", resultado)

    def test_enviar_comando_dentro_de_rango(self):
        self.servidor.motores_activados = True
        self.servidor.homing_realizado = True
        resultado = self.servidor.enviarGCode("usuario_test", "G1 Z100")  # Z dentro del rango permitido
        self.assertNotIn("Error", resultado)  # Verificar que el comando se ejecute correctamente

if __name__ == '__main__':
    unittest.main()

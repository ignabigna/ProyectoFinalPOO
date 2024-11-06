import unittest
from unittest.mock import MagicMock
from servidor import Servidor
import json

class TestServidor(unittest.TestCase):
    def setUp(self):
        # Inicializar una instancia de Servidor
        self.servidor = Servidor()
        
        # Simulación: el puerto serial se define como un objeto mock con métodos simulados
        self.servidor.serial_port = MagicMock()
        self.servidor.serial_port.write = MagicMock()
        self.servidor.serial_port.readline = MagicMock(side_effect=[
            b'INFO: ROBOT ONLINE\n',
            b'INFO: SEND G28 TO CALIBRATE\n'
        ])
        
    def test_enviar_comando_sin_motores_activados(self):
        resultado = self.servidor.enviarGCode("usuario_test", "G1 X100")
        self.assertIn("Los motores deben estar activados", resultado)

    def test_enviar_comando_sin_homing(self):
        self.servidor.motores_activados = True
        resultado = self.servidor.enviarGCode("usuario_test", "G1 X100")
        self.assertIn("El homing (G28) debe realizarse", resultado)


    def test_enviar_comando_fuera_de_rango_z(self):
        self.servidor.motores_activados = True
        self.servidor.homing_realizado = True
        resultado = self.servidor.enviarGCode("usuario_test", "G1 Z200")
        self.assertIn("ERROR: Z=200.0 fuera del rango permitido (-88, 150)", resultado)

    def test_enviar_comando_fuera_de_rango_r(self):
        self.servidor.motores_activados = True
        self.servidor.homing_realizado = True
        resultado = self.servidor.enviarGCode("usuario_test", "G1 R50")
        self.assertIn("ERROR: R=50.0 fuera del rango permitido (100, 240)", resultado)


    def test_activar_motores(self):
        resultado = self.servidor.activar_motores()
        self.servidor.serial_port.write.assert_called_once()
        self.assertIn("Motores activados correctamente.", resultado)

    def test_desconectar_robot(self):
        self.servidor.serial_port = MagicMock()  # Simula el puerto serial
        resultado = self.servidor.desconectar_robot()
        self.assertIn("El robot se ha desconectado correctamente.", resultado)

    def test_desconectar_robot_no_conectado(self):
        self.servidor.serial_port = None  # Simula un estado donde el robot no está conectado
        resultado = self.servidor.desconectar_robot()
        self.assertIn("El robot ya está desconectado.", resultado)

    def test_mostrar_estado(self):
        estado_json = self.servidor.mostrar_estado()
        estado = json.loads(estado_json)  # Carga el JSON para verificar
        self.assertIn("Estado de conexión", estado)
        self.assertIn("Motores activados", estado)
        self.assertIn("Homing realizado", estado)
        self.assertIn("Servidor RPC", estado)


if __name__ == '__main__':
    unittest.main()

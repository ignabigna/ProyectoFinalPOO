# ErrorManager.py
class ErrorManager:
    def __init__(self, logger):
        self.logger = logger

    def handle_error(self, exception, custom_message=None):
        """Maneja un error específico y lo registra en los logs."""
        error_message = f"{custom_message}: {str(exception)}" if custom_message else str(exception)
        self.logger.error(error_message)
        return error_message

    def handle_warning(self, warning_message):
        """Maneja y registra una advertencia."""
        self.logger.warning(warning_message)
        return warning_message

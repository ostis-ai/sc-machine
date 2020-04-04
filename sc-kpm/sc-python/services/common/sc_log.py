from termcolor import colored, cprint

class Log:

  def __init__(self, subsystem_name):
    self.name = subsystem_name

  def __print_colored(self, level, level_color, message, message_color):
    print (colored('[' + self.name + '][' + level + ']', level_color) + colored(': ' + message, message_color))

  def debug(self, message):
    self.__print_colored('debug', 'grey', message, 'grey')

  def info(self, message):
    self.__print_colored('info', 'white', message, 'white')

  def error(self, message):
    self.__print_colored('error', 'red', message, 'red')

  def warning(self, message):
    self.__print_colored('warning', 'yellow', message, 'yellow')
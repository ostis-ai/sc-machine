class ScKeynodeException(Exception):
    def __init__(super, sys_idtf):
        super(ScKeynodeException, super).__init__("Can't find keynode: {}".format(sys_idtf))
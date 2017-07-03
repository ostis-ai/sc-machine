from common.sc_keynodes import ScKeynodes
from common.sc_exception import ScKeynodeException
import urllib.request
import time, sys, traceback

from scb import *

class ScModule:

    def __init__(self, context, keynodes=[], cpp_bridge=None):
        self.sc_context = context
        self.keynodes = ScKeynodes(self.sc_context)
        self.cpp = cpp_bridge

        for k in keynodes:
            addr = self.keynodes[k]
            if not addr:
                raise ScKeynodeException(k)
    
    # --- overloads ---
    def OnInitialize(self, data):
        """Should be overloaded in a child class.
        This function calls when child module should be initialized
        """
        pass

    def OnCppEvent(self, name, payload, onSuccess, onError):
        """Should be overloaded in a child class.
        ItCalls every time, when receive event from cpp_bridge.
        * `name` - event name;
        * `payload` - event payload data (depend on event)
        * `onSuccess` - function that should be called,
        if event processed whitout errors. Example: `onSuccess(data)`
        * `onError` - function that should be called, when there are any error
        during event processing. Examle: `onError(data)`
        """
        pass
    
    def OnCppUpdate(self):
        """Can be overloaded in a child class.
        Calls every tick, when trying to get event from cpp_bridge.abs
        You should use this function, when you need periodical update
        in your module
        """
        pass

    # --- common state functions ---
    def Initialize(self, data):
        self.OnInitialize(data)

    def Run(self):
        if self.cpp:
            # wait until it will be initialized
            self.cpp.Initialize()
            
            while (self.cpp.Exist()):
                self.OnCppUpdate()

                request = self.cpp.GetRequest()
                if (request.IsValid()):

                    def onSuccess(data):
                        self.MakeCppResultOk(request, data)

                    def onError(data):
                        self.MakeCppResultError(request, data)

                    try:
                        self.OnCppEvent(
                            request.GetName(),
                            request.GetData(),
                            onSuccess,
                            onError)
                    except:
                        msg = "Unexpected error: {}".format(sys.exc_info()[0])
                        print(msg)
                        traceback.print_exc(file=sys.stdout)
                        onError(msg)
                else:
                    time.sleep(0.1)
        else:
            self.Initialize(None)

    # Work with CPP bridge
    def MakeCppResult(self, req, status, data):
        req.MakeResponse(status, data)

    def MakeCppResultError(self, req, data):
        self.MakeCppResult(req, ResponseStatus.Error, data)

    def MakeCppResultOk(self, req, data):
        self.MakeCppResult(req, ResponseStatus.Ok, data)

    # Set of usefull functions
    @staticmethod
    def GetDataByUrl(url):
        try:
            with urllib.request.urlopen(url) as response:
                data = response.read()
        except:
            return None

        return data
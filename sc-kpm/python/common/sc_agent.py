from enum import Enum

from common.sc_keynodes import ScKeynodes
from common.sc_set import ScSet

from sc import *
from scb import *

class ScAgent:

    class Status(Enum):
        SC_RESULT_ERROR                 = 0 # unknown error
        SC_RESULT_OK                    = 1 # no any error
        SC_RESULT_ERROR_INVALID_PARAMS  = 2 # invalid function parameters error
        SC_RESULT_ERROR_INVALID_TYPE    = 3 # invalied type error
        SC_RESULT_ERROR_IO              = 4 # input/output error
        SC_RESULT_ERROR_INVALID_STATE   = 5 # invalid state of processed object
        SC_RESULT_ERROR_NOT_FOUND       = 6 # item not found
        SC_RESULT_ERROR_NO_WRITE_RIGHTS = 7 # no ritghs to change or delete object
        SC_RESULT_ERROR_NO_READ_RIGHTS  = 8 # no ritghs to read object
        SC_RESULT_NO                    = 9 # no any result
        SC_RESULT_UNKNOWN               = 10 # result unknown

    # keynodes
    kCmdInitiated = 'command_initiated'
    kCmdFinished = 'command_finished'
    kNrelResult = 'nrel_result'

    kResultNo = 'sc_result_no'
    kResultUnknown = 'sc_result_unknown'
    kResultError = 'sc_result_error'
    kResultErrorInvalidParams = 'sc_result_error_invalid_params'
    kResultErrorInvalidType = 'sc_result_error_invalid_type'
    kResultErrorIO = 'sc_result_error_io'
    kResultInvalidState = 'sc_result_invalid_state'
    kResultErrorNotFound = 'sc_result_error_not_found'
    kResultErrorNoWriteRights = 'sc_result_error_no_write_rights'
    kResultErrorNoReadRights = 'sc_result_error_no_read_rights'

    keynodes = None

    @staticmethod
    def InitGlobal(module):
        ScAgent.keynodes = ScKeynodes(module.ctx)

    def __init__(self, module):
        self.module = module
        self.evt = None

    def Register(self, addr, evt_type):
        """Register this agent to a specified event
        addr - ScAddr of sc-element to subscribe event
        evt_type - on of ScPythonEventType values (type of event)
        """
        assert self.evt == None
        self.evt = self.module.events.CreateEventInternal(addr, evt_type, self._run)

    def Unregister(self):
        self.module.events.DestroyEvent(self.evt)
        self.evt = None

    def RunImpl(self, evt):
        """Should be override and do agent logic.
        It should return one of ScAgent.Status values

        evt - ScEventParams instance
        """
        return ScAgent.Status.SC_RESULT_OK

    def CheckImpl(self, evt):
        """This function can be overrided to check any conditions before run.
        If this function returns True, then RunImpl should be called; 
        potherwise it woudn't be
        """
        return True

    # --- Internal usage methods ---
    def _run(self, evt):
        """Just for internal usage
        """
        if self.CheckImpl(evt):
            self.RunImpl(evt)


class ScAgentCommand(ScAgent):
    """This type of agents initiated with command_initiated set.
    It check if initiated command class is equal to specified one,
    then run it. You doesn't need to call register function for this
    type of agents
    """

    def __init__(self, module, cmd_class_addr):
        ScAgent.__init__(self, module)
        self.cmd_class = cmd_class_addr
        self.cmd_addr = ScAddr()
        self.result_set = None

        self.Register(
            ScAgent.keynodes[ScAgent.kCmdInitiated],
            ScPythonEventType.AddOutputEdge)

    def CheckImpl(self, evt):
        """Check if type of initiated command is equal to specified one
        """
        return self.module.ctx.HelperCheckEdge(
            self.cmd_class,
            evt.other_addr,
            ScType.EdgeAccessConstPosPerm)

    def RunImpl(self, evt):
        """Get all data that can be usefule and cache it
        """
        self.cmd_addr = evt.other_addr
        assert self.cmd_addr.IsValid()

        # create result structure
        templ = ScTemplate()
        templ.TripleWithRelation(
            self.cmd_addr,
            ScType.EdgeDCommonVar,
            ScType.NodeVarStruct >> '_result',
            ScType.EdgeAccessVarPosPerm,
            ScAgent.keynodes[ScAgent.kNrelResult])

        gen_res = self.module.ctx.HelperGenTemplate(templ, ScTemplateGenParams())
        assert gen_res.Size() > 0

        self.result_set = ScSet(self.module.ctx, gen_res['_result'])

        # run implementation of command
        result = self.DoCommand()

        return result

    def DoCommand(self):
        """Should be overrided.
        This method calls to run command implementation
        Should return value like RunImpl
        """
        return ScAgent.Status.SC_RESULT_NO

    def GetParam(self, index):
        """Return parameter of command by specified index.
        Index value starts from 1. This function trying to find
        sc-element in command structure with attribute `rrel_<index>`
        """
        templ = ScTemplate()
        templ.TripleWithRelation(
            self.cmd_addr,
            ScType.EdgeAccessVarPosPerm,
            ScType.Unknown >> '_el',
            ScType.EdgeAccessVarPosPerm,
            self.keynodes['rrel_{}'.format(index)])

        search_res = self.module.ctx.HelperSearchTemplate(templ)
        if search_res.Size() == 0:
            return ScAddr()

        return search_res[0]['_el']

    # --- internal functions ---
    def _kb_resolve_status_addr(self, status):
        if status == ScAgent.Status.SC_RESULT_ERROR:
            sc_result_ok
    

        return ScAddr()

    def _kb_generate_status(self, status):
        """Append result structure to a special set depending on a status
        """
        pass
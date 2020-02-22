from enum import Enum

from common.sc_keynodes import ScKeynodes
from common.sc_set import ScSet
from common.sc_event import ScEventParams

from sc import *
from scb import *


class ScAgent:

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

  def __init__(self, module):
    self.module = module
    self.evt = None
    self.keynodes = ScKeynodes(module.ctx)

  def Register(self, addr, evt_type):
    """Register this agent to a specified event
    addr - ScAddr of sc-element to subscribe event
    evt_type - on of ScPythonEventType values (type of event)
    """
    assert self.evt == None
    self.evt = self.module.events.CreateEventInternal(
        addr, evt_type, self._run)
    
    self.module.log.info(self.__class__.__name__ + ' registered')

  def Unregister(self):
    self.module.events.DestroyEvent(self.evt)
    self.evt = None

    self.module.log.info(self.__class__.__name__ + ' unregistered')

  def RunImpl(self, evt: ScEventParams) -> ScResult:
    """Should be override and do agent logic.
    It should return one of ScAgent.Status values

    evt - ScEventParams instance
    """
    return ScResult.Error

  def CheckImpl(self, evt: ScEventParams):
    """This function can be overrided to check any conditions before run.
    If this function returns True, then RunImpl should be called; 
    otherwise it woudn't be run
    """
    return True

  # --- Internal usage methods ---
  def _run(self, evt: ScEventParams):
    """Just for internal usage
    """
    if self.CheckImpl(evt):
      self.module.log.info(self.__class__.__name__ + ' emited')
      result = self.RunImpl(evt)
      if result != ScResult.Ok:
        self.module.log.warning(self.__class__.__name__ + ' finished with error')
      else:
        self.module.log.info(self.__class__.__name__ + ' finished')


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
        self.keynodes[ScAgent.kCmdInitiated],
        ScPythonEventType.AddOutputEdge)

  def CheckImpl(self, evt):
    """Check if type of initiated command is equal to specified one
    """
    return self.module.ctx.HelperCheckEdge(
        self.cmd_class,
        evt.other_addr,
        ScType.EdgeAccessConstPosPerm)

  def RunImpl(self, evt):
    self.cmd_addr = evt.other_addr
    assert self.cmd_addr.IsValid()

    # change state to a progress
    progress_edge = evt.edge_addr

    def change_progress(state: ScAddr):
      self.module.ctx.DeleteElement(progress_edge)
      self.module.ctx.CreateEdge(ScType.EdgeAccessConstPosPerm, state, self.cmd_addr)

    change_progress(ScKeynodes.kCommandProgressdAddr())

    # create result structure
    templ = ScTemplate()
    templ.TripleWithRelation(
        self.cmd_addr,
        ScType.EdgeDCommonVar,
        ScType.NodeVarStruct >> '_result',
        ScType.EdgeAccessVarPosPerm,
        self.keynodes[ScAgent.kNrelResult])

    gen_res = self.module.ctx.HelperGenTemplate(templ, ScTemplateGenParams())
    assert gen_res.Size() > 0

    res_addr = gen_res['_result']
    self.result_set = ScSet(self.module.ctx, res_addr)

    # run implementation of command
    result = self.DoCommand()

    # generate result type
    self.module.ctx.CreateEdge(ScType.EdgeAccessConstPosPerm, ScKeynodes.GetResultCodeAddr(result), res_addr)
    change_progress(ScKeynodes.kCommandFinishedAddr())

    return result

  def DoCommand(self) -> ScResult:
    """Should be overrided.
    This method calls to run command implementation
    Should return value like RunImpl
    """
    return ScResult.No

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

  @staticmethod
  def CreateCommand(ctx: ScMemoryContext, cmd_class_addr: ScAddr, params: [ScAddr]) -> ScAddr:
    return ScAgentCommandImpl.CreateCommand(ctx, cmd_class_addr, params)

  @staticmethod
  def RunCommand(ctx: ScMemoryContext, cmd_addr: ScAddr) -> bool:
    return ScAgentCommandImpl.RunCommand(ctx, cmd_addr)

  @staticmethod
  def RunCommandWait(ctx: ScMemoryContext, cmd_addr: ScAddr, wait_timeout_ms: int) -> bool:
    return ScAgentCommandImpl.RunCommandWait(ctx, cmd_addr, wait_timeout_ms)

  @staticmethod
  def GetCommandResultAddr(ctx: ScMemoryContext, cmd_addr: ScAddr) -> ScAddr:
    return ScAgentCommandImpl.GetCommandResultAddr(ctx, cmd_addr)

  # --- internal functions ---
  def _kb_resolve_status_addr(self, status):
    if status == ScAgent.Status.SC_RESULT_ERROR:
      sc_result_ok

    return ScAddr()

  def _kb_generate_status(self, status):
    """Append result structure to a special set depending on a status
    """
    pass

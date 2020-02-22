from unittest import TestCase

from common import *
from sc import *

from sc_tests.test_utils import *

class TestScAgent(TestCase):

  def test_static_dummy(self):
    ctx = TestScAgent.MemoryCtx()

    self.assertTrue(ctx)

    cmd_class = ctx.CreateNode(ScType.NodeConstClass)
    self.assertTrue(cmd_class.IsValid())

    params = []
    for i in range(10):
        p = ctx.CreateNode(ScType.NodeConst)
        self.assertTrue(p)

        params.append(p)

    # CreateCommand
    cmd = ScAgentCommand.CreateCommand(ctx, cmd_class, params)
    self.assertTrue(cmd.IsValid())

    # RunCommand
    run_result = ScAgentCommand.RunCommand(ctx, cmd)
    self.assertTrue(run_result)

    # RunCommandWait
    run_result = ScAgentCommand.RunCommandWait(ctx, cmd, 1000)
    self.assertFalse(run_result)

    # GetCommandResultAddr
    result_addr = ScAgentCommand.GetCommandResultAddr(ctx, cmd)
    self.assertFalse(result_addr.IsValid())


# Python

You can implement agents using **Python 3**. All python modules should be implemented with specified rules.

1. You can specify search paths of a python modules by `python.modules_path` value in a [configuration file](../other/config.md).


# Usage example
There are two ways to run python script:

- when you need run script and wait it finished, then just use this code:
```cpp
#include <sc-memory/cpp/python/sc_python_interp.hpp>
...
void doSomething()
{
  ...
  py::ScPythonInterpreter::RunScript("lg/tv_service.py", m_bridge);
  ...
}
```
Then in python you can write any code you want:
```python
import any_module

# create context (with minimal access)
ctx = ScMemoryContext.Create('context name')

# create node
nodeAddr = ctx.CreateNode(ScType.NodeConst)
...

# you can use any installed python libraries
any_module.doSomething()
```

- if you want to run script as a service and communicate with it, then use code:

**my_service.hpp**
```cpp
#pragma once

#include <sc-memory/cpp/python/sc_python_service.hpp>

class MyService : public py::ScPythonService
{
public:
  MyService();

private:
  virtual void RunImpl() override;
  virtual void StopImpl() override;
};

}
```

**my_service.cpp**
```cpp
#include "my_service.hpp"

MyService::MyService()
  : ScPythonService("my_service.py")
{
}

void MyService::RunImpl()
{
  // your run code that need to be run after service started
}

void MyService::StopImpl()
{
  // your run code that need to be run after service stopped
}
```

Then in python code use common module:

**my_service.py**
```python
from common import ScModule

class TestModule(ScModule):
    def __init__(self):
        ScModule.__init__(self,
            ctx=__ctx__,
            cpp_bridge=__cpp_bridge__,
            keynodes = [
            ])

    def DoSomething(self):
        pass

    def OnContentChanged(self, evt_data):
        pass

    def OnInitialize(self, params):
        # subscribe to any event
        self.events.CreateEventContentChanged(content_addr, self.OnContentChanged)

    def OnShutdown(self):
        pass

module = TestModule()
module.Run()
```


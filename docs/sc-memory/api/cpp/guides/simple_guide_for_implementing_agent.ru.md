# **Реализация агента на C++**

!!! примечание
    Это верно только для версий sc-machine >= 0.10.0.
--- 

Все агенты в C++ представляют некоторые классы C++. Чтобы реализовать агент на C++, вам необходимо выполнить следующие простые шаги:

1. Запишите входную (исходную) конструкцию и выходную (результатную) конструкцию вашего будущего агента в SC-коде.
2. Создайте папку с исходными файлами и файлами заголовков для реализации sc-агента.
3. Также вам нужно написать файл `CMakeLists.txt`. Мы используем cmake для создания проектов на C++.
4. В заголовочном файле определите класс на C++ для этого агента и укажите в нем как минимум класс действий, которые выполняет этот агент, а также его программу. В таком классе вы также можете указать первичное условие инициации, условие инициации и условие результата.
5. В исходном файле реализуйте все объявленные методы класса агента. Вы также можете реализовать свои собственные методы и использовать их в программе-агенте. Вы можете максимально использовать все инструменты C++ и ООП. 
6. Создайте файл и реализуйте класс для ключевых узлов, используемых реализованным агентом.
7. Реализуйте класс для модуля для подписки реализованного агента.
8. Напишите тесты для реализованного агента.

---

Рассмотрим пример реализации агента для подсчета мощности заданного множества:

### **1. Укажите входы и выходы вашего будущего агента.**

Первоначальная конструкция агента может выглядеть так:

```scs
..action
<- action_calculate_set_power;
<- action_initiated;
-> rrel_1: ..some_set;;

..some_set
-> ..element_1;
-> ..element_2;
-> ..element_3;;
```

<image src="../images/agents/agent_calculate_set_power_input_construction.png"></image>

Результирующая конструкция агента может выглядеть следующим образом:

```scs
..some_action
=> nrel_result: [*
  ..some_set => nrel_set_power: [3];;
*];;
```

<image src="../images/agents/agent_calculate_set_power_output_construction.png"></image>

В дополнение к агентам, которые самостоятельно инициируют действия и затем их выполняют, существует необходимость реализовать агентов, которые выполняют действия, инициированные другими агентами. Для этого класса агентов гораздо проще сгенерировать начальную конструкцию инициации в базе знаний. Смотрите [**C++ Actions API**](../extended/agents/actions.md), чтобы узнать больше о действиях.

--- 

### **2. Создайте папку с исходными и заголовочными файлами для агента и файла `CMakeLists.txt`.**

У вас должна получиться примерно такая структура:

```plaintext
set-agents-module/
├── CMakeLists.txt
├── agent/
│   ├── sc_agent_calculate_set_power.hpp
│   └── sc_agent_calculate_set_power.сpp
```

---

### **3. Напишите файл `CMakeLists.txt`.**
  
Файл `CMakeLists.txt` должен описывать процесс создания кода вашего агента. Он должен содержать различные инструкции и параметры, необходимые для компиляции и связывания исходного кода агента с библиотеками sc-machine.

**CMakeLists.txt**

```cmake
file(GLOB SOURCES CONFIGURE_DEPENDS
    "*.cpp" "*.hpp"
    "agent/*.cpp" "agent/*.hpp"
)

# Создайте и свяжите свою библиотеку, используя библиотеки.
add_library(set-agents SHARED ${SOURCES})
target_link_libraries(set-agents LINK_PUBLIC sc-machine::sc-memory)
target_include_directories(set-agents PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
# Установите выходной путь для вашего расширения `set-agents.so`. 
# Убедитесь, что переменная `SC_EXTENSIONS_DIRECTORY` имеет значение.
set_target_properties(set-agents 
    PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${SC_EXTENSIONS_DIRECTORY}
)
```

!!! примечание
    Свойство `LIBRARY_OUTPUT_DIRECTORY` должно быть установлено только для библиотек, представляющих модули с агентами.

!!! примечание
    Вы можете установить `CMAKE_OUTPUT_LIBRARY_DIRECTORY` вместо установки свойства `LIBRARY_OUTPUT_DIRECTORY` для каждого расширения.

!!! примечание
    Переменная `SC_EXTENSIONS_DIRECTORY` должна содержать путь к каталогу с расширениями для sc-машины. После сборки модуля с агентом этот путь к каталогу должен быть указан через `--extensions` при запуске sc-машины для загрузки реализованного модуля с агентом.

---

### **4. Определите класс на C++ для этого агента и укажите класс действий, которые выполняет этот агент, и его программу.**

**sc_agent_calculate_set_power.hpp**

```cpp
#pragma once

#include <sc-memory/sc_agent.hpp>

class ScAgentCalculateSetPower : public ScActionInitiatedAgent
{
public:
  ScAddr GetActionClass() const override;

  ScResult DoProgram(ScAction & action) override;
};
```

Реализуемый класс агента должен соответствовать следующим требованиям:

* Он должен наследовать один из общих классов для реализации агентов: 
  
  - `template <class TScEvent> class ScAgent`,
  - или `class ScActionInitiatedAgent`.

Базовый класс `ScAgent` содержит API для реализации агентов, которые реагируют на любые sc-события. Базовый класс `ScActionInitiatedAgent` наследует базовый класс `ScAgent` и предоставляет API для реализации агентов, которые реагируют на sc-события инициирования sc-действия.

* Он должен переопределить как минимум методы `ScAddr GetAction() const` и `ScResult DoProgram(ScActionInitiatedEvent const & event, ScAction & action)`.

* Переопределенные методы должны быть общедоступными. В противном случае вы не сможете создать свой код, поскольку sc-машина не сможет вызывать методы вашего класса агента.

* Вы можете реализовать другие методы в классе агента.

Чтобы узнать больше о возможностях и ограничениях при реализации агентов, смотрите [**C++ Agents API**](../extended/agents/agents.md).

---

### **5. Реализовать все объявленные методы класса агента..**

**sc_agent_calculate_set_power.cpp**

```cpp
#include "sc_agent_calculate_set_power.hpp"

#include <sc-memory/sc_memory_headers.hpp>

ScAddr ScAgentCalculateSetPower::GetActionClass() const
{
  return m_context.SearchElementBySystemIdentifier("action_calculate_set_power");
  // Вы должны убедиться, что этот класс есть в базе знаний.
}
// Вы должны указать действительный класс действия. В противном случае агент не может быть 
// подписан на sc-событие.

ScResult ScAgentCalculateSetPower::DoProgram(ScAction & action)
{
  // Класс `ScAction` инкапсулирует информацию о sc-действии. 
  // Указанное действие — это действие, которое данный агент выполняет прямо сейчас. 
  // Оно принадлежит классу action_calculate_set_power`. 
  // Действия можно копировать и перемещать. ScAction унаследован от ScAddr.

  auto const & [setAddr] = action.GetArguments<1>(); 
  // Этот метод находит конструкцию `action -> rrel_1: setAddr`.
  // Здесь 1 — количество аргументов, которые должно иметь действие. На шаге 1 
  // мы указали, что действие должно иметь множество в качестве первого и 
  // единственного аргумента. Но тот, кто вызывает этого агента, может не указывать
  // аргумент действия. Поэтому нам нужно проверить, что у действия есть аргумент.
  if (!m_context.IsElement(setAddr))
  {
    SC_AGENT_LOG_ERROR("Action does not have argument."); 
    // вывод: «ScAgentCalculateSetPower: Действие не имеет аргумента».
    return action.FinishWithError();
  }
  // Возможна ситуация, когда кто-то пытается указать количество 
  // аргументов больше, чем необходимо. Вы также можете проверить это, указав, 
  // например, номер 2 вместо номера 1. Но не всегда нужно 
  // делать это.

  // Чтобы вычислить мощность множества, мы можем пройтись по всем постоянным  
  // положительным перманентым дугам из множества и подсчитать количество этих дуг.
  // Однако в любой задаче необходимо учитывать наличие НОН-факторов,  
  // но здесь это опущено.
  size_t setPower = 0;
  ScIterator3Ptr const it3 = m_context.CreateIterator3( 
    setAddr,
    ScType::ConstPermPosArc,
    ScType::ConstNode
  );
  while (it3->Next())
    ++setPower;

  ScAddr const & setPowerAddr = m_context.GenerateLink(ScType::ConstNodeLink);
  m_context.SetLinkContent(setPowerAddr, setPower);
  ScAddr const & arcCommonAddr 
    = m_context.GenerateConnector(ScType::ConstCommonArc, setAddr, setPowerAddr);
  ScAddr const & nrelSetPowerAddr 
    = m_context.SearchElementBySystemIdentifier("nrel_set_power");
  // Вы должны убедиться, что это неролевое отношение есть в базе знаний.
  ScAddr const & arcAccessAddr = m_context.GenerateConnector(
    ScType::ConstPermPosArc, nrelSetPowerAddr, arcCommonAddr);

  action.FormResult(
    setAddr, arcCommonAddr, setPowerAddr, arcAccessAddr, nrelSetPowerAddr);
  SC_AGENT_LOG_DEBUG("Set power was counted: " << setPower << ".");

  // В конце программы агента необходимо вызвать один из трёх методов 
  // (`FinishSuccessful`, `FinishUnsuccessful`, `FinishWithError`) 
  // чтобы указать, что выполнение действия агента завершено:
  // - Метод `FinishSuccessful` указывает, что действие выполнено 
  // агентом успешно (устанавливает класс `action_finished_successful`). 
  // Это означает, что агент решил указанную задачу.
  // - Метод `FinishUnsuccessful` указывает, что действие было выполнено 
  // агентом неудачно (устанавливается класс `action_finished_unsuccessful`). 
  // Это означает, что агент не решил указанную задачу.
  // - Метод `FinishWithError` указывает, что действие было выполнено агентом
  // с ошибкой (устанавливает класс `action_finished_with_error`). 
  // Это означает, что в базе знаний произошла некорректная ситуация.
  // Все эти методы возвращают объекты класса `ScResult`. 
  // Вы не можете сгенерировать объект ScResult через конструктор, 
  // поскольку он является частным(private).
}
```

---

### **6. Определите ключевые узлы для реализованного агента и интегрируйте их в программу агента.**

Для каждого агента можно указать ключевые sc-элементы, которые этот агент использует во время выполнения своей программы. Эти ключевые sc-элементы представляют собой sc-элементы, которые агент не генерирует, а использует в процессе поиска или создания связей между сущностями в базе знаний. Ключевые sc-элементы называются ключевыми узлами. Вы можете найти эти ключевые узлы по их системным идентификаторам (метод `SearchElementBySystemIdentifier`), если они имеют такие идентификаторы. Кроме того, вы можете использовать класс `ScKeynode` для определения ключевых узлов как статических объектов и использования их в агентах.

```diff
set-agents-module/
 ├── CMakeLists.txt
 ├── agent/
 │   ├── sc_agent_calculate_set_power.hpp
 │   └── sc_agent_calculate_set_power.сpp
+├── keynodes/
+│   └── sc_set_keynodes.hpp
```

**CMakeLists.txt**

```diff
file(GLOB SOURCES CONFIGURE_DEPENDS
    "*.cpp" "*.hpp"
    "agent/*.cpp" "agent/*.hpp"
+   "keynodes/*.hpp"
)
```

**sc_set_keynodes.hpp**

```cpp
#include <sc-memory/sc_keynodes.hpp>

// Этот класс объединяет ключевые узлы, используемые агентами одного модуля 
// (с одним смыслом). Вы можете реализовать иерархию ключевых узлов.
class ScSetKeynodes : public ScKeynodes
{
public:
  static inline ScKeynode const action_calculate_set_power{
    "action_calculate_set_power", ScType::ConstNodeClass};
  static inline ScKeynode const nrel_set_power{
    "nrel_set_power", ScType::ConstNodeNonRole};
  // Здесь первым аргументом конструктора является системный идентификатор 
  // sc-ключевого узла, а второй аргумент — это sc-тип этого sc-ключевого узла. 
  // Если в базе знаний нет sc-ключевого узла с таким системным идентификатором, 
  // то будет сгенерирован узел с указанным sc-типом. 
  // Здесь не нужно указывать тип sc-ключевого узла, по умолчанию это
  // `ScType::ConstNode`. Но вы должны быть уверены, что ваш код будет правильно 
  // использовать этот ключевой узел типа `ScType::ConstNode`.
};
```

!!! предупреждение
    Вы не можете указать sc-ключевой узел с пустым системным идентификатором. Оно может быть недействительным.

!!! предупреждение
    Все ключевые узлы должны быть статическими объектами. Вы можете определять ключевые узлы как статические объекты везде (не только в классах).

Класс `ScKeynodes` является базовым классом для всех классов с ключевыми узлами. Он содержит основные ключевые узлы, которые можно использовать в каждом агенте. Смотреть [**C++ Keynodes API**](../extended/agents/keynodes.md), чтобы узнать больше о ключевых узлах.

**sc_agent_calculate_set_power.cpp**

```diff
#include "sc_agent_calculate_set_power.hpp"

#include <sc-memory/sc_memory_headers.hpp>

+ #include "keynodes/sc_set_keynodes.hpp"

ScAddr ScAgentCalculateSetPower::GetActionClass() const
{
- return m_context.SearchElementBySystemIdentifier("action_calculate_set_power");
+ return ScSetKeynodes::action_calculate_set_power;
}

ScResult ScAgentCalculateSetPower::DoProgram(ScAction & action)
{
  ...

- ScAddr const & nrelSetPowerAddr 
-   = m_context.SearchElementBySystemIdentifier("nrel_set_power");
- ScAddr const & arcAccessAddr = m_context.GenerateConnector(
-   ScType::ConstPermPosArc, nrelSetPowerAddr, arcCommonAddr);
+ ScAddr const & arcAccessAddr = m_context.GenerateConnector(
+   ScType::ConstPermPosArc, 
+   ScSetKeynodes::nrel_set_power, 
+   arcCommonAddr);
- action.FormResult(
-   setAddr, arcCommonAddr, setPowerAddr, arcAccessAddr, nrelSetPowerAddr);
+ action.FormResult(
+   setAddr, 
+   arcCommonAddr,
+   setPowerAddr,
+   arcAccessAddr, 
+   ScSetKeynodes::nrel_set_power);
  ...
}
```

---

### **7. Реализуйте класс модуля, чтобы подписать вашего агента на указанное sc-событие.**

Кто-то должен подписать вашего агента на событие. Это может быть другой агент или любой код. Вы можете реализовать класс, который позволяет подписывать агентов. Этот класс называется sc-module. Каждый sc-module должен подписывать агентов с общим смыслом.

```diff
 set-agents-module/
 ├── CMakeLists.txt
 ├── agent/
 │   ├── sc_agent_calculate_set_power.hpp
 │   └── sc_agent_calculate_set_power.сpp
 ├── keynodes/
 │   └── sc_set_keynodes.hpp
+├── sc_set_module.hpp
+└── sc_set_module.cpp
```

**sc_set_module.hpp**

```cpp
#pragma once

#include <sc-memory/sc_module.hpp>

class ScSetModule : public ScModule
{
  // Здесь класс пуст. Вам не нужно реализовывать какие-либо методы. 
  // Класс `ScModule` содержит все необходимые API для подписки ваших
  // агентов как отдельный sc-модуль.
};
```

**sc_set_module.cpp**

```cpp
#include "sc_set_module.hpp"

#include "agent/sc_agent_calculate_set_power.hpp"

SC_MODULE_REGISTER(ScSetModule)
  ->Agent<ScAgentCalculateSetPower>();
  // Этот метод указывает на модуль, к которому класс агента  `ScAgentCalculateSetPower`
  // должен подписаться на sc-событие добавления исходящей sc-дуги из sc-элемента
  // `action_initiated`. Этот параметр по умолчанию в этом методе, если вы хотите
  // подписаться на класс агента, унаследованный от `ScActionInitiatedAgent`.

// Такой способ подписки на агентов упрощает написание кода. 
// Вам не придется думать об отписке агентов после 
// выключение системы - ваш модуль сделает все сам.
```

Если вы хотите инициализировать в своем модуле что-то еще, помимо агентов, вы можете переопределить методы `Initialize(ScMemoryContext * context) override;` и `Shutdown(ScMemoryContext * context) override;`.

**sc_set_module.hpp**

```diff
class ScSetModule : public ScModule
{
+ void Initialize(ScMemoryContext * context) override;
+ void Shutdown(ScMemoryContext * context) override;
};
```

**sc_set_module.cpp**

```diff
SC_MODULE_REGISTER(ScSetModule)
  ->Agent<ScAgentCalculateSetPower>();

+ // Этот метод будет вызван один раз. 
+ void ScSetModule::Initialize(ScMemoryContext * context)
+ {
+   // Здесь реализуем инициализацию ваших объектов.
+ }
+ // Этот метод будет вызван один раз. 
+ void ScSetModule::Shutdown(ScMemoryContext * context)
+ {
+   // Здесь реализуем выключение ваших объектов.
+ }
```

Со всем функционалом модулей вы можете ознакомиться в разделе [**C++ Modules API**](../extended/agents/modules.md).

### **8. Написание тестов для реализованного агента. Проверка логики агента.**

Чтобы убедиться в том, как работает ваш агент, лучше всего создать тесты и охватить ими все возможные случаи, которые должен справиться ваш агент. Для этого создайте отдельный файл с тестовыми случаями и реализуйте их.

```diff
 set-agents-module/
 ├── CMakeLists.txt
 ├── agent/
 │   ├── sc_agent_calculate_set_power.hpp
 │   └── sc_agent_calculate_set_power.сpp
 ├── keynodes/
 │   └── sc_set_keynodes.hpp
+├── tests/
+│   └── test_sc_agent_calculate_set_power.cpp
 ├── sc_set_module.hpp
 └── sc_set_module.cpp
```

**CMakeLists.txt**

```diff
file(GLOB SOURCES CONFIGURE_DEPENDS
    "*.cpp" "*.hpp"
    "agent/*.cpp" "agent/*.hpp"
    "keynodes/*.hpp"
)

add_library(set-agents SHARED ${SOURCES})
target_link_libraries(set-agents LINK_PUBLIC sc-machine::sc-memory)
target_include_directories(set-agents PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties(set-agents 
    PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${SC_EXTENSIONS_DIRECTORY}
)

+file(GLOB TEST_SOURCES CONFIGURE_DEPENDS
+    "tests/*.cpp"
+)

# Генерируем исполняемый файл для тестов.
+add_executable(set-agents-tests ${TEST_SOURCES})
+target_link_libraries(set-agents-tests LINK_PRIVATE set-agents)
+target_include_directories(set-agents-tests 
+    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
+)

+gtest_discover_tests(set-agents-tests
+    TEST_LIST ${TEST_SOURCES}
+    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests
+)
```

**test_sc_agent_calculate_set_power.cpp**

```cpp
#include <sc-memory/test/sc_test.hpp>

#include <sc-memory/sc_memory_headers.hpp>

#include "agent/sc_agent_calculate_set_power.hpp"
#include "keynodes/sc_set_keynodes.hpp"

using AgentTest = ScMemoryTest;

TEST_F(AgentTest, AgentCalculateSetPowerFinishedSuccessfully)
{
  // Генерируем действие с классом, которое выполняет ваш агент.
  ScAction action 
    = m_ctx->GenerateAction(ScSetKeynodes::action_calculate_set_power);

  // Генерируем множество из двух sc-элементов.
  ScSet set = m_ctx->GenerateSet();
  ScAddr nodeAddr1 = m_ctx->GenerateNode(ScType::ConstNode);
  ScAddr nodeAddr2 = m_ctx->GenerateNode(ScType::ConstNode);
  set << nodeAddr1 << nodeAddr2;

  // Установить сгенерированное множество в качестве аргумента для действия.
  action.SetArgument(1, set);

  // Инициируем и ждем, пока действие завершится.
  action.InitiateAndWait();

  // Проверяем, что действие завершилось успешно.
  EXPECT_TRUE(action.IsFinishedSuccessfully());

  // Получаем структуру результата действия.
  ScStructure structure = action.GetResult();
  // Проверяем наличие sc-элементов.
  EXPECT_FALSE(structure.IsEmpty());

  // Проверяем sc-конструкции в структуре результата.
  // Проверяем первые три элемента конструкции.
  ScIterator3Ptr it3 = m_ctx->CreateIterator3(
    structure, ScType::ConstPermPosArc, ScType::ConstCommonArc);
  EXPECT_TRUE(it3->Next());
  ScAddr arcAddr = it3->Get(2);

  ScAddr beginAddr;
  ScAddr linkAddr;
  m_ctx->GetConnectorIncidentElements(arcAddr, beginAddr, linkAddr);
  EXPECT_EQ(beginAddr, set);
  EXPECT_TRUE(m_ctx->GetElementType(linkAddr).IsLink());

  // Проверяем, что содержимое ссылки равно 2.
  size_t setPower;
  EXPECT_TRUE(m_ctx->GetLinkContent(linkAddr, setPower));
  EXPECT_EQ(setPower, 2u);

  // Проверяем вторую трехэлементную конструкцию.
  it3 = m_ctx->CreateIterator3(
    structure, ScType::ConstPermPosArc, ScType::ConstPermPosArc);
  EXPECT_TRUE(it3->Next());
  ScAddr arcAddr2 = it3->Get(2);

  ScAddr relationAddr;
  ScAddr targetArcAddr;
  m_ctx->GetConnectorIncidentElements(arcAddr2, relationAddr, targetArcAddr);
  EXPECT_EQ(relationAddr, ScSetKeynodes::nrel_set_power);
  EXPECT_EQ(targetArcAddr, arcAddr);
}

// Предоставляем тесты для неудачных и ошибочных ситуаций.
...
```

!!! примечание
    Хороший код — это код, покрытый тестами.

Класс `ScMemoryTest` включает `m_ctx`, который является объектом класса `ScAgentContext`. Вы можете использовать его для работы с sc-памятью. Смотрите [**C++ Core API**](../core/api.md) и [**C++ Agent context API**](../extended/agents/agents.md), чтобы узнать больше о доступных методах для работа с sc-памятью.

По умолчанию репозиторий sc-machine содержит конфигурацию для запуска тестов в vscode. Если вы разрабатываете на vscode, вы можете использовать эту конфигурацию и расширение [C++ TestMate](https://marketplace.visualstudio.com/items?itemName=matepek.vscode-catch2-test-adapter) для отладки кода.

--- 

## **Часто Задаваемые Вопросы**

<!-- no toc -->
- [Как правильно написать CMakeLists.txt? Что это такое? Как с этим работать?](#what-is-the-correct-way-to-write-cmakeliststxt-what-is-it-how-to-work-with-it)
- [Как правильно писать тесты?](#how-to-write-tests-correctly)

### **Как правильно написать CMakeLists.txt? Что это такое? Как с этим работать?**

CMake — это широко используемая система сборки, которая упрощает управление сборками программного обеспечения, особенно для проектов C++. Это позволяет разработчикам определять процесс сборки независимым от платформы способом, который затем можно использовать для создания собственных сценариев сборки для различных сред.

Используйте [это руководство](https://cliutils.gitlab.io/modern-cmake/), чтобы правильно написать CMakeLists.txt.

### **Как правильно писать тесты?**

Мы используем GoogleTest для тестирования нашего кода. GoogleTest, часто называемый gtest, — это среда тестирования C++, разработанная Google. Он используется для написания и запуска модульных тестов в проектах C++.

Используйте [это руководство](https://google.github.io/googletest/), чтобы писать хорошие тесты.

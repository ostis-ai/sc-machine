# **ScHelper API**

!!! примечание
    Это верно только для версий sc-machine, которые >= 0.10.0.
---

Этот API позволяет работать с системными идентификаторами sc-элементов. 
Все методы этого API являются частью класса `ScMemoryContext`. 
Таким образом, вы можете использовать их из объекта класса `ScMemoryContext`.

Системный идентификатор - это идентификатор, уникальный во всей базе знаний. Этот идентификатор обычно используется в исходных текстах базы знаний более низкого уровня. Для обеспечения интернационализации рекомендуется, чтобы системные идентификаторы были написаны на английском языке. Символами, используемыми в системном идентификаторе, могут быть буквы латинского алфавита, цифры, символы подчеркивания и тире. Таким образом, наиболее целесообразно сформировать системный идентификатор sc-элемента из основного английского, заменив все символы, не входящие в описанный выше алфавит, на символ “_”.


!!! примечание
    Все системные идентификаторы должны соответствовать стандартному шаблону `([a-z]|[A-Z]|'_'|'.'|[0-9])+`.

!!! примечание
    Теперь только sc-узлы и sc-ссылки могут иметь системные идентификаторы.
    
У sc-узлов есть пятиэлементная конструкция с их системным идентификатором.

<scg src="../images/helper/system_identifier_example_1.gwf"></scg>

!!! примечание
    Чтобы подключить этот API, укажите `#include <sc-memory/sc_memory.hpp>` в вашем исходном файле hpp.

## **SetElementSystemIdentifier**

Чтобы задать системный идентификатор для некоторого sc-узла или sc-ссылки, используйте метод `SetElementSystemIdentifier`. Если переданный системный идентификатор неверен, то метод `SetElementSystemIdentifier` выдаст исключение `utils::ExceptionInvalidParams` с описанием ошибки. Если переданный системный идентификатор уже используется для другого sc-элемента, то метод вернет значение `false`.


```cpp
...
ScAddr const & nodeAddr = context.GenerateNode(ScType::ConstNode);

bool const & isSystemIdentifierSet 
    = context.SetElementSystemIdentifier("my_node", nodeAddr);
// Значение `isSystemIdentifierSet` должно быть равно `true`.
...
```

Если вы хотите получить созданную пятиэлементную конструкцию, вы можете указать переменную типа `ScSystemIdentifierQuintuple` в качестве параметра out.

```cpp
...
ScAddr const & nodeAddr = context.GenerateNode(ScType::ConstNode);

ScSystemIdentifierQuintuple quintuple;
bool const & isSystemIdentifierSet 
    = context.SetElementSystemIdentifier("my_node", nodeAddr, quintuple);
ScAddr const & myNodeAddr = quintuple.addr1;
// Значение `myNodeAddr` должно быть равно значению `nodeAddr`.
ScAddr const & arcToSystemIdtfLinkAddr = quintuple.addr2;
// Sc-адрес общей sc-дуги между вашим sc-узлом и 
// sc-ссылкой с системным идентификатором вашего sc-узла.
ScAddr const & systemIdtfLinkAddr = quintuple.addr3;
// Sc-адрес sc-ссылки с системным идентификатором вашего sc-узла.
ScAddr const & arcToArcToSystemIdtfLinkAddr = quintuple.addr4;
// Sc-адрес sc-дуги членства между бинарным sc-отношением 
// с системным идентификатором `nrel_system_identifier` и общей sc-дугой
// между вашим sc-узлом и sc-ссылкой с системным идентификатором вашего sc-узла.
ScAddr const & nrelSystemIdtfAddr = quintuple.addr5;
// Sc-адрес бинарного sc-отношения с системным идентификатором 
// `nrel_system_identifier`.
...
```

## **GetElementSystemIdentifier**

Чтобы получить системный идентификатор sc-элемента, вы можете использовать метод ```GetElementSystemIdentifier```. Если sc-элемент не имеет системного идентификатора, то метод вернет пустую строку.

```cpp
...
ScAddr const & nodeAddr = context.GenerateNode(ScType::ConstNode);

bool const & isSystemIdentifierSet 
    = context.SetElementSystemIdentifier("my_node", nodeAddr);
// Значение `isSystemIdentifierSet` должно быть равно `true`.

std::string const & systemIdtf = context.GetElementSystemIdentifier(nodeAddr);
// Значение `systemIdtf` должно быть равно `"my_node".
...
```

## **SearchElementBySystemIdentifier**

Вы можете найти sc-элемент по его системному идентификатору. Для этого используйте метод `SearchElementBySystemIdentifier`. Он вернет значение `true`, если есть sc-элемент с указанным системным идентификатором, в противном случае значение `false`.

```cpp
...
ScAddr const & nodeAddr = context.GenerateNode(ScType::ConstNode);

bool const & isSystemIdentifierSet 
    = context.SetElementSystemIdentifier("my_node", nodeAddr);
// Значение `isSystemIdentifierSet` должно быть равно `true`.

ScAddr resultAddr;
bool const & isElementWithSystemIdtfFound 
    = context.SearchElementBySystemIdentifier("my_node", resultAddr);
// Значение `isElementWithSystemIdtfFound` должно быть равно `true` 
// а значение `resultAddr` должно быть равно значению `nodeAddr`.

// Или используйте другое определение этого метода.
resultAddr = context.SearchElementBySystemIdentifier("my_node");
// Значение `resultAddr` должно быть равно значению `nodeAddr`.

// Или используйте другое определение этого метода.
resultAddr = context.SearchElementBySystemIdentifier("not_my_node");
// Значение `resultAddr` должно быть неверным.
...
```

Если вы хотите найти пятиэлементную конструкцию по её системному идентификатору, вы можете указать переменную типа ```ScSystemIdentifierQuintuple``` в качестве параметра out.

```cpp
...
ScAddr const & nodeAddr = context.GenerateNode(ScType::ConstNode);

bool const & isSystemIdentifierSet 
    = context.SetElementSystemIdentifier("my_node", nodeAddr);
// Значение `isSystemIdentifierSet` должно быть равно `true`.

ScSystemIdentifierQuintuple quintuple;
bool const & isElementWithSystemIdtfFound 
    = context.SearchElementBySystemIdentifier("my_node", quintuple);
ScAddr const & myNodeAddr = quintuple.addr1;
// Значение `myNodeAddr` должно быть равно значению `nodeAddr`.
ScAddr const & arcToSystemIdtfLinkAddr = quintuple.addr2;
// Sc-адрес общей sc-дуги между вашим sc-узлом 
// и sc-ссылка с системным идентификатором вашего sc-узла.
ScAddr const & systemIdtfLinkAddr = quintuple.addr3;
// Sc-адрес sc-ссылки с системным идентификатором вашего sc-узла.
ScAddr const & arcToArcToSystemIdtfLinkAddr = quintuple.addr4;
// Sc-адрес sc-дуги членства между бинарным sc-отношением 
// с системным идентификатором `nrel_system_identifier` и общей sc-дугой 
// между вашим sc-узлом и sc-ссылкой с системным идентификатором вашего sc-узла.
ScAddr const & nrelSystemIdtfAddr = quintuple.addr5;
// Sc-адрес бинарного sc-отношения с системным идентификатором 
// `nrel_system_identifier`.
...
```

## **ResolveElementSystemIdentifier**

Очень часто вам может потребоваться определить sc-элемент по системному идентификатору. Определение sc-элемента по системному идентификатору означает поиск sc-элемента с указанием системного идентификатора и (если такой sc-элемент не найден) создание sc-элемента с указанным системным идентификатором и типом sc.

```cpp
...
ScAddr const & nodeAddr 
    = ResolveElementSystemIdentifier("my_node", ScType::ConstNode);
// Если нет sc-элемента с системным идентификатором `"my_node"`, 
// то метод создаст sc-элемент с этим системным идентификатором 
// и указанным sc-типом `ScType::ConstNode`.
...
```

!!! примечание
    Указанный sc-тип должен быть подтипом sc-типа sc-узла или sc-типа sc-ссылки.

Если вы хотите разрешить использование пятиэлементной конструкции с его системным идентификатором, вы можете указать переменную типа ```ScSystemIdentifierQuintuple``` в качестве параметра out.

```cpp
...
ScSystemIdentifierQuintuple quintuple;
bool const & isSystemIdentifierResolved 
    = context.ResolveElementSystemIdentifier("my_node", ScType::ConstNode, quintuple);
ScAddr const & myNodeAddr = quintuple.addr1;
// Sc-адрес разрешенного sc-узла по предоставленному системному идентификатору.
ScAddr const & arcToSystemIdtfLinkAddr = quintuple.addr2;
// sc-адрес общей sc-дуги между вашим sc-узлом и sc-ссылкой 
// с системным идентификатором вашего sc-узла.
ScAddr const & systemIdtfLinkAddr = quintuple.addr3;
// Sc-адрес sc-ссылки с системным идентификатором вашего sc-узла.
ScAddr const & arcToArcToSystemIdtfLinkAddr = quintuple.addr4;
// sc-адрес sc-дуги членства между бинарным sc-отношением 
// с системным идентификатором `nrel_system_identifier` и общей sc-дугой 
// между вашим sc-узлом и sc-ссылкой с системным идентификатором вашего sc-узла.
ScAddr const & nrelSystemIdtfAddr = quintuple.addr5;
// Sc-адрес бинарной sc-связи с системным идентификатором 
// `nrel_system_identifier`.
...
```

--- 


## **Часто Задаваемые Вопросы**

<!-- no toc -->
- [Могу ли я указать пустой системный идентификатор для sc-элемента?](#can-i-specify-empty-system-identifier-for-sc-element)

### **Могу ли я указать пустой системный идентификатор для sc-элемента?**

Вы не можете указать пустой системный идентификатор для sc-элемента. Все системные идентификаторы должны соответствовать стандартному шаблону `([a-z]|[A-Z]|'_'|'.'|[0-9])+`.

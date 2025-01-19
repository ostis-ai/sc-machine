# **C++ Core API**

!!! примечание
    Это верно только для версий sc-machine, которые >= 0.10.0.
---

API предоставляет основные функциональные возможности для создания, извлечения и удаления sc-элементов в sc-памяти. Эта sc-память предназначена для представления знаний в структурированном и графовом формате.

Sc-память состоит из sc-элементов. Sc-элементы могут быть сгруппированы в sc-конструкции (семантические графы или графы знаний). Среди sc-элементов есть sc-узлы, sc-связи и sc-коннекторы между ними. Все sc-элементы имеют синтаксический и семантический sc-типы. Sc-коннекторы делятся на неориентированные sc-коннекторы (sc-ребра) и ориентированные sc-коннекторы (sc-дуги). Каждый sc-коннектор имеет начальный и конечный sc-элементы. Эти sc-элементы могут быть sc-узлами, sc-ссылками или sc-коннекторами. Все эти атрибуты sc-элементов и структур sc-элементов скрыты от пользователя, но вы можете получить эти функции, используя этот API.

!!! примечание
    Все методы API являются потокобезопасными.
 
!!! примечание
    Чтобы подключить этот API, укажите ```#include <sc-memory/sc_memory.hpp>``` в вашем            исходный файле hpp.

!!! предупреждение 
    Объекты класса ```ScMemoryContext``` являются подвижными, но не копируемыми.

## **ScAddr**

Каждый sc-элемент имеет sc-адрес, с помощью которого вы можете получить информацию об этом sc-элементе.

```cpp
...
ScAddr elementAddr1 = // некоторый вызов метода API 1
ScAddr elementAddr2 = // некоторый вызов метода API 2
// Сравниваем sc-адреса
bool const isAddrsEqual = elementAddr1 == elementAddr2;
```
## **ScMemoryContext**

Это класс, предоставляющий все методы API для создания, управления, извлечения и удаления sc-элементов в sc-памяти. Он относится к набору атрибутов и прав некоторого субъекта действия в sc-памяти (пользователя или агента), которые используются во время выполнения методов API.

```cpp
...
// Для создания такого контекста используйте конструктор ScMemoryContext.
ScMemoryContext context;
// После этого вы можете использовать этот объект для вызова любых методов API.
```

### **GenerateNode**

Для создания sc-узлов вы можете использовать метод `GenerateNode`, для создания sc-ссылок - метод `GenerateLink`, для создания sc-коннекторов между ними - метод `GenerateConnector`. Все эти методы проверяют переданные sc-типы. Если указанный sc-тип недействителен, то метод выдает исключение `utils::ExceptionInvalidParams` с описанием этой ошибки.

```cpp
...
// Сгенерировать sc-узел и получить его sc-адрес в sc-памяти.
ScAddr const & nodeAddr = context.GenerateNode(ScType::ConstNode);
// Указанный sc-тип должен быть одним из типов ScType::...Node... type.
```

### **GenerateLink**

```cpp
...
// Сгенерируйте sc-ссылку и получите ее sc-адрес в sc-памяти.
ScAddr const & linkAddr = context.GenerateLink(ScType::ConstNodeLink);
// Указанный sc-тип должен быть одним из типов ScType::...NodeLink... type.
```

!!! примечание
    Теперь все sc-ссылки не являются sc-узлами. Это может быть исправлено в последующих            версиях sc-machine. Но вы можете использовать метод `GenerateNode` для создания sc-ссылок.

### **GenerateConnector**

```cpp
...
// Сгенерировать sc-дугу между sc-узлом и sc-каналом и получить sc-адрес в 
// sc-памяти этого узла.
ScAddr const & arcAddr = context.GenerateConnector(
    ScType::ConstPermPosArc, nodeAddr, linkAddr);
// Указанный sc-тип должен быть одним из типов ScType::Edge... type.
```

Если указанные sc-адреса начального и конечного sc-элементов недействительны, то метод выдает исключение `utils::ExceptionInvalidParams` с описанием того, что некоторые из указанных sc-адресов недействительны.

!!! примечание
    Хотя этот метод вызван неправильно и может ввести в заблуждение, но с его помощью вы           можете создать любые sc-коннекторы.

### **IsElement**

Чтобы проверить, является ли указанный sc-адрес действительным в sc-памяти, вы можете использовать метод `IsElement`. Действительный sc-адрес относится к sc-адресу, который существует в sc-памяти и который соответствует некоторому sc-элементу в ней.

```cpp
...
// Проверьте, все ли созданные sc-элементы допустимы.
bool const isNodeValid = context.IsElement(nodeAddr);
bool const isLinkValid = context.IsElement(linkAddr);
bool const isArcValid = context.IsElement(arcAddr);
```

!!! примечание
    Вы можете проверить, не является ли указанный sc-адрес пустым, вызвав из этого sc-адреса       объектный метод `IsValid`. Но предпочтительнее использовать `IsElement`, он            проверяет, существует ли указанный sc-адрес и действителен ли он в sc-памяти.

### **GetElementType**

При необходимости вы можете получить типы sc-элементов по их sc-адресам. Если указанный sc-адрес неверен, то метод выдает исключение `utils::ExceptionInvalidParams` с описанием того, что указанный sc-адрес неверен.

```cpp
...
// Получить созданные sc-элементы sc-типов.
ScType const & nodeType = context.GetElementType(nodeAddr);
ScType const & linkType = context.GetElementType(linkAddr);
ScType const & arcType = context.GetElementType(arcAddr);
```

### **SetElementSubtype**

Вы можете изменить семантический sc-тип sc-элемента. Используйте метод `SetElementSubtype` и укажите семантический sc-тип для синтаксического sc-типа sc-элемента.

```cpp
...
// Сгенерировать sc-узел и получить его sc-адрес в sc-памяти.
ScAddr const & nodeAddr = context.GenerateNode(ScType::Node);
bool const isSubtypeElementChanged 
    = context.SetElementSubtype(nodeAddr, ScType::ConstNode);
// Значение `isSubtypeElementChanged` должно быть равно `true`.
```

!!! примечание
    Не используйте этот метод для изменения синтаксического sc-типа для sc-элемента. Это           вводит в заблуждение.

### **GetConnectorIncidentElements**

Чтобы получить инцидентные (начальные и конечные) sc-элементы, вы можете использовать методы `GetConnectorIncidentElements` `GetArcSourceElement` и `GetArcTargetElement`. Если указанный sc-адрес недействителен, то эти методы выдадут исключение utils::ExceptionInvalidParams с описанием того, что указанный sc-адрес sc-коннектора недействителен.

```cpp
...
// Получите инцидентные sc-элементы для sc-дуги.
auto const [sourceAddr, targetAddr] 
  = context.GetConnectorIncidentElements(arcAddr);
// sc-адрес `sourceAddr` должен быть равен sc-адресу `nodeAddr` 
// и sc-адрес `targetAddr` должен быть равен sc-адресу `linkAddr`.
...
// Или получите инцидентные источник и цель sc-дуги отдельно.
ScAddr const & sourceAddr = context.GetArcSourceElement(arcAddr);
// sc-адрес `sourceAddr` должен быть равен sc-адресу `nodeAddr`.
ScAddr const & targetAddr = context.GetArcTargetElement(arcAddr);
// sc-адрес `targetAddr` должен быть равен sc-адресу `linkAddr`.
```
Чтобы найти ранее созданные sc-конструкции, вы можете использовать 3-элементные sc-итераторы и 5-элементные sc-итераторы из C++ API. На рисунке ниже показана нумерация sc-элементов в 3-элементных (слева) и 5-элементных (справа) sc-итераторах.


<scg src="../images/iterators_scheme.gwf">**Нумерация элементов итераторов**</scg>

Для них обоих у вас есть два подхода. Первый подход заключается в использовании простого цикла while. Он подходит, когда вам нужно разорвать этот цикл в зависимости от логики:

### **ScIterator3**

```cpp
...
// Создать sc-итератор для поиска всех sc-адресов sc-элементов 
// с неизвестным sc-типом, принадлежащим sc-множеству, с sc-адресом `setAddr`.
ScIterator3Ptr it3 = context.CreateIterator3(
    setAddr,
    ScType::ConstPermPosArc,
    ScType::Unknown);
// Используйте it3->Next(), чтобы перейти к следующей подходящей
// по условию sc-конструкции.
while (it3->Next())
{
// Чтобы получить значения, используйте `it3->Get(index)`,
// где index в диапазоне [0; 2]. 
 // Он возвращает `true`, если найдена следующая подходящая конструкция,
// в противном случае `false`.
 ... // Напишите свой код для обработки найденной sc-конструкции.
}
```

### **ScIterator5**

```cpp
...
// Создать sc-итератор для поиска всех sc-адресов sc-узлов,
// пары которых с sc-элементом с адресом `setAddr` принадлежат отношению 
// с sc-адресом `nrelDecompositionAddr`.
ScIterator5Ptr it5 = context.CreateIterator5(
    setAddr,
    ScType::ConstCommonArc,
    ScType::ConstNode,
    ScType::ConstPermPosArc,
    nrelDecompositionAddr);
// Используйте `it5->Next()`, чтобы перейти к следующей
// подходящей по условию sc-конструкции. 
// Она возвращает `true`, если найдена следующая подходящая конструкция,
// в противном случае `false`.
while (it5->Next())
{
  // Чтобы получить значения, используйте `it5->Get(index)`,
  // где index в диапазоне [0; 4].
 ... // Напишите свой код для обработки найденной sc-конструкции.
}
```

Второй подход позволяет вам выполнять итерацию 3-элементных и 5-элементных конструкций с меньшим объемом кода, и он подходит, когда вам нужно проитерировать все результаты.

!!! примечание
    Используйте следующие методы, если вам нужно проитерировать все результаты. Потому что так     проще.

### **ForEach**

```cpp
...
// Создать sc-итератор на основе обратного вызова
// для поиска всех sc-адресов sc-элементов 
// с неизвестным sc-типом, принадлежащим sc-множеством, с sc-адресом `setAddr`.
context.ForEach(
    setAddr,
    ScType::ConstPermPosArc,
    ScType::Unknown,
    [] (ScAddr const & srcAddr, ScAddr const & edgeAddr, ScAddr const & trgAddr)
{
// srcAddr равен 0-му значению итератора
 // edgeAddr равен 1-му значению итератора
 // trgAddr равен 2-му значению итератора
 ... // Напишите свой код для обработки найденной sc-конструкции.
});
```

```cpp
...
// Создать sc-итератор на основе обратного вызова
// для поиска всех sc-адресов sc-узлов,
// пары которых с sc-элементом с адресом `setAddr` принадлежат отношению 
// с sc-адресом `nrelDecompositionAddr`.
context.ForEach(
  setAddr,
  ScType::ConstCommonArc,
  ScType::ConstNode,
  ScType::ConstPermPosArc,
  nrelDecompositionAddr
  [] (ScAddr const & srcAddr, 
      ScAddr const & connectorAddr, 
      ScAddr const & trgAddr, 
      ScAddr const & connectorAttrAddr, 
      ScAddr const & attrAddr)
{
 // srcAddr равен 0-му значению sc-итератора
 // connectorAddr равен 1-му значению sc-итератора
 // trgAddr равен 2-му значению sc-итератора
 // connectorAttrAddr равен 3-му значению sc-итератора
 // attrAddr равен 4-му значению sc-итератора
 ... // Напишите свой код для обработки найденной sc-конструкции.
});
```

### **EraseElement**

Все sc-элементы могут быть удалены из sc-памяти. Для этого вы можете использовать метод ```EraseElement```.

```cpp
...
// Удалить все созданные sc-элементы.
bool const isNodeErased = context.EraseElement(nodeAddr);
// Необходимо удалить sc-элементы с sc-адресами `nodeAddr` и `arcAddr`.
bool const isArcErased = context.EraseElement(arcAddr);
// sc-элемент с sc-адресом `targetAddr` должен быть удален.
```

### **SetLinkContent**

Помимо создания и проверки элементов, API также поддерживает обновление и удаление содержимого sc-ссылок. Если указанный sc-элемент является sc-ссылкой, вы можете установить в него содержимое. При повторной установке нового содержимого в sc-ссылки предыдущее содержимое из этой sc-ссылки удаляется. Если указанный sc-адрес неверен, то метод выдает исключение `utils::ExceptionInvalidParams` с описанием того, что указанный sc-адрес неверен.

```cpp
...
ScAddr const & linkAddr1 = context.GenerateLink(ScType::ConstNodeLink);
// Установите содержимое строки в созданную sc-ссылку.
context.SetLinkContent(linkAddr1, "my content");

ScAddr const & linkAddr2 = context.GenerateLink(ScType::ConstNodeLink);
// Установите числовое содержимое в созданную sc-ссылку.
context.SetLinkContent(linkAddr2, 10f);
...
```

!!! примечание
    Не используйте значение результата, оно ничего не значит.

### **GetLinkContent**

Чтобы получить существующее содержимое из sc-ссылки, вы можете использовать метод  `GetLinkContent`. Содержимое может быть представлено в виде числа или строки. Если содержимое отсутствует в sc-ссылке, метод ```GetLinkContent``` вернет значение ```false```, а результат будет пустым.

```cpp
...
// Получить содержимое строки из sc-link.
std::string stringContent;
bool const stringContentExist 
  = context.GetLinkContent(linkAddr1, stringContent);

// Получите числовое содержимое из sc-link.
float numericContent;
bool const numericContentExist 
  = context.GetLinkContent(linkAddr1, numericContent);
...
```

!!! примечание
    Вы можете установить пустое содержимое в sc-ссылку, но это означает, что эта sc-ссылка         имеет содержимое, и этот метод для этой sc-ссылки вернет значение `true`.

### **SearchLinksByContent**

Вы можете найти sc-ссылки по их содержимому. Для этого воспользуйтесь методом `SearchLinksByContent`.

```cpp
...
// Найдите sc-ссылки с указанным строковым содержимым.
ScAddrSet const & linkAddrs1 = context.SearchLinksByContent("my content");
// Множество `linkAddrs1` должно содержать sc-адрес `linkAddr1`.

// Найдите sc-ссылки с указанным числовым содержимым.
ScAddrSet const & linkAddrs2 = context.SearchLinksByContent(10f);
// Множество `linkAddrs2` должно содержать sc-адрес `linkAddr2`.
```

### **SearchLinksByContentSubstring**

Вы можете найти sc-ссылки по подстроке их содержимого. Для этого используйте метод `SearchLinksByContentSubstring`.

```cpp
...
// Найдите sc-ссылки с указанной подстрокой содержимого строки.
ScAddrSet const & linkAddrs1 
  = context.SearchLinksByContentSubstring("my cont");
// Множество `linkAddrs1` должен содержать sc-адрес `linkAddr1`.
```

### **ScException**

Чтобы объявить свои собственные исключения, наследуйте их от класса `ScException`.

 ```cpp
class MyException final : public ScException
{
public:
  explicit MyException(std::string const & msg) 
    : ScException("MyException: " + msg)
  {}
};
```

Чтобы выбрасывать исключения, используйте ```SC_THROW_EXCEPTION(имяИсключения, сообщение);```.

```cpp
SC_THROW_EXCEPTION(MyException, "It is my exception.");
```
Выбрасывает исключение для не реализованной части кода.

```cpp
SC_NOT_IMPLEMENTED("This code is not implemented.");
```

### **ScLogger**

Существуют стандартные макросы, которые вы можете использовать для логирования вашего кода. Они инкапсулируют класс `ScLogger`, чтобы предотвратить неуместный доступ к нему и предоставить метапрограммирование при логировании сообщений.



| Макрос                 | Префикс сообщения | Цвет сообщения                | Уровни логирования               |
|------------------------|-------------------|-------------------------------|-----------------------------|
| `SC_LOG_DEBUG(msg);`   | `[Debug]`         | `ScConsole::Color::Grey`      | Debug                       |
| `SC_LOG_INFO(msg);`    | `[Info]`          | `ScConsole::Color::LightBlue` | Debug, Info                 |
| `SC_LOG_WARNING(msg);` | `[Warning]`       | `ScConsole::Color::Yellow`    | Debug, Info, Warning        |
| `SC_LOG_ERROR(msg);`   | `[Error]`         | `ScConsole::Color::Red`       | Debug, Info, Warning, Error |




В настоящее время вы не можете добавлять уровни логирования. Но если вы хотите использовать другие префиксы или цвета в логировании, вы можете использовать `SC_LOG_INFO_COLOR(msg, color);`. Это выводит цветное информационное сообщение. Смотрите константы цветов в `ScConsole::Color`.

Уровень логирования можно настроить в конфигурационном файле `sc-machine.ini`. Измените параметр `log_level` в группе `[sc-memory]` на одно из значений из списка `[Debug, Info, Warning, Error]`. Посмотрите [пример конфигурационного файла](../../../../build/config.md), чтобы узнать больше о группах и их параметрах.

## **Расширенный API**

Описанные методы являются частью базового C++ API sc-memory. Вы можете ознакомиться с расширенным C++ API sc-memory и использовать его:

- [ScHelper C++ API](../extended/helper_api.md) to manipulate with system identifiers of sc-elements;
- [ScTemplate C++ API](../extended/template_api.md), providing functionality for creating, manipulating and retrieving large graph structures.

## **Часто Задаваемые Вопросы**
- [В чем разница между ScType::ConstCommonArc и ScType::ConstPermPosArc?](#в-чем-разница-между-sctypeconstcommonarc-и-sctypeconstpermposarc)
- [Как я могу указать пустой ScAddr?](#как-я-могу-указать-пустой-scaddr)

### **В чем разница между ScType::ConstCommonArc и ScType::ConstPermPosArc?**

`ScType::ConstCommonArc` — это sc-тип sc-дуги, который соединяет два sc-элемента в некотором отношении. `ScType::ConstPermPosArc` — это sc-тип sc-дуги, который обозначает принадлежность конечного sc-элемента к начальному sc-элементу. Sc-дуга с sc-типом `ScType::ConstCommonArc` между двумя sc-элементами может быть преобразована в sc-узел, к которому принадлежат эти два sc-элемента.

### **Как я могу указать пустой ScAddr?**

Пустой `ScAddr` - это sc-адрес, хэш которого равен 0.

```cpp
...
ScAddr addr;
// Здесь `addr` пуст.

myFunc(addr);
// или
myFunc(ScAddr::Empty);
...
```


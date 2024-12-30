# **ScTemplate API**

!!! Предупреждение
    Эта документация верна только для версий sc-machine, которые >= 0.10.0.
---

Sc-шаблоны - это очень мощный механизм для работы с семантической сетью (графом). С помощью sc-шаблонов вы можете генерировать любые конструкции и выполнять поиск по ним. На следующем рисунке показан sc-шаблон и изоморфная sc-конструкция.

<scg src="../images/templates/template_example_1.gwf"></scg>

!!! примечание
    Чтобы подключить этот API, укажите #include <sc-memory/sc_memory.hpp> в вашем исходном файле hpp.

## **ScTemplate**

Класс для работы с sc-шаблонами в C++ API. Перед прочтением этого абзаца вам необходимо ознакомиться с общей информацией о типах sc-элементов. **ССЫЛКА НА ФАЙЛ**

Будем использовать символы `f` для обозначения постоянного параметра sc-шаблона. Будем использовать символ `a` для обозначения переменного параметра sc-шаблона. Тогда sc-шаблон для поиска всех исходящих sc-коннекторов из указанного sc-элемента будет тройкой:

* где первый sc-элемент известен как `f`;
* второй и третий sc-элементы должны быть найдены как `a`.

Существует три возможных типа тройных sc-шаблонов:

* `f_a_a` — sc-шаблон для поиска всех исходящих sc-коннекторов из указанного sc-элемента;
* `f_a_f` — sc-шаблон для поиска всех sc-коннекторов между двумя указанными sc-элементами;
* `a_a_f` — sc-шаблон для поиска всех входящих sc-коннекторов в указанный sc-элемент.

Существует семь возможных типов пятиэлементных sc-шаблонов:

* `f_a_a_a_a` — sc-шаблон для поиска всех исходящих sc-коннекторов из указанного sc-элемента со всеми атрибутами этих sc-коннекторов;
* `f_a_f_a_a` — sc-шаблон для поиска всех sc-коннекторов между двумя указанными sc-элементами со всеми атрибутами этих sc-коннекторов;
* `f_a_a_a_f` — sc-шаблон для поиска всех исходящих sc-коннекторов из указанного sc-элемента с указанным атрибутом;
* `f_a_f_a_f` — sc-шаблон для поиска всех sc-коннекторов между двумя указанными sc-элементами с указанным атрибутом;
* `a_a_f_a_a` — sc-шаблон для поиска всех входящих sc-коннекторов в указанный sc-элемент со всеми атрибутами этих sc-коннекторов;
* `a_a_f_a_f` — sc-шаблон для поиска всех входящих sc-коннекторов в указанный sc-элемент с указанным атрибутом;
* `a_a_a_a_f` — sc-шаблон для поиска всех sc-коннекторов с указанным атрибутом.

Здесь атрибут — это sc-элемент, от которого sc-коннектор исходит к искомым sc-коннекторам.

Существуют некоторые методы, доступные для класса `ScTemplate`:

### **Triple**

Это метод, который добавляет тройную sc-конструкцию в sc-шаблон. Есть несколько примеров использования этой функции для создания простых sc-шаблонов:

<table>
  <tr>
    <th>Шаблон</th>
    <th>Описание</th>
  </tr>

  <tr>
    <td>f_a_a</td>
    <td>
      <strong>Графическое представление</strong>
      <br/><scg src="../images/templates/template_triple_f_a_a_example.gwf"></scg>
      <br/><strong>Соответствующий код на C++</strong>
      <br/>
<pre><code class="cpp">
ScTemplate templ;
templ.Triple(
  param1,
  ScType::VarPermPosArc,
  ScType::VarNode
);
</code></pre>
      <br/>Это трехэлементный sc-шаблон используется для обхода исходящих sc-коннекторов из указанного sc-элемента.
      <br/> <code>param1</code> - это известный sc-адрес sc-элемента. Он должен быть действительным (используйте метод <code>IsElement</code> для проверки). Параметры <code>_param2</code> и <code>_param3</code> представляют собой sc-типы, которые будут использоваться для сравнения в поисковом движке. Когда поисковый движок будет обходить исходящие sc-коннекторы из <code>param1</code>, в результат обхода будет добавлена конструкция, где исходящие sc-дуги из <code>param1</code> будет соответствовать указанному типу <code>_param2</code>, а тип конечного sc-элемента этой sc-дуги будет соответствовать типу <code>_param3</code>.
    <br/>Вы можете использовать любой sc-тип <code>_param3</code> (включая sc-коннекторы) в зависимости от sc-конструкции, которую вы хотите найти. Но <code>_param2</code> должен быть любым sc-типом переменного sc-коннектора.
    </td>
  </tr>



  <tr>
    <td>f_a_f</td>
    <td>
      <strong>Графическое представление</strong>
      <br/><scg src="../images/templates/template_triple_f_a_f_example.gwf"></scg>
      <br/><strong>Соответствующий код на C++</strong>
      <br/>
<pre><code class="cpp">
ScTemplate templ;
templ.Triple(
  param1,
  ScType::VarPermPosArc,
  param3
);
</code></pre>
      <br/> Этот тройной sc-шаблон используется для поиска sc-дуги между <code>param1</code> и <code>param3</code>.
      <br/>Существуют <code>param1</code> и <code>param3</code>, известные <code>ScAddr</code> sc-элементов. Тип Arc <code>_param2</code> должен быть переменным.
    </td>
  </tr>
  <tr>
    <td>a_a_f</td>
    <td>
      <strong>Графическое представление</strong>
      <br/><scg src="../images/templates/template_triple_a_a_f_example.gwf"></scg>
      <br/><strong>Соответствующий код на C++</strong>
      <br/>
<pre><code class="cpp">
ScTemplate templ;
templ.Triple(
  ScType::VarNode,
  ScType::VarPermPosArc,
  param3
);
</code></pre>
      <br/>Этот трехэлементный sc-шаблон используется для перемещения входящих sc-коннекторов к указанному sc-элементу.
      <br/>Здесь <code>param3</code> - это известный sc-адрес sc-элемента. Вы можете использовать любой тип <code>_param1</code> (включая sc-коннекторы) в зависимости от конструкции, которую вы хотите найти. Но <code>_param2</code> должен быть переменным коннектором любого типа.
    </td>
  </tr>
</table>

### **Quintuple**

Это метод, который добавляет пятиэлементную sc-конструкцию в sc-шаблон. Есть несколько примеров использования этой
функции для создания простых sc-шаблонов:

<table>
  <tr>
    <th>Шаблон</th>
    <th>Описание</th>
  </tr>

  <tr>
    <td>f_a_a_a_a</td>
    <td>
      <strong>Графическое представление</strong>
      <br/><scg src="../images/templates/template_quintuple_f_a_a_a_a_example.gwf"></scg>
      <br/><strong>Соответствующий код на C++</strong>
      <br/>
<pre><code class="cpp">
ScTemplate templ;
templ.Quintuple(
  param1,
  ScType::VarPermPosArc,
  ScType::VarNode,
  ScType::VarPermPosArc,
  ScType::VarNode
);
</code></pre>
    </td>
  </tr>

  <tr>
    <td>f_a_f_a_a</td>
    <td>
      <strong>Графическое представление</strong>
      <br/><scg src="../images/templates/template_quintuple_f_a_f_a_a_example.gwf"></scg>
      <br/><strong>Соответствующий код на C++</strong>
      <br/>
<pre><code class="cpp">
ScTemplate templ;
templ.Quintuple(
  param1,
  ScType::VarPermPosArc,
  param3,
  ScType::VarPermPosArc,
  ScType::VarNode
);
</code></pre>
    </td>
  </tr>

  <tr>
    <td>f_a_a_a_f</td>
    <td>
      <strong>Графическое представление</strong>
      <br/><scg src="../images/templates/template_quintuple_f_a_a_a_f_example.gwf"></scg>
      <br/><strong>Соответствующий код на C++</strong>
      <br/>
<pre><code class="cpp">
ScTemplate templ;
templ.Quintuple(
  param1,
  ScType::VarPermPosArc,
  ScType::VarNode,
  ScType::VarPermPosArc,
  param5
);
</code></pre>
    </td>
  </tr>

  <tr>
    <td>f_a_f_a_f</td>
    <td>
      <strong>Графическое представление</strong>
      <br/><scg src="../images/templates/template_quintuple_f_a_f_a_f_example.gwf"></scg>
      <br/><strong>Соответствующий код на C++</strong>
      <br/>
<pre><code class="cpp">
ScTemplate templ;
templ.Quintuple(
  param1,
  ScType::VarPermPosArc,
  param3,
  ScType::VarPermPosArc,
  param5
);
</code></pre>
    </td>
  </tr>

  <tr>
    <td>a_a_f_a_a</td>
    <td>
      <strong>Графическое представление</strong>
      <br/><scg src="../images/templates/template_quintuple_a_a_f_a_a_example.gwf"></scg>
      <br/><strong>Соответствующий код на C++</strong>
      <br/>
<pre><code class="cpp">
ScTemplate templ;
templ.Quintuple(
  ScType::VarNode,
  ScType::VarPermPosArc,
  param3,
  ScType::VarPermPosArc,
  ScType::VarNode
);
</code></pre>
    </td>
  </tr>

  <tr>
    <td>a_a_f_a_f</td>
    <td>
      <strong>Графическое представление</strong>
      <br/><scg src="../images/templates/template_quintuple_a_a_f_a_f_example.gwf"></scg>
      <br/><strong>Соответствующий код на C++</strong>
      <br/>
<pre><code class="cpp">
ScTemplate templ;
templ.Quintuple(
  ScType::VarNode,
  ScType::VarPermPosArc,
  param3,
  ScType::VarPermPosArc,
  param5
);
</code></pre>
    </td>
  </tr>

  <tr>
    <td>a_a_a_a_f</td>
    <td>
      <strong>Графическое представление</strong>
      <br/><scg src="../images/templates/template_quintuple_a_a_a_a_f_example.gwf"></scg>
      <br/><strong>Соответствующий код на C++</strong>
      <br/>
<pre><code class="cpp">
ScTemplate templ;
templ.Quintuple(
  ScType::VarNode,
  ScType::VarPermPosArc,
  ScType::VarNode,
  ScType::VarPermPosArc,
  param5
);
</code></pre>
    </td>
  </tr>
</table>

Когда поисковая система sc-template работает, она пытается просмотреть граф по простому (трехэлементному или пятиэлементному) sc-шаблону в указанном порядке. 
Например, нам нужно проверить, включен ли указанный sc-элемент (`_set`) в классы `concept_set` и `concept_binary_relation`:

<scg src="../images/templates/template_example_2.gwf"></scg>

Вот пример кода таких шаблонов.

```cpp
...
// Найдите ключевые понятия, которые следует использовать в sc-шаблоне.
ScAddr const & conceptSetAddr = context.SearchElementBySystemIdentifier("concept_set");
ScAddr const & conceptBinaryRelationAddr 
    = context.SearchElementBySystemIdentifier("concept_binary_relation");

// Сгенерируйте sc-шаблон и добавьте тройку элементов в этот sc-шаблон.
ScTemplate templ;
templ.Triple(
  conceptSetAddr,    // sc -адрес узла concept set
  ScType::VarPermPosArc,
  ScType::VarNode >> "_set"
);
templ.Triple(
  conceptBinaryRelationAddr,    // sc -адрес узла concept binary relation
  ScType::VarPermPosArc,
  "_set"
);
..
```

В коде вы можете увидеть конструкцию `ScType::VarNode >> _set` - это имя для sc-элемента sc-шаблона.
Это позволяет задать псевдоним для указанного sc-элемента в sc-шаблоне и использовать его много раз в разных тройках. Вы можете видеть, что во второй тройке мы используем этот псевдоним ""_set"`. Это означает, что нам нужно поместить результат поиска из первой тройки во вторую. Итак, вторая тройка - это тройка в стиле `f_a_f`.


Итак, если вы хотите использовать один и тот же sc-элемент `_x` в разных тройках, и вы не знаете, что это за `ScAddr`, тогда просто используйте два
основных правила:

* Задайте псевдоним этому sc-элементу при первом появлении этого sc-элемента в тройках sc-шаблона. Для этого вам нужно использовать оператор `>>`
(смотрите код ниже, последний sc-элемент первой тройки).
* Если вам нужно использовать псевдоним sc-элемента в следующих тройках, просто используйте его псевдоним вместо `ScType` или `ScAddr` (смотрите код
ниже, первый sc-элемент второй тройки).

Вот пример кода с именованием(псевдонимом).

```cpp
...
ScTemplate templ;
templ.Triple(
  anyAddr, // sc -адрес известного sc-элемента
  ScType::VarPermPosArc,  // тип неизвестной sc-дуги
  ScType::VarNode >> `_x`  // тип и псевдоним для неизвестного sc-элемента
);
templ.Triple(
  `_x`,  // скажем, это тот же sc-элемент, что и последний в предыдущей тройке
  ScType::VarPermPosArc,  // тип неизвестной sc-дуги
  ScType::VarNode  // тип неизвестного sc-узла
);
...
```

Внутри программного объекта sc-шаблона все его конструкции представлены в виде троек.

### **HasReplacement**

Чтобы проверить, что sc-шаблон содержит псевдоним sc-элемента, вы можете использовать метод `HasReplacement`.

```cpp
...
ScTemplate templ;
templ.Triple(
  anyAddr,
  ScType::VarPermPosArc,
  ScType::VarNode >> `_x`
);
bool const hasAliasX = templ.HasReplacement(`_x`);
// Значение `hasAliasX` должно быть равно `true`.
...
```

### **Size**

Чтобы получить количество троек в sc-шаблоне, используйте метод `Size`. Это может быть полезно, если ваша программа может выбрать оптимальный для работы sc-шаблон.

```cpp
ScTemplate templ;
templ.Triple(
  anyAddr, // sc-address of known sc-element
  ScType::VarPermPosArc,  // type of unknown sc-arc
  ScType::VarNode >> `_x`  // type and alias for an unknown sc-element
);
templ.Triple(
  `_x`,  // say that is the same sc-element as the last on in a previous triple
  ScType::VarPermPosArc,  // type of unknown sc-arc
  ScType::VarNode  // type of unknown sc-node
);

size_t const tripleCount = templ.Size();
// Количество троек должно быть рано `2`.
...
```

### **IsEmpty**

Если вам нужно, чтобы sc-шаблон был пустым, вам не нужно добавлять в него какие-либо конструкции. Но вы должны знать
, что результат генерации по этому sc-шаблону всегда `true`, а результат поиска по этому sc-шаблону всегда
`false`. Чтобы проверить, что sc-шаблон пуст, используйте метод `isEmpty`.

```cpp
...
ScTemplate templ;
bool const isEmpty = templ.IsEmpty();
// Значение `isEmpty` должно быть равно `true`.
...
```

## **ScTemplateBuild**

Кроме того, вы можете создавать sc-шаблоны, используя [SCs-code](../../../../scs/scs.md).

```cpp
...
// Напишите свой sc-шаблон на SCs-коде.
sc_char const * data = 
  "_set"
  "  _<- concept_set;"
  "  _<- concept_binary_set;;";

// Создайте программный объект по этому sc-шаблону.
ScTemplate templ;
context.BuildTemplate(templ, data);
...
```

!!! примечание
    Не используйте результирующее значение, оно ничего не значит.

При построении sc-шаблона все константы будут определены по их системному идентификатору (
например, `concept_set`, `concept_binary_set`), поэтому в результате `templ` будет содержать sc-шаблон:

<scg src="../images/templates/template_example_2.gwf"></scg>

Или вы можете сделать это, указав действительный sc-адрес некоторого sc-шаблона в sc-памяти.

```cpp
...
// Найдите по системному идентификатору ваш sc-шаблон в sc-памяти.
ScAddr const & templAddr = context.SearchElementBySystemIdentifier("my_template");

// Создайте программный объект по этому sc-шаблону.
ScTemplate templ;
context.BuildTemplate(templ, templAddr);
...
```

!!! примечание
    Не используйте результирующее значение, оно ничего не значит.

## **ScTemplateParams**

Вы можете заменить существующие sc-переменные в sc-шаблонах на свои собственные. Для обеспечения различных замен sc-переменных в разных случаях существует класс `ScTemplateParams`. Он хранит связь между sc-переменными и указанными значениями (заменами).

```cpp
...
ScTemplateParams params;
...
```

### **Add**

Вы можете добавить замену для sc-переменной, указав системный идентификатор этой sc-переменной, если sc-шаблон создан на основе SCs-кода.

```cpp
...
// Опишите свой sc-шаблон в SCs-коде.
sc_char const * data = 
  "_set"
  "  _<- concept_set;"
  "  _<- concept_binary_set;;";

// Сгенерировать замену в sc-памяти.
ScAddr const & setAddr = context.GenerateNode(ScType::ConstNode);
// Также вы можете найти какую-нибудь замену из sc-memory.

// Определите замены для sc-переменных в sc-шаблоне.
ScTemplateParams params;
params.Add("_set", setAddr);

// Создайте программный объект по этому sc-шаблону, указав замены.
ScTemplate templ;
context.BuildTemplate(templ, data, params);
...
```

Или вы можете добавить замену для sc-переменной, указав sc-адрес этой sc-переменной, если sc-шаблон создан на основе
sc-адреса sc-структуры в sc-памяти.

```cpp
...
// Найдите по системному идентификатору ваш sc-шаблон в sc-памяти.
ScAddr const & templAddr = context.SearchElementBySystemIdentifier("my_template");

// Найдите по системному идентификатору sc-адрес sc-переменной в вашем sc-шаблоне.
ScAddr const & setVarAddr = context.SearchElementBySystemIdentifier("_set");

// Сгенерировать замену в sc-памяти.
ScAddr const & setAddr = context.GenerateNode(ScType::ConstNode);
// Также вы можете найти какую-нибудь замену из sc-memory.

// Определите замены для sc-переменных в sc-шаблоне.
ScTemplateParams params;
params.Add(setVarAddr, setAddr);

// Создайте программный объект по этому sc-шаблону, указав замены.
ScTemplate templ;
context.BuildTemplate(templ, templAddr, params);
...
```

### **Get**

Иногда вам нужно проверить, есть ли замена в параметрах. Вы можете сделать это, используя метод `Get`. Он также работает с
системными идентификаторами и sc-адресами sc-переменных.

```cpp
...
// Сгенерировать замену в sc-памяти.
ScAddr const & setAddr = context.GenerateNode(ScType::ConstNode);
// Также вы можете найти какую-нибудь замену из sc-memory.

// Определите замены для sc-переменных в sc-шаблоне.
ScTemplateParams params;
params.Add("_set", setAddr);

ScAddr const & replAddr = params.Get("_set");
// Значение `replAddr` должно быть равно значению `setAddr`.
...
```

```cpp
...
// Найдите по системному идентификатору sc-адрес sc-переменной в вашем sc-шаблоне.
ScAddr const & setVarAddr = context.SearchElementBySystemIdentifier("_set");

// Сгенерировать замену в sc-памяти.
ScAddr const & setAddr = context.GenerateNode(ScType::ConstNode);
// Также вы можете найти какую-нибудь замену из sc-memory.

// Определите замены для sc-переменных в sc-шаблоне.
ScTemplateParams params;
params.Add(setVarAddr, setAddr);

ScAddr const & replAddr = params.Get(setVarAddr);
// Значение `replAddr` должно быть равно значению `setAddr`.
...
```

!!! примечание
    Если нет замен по указанному системному идентификатору или sc-адресу sc-переменной sc-шаблона, то
    метод `Get` вернет пустой sc-адрес.

### **IsEmpty**

Чтобы проверить, что набор замен пуст, используйте метод `IsEmpty`.

```cpp
...
ScTemplateParams params;
bool const isEmpty = params.IsEmpty();
// Значение `isEmpty` должно быть равно `true`.
...
```

## **GenerateByTemplate**

Используйте sc-шаблон для создания графов в sc-памяти и получения замен из результата.

```cpp
...
// Укажите sc-шаблон для поиска sc-конструкций в sc-памяти.
// Вы можете использовать методы `ScTemplate` или метод ScTemplateBuild для преобразования 
// sc-шаблона из SCs-кода или sc-памяти в программное представление.
ScTemplateResultItem result;
context.GenerateByTemplate(templ, result);
// Sc-адреса sc-элементов сгенерированной sc-конструкции могут быть получены из 
// `result`.
...
```

!!! примечание 
    Помните, что sc-шаблон должен содержать только действительные sc-адреса sc-элементов, а все sc-коннекторы в нем должны 
    быть sc-переменными.
    В противном случае этот метод может выдать `utils::ExceptionInvalidParams` с описанием этой ошибки.

## **ScTemplateResultItem**

Это класс, который хранит информацию о sc-конструкции.

### **Safe Get**

Вы можете получить sc-адреса sc-элементов сгенерированной sc-конструкции по системному идентификатору или sc-адресу sc-переменной в sc-шаблоне. Чтобы сделать это безопасно (без выбрасывания исключений, если нет замен по указанному системному идентификатору или sc-адресу sc-переменной sc-шаблона), используйте методы `Get` и укажите результат замены в качестве параметра out в этом методе.

```cpp
...
ScTemplate templ;
templ.Triple(
  conceptSetAddr,
  ScType::VarPermPosArc,
  ScType::VarNode >> "_x"
);

ScTemplateResultItem result;
context.GenerateByTemplate(templ, result);

ScAddr setAddr;
bool replExist = result.Get("_x", setAddr);
// Значение `replExist` должно быть равно `true`.

bool replExist = result.Get("_y", setAddr);
// Значение `replExist` должно быть равно `false`.
...
```

### **Get**

Если вы хотите перехватывать исключения, если нет замен по указанному системному идентификатору или sc-адресу sc-переменной
sc-шаблона, используйте метод `Get` и получите замену в результате этого метода. Тогда этот метод выдаст 
`utils::ExceptionInvalidParams` с описанием ошибки.

```cpp
...
ScTemplate templ;
templ.Triple(
  conceptSetAddr,
  ScType::VarPermPosArc,
  ScType::VarNode >> "_x"
);

ScTemplateResultItem result;
context.GenerateByTemplate(templ, result);

ScAddr setAddr = result.Get("_x");

setAddr = result.Get("_y", setAddr);
// Это вызовет исключение "utils::ExceptionInvalidParams`.
...
```

### **Has**

Чтобы проверить, есть ли замена на указанный системный идентификатор или sc-адрес sc-переменной sc-шаблона, используйте
метод `Has`.

```cpp
...
ScTemplate templ;
templ.Triple(
  conceptSetAddr,
  ScType::VarPermPosArc,
  ScType::VarNode >> "_x"
);

ScTemplateResultItem result;
context.GenerateByTemplate(templ, result);

bool const replExist = result.Has("_x");
// The value of `replExist` be equal to `true`.
...
```

### **operator[]**

Чтобы получить все замены из результата, вы можете использовать `operator[]`. Он возвращает замену по индексу sc-переменной в sc-шаблоне. Если нет sc-переменной с указанным индексом, этот метод выдаст исключение `utils::ExceptionInvalidParams` с описанием ошибки.

```cpp
...
ScTemplate templ;
templ.Triple(
  conceptSetAddr,
  ScType::VarPermPosArc,
  ScType::VarNode >> "_x"
);

ScTemplateResultItem result;
context.GenerateByTemplate(templ, result);

ScAddr const & setAddr = result[2];
// Оно равно `result.Get("_x")`.
...
```

### **Size**

Если вы хотите перебрать все замены в результате, вам нужно знать размер этого результата.

```cpp
...
ScTemplate templ;
templ.Triple(
  conceptSetAddr,
  ScType::VarPermPosArc,
  ScType::VarNode >> "_x"
);

ScTemplateResultItem result;
context.GenerateByTemplate(templ, result);

// Выполнить итерацию по всем заменам в результате.
for (size_t i = 0; i < result.Size(); ++i)
{
  ScAddr const & addr = result[i];
// Реализуйте свой код для обработки замен.
}
...
```

!!! примечание
    Метод `Size` возвращает итоговое количество индексов замен в каждой тройке в sc-шаблоне. Если есть 
    `2` тройки в sc-шаблоне, тогда в sc-шаблоне есть `2 * 3 = 6` разных индексов замен.

## **SearchByTemplate**

Вы можете выполнить поиск sc-конструкции в sc-памяти при помощи sc-шаблонов. Этот поиск относится к изоморфному поиску по графовому шаблону. Алгоритм поиска пытается найти все возможные варианты указанной конструкции. Он использует любые константы (доступные sc-адреса из параметров для поиска эквивалентных sc-конструкций в sc-памяти).

```cpp
...
// Укажите sc-шаблон для поиска sc-конструкций в sc-памяти.
// Вы можете использовать методы `ScTemplate` или метод ScTemplateBuild для преобразования 
// sc-шаблона из SCs-кода или sc-памяти в программное представление.
ScTemplateSearchResult result;
bool const isFoundByTemplate = context.SearchByTemplate(templ, result);
// Программное представление sc-конструкций в `ScTemplateResultItem` 
// может быть получено из `result`.
...
```

!!! примечание 
    Помните, что sc-шаблон должен содержать только допустимые sc-адреса sc-элементов, а все sc-коннекторы в нем должны быть
    sc-переменными. В противном случае этот метод может выдать `utils::ExceptionInvalidParams` с описанием этой ошибки.

## **ScTemplateSearchResult**

Это класс, который хранит информацию о sc-конструкциях, представленных в `ScTemplateResultItem`.
Объект класса `ScTemplateSearchResult` может быть отнесен к вектору объектов класса `ScTemplateResultItem`.

### **Safe Get**

Чтобы получить объект класса "ScTemplateResultItem", вы можете использовать метод "Get". Если вы хотите безопасно получать объекты, используйте метод "Get` и укажите "ScTemplateResultItem` в качестве параметра out в этом методе.

```cpp
...
ScTemplate templ;
templ.Triple(
  conceptSetAddr,
  ScType::VarPermPosArc,
  ScType::VarNode >> "_x"
);
// Существует одна sc-конструкция, которая изоморфна этому sc-шаблону.

ScTemplateSearchResult result;
bool const isFoundByTemplate = context.SearchByTemplate(templ, result);

ScTemplateResultItem item;
bool constrExist = result.Get(0, item);
// Значение `constrExist` должно быть равно `true`.

constrExist = result.Get(1, item);
// Значение `constrExist` должно быть равно `false`, и элемент не является действительным.
...
```

### **Get**

Если вы хотите перехватывать исключения, используйте метод `Get` и получите результат в качестве возвращаемого значения. Если нет sc-конструкции с указанным индексом, этот метод выдаст исключение utils::ExceptionInvalidParams с описанием ошибки.

```cpp
...
ScTemplate templ;
templ.Triple(
  conceptSetAddr,
  ScType::VarPermPosArc,
  ScType::VarNode >> "_x"
);
// Существует одна sc-конструкция, которая изоморфна этому sc-шаблону.

ScTemplateSearchResult result;
bool const isFoundByTemplate = context.SearchByTemplate(templ, result);

ScTemplateResultItem item = result.Get(0);
// Это действительный элемент.

item = result.Get(1);
// Он выдает `utils::ExceptionInvalidParams`.
...
```

### **operator[]**

Также для этого можно использовать метод `operator[]`. Если нет sc-конструкции с указанным индексом, этот метод
выдаст исключение utils::ExceptionInvalidParams с описанием ошибки.

```cpp
...
ScTemplate templ;
templ.Triple(
  conceptSetAddr,
  ScType::VarPermPosArc,
  ScType::VarNode >> "_x"
);
// Существует одна sc-конструкция, которая изоморфна этому sc-шаблону.

ScTemplateSearchResult result;
bool const isFoundByTemplate = context.SearchByTemplate(templ, result);

ScTemplateResultItem item = result[0];
// Это действительный элемент.

item = result[1];
// Он выдает `utils::ExceptionInvalidParams`.
...
```

### **Size**

Чтобы получить количество найденных sc-конструкций по sc-шаблону, вы можете использовать метод `Size`.

```cpp
...
ScTemplate templ;
templ.Triple(
  conceptSetAddr,
  ScType::VarPermPosArc,
  ScType::VarNode >> "_x"
);
// Существует одна sc-конструкция, которая изоморфна этому sc-шаблону.

ScTemplateSearchResult result;
bool const isFoundByTemplate = context.SearchByTemplate(templ, result);

size_t const count = result.Size();
// Значение параметра `count" должно быть равно `1`.
...
```

### **IsEmpty**

Чтобы проверить, является ли найденный результат пустым, используйте метод `isEmpty`.

```cpp
...
ScTemplate templ;
templ.Triple(
  conceptSetAddr,
  ScType::VarPermPosArc,
  ScType::VarNode >> "_x"
);
// Существует одна sc-конструкция, которая изоморфна этому sc-шаблону.

ScTemplateSearchResult result;
bool const isFoundByTemplate = context.SearchByTemplate(templ, result);

bool const count = result.IsEmpty();
// Значение `count` должно быть равно `false`.
...
```

### **Clear**

Чтобы очистить всю информацию о найденных sc-конструкциях, используйте метод "Очистить`.

```cpp
...
ScTemplate templ;
templ.Triple(
  conceptSetAddr,
  ScType::VarPermPosArc,
  ScType::VarNode >> "_x"
);
// Существует одна sc-конструкция, которая изоморфна этому sc-шаблону.

ScTemplateSearchResult result;
bool const isFoundByTemplate = context.SearchByTemplate(templ, result);

result.Clear();
// После этого "результат" не содержит никакой информации о sc-конструкциях.
...
```

### **ForEach**

Чтобы перебрать все объекты программы, найденные в sc-конструкциях по sc-шаблону, вы можете использовать цикл for-each.

```cpp
...
ScTemplate templ;
templ.Triple(
  conceptSetAddr,
  ScType::VarPermPosArc,
  ScType::VarNode >> "_x"
);
// Существует одна sc-конструкция, которая изоморфна этому sc-шаблону.

ScTemplateSearchResult result;
bool const isFoundByTemplate = context.SearchByTemplate(templ, result);

for (size_t i = 0; i < result.Size(); ++i)
{
  ScTemplateResultItem const & item = result.Get(i);
// Реализовать логику для обработки найденных sc-конструкций.
}
...
```

Или вы можете использовать метод `ForEach` для этого.

```cpp
...
ScTemplate templ;
templ.Triple(
  conceptSetAddr,
  ScType::VarPermPosArc,
  ScType::VarNode >> "_x"
);
// Существует одна sc-конструкция, которая изоморфна этому sc-шаблону.

ScTemplateSearchResult result;
bool const isFoundByTemplate = context.SearchByTemplate(templ, result);

result.ForEach([](ScTemplateResultItem const & item) {
  // Реализовать логику для обработки найденных sc-конструкций.
});
...
```

## **SearchByTemplateInterruptibly**

Этот метод ищет конструкции по изоморфному sc-шаблону и передает найденные sc-конструкции в `callback` лямбда-функцию. `callback` лямбда-функция должна возвращать значение команды запроса для управления поиском sc-шаблона:

- ScTemplateSearchRequest::CONTINUE,
- ScTemplateSearchRequest::STOP,
- ScTemplateSearchRequest::ERROR.

Когда возвращается ScTemplateSearchRequest::CONTINUE, поиск sc-шаблона будет продолжен. Если возвращается ScTemplateSearchRequest::STOP или ScTemplateSearchRequest::ERROR, то поиск sc-шаблона останавливается. Если поиск sc-шаблона остановлен из-за ScTemplateSearchRequest::ERROR, то выбрасывается исключение utils::ExceptionInvalidState. Если передан `filterCallback`, то все найденные тройки sc-конструкций фильтруются по условию `filterCallback`.

```cpp
...
ScAddr const & structureAddr = context.SearchElementBySystemIdentifier("my_structure");
ScAddr const & setAddr = context.SearchElementBySystemIdentifier("my_set");
ScAddr const & classAddr = context.SearchElementBySystemIdentifier("my_class");

ScTemplate templ;
templ.Triple(
  classAddr,
  ScType::VarPermPosArc >> "_arc",
  ScType::Unknown >> "_addr2"
);
m_context->SearchByTemplateInterruptibly(templ, [&context](
    ScTemplateSearchResultItem const & item) -> ScTemplateSearchRequest 
{
  ScAddr const & arcAddr = item["_arc"];
  if (context->CheckConnector(
      structureAddr, arcAddr, ScType::ConstPermPosArc))   
    return ScTemplateSearchRequest::CONTINUE;

  if (context.GenerateConnector(
      ScType::ConstTempPosArc, setAddr, item["_addr2"]))
    return ScTemplateSearchRequest::STOP;

  return ScTemplateSearchRequest::ERROR;
});
...
```

--- 

## **Часто задаваемые вопросы**

- [Как лучше всего описать sc-шаблоны для поиска? С помощью sc-template C++ API или с помощью SC-кода?](#what-is-the-best-way-to-describe-sc-templates-for-search-by-sc-template-c-api-or-on-the-sc-code)
- [Что лучше: поиск по sc-шаблону или по итератору?](#which-is-better-searching-by-sc-template-or-by-iterator)

### **Как лучше всего описать sc-шаблоны для поиска? С помощью sc-template C++ API или с помощью SC-кода?**

Описание sc-шаблонов в базе знаний поощряет использование для них рефлексии. Sc-шаблоны, описанные в базе знаний, могут быть легко расширены и улучшены. Однако sc-шаблоны, представленные через API не требуют предварительной обработки (перевода из базы знаний), поэтому скорость работы программы, используемой такими sc-шаблонами, может быть выше, если sc-шаблоны имеют значительный размер.

### **Что лучше: поиск по sc-шаблону или по итератору?**

Во всех случаях sc-итератор ускоряет поиск по sc-шаблону. Если вы хотите выполнить поиск по большим sc-конструкциям, то не
выполняйте поиск по одному большому sc-шаблону, разделите его на определенные sc-шаблоны или используйте sc-итераторы вместо sc-шаблона.

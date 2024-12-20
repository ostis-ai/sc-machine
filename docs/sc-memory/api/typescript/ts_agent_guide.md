<!-- создать вход конструкцию в ims 
функция callback 
функция найти some set ч/з rrel_1
функция calculateSetPower
функция выходной конструкции добавить action_finished_successfully -->

# **Реализация агента на TypeScript**

!!! note
    Руководство актуально только для sc-machine версии 0.10.0 и больше.
--- 


Агенты в TypeScript создаются преимущественно при работе с sc-memory через сеть. Рассмотрим создание агента на примере TypeScript агентов ostis-системы NIKA.
Для реализации агента в TypeScript, необходимо выполнить следующую последовательность шагов:

1. Укажите входную (инициализирующую) конструкцию и выходную (результирующую) конструкцию Вашего будущего агента в SC-коде.
2. Создайте файл `client.ts` и создайте экземпляр класса `ScClient` (клиента sc-server).
3. Создайте файл агента и укажите функции, реализующие программму работы агента.
4. Реализуйте основную программу Вашего агента.
5. Зарегистрируйте агента. Создание функции установления условия реакции агента на sc-событие в `client.ts`.

---

Рассмотрим пример реализации агента для подсчета мощности данного множества:

### **1. Опишите входные и выходные конструкции Вашего агента.**

Первоначальная конструкция для вызова агента может выглядеть так:

```scs
..action
<- get_set_power_action;
<- action_initiated;
-> rrel_1: ..some_set;;

..some_set
-> ..element_1;
-> ..element_2;
-> ..element_3;;
```

<image src="../images/get_set_power_agent_input_construction.png", width="350", height="350"></image>

Конструкция результата может быть представлена так:

```scs
..some_action
=> nrel_result: [*
  ..some_set => nrel_set_power: [3];;
*];;
```

<image src="../images/get_set_power_agent_output_construction.png", width="350", height="350"></image>

--- 

### **2. Создайте файл `client.ts` и создайте экземпляр класса `ScClient` (клиента sc-server).**

По умолчанию `sc-machine` содержит `sc-server`, т. е. способ взаимодействия с базой знаний через сеть.
Для взаимодействия с ней используется класс `ScClinet`, являющимся составляющей `ts-sc-client`. Данный класс предоставляет множественные методы для работы с sc-server. Для того, чтобы использовать методы, требуется создать экземпляр класса `ScClinet`.
Более подробную информацию о ts-sc-client и его методах и классах см. [в этом разделе](https://github.com/ostis-ai/ts-sc-client?tab=readme-ov-file).

Для начала создайте файл `client.ts`, где будет создан экземпляр класса `ScClinet` и, соответственно, зарегистрирован агент.

Рекомендуется размещать файлы агента примерно следующим образом:

```plaintext
interface/
├──api/
│   ├──sc/
│   │   ├──client.ts
│   │   └──agents/
```

**client.ts**

```typescript
import { SC_URL } from '@constants';
// SC_URL: в ostis-системе NIKA "SC_URL" является описанный заранее WebSocket
//localhost адрес базы знаний ('ws://localhost:8090')
// Однако адрес можно указать явно при создании объекта ts-sc-client
import { ScClient,ScType,ScEventParams,ScEventType } from 'ts-sc-client';
// Импортирование класса ScClient и необходимых методов для регистрации агента, 
//a это: типы sc-элементов (ScType), параметры регистрации агента (ScEventParams)
```

---

### **3. Создайте файл агента и проверьте, инициализирован ли Ваш агент.**

Для начала работы следует указать функцию, проверяющую, принадлежит ли инициированное действие классу Вашего агента. Данная функция будет вызываться в основной функции при регистрация агента. Рассмотрим самое распространненное sc-событие, на которое реагирует агент - проведение дуги от узла класса инициированного дейсвия.
Класс инициированного действия существует в базе знаний с идентификатором `action_initiated`. Используйте его для таких событий, как проведение дуги к узлу действия (в примере входной конструкции это узел `some_action`)



Создадим файл агента: 

```diff
 interface/
 ├──api/
 │   ├──sc/
 │   │   ├──client.ts
 │   │   ├──agents/
+│   │   │   └──agent.ts
```

**agent.ts**

```typescript
import { client } from '@api/sc';
//Подключение созданного ранее файла
import { ScAddr, ScConstruction, ScLinkContent, ScTemplate, ScType,
ScLinkContentType } from 'ts-sc-client';
//Импортирование требуемых классов и методов


//Для начала следует создать функцию проверки, которая будет вызываться при
//реакции агента на sc-событие
//в качестве аргументов нам понадобится адрес узла конкретного 
//инициализированного действия (action) и список  ключевых sc-элементов,
// которые этот агент использует во время выполнения своей программы (keynodes).
// Подробнее о keynodes будет будет рассказано в разделе 4.
export const check = async(action: ScAddr,keynodes: Record<string, ScAddr>) => {
  //Создание ScTemplate для последующего поиска по шаблону.
    const checkTemplate = new ScTemplate();
    checkTemplate.triple(                        // класс_Вашего_агента
        keynodes["get_set_power_action"],        //    |
        //Или любое другое название                    |
        // класса Вашего агента                        |
        ScType.EdgeAccessVarPosPerm,             //   \/
        action,                                  // действие
    );
    const checkResult = await client.templateSearch(checkTemplate);
    if (checkResult.length==0) 
    //Проверяется, найдена ли данная структура в базе знаний
        {
            return false;

        }
    return true;
}

```

Данная функция реализовывает поиск по шаблону для определения, принадлежит ли узел инициированного действия (`action`) классу Вашего агента.

В процессе создания `checkTemplate` используются переменные, существование которых требуется проверить. Функция `templateSearch`
ищет данную конструкцию с константными элементами и, при нахождении, возвращает в переменную (См. [документацию ts-sc-client](https://github.com/ostis-ai/ts-sc-client?tab=readme-ov-file)).


---

### **4. Реализуйте основную программу Вашего агента.**

Теперь, когда Вы убедились, что именно Ваш агент инициализирован, он должен выполнить все предназначенные ему действия.
Давайте реализуем функцию агента, которая будет выполнять роль основной программы и подаваться в качестве аргумента при регистрации агента.

Для начала следует определить ключевые узлы, с которыми взаимодействует агент, найти их в базе знаний и интегрировать их в программу.
Для каждого агента можно указать ключевые sc-элементы, которые этот агент использует во время выполнения своей программы. Эти ключевые sc-элементы — это sc-элементы, которые агент не генерирует, а использует в процессе поиска или создания связей между сущностями в базе знаний. Ключевые sc-элементы называются keynodes.

!!! warning
    Sc-keynode элемент нельзя создать без системного идентификатора.
    
!!! warning
    Все ключевые узлы должны быть статическими объектами. Это значит, что ключевые узлы требуется определять как статические объекты везде (не только в классах).
    

**agent.ts**

```typescript
//Определим sc-keynodes в основной функции "getSetPowerAgent", которая позже
// будет подаваться в качестве аргумента при регистрации агента.
export const getSetPowerAgent = async(subscribedAddr: ScAddr, foundEdge: ScAddr,
actionNode: ScAddr) => {
//В случае, когда агент реагирует на такое событие как провдение sc-дуги, 
//основная функция принимает в каечтсве аргументов узел, от которого проведен 
//sc-коннектор (событие), sc-коннектор и узел действия

    const getSetPowerAction = "get_set_power_action";

    //Для начала укажите класс вашего агента
    const action = 'action'; 
    //узел действия, которое к которому проводиться коннектор 
    //как одно из sc-событий.

    const actionInitiated = 'action_initiated';
    //Класс инициированного действия
    //Требуется для определения инициализации действия

    const actionFinished = 'action_finished';
    //Узел, указывающий о завершении агента.
    //От узла проводится коннектор как событие завершения агента

    //Агент может завершиться успешно (actionFinishedSuccessfully) при 
    //удачном решении делегированной ему задачи
    //агент может завершиться неуспешно (actionFinishedUnsuccessfully),
    //это означает, что делегированная задача не была решена
    const actionFinishedSuccessfully = 'action_finished_successfully';
    const actionFinishedUnsuccessfully = 'action_finished_unsuccessfully';

    const nrelResult = 'nrel_result';
    //'nrel_result' узел требуется для построения выходной конструкции
    //в последующем и указании результат работы агента
    const nrelSetPower="nrel_set_power";
    //Неролевое отношение, указывающее мощность множества

    const rrel1 = 'rrel_1';
    //Ролевое отношение 'rrel_1' служит для указания аргумента, 
    //передающегося агенту.

    const baseKeynodes = [
        { id: getSetPowerAction, type: ScType.NodeConstClass },
        { id: rrel1, type: ScType.NodeConstRole },
        { id: nrelResult, type: ScType.NodeConstNoRole },
        { id: nrelSetPower, type: ScType.NodeConstNoRole },
        { id: actionFinished, type: ScType.NodeConstClass },
        { id: actionFinishedSuccessfully, type: ScType.NodeConstClass },
        { id: actionFinishedUnsuccessfully, type: ScType.NodeConstClass },
        { id: action, type: ScType.NodeConstClass },
        { id: actionInitiated, type: ScType.NodeConstClass },
    ];   //проведите соответствие между идентификатором sc-keynodes
         //и его типом
    const keynodes = await client.resolveKeynodes(baseKeynodes); 
    //функция 'resolveKeynodes' производит поиск существующих указанных ключевых
    //элементов в базе знаний и, при их отсутствии, создает новые элементы 
    //в базе знаний.

    //Далее следует проверить принадлежность инициированного действия
    //классу Вашего агента с помощью написанной ранее функции check() 


     if(await check(actionNode,keynodes)===false)
    {
        return;
    }

    //Здесь начинается бизнес-логика Вашей программы 
    //Пожалуйста, ознакомьтесь с документацией ts-sc-client
    //для возможности использования всего функционала 


    //Для того, чтобы найти мощность множества, вам потребуется найти 
    //Количество элементов множества, являющимся аргументом агента
    //Создадим ScTemplate структуру для последующего поиска элементов
    const setAlias = '_set'; //узел множества (его идентификатор)
    const elAlias = '_el'; //узел элемента множества (его идентификатор)
    const template = new ScTemplate(); 
    template.triple(                            // класс_действия
        keynodes[getSetPowerAction],            //   |
        ScType.EdgeAccessVarPosPerm,            //   \/
        actionNode,                             // действие
    )
    template.tripleWithRelation(
        actionNode,                         // действие
        ScType.EdgeAccessVarPosPerm,        //   ||<--rrel_1(аргумент)
        [ScType.NodeVar, setAlias],         //   \/
        ScType.EdgeAccessVarPosPerm,        // множество
        keynodes[rrel1],
    )
    template.triple(                        // множество
        setAlias,                           //   |
        ScType.EdgeAccessVarPosPerm,        //   \/
        [ScType.NodeVar, elAlias],          // элемент
    )
    const result=await client.templateSearch(template);
    //каждый элемент 'result' хранит в себе найденную структуру  
    if (!result.length) { //если конструкция не найдена
        const finalConstruction= new ScConstruction();
        //ScConstruction() - класс-строитель sc-элементов
        //с помощью данного класса строются графовые структуры элементов,
        //которые, с помощью функции createElements(ScConstruction) 
        //синхронизируются с базой знаний
        finalConstruction.createEdge(ScType.EdgeAccessConstPosPerm,
        keynodes[actionFinished], actionNode);
        //Аргументы CreateEdge: тип sc-коннектора, область отправления, 
        //область прибытия.
        //В данном примере, так как мощность множества не была найдена,
        //проводятся дуги от ключевых элементов 'Actionfinised'
        //и 'actionFinishedUnsuccessfully'
        finalConstruction.createEdge(ScType.EdgeAccessConstPosPerm,
        keynodes[actionFinishedUnsuccessfully], actionNode);
        await client.createElements(finalConstruction); 
        return ;
    }


    const elements:ScAddr[] = [];
    for (let i = 0; i < result.length; i++) { 
       //C помощью цикла выбирается элемент множества из
       // каждой найденной конструкции
       //(!!!в одной конструкции у множества только один элемент)
        elements.push(result[i].get(elAlias));
    }
    const set = result[0].get(setAlias); 
    //'set' -  узел самого множества из первой найденной конструкции, 
    //так как узлы множества одинаковы во всех конструкциях
    const power = elements.length.toString();;
    //Подсчитанное множество требуется преобразовать
    //в строку для вывода в ScClink

    //На данном этапе основной задачей стоит формирование вызодной 
    //конструкции с помощью ScConstruction()

    //Примером обычно служит выходная конструкция (см. главу 1)


    const powerSetConstruction = new ScConstruction(); 
    //Конструкция,где будет только множество и его мощность

    const setPowerLinkAlias="set_power_link"; 
    //метки для всех будущих sc-элементов
    const edgeCommonSetPowerAlias="edge_common_set_power";
    const edgePosPermSetPowerAlias="edge_pos_perm_set_power";
    const resultConstructionNodeAlias="result_construction_node";
    const edgePosPermResultAlias="edge_pos_perm_result";
    const edgeCommonResultAlias="edge_common_result";


    powerSetConstruction.createLink(ScType.LinkConst,
    new ScLinkContent(power, ScLinkContentType.String),setPowerLinkAlias);
    //Создание ссылки createLink(): тип sc-элемента, 
    //настраиваемое содержиме ссылки, идентификатор
    powerSetConstruction.createEdge(ScType.EdgeDCommonConst,set,
    setPowerLinkAlias,edgeCommonSetPowerAlias); 
    //создание sc-дуги общего вида от узла множества к sc-ссылке
    powerSetConstruction.createEdge(ScType.EdgeAccessConstPosPerm,
    keynodes[nrelSetPower],edgeCommonSetPowerAlias,edgePosPermSetPowerAlias); 
    //создание неролевого отношения мощности множества
    const powerSetRes = await client.createElements(powerSetConstruction);     
    //Функция создания sc-элементов в базе знаний 
    //и последующая проверка ее создания   
       if (powerSetRes && powerSetRes.length > 0) {
        const resultConstruction = new ScConstruction(); 
        //Конструкция, где действие будет соединяться 
        //с предыдущей конструкцией отношением nrel_result
        resultConstruction.createNode(ScType.NodeConst,
        resultConstructionNodeAlias);
        //Создание узла-структуры требуется для проведения sc-дуги 
        //к нескольким элементам сразу
        

        resultConstruction.createEdge(ScType.EdgeDCommonConst,actionNode,
        resultConstructionNodeAlias,edgePosPermResultAlias);
        resultConstruction.createEdge(ScType.EdgeAccessConstPosPerm,
        keynodes[nrelResult],edgePosPermResultAlias,edgeCommonResultAlias);
        resultConstruction.createEdge(ScType.EdgeAccessConstPosPerm,
        resultConstructionNodeAlias,
        powerSetRes[powerSetConstruction.getIndex(setPowerLinkAlias)]);
        //Чтобы получить доступ к sc-элементом из
        //другой ScConstruction используйте метод getIndex(element)

        resultConstruction.createEdge(ScType.EdgeAccessConstPosPerm,
        resultConstructionNodeAlias,set);
        resultConstruction.createEdge(ScType.EdgeAccessConstPosPerm,
        resultConstructionNodeAlias,
        powerSetRes[powerSetConstruction.getIndex(edgeCommonSetPowerAlias)]);
        resultConstruction.createEdge(ScType.EdgeAccessConstPosPerm,
        resultConstructionNodeAlias,
        powerSetRes[powerSetConstruction.getIndex(edgePosPermSetPowerAlias)]);
        resultConstruction.createEdge(ScType.EdgeAccessConstPosPerm,
        resultConstructionNodeAlias,keynodes[nrelSetPower]);
        const resultRes = await client.createElements(resultConstruction);
        if (resultRes && resultRes.length > 0) {
            const finalConstruction= new ScConstruction();
            finalConstruction.createEdge(ScType.EdgeAccessConstPosPerm,
            keynodes[actionFinished], actionNode);
            finalConstruction.createEdge(ScType.EdgeAccessConstPosPerm,
            keynodes[actionFinishedSuccessfully], actionNode);
            await client.createElements(finalConstruction); 
            return ;

        //Вы можете переиспользовать переменные типа ScConstruction
        //для создания разных структур (finalConstruction)

        }
        //В случае, если не удалось создать sc-конструкция resultRes,
        // считается, что агент не смог выполнить свою задачу
        const finalConstruction= new ScConstruction();
        finalConstruction.createEdge(ScType.EdgeAccessConstPosPerm,
        keynodes[actionFinished], actionNode);
        finalConstruction.createEdge(ScType.EdgeAccessConstPosPerm,
        keynodes[actionFinishedUnsuccessfully], actionNode);
        await client.createElements(finalConstruction); 
        return ;
    }
    //В случае, если не удалось создать sc-конструкцию powerSetRes,
    // считается, что агент не смог выполнить свою задачу
    const finalConstruction= new ScConstruction();
    finalConstruction.createEdge(ScType.EdgeAccessConstPosPerm,
    keynodes[actionFinished], actionNode);
    finalConstruction.createEdge(ScType.EdgeAccessConstPosPerm,
    keynodes[actionFinishedUnsuccessfully], actionNode);
    await client.createElements(finalConstruction); 
    return ;
};
```

--- 

### **5. Зарегистрируйте агента.**

Для того чтобы зарегистрировать агента, требуется написать функцию, связывающую требуемое событие с основной функцией агента, реализующей его программу.

Реализуем данную функцию в `client.ts`:



**client.ts**

```typescript
//Это уже прописанная Вами преамбула 
import { SC_URL } from '@constants';
import { ScClient,ScType,ScEventParams,ScEventType } from 'ts-sc-client';

//Добавьте основную функцию агента, которая будет подписана на событие
import { getSetPowerAgent } from "@agents/agent"


//Функция регистрации агента
//В каечстве примера, событие, на которое реагирует агент, возьмем 
//проведение sc-коннектора от класса "action_initiated"
async function registerAgent() {
    const actionInitiated = "action_initiated";
    const keynodes = [
        { id: actionInitiated, type: ScType.NodeConstNoRole },
    ];

    const keynodesAddrs = await client.resolveKeynodes(keynodes);
    //Процесс нахождения "action_initiated" в базе знаний

    const eventParams = new ScEventParams(keynodesAddrs[actionInitiated],
    ScEventType.AddOutgoingEdge, getSetPowerAgent);
    //Создание связи (подписки) между событием и основной функцией Вашего агента
    //Функция принимает в качестве аргументов 
    //узел класса иницированного действия, 
    //тип события (в данном случае, это проведение sc-коннектора)
    //и подписанную функцию программы агента

    await client.eventsCreate([eventParams]); 
}

export const client = new ScClient(SC_URL);
```

Для того, чтобы проверить агента, следует вызвать функцию регистрации Вашего агента.
Это можно сделать в любом основном файле Вашего TypeScript проекта.

## **Часто задаваемые вопросы**

<!-- no toc -->
- [Могу ли я использовать классы ScIterator3Ptr и ScIterator5Ptr из m_context для поиска ранее созданных sc-конструкций?](#могу-ли-я-использовать-классы-sciterator3ptr-и-sciterator5ptr-из-m_context-для-поиска-ранее-созданных-sc-конструкций)

- [Чем агенты C++ отличаются от TypeScript? Могу ли я теперь всегда реализовывать только TypeScript агенты?](#чем-агенты-c-отличаются-от-typescript-могу-ли-я-теперь-всегда-реализовывать-только-typescript-агенты)

### Могу ли я использовать классы ScIterator3Ptr и ScIterator5Ptr из m_context для поиска ранее созданных sc-конструкций?

К сожалению, не весь функционал C++ Core API отражен в ts-sc-client. Таким образом, некоторые методы объекта класса ScMemoryContext недоступны для реализации в TypeScript, в том числе ScIterator3Ptr и ScIterator5Ptr.

### Чем агенты C++ отличаются от TypeScript? Могу ли я теперь всегда реализовывать только TypeScript агенты?

Ввиду особенностей организации sc-памяти и фундаменатльных преимуществ языка программирования C++ таких, как производительность и многофункциональность, агент, реализованный на TypeScript, может уступать агенту C++ по времени совершения операций. Мы рекомендуем использовать язык C++ для реализации агентов со сложной логикой, а TypeScript агенты реализовывать для незамысловатой работы с интерфейсом такой, как взаимодействие с базой знаний для получения структур и вывода их в интерфейс.


import { client } from '@api/sc';
import { ScAddr, ScConstruction, ScLinkContent, ScTemplate, ScType, ScLinkContentType } from 'ts-sc-client';


export const check = async(action: ScAddr,keynodes: Record<string, ScAddr>) => {
    const checkTemplate = new ScTemplate();
    checkTemplate.triple(                        // класс_действия
        keynodes["get_set_power_action"],  //   |
        ScType.EdgeAccessVarPosPerm,             //   \/
        action,                                  // действие
    );
    const checkResult = await client.templateSearch(checkTemplate);
    if (checkResult.length==0)
        {
            return false;

        }
    return true;
}

export const getSetPowerAgent = async(subscribedAddr: ScAddr, foundEdge: ScAddr, actionNode: ScAddr) => {
    const getSetPowerAction = "get_set_power_action";
    const action = 'action';
    const actionInitiated = 'action_initiated';
    const actionFinished = 'action_finished';
    const actionFinishedSuccessfully = 'action_finished_successfully';
    const actionFinishedUnsuccessfully = 'action_finished_unsuccessfully';
    const nrelResult = 'nrel_result';
    const nrelSetPower="nrel_set_power";
    const rrel1 = 'rrel_1';
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
    ];   
    const keynodes = await client.resolveKeynodes(baseKeynodes); //собрали все кейноды из БЗ
    if(await check(actionNode,keynodes)===false) //сделали проверку на класс действия
    {
        return;
    }

    const setAlias = '_set'; //узел множества
    const elAlias = '_el'; //узел элемента множества
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
    if (!result.length) { //если конструкция не найдена
        const finalConstruction= new ScConstruction();
        finalConstruction.createEdge(ScType.EdgeAccessConstPosPerm,keynodes[actionFinished], actionNode);
        finalConstruction.createEdge(ScType.EdgeAccessConstPosPerm,keynodes[actionFinishedUnsuccessfully], actionNode);
        await client.createElements(finalConstruction); 
        return ;
    }


    const elements:ScAddr[] = [];
    for (let i = 0; i < result.length; i++) {  //выбираем элемент множества из кажой найденной конструкции(!!!в одной конструкции у множества только один элемент)
        elements.push(result[i].get(elAlias));
    }
    const set = result[0].get(setAlias); // выбираем узел самого множества из первой найденной конструкции, потому что узел множества он и в Африке узел множества
    const power = elements.length.toString();;


    const powerSetConstruction = new ScConstruction(); //конструкция,где будет только множество и его мощность
    const setPowerLinkAlias="set_power_link"; //метки для всех будущих элементов
    const edgeCommonSetPowerAlias="edge_common_set_power";
    const edgePosPermSetPowerAlias="edge_pos_perm_set_power";
    const resultConstructionNodeAlias="result_construction_node";
    const edgePosPermResultAlias="edge_pos_perm_result";
    const edgeCommonResultAlias="edge_common_result";
    powerSetConstruction.createLink(ScType.LinkConst,new ScLinkContent(power, ScLinkContentType.String),setPowerLinkAlias);//создаем ссылку с числом                                              ссылка
    powerSetConstruction.createEdge(ScType.EdgeDCommonConst,set,setPowerLinkAlias,edgeCommonSetPowerAlias); //соединяем ее с множеством дугой для отношений                                                       /\
    powerSetConstruction.createEdge(ScType.EdgeAccessConstPosPerm,keynodes[nrelSetPower],edgeCommonSetPowerAlias,edgePosPermSetPowerAlias); //соедиянемя nrel_set_power с предыдущей дугой          ||<--nrel_set_power
    const powerSetRes = await client.createElements(powerSetConstruction);                                                                 //                                                    множестов
    if (powerSetRes && powerSetRes.length > 0) {
        const resultConstruction = new ScConstruction(); //конструкция,где действие будет соединяться с предыдущей конструкцией отношением nrel_result
        resultConstruction.createNode(ScType.NodeConst,resultConstructionNodeAlias); //создаем узел-посредний
        resultConstruction.createEdge(ScType.EdgeDCommonConst,actionNode,resultConstructionNodeAlias,edgePosPermResultAlias);// соединяем с действием дугой для отношений
        resultConstruction.createEdge(ScType.EdgeAccessConstPosPerm,keynodes[nrelResult],edgePosPermResultAlias,edgeCommonResultAlias); //соедиянемя nrel_result с предыдущей дугой
        resultConstruction.createEdge(ScType.EdgeAccessConstPosPerm,resultConstructionNodeAlias,powerSetRes[powerSetConstruction.getIndex(setPowerLinkAlias)]);// соединяем узел-посредний со всеми элементами powerSetConstruction
        resultConstruction.createEdge(ScType.EdgeAccessConstPosPerm,resultConstructionNodeAlias,set);
        resultConstruction.createEdge(ScType.EdgeAccessConstPosPerm,resultConstructionNodeAlias,powerSetRes[powerSetConstruction.getIndex(edgeCommonSetPowerAlias)]);
        resultConstruction.createEdge(ScType.EdgeAccessConstPosPerm,resultConstructionNodeAlias,powerSetRes[powerSetConstruction.getIndex(edgePosPermSetPowerAlias)]);
        resultConstruction.createEdge(ScType.EdgeAccessConstPosPerm,resultConstructionNodeAlias,keynodes[nrelSetPower]);
        const resultRes = await client.createElements(resultConstruction);
        if (resultRes && resultRes.length > 0) {
            const finalConstruction= new ScConstruction();
            finalConstruction.createEdge(ScType.EdgeAccessConstPosPerm,keynodes[actionFinished], actionNode);
            finalConstruction.createEdge(ScType.EdgeAccessConstPosPerm,keynodes[actionFinishedSuccessfully], actionNode);
            await client.createElements(finalConstruction); 
            return ;
        }
        const finalConstruction= new ScConstruction();
        finalConstruction.createEdge(ScType.EdgeAccessConstPosPerm,keynodes[actionFinished], actionNode);
        finalConstruction.createEdge(ScType.EdgeAccessConstPosPerm,keynodes[actionFinishedUnsuccessfully], actionNode);
        await client.createElements(finalConstruction); 
        return ;
    }
    const finalConstruction= new ScConstruction();
    finalConstruction.createEdge(ScType.EdgeAccessConstPosPerm,keynodes[actionFinished], actionNode);
    finalConstruction.createEdge(ScType.EdgeAccessConstPosPerm,keynodes[actionFinishedUnsuccessfully], actionNode);
    await client.createElements(finalConstruction); 
    return ;
};
import { SC_URL } from '@constants';
import { ScClient,ScType,ScEventParams,ScEventType } from 'ts-sc-client';
import { getSetPowerAgent } from "@agents/agent"

async function testAgent() {
    const actionInitiated = "action_initiated";
    const keynodes = [
        { id: actionInitiated, type: ScType.NodeConstNoRole },
    ];

    const keynodesAddrs = await client.resolveKeynodes(keynodes);

    const eventParams = new ScEventParams(keynodesAddrs[actionInitiated], ScEventType.AddOutgoingEdge, getSetPowerAgent);
    await client.eventsCreate([eventParams]); 
}

export const client = new ScClient(SC_URL);
testAgent();






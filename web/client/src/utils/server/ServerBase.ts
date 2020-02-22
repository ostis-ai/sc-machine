
import { ScNet, ScAddr, ScTemplate, ScType, ScTemplateResult, ScLinkContent, ScLinkContentType } from '@ostis/sc-core';
import { ServerKeynodes } from './ServerKeynodes';

type CallbackIdentifierFunction = (addr: ScAddr, idtf: string) => void;

export class ServerBase {

  private _client: ScNet = null;
  private _keynodes: ServerKeynodes = null;

  constructor(client: ScNet, keynodes: ServerKeynodes) {
    this._client = client;
    this._keynodes = keynodes;
  }

  public get client(): ScNet {
    return this._client;
  }

  public get keynodes(): ServerKeynodes {
    return this._keynodes;
  }

  public get host(): string {
    return location.protocol + '//' + location.hostname + (location.port ? ':' + location.port : '');
  }

  public GetContentURL(addr: ScAddr): string {
    return `http://${this.host}/content/${addr.value}`
  }

  public async GetSystemIdentifier(addrs: ScAddr[], callback: CallbackIdentifierFunction = null) : Promise<{ [key:number]:string; }> {

    let result: { [key:number]:string; } = {};

    for (let a of addrs) {
      let templ: ScTemplate = new ScTemplate();

      templ.TripleWithRelation(
        a,
        ScType.EdgeDCommonVar,
        [ScType.LinkVar, '_link'],
        ScType.EdgeAccessVarPosPerm,
        this.keynodes.kNrelSysIdtf);

        let searchResult: ScTemplateResult[] = await this.client.TemplateSearch(templ);
        if (searchResult.length > 0) {
          let linkAddr: ScAddr = searchResult[0].Get('_link');
          let content: ScLinkContent[] = await this.client.GetLinkContents([ linkAddr ]);
          
          if (content[0].type != ScLinkContentType.String) {
            throw "System identifier should be a string";
          }

          let idtf: string = content[0].data as string;
          result[a.value] = idtf;
          if (callback) {
            callback(linkAddr, idtf);
          }
        }
    }

    return new Promise<{ [key:number]:string; }>((resolve) => {
      resolve(result);
    });
  }
};
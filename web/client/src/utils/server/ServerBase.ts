
import { ScNet, ScAddr } from '@ostis/sc-core';
import { ServerKeynodes } from './ServerKeynodes';


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

};
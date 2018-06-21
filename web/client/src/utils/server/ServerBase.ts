import { Store } from 'react-redux';

import {
  ScNet, ScAddr, ScType, ScLinkContentType, ScLinkContent,
  ScTemplate, ScTemplateSearchResult, ScTemplateResult
} from '@ostis/sc-core';
import { ServerKeynodes } from './ServerKeynodes';
import * as store from '../../store';

export type Store = Store<{}>;

export class ServerBase {

  protected _client: ScNet = null;
  protected _keynodes: ServerKeynodes = null;
  protected _store: Store<{}> = null;

  constructor(client: ScNet, keynodes: ServerKeynodes, store: Store<{}>) {
    this._client = client;
    this._keynodes = keynodes;
    this._store = store;
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

  protected GetStore(): store.Store {
    return this._store.getState() as store.Store;
  }
};
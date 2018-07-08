import { Store } from 'react-redux';

import { ScNet } from '@ostis/sc-core';

import * as actions from '../../store/actions';

import { UIMode } from '../../store/store';

import { ServerKeynodes } from './ServerKeynodes';
import { ServerTemplates } from './ServerTemplates';

export class ServerBridge {
  private _client: ScNet = null;
  private _store: Store<{}> = null;

  private _serverKeynodes: ServerKeynodes = null;
  private _serverTemplates: ServerTemplates = null;

  /** Constructor
   * @param url URL to websocket service
   * @param store Redux store instance
   */
  constructor(url: string, store: Store<{}>) {
    this._store = store;
    this._store.dispatch(actions.ui.changeUIInitText('Connect to knowledge base'));
    this._client = new ScNet(url, this.OnConnected.bind(this), this.OnDisconnected.bind(this), this.OnError.bind(this));

    this._serverKeynodes = new ServerKeynodes(this._client);
    this._serverTemplates = new ServerTemplates(this._client, this._serverKeynodes, this._store);

    this._store.dispatch(actions.services.setupServices({
      bridge: this,
      keynodes: this._serverKeynodes,
      templates: this._serverTemplates,
    }));
  }

  private OnConnected() {
    this._store.dispatch(actions.net.changeNetSate(true));
    this.Initialize();
  }

  private OnDisconnected() {
    this._store.dispatch(actions.net.changeNetSate(false));
  }

  private OnError() {

  }

  public async Initialize(): Promise<boolean> {
    const self = this;

    this._store.dispatch(actions.ui.changeUIInitText('Initialize keynodes'));
    let result: boolean = await self._serverKeynodes.Initialize();

    this._store.dispatch(actions.ui.changeUIInitText('Initialize templates'));
    result = await self._serverTemplates.Initialize();

    this._store.dispatch(actions.ui.changeUIMode(UIMode.Normal));
    return new Promise<boolean>(function (resolve) { resolve(result); });
  }
};
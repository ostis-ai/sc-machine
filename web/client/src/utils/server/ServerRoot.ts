
import { ScNet } from '@ostis/sc-core';

import { ServerKeynodes } from './ServerKeynodes';
import { ServerTemplates } from './ServerTemplates';

type CallbackChangeInitState = (text: string) => void;
type CallbackChangeNetworkState = (isConnected: boolean) => void;
type CallbackInitialized = (isSuccess: boolean) => void;

interface ServerCallbacks {
  changeInitStateCallback: CallbackChangeInitState,
  changeNetworkStateCallback: CallbackChangeNetworkState,
  initializedCallback: CallbackInitialized,
}

export class ServerRoot {
  private _client: ScNet = null;
  private _url: string = '';

  private _callbackChangeInitState: CallbackChangeInitState = null;
  private _callbackChangeNetworkState: CallbackChangeNetworkState = null;
  private _callbackInitialized: CallbackInitialized = null;

  private _serverKeynodes: ServerKeynodes = null;
  private _serverTemplates: ServerTemplates = null;

  /** Constructor
   * @param url URL to websocket service
   * @param store Redux store instance
   */
  constructor(url: string, callbacks: ServerCallbacks) {

    this._url = url;
    this._callbackChangeInitState = callbacks.changeInitStateCallback;
    this._callbackChangeNetworkState = callbacks.changeNetworkStateCallback;
    this._callbackInitialized = callbacks.initializedCallback;
  }

  public Start() {
    this.NotifyChangeInitState('Connect to knowledge base');
    this._client = new ScNet(this._url, this.OnConnected.bind(this), this.OnDisconnected.bind(this), this.OnError.bind(this));

    this._serverKeynodes = new ServerKeynodes(this._client);
    this._serverTemplates = new ServerTemplates(this._client, this._serverKeynodes);
  }

  private NotifyChangeInitState(text: string) {
    if (this._callbackChangeInitState) {
      this._callbackChangeInitState(text);
    }
  }

  private NotifyChangeNetworkState(isConnected: boolean) {
    if (this._callbackChangeNetworkState) {
      this._callbackChangeNetworkState(isConnected);
    }
  }

  private OnConnected() {
    this.NotifyChangeNetworkState(true);
    this.Initialize();
  }

  private OnDisconnected() {
    this.NotifyChangeNetworkState(false);
  }

  private OnError() {

  }

  private async Initialize(): Promise<boolean> {
    const self = this;

    this.NotifyChangeInitState('Initialize keynodes');
    let result: boolean = await self._serverKeynodes.Initialize();

    this.NotifyChangeInitState('Initialize templates');
    result = await self._serverTemplates.Initialize();

    return new Promise<boolean>(function (resolve) { 
      if (self._callbackInitialized) {
        self._callbackInitialized(result);
      }
      resolve(result);
    });
  }

  public get keynodes() : ServerKeynodes {
    return this._serverKeynodes;
  }

  public get templates() : ServerTemplates {
    return this._serverTemplates;
  }
};
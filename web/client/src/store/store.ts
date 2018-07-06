import { Map, List } from 'immutable';
import { ScAddr } from '@ostis/sc-core';
import * as services from '../utils/server';

export enum UIMode {
  Initialize = 0,
  Normal
}

export enum UIActivePage {
  Dashboard,
}

export interface ScObject {
  addr: ScAddr,
  name: string,
}

export interface UIState {
  activePage: UIActivePage,
  activeMode: UIMode,
  initializeText: string,
};

export let uiInitState: UIState = {
  activePage: UIActivePage.Dashboard,
  activeMode: UIMode.Initialize,
  initializeText: "...",
};

export interface NetworkState {
  connected: boolean,
};

export let netInitState: NetworkState = {
  connected: false,
};

export interface ServicesState {
};

export let servicesInitState: ServicesState = {

};

export interface KBViewTemplate {
  title: string,
  isSaved: boolean,
  content: string,
  id: number,
};

export interface KBViewState {
  templates: Map<number, KBViewTemplate>,
  templCounter: number,
  templEditID: number,
};

export let kbViewInitState: KBViewState = {
  templates: Map<number, KBViewTemplate>([
    [0, {
      title: null,
      isSaved: false,
      content: '/* Write your template there using SCs-code: \n * http://ostis-dev.github.io/sc-machine/other/scs/ \n */',
      id: 0,
    }],
    [1, {
      title: 'Test',
      isSaved: true,
      content: '/* Write your template there using SCs-code: \n * http://ostis-dev.github.io/sc-machine/other/scs/ \n */\n sc_node -> test;;',
      id: 1,
    }]
  ]),
  templCounter: 0,
  templEditID: 0,
};

/// ---------------------------------------------------------
export interface Store {
  ui: UIState,
  net: NetworkState,
  services: ServicesState,
  kbView: KBViewState,
}

export const initialState: Store = {
  ui: uiInitState,
  net: netInitState,
  services: servicesInitState,
  kbView: kbViewInitState,
};
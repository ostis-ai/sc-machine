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

/// ---------------------------------------------------------
export interface Store {
  ui: UIState,
  net: NetworkState,
  services: ServicesState,
}

export const initialState: Store = {
  ui: uiInitState,
  net: netInitState,
  services: servicesInitState,
};
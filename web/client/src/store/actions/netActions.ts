import { Action } from './action';
import { UIMode, UIActivePage } from '../store';

export namespace NET {
  export const CHANGE_STATE = "NET_CHANGE_STATE";
}

export type ChangeNetSate = { isConnected: boolean };

export function changeNetSate(newState: boolean): Action<ChangeNetSate> {
  return {
    type: NET.CHANGE_STATE,
    payload: {
      isConnected: newState
    }
  };
}

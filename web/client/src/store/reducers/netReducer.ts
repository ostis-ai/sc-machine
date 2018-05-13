import { NetworkState, netInitState } from '../store';
import { Action } from '../actions/action';
import { ChangeNetSate, NET } from '../actions/netActions';

export function netReducer(state: NetworkState = netInitState, action: Action<any>) {

  switch (action.type) {
    case NET.CHANGE_STATE:
      return { ...state, connected: action.payload.isConnected };

    default:
      return state;
  }
}

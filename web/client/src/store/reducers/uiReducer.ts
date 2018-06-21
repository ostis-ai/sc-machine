import { UIState, uiInitState } from '../store';
import { Action } from '../actions/action';
import { ChangeUIModeAction, UI } from '../actions/uiActions';

export function uiReducer(state: UIState = uiInitState, action: Action<any>) {

  switch (action.type) {

    case UI.CHANGE_MODE:
      return { ...state, activeMode: action.payload.mode };

    case UI.CHANGE_INIT_TEXT:
      return { ...state, initializeText: action.payload.text };

    case UI.CHANGE_PAGE:
      return { ...state, activePage: action.payload.page };

    default:
      return state;
  }
}

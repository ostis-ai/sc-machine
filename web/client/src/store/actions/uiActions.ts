import { Action } from './action';
import { UIMode, UIActivePage, ScObject } from '../store';

export namespace UI {
  export const CHANGE_PAGE = 'UI_CHANGE_TAB';
  export const CHANGE_MODE = 'UI_CHANGE_MODE';
  export const CHANGE_INIT_TEXT = 'UI_CHANGE_INIT_TEXT';
}

export type ChangeActivePage = { page: UIActivePage };
export type ChangeUIModeAction = { mode: UIMode };
export type ChangeUIInitText = { text: string };

export function changeUIPage(newPage: UIActivePage): Action<ChangeActivePage> {
  return {
    type: UI.CHANGE_PAGE,
    payload: {
      page: newPage
    }
  }
}

export function changeUIMode(newMode: UIMode): Action<ChangeUIModeAction> {
  return {
    type: UI.CHANGE_MODE,
    payload: {
      mode: newMode
    }
  };
}

export function changeUIInitText(newText: string): Action<ChangeUIInitText> {
  return {
    type: UI.CHANGE_INIT_TEXT,
    payload: {
      text: newText
    }
  };
}

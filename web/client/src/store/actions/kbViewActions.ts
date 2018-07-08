import { Action } from './action';
import { KBViewTemplate } from '../store';

export namespace KBView {
  export const ADD_TEMPLATE = "KB_VIEW_ADD_TEMPLATE";
  export const SELECT_TEMPLATE = "KB_VIEW_SELECT_TEMPLATE";
  
  export const CHANGE_TEMPLATE = "KB_VIEW_CHANGE_TEMPLATE";
}

export function addTemplate(
    title: string = null,
    isSaved: boolean = false,
    content: string = ''): Action<KBViewTemplate> {

  return {
    type: KBView.ADD_TEMPLATE,
    payload: {
      title: title,
      isSaved: isSaved,
      content: content,
      id: -1,
    }
  };
}

export function selectTemplate(id: number): Action<number> {
  return {
    type: KBView.SELECT_TEMPLATE,
    payload: id,
  };
}

export interface ChangeTemplateAction {
  templID: number,
  newContent: string,
};

export function changeTemplate(templID: number, newContent: string): Action<ChangeTemplateAction> {
  return {
    type: KBView.CHANGE_TEMPLATE,
    payload: {
      templID: templID,
      newContent: newContent,
    }
  };
}
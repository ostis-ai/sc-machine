import { KBViewState, kbViewInitState, KBViewTemplate } from '../store';
import { Action } from '../actions/action';
import * as actions from '../actions/kbViewActions';

import { KBView } from '../actions/kbViewActions';


function addTemplateImpl(state: KBViewState, action: Action<KBViewTemplate>) : KBViewState {
  const newTabID = state.templCounter + 1;
  action.payload.id = newTabID; 
  
  return { ...state, 
    templates: state.templates.set(newTabID, action.payload),
    templCounter: newTabID,
  };
}

function selectTemplateImpl(state: KBViewState, action: Action<number>) : KBViewState {
  return { ...state, 
    templEditID: action.payload,
  };
}

function changeTemplateImpl(state: KBViewState, action: Action<actions.ChangeTemplateAction>) : KBViewState {
  const templID: number = action.payload.templID;
  const templ: KBViewTemplate = state.templates.get(templID);

  console.log(action.payload, templ);
  
  return { ...state, 
    templates: state.templates.set(templID, {...templ,
      isSaved: false,
      content: action.payload.newContent,
    })
  };
}

export function kbViewReducer(state: KBViewState = kbViewInitState, action: Action<any>) {

  function ActionT<T>(a: Action<any>): Action<T> {
    return a as Action<T>;
  }

  switch (action.type) {

    case KBView.ADD_TEMPLATE:
    return addTemplateImpl(state, ActionT<KBViewTemplate>(action));

    case KBView.SELECT_TEMPLATE:
      return selectTemplateImpl(state, ActionT<number>(action));

    case KBView.CHANGE_TEMPLATE:
      return changeTemplateImpl(state, ActionT<actions.ChangeTemplateAction>(action));

    default:
      return state;
  }
}

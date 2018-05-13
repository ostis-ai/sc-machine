import { ServicesState, servicesInitState } from '../store';
import { Action } from '../actions/action';
import { SERVICE, setupServices } from '../actions/serviceActions';

export function serviceReducer(state: ServicesState = servicesInitState, action: Action<any>) {

  switch (action.type) {
    case SERVICE.SETUP:
      return action.payload;

    default:
      return state;
  }
}

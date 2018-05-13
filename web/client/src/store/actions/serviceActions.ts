import { Action } from './action';
import { ServicesState } from '../store';

export namespace SERVICE {
  export const SETUP = "SERVICES_SETUP";
}

export function setupServices(newServices: ServicesState): Action<ServicesState> {
  return {
    type: SERVICE.SETUP,
    payload: newServices
  };
}

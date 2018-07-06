import { combineReducers, createStore } from 'redux';

import { uiReducer } from './uiReducer';
import { netReducer } from './netReducer';
import { serviceReducer } from './servicesReducer';
import { kbViewReducer } from './kbViewReducers';

export const rootReducer = combineReducers({
  ui: uiReducer,
  net: netReducer,
  services: serviceReducer,
  kbView: kbViewReducer,
});

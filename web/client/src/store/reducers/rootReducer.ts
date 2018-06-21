import { combineReducers, createStore } from 'redux';
import { routerReducer } from 'react-router-redux';

import { uiReducer } from './uiReducer';
import { netReducer } from './netReducer';
import { serviceReducer } from './servicesReducer';

export const rootReducer = combineReducers({
  ui: uiReducer,
  net: netReducer,
  services: serviceReducer,
});

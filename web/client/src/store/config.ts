import { createStore, applyMiddleware, Store } from 'redux';
import { rootReducer } from './reducers/rootReducer';
import { initialState } from './store';
import { composeWithDevTools } from 'redux-devtools-extension';

const composeEnhancers = composeWithDevTools({
  // Specify name here, actionsBlacklist, actionsCreators and other options if needed
});

export function configureStore(): Store<{}> {
  const store = createStore(rootReducer, initialState, composeEnhancers());
  return store;
}

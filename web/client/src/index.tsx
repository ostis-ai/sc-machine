import * as React from "react";
import * as ReactDOM from "react-dom";
import { Provider, Store } from 'react-redux';
import { Router, Route, BrowserRouter } from 'react-router-dom';

import { AppContainer } from "./containers/AppContainer";

import { configureStore } from './store/config';
import { ServerBridge } from './utils/server/ServerBridge';

import * as actions from './store/actions';
import * as st from './store/store';

import { SCsInitGlobal } from '@ostis/scs-js-editor';

const store: Store<{}> = configureStore();
const bridge = new ServerBridge('ws://localhost:8090/ws_json', store);

SCsInitGlobal();

ReactDOM.render(
  <Provider store={store}>
    <BrowserRouter>
      <AppContainer />
    </BrowserRouter>
  </Provider>,
  document.getElementById("content")
);
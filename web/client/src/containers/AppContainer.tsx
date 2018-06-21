import * as React from "react";
import {
  Segment, Menu, Icon,
  Loader, Dimmer
} from 'semantic-ui-react';
import { Route, Link, Switch, withRouter } from 'react-router-dom';

import { connect } from 'react-redux';
import { bindActionCreators } from 'redux';

import { DashBoardContainer } from './DashboardContainer';
import { KnowBaseContainer } from './KnowBaseContainer';

import { Store, NetworkState, UIState, UIMode, UIActivePage } from '../store/store';
import * as storeActions from '../store/actions';

interface AppContainerProps {
  store?: Store,
};

function mapStateToProps(state: Store): any {
  return {
    store: state,
  };
}

export class AppContainerImpl extends React.Component<AppContainerProps, any> {

  render() {
    const self = this;
    const uiMode: UIMode = this.props.store.ui.activeMode;
    const uiPage: UIActivePage = this.props.store.ui.activePage;
    const initText: string = this.props.store.ui.initializeText;

    return (
      <div>
        <Menu attached='top' inverted style={{ borderRadius: '0px' }} icon='labeled'>
          <Menu.Item name='status'>
            <Icon name='signal' color={this.props.store.net.connected ? 'green' : 'red'} size='large' />
            {this.props.store.net.connected ? 'Connected' : 'Diconnected'}
          </Menu.Item>

          <Menu.Item as={Link} to='/' name='dashboard' link>
            <Icon name='home' />
            Dashboard
                    </Menu.Item>
          <Menu.Item as={Link} to='/kb' name='kb' link>
            <Icon name='sitemap' />
            Knowledge base
                    </Menu.Item>
        </Menu>
        {this.props.store.ui.activeMode != UIMode.Initialize ?
          (
            <Segment basic className="container-page">
              <Switch>
                <Route exact path='/' component={DashBoardContainer as any} />
                <Route exact path='/kb' component={KnowBaseContainer as any} />
              </Switch>
            </Segment>
          ) : (
            <Dimmer active>
              <Loader size='massive'>{initText + '...'}</Loader>
            </Dimmer>
          )}
      </div>
    );
  }
}


export const AppContainer = withRouter(connect(
  mapStateToProps
)(AppContainerImpl) as any);
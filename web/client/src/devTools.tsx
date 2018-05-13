import * as React from 'react';
import { createDevTools } from 'redux-devtools';
import LogMonitor from 'redux-devtools-log-monitor';
import DockMonitor from 'redux-devtools-dock-monitor';

// Look at: https://onsen.io/blog/react-redux-devtools-with-time-travel/

/**
 * Create the DevTools component and export it.
 */
export default createDevTools(
  <DockMonitor
    /**
     * Hide or show the dock with "ctrl-h".
     */
    toggleVisibilityKey='ctrl-h'
    /**
     * Change the position of the dock with "ctrl-q".
     */
    changePositionKey='ctrl-q'
    defaultIsVisible={true}
    defaultPosition='right'
  >
    <LogMonitor theme='tomorrow' />
  </DockMonitor>
);
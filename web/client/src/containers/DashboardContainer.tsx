import * as React from "react";
import { Header } from 'semantic-ui-react';
import { BaseContainer } from './BaseContainer';

export interface DashBoardContainerProps {
};

export interface DashBoardContainerState {
};

export class DashBoardContainer extends React.Component<DashBoardContainerProps, DashBoardContainerState> {

  render() {
    return (
      <BaseContainer title='Dashboard'>
      </BaseContainer>
    );
  }
}
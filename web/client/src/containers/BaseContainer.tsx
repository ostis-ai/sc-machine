import * as React from "react";
import { Header, Container } from 'semantic-ui-react'

export interface BaseContainerProps {
  title: string,
};

export class BaseContainer extends React.Component<BaseContainerProps, null> {

  render() {
    return (
      <Container fluid className="base-page">

        <Header as='h2'>{this.props.title}</Header>
        {this.props.children}
      </Container>
    );
  }
}
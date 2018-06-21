import * as React from "react";
import { Header } from 'semantic-ui-react';
import { BaseContainer } from './BaseContainer';
import { SCsEditorView } from './views/SCsEditor';

export interface KnowBaseContainerProps {
};

export interface KnowBaseContainerState {
};

export class KnowBaseContainer extends React.Component<KnowBaseContainerProps, KnowBaseContainerState> {

  render() {
    return (
      <BaseContainer title='Knowledge base'>
        <SCsEditorView editorID="scs-editor-kb"/>
      </BaseContainer>
    );
  }
}
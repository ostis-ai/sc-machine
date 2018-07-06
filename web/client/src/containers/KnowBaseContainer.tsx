import * as React from "react";
import { connect } from 'react-redux';

import { BaseContainer } from './BaseContainer';
import { SCsEditorView } from './views/SCsEditor';

import { Grid, Button, TextArea, List, Segment } from 'semantic-ui-react';

import { Store, KBViewTemplate } from '../store';
import * as actions from '../store/actions/kbViewActions';

export interface KnowBaseContainerProps {
  store: Store;
  
  dispatchChangedTab: (tabID: number) => void;
  dispatchChangedContent: (tabID: number, content: String) => void;
};

export interface KnowBaseContainerState {
  editTemplateID: number,
};

function mapStateToProps(state: Store) {
  return {
    store: state,
  };
}

function mapDispatchToProps(dispatch: any) {
  return {
    dispatchChangedTab: (tabID: number) => {
      dispatch(actions.selectTemplate(tabID));
    },
    dispatchChangedContent: (tabID: number, content: string) => {
      dispatch(actions.changeTemplate(tabID, content));
    },
  };
}

export class KnowBaseContainerImpl extends React.Component<KnowBaseContainerProps, KnowBaseContainerState> {

  constructor(props) {
    super(props);

    this.state = {
      editTemplateID: this.props.store.kbView.templEditID,
    }
  }

  private onDoSearch() {
    console.log(this.refs);
  }

  private onDoGenerate() {
    alert('run generate');
  }

  private onTemplateSelected(tabID: number) : void {
    if (tabID != this.state.editTemplateID) {
      this.setState({
        editTemplateID: tabID,
      });

      this.props.dispatchChangedTab(tabID);
    }
  }

  private getEditTemplateContent() : string {
    const template: KBViewTemplate = this.props.store.kbView.templates.get(this.state.editTemplateID);
    const content: string = template ? template.content : '';

    return content;
  }

  private renderTabs() : any {
    const self = this;
    return (
      self.props.store.kbView.templates.valueSeq().map((item: KBViewTemplate) => {
        const title = item.title ? item.title : "Untitled";
        return (
            <List.Item 
              key={ item.id }
              active={ self.props.store.kbView.templEditID == item.id }
              onClick={ () => this.onTemplateSelected(item.id) }
              >
              <List.Content>
                { item.isSaved ? 
                  ( title )
                  :
                  ( <List.Header>{ title + ' *' }</List.Header> )
                }
              </List.Content>
            </List.Item>
        );
      })
    );
  }

  render() {
    const self = this;

    return (
      <BaseContainer title='Knowledge base'>
        <Grid columns="equal">

          <Grid.Column>
            <Grid columns='equal'>
              <Grid.Column width={3}>
                <List divided relaxed selection>
                  {this.renderTabs()}
                </List>
              </Grid.Column>
        
              <Grid.Column>
                <SCsEditorView
                  ref="SCsEditor"
                  editorID="scs-editor-kb"
                  content={ this.getEditTemplateContent() }
                  templateID={ this.state.editTemplateID }
                  onContentChanged={ (newContent: String) => this.props.dispatchChangedContent(this.state.editTemplateID, newContent) }/>

                <Button.Group attached="bottom" floated="right">
                  <Button color="green" onClick={this.onDoSearch.bind(self)}>Search</Button>
                  <Button color="red" onClick={this.onDoGenerate.bind(self)}>Generate</Button>
                </Button.Group>
              </Grid.Column>
            </Grid>
          </Grid.Column>

          <Grid.Column>
            <TextArea autoHeight placeholder="Result"></TextArea>
          </Grid.Column>

        </Grid>
      </BaseContainer>
    );
  }
}

export const KnowBaseContainer = connect( 
  mapStateToProps, 
  mapDispatchToProps,
  null
)(KnowBaseContainerImpl);
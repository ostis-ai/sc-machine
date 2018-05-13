import * as React from "react";
import { connect } from 'react-redux';
import { findDOMNode } from 'react-dom';

import { SCsEditor } from '@ostis/scs-js-editor';

import * as store from '../../store/store';

interface SCsEditorViewProps {
  store?: store.Store,
  editorID: string,
};

function mapStateToProps(state: store.Store, ownProps: SCsEditorViewProps) {
  return {
    store: state,
    editorID: ownProps.editorID,
  };
}

function mapDispatchToProps(dispatch: any) {
  return {};
}

interface SCsEditorViewState {
  _editor: SCsEditor;
}

class SCsEditorViewImpl extends React.Component<SCsEditorViewProps, SCsEditorViewState> {

  constructor(props) {
    super(props);

    this.state = {
      _editor: null
    };
  }

  componentDidMount() : void {
    const editor: SCsEditor = new SCsEditor(document.getElementById(this.props.editorID));
    this.setState({
      _editor: editor
    });
  }

  render() {
    return (
      <div id={this.props.editorID} className="scs-editor-container"></div>
    );
  }
}

export const SCsEditorView = connect(
  mapStateToProps,
  mapDispatchToProps,
  null
)(SCsEditorViewImpl);
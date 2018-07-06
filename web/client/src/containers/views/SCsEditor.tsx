import * as React from "react";
import { connect } from 'react-redux';

import { SCsEditor } from '@ostis/scs-js-editor';

import * as store from '../../store/store';

interface SCsEditorViewProps {
  store?: store.Store,
  editorID: string,
  content?: string,
  templateID: number,

  onContentChanged?: (newContent: String) => void,
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
  
  shouldComponentUpdate(nextProps: SCsEditorViewProps, nextState: SCsEditorViewState) : boolean {
    return (nextProps.templateID != this.props.templateID);
  }

  componentDidUpdate() : void {
    if (this.state._editor) {
      const callback = this.state._editor.onContentChanged;
      this.state._editor.onContentChanged = null;
      this.state._editor.content = this.props.content;
      this.state._editor.onContentChanged = callback;
    }
  }

  componentDidMount() : void {
    const editor: SCsEditor = new SCsEditor(document.getElementById(this.props.editorID));
    editor.content = this.props.content ? this.props.content : '';
    editor.onContentChanged = this.props.onContentChanged;

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
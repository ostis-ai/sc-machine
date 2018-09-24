import { BaseView } from './BaseView';
import { SCsInputContainer } from './kbView';

export class KBView extends BaseView {

  private _scsView: SCsInputContainer = null;

  constructor(parent: HTMLElement) {
    super('Knowledge Base', parent);
    this.InitHtml(this.container);
  }

  private InitHtml(parent: HTMLElement) {

    const grid: HTMLElement = document.createElement('div');
    grid.setAttribute('class', 'ui equal width grid');
    grid.style.height = '100%';

    grid.innerHTML = `
        <div class="stretched row">
          <div class="column equal width" left>
          </div>
          <div class="column equal width" right>
          </div>
        </div>
    `;

    parent.appendChild(grid);

    this._scsView = new SCsInputContainer(this.leftControls);
  }

  private get leftControls() : HTMLElement {
    return this.container.querySelector('div[left]');
  }

  private get rightControls() : HTMLElement {
    return this.container.querySelector('div[right]');
  }
  // <Grid.Column>
  //           <Grid columns='equal'>
  //             <Grid.Column width={3}>
  //               <List divided relaxed selection>
  //                 {this.renderTabs()}
  //               </List>
  //             </Grid.Column>
        
  //             <Grid.Column>
  //               <SCsEditorView
  //                 ref="SCsEditor"
  //                 editorID="scs-editor-kb"
  //                 content={ this.getEditTemplateContent() }
  //                 templateID={ this.state.editTemplateID }
  //                 onContentChanged={ (newContent: String) => this.props.dispatchChangedContent(this.state.editTemplateID, newContent) }/>

  //               <Button.Group attached="bottom" floated="right">
  //                 <Button color="green" onClick={this.onDoSearch.bind(self)}>Search</Button>
  //                 <Button color="red" onClick={this.onDoGenerate.bind(self)}>Generate</Button>
  //               </Button.Group>
  //             </Grid.Column>
  //           </Grid>
  //         </Grid.Column>

  //         <Grid.Column>
  //           <TextArea autoHeight placeholder="Result"></TextArea>
  //         </Grid.Column>

  //       </Grid>
    
  }

  // private onDoSearch() {
  // }

  // private onDoGenerate() {
  // }

  // private onTemplateSelected(tabID: number) : void {
  //   if (tabID != this.state.editTemplateID) {
  //     this.setState({
  //       editTemplateID: tabID,
  //     });

  //     this.props.dispatchChangedTab(tabID);
  //   }
  // }

  // private getEditTemplateContent() : string {
  //   const template: KBViewTemplate = this.props.store.kbView.templates.get(this.state.editTemplateID);
  //   const content: string = template ? template.content : '';

  //   return content;
  // }

  // private renderTabs() : any {
  //   const self = this;
  //   return (
  //     self.props.store.kbView.templates.valueSeq().map((item: KBViewTemplate) => {
  //       const title = item.title ? item.title : "Untitled";
  //       return (
  //           <List.Item 
  //             key={ item.id }
  //             active={ self.props.store.kbView.templEditID == item.id }
  //             onClick={ () => this.onTemplateSelected(item.id) }
  //             >
  //             <List.Content>
  //               { item.isSaved ? 
  //                 ( title )
  //                 :
  //                 ( <List.Header>{ title + ' *' }</List.Header> )
  //               }
  //             </List.Content>
  //           </List.Item>
  //       );
  //     })
  //   );
  // }

  // render() {
  //   const self = this;

  //   return (
  //     <BaseView title='Knowledge base'>
  //       <Grid columns="equal">

  //         <Grid.Column>
  //           <Grid columns='equal'>
  //             <Grid.Column width={3}>
  //               <List divided relaxed selection>
  //                 {this.renderTabs()}
  //               </List>
  //             </Grid.Column>
        
  //             <Grid.Column>
  //               <SCsEditorView
  //                 ref="SCsEditor"
  //                 editorID="scs-editor-kb"
  //                 content={ this.getEditTemplateContent() }
  //                 templateID={ this.state.editTemplateID }
  //                 onContentChanged={ (newContent: String) => this.props.dispatchChangedContent(this.state.editTemplateID, newContent) }/>

  //               <Button.Group attached="bottom" floated="right">
  //                 <Button color="green" onClick={this.onDoSearch.bind(self)}>Search</Button>
  //                 <Button color="red" onClick={this.onDoGenerate.bind(self)}>Generate</Button>
  //               </Button.Group>
  //             </Grid.Column>
  //           </Grid>
  //         </Grid.Column>

  //         <Grid.Column>
  //           <TextArea autoHeight placeholder="Result"></TextArea>
  //         </Grid.Column>

  //       </Grid>
  //     </BaseContainer>
  //   );
  // }
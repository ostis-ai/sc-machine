import { SCsEditor } from "@ostis/scs-js-editor";
import { App } from "../../App";
import { ServerTemplates } from "../../utils/server";


export class SCsInputContainer {

  private _container: HTMLElement = null;
  private _scsEditor: SCsEditor = null;

  constructor(parent: HTMLElement) {
    this.InitHtml(parent);
    this.LoadTemplates();
  }

  private InitHtml(parent: HTMLElement) {
    this._container = document.createElement('div');
    parent.appendChild(this._container);

    this._container.setAttribute('class', 'ui equal width grid');

    this._container.innerHTML = `
        <div class="stretched row">
          <div class="three wide column">
            <div class="ui divided relaxed selection list" scs-list>
            </div>
          </div>
          <div class="column">
            <div class="scs-editor-container" scs-editor>
            </div>
          </div>
        </div>
    `;

    this._scsEditor = new SCsEditor(this.scsEditorContainer, 
                                    this.defaultContent);

    // append buttons
    const buttonsGroup: HTMLElement = document.createElement('div');
    this.scsEditorContainer.appendChild(buttonsGroup);
    buttonsGroup.setAttribute('class', 'ui right floated buttons');

    buttonsGroup.innerHTML = `
      <button class="ui green left bottom attached button" scs-search>Search</button>
      <button class="ui red right bottom attached button" scs-generate>Generate</button>
    `;
  }

  private get scsListContainer() : HTMLElement {
    return this._container.querySelector('div[scs-list]');
  }

  private get scsEditorContainer() : HTMLElement {
    return this._container.querySelector('div[scs-editor]');
  }

  private get scsSearchButton() : HTMLElement {
    return this._container.querySelector('button[scs-search]');
  }

  private get scsGenerateButton() : HTMLElement {
    return this._container.querySelector('gutton[scs-generate]');
  }

  private get defaultContent() : string {
    return '/* Write your template there using SCs-code: \n * http://ostis-dev.github.io/sc-machine/other/scs/ \n */\n';
  }

  private LoadTemplates() {
    
  }
};
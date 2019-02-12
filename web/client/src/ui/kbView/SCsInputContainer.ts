import { SCsEditor } from "@ostis/scs-js-editor";
import { App } from "../../App";
import { ServerTemplatesListener } from "../../utils/server/ServerTemplates";
import { KBTemplate } from "../../utils/types";
import { ScAddr, ScTemplateSearchResult } from "@ostis/sc-core";


type OnRequestCallback = (scsText: string) => Promise<boolean>;

export class SCsInputContainer extends ServerTemplatesListener {

  private _container: HTMLElement = null;
  private _scsEditor: SCsEditor = null;
  private _activeTemplate: ScAddr = null;

  private _onNewSeachRequestCallback: OnRequestCallback = null;
  private _onNewGenerateRequestCallback: OnRequestCallback = null;

  constructor(parent: HTMLElement) {
    super();

    this.InitHtml(parent);
    App.Templates().AddListener(this);
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

    this._scsEditor = new SCsEditor(this.scsEditorContainer);

    // append buttons
    const buttonsGroup: HTMLElement = document.createElement('div');
    this.scsEditorContainer.appendChild(buttonsGroup);
    buttonsGroup.setAttribute('class', 'ui right floated buttons');

    buttonsGroup.innerHTML = `
      <button class="ui green left bottom attached button" scs-search>Search</button>
      <button class="ui red right bottom attached button" scs-generate>Generate</button>
    `;

    this.scsSearchButton.onclick = () => {
      this.DoSearch(this._scsEditor.content);
    }

    this.scsGenerateButton.onclick = () => {
      this.DoGenerate(this._scsEditor.content);
    }
  }

  private get scsListContainer() : HTMLElement {
    return this._container.querySelector('div[scs-list]');
  }

  private get scsEditorContainer() : HTMLElement {
    return this._container.querySelector('div[scs-editor]');
  }

  private get scsSearchButton() : HTMLElement {
    return this.scsEditorContainer.querySelector('button[scs-search]');
  }

  private get scsGenerateButton() : HTMLElement {
    return this.scsEditorContainer.querySelector('button[scs-generate]');
  }

  public set onNewSearchRequest(callback: OnRequestCallback) {
    this._onNewSeachRequestCallback = callback;
  }

  public set onNewGenerateRequest(callback: OnRequestCallback) {
    console.log(callback);
    this._onNewGenerateRequestCallback = callback;
  }

  private OnTemplateSelected(templAddr: ScAddr) {
    if (this._activeTemplate && (this._activeTemplate.value === templAddr.value)) {
      return; // do nothing
    }

    const templates: KBTemplate[] = App.Templates().templates;
    
    for (let i = 0; i < templates.length; ++i) {
      const templ: KBTemplate = templates[i];

      if (templ.addr.value === templAddr.value) {
        this._scsEditor.content = templ.scsContent;
      }
    }

    // update UI
    if (this._activeTemplate) {
      this.scsListContainer.querySelector(`div[sc-addr="${this._activeTemplate.value}"]`).classList.remove('selected');
    }
    this.scsListContainer.querySelector(`div[sc-addr="${templAddr.value}"]`).classList.add('selected');

    this._activeTemplate = templAddr;
  }

  public OnTemplatesChanged(newTemplates: KBTemplate[]) {
    
    // update templates there
    newTemplates.forEach(templ => {
      const existItem: HTMLElement = this.scsListContainer.querySelector(`div[sc-addr="${templ.addr}"]`);
      if (!existItem) {
        const item: HTMLElement = document.createElement('div');
        item.setAttribute('class', 'item');
        item.setAttribute('sc-addr', templ.addr.value.toString());

        item.innerHTML = `
          <div class="content">
            <div class="description">${templ.title}</div>
          </div>
        `;

        this.scsListContainer.appendChild(item);

        item.onclick = () => {
          this.OnTemplateSelected(templ.addr);
        };
      }
    });

    if (!this._activeTemplate && newTemplates.length > 0) {
      this.OnTemplateSelected(newTemplates[0].addr);
    }
  }

  public async DoSearch(scsContent: string) : Promise<void> {
    if (this._onNewSeachRequestCallback) {
      const result: boolean = await this._onNewSeachRequestCallback(scsContent);
    }

    return new Promise<void>((resolve) => {
      resolve();
    });
  }

  public async DoGenerate(scsContent: string) {
    if (this._onNewGenerateRequestCallback) {
      const result: boolean = await this._onNewGenerateRequestCallback(scsContent);
    }

    return new Promise<void>((resolve) => {
      resolve();
    });
  }

};
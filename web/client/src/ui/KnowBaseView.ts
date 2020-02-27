import { BaseView } from './BaseView';
import { SCsInputContainer, SCgContainer } from './kbView';
import { ScTemplateSearchResult, ScTemplateGenerateResult, ScAddr } from '@ostis/sc-core';
import { App } from '../App';
import { SCgStruct } from '@ostis/scg-js-editor';

export class KBView extends BaseView {

  private _scsView: SCsInputContainer = null;
  private _scgView: SCgContainer = null;

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
    this._scsView.onNewSearchRequest = this.OnRequestSearch.bind(this);
    this._scsView.onNewGenerateRequest = this.OnRequestGenerate.bind(this);
    this._scsView.onNewCreateRequest = this.OnRequestCreate.bind(this);

    this._scgView = new SCgContainer(this.rightControls);
  }

  // --- Properties ---

  private get leftControls() : HTMLElement {
    return this.container.querySelector('div[left]');
  }

  private get rightControls() : HTMLElement {
    return this.container.querySelector('div[right]');
  }

  // --- Methods ---
  private async OnRequestSearch(scsText: string) : Promise<boolean> {
    const searchResult: ScTemplateSearchResult = await App.Templates().DoSearch(scsText);
    const scgStruct: SCgStruct = await App.Templates().MakeSCgStruct(searchResult);

    this._scgView.ShowResult(scgStruct);

    return new Promise<boolean>((resolve) => {
      resolve(true);
    });
  }

  private async OnRequestGenerate(scsText: string) : Promise<boolean> {

    const generateResult: ScTemplateGenerateResult = await App.Templates().DoGenerate(scsText);
    const scgStruct: SCgStruct = await App.Templates().MakeSCgStruct([generateResult]);

    this._scgView.ShowResult(scgStruct);

    return new Promise<boolean>((resolve) => {
      resolve(true);
    })
  }

  private async OnRequestCreate(scsText: string) : Promise<boolean> {
    const createResult: ScAddr = await App.Templates().DoCreate(scsText);

    return new Promise<boolean>((resolve) => {
      resolve(true);
    })
  }
}
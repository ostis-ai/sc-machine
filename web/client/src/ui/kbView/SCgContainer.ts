import { SCgViewer, SCgStruct } from '@ostis/scg-js-editor';
import { ScTemplateSearchResult, ScAddr } from '@ostis/sc-core';
import { App } from '../../App';

export class SCgContainer {

  private _scgViewer: SCgViewer = null;

  constructor(parent: HTMLElement) {
    this.InitHtml(parent);
  }

  private InitHtml(parent: HTMLElement) {
    parent.id = 'kb-scg-viewer';
    
    this._scgViewer = new SCgViewer(parent.id);
  }

  public async ShowResult(scgStruct: SCgStruct) : Promise<void> {
    
    this._scgViewer.SetStruct(scgStruct);
    scgStruct.Update();
    this._scgViewer.Layout();

    return new Promise<void>((resolve) => {
      resolve();
    });
  }
};
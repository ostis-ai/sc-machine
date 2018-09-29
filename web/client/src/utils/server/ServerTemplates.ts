import { ScNet, ScType, ScAddr, ScTemplate, 
         ScTemplateSearchResult, ScTemplateResult, ScLinkContent } from '@ostis/sc-core';
import { ServerKeynodes } from './ServerKeynodes';
import { ServerBase } from './ServerBase';
import { KBTemplate } from '../types';
import { SCgStruct } from '@ostis/scg-js-editor';

export abstract class ServerTemplatesListener {
  public abstract OnTemplatesChanged(newTemplates: KBTemplate[]);
};

export class ServerTemplates extends ServerBase {

  private _templates: KBTemplate[] = [];
  private _listeners: ServerTemplatesListener[] = [];

  constructor(client: ScNet, keynodes: ServerKeynodes) {
    super(client, keynodes);
  }

  public async Initialize(): Promise<boolean> {
  
    this._templates = await this.LoadTemplatesFromKB();
    this.NotifyAllListeners();

    return new Promise<boolean>(function (resolve) {
      resolve(true);
    });
  }

  public get templates() :  KBTemplate[] {
    return this._templates;
  }

  public AddListener(listener: ServerTemplatesListener) {
    if (this._listeners.indexOf(listener) === -1) {
      this._listeners.push(listener);
      this.NotifyListener(listener);
    }
  }

  public RemoveListener(listener: ServerTemplatesListener) {
    const index = this._listeners.indexOf(listener);
    if (index > -1) {
      this._listeners.splice(index, 1);
    }
  }

  private NotifyListener(listener: ServerTemplatesListener) {
    listener.OnTemplatesChanged(this._templates);
  }

  private NotifyAllListeners() {
    const self = this;
    this._listeners.forEach(listener => {
      self.NotifyListener(listener);
    });
  }

  public async LoadTemplatesFromKB() : Promise<KBTemplate[]> {

    // find all templates
    const user: ScAddr = this.keynodes.kTestUser;

    const templ: ScTemplate = new ScTemplate();
    templ.TripleWithRelation(
      [user, '_user'],
      ScType.EdgeDCommonVar,
      [ScType.NodeVar, '_templates'],
      ScType.EdgeAccessVarPosPerm,
      this.keynodes.kNrelWebTemplates
    );

    templ.Triple(
      '_templates',
      ScType.EdgeAccessVarPosPerm,
      [ScType.NodeVar, '_templ']
    );

    templ.TripleWithRelation(
      '_templ',
      ScType.EdgeDCommonVar,
      [ScType.LinkVar, '_scs_templ'],
      ScType.EdgeAccessVarPosPerm,
      this.keynodes.kNrelSCsTemplate,
    );

    templ.TripleWithRelation(
      '_templ',
      ScType.EdgeDCommonVar,
      [ScType.LinkVar, '_title'],
      ScType.EdgeAccessVarPosPerm,
      this.keynodes.kNrelMainIdtf
    );
    
    // TODO: support current language
    
    const self = this;
    const result: ScTemplateSearchResult = await this.client.TemplateSearch(templ);
    
    // collect templates data
    const templateAddrs: ScAddr[] = [];
    const titleAddrs: ScAddr[] = [];
    const scsAddrs: ScAddr[] = [];
    result.forEach((r: ScTemplateResult) => {
      const t: ScAddr = r.Get('_templ');
      templateAddrs.push(t);

      const title: ScAddr = r.Get('_title');
      titleAddrs.push(title);

      const scs: ScAddr = r.Get('_scs_templ');
      scsAddrs.push(scs);
    });

    // get titles
    let templates: KBTemplate[] = [{
      title: 'Unknown',
      addr: new ScAddr(),
      scsContent: '/* Write your template there using SCs-code: \n * http://ostis-dev.github.io/sc-machine/other/scs/ \n */\n',
    }];
    
    if (titleAddrs.length > 0) {
      const titles: ScLinkContent[] = await self.client.GetLinkContents(titleAddrs);
      const scsValues: ScLinkContent[] = await self.client.GetLinkContents(scsAddrs);

      for (let i = 0; i < titles.length; ++i) {
        templates.push({
          addr: templateAddrs[i],
          title: titles[i].data as string,
          scsContent: scsValues[i].data as string,
        });
      }
    }
    
    return new Promise<KBTemplate[]>(function(resolve) {
      resolve(templates);
    });
  }

  public async DoSearch(scsTempl: string): Promise<ScTemplateSearchResult> {

    const searchResult: ScTemplateSearchResult = await this.client.TemplateSearch(scsTempl);

    return new Promise<ScTemplateSearchResult>(function (resolve) {
      resolve(searchResult);
    });
  };

  public async MakeSCgStruct(data: ScTemplateResult[]) : Promise<SCgStruct> {
    const result: SCgStruct = new SCgStruct();

    let values: number[] = [];
    let addrs: ScAddr[] = [];
    data.forEach(item => {
      for (let i = 0; i < item.size; ++i) {
        const a: ScAddr = item.Get(i);
        
        if (values.indexOf(a.value) === -1) {
          values.push(a.value);
          addrs.push(a);
        }
      }
    });

    // collect elements type
    const types: ScType[] = await this.client.CheckElements(addrs);

    // insert all nodes
    const edgeTypes: Map<number, ScType> = new Map<number, ScType>();
    for (let i = 0; i < types.length; ++i) {
      const a: ScAddr = addrs[i];
      const t: ScType = types[i];
      if (t.isNode()) {
        result.AddObject({
          addr: a,
          type: t,
        })
      } else if (t.isEdge()) {
        edgeTypes.set(a.value, t);
      }
    }

    // insert edges
    data.forEach(item => {
      item.ForEachTriple((src: ScAddr, edge: ScAddr, trg: ScAddr) => {
        result.AddObject({
          addr: edge,
          type: edgeTypes.get(edge.value),
          src: src,
          trg: trg
        });
      });
    });

    return new Promise<SCgStruct>((resolve) => {
      resolve(result);
    });
  }
};
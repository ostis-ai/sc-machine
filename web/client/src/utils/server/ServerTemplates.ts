import { ScNet, ScType, ScAddr, ScTemplate, 
         ScTemplateSearchResult, ScTemplateResult, ScLinkContent } from '@ostis/sc-core';
import { ServerKeynodes } from './ServerKeynodes';
import { ServerBase } from './ServerBase';
import { KBTemplate } from '../types';

export class ServerTemplates extends ServerBase {

  private _templates: KBTemplate[] = [];

  constructor(client: ScNet, keynodes: ServerKeynodes) {
    super(client, keynodes);
  }

  public async Initialize(): Promise<boolean> {
  
    this._templates = await this.LoadTemplatesFromKB();

    return new Promise<boolean>(function (resolve) {
      resolve(true);
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
    const usedTemplates: number[] = [];
    const titleAddrs: ScAddr[] = [];
    const scsAddrs: ScAddr[] = [];
    result.forEach((r: ScTemplateResult) => {
      const t: ScAddr = r.Get('_templ');

      if (usedTemplates.indexOf(t.value) < 0) {
        usedTemplates.push(t.value);

        const title: ScAddr = r.Get('_title');
        titleAddrs.push(title);

        const scs: ScAddr = r.Get('_scs_templ');
        scsAddrs.push(scs);
      }
    });

    // get titles
    let templates: KBTemplate[] = [];
    if (titleAddrs.length > 0) {
      const titles: ScLinkContent[] = await self.client.GetLinkContents(titleAddrs);
      const scsValues: ScLinkContent[] = await self.client.GetLinkContents(scsAddrs);

      for (let i = 0; i < titles.length; ++i) {
        templates.push({
          addr: usedTemplates[i],
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

    const result: ScTemplateSearchResult = await this.client.TemplateSearch(scsTempl);

    return new Promise<ScTemplateSearchResult>(function (resolve) {
      resolve(result);
    });
  };
};
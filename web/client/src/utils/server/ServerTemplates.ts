import { ScNet, ScType, ScAddr, ScTemplate, 
         ScTemplateSearchResult, ScTemplateResult, ScLinkContent } from '@ostis/sc-core';
import { ServerKeynodes } from './ServerKeynodes';
import { ServerBase, Store } from './ServerBase';

import * as actions from '../../store/actions/kbViewActions';

export class ServerTemplates extends ServerBase {

  constructor(client: ScNet, keynodes: ServerKeynodes, store: Store) {
    super(client, keynodes, store);
  }

  public async Initialize(): Promise<boolean> {
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
    if (titleAddrs.length > 0) {
      const titles: ScLinkContent[] = await self.client.GetLinkContents(titleAddrs);
      const scsValues: ScLinkContent[] = await self.client.GetLinkContents(scsAddrs);

      for (let i = 0; i < titles.length; ++i) {
        const title: string = titles[i].data as string;
        const scs: string = scsValues[i].data as string;

        this.store.dispatch(actions.addTemplate(title, true, scs));
      }
    }

    return new Promise<boolean>(function (resolve) {
      resolve(true);
    });
  }
};
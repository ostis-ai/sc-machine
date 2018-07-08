import { ScNet, ScAddr, ScType, ScIdtfResolveParams, ResolveIdtfMap } from '@ostis/sc-core';

const sNrelSystemIdentifier: string = 'nrel_system_identifier';
const sNrelMainIdtf: string = 'nrel_main_idtf';
const sNrelWebTemplates: string = 'nrel_web_templates';
const sNrelSCsTemplate: string = 'nrel_scs_template';

// Test
const sTestUser: string = 'test_user';

export class ServerKeynodes {
  private _client: ScNet = null;

  private _kNrelSysIdtf: ScAddr = null;
  private _kNrelMainIdtf: ScAddr = null;
  private _kNrelWebTemplates: ScAddr = null;
  private _kNrelSCsTemplate: ScAddr = null;

  // test
  private _kTestUser: ScAddr = null;

  constructor(client: ScNet) {
    this._client = client;
  }

  public async Initialize(): Promise<boolean> {
    const self = this;
    return new Promise<boolean>(function (resolve, reject) {
      const keynodesList: ScIdtfResolveParams[] = [
        { idtf: sNrelSystemIdentifier,  type: ScType.NodeConstNoRole },
        { idtf: sNrelMainIdtf,          type: ScType.NodeConstNoRole },
        { idtf: sNrelWebTemplates,      type: ScType.NodeConstNoRole },
        { idtf: sNrelSCsTemplate,       type: ScType.NodeConstNoRole },

        { idtf: sTestUser,              type: ScType.NodeConst },
      ];

      self._client.ResolveKeynodes(keynodesList).then(function (res: ResolveIdtfMap) {

        self._kNrelSysIdtf = res[sNrelSystemIdentifier];
        self._kNrelMainIdtf = res[sNrelMainIdtf];
        self._kNrelWebTemplates = res[sNrelWebTemplates];
        self._kNrelSCsTemplate = res[sNrelSCsTemplate];

        self._kTestUser = res[sTestUser];

        let resValue: boolean = true;
        for (let i = 0; i < keynodesList.length; ++i) {
          const idtf: string = keynodesList[i].idtf;
          const addr: ScAddr = res[idtf];
          console.log(`Resolve keynode ${idtf} = ${addr.value}`);

          resValue = resValue && addr.isValid();
        }

        resolve(resValue);
      });
    });
  }

  public get kNrelSysIdtf(): ScAddr { return this._kNrelSysIdtf; }
  public get kNrelMainIdtf(): ScAddr { return this._kNrelMainIdtf; }
  public get kNrelWebTemplates(): ScAddr { return this._kNrelWebTemplates; }
  public get kNrelSCsTemplate(): ScAddr { return this._kNrelSCsTemplate; }

  public get kTestUser(): ScAddr { return this._kTestUser; }
};
import { ScNet, ScAddr, ScType, ResolveIdtfMap } from '@ostis/sc-core';

const sNrelSystemIdentifier: string = 'nrel_system_identifier';

// device types
const sDeviceTV: string = 'device_tv';

const sSelf: string = 'self';
const sMyHome: string = 'my_home';
const sMainNLDialogue: string = 'main_nl_dialogue_instance';
const sUser: string = 'ui_user';
const sUserRegistered: string = 'ui_user_registered';

const sTestUser: string = 'denis_koronchik';

export class ServerKeynodes {
  private _client: ScNet = null;

  private _kNrelSysIdtf: ScAddr = null;

  constructor(client: ScNet) {
    this._client = client;
  }

  public async Initialize(): Promise<boolean> {
    const self = this;
    return new Promise<boolean>(function (resolve, reject) {
      const keynodesList = [
        sNrelSystemIdentifier,
      ];

      self._client.ResolveKeynodes(keynodesList).then(function (res: ResolveIdtfMap) {

        self._kNrelSysIdtf = res[sNrelSystemIdentifier];

        let resValue: boolean = true;
        for (let i = 0; i < keynodesList.length; ++i) {
          const idtf: string = keynodesList[i];
          const addr: ScAddr = res[idtf];
          console.log(`Resolve keynode ${idtf} = ${addr.value}`);

          resValue = resValue && addr.isValid();
        }

        resolve(resValue);
      });
    });
  }

  public get kNrelSysIdtf(): ScAddr { return this._kNrelSysIdtf; }
};
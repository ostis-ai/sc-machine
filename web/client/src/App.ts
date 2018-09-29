
import { MainMenu } from './ui/MainMenu';
import { ServerRoot, ServerKeynodes, ServerTemplates } from './utils/server';
import { DashBoardView, KBView } from './ui';
import { BaseView } from './ui/BaseView';
import { PageDimmer } from './ui/PageDimmer';

export class App {
  
  private static _instance: App = null;

  private _dimmer: PageDimmer = null;
  private _mainMenu: MainMenu = null;
  private _serverRoot: ServerRoot = null;

  private _dashView: DashBoardView = null;
  private _kbView: KBView = null;
  private _activeView: BaseView = null;

  public static Instance() : App {
    if (!App._instance) {
      App._instance = new App();
    }
    return App._instance;
  }

  public static Keynodes() : ServerKeynodes {
    return App.Instance().serverRoot.keynodes;
  }

  public static Templates() : ServerTemplates {
    return App.Instance().serverRoot.templates;
  }

  private constructor() {
  }

  public Initialize() {
    this._serverRoot = new ServerRoot(
      'ws://localhost:8090/ws_json',
      {
        changeInitStateCallback: this.OnInitStateChanged.bind(this),
        changeNetworkStateCallback: this.OnNetworkStateChanged.bind(this),
        initializedCallback: this.OnServerInitialized.bind(this),
      }
    );

    this.InitDimmer();
    this._serverRoot.Start();
    this.InitUI();
  }

  private InitDimmer() {
    const mainDiv: HTMLElement = document.getElementById('content');
    this._dimmer = new PageDimmer(mainDiv);
    this._dimmer.Show();
  }

  private InitUI() {
    const mainDiv: HTMLElement = document.getElementById('content');
    this._mainMenu = new MainMenu(mainDiv);

    // create views
    this._dashView = new DashBoardView(mainDiv);
    this._kbView = new KBView(mainDiv);

    const self = this;
    this._mainMenu.OnDashSelected(() => {
      self.activeView = self._dashView;
    });

    this._mainMenu.OnKBSelected(() => {
      self.activeView = self._kbView;
    });

    this.activeView = this._dashView;
  }

  private OnInitStateChanged(text: string) {
    this._dimmer.text = text;
  }

  private OnNetworkStateChanged(isConnected: boolean) {
    this._mainMenu.OnNewNetworkStatus(isConnected);
  }

  private OnServerInitialized(isSuccess: boolean) {
    if (isSuccess) {
      this._dimmer.Hide();
    }
  }

  protected set activeView(view: BaseView) {
    if (view === this._activeView) {
      return; // do nothing
    }

    if (this._activeView) {
      this._activeView.Hide();
    }

    this._activeView = view;

    if (this._activeView) {
      this._activeView.Show();
    }
  }

  public get serverRoot() : ServerRoot {
    return this._serverRoot;
  }

  //           <Dimmer active>
  //             <Loader size='massive'>{initText + '...'}</Loader>
  //           </Dimmer>
  //         )}
}

import { clearLine } from "readline";

type ClickFunction = () => void;

class MenuItem {
  private _item: HTMLElement = null;
  
  constructor(menu: HTMLElement, iconName: string, text: string = null) {
    this._item = document.createElement('a');
    
    this._item.setAttribute('class', 'item');

    this._item.innerHTML = 
      `<i class="icon ${iconName}"></i>
       <span></span>
      `;
    
    menu.appendChild(this._item);

    if (text) {
      this.text = text;
    }
  }

  protected set text(value: string) {
    this._item.querySelector('span').textContent = value;
  }

  protected get icon() : HTMLElement {
    return this._item.querySelector('i');
  }

  public set OnClick(func: ClickFunction) {
    this._item.addEventListener('click', () => {
      if (func) {
        func();
      }
    });
  }
}

class NetStatus extends MenuItem {

  private _status: boolean = true;

  constructor(menu: HTMLElement) {
    super(menu, 'signal');

    this.status = false;
  }

  private UpdateState() {
    const getColor = function(flag: boolean) {
      return flag ? 'green' : 'red';
    };

    this.icon.classList.remove(getColor(!this._status));
    this.icon.classList.add(getColor(this._status));

    this.text = this._status ? 'Connected' : 'Diconnected';
  }

  public set status(isConnected: boolean) {

    if (isConnected === this._status) {
      return; // do nothing
    }

    this._status = isConnected;
    this.UpdateState();
  }
}

/// -----------------------------------
export class MainMenu {

  private _netStatus: NetStatus = null;
  private _dashItem: MenuItem = null;
  private _kbItem: MenuItem = null;
  
  constructor(parent: HTMLElement) {
    this.InitHtml(parent);
  }

  private InitHtml(parent: HTMLElement) {
    const menu: HTMLElement = document.createElement('div');
    menu.setAttribute('class', 'ui top attached inverted icon labeled menu');

    this._netStatus = new NetStatus(menu);
    this._dashItem = new MenuItem(menu, 'home', 'Dashboard');
    this._kbItem = new MenuItem(menu, 'sitemap', 'Knowledge Base');

    parent.appendChild(menu);
  }

  public OnNewNetworkStatus(isConnected: boolean) {
    this._netStatus.status = isConnected;
  }

  public OnDashSelected(callback: ClickFunction) {
    this._dashItem.OnClick = () => {
      callback();
    };
  }

  public OnKBSelected(callback: ClickFunction) {
    this._kbItem.OnClick = () => {
      callback();
    };
  }
}

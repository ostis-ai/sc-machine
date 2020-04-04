
export class BaseView {

  private _title: string = '';
  private _container: HTMLElement = null;

  constructor(title: string, parent: HTMLElement) {
    this._title = title;
    
    this.CreateHtml(parent);
    this.Hide();
  }

  private CreateHtml(parent: HTMLElement) {
    const container: HTMLElement = document.createElement('div');
    container.setAttribute('class', 'ui container base-page');

    const header: HTMLElement = document.createElement('h2');
    header.setAttribute('class', 'ui header')
    header.innerHTML = this.title;

    parent.appendChild(container);
    container.appendChild(header);

    this._container = container;
  }

  public get title() : string {
    return this._title;
  }

  public get container() : HTMLElement {
    return this._container;
  }

  public Show() {
    this._container.classList.remove('hidden');
  }

  public Hide() {
    this._container.classList.add('hidden');
  }
}
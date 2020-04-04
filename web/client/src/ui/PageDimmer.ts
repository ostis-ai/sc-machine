

export class PageDimmer {

  private _element: HTMLElement = null;

  constructor(parent: HTMLElement) {

    this._element = document.createElement('div');
    this._element.setAttribute('class', 'ui page dimmer');

    this._element.innerHTML = '<div class="ui large text loader">Loading</div>';

    parent.appendChild(this._element);
  }

  public Hide() {
    this._element.classList.remove('active');
  }

  public Show() {
    this._element.classList.add('active');
  }

  public set text(value: string) {
    this._element.querySelector('div').innerText = value;
  }
}
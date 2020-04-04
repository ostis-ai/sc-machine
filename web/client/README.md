# README
The project fully using TypeScript. Components are added to the DOM as strings

## Example of button with handler creation
Let's see an example of ui components creation and it's operation with sc-memory.
We will create a button for node creation and button for it's visualization in SCg.

Firs of all we need to open needed file. In our case `SCsInputContainer.ts`. We can find these lines:
```js
buttonsGroup.innerHTML = `
    <button class="ui green left bottom attached button" scs-search>Search</button>
    <button class="ui bottom attached button" scs-create>Create</button>
    <button class="ui disabled red right bottom attached button" scs-generate>Generate</button>
`;
```

This code block shows addition of HTML-elements to the DOM tree.

Then we need to create event:
```js
private get scsCreateButton() : HTMLElement {
    return this.scsButtonsContainer.querySelector('button[scs-create]');
}
```

This getter will give us button component. Inside `InitHtml` we can find onclick event binding:
```js
this.scsCreateButton.onclick = () => {
    this.DoCreate(this._scsEditor.content)
}
```

## Example of connection to sc-memory
In onclick event we are bind already we will create node with system identifier and main identifier (identifier received in SCs-editor).
The purpose of `DoCreate` is to handle input text, call query to sc-memory and wait for the answer.

```js
public async DoCreate(scsContent: string) {
    if (this._onNewCreateRequestCallback) { // проверяем наличие метода с запросом
        const result: boolean = await this._onNewCreateRequestCallback(scsContent); // call the method and pass input text to it
    }

    return new Promise<void>((resolve) => {
        resolve();
    });
}
```

Let's move to the `KBView` class inside `KnowBaseView.ts`.

```js
private async OnRequestCreate(scsText: string): Promise<boolean> {
    const createResult: ScAddr = await App.Templates().DoCreate(scsText);

    return new Promise<boolean>((resolve) => {
        resolve(true);
    })
}
```

Then `ServerTemplates` class inside `ServerTemplates.ts`.

```js
public async DoCreate(scsTempl: string): Promise<ScAddr> {
    const addr: ScAddr = await this.CreateNodeWithMainIdentifier(scsTempl);
    this._addr = addr;
    alert(JSON.stringify(addr));
    return new Promise<ScAddr>((resolve) => {
        resolve(addr);
    });
}
```

Method of node generation with specified system and main identifier:
```js
public async CreateNodeWithMainIdentifier(identifier: string): Promise<ScAddr> {
    let construction = new ScConstruction();
    construction.CreateNode(ScType.Node, 'node')
    construction.CreateLink(ScType.Link, new ScLinkContent(identifier, ScLinkContentType.String), 'link')
    construction.CreateEdge(ScType.EdgeDCommonConst, 'node', 'link', 'edge')
    construction.CreateEdge(ScType.EdgeAccessConstPosPerm, this.keynodes.kNrelMainIdtf, 'edge');
    construction.CreateEdge(ScType.EdgeAccessConstPosPerm, this.keynodes.kNrelSysIdtf, 'edge');
    let addr = await this.client.CreateElements(construction);
    return addr[0];
}
```
Method of main identifier search by given ScAddr:

```js
  public async GetMainIdentifier(addr: ScAddr): Promise<ScTemplateSearchResult> {
    let scTemplate = new ScTemplate();
    let linkAlias = "link";
    scTemplate.TripleWithRelation(addr, ScType.Unknown, [ScType.Link, linkAlias], ScType.Unknown, this.keynodes.kNrelMainIdtf);
    let result = await this.client.TemplateSearch(scTemplate);
    return result;
  }
```
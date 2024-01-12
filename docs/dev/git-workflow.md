### Initialize

To initialize your repo do:

* make fork from `https://github.com/ostis-ai/sc-machine`
* clone your fork to your machine and prepare

```sh
git clone git@github.com:yourlogin/sc-machine.git
cd sc-machine
git config --global user.name "Your Name"
git config --global user.email youremail@example.com
git remote add upstream git@github.com:ostis-ai/sc-machine.git
```

### Update

To update your main from `upstream` use:

```sh
git remote update upstream
git fetch upstream
git checkout upstream/main
```

### Rebase

To rebase your branch to master:
```sh
git checkout <yourbranch>
git rebase upstream/main
```

If you have any problems, then:
* redo

```sh
git rebase --abort
```

* ask in [Element](https://app.element.io/index.html#/room/#ostis_tech_support:matrix.org)

### Common rules
* use `git rebase` instead of `merge`. [More documentation about this command](https://git-scm.com/docs/git-rebase) This command just try to apply your commits (from current branch to commits in specified branch)

### Git hooks

To setup `pre-commit` hook run:
```shell
pre-commit install
```

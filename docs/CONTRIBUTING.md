This page describes rules to contribute changes and features by Pull Requests creating.

## Initialize

To initialize your repo do:

* Fork `https://github.com/ostis-ai/sc-machine`.
* Clone your fork to your machine and prepare (see [Readme](https://github.com/ostis-ai/sc-machine)).

```sh
git clone git@github.com:yourlogin/sc-machine.git
cd sc-machine
git remote add upstream git@github.com:ostis-ai/sc-machine.git
```

* To update your `main` from `upstream` use:

```sh
git fetch upstream
git checkout upstream/main
```

* Use `git rebase` instead of `merge`. See [documentation about this command](https://git-scm.com/docs/git-rebase). To rebase your branch against main use:

```sh
git checkout <yourbranch>
git rebase upstream/main
```

* If you have any problems, then redo:

```sh
git rebase --abort
```

* Or ask in [Element](https://app.element.io/index.html#/room/#ostis_tech_support:matrix.org).

## Commits message format

Each commit message should be formed as: `[tag1]...[tagN] Message text (#issue)`.

Message text should start from an upper case letter. If commit doesn't fix or implement any #issue, then it shouldn't be pointed in commit message.

Examples:
<pre>
[cpp] Colored log output
[cpp][test] Add unit test for ScEvent class
[kpm][search] Relation type check added
</pre>

Possible tags:

  * `[build]` - changes in build system;
  * `[memory]` - changes in `sc-memory` module;
  * `[kpm]` - changes in `sc-kpm` module;
  * `[tests]` or `[test]` - changes in tests;
  * `[tools]` - changes in `sc-tools`;
  * `[server]` - changes in `sc-server` module;
  * `[builder]` - changes in `sc-builder` module;
  * `[config]` - commits with changes in configuration;
  * `[review]` - commits with review fixes;
  * `[refactor]` - commits with some code refactoring;
  * `[changelog]` - use when you update changelog;
  * `[docs]` or `[doc]` - use when you update documentation;
  * `[docker]` - changes in Dockerfile, .dockerignore or Docker image build pipeline;
  * `[scripts]` - updates in the `sc-machine/scripts` files;
  * `[ci]` - changes in `ci` configuration or scripts;
  * `[git]` - changes in `git` configuration;
  * `[cmake]` - changes in `cmake` build system.

Each commit in Pull Request should be an atomic. In other words, it should implement or fix one feature. For example:
<pre>
Last commit
...
[cpp] Colored log output
[cpp] Add class to work with console
...
Init commit
</pre>

In this example we add class to work with console (where implemented colored output), then in another commit we add implementation of colored log output.

***
Each commit should have not much differences excluding cases, with:

  * CodeStyle changes; 
  * Renames; 
  * Code formatting.

**Do atomic commits for each changes.** For example if you rename some members in `ClassX` and `ClassY`, then do two commits:
<pre>
[refactor] Rename members in ClassX according to codestyle
[refactor] Rename members in ClassY according to codestyle
</pre>

**Do not mix codestyle changes and any logical fixes in one commit.**

All commits that not follow these rules should be split according to these rules. Otherwise they will be rejected with Pull Request.

***

## Pull request

### Pull Request Preparation

 - Read rules to create PR in documentation;
 - Update changelog;
 - Update documentation;
 - Cover new functionality by tests;
 - Your code should be written according to a [codestyle](docs/dev/codestyle.md).

### Pull Request creation

 - Create PR on GitHub;
 - Check that CI checks were passed successfully;

### Pull Request Review

 - Reviewer tests code from PR if CI doesn't do it;
 - Reviewer submits review as set of conversations;
 - Author makes review fixes at `Review fixes` commits;
 - Author re-requests review;
 - Reviewer resolves conversations and approves PR if conversations were fixed.

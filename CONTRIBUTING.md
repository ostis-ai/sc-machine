This page describes rules to contribute changes and features by Pull Requests creating.

## Commits message format

Each commit message should be formed as: `[tag1]...[tagN] Message text (#issue)`.

Message text should start from an upper case letter. If commit doesn't fix or implement any #issue, then it shouldn't 
be pointed in commit message.

Examples:
<pre>
[cpp] Colored log output
[cpp][test] Add unit test for ScEvent class
[kpm][search] Relation type check added
</pre>

Possible tags:

  * `[memory]` - changes in `sc-memory` module;
  * `[kpm]` - changes in `sc-kpm` module;
  * `[tests]` or `[test]` - changes in tests;
  * `[tools]` - changes in `sc-tools`;
  * `[server]` - changes in `sc-server` module;
  * `[builder]` - changes in `sc-builder` module;
  * `[codegen]` - changes in `code generator`;
  * `[config]` - commits with changes in configuration;
  * `[review]` - commits with review fixes;
  * `[refactor]` - commits with some code refactoring;
  * `[changelog]` - use when you update changelog;
  * `[docs]` or `[doc]` - use when you update documentation;
  * `[docker]` - changes in Dockerfile, .dockerignore or Docker image build pipeline
  * `[scripts]` - updates in the `sc-machine/scripts` files
  * `[ci]` - changes in `ci` configuration or scripts;
  * `[git]` - changes in `git` configuration;
  * `[cmake]` - changes in `cmake` build system.

Each commit in Pull request should be an atomic. Another word implement or fix one feature. For example:
<pre>
Last commit
...
[cpp] Colored log output
[cpp] Add class to work with console
...
Init commit
</pre>

In this example we add class to work with console (where implemented colored output), then in another commit we had 
implementation of colored log output.

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

All commit, that not applies to this rules, should be split by this rules. Another way they will be rejected with Pull request.

***
## Pull request

Each pull request with many changes, that not possible to review (excluding codestyle, rename changes), will be rejected.

_**All commit, that not applies to this rules, should be split by this rules. Another way they will be rejected with Pull request.**_

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

 - Reviewer should test code from PR if CI don't do it;
 - Reviewer submit review as set of conversations;
 - Author make review fixes at `Review fixes` commits;
 - Author re-request review;
 - Reviewer resolve conversations if they were fixed and approve PR.


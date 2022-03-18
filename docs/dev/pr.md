This page describes rules to create Pull Requests.
### Commits
Each commit message should be formed as: `[tag1]...[tagN] Message text (#issue)`

Message text should start from an upper case letter. If commit doesn't fix or implement any `#issue`, then it shouldn't be pointed in commit message
  Examples:
<pre>
[cpp] Colored log output
[cpp][test] Add unit test for ScEvent class
[kpm][search] Relation type check added
</pre>

Possible tags:

  * `[cpp]` - use, when you made changes in cpp wrapper code;
  * `[memory]` - use, when you made changes in c core code;
  * `[tests]` or `[test]` - use, when you made changes in tests;
  * `[tools]` - use, when you made changes in tools;
  * `[builder]` - use, when you made changes in builder tools;
  * `[codegen]` - use, when you made changes in code generator;
  * `[review]` - use, when you made commit with review fixes;
  * `[refactor]` - use, when you made commit with some code refactoring;
  * `[changelog]` - use, when you update changelog;
  * `[docs]` - use, when you update documentation;
  * `[python]` - use, when you made commit with python module changes;
  * `[ci]` - changes in `ci` configuration or scripts;
  * `[git]` - changes in `git` configuration.

***
Each commit in Pull request should be an atomic. Another word implement or fix one feature. For example:
<pre>
Lat commit
...
[cpp] Colored log output
[cpp] Add class to work with console
...
Init commit
</pre>

 In this example we add class to work with console (where implemented colored output), then **in another commit** we had implementation of colored log output

***
Each commit should have not much differences excluding cases, with:

  * CodeStyle changes;
  * Renames;
  * Code formating;

**Do atomic commits for each changes.** For example if you rename some members in `ClassX` and `ClassY`, then do two commits:
<pre>
[refactor] Rename members in ClassX according to codestyle
[refactor] Rename members in ClassY according to codestyle
</pre>

**Do not mix codestyle changes and any logical fixes in one commit**

***
_**All commit, that not applies to this rules, should be split by this rules. Another way they will be rejected with Pull request.**_
### Pull request
Each pull request with many changes, that not possible to review (excluding codestyle, rename changes), will be rejected.
***
For each pull request you should do:

  * update `docs/changelog.md` file according to your commits
  * ensure prepare updates for documentation
  * cover new functionality [by tests](test.md)
  * your code should be written according to a [codestyle](codestyle.md)

***
After prepare pull request an automation tests will run on it. If any test doesn't passed, then pull request couldn't be merged and you should to fix it.
***
All review or other fixes to pull request should be appended as new commits to it

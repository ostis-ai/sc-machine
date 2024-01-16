## Knowledge base repository file - repo.path
For more complex KB setups (with several folders across the filesystem, for example), we've created an internal file format called `repo.path`. It allows you to specify all the folders that should be built into the KB or exclude some files or subfolders.
### Example
```sh
# Comments should start with hashtag as a first character in the line
# Here you can specify path to one or several kb folders
# Paths can be relative
../ims.ostis.kb
/full/path/to/kb
../custom_kb
# you can also exclude files or folders by adding a "!" symbol at the beginning of the line
!../ims.ostis.kb/ims/ostis_tech
!../custom_kb/test.scs
```

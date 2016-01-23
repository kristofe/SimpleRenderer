SimpleRenderer
==============

to generate a project to compile...

0)if there isn't a build directory create it.

1)switch to the build directory

2)
  FOR APPLE:
  run cmake .. -G "Xcode"
  or
  run cmake .. -G "Unix Makefiles"

  FOR LINUX
  run cmake .. -G "Unix Makefiles"

3) the makefile of ide project will now be in the cmake directory

4) the build directory should never be committed to the repo.



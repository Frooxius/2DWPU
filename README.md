# 2DWPU

# Building

Currently I do not have the project in a state that would build easily with latest versions of Visual Studio. If anybody would like to upgrade it, feel free to submit a PR! Here's some info:

The GUI simulator requires Qt 4.8: https://download.qt.io/archive/qt/4.8/4.8.0/

The Qt 4.8 requires Visual Studio Tools 2010 to link against the Qt libraries. There are two options:
- Compile Qt 4.8 for newer visual studio
- Compile the 2DWPU project with the VS2010 tools

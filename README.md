# TChess program with MFC GUI

As a school assignment to create a *GUI* application 
with *Microsoft Foundation Classes* I'm adding a GUI 
to my [console TChess application](https://github.com/Gtomika/tchess).

# Structure

In the root directory there are *MFC* related files, and in the 
*tchess* directory there are the files from the original 
TChess application, providing the chess logic.

# Download

When there will be a working version I'll put it in 
downloads, so you don't have to build it.

# How to build

This is a *Visual Studio* project, and should be imported 
and built there.

**Build variant:** Build the *Release* version, because the engine in the *Debug* version is VERY slow.

**Boost:** This project uses Boost library, so to get a succesful 
build you
 - must have Boost headers on your computer.
 - must change the include path to your local path to Boost. 
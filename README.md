# G Embedded Python Interpreter Toolkit

Experimenting with embedding the Python Interpreter into LabVIEW

![Block Diagram - LabVIEW code for calling a function from a Python Script](LabVIEW/img/call_function_test.png?raw=true)

## _I'm Sorry, What Now?_
An execellent question!

Python and LabVIEW are natural frenemies but what if you could call Python Scripts in LabVIEW? Wouldn't that be great? I certainly think so.

Then, what if we could also bring C++ to the party? Why, that would be the ultimate trio:

The ease of programming C++, the peformance of Python and the massive potential user base of literally tens of LabVIEW developers.

## _Aren't you forgetting that [the Python Integration Node](https://www.ni.com/docs/en-US/bundle/labview/page/glang/python_node.html) already exisits?_
I suppose next you will be saying that integrating Python into LabVIEW has definately been a thing since 2016 and probably before.

## _So Really, WHY?_
_... and why have you written this first README in the style of an imagined conversation?_

If I understand this properly, the Python Integration Node executes in a seperate process which means any data you pass needs to be bundled/packaged up and sent over some inter-process interface which can really slow things down when dealing with large arrays and images.

But... Python is written in C and supports being built into another application so we can construct a DLL which wraps up the interpreter and LabVIEW's memory manager functionality. The upshot is Python directly interacting with LabVIEW managed memory 👏.

Working directly with the Python's C interpreter inteface is tricky so we are going to use [pybind11](https://github.com/pybind/pybind11) to make life a bit easier.

(thank you for putting up with my choice of communication, this is a really experimental and hopefully fun project)

## Usage Dependencies
* LabVIEW 2020 SP1 on Windows
* Python 3.10 (other versions supported but require building of binaries) - usage of environmental managers (anaconda?) not tested or supported.
* [Caraya Toolkit](https://github.com/JKISoftware/Caraya)

## Developer Dependencies
* CMake 3.21 (or later)
* MSVC build tools (2017 or later)
* vscode (recommended) with C/C++ and CMake extensions
* [Dependencies](https://github.com/lucasg/Dependencies) (recommended) to check binaries can load their dependencies

## Building Binaries
* [clone this repo with submodules](https://stackoverflow.com/a/4438292/5609762)
* Use cmake to configure and build. The `.vscode-example` directory contains the settings for vscode C++ and CMake extensions
* **Do Not Use a Normal Debug Build** - the Python DLL loads lots of dependencies so unless you compiled a special debug build of Python, a mix of debug and non-debug libs will be loaded and cause issues. You can debug your C++ code using a _Release-With-Debug-Info_ build and the provided vscode `launch.json`. Build the `install` target to update the binary in the `LabVIEW/bin` directory.

## Contributions
Very welcome. Open an issue to discuss anything or to put me right on how the Python Integration node works.

## To Do
- [x] Setup Build System and Dependency Management
- [x] Simple Working Example - Evaluate Python Script and Call Functions
- [ ] Explore Passing Python Objects back to LabVIEW (In Progress)
- [ ] Explore Passing LabVIEW managed buffers and IMAQ images to Python
- [ ] Explore Passing Python allocated buffers back to LabVIEW
- [ ] Explore Multi-Threaded Operation (LabVIEW side only - Only a single Python Interpreter instance will be running due to limitations of pybind11 and acceptable levels of sanity and public order)

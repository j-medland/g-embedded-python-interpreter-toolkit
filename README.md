# G Embedded Python Interpreter Toolkit

Experimenting with embedding the Python Interpreter into LabVIEW. This is mostly a proof of concept to see what is possible and worthwhile.

![Block Diagram - LabVIEW code for calling a function from a Python Script](LabVIEW/img/call_class_example.png?raw=true)

```python
class Counter:
    x: int

    def __init__(self,startingValue : int):
        self.x = startingValue
    
    def increment(self):
        self.x += 1
        return self.x

    def decrement(self):
        self.x -= 1
        return self.x

```

## Motivation

Python and LabVIEW are natural frenemies but what if you could call Python Scripts in LabVIEW? Wouldn't that be great? I certainly think so.

Then, what if we could also bring C++ to the party? Why that would be the ultimate trio:

The ease of programming C++, the performance of Python and the massive potential user base of literally _tens_ of LabVIEW developers.

_So why not just use the [the Python Integration Node](https://www.ni.com/docs/en-US/bundle/labview/page/glang/python_node.html)?_

If I understand this properly, the Python Integration Node executes in a separate process which means any data you pass needs to be bundled/packaged up and sent over some inter-process interface which can really slow things down when dealing with large arrays and images.

But... the Python Interpreter is written in C and supports being built into another application so we can construct a DLL which wraps up the interpreter and LabVIEW's memory manager functionality. The upshot is Python directly interacting with LabVIEW-managed-memory 👏.

Working directly with Python's C interpreter interface is tricky so we are going to use [pybind11](https://github.com/pybind/pybind11) to make life a bit easier.

## Usage Dependencies
* LabVIEW 2020 SP1 for Windows or later
* Python 3.10 (other versions supported but require building of binaries) - usage of environmental managers (anaconda?) not tested.
* [Caraya Toolkit](https://github.com/JKISoftware/Caraya)

## Developer Dependencies
* [CMake](https://cmake.org/download/) (3.21 or later)
* [MSVC build tools](https://visualstudio.microsoft.com/downloads/) (2017 or later)
* vscode (recommended) with C/C++ and CMake extensions
* [Dependencies](https://github.com/lucasg/Dependencies) (recommended) to check binaries can load their dependencies

## Building Binaries
* [clone this repo with submodules](https://stackoverflow.com/a/4438292/5609762)
* Use cmake to configure, build and install. The `.vscode-example` directory contains the settings for vscode C++ and CMake extensions
* Debugging
    * If you have issues building the binaries in the _Debug_ configuration, try the alternative _Release-With-Debug-Info_. This can happen when some of the Python dependencies attempt to load both the Release and Debug versions, causing conflicts. Unfortunately any Release-type build will optimize unused code which makes debugging more difficult; Installing the Debug Files during Python installation may help. 
    * Settings for debugging the DLL in LabVIEW are provided in the vscode `launch.json`. Build the `install` target to update the binary in the `LabVIEW/bin` directory.

## Contributions
Very welcome. Open an issue to discuss anything or to put me right on how the Python Integration node works.

## To Do
- [x] Setup Build System and Dependency Management
- [x] Simple Working Example - Evaluate Python Script and Call Functions
- [x] Explore Passing Python Objects back to LabVIEW (In Progress - Done but adding more types)
- [ ] Explore Passing LabVIEW managed buffers and IMAQ images to Python (In Progress)
- [ ] Explore Passing Python allocated buffers back to LabVIEW
- [ ] Explore making it easy to add the LabVIEW and C++ code to handle custom LabVIEW/Python types
- [ ] Explore Multi-Threaded Operation (Currenly all CLFNs run in the UI thread which reduces peformance)

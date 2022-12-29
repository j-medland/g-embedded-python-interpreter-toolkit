# G Embedded Python Interpreter Toolkit

Experimenting with embedding the Python Interpreter into LabVIEW. This is mostly a proof of concept to see what is possible and worthwhile.

![Front Panel - The Moving Average Example](LabVIEW/img/moving-average-example.gif?raw=true)
![Block Diagram - LabVIEW code for calling a function from a Python Script](LabVIEW/img/moving-average-example.png?raw=true)

```python
import numpy as np

class MovingAverager:
    depth = 5
    insertAt = 0
    sampleSize = 0

    def __init__(self, sampleSize : int):
        self.memory = np.zeros(self.depth * sampleSize)
        self.sampleSize = sampleSize
    
    def add_sample(self, sample : np.ndarray):
        end = self.insertAt+self.sampleSize
        self.memory[self.insertAt:end] = sample
        self.insertAt = end % (self.depth * self.sampleSize)
        return self.memory.mean()

```

## Features

* Evaluate Scripts from File, import modules like normal
* Call functions, class constructors and class methods without a wrapper
* Pass LabVIEW Multi-Dimensional Arrays and IMAQ Images as Read-Only `numpy.ndarrays`
* Create and Casting Python Objects to-from LabVEW types (in progress)

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
* LabVIEW IMAQ and IMAQdx (for IMAQ based examples)

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
- [x] Explore Passing Python Objects back to LabVIEW (Done but adding more types)
- [x] Explore Passing LabVIEW managed buffers and IMAQ images to Python (Done but considering a Python-writable option)
- [ ] Explore Passing Python allocated buffers back to LabVIEW
- [ ] Explore making it easy to add the LabVIEW and C++ code to handle custom LabVIEW/Python types
- [x] Explore Multi-Threaded Operation (Results: Managing the GIL and object lifetimes leads to more crashes)

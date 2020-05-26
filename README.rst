libSLM Library for Selective Laser Melting
==================================================


libSLM is a c++ support library for the translation (reading and writing) of Machine build files commonyl used with commercial Selective Laser Melting (SLM) and DMLS systems. This is a re-write based on previous work to provide a vanilla c++ interface using std::classes, and also generating Python bindings via `pybind <https://pybind11.readthedocs.io/en/stable/>`_.

The library is used in conjunction with the `PySLM <https://github.com/drlukeparry/pyslm>`_ project in order to provide a framework for working with SLM systems.

The code does not generate any intermediate G-Code files, but simply imports and exports a collection of layers containing a number of layer geometries containg points, contours and scan vectors. 

Current Features
#################
Current importing and exporting capabilities available via libSLM:

* Renishaw (.MTT) File Format
* DMG Mori - Realizer (.rea) 
* EOS (.sli) WIP

Access to these specific translators are currently available on request as pre-compiled modules due to sensitiviy of the proprietary formats. The source code of these translators will be made available for research (non-commercial) purposes via requests at the discretion of the author until prior notice. 

Installation
#################
libSLM is fundamentally a c++ library for directly interfacing with machine build files used in commercial SLM systems and also for interpreting files for development and research purposes. 

No strict dependencies are required for compiling libSLM, originally based on the Qt library. This design decision was taken to improve the cross-platform behaviour of the project. Python bindings are generated via `pybind <https://pybind11.readthedocs.io/en/stable/>`_, which is automatically pulled in by as sub-module by calling git clone with `--recursive`. 


.. code:: bash

    git clone --recursive https://github.com/libSLM

Compiler requirements
**********************

**On Unix (Linux, OS X)**

* A compiler (GCC, Clang) with C++11 support
* CMake >= 2.8.12

**On Windows**

* Visual Studio 2015 (required for all Python versions, see notes below)
* CMake >= 3.1


Installation: Python - Compiling from Source
*********************************************

Just clone this repository and pip install. Note the `--recursive` option which is
needed for the pybind11 submodule:

.. code:: bash

    git clone --recursive https://github.com/libSLM
    pip install ./libSLM


With the `setup.py` file included in this example, the `pip install` command will
invoke CMake and build the pybind11 module as specified in `CMakeLists.txt`. The Cmake flag `BUILD_PYTHON` will be automatically toggled on during the build phase. 


Installation: Python Package
****************************

libSLM can be installed from pre-compiled versions in PyPi

.. code:: bash

    pip install libSLM

libSLM Library for Selective Laser Melting
==================================================


libSLM is a c++ support library for the translation (reading and writing) of Machine build files commonyl used with commercial Selective Laser Melting (SLM) and DMLS systems. This is a re-write based on previous work to provide a vanilla c++ interface using std::classes, and also generating Python bindings via `pybind <https://pybind11.readthedocs.io/en/stable/>`_.

The library is used in conjunction with the `PySLM <https://github.com/drlukeparry/pyslm>`_ project in order to provide a framework for working with SLM systems.

The code does not generate any intermediate G-Code files, but simply imports and exports a collection of layers containing a number of layer geometries containg points, contours and scan vectors. 

Current Features
******************
Current importing and exporting capabilities available via libSLM:

* Renishaw (.MTT) File Format
* DMG Mori - Realizer (.rea) 
* EOS (.sli) WIP

Access to specific translators are currently available on request:

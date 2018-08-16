.. _python_interface_tutorial:

Python interface
----------------


.. _install_open3d_module:

Install open3d from PyPi
========================

Open3D prebuilt binaries can be found at `open3d-python <https://pypi.org/project/open3d-python/>`_.

.. code-block:: sh

    pip install --user open3d-python
    # or
    pip3 install --user open3d-python
    # or
    python -m pip install --user open3d-python
    # or
    python3 -m pip install --user open3d-python

Open3D is supported on Ubuntu/macOS/Windows only on a standard/native Python distribution, **not Anaconda**. ``pip install open3d-python`` was tested and found to be working out of the box with:

* Windows, python installed from https://www.python.org/downloads/, 2.7 & 3.5 32bit and 64bit
* MacOS, system python 2.7
* Ubuntu, system python 2.7 and python 3.5 installed through apt.

Install open3d from source
==========================

For installing from source, see :ref:`getting_started_compilation`.

If Open3D is successfully compiled with Python binding, it will create a Python library with the name ``open3d``.
Typically, you will find a file ``open3d.so`` in ``build/lib/Python`` directory.


.. _import_open3d_module:

Import open3d module
====================

This tutorial shows how to import ``open3d`` module and print out help information.
For trouble shooting, see :ref:`python_binding`.

.. code-block:: python

    # src/Python/Tutorial/Basic/python_binding.py

    import numpy as np

    def example_help_function():
        import open3d
        help(open3d)
        help(open3d.PointCloud)
        help(open3d.read_point_cloud)

    def example_import_function():
        from open3d import read_point_cloud
        pcd = read_point_cloud("../../TestData/ICP/cloud_bin_0.pcd")
        print(pcd)

    if __name__ == "__main__":
        example_help_function()
        example_import_function()

This scripts has two functions: ``example_help_function`` and ``example_import_all``
that show very basic usage of Open3D Python module.

.. note:: Depending on environment, the name of Python library may not be ``open3d.so``. Regardless of the file name, ``import open3d`` should work.

.. code-block:: python

    def example_import_function():
        from open3d import read_point_cloud
        pcd = read_point_cloud("../../TestData/ICP/cloud_bin_0.pcd")
        print(pcd)

This imports ``read_point_cloud`` function from ``open3d`` module. It reads a point cloud file and returns an instance of ``PointCloud`` class. ``print(pcd)`` prints brief information of the point cloud:

.. code-block:: sh

    PointCloud with 198835 points.


.. _using_builtin_help_function:

Using built-in help function
````````````````````````````

It is recommended to use Python built-in ``help`` function to get definitions and instructions of Open3D functions and classes. For example,

.. code-block:: python

    def example_help_function():
        import open3d
        help(open3d)
        help(open3d.PointCloud)
        help(open3d.read_point_cloud)


Browse open3d
`````````````

``help(open3d)`` prints documents of ``open3d`` module.

.. code-block:: sh

    Help on module open3d:

    NAME
        open3d - Python binding of Open3D

    FILE
        /Users/myaccount/Open3D/build/lib/open3d.so

    CLASSES
        __builtin__.object
            CorrespondenceChecker
                CorrespondenceCheckerBasedOnDistance
                CorrespondenceCheckerBasedOnEdgeLength
                CorrespondenceCheckerBasedOnNormal
            DoubleVector
            Feature
            Geometry
                Geometry2D
                    Image
                Geometry3D
                    PointCloud
                    TriangleMesh
    :


Description of a class in open3d
````````````````````````````````

``help(open3d.PointCloud)`` provides description of ``PointCloud`` class.

.. code-block:: sh

    Help on class PointCloud in module open3d:

    class PointCloud(Geometry3D)
     |  Method resolution order:
     |      PointCloud
     |      Geometry3D
     |      Geometry
     |      __builtin__.object
     |
     |  Methods defined here:
     |
     |  __add__(...)
     |      __add__(self: open3d.PointCloud, arg0: open3d.PointCloud) -> open3d.PointCloud
     |
    :


Description of a function in open3d
```````````````````````````````````

``help(open3d.read_point_cloud)`` provides description of input argument and return type of ``read_point_cloud`` function.

.. code-block:: sh

    Help on built-in function read_point_cloud in module open3d:

    read_point_cloud(...)
        read_point_cloud(filename: unicode) -> open3d.PointCloud

        Function to read PointCloud from file

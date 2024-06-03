# Wenglor Vision Library POC
A Fancy Computer Vision Library by [Wenglor](https://www.wenglor.com)

<img src="logo-wenglor.svg" width=200 ></img>

## What is included so far:
* Matrix, Point, PointCloud and Structured point cloud types
* Polygonal Mesh type
* Saving/Loading to [E57](http://www.libe57.org/) format
* Saving/Loading to PLY format
* Loading from ASCII
* Statistical Outliers Removal for structured pointclouds
* Magic Filter for structured pointclouds
* Magic SOR for structured pointclouds
* Normals estimation for structured pointclouds
* C++ wrapper for ShapeDrive SDK
* Holes filling for structured pointclouds

## How to install the Library

```bash
mkdir build
cd build
cmake -DCMAKE_INSTALL_PREFIX=<your install dir> -DBUILD_TEST_APP=OFF ..
cmake --install .
```

## Build and install the Demo App

```bash
cmake -DCMAKE_INSTALL_PREFIX=<your install dir> -DBUILD_TEST_APP=ON ..
cmake --build . --config Release
cmake --install . --config Release
```

## Enjoy 😊


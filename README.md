# Monte Carlo Path Tracer
 - Some demo objects are downloaded from free3d website.
 - I referenced the rendering architecture from JamesGriffin.
## Features
 - Mesh Rendering
 - Obj Model Import
 - KdTree acceleration.
 - Ambient, refraction, Diffuse, specular, and emission material types.
 - Convenient configuration file.
 - Some example obj&mtl.
 - No extra library, easy to deploy on different platforms.
 - Eigen(SSE) acceleration.
## Compiling and Running
Require cmake to build the project.
OpenMP can be used to accerelate path tracing, but you can run it without it if you're not conventient to install it.(LLVM on Mac may not support OpenMP)

Compile:
lots of optimization instructions were assigned in RELEASE mode.
```
$ mkdir build
$ cd build
$ cmake .. -DCMAKE_BUILD_TYPE=RELEASE
$ make
```
Run:
`config.json` in `bin` directory will be used as configuration. I've set some example configurations for each object I've showed as demo in `obj`. You can just replace `config.json` with other configuration files and modify it to try different effects.
```
cd ../bin
./pathtracer
```
## Sample Images
![Cornell Box](bin/scene01/scene01.png?raw=true "Cornell Box")

![Bars](bin/scene02/scene02.png?raw=true "Bars")

![vase](bin/vase/vase.png?raw=true "vase")

![glass](bin/glass/glass.png?raw=true "glass")

![Glass_1](bin/Glass_1/Glass_1.png?raw=true "Glass_1")

![Glass_2](bin/Glass_2/Glass_2.png?raw=true "Glass_2")

![Toilet](bin/Toilet/Toilet.png?raw=true "Toilet")


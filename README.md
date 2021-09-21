# gRPC.Server.MQTT.PAHO.CPP
This project is a C++ gRPC wireless server that provides API functions of MQTT lib (Paho MQTT CPP).

- The gRPC server implement the simple MQTT functions (connect, disconnect, publish, subscribe)

- The proto file contains the description for those functions (connect, disconnect, publish, subscribe)

- After compiling the proto file, the generated C++ header files will contain a virtual methods for the MQTT functions,
so create a server that includes those header files and overrides those functions using the Paho MQTT library.

- Now, we can create a client that uses the same header files but as a client, and test the server calls.

*****************************************************************************************************************************************
*****************************************************************************************************************************************
*****************************************************************************************************************************************
**Install The Mosquitto MQTT Broker:**
```css

    sudo apt-add-repository ppa:mosquitto-dev/mosquitto-ppa
    sudo apt-get update
    sudo apt-get install mosquitto
    sudo apt-get install mosquitto-clients
    sudo apt clean
```
*****************************************************************************************************************************************
*****************************************************************************************************************************************
*****************************************************************************************************************************************
**Eclipse Paho MQTT C++ Client Library install:**

- Please note that the Paho C++ library requires the Paho C library, v1.3.8 or greater, to be built and installed first.
- The build process requires the following tools:
    CMake v3.5 or newer
    GCC v4.8 or newer or Clang v3.9 or newer
    GNU Make
```css
    sudo apt-get install build-essential gcc make cmake cmake-gui cmake-curses-gui
   ```
- If you will be using secure sockets: 
```css
    sudo apt-get install libssl-dev
 ```
- Building the documentation requires doxygen and optionally graphviz to be installed:
```css
    sudo apt-get install doxygen graphviz
```


**Building the Paho C library:**

- Before building the C++ library, first, build and install the Paho C library, if not already present. Note, this version of the C++ library requires Paho C v1.3.8 or greater.
```css
    git clone https://github.com/eclipse/paho.mqtt.c.git

    cd paho.mqtt.c
    git checkout v1.3.8

    cmake -Bbuild -H. -DPAHO_ENABLE_TESTING=OFF -DPAHO_BUILD_STATIC=ON \
        -DPAHO_WITH_SSL=ON -DPAHO_HIGH_PERFORMANCE=ON
    sudo cmake --build build/ --target install
    sudo ldconfig
```
- This builds with SSL/TLS enabled. If that is not desired, omit the -DPAHO_WITH_SSL=ON.

It also uses the "high performace" option of the C library to disable more extensive internal memory checks. Remove the PAHO_HIGH_PERFORMANCE option (i.e. turn it off) to debug memory issues, but for most production systems, leave it on for better performance.

To install the library to a non-standard location, use the CMAKE_INSTALL_PREFIX to specify a location. For example, to install into under the build directory, perhaps for local testing, do this:
```css
    cmake -Bbuild -H. -DPAHO_ENABLE_TESTING=OFF -DPAHO_BUILD_STATIC=ON \
        -DPAHO_WITH_SSL=ON -DPAHO_HIGH_PERFORMANCE=ON \
        -DCMAKE_INSTALL_PREFIX=./build/_install
  ```
**Building the Paho C++ library:**

- An example CMake build session might look like this:
```css
    git clone https://github.com/eclipse/paho.mqtt.cpp
    cd paho.mqtt.cpp

    cmake -Bbuild -H. -DPAHO_BUILD_STATIC=ON \
        -DPAHO_BUILD_DOCUMENTATION=TRUE -DPAHO_BUILD_SAMPLES=TRUE
    sudo cmake --build build/ --target install
    sudo ldconfig
 ```
 - If you did not install Paho C library to a default system location or you want to build against a different version, use the CMAKE_PREFIX_PATH to specify its install location. Perhaps something like this:
```css
     cmake -Bbuild -H. -DPAHO_BUILD_DOCUMENTATION=ON -DPAHO_BUILD_SAMPLES=ON \
        -DPAHO_BUILD_STATIC=ON \
        -DCMAKE_PREFIX_PATH=$HOME/mqtt/paho.mqtt.c/build/_install
 ```


*****************************************************************************************************************************************
*****************************************************************************************************************************************
*****************************************************************************************************************************************
**Build and locally install gRPC and Protocol Buffers**
**Setup**
- Choose a directory to hold locally installed packages. This page assumes that the environment variable MY_INSTALL_DIR holds this directory path. For example:
```css
    export MY_INSTALL_DIR=$HOME/local
```
- Ensure that the directory exists:
```css
    mkdir -p $MY_INSTALL_DIR
```
- Add the local bin folder to your path variable, for example:
```css
    export PATH="$MY_INSTALL_DIR/local/bin:$PATH"
```
**Install cmake**
- You need version 3.13 or later of cmake. Install it by following these instructions:
```css
    sudo apt install -y cmake
```
- Check the version of cmake:
```css
    cmake --version
```
**Install other required tools**
- Install the basic tools required to build gRPC:
```css
    sudo apt install -y build-essential autoconf libtool pkg-config
```
**Clone the grpc repo**
- Clone the grpc repo and its submodules:
```css
    git clone --recurse-submodules -b v1.38.0 https://github.com/grpc/grpc
```
**Build and install gRPC, Protocol Buffers, and Abseil**
- While not mandatory, gRPC applications usually leverage Protocol Buffers for service definitions and data serialization, and the example code uses proto3.

gRPC uses the Abseil C++ library, so it needs to be installed as well.

The following commands build and locally install gRPC, Protocol Buffers, and Abseil:
```css
    cd grpc
    mkdir -p cmake/build
    pushd cmake/build
    cmake -DgRPC_INSTALL=ON \
          -DgRPC_BUILD_TESTS=OFF \
          -DCMAKE_INSTALL_PREFIX=$MY_INSTALL_DIR \
          ../..
    make -j
    make install
    popd
    mkdir -p third_party/abseil-cpp/cmake/build
    pushd third_party/abseil-cpp/cmake/build
    cmake -DCMAKE_INSTALL_PREFIX=$MY_INSTALL_DIR \
          -DCMAKE_POSITION_INDEPENDENT_CODE=TRUE \
          ../..
    make -j
    make install
    popd
    
    cd grpcgit
    submodule update --init
    cd grpc/third_party/protobuf
    sudo make install
```
**use this method to continue installation of gRPC**
```css
    sudo apt-get install build-essential autoconf libtool pkg-config
    cd grpc
    git submodule update --init
    cd third_party/protobuf
    sudo make install
    cd third_party/protobuf
    ./autogen.sh
    ./configure
    make
    make check
    make install
    sudo ldconfig
```

**If you get this error "Package grpc was not found in the pkg-config search path.
Perhaps you should add the directory containing `grpc.pc'
to the PKG_CONFIG_PATH environment variable
No package 'grpc' found
" please write these lines.**
```css
    put the following files in /usr/local/lib/pkgconfig : gpr.pc, grpc.pc, grpc++.pc, grpc++_unsecure.pc, grpc_unsecure.pc
```

*****************************************************************************************************************************************
*****************************************************************************************************************************************
*****************************************************************************************************************************************
                    ***If the above installation is done successfully you can start testing***
```diff
-Working with mosquitto:
```
- Launching a mosquitto broker command: **mosquitto**
- When error address already in use occure type command: **ps -ef | grep mosquitto** and then type **sudo kill "process id"** to kill it.
- Starting a mosquitto subscriber command: **mosquitto_sub -h 127.0.0.1 -p 1883 -t test**
- Starting a mosquitto publisher command: **mosquitto_pub -h 127.0.0.1 -p 1883 -t test -m "I'm publishing !"**

```diff
-Executing MakeFile:
```
- Go to the files directory.
- Clean old files, after this command only these files will remain: Makefile, server.cc, client.cc, myServer.proto:
        **make clean**
- Building files with Make file, this command will generate 
  myServer.pb.o myServer.grpc.pb.o client.o files & 
  myServer.pb.o myServer.grpc.pb.o server.o files: 
        **make all**
```diff
-Getting started with Subscribe function example:
```
- Server launching command:
        **./server**
- Launch the broker (on a different terminal):
        **mosquitto**
- Client launching command (on a different terminal):
        **./client**
- 3 ==> Broker address: 0.0.0.0:1883 ==> This device id: first_try ==> Topic: test
     
- Message publishing command (on a different terminal):
        **mosquitto_pub -h 127.0.0.1 -p 1883 -t test -m "Hello!"**
- On the client terminal you should see the message Hello!

```diff
-Notes:
```
- In the above example we are working with local hosts, however you can 
  work via wifi all you have to do is changing the broker address in the client.cc script.
- If you get connection error while executing the client please make sure that you started the broker.
- If you want to sent a sentence use underscores example:
                     ***Bad***: ~~Hello my name is Bob~~/
                     ***Good***: Hello_my_name_is_Bob

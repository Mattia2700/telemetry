# Telemetry Usage
Is a large project so it needs some time to be installed properly.  
Documentation is always not exaustive enough but I'll try my best.  
This installation procedure is tested and written only for Linux, this is because is developed on Linux and the telemetry software is running on a RaspberryPi, sooo...

## Installation

### Requirements
- Required cmake and other stuff to compile the project:  
  ~~~
  sudo apt install build-essential cmake
  ~~~
- Required can-utils:  
  ~~~
  sudo apt install can-utils
  ~~~
- Required Boost libs, install them with:  
  ~~~
  sudo apt install libboost-iostreams-dev libboost-system-dev libboost-filesystem-dev
  ~~~

- Required Google Protobuf
  Download [lastest](https://github.com/protocolbuffers/protobuf/releases)  
  Extract files and cd into it.  
  ~~~
  ./configure
  make
  make check
  sudo make install
  sudo ldconfig
  ~~~

  or
  ~~~
  git clone https://github.com/protocolbuffers/protobuf.git
  cd protobuf
  git submodule update --init --recursive
  ./autogen.sh

  # Then

  ./configure
  make
  make check
  sudo make install
  sudo ldconfig # refresh shared library cache.
  ~~~

- Websocketpp  
  ~~~
  git clone git://github.com/zaphoyd/websocketpp.git  
  cd websocketpp  
  cmake .  
  sudo make install  
  ~~~

- GNU PLOT
  Sorry read the docs to install it
  ~~~
  git clone https://github.com/dstahlke/gnuplot-iostream.git
  ~~~

- Raspi Cam
  ~~~
  git clone https://github.com/rmsalinas/raspicam
  cd raspicam
  mkdir build
  cd build
  cmake ..
  make
  sudo make install
  sudo ldconfig
  ~~~

- LibHaru
  libharu is a submodule in this repo but needs a couple of commands to work properly.  
  ~~~
  cd thitdparty/libharu
  buildconf.sh
  ./configure
  ~~~

- OpenCV  
  This is the latest added so please wait for the installation, OpenCV is a HUGE library but for this project only a tiny part is required. This guide will avoid installing the whole library.

### Building
Once all the requirements are installed:
~~~bash
mkdir build
cd build
cmake ..
make
~~~
Most of the times running the cmake and make commands generates lots of errors, please contact the [developer](https://github.com/Pippo98) to be helped and to allow him to integrate this giude with possible solutions.

All the generated binaries are located in **bin** folder.

## Running
To run check if the device you are using has a CAN interface:
~~~bash
ifconfig
~~~
In the infos displayed make sure **can0**/**can1**/**vcan0** device is shown, if not: if you want to test on an actual CAN interface run `bash canSetup.sh`, if you want to use a symulated interface run `bash vcanSetup.sh`.

Then to run the actual telemetry use:
~~~
./bin/telemetry
~~~
The script will log lots of informations, check for eventual errors and contact the developer for help.

***
**Telemetry generates a debug file in which all the debug/warn/errors are logged.**  
This file is in home folder: **~/telemetry_debug.log**.
***

## Configurations
At the first telemetry run, it generates some .json files that can be used to configure how the code acts.
all configs are in home folder (**~**).  
The most important is **telemetry_config.json**. The most important fields are: **can_device, ws_enabled, ws_server_url**.

## Starting
Can be started with a can message:
~~~
0A0#6601
~~~
and stopped with:
~~~
0A0#6600
~~~
Using the telemetry [application](https://github.com/eagletrt/telemetry-app).


## Output
The data that telemetry produces can be found in:
~~~
~/logs/<date>/<race>
~~~
where date is the current date **dd_mm_yyyy**, race is the current session configuration, uses race and configuration to bould the folder name.
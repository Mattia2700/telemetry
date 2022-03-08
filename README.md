# Eagle TRT
Log messages received in CAN bus.  
Start and stop logging by sending specific command messages.  

> :warning: **THIS VERSION IS NOT COMPATIBLE WITH LOGS BEFORE 24/05/2021**

GPS messages reference:  
https://d1.amobbs.com/bbs_upload782111/files_44/ourdev_670625HVMMR6.pdf
http://www.scarpaz.com/attic/documents/GPS-NMEA.pdf    

## Contents
- [Telemetry](#telemetry)
- [CSV](#csv)
- [CSV to MAT](#csv-to-mat)

## Requirements
- Required Boost libs, install them with:  

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
  ~~~
  git clone https://github.com/dstahlke/gnuplot-iostream.git
  ~~~

To use **csv_to_mat.py** are needed some other python modules.  
Run ***pip3 install*** on each of the following:
~~~
tqdm
numpy
scipy
pandas
termcolor
~~~

The last thing is to install are some modules from a [github repo](https://github.com/Pippo98/ownModules.git). Follow there the **instructions**.  

# Telemetry
## Usage
Run the program.  
To start logging send:  
~~~
0A0#6601
~~~
To stop logging send:
~~~
0A0#6600
~~~

A configuration can me sent when starting logging.  
Can be configured pilot, race and circuit:
~~~
PILOTS = [
  "none",
  "Ivan",
  "Filippo",
  "Mirco",
  "Nicola",
  "Davide",
]
RACES = [
  "none",
  "Autocross",
  "Skidpad",
  "Endurance",
  "Acceleration",
]
CIRCUITS = [
  "none",
  "Vadena",
  "Varano",
  "Povo",
  "Skio",
]
~~~

Send the index of the parameter to be configured.  
Parameter 1: Pilot  
Parameter 2: Race  
Parameter 3: Circuit  
~~~
//      pprrcc
0A0#6601010302
~~~

pp stands for pilot   vector index expressed in hexadecimal.  
rr stands for race    vector index expressed in hexadecimal.  
cc stands for circuit vector index expressed in hexadecimal.  

So, in the start message, using parameters, the payload contains 4 parameters:
0: start
1: pilot
2: race
3: circuit


## OUTPUT

Logs file can be found in:  
~~~
~/Desktop/logs
~~~
At each start a new sub_directory is created like:  
~~~
20201215_183218_Mirco_Skidpad
Date     Time   Pilot Race
~~~

### CANDUMP
In the sub_folder will be created a file named **candump.log**.  
A file sample:  
~~~
*** EAGLE-TRT
*** Telemetry Log File
*** Tue May 11 16:15:52 2021

*** Pilot: Filippo
*** Race: Autocross
*** Circuit: Vadena

(1620742552.534499)	vcan0	4C5#B2234225
(1620742552.535561)	vcan0	1DF#2732285B85ADF70B
(1620742552.536678)	vcan0	0B9#F1EF2A213FC5D83A
(1620742552.537935)	vcan0	739#9EC0966545489804
(1620742552.539073)	vcan0	7D1#204E31137A7DAA7B
(1620742552.540215)	vcan0	135#F8F5D007F86B
(1620742552.541311)	vcan0	697#B842E511A8109277
(1620742552.542465)	vcan0	670#DB1B706B36FB9E1A
(1620742552.543557)	vcan0	6BE#A56C28179D00AA0D
(1620742552.544704)	vcan0	4DB#22AEA11995EDD25C
(1620742552.545826)	vcan0	108#D4B2AB17
(1620742552.546967)	vcan0	28D#86B821
~~~

### JSON
If the requirement is satisfied will be created **stat.json**.  
~~~
{
  "Date": "Tue May 11 18:47:52 2021\n",

  "Pilot": "Filippo",
  "Race": "Endurance",
  "Circuit": "Vadena",

  "Messages": 18353,
  "Average Frequency": 878,
  "Duration": 20.889837674
}
~~~


# CSV
Tool made to parse CANDUMP files in csv. It creates a CSV file for each device defined in CAN bus.  
The input files must be formatted like the telemetry output files.  

## Usage
Run:
~~~
./bin/csv
~~~
The program needs you to select only one folder containing some **.log** files.  
The algorithm searches in all sub-directories.  
It will start parsing files using all hardware threads available.  

## Output
The generated CSV files are in the same folder as the **.log** file.  
If the **.log** file has an integer name (0.log, 1.log, ...) it will create a folder called as the integer number, otherwise will create a **parsed** folder.
In the created folder can be found all the **.csv** files names like the CAN bus device.  

# CSV to MAT
Python tool that converts **.csv** files in **Matlab** files.  
Run:  
~~~
python csv_to_mat.py
~~~

Select a folder and let it parse all the files.  
The results can be found in the selected folder as a **.zip** archive.

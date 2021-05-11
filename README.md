# Telemetry
C++ interface to CAN bus  

Saving log files of can messages.  

## Requirements
Required Boost libs, install them with:  
~~~bash
sudo apt-get install libboost-all-dev
~~~
Optional JSONCPP:  
~~~bash
sudo apt-get install libjsoncpp-dev
~~~

## Usage
Run the program.  
To start logging send:  
~~~
0A0#6501
~~~
To stop logging send:
~~~
0A0#6500
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
0A0#6501010302
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

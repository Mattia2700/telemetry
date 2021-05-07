# Telemetry
C++ interface to CAN bus

## Telemetry
Saving log files of can messages.  

### Usage
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

# INDOORINO
# Smart home with Arduino and ESP8266.

## NOTE: this project is still under development. 

Indoorino is a multi platform packet system C/C++ library. It provides tools for user defined smart-homes. 
The whole system is made by a server on a local static address that manage connections from ESP devices and UI shells. Connection framework is asynchronous, packets are encrypted before sending and can be safe transimitted by a checksum.
ESP and AVR boards (usually Arduino) comincate via serial and provides ambient data and actuators to the server. UI shell is also a client but on a different port and let the user interact with the devices. Library provides the following components:

 1) Packets framework, upgradable and house control oriented.
 2) Asynchronous boost connection framework and server logic
 3) AVR and ESP framework for controlling sensors and actuators
 4) Python module for packet translation to dictionary (and vice-versa)
 5) Graphical UI
 6) Code generator script for packet managing
 
about me:

I have no idea why I'm writing all of this. Ah, yep, it's my hobby. But that's enough from me, let's here some valuable opinions

stackoverflow: your question is just like you, stupid and pointless.

linuxquestions: please read the man, it's written in there.

google: please stop asking me how to write "available" or "received".

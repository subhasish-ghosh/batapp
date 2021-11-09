# batapp
Battery Management

![Battery Logo](https://thumbs.dreamstime.com/t/battery-charging-icon-signs-symbols-can-be-used-web-logo-mobile-app-ui-ux-white-background-140818850.jpg)

This repository contains sources for the Battery Management software for ABC Ltd.

For Licensing information refer the LICENSE file.

## Testing

To test the application, goto the bin directory and execute the command on a cmd shell:
D:> batapp.exe CodingTest.bin

The output generated is as follows:
B;1;HIGH
S;3;0;1
B;7;MED
S;7;1;2
S;10;2;3
S;17;3;2
S;21;2;0

## Compilation

To compile the sources:

* Clone the repository using git bash:
	$> git clone git@github.com:subhasish-ghosh/batapp.git

* Use visual studio 2019 to import the solution file: batapp.sln

* Right-click on the solution "batapp" and build

* To execute, use Debug tab -> Start Debugging

This should print out the log as shown above.

## Adding a new packet type

To add a new packet type, follow these steps:

* add a new file similar to batapp_pktpower.c or batapp_pktstatus.c

* Define the packet operations as mentioned in batapp_pktops_t

* In the file batapp_pktparser.c, add an entry for the batapp_pktops_t

* In the file batapp_pkttypes.h:
    * add a _HDR macro for packet log header
    * add a packet typeentry in batapp_pkttypes_t
    * add a extern batapp_pktops_t to retrieve the packet object

## Support
For any support contact: support@batterymanagement.co.uk

# SBIG-CFW8-controller
Arduino based controller for the CFW8 filter wheel manufactured by SBIG

The filter wheel will intialized to the first position when powered on.
In this case the first filter is RED. The filters in sequence are Red, Green, Blue, Luminance and an open spot (labeled clear in the code).

Change char *filter_color[] = {"Red", "Green", "Blue","Lumin.", "Clear"};  // change names to suit your setup.


Implemented serial support for communication with INDI, ASCOM...

*********************************
Serial communication description:
*********************************

Sending a number from 1 to 5 ended with carriage return moves to appropriate filter and returns a filter name, for example:

'1\r' -> serial -> CFW
CFW -> 'Red' -> serial

Sending "9" retreives a current filter position:

'9\r' -> serial -> CFW
CFW -> 'Red' -> serial

Any other number returns an error message:

'8\r' -> serial -> CFW
CFW -> 'Error' -> serial

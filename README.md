# SBIG-CFW8-controller
Arduino based controller for the CFW8 filter wheel manufactured by SBIG


The filter wheel will intialized to the first position when powered on.
In this case the first filter is red. The filters in sequence are Red, Green, Blue, Luminance and an open spot (labeled clear in the code).
Change char *filter_color[] = {"Red", "Green", "Blue","Lumin.", "Clear"};  // change names to suit your setup.

Planning on adding serial support throught the arduino, with an ascom and indi driver for automation.

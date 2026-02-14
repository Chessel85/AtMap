# Accessible Atlas Introduction

This document gives information about Accessible Atlas version 0.0.2.
It scores over version 0.0.1 as performance is much improved and it has a few useful features.

Installation is still a little manual but hopefully not too difficult.  If you are interested please read on.

# System requirements

The Accessible Atlas runs on Windows currently and requires about 6Gb of disk space.  The maps are big and not optimized.

The application is written using QT which means, in theory, it should be portable to Mac but this is yet to be proven.  There maybe dependencies which prevent this from being easily done.

# to Download

You can [download Accessible Atlas with this link](https://www.dropbox.com/scl/fi/uer6hv7usvmq75gl5tqgm/Atlas.zip?rlkey=io5tooebvjblqq2k4wrf4an40&dl=0).

Unpack this zip file in the usual way.  It creates a folder called Atlas.

# To Run

To run Accessible Atlas you go into the Atlas folder and run 'Hello11.exe'.  It might work for you straight away.

If it doesn't work because you get a run-time DLL cannot be loaded error, then run the vc_redist.x64.exe which is in the same folder and is a C++ redistributable from Microsoft.

If this doesn't fix any problems you have then please let me know.  After all, getting it running on other people's devices is half the point.

# How To Use Accessible Atlas

When it starts up your screen reader should announce the application title and that you are in the compass control area.  
Here are key points on how to use it:

* The majority of the application is a vector map on the left showing countries of the world.  It defaults to the UK.
* The right column is divided into a top and bottom portion with the top part being direction keys and basic location information.
* The bottom part shows a list of bordering countries or seas.

To move, you can either press one of the cursor keys on your keyboard or tab to one of the compass buttons and press space.  Clicking on them works too.
The map will redraw and your new location and coordinates will be announced by your screen reader.  You can also tab beyond the direction buttons to get to a location label which your screen reader should also be able to read out.

The default step size when moving is 100km.  You can increase this by pressing the plus key on your keyboard or  tabbing to the plus button and hitting space.  the maximum step size is 1000km.  
To reduce the step size, press the minus key or tab to the minus button and hit space.  You can go all the way down to a step size of 1 metre.

# Bordering Countries And Seas

You can move between the compass buttons and the information panel with F6.  The information panel shows countries or seas bordering the current location.  Press tab to get to the list which you can arrow up and down.  Hit enter to  jump to the centre of this place.

Press F6 to return to the compass buttons so you can move around with the cursor keys again.

# Menus 

There is a menu bar with File, Map and Help.

* File just contains a quit menu tiem
* Map contains menu items for going to a specific coordinate specified in longitude and latitude  (entered in decimal format) and a search option where you can search for countries.  Enter the beginning letters and tab to the search button.
* Help contains a link to this document, licensing information and an About dialogue.

# Known Problems 

* Belgium is missing.  Sorry. Hope I can get it back for next time. 
* Moving left and right, as well as moving left or right,  also moves the user very slightly towards the equator
* At step sizes of more than 20 kilometres there are thin gaps between countries
* The map changes the width of objects depending on where the user is.  It is down to projections and I am not sure if it is a bug or a feature.

# Next Steps

* Develop an MSI for easier installation by users
* Port to Mac 
* Put it on GitHub so others can contribute 
* Add more  data like cities, lakes and other points of interest
* What's near by list ordered by distance or direction 
* Adding sonification to give audio feedback on what's around 
* Showing the next level down in detail for countries e.g. States for the US, counties in the UK, districts in France
* Jumping to a network view so road, river and rail networks can be navigated 

Plus a whole bunch more.

Chris G 11th August 2025
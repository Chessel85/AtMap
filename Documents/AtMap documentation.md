# AtMap Introduction

This document gives information about AtMap version 1.0.1.
It scores over version 1.0.0 as performance is improved and it has some new useful features.

# System requirements

AtMap runs on Windows currently and requires about 6Gb of disk space.  The maps are big and not optimized.

The application is written using QT which means, in theory, it should be portable to Mac but this is yet to be proven.  There maybe dependencies which prevent this from being easily done.

# to Download

You can [download the AtMap installer from Github](https://github.com/Chessel85/AtMap/releases).  
In the assets section look for AtMap.exe.  Download this installer and run it.

# How To Use AtMap

When it starts up your screen reader will announce the application title and that you are in the compass control area.  
Here are key points on how to use it:

* The majority of the application is a vector map on the left showing countries of the world.  It defaults to the UK.
* The right column is divided into a top and bottom portion with the top part being direction keys and basic location information.
* The bottom part shows a variety of tabs each containing useful information such as bordering countries or what is currently displayed on the map.

To move, you can either press one of the cursor keys on your keyboard or tab to one of the compass buttons and press space.  Clicking on them works too.
The map will redraw and your new location and coordinates will be announced by your screen reader.  You can also tab beyond the direction buttons to get to a location label which your screen reader should also be able to read out.

The default step size when moving is 100km.  You can increase this by pressing the plus key on your keyboard or  tabbing to the plus button and hitting space.  the maximum step size is 1000km.  
To reduce the step size, press the minus key or tab to the minus button and hit space.  You can go all the way down to a step size of 1 metre.

# Bordering Countries And Seas

You can move between the compass buttons and the information panel with F6.  The information panel shows countries or seas bordering the current location.  Press tab to get to the list which you can arrow up and down.  Hit enter to  jump to the centre of this place.

Press F6 to return to the compass buttons so you can move around with the cursor keys again.

# Menus 

There is a menu bar with File, View, Map and Help.

* File just contains a quit menu tiem
* View > Outline draws the current country with a high contrast version.  Use arrow keys to vary the intensity of the foreground and background. Escape returns to the main view.
* View > Copy to clipboard copies the map to the clipboard 
* Map contains menu items for going to a specific coordinate specified in longitude and latitude  (entered in decimal format) and a search option where you can search for countries.  Enter the beginning letters and tab to the search button.
* Map also allows you to vary the level of detail on the base layers.  The Layers option allows control of all layers.
* Help contains a link to this document, licensing information and an About dialogue.

# Known Problems 

* Outline view can draw some countries at an angle

# Next Steps

* Port to Mac 
* What's near by list ordered by distance or direction 
* Adding sonification to give audio feedback on what's around 
* Jumping to a network view so road, river and rail networks can be navigated 
* User defined markers

Plus a whole bunch more.

Chris Goodwin 19th April 2026
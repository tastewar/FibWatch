Upon seeing it, my father-in-law was immediately intrigued by our Fibonacci
Clock by Philippe Chretien (http://geekoclock.com/contact/). He asked me to 
"invent" a Fibonacci Watch for him. I dismissed the idea immediately, but
later came across TinyCircuits watch kit, and thought "Ah ha!"

So the watch, and I, owe a debt of gratitude to Philippe. The display was
obviously designed to mimic his, although the code for doing so is my own,
and I only looked at his after I wrote mine.

This watch will run on the TinyCircuits TinyDuino platform with the TinyScreen
and TinyShield RTC. The intent is to have it sleep almost all the time, awakening
only when a button is pressed. Buttons are as follows:

 |==========|
+1          2+
 |          |
 |          |
+3          4+
 |==========|


Button 1 when pressed will display the time until 12.5 seconds after released
Button 2 enters settings (menu) mode

When in menu mode, Button 1 exits, Button 3 retreats current item,
Button 4 advances current item, and Button 2 accepts input. Note that the 
buttons are orientation dependent, i.e. if you flip the screen, the buttons
adjust as well.

When first entering time setting mode, you can choose from a list of time zones.
Once the TZ is accepted, the change goes into effect immediately for the date
and time setting portion. We then go through Year, Month, Date, Hour, Minute,
Second. If Button 1 is pressed at any time in the process, the current and any
subsequent items are left as is, but any already accepted data takes effect. If
no input is detected for 30 seconds, proceed as if Button 1 was pressed.

If buttons 1 and 2 are pressed together, the time is displayed as text, and the
display is turned off 12.5 seconds later.

Settings Menus are as follows:

Settings: -- menu
========
Timezone -- text setting
	Hawaii
	Alaska
	Pacific
	Mountain
	Arizona
	Central
	Eastern
	Atlantic
	Nova Scotia
	UK
	CentralEU
	EasternEU
Minutes -- numeric 0-59
Hours -- numeric 0-23
Date -- numeric 1-{28,29,30,31}
Month -- numeric 1-12
Year -- numeric 2000-2099
Hardware -- menu

Hardware: -- menu
=========
Orientation -- text
	Default
	Inverted
Brightness -- numeric 0-15
DisplayTimeout -- numeric
Color Scheme -- text
	

To-Do List:
==========
==========

1. Sleep support
2. Menu!
3. Hardware Parameter Storage & Retrieval
4. Move big tables to PROGMEM
5. Battery state display?
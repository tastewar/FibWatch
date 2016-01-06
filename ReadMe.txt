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
Button 4 advances current item, and Button 2 accepts input.

When first entering time setting mode, you can choose from a list of time zones.
Once the TZ is accepted, the change goes into effect immediately for the date
and time setting portion. We then go through Year, Month, Date, Hour, Minute,
Second. If Button 1 is pressed at any time in the process, the current and any
subsequent items are left as is, but any already accepted data takes effect. If
no input is detected for 30 seconds, proceed as if Button 1 was pressed.

If buttons 1 and 2 are pressed together, the time is displayed as text, and the
display is turned off 12.5 seconds later.

Settings Menus are as follows:

Settings:
========
Timezone
Minutes
Hours
Date
Month
Year
Hardware

Timezone:
========
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

Hardware:
=========
Orientation
Brightness
Battery
Color Scheme

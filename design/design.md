# NVMR Video Display

This software is intended to replace the analog video display that exists
only in Washington/Atlanta with a new digital system, so that the cameras
can be viewed from around the layout.

Requirements:

1. Receive video from network IP cameras.  These cameras are assumed to be
setup to send video via IP multicast.  Likely through a Raspberry Pi.
2. Receive video from Soospy IP camera.  This wireless IP camera is small
enough to fit into a locomotive; currently a Proto 1000 RDC.
3. View up to four cameras at a time.
4. View a single camera.
5. Controlable by using only a numpad, so a full-size keyboard is not required.
6. User-selectable camera to view.
7. Runnable on a Raspberry Pi.
8. View a webpage as provided by JMRI

# Finding Videos

Two options:
1. Come up and scan the network for cameras(avahi for RPi cameras)
2. Read config file for the cameras.  Don't update the config file unless
something changes(?)

For network simplicity, we will do option 1.  Option 2 will be needed for
soospy cameras, but those cameras are a little bit away from being
operational.

# Keypad Control

The intention is to have the GUI controlled by a USB keypad in order to save
space.  This keypad could also be mounted to the wall for the aisle TV.

To view cameras, the \*  or + button is used to indicate that the command is for
cameras.  A single \*<enter> will cause the view to be reset to the default
settings.  \*<camera-num><enter> will view a specific camera.  Using the + key
will add a camera to the currently viewed cameras, up to a maximum of 4.  Old
views will be replaced by new ones.

Web pages are similar, except that they start with a period.  The default
webpage can always be found by doing .<enter>.

Basic Command examples:

\*2<enter> - View Camera 2
+3<enter> - Add camera 3 to viewport(multi-view)
\*<enter> - reset camera view to default
.<enter> - Go to web view
.2<enter> - View webpage 2

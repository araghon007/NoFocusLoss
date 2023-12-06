# NoFocusLoss
This is a utility that prevents games from pausing/muting on background by making sure the unfocus message doesn't get to it.

This will break some things but uhh

Uses injection since I can't really find any other way to do what I need.

## Usage

1. Launch NoFocusLossGUI.exe

![image](https://github.com/araghon007/NoFocusLoss/assets/10870921/c3a263ea-d35d-4521-adf3-c423694d8033)

2. Press Refresh. This may take a short while.

![image](https://github.com/araghon007/NoFocusLoss/assets/10870921/daed98f0-6078-485e-b026-77139057e38c)

3. From the list on top, select the window name of the game you want to fix. You can also type the name on your keyboard after clicking on any item for it to be selected. Magic!

![NoFocusLossGUI_3Eq3gTvu8R](https://github.com/araghon007/NoFocusLoss/assets/10870921/5eb3f0bd-2467-4cdb-b85d-dff27e9425c5)

4. Click on Inject

![NoFocusLossGUI_hNECUefsBt](https://github.com/araghon007/NoFocusLoss/assets/10870921/8dc1c1f8-f427-4b28-9614-c58f7ee01174)

5. Enjoy!

To revert the fix, click on the game in the bottom list and click Unload.

![image](https://github.com/araghon007/NoFocusLoss/assets/10870921/5394dfdc-1a10-425c-b27b-7d891f027c3b)


## Why?
Multimonitor users

## How?
Magic beyond my comprehension (I will forget how this entire repo works within a week)

Really though, it's just replacing the window's WndProc to block certain messages and detouring calls to GetForegroundWindow and SetCursorPos

## Features:
- Alt-tab during cutscenes without having to worry about them pausing or muting
- Play games on background using a controller as long as they're using xinput and not Windows.Gaming.Input (Thanks Microsoft)
- Bypass Unity's shit no good very bad input system that makes you unable to use controllers when the game isn't focused, even in VR
- Stops mouse capture by disabling calls to SetCursorPos (which is a dumb way to do capture in the first place)

## Limitations:
- For now only affects the main window of the injected program

## Drawbacks:
- Injection, maybe don't use it in multiplayer games

## Tested Games:
- Teardown
- Alan Wake
- Need For Speed 2015 (2016)
- I Expect You To Die 3 (yay, this fixed controller input)

## Known Issues:
- EA's Origin overlay appears frozen, until you try alt-tabbing which forces it to refresh

## TODO:
- Figure out automatic injection?
- Maybe a truly global solution?
- Try to do this with UWP apps (Yeah, good luck)
- Add support for doing this for multiple app windows, and specifying a window you want to do this with other than the supposedly main one
- Maybe add some way to force games that use SetCursorPos for mouse capture to use ClipCursor instead, a counter that would count the amount of SetCursorPos calls per second and then redirect?

### Thanks to pretty much all the same people from my X1nput repo, since I kinda used that as a base for this, plus whoever is mentioned in the code

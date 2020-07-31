<img src="https://github.com/stuomas/disorient/blob/master/disorient.png" width=250>

<h1>&#8291;</h1>

Disorient is an application that integrates your Windows PCs to your IoT fleet. It allows you to control various things over MQTT or a WebSocket. It is meant to coexist with home automation software like Home-Assistant to automatically perform tasks when a specific activity happens.

Some example use cases include

- DIY auto-pivot for a display. Build an Arduino/ESP with a tilt sensor and send Disorient a message when the display should be turned and to what orientation. This was the original purpose of the application.
- Change selected display orientation using global hotkeys `CTRL + ALT + <arrow key>`.
- Switch places of primary and secondary displays when starting to play games to allow fullscreen games to be played on your home theatre that would normally be the secondary display.
- Switch the default audio output device between headphones/speakers/theatre, for example when starting a gaming activity, or when your wireless headphones do not automatically become default when turned on (looking at you, Arctis).
- Launch a script or an application or execute PowerShell commands.

The first four examples are probably relevant only to a small niche, perhaps only me, but the last one might make Disorient useful even to you.

## Installation

No installation required. Launch the executable, choose settings and forget.

## Usage

Disorient accepts input from a MQTT broker or a WebSocket server. 

In the settings tab, set up the connection to your MQTT broker or WebSocket server, or both. In payload mapping section, select what incoming text payload runs what function. More information about available functions and their arguments in [Payload mapping](#payload-mapping).

### Payload mapping

Map any text coming from WebSocket or MQTT broker to a function of choice. The function list below explains what arguments are needed.

#### Functions

> Rotate screen (index, angle)

Rotates the display of given `index` to given `angle`. Angle can only be 0, 90, 180, or 270. Get the correct index from the display list in the Help tab. If all your displays are called *Generic PnP Monitor* it just means you have not installed the manufacturer's driver, or there isn't any, and it probably doesn't matter, their corresponding indices stay the same.

For example setting a following mapping

`set_portrait_1 `  `Rotate screen (index, angle)`  `0,90`

sets your first listed display to portrait mode when "set_portrait_1" message arrives from a subscribed MQTT topic or a WebSocket server.

> Arrange displays (index1, index2)

Swap places of two displays, the `index1` becoming primary display and `index2` non-primary, maintaining the same layout as set in Windows display settings. Get the correct indices from the display list in the Help tab. Not sure how well this works with more than two displays, let me know if it needs tweaking.

> Set audio device (name)

Set default (active) audio device. Get the correct `name` from the audio list in the Help tab, in exactly same format. This is the name that can be given to the device in Windows Sound Control Panel. Give the devices simple nicknames to make your life easier.

> Run executable (path)

Run a script or an executable. Give the full `path` as argument. File extensions `ps1`, `bat`, `cmd`, and `exe` supported.

#### Execute unrecognized payloads

Checking *Attempt to execute unrecognized payloads in PowerShell* will blindly try to execute any incoming payload that is not mapped to a function. This is dangerous, so use only if your systems are secure and you know what you are doing. It is better to write a script and use the *Run script* function. Checking Publish output will publish the standard output/error of the attempt in topic/output.

## Development

The current state of the application is "it works on my machine". If you have bugs to report or features to request, please do so.

For developing and building from sources, I recommend setting up Qt Creator, static build of Qt 5.15.0, and 64-bit MSVC 2019 compiler. Happy to give more details if you face troubles.


<a align="center" href="https://www.buymeacoffee.com/stuomas"><img src="https://cdn.buymeacoffee.com/buttons/default-orange.png" width=170></a>

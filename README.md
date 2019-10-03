<img align="left" src="https://github.com/stuomas/disorient/blob/master/disorient.png" width="250"/>
<h1>&#8291;</h1>
Not all monitors are created equal. If yours does not come with some sort of "auto pivot" functionality, you can build your own position sensor, attach it to the back of the monitor, and feed its output to Disorient.

### Installation
Only Windows is supported at the moment. You can compile the sources yourself using QtCreator, or download the release binary. No installation required. Set up and forget.

### Usage
Currently Disorient accepts external inputs only from a WebSocket server. It also allows changing the display orientation using global hotkeys (CTRL + ALT + ↑/→/↓/←) even without Intel graphics. 

Disorient acts as a WebSocket client. Set the WebSocket server address in the GUI. The server should send commands as text frames. The following text frames are expected, others are ignored:

`landscape` for default landscape view

`portrait` for portrait view (90 degrees counterclockwise)

`flandscape` for flipped landscape view (180 degrees)

`fportrait` for flipped portrait view (90 degrees clockwise)

### Development
Support for non-networking solutions such as basic Arduinos is planned with serial port inputs, but it is not implemented yet.

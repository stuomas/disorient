#Barebones WebSocket server implementation for testing purposes.
#Enter ws://localhost:8765 as websocket server address in Disorient.

import websockets
import asyncio
import sys

async def hello(websocket, path):
    name = await websocket.recv()
    print(f"Received: {name}")
    greeting = f"Hello, client!"
    await websocket.send(greeting)
    print(f"Sent: {greeting}")
    while True:
        msg = input("New message: ")
        if msg == "q":
            sys.exit()
        await websocket.send(msg)
        print(f"Sent: {msg}")

start_server = websockets.serve(hello, "localhost", 8765)

asyncio.get_event_loop().run_until_complete(start_server)
asyncio.get_event_loop().run_forever()

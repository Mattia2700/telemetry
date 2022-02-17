import json
import time
import asyncio
import websockets

async def on_message(ws, msg):
    try:
      msg = json.loads(msg)
    except Exception as e:
      print(msg)
      print(e)
      return
    if(msg["type"] == "ping"):
      data = {
        "type": "server_answer_ping",
        "time": (time.time() - msg["time"])
      }
      await ws.send(json.dumps(data))
      time.sleep(0.5)
      # await ws.send(json.dumps({"type": "server_get_all_ping"}))
      await ws.send(json.dumps({"type": "telemetry_kill"}))
      # await ws.send(json.dumps({"type": "server_get_num_connections"}))
    else:
      print("else ----- ", msg)

async def client():
    uri = "ws://localhost:3000"
    while True:
        try:
            async with websockets.connect(uri) as websocket:
                print("Connected")
                credentials = json.dumps({"identifier": "client"})
                await websocket.send(credentials)
                time.sleep(0.5)

                mm = json.dumps({"type": "server_collect_all_ping"})
                await websocket.send(mm)

                while True:
                  msg = await websocket.recv()
                  await on_message(websocket, msg)



        except KeyboardInterrupt:
            print("End by User")
            break
        except Exception as e:
            print("Exception: ", e)
            time.sleep(2)
            continue


asyncio.get_event_loop().run_until_complete(client())

const WebSocket = require('ws');

const ws = new WebSocket('ws://localhost:8080');

ws.on('open', function open() {
  ws.send(JSON.stringify({"identifier": "telemetry"}));
  a = setInterval(()=>{
    ws.send('something');
    var obj = {
      type: "update_data",
      data: "Updating data"
    }
    ws.send(JSON.stringify(obj));
  }, 1000);
});


ws.on('message', (msg)=>{console.log(msg.toString())});

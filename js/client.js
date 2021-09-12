const WebSocket = require('ws');

for(var i = 0; i < 10; i++){
  const ws = new WebSocket('ws://localhost:8080');

  ws.on('open', function open() {
    ws.send(JSON.stringify({identifier:"client"}));
  });

  ws.on('message', function incoming(message) {
    console.log('received: %s', message);
  });
}

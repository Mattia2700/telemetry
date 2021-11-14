/**
 * Opens a websocket server, can accept two types of connections:
 * - telemetry: the one who sends data that need to be broadcasted to the clients (can be only one)
 * - clients: multiple devices that receives messages coming from telemetry
 * The first message for each connection must be an authentication one, to define which type of connection will be established.
 * If the type is not recognized the connection is closed.
 * For each message from telemetry of type "update" will be send the "data" to all the clients
 */

const fs = require('fs');

let rawdata = fs.readFileSync('config.json');
let config = JSON.parse(rawdata);
console.log(config);

const WebSocket = require('ws');

console.log("Opening websocket with config: ", { port: config["port"], host: config["ip"] });
const wss = new WebSocket.Server({ port: config["port"], host: config["ip"] })
console.log("DONE");

var unauthenticated = []
var clients = [];
var telemetry = [];

MESSAGE_MALFORMED = JSON.stringify({ error: "message malformed" });

wss.on('connection', function connection(ws) {
  unauthenticated.push(ws);
  // insert the current websocket to the unauthenticate ones

  var unauthenticated_on_message = function (data) {
    ws.removeListener('message', unauthenticated_on_message);
    // Can be accepted only one message in this phase, so remove immediately the callback

    data = data.toString();
    console.log(data);
    var obj;
    try {
      obj = JSON.parse(data);
    } catch (e) {
      console.log(MESSAGE_MALFORMED);
      JSON.stringify({ error: MESSAGE_MALFORMED })
      ws.close();
      return;
    }

    // Can be accepted only JSON object
    if (obj.hasOwnProperty("identifier")) {
      // This os the key needed to recognize the type of connection

      var type = obj["identifier"];
      console.log(type);
      if (type == "telemetry") {
        ws.on('message', telemetry_on_message);
        ws.on('close', () => {
          ws.removeListener('message', telemetry_on_message)
          const index = telemetry.indexOf(ws);
          if (index > -1) {
            telemetry.splice(index, 1);
          }
        });
        telemetry.push(ws);
        // setup callbacks

      } else if (type == "client") {
        ws.on('close', () => {
          ws.removeListener('message', telemetry_on_message);
          const idx = clients.indexOf(ws);
          if (idx != -1) {
            clients.splice(idx, 1);
          }
        });
        clients.push(ws);
      }
    }
  };

  ws.on('message', unauthenticated_on_message);
});

function telemetry_on_message(data) {
  // data = data.toString();
  try {
    data_json = JSON.parse(data);
    if (!has_all_keys(data_json, ["type"]))
      throw ("json has not all keys required");
  } catch {
    telemetry.send(MESSAGE_MALFORMED);
    return;
  }
  // Can be accepted only JSON object

  if (data_json["type"] == "update_data") {
    var obj = {
      type: data_json["type"],
      data: data_json["data"]
    }
    // crete an object to be sent to the clients containing data coming from telemetry
  }
  broadcast_to_clients(data);
}






function broadcast_to_clients(message) {
  clients.forEach((client, i) => {
    client.send(message);
  });
}

function has_all_keys(obj, keys) {
  keys.forEach((item, i) => {
    if (!obj.hasOwnProperty(item))
      return false;
  });
  return true;
}

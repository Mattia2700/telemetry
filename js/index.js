const WebSocket = require('ws');

const wss = new WebSocket.Server({port: 8080})

var unauthenticated = []
var clients = [];
var telemetry = null;

MESSAGE_MALFORMED = JSON.stringify({error: "message malformed"});

wss.on('connection', function connection(ws) {
  unauthenticated.push(ws);

  var unauthenticated_on_message = function(data){
    ws.removeListener('message', unauthenticated_on_message);
    data = data.toString();
    var obj;
    try{
      obj = JSON.parse(data);
    }catch(e){
      console.log("Message is not JSON obj");
      ws.close();
      return;
    }

    if(obj.hasOwnProperty("identifier")){
      var type = obj["identifier"];
      if(type == "telemetry"){
        if(telemetry != null){
          ws.send(JSON.stringify({error:"Another telemetry exists"}));
          ws.close();
          return;
        }

        ws.on('message', telemetry_on_message);
        ws.on('close', ()=>{
          ws.removeListener('message', telemetry_on_message);
          telemetry = null;
        });
        telemetry = ws;
      }else if(type == "client"){
        ws.on('close', ()=>{
          ws.removeListener('message', telemetry_on_message);
          const idx = clients.indexOf(ws);
          if(idx != -1){
            clients.splice(idx, 1);
          }
        });
        clients.push(ws);
      }
    }
  };

  ws.on('message',unauthenticated_on_message);
});

function telemetry_on_message(data){
  data = data.toString();
  try{
    data = JSON.parse(data);
    if(!has_all_keys(data, ["type"]))
      throw("json has not all keys required");
  }catch{
    telemetry.send(MESSAGE_MALFORMED);
    return;
  }

  if(data["type"] == "update_data"){
    var obj = {
      type: data["type"],
      data: data["data"]
    }
    broadcast_to_clients(JSON.stringify(obj));
  }
}






function broadcast_to_clients(message){
  clients.forEach((client, i) => {
    client.send(message);
  });
}

function has_all_keys(obj, keys){
  keys.forEach((item, i) => {
    if(!obj.hasOwnProperty(item))
      return false;
  });
  return true;
}

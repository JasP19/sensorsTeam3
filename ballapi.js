var express = require("express");
var path = require('path');
var bodyParser = require('body-parser');
var urlencodedParser = bodyParser.urlencoded({ extended: true });
var app = express();
const WebSocket = require('ws')
const wss = new WebSocket.Server({ port: 8080 })
var wsTeam1 = null;
var wsTeam2 = null;


app.use(bodyParser.json());
app.engine('html', require('ejs').renderFile);
app.set('views', path.join(__dirname, '/'));
app.set('view engine', 'html');
app.use(express.static(path.join(__dirname, '')));
app.listen(44302);
console.log('Express server started on port 44302');

var isActive = "false";

wss.on('connection', ws => 
{
    
    ws.on('message',function incoming(data)
    {
        if (data == "CONNECTBALL")
        {
            if (wsTeam1 != null)
            {
                wsTeam1.send("YOU ARE NO LONGER THE BALL HANDLER");
            }
            wsTeam1 = ws;
            wsTeam1.onmessage = onBallControllerMessage;
            console.log("Client Promoted to Ball Handler");
            ws.send("Promoted to Ball Handler")
        }
        else
        {
            wsTeam2 = ws;
            wsTeam2.onmessage = onTeam2message;
            console.log("Client Connected");
        }
    })
})



function onTeam2message(event)
{
    try
    {
        wsTeam1.send(event.data);
        wsTeam2.send("COMMAND SENT");
    }
    catch (ex)
    {
        console.log("Ball Controller is Offline");
        wsTeam2.send("Ball Handler Offline");
    }
}

function onBallControllerMessage(event)
{
    console.log("Promoted Message")
}


/*wss.on('message', ws => 
{
    console.log("Message pah");
    if (ws == outClient)
    {
        ws.send("You are the handler")
    }
    else
    {
        ws.send("You cannot control");
    }
})*/

/*app.post('/ballapi/', urlencodedParser, function (req, res)
{
    var outputJSON = 
    {
        "success" : "1"
    }

    var mang = req.body.data;

    if (outClient.readyState === WebSocket.OPEN)
    {
        outClient.send(mang);
    }
    


    res.setHeader('Content-Type', 'application/json');
    res.status(200).json(outputJSON);
    res.end();
});*/
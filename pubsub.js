const WebSocket = require('ws')
const wss = new WebSocket.Server({ port: 8080 })
const Sensor = require('./sensor.js')

let team2Socket = null
let sensorSockets = []

wss.on('connection', ws => {
  ws.on('message', data => {
    if (data[0] === 's') {
      data = parseInt(data[1])
      sensorSockets.push(new Sensor(ws, data))
      sensorSockets[findSensorPos(data)].socket.send('start')
      sensorSockets[findSensorPos(data)].socket.on('message', event => {
        console.log(data)
        if (team2Socket) {
          team2Socket.send(`{ 'SensorId': ${event} }`)
        }
        setTimeout(() => {
          sensorSockets[findSensorPos(data)].socket.send('start')
        }, 1000)
      })
    } else if (data === 'c') {
      team2Socket = ws
    }
  })
})

function findSensorPos (id) {
  for (let i = 0; i < sensorSockets.length; i++) {
    if (sensorSockets[i].id === id) {
      return i
    }
  }
  return -1
}

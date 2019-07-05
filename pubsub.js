const WebSocket = require('ws')
const wss = new WebSocket.Server({ port: 8080, concurrencyLimit: 1000 })
console.log('Test')

let team2Socket = null

wss.on('connection', ws => {
  ws.on('open', (event) => {
    ws.send('start')
    console.log('Socket opened')
  })
  ws.on('message', data => {
    console.log(data)
    if (data[0] === 'c') {
      team2Socket = ws
      return
    }

    if (data[0] === 's') {
      ws.send('start')
      console.log('Sensor connected')
      return
    }

    if (team2Socket) {
      team2Socket.send(`{ "SensorId": ${data} }`)
    }

    setTimeout(() => {
      ws.send('start')
    }, 1000)
  })

  ws.on('close', data => {
    console.log('Socket closed')
  })

  ws.on('error', (event) => console.log(event))
})

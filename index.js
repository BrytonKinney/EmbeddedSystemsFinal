const http = require('http');
const WebSocket = require('ws');
const path = require('path');
const fs = require('fs');
const url = require('url');

const server = http.createServer(function(request, response) {
	try {
		console.log((new Date()) + ' Received request for ' + request.url);
		var fpath = path.join(__dirname, request.url);
		console.log(`Serving file for request: ${fpath}`);
		var contentType = "";
		response.writeHead(200, {'Content-Type': 'text/html'});
		var readStream = fs.createReadStream(fpath);
		readStream.pipe(response);
		readStream.on('end', () => {
			response.end();
		});
	}
	catch(err) {
		response.writeHead(404);
		response.write('404 - Page not found');
	}
});

const wss = new WebSocket.Server({ noServer: true, clientTracking: true });

wss.on('connection', function connection(ws) {
	console.log('Connection from ws');
	ws.send('acknowledged.');
	ws.on('message', function incoming(data) {
		// Broadcast to everyone else.
		wss.clients.forEach(function each(client) {
			if (client !== ws && client.readyState === WebSocket.OPEN) {
				client.send(data);
			}
		});
	});
});

server.on('upgrade', function upgrade(request, socket, head) {
	console.log('Received upgrade request from ' + request);
	const pathname = url.parse(request.url).pathname;
	wss.handleUpgrade(request, socket, head, function done(ws) {
		wss.emit('connection', ws, request);
    });
});

server.listen(8080);

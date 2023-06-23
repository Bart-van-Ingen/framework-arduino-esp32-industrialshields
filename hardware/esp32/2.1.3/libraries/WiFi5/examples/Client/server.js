#!/usr/bin/env node

const net = require('net');

const server = net.createServer((socket) => {
	console.log(`client connected: ${socket.remoteAddress}`);

	socket.on('data', (data) => {
		console.log(`data received from ${socket.remoteAddress}: ${data}`);

		if (data.toString('utf-8') == 'get-number') {
			const number = (Date.now() * Math.random()) % 1000;
			socket.write(number.toFixed(0));

		} else {
			socket.write('unknown');
		}
	});

	socket.on('end', () => {
		console.log(`client disconnected: ${socket.remoteAddress}`);
	});

	socket.on('error', (err) => {
		console.error(`client ${socket.remoteAddress} error: ${err.message}`);
	});
});

server.on('error', (err) => {
	console.error(`server error: ${err.message}`);
});

server.listen(1234, () => {
	console.log(`server listening`);
});

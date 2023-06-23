#!/usr/bin/env node

const host = process.argv[2] || '192.168.1.104';
const port = Number(process.argv[3] || 1234);

const net = require('net');

const socket = new net.Socket();

socket.on('end', () => {
	console.log('Socket end done');
});

socket.on('error', (err) => {
	console.error(`Socket error: ${err}`);
});

socket.connect(port, host, () => {
	console.log('Socket connected');
	socket.end('abcd');
});

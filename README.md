# infinite-file-stream
Minimalistic infinite file stream. 
A stream that takes a filename and streams it by loop reading.


An easy way to create `readable` `Stream` that loops over file contents infinitely. 

`npm i infinite-file-stream`

* Pass in `file` [string] and optional `encoding` [string] options.
* Use it as a `readable` node `Stream`

Can pipe

``` js
const IFS = require('infinite-file-stream');

const stream = new FileStreamer({ file });
stream.pipe(process.stdout);

```

Or, emit events

``` js
const IFS = require('infinite-file-stream');

const stream = new FileStreamer({ file });
stream.on('data' console.log.bind(console));
```

Useful with test event servers to emit data per line

``` js
const IFS = require('infinite-file-stream');
const through = require('through');
const koa = require('koa');
const app = koa();

const file = './some/sample.txt';

app.use(function *response() {
  this.type = 'text/event-stream; charset=utf-8';
  this.set('Cache-Control', 'no-cache');
  this.set('Connection', 'keep-alive');

  this.body = (new IFS({ file }))
    .on('error', console.error.bind(console))
    .pipe(es.split('\n'))
    .pipe(through(send, end));

  const socket = this.socket;
  function close() {
    socket.removeListener('error', close);
    socket.removeListener('close', close);
  }
  socket.on('error', close);
  socket.on('close', close);
});

app.listen(9000);

```


## License

MIT
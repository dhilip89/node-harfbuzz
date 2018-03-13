# node-harfbuzz

node.js [harfbuzz](https://github.com/behdad/harfbuzz) bindings

# Examples

Works with fonts loaded from buffers

```js
var fs = require('fs');
var hb = require('../index.js');

var font = hb.createFont(fs.readFileSync(process.argv[2]), 64);
var glyphs = hb.shape(font, process.argv[3]);

console.log(glyphs);
```

Also loads fonts from disk as a convenience

```js
var hb = require('../index.js');

var font = hb.createFont(process.argv[2], 64);
var glyphs = hb.shape(font, process.argv[3]);

console.log(glyphs);
```

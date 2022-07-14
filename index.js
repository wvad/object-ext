'use strict';

const native = require('bindings')('addon');

[
  "setLazyAccessor",
  "setAccessor",
  "hasRealProperty",
  "getInfo",
  "sameContextStructuredClone"
].forEach(name => (exports[name] = native[name]));

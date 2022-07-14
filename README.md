# object-ext
## Usage
```js
const {
  getInfo,
  setLazyAccessor,
  setAccessor,
  hasRealProperty,
  sameContextStructuredClone
} = require('object-ext');

const object1 = ... // something

console.log(getInfo(object1));

setLazyAccessor(
  object1,         // target
  'lasyProperty',  // property name
  () => Date.now() // getter
);

console.log('now:', Date.now());

setAccessor(
  object1,                       // target
  'prop',                        // property name
  () => object1._prop,           // getter
  value => object1._prop = value // setter
);

object1.prop = 123;

console.log(object1);

const proxyTarget = { prop1: 'abc' };
const proxy = new Proxy(proxyTarget, {});

console.log(hasRealProperty(proxyTarget, 'prop1'));
console.log(hasRealProperty(proxy, 'prop1'));
```

## Example output (getInfo)
```js
{
  entries: [ {} ],
  internalFields: [],
  className: 'WeakSet',
  id: 961779,
  external: undefined,
  isCallable: false,
  isConstructor: false,
  isApiWrapper: false
}
```
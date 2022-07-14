type ObjectInformation = {
  entries: undefined,
  internalFields: unknown[],
  className: string,
  id: number,
  external: string | undefined,
  isCallable: boolean,
  isConstructor: boolean,
  isApiWrapper: boolean
};

type ObjectInformationWithValueEntries = {
  entries: unknown[] & { isKeyValue: false },
  internalFields: unknown[],
  className: string,
  id: number,
  external: string | undefined,
  isCallable: boolean,
  isConstructor: boolean,
  isApiWrapper: boolean
};

type ObjectInformationWithKeyValueEntries = {
  entries: [unknown, unknown][] & { isKeyValue: true }
  internalFields: unknown[],
  className: string,
  id: number,
  external: string | undefined,
  isCallable: boolean,
  isConstructor: boolean,
  isApiWrapper: boolean
};

export function setLazyAccessor(target: Object, name: string | symbol, getter: Function): void;

export function setAccessor(target: Object, name: string | symbol, getter: Function, setter: Function): void;

export function hasRealProperty(target: Object, name: any): "indexed" | "named" | "namedCallback" | false;

export function getInfo(target: Object): ObjectInformation | ObjectInformationWithValueEntries | ObjectInformationWithKeyValueEntries;

export function sameContextStructuredClone<T>(target: T): T;

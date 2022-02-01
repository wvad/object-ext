type ObjectInformation = {
  entries: undefined,
  className: string,
  id: number,
  isCallable: boolean,
  isConstructor: boolean,
  isApiWrapper: boolean
};

type ObjectInformationWithValueEntries = {
  entries: unknown[] & { isKeyValue: false },
  className: string,
  id: number,
  isCallable: boolean,
  isConstructor: boolean,
  isApiWrapper: boolean
};

type ObjectInformationWithKeyValueEntries = {
  entries: [unknown, unknown][] & { isKeyValue: true }
  className: string,
  id: number,
  isCallable: boolean,
  isConstructor: boolean,
  isApiWrapper: boolean
};

export function setLazyAccessor(target: Object, name: string | symbol, getter: Function): void;

export function setAccessor(target: Object, name: string | symbol, getter: Function, setter: Function): void;

export function hasRealProperty(target: Object, name: any): "indexed" | "named" | "namedCallback" | false;

export function getInfo(target: Object): ObjectInformation;

export function getAddress(externalObject: unknown): string | undefined;

export function sameContextStructuredClone<T>(target: T): T;

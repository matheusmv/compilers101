export class Environment<T> {
  private env: Map<string, T> = new Map();

  constructor(private outerEnv?: Environment<T>) {}

  exists(name: string): boolean {
    return this.env.has(name);
  }

  getValue(name: string): T | null {
    let value: T = this.env.get(name);
    if (!value && this.outerEnv) {
      value = this.outerEnv.getValue(name);
    }
    return value || null;
  }

  setValue(name: string, value: T): T {
    this.env.set(name, value);
    return value;
  }

  assignValue(name: string, value: T): T {
    if (this.env.has(name)) {
      return this.setValue(name, value);
    }

    if (this.outerEnv) {
      return this.outerEnv.assignValue(name, value);
    }

    return null;
  }
}

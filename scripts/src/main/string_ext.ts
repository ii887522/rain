"use strict";

export function endsWithAny(self: string, ...endStrs: string[]): boolean {
    for (const endStr of endStrs) if (self.endsWith(endStr)) return true;
    return false;
}
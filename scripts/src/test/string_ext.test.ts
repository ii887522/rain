"use strict";

import { endsWithAny } from "../main/string_ext.js";

console.assert(!endsWithAny("calculator.pdn", ".png"));
console.assert(endsWithAny("calculator.png", ".png"));
console.assert(!endsWithAny("calculator.png", ".exe", ".dll"));
console.assert(endsWithAny("Calculator.exe", ".exe", ".dll"));
console.assert(endsWithAny("libfreetype-6.dll", ".exe", ".dll"));
console.assert(!endsWithAny("calculator.pdn", ".exe", ".dll", ".png"));
console.assert(endsWithAny("libfreetype-6.dll", ".exe", ".dll", ".png"));
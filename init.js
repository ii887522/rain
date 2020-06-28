"use strict";

import { setSolutionConfigPaths, dependencies, zip, file } from "./scripts/out/main/init_system.js";

setSolutionConfigPaths("Rain/Debug/", "Rain/Release/");

dependencies(() => {
    zip("https://github.com/g-truc/glm/releases/download/0.9.9.8/glm-0.9.9.8.zip");
    zip("https://github.com/glfw/glfw/releases/download/3.3.2/glfw-3.3.2.bin.WIN32.zip");
    zip("https://github.com/glfw/glfw/releases/download/3.3.2/glfw-3.3.2.bin.WIN64.zip");
    file("https://raw.githubusercontent.com/nothings/stb/master/stb_image.h");
});
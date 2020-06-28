"use strict";

import fs from "fs";
import { endsWithAny } from "./string_ext.js";
import pngToIco from "png-to-ico";
import { getFileNameWithoutExt } from "./synthetic_fs.js";
import createDesktopShortcuts from "create-desktop-shortcuts";
import child_process from "child_process";

if (process.argv.length !== 10 && process.argv.length !== 11 || process.argv[2] !== "--release-path" ||
    process.argv[4] !== "--resource-path" || process.argv[6] !== "--ico" || !process.argv[7].endsWith(".png") ||
    process.argv[8] !== "--exe-file" || !process.argv[9].endsWith(".exe") || process.argv.length === 11 &&
    process.argv[10] !== "--shortcut")
    throw new Error(
        "Invalid Command Line!   Usage: node scripts/out/main/bundle.js --release-path <path> --resource-path <path> " +
        "--ico <file-name>.png --exe-file <file-name>.exe [--shortcut]"
    );
const outPath = "out/";
const outResPath = `${outPath}res/main/`;
if (process.argv.length === 10) {
    fs.rmdirSync(outPath, { recursive: true });
    fs.mkdirSync(outResPath, { recursive: true });
    const args = [
        {
            src: process.argv[3],
            exts: [".exe", ".dll"],
            dest: outPath
        },
        {
            src: process.argv[5],
            exts: [".png", ".ttf", ".spv"],
            dest: outResPath
        }
    ];
    for (const arg of args) {
        fs.readdir(arg.src, (err, files) => {
            if (err) throw err;
            for (const file of files) {
                if (!endsWithAny(file, ...arg.exts)) continue;
                fs.copyFile(`${arg.src}${file}`, `${arg.dest}${file}`, err => {
                    if (err) throw err;
                });
            }
        });
    }
    pngToIco(`${process.argv[5]}${process.argv[7]}`).then(value => {
        fs.writeFile(`${outPath}${getFileNameWithoutExt(process.argv[7])}.ico`, value, err => {
            if (err) throw err;
        });
    }, reason => {
        throw new Error(reason);
    }).catch(reason => {
        throw new Error(reason);
    });
    process.on("exit", code => {
        child_process.fork(process.argv[1], [...process.argv.slice(2), "--shortcut"], { detached: true });
    });
} else if (process.argv.length === 11) {
    createDesktopShortcuts({
        onlyCurrentOS: true,
        verbose: true,
        customLogger: function(message, error) {
            console.error(message, error);
        },
        windows: {
            filePath: `${process.cwd()}/${outPath}${process.argv[9]}`,
            outputPath: `${process.cwd()}/${outPath}`,
            name: `${getFileNameWithoutExt(process.argv[9])}`,
            icon: `${process.cwd()}/${outPath}${getFileNameWithoutExt(process.argv[7])}.ico`,
            windowMode: "normal"
        }
    });
}
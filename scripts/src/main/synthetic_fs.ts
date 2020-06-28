"use strict";

export function getFileName(path: string): string {
    return path.substring(path.lastIndexOf("/") + 1);
}

export function getFileNameWithoutExt(filename: string): string {
    return filename.substring(0, filename.lastIndexOf("."));
}
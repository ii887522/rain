declare module "create-desktop-shortcuts" {
    export interface LoggerFunction {
        (message: string, error: object): void;
    }

    export interface WindowsOptions {
        filePath: string;
        outputPath: string;
        name: string;
        icon: string;
        windowMode: string;
    }

    export interface CreateDesktopShortcutsOptions {
        onlyCurrentOS?: boolean;
        verbose?: boolean;
        customLogger?: LoggerFunction;
        windows: WindowsOptions;
    }

    export default function(options: CreateDesktopShortcutsOptions): boolean;
}
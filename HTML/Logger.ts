export class Logger {
    public static LEVEL = {
        DEBUG: 0,
        INFO: 1,
        WARNING: 2,
        ERROR: 3,
        CRITICAL: 4
    };
    public static Output(Data: string, Level: number): void {
        const LevelStrings = [
            "DEBUG",
            "INFO",
            "WARNING",
            "ERROR",
            "CRITICAL"
        ];
        const LevelColors = [
            "color: #fff; background-color: #6c757d;",
            "color: #fff; background-color: #0dcaf0;",
            "color: #fff; background-color: #ffc107;",
            "color: #fff; background-color: #dc3545;",
            "color: #fff; background-color: #dc3545; font-weight: bold;"
        ];
        console.log("%c " + LevelStrings[Level] + " [" + new Date().toLocaleString() + "]" + " %c " + Data + " ",
            "padding: 2px 1px; border-radius: 3px 0 0 3px; font-weight: bold; color: #fff; background: #000000;",
            "padding: 2px 1px; border-radius: 0 3px 3px 0; font-weight: bold; " + LevelColors[Level]);
    };
};
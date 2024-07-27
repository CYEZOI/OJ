import { Logger } from "./Logger";
import { Utilities } from "./Utilities";

export class API {
    static async Request<T>(Action: string, Data: any,
        CallBack: () => void,
        SuccessCallback: (Data: T) => void,
        FailCallback: () => void,
        ErrorCallback: () => void) {
        Logger.Output("API sent: " + Action + " " + JSON.stringify(Data), Logger.LEVEL.INFO);
        let Abort = new AbortController();
        let Timeout = setTimeout(() => {
            // Abort.abort();
            // CallBack();
            // ErrorCallback();
            // Utilities.ShowError("请求超时");
        }, 5000);
        await fetch("/" + Action, {
            method: "POST",
            headers: {
                "Content-Type": "application/json"
            },
            signal: Abort.signal,
            body: JSON.stringify(Data)
        }).then(async (Response) => {
            clearTimeout(Timeout);
            CallBack();
            if (Response.status == 200) {
                await Response.json().then(Response => {
                    if (Response.Success === true) {
                        Logger.Output("API received: " + JSON.stringify(Response.Data), Logger.LEVEL.INFO);
                        SuccessCallback(Response.Data);
                    } else {
                        Logger.Output("API failed: " + Response.Message, Logger.LEVEL.ERROR);
                        Utilities.ShowError(Response.Message);
                        FailCallback();
                    }
                });
            } else {
                Logger.Output("API failed: " + Response.status + " " + Response.statusText, Logger.LEVEL.ERROR);
                Utilities.ShowError("API请求失败: " + Response.status + " " + Response.statusText);
                ErrorCallback();
            }
        }).catch((Error) => {
            CallBack();
            Utilities.ShowError("API请求失败: " + Error);
            ErrorCallback();
        });
    }
}
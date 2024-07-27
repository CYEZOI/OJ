import React from "react";
import { Logger } from "./Logger";

export class MessagePipe {
    Pipe: { [Key: string]: ((Data: any) => void) } = {};
    Register(Key: string, Callback: (Data: any) => void) {
        Logger.Output("MessagePipe: Registering key: " + Key, Logger.LEVEL.DEBUG);
        this.Pipe[Key] = Callback;
    }
    Send(Key: string, Data: any) {
        Logger.Output("MessagePipe: Sending key: " + Key + " Data: " +
            JSON.stringify(Data, (Key, Value) => {
                if (React.isValidElement(Value)) {
                    return "[ReactElement]";
                }
                return Value;
            }),
            Logger.LEVEL.DEBUG);
        if (this.Pipe[Key]) {
            this.Pipe[Key](Data);
        }
        else {
            Logger.Output("MessagePipe: No such key: " + Key, Logger.LEVEL.ERROR);
        }
    }
}

export const MessagePipeInstance = new MessagePipe();
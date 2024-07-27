import { MessagePipeInstance } from "./MessagePipe";

export class Utilities {
    static ShowError(Message: string) {
        MessagePipeInstance.Send("PopupToast", {
            Success: false,
            Content: Message,
        });
    }
    static ShowSuccess(Message: string) {
        MessagePipeInstance.Send("PopupToast", {
            Success: true,
            Content: Message,
        });
    }
}
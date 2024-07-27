import React from "react";
import { Load } from "./Interface/Load";
import { Popup } from "./Interface/Popup";
import { ContentRoot, PopupRoot } from "./Interface";
import { Logger } from "./Logger";

console.error = (message?: any, ...optionalParams: any[]) => {
    Logger.Output(message as string + " " + optionalParams.join(" "), Logger.LEVEL.CRITICAL);
}
ContentRoot.render(<Load />);
PopupRoot.render(<Popup />);

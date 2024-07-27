import React from "react";
import { PopupNotifications } from "./PopupNotifications";
import { PopupAction } from "./PopupAction";
import { PopupToast } from "./PopupToast";
import { Logger } from "../Logger";

export class Popup extends React.Component {
    render(): React.ReactNode {
        Logger.Output("Rendered: Popup", Logger.LEVEL.DEBUG);
        return <div>
            <PopupNotifications />
            <PopupAction />
            <PopupToast />
        </div>
    }
}
import React from "react";
import { Logger } from "../Logger";

export class Footer extends React.Component {
    render() {
        Logger.Output("Rendered: Footer", Logger.LEVEL.DEBUG);
        return <div className="m-3 d-flex justify-content-center text-secondary">
            Made by @langningchen
        </div>
    }
}
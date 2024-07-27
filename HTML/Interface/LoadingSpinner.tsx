import React from "react";
import { Logger } from "../Logger";

export class LoadingSpinner extends React.Component {
    render() {
        Logger.Output("Rendered: LoadingSpinner", Logger.LEVEL.DEBUG);
        return <div className="w-100 text-center">
            <div className="spinner-border"></div>
        </div>;
    };
}
import React from "react";
import { BodyList } from "./BodyList";
import { BodyContent } from "./BodyContent";
import { Logger } from "../Logger";

export class Body extends React.Component {
    render(): React.ReactNode {
        Logger.Output("Rendered: Body", Logger.LEVEL.DEBUG);
        return <div className="d-flex overflow-hidden" style={{ flex: 1 }}>
            <div className="border rounded bg-text-dark overflow-y-auto m-1" style={{ width: "15em" }}>
                <BodyList />
            </div>
            <div className="border rounded bg-text-dark overflow-y-auto m-1 p-3" style={{ flex: 1 }}>
                <BodyContent />
            </div>
        </div>
    }
}
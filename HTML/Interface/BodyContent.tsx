import React from "react";
import { Logger } from "../Logger";
import { Welcome } from "./Contents/Welcome";
import { MessagePipeInstance } from "../MessagePipe";

export class BodyContent extends React.Component {
    state = {
        BodyContent: <Welcome />,
    };
    componentDidMount(): void {
        MessagePipeInstance.Register("SetBodyContent", (Data: React.ReactNode) => {
            this.setState({ BodyContent: Data });
        });
    }
    render(): React.ReactNode {
        Logger.Output("Rendered: BodyContent", Logger.LEVEL.DEBUG);
        return this.state.BodyContent;
    }
}
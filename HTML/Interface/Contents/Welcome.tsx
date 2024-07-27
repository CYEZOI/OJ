import React from "react";
import { Logger } from "../../Logger";
import { MessagePipeInstance } from "../../MessagePipe";

export class Welcome extends React.Component {
    componentDidMount(): void {
        MessagePipeInstance.Send("SetHeaderBreadcrumb", [
            {
                "Title": "欢迎",
                "Component": <Welcome />,
            },
        ]);
        MessagePipeInstance.Send("SetBodyList", []);
    }
    render(): React.ReactNode {
        Logger.Output("Rendered: Welcome", Logger.LEVEL.DEBUG);
        return <div>欢迎</div>
    }
}
import React from "react";
import { Logger } from "../../Logger";
import { MessagePipeInstance } from "../../MessagePipe";
import { Welcome } from "./Welcome";
import { Contest } from "./Contest";

export class Contests extends React.Component {
    componentDidMount(): void {
        MessagePipeInstance.Send("SetHeaderBreadcrumb", [
            {
                "Title": "欢迎",
                "Component": <Welcome />,
            },
            {
                "Title": "比赛",
                "Component": <Contests />,
            },
        ]);
        MessagePipeInstance.Send("SetBodyList", [
            {
                "Title": "1166",
                "Component": <Contest ContestID={1166} />,
            },
        ]);
    }
    render(): React.ReactNode {
        Logger.Output("Rendered: Contests", Logger.LEVEL.DEBUG);
        return <div>比赛</div>
    }
}
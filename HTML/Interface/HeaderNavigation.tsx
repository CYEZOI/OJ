import React from "react";
import { Logger } from "../Logger";
import { Welcome } from "./Contents/Welcome";
import { MessagePipeInstance } from "../MessagePipe";
import { Contests } from "./Contents/Contests";

export class HeaderNavigation extends React.Component {
    render(): React.ReactNode {
        const HeaderNavigationData = [
            {
                Title: "欢迎",
                Component: <Welcome />,
            },
            {
                Title: "比赛",
                Component: <Contests />,
            },
        ];
        Logger.Output("Rendered: HeaderNavigation", Logger.LEVEL.DEBUG);
        return <div className="border rounded p-2 m-1" style={{ flex: 1 }}>
            <div className="d-flex justify-content-center">
                {
                    HeaderNavigationData.map((Data) => {
                        return <a className="nav-link mx-2" href="#" onClick={() => {
                            MessagePipeInstance.Send("SetBodyContent", Data.Component);
                        }}>{Data.Title}</a>
                    })
                }
            </div>
        </div>;
    }
}
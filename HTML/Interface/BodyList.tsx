import React from "react";
import { Logger } from "../Logger";
import { MessagePipeInstance } from "../MessagePipe";
import { LoadingSpinner } from "./LoadingSpinner";

type StateType = {
    BodyListData: BodyListDataStructure[];
};

export interface BodyListDataStructure {
    Title: string,
    Component: React.ReactNode,
}

export class BodyList extends React.Component<{}, StateType> {
    state = {
        BodyListData: [],
    }
    componentDidMount(): void {
        MessagePipeInstance.Register("SetBodyList", (Data: BodyListDataStructure[]) => {
            this.setState({ BodyListData: Data });
        });
    }
    render(): React.ReactNode {
        Logger.Output("Rendered: BodyList", Logger.LEVEL.DEBUG);
        if (this.state.BodyListData.length === 0) {
            return <div className="mt-3">
                <LoadingSpinner />
            </div>;
        }
        return <div className="list-group list-group-flush">
            {
                (() => {
                    const RealData: BodyListDataStructure[] = this.state.BodyListData;
                    return RealData.map((Data) => {
                        return <button className="list-group-item list-group-item-action" onClick={() => {
                            MessagePipeInstance.Send("SetBodyContent", Data.Component);
                        }}>{Data.Title}</button>
                    });
                })()
            }
        </div>;
    }
}
import React from "react";
import { Breadcrumb } from "react-bootstrap";
import { Logger } from "../Logger";
import { MessagePipeInstance } from "../MessagePipe";

type StateType = {
    HeaderBreadcrumbData: HeaderBreadcrumbDataStructure[],
};

export interface HeaderBreadcrumbDataStructure {
    Title: string,
    Component: React.ReactNode,
}

export class HeaderBreadcrumb extends React.Component<{}, StateType> {
    state = {
        HeaderBreadcrumbData: [],
    };
    componentDidMount(): void {
        MessagePipeInstance.Register("SetHeaderBreadcrumb", (Data: HeaderBreadcrumbDataStructure[]) => {
            this.setState({ HeaderBreadcrumbData: Data });
        });
    }
    render(): React.ReactNode {
        Logger.Output("Rendered: HeaderBreadcrumb", Logger.LEVEL.DEBUG);
        return <Breadcrumb className="ms-1 me-1">
            {
                (() => {
                    const RealData: HeaderBreadcrumbDataStructure[] = this.state.HeaderBreadcrumbData;
                    return RealData.map((Data) => {
                        return <Breadcrumb.Item onClick={() => {
                            MessagePipeInstance.Send("SetBodyContent", Data.Component);
                        }}>{Data.Title}</Breadcrumb.Item>
                    })
                })()
            }
        </Breadcrumb>
    }
}
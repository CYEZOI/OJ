import React from "react";
import { Logger } from "../../Logger";
import { MessagePipeInstance } from "../../MessagePipe";
import { Welcome } from "./Welcome";
import { Contests } from "./Contests";
import { LoadingSpinner } from "../LoadingSpinner";
import { API } from "../../API";

type PropsType = {
    ContestID: number;
}

export interface ProblemDataStructure {
    Name: string;
    InputFilename: string;
    OutputFilename: string;
    TimeLimit: number;
    MemoryLimit: number;
    SpecialJudge: boolean;
    EnableO2: boolean;
    SubmitCount: number;
    SolvedCount: number;
    Description: string;
    InputFormat: string;
    OutputFormat: string;
    Samples: Array<{
        Input: string;
        Output: string;
        Description: string;
    }>;
    DataRange: string;
    Hint: string;
}

export class Contest extends React.Component<PropsType> {
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
            {
                "Title": this.props.ContestID,
                "Component": <Contest ContestID={this.props.ContestID} />,
            },
        ]);
        MessagePipeInstance.Send("SetBodyList", []);
        API.Request("GetContest", { ContestID: this.props.ContestID }, () => { }, (Data: ProblemDataStructure) => {

        }, () => {
            setTimeout(() => {
                MessagePipeInstance.Send("SetBodyContent", <Contests />);
            }, 1000);
        }, () => { });
    }
    render(): React.ReactNode {
        Logger.Output("Rendered: Contest, ContestID: " + this.props.ContestID, Logger.LEVEL.DEBUG);
        return <LoadingSpinner />;
    }
}
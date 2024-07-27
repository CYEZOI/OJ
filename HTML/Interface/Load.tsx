import React from "react";
import { Main } from "./Main";
import { API } from "../API";
import { Initiate } from "./Initiate";
import { Logger } from "../Logger";
import { ContentRoot } from ".";

export class Load extends React.Component {
    state = {
        "检查设置": false,
        "登录": false,
        "刷新缓存": false,
    }
    private ShowDetails: boolean = false;
    render() {
        Logger.Output("Rendered: Load", Logger.LEVEL.DEBUG);
        if (Object.values(this.state).every((Value) => Value)) {
            setTimeout(() => {
                ContentRoot.render(<Main />);
            }, 1000);
        }
        return <div className="d-flex justify-content-center align-items-center flex-column" style={{ height: "100vh" }}>
            <div>
                <h1 className="display-1 mb-3">CYEZOJ-Client</h1>
                <div className="d-flex justify-content-center mb-5">
                    <div className="progress" style={{ width: "20em" }}>
                        <div className="progress-bar progress-bar-striped progress-bar-animated" style={{ width: "100%" }}></div>
                    </div>
                </div>
                <div className="d-flex flex-row-reverse">
                    <div>
                        <button className="btn" onClick={() => { this.ShowDetails = !this.ShowDetails; this.forceUpdate(); }}>
                            <span className="me-2">详细信息</span>
                            {this.ShowDetails ? <i className="bi bi-chevron-up"></i> : <i className="bi bi-chevron-down"></i>}
                        </button>
                    </div>
                </div>
                <ul style={{ height: "20em" }} className="list-group">
                    {this.ShowDetails && Object.keys(this.state).map((Data) => {
                        return <li className="list-group-item d-flex justify-content-between align-items-center" key={Data}>
                            {Data}
                            {this.state[Data as keyof typeof this.state] ? <i className="bi bi-check-circle-fill text-success"></i> : <i className="spinner-border spinner-border-sm text-primary"></i>}
                        </li>;
                    })}
                </ul>
            </div>
        </div>;
    };
    async componentDidMount() {
        var Stop: boolean = false;
        await (async () => {
            await API.Request("GetSettings", {}, () => { }, (Data: any) => {
                const Settings = Data["Settings"];
                var SettingsCorrect: boolean = true;
                if (!Settings.hasOwnProperty("CYEZOJUsername") || !Settings.hasOwnProperty("CYEZOJPassword") || !Settings.hasOwnProperty("CYEZOJBaseURL")) {
                    SettingsCorrect = false;
                }
                if (SettingsCorrect) {
                    this.setState({ "检查设置": true });
                }
                else {
                    ContentRoot.render(<Initiate />);
                    Stop = true;
                }
            }, () => {
                ContentRoot.render(<Initiate />);
                Stop = true;
            }, () => { });
        })();
        if (Stop) {
            return;
        }
        await (async () => {
            await API.Request("Login", {}, () => { }, () => {
                this.setState({ "登录": true });
            }, () => {
                ContentRoot.render(<Initiate />);
            }, () => { });
        })();
        (async () => {
            this.setState({ "刷新缓存": true });
        })();
    };
}
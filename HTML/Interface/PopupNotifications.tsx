import React from "react";
import { Toast } from "react-bootstrap";
import { MessagePipeInstance } from "../MessagePipe";
import { Logger } from "../Logger";

type StateType = {
    PopupNotificationsData: PopupNotificationsDataStructure[],
};

export interface PopupNotificationsDataStructure {
    Success: boolean,
    Title: string,
    Content: string,
}

export class PopupNotifications extends React.Component<{}, StateType> {
    state = {
        PopupNotificationsData: [],
    };
    componentDidMount(): void {
        MessagePipeInstance.Register("PopupNotifications", (Data: PopupNotificationsDataStructure) => {
            const RealData: PopupNotificationsDataStructure[] = this.state.PopupNotificationsData;
            RealData.push(Data);
            this.setState({ "PopupNotificationsData": RealData });
        });
    }
    render(): React.ReactNode {
        Logger.Output("Rendered: PopupNotifications", Logger.LEVEL.DEBUG);
        return <div className="position-fixed top-0 end-0 p-3 z-3 pe-none">
            {
                (() => {
                    const RealData: PopupNotificationsDataStructure[] = this.state.PopupNotificationsData;
                    if (RealData.length == 0) {
                        return null;
                    }
                    return RealData.map((Data, Index) => {
                        return <Toast show={true} onClose={() => {
                            RealData.splice(Index, 1);
                            this.setState({ "PopupNotificationsData": RealData });
                        }} delay={5000} autohide className="mb-2 pe-auto">
                            <Toast.Header>
                                {Data.Success ? <i className="bi bi-check-circle-fill text-success"></i> : <i className="bi bi-x-circle-fill text-danger"></i>}
                                <strong className="ms-2 me-auto">{Data.Title}</strong>
                            </Toast.Header>
                            <Toast.Body>{Data.Content}</Toast.Body>
                        </Toast>
                    })
                })()
            }
        </div>;
    }
}
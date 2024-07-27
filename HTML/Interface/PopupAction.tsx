import React from "react";
import { Offcanvas } from "react-bootstrap";
import { Logger } from "../Logger";


type StateType = {
    PopupActionData: PopupActionDataStructure | null,
};

export interface PopupActionDataStructure {
    Title: string,
    Steps: {
        Element: React.ReactNode,
        Next: () => void,
    }[],
    Progress: number,
    Data: any,
}

export class PopupAction extends React.Component<{}, StateType> {
    state = {
        PopupActionData: null,
    }
    render(): React.ReactNode {
        Logger.Output("Rendered: PopupAction", Logger.LEVEL.DEBUG);
        const PopupActionData = this.state.PopupActionData;
        if (!PopupActionData) {
            return null;
        }
        const RealData: PopupActionDataStructure = PopupActionData;
        return <Offcanvas placement="end" backdrop="static" show="true" id="OffcanvasElement" onHide={() => { this.setState({ "PopupActionData": null }); }}>
            <Offcanvas.Header closeButton>
                <Offcanvas.Title>{RealData.Title}</Offcanvas.Title>
                {
                    RealData.Steps.length > 1 &&
                    <div className="position-relative m-4" style={{ flex: 1 }}>
                        <div className="progress" role="progressbar" style={{ height: "1px" }}>
                            <div className="progress-bar" style={{ width: (RealData.Progress / (RealData.Steps.length - 1) * 100) + "%" }}></div>
                        </div>
                        {
                            RealData.Steps.map((_Data, Index) => {
                                return <button type="button"
                                    className={"position-absolute " + " translate-middle btn btn-sm " + (Index <= RealData.Progress ? "btn-primary" : "btn-secondary") + " rounded-pill p-0"}
                                    style={{ width: "0.5em", height: "0.5em", left: (Index / (RealData.Steps.length - 1) * 100) + "%", top: "0em" }} />
                            })
                        }
                    </div>
                }
            </Offcanvas.Header>
            <Offcanvas.Body>
                {RealData.Steps[RealData.Progress].Element}
                <div className="d-flex justify-content-end">
                    {RealData.Progress > 0 && <button className="btn btn-outline-secondary me-2" onClick={() => { this.setState({ "PopupActionData": { ...RealData, Progress: RealData.Progress - 1 } }); }}>上一步</button>}
                    {
                        RealData.Progress < RealData.Steps.length - 1 ?
                            <button className="btn btn-outline-primary" onClick={() => {
                                this.setState({ "PopupActionData": { ...RealData, Progress: RealData.Progress + 1 } });
                                RealData.Steps[RealData.Progress].Next();
                            }}>下一步</button> :
                            <button className="btn btn-outline-primary" onClick={() => {
                                this.setState({ "PopupActionData": null });
                            }}>完成</button>
                    }
                </div>
            </Offcanvas.Body>
        </Offcanvas>
    }
}
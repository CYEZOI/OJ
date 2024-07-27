import React from "react";
import { HeaderBreadcrumb } from "./HeaderBreadcrumb";
import { HeaderNavigation } from "./HeaderNavigation";
import { Logger } from "../Logger";

export class Header extends React.Component {
    render(): React.ReactNode {
        Logger.Output("Rendered: Header", Logger.LEVEL.DEBUG);
        return <div>
            <div className="d-flex text-center">
                <div className="border rounded p-2 m-1">
                    CYEZOJ-Client
                </div>
                <HeaderNavigation />
                <div className="border rounded p-2 m-1">
                    <i className="bi bi-bell m-2"></i>
                    <i className="bi bi-person m-2"></i>
                </div>
            </div>
            <HeaderBreadcrumb />
        </div>
    }
}
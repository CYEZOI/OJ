import React from "react";
import { Header } from "./Header";
import { Body } from "./Body";
import { Footer } from "./Footer";
import { Logger } from "../Logger";

export class Main extends React.Component {
    render(): React.ReactNode {
        Logger.Output("Rendered: Main", Logger.LEVEL.DEBUG);
        return <div className="placeholder-glow p-1 h-100 d-flex flex-column">
            <Header />
            <Body />
            <Footer />
        </div >;
    }
}
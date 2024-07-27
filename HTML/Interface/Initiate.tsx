import React from "react";
import { Button, Container, FloatingLabel, Form } from "react-bootstrap";
import { ContentRoot } from ".";
import { Load } from "./Load";
import { API } from "../API";
import * as Formik from "formik";
import * as yup from "yup";
import { Logger } from "../Logger";

type StateType = {
    Step: number;
};
type SettingsType = {
    CYEZOJUsername: string;
    CYEZOJPassword: string;
    CYEZOJBaseURL: string;
};

export class Initiate extends React.Component<{}, StateType> {
    state = {
        Step: 0,
    };
    Settings: SettingsType | null = null;
    Steps: React.ReactNode[] = [
        <div>
            <h1 className="mb-3">正在加载配置</h1>
        </div>,
        <div>
            <h1 className="mb-3">欢迎使用 CYEZOJ-Client</h1>
            <p>我们检测到您的配置不存在或有误，请您跟随我们的引导进行配置。</p>
            <Button variant="primary" onClick={() => { this.setState({ Step: this.state.Step + 1 }); }}>开始</Button>
        </div>,
    ];

    async componentDidMount() {
        await API.Request("GetSettings", {}, () => { }, (Data: { Settings: SettingsType }) => {
            this.Settings = Data.Settings;
            this.setState({ Step: 1 });
            this.Steps.push(
                <div>
                    <h1 className="mb-3">CYEZOJ账户</h1>
                    <p>请填写您的CYEZOJ账户信息，以便我们验证您的身份并获取相关数据。</p>
                    <Formik.Formik
                        enableReinitialize
                        onSubmit={() => {
                            const UsernameInput = document.getElementById("CYEZOJUsername") as HTMLInputElement;
                            const PasswordInput = document.getElementById("CYEZOJPassword") as HTMLInputElement;
                            if (UsernameInput.value && PasswordInput.value) {
                                this.Settings!.CYEZOJUsername = UsernameInput.value;
                                this.Settings!.CYEZOJPassword = PasswordInput.value;
                                this.setState({ Step: this.state.Step + 1 });
                            }
                        }}
                        initialValues={{ CYEZOJUsername: this.Settings?.CYEZOJUsername, CYEZOJPassword: this.Settings?.CYEZOJPassword }}
                        validationSchema={yup.object().shape({
                            CYEZOJUsername: yup.string().required("请填写用户名"),
                            CYEZOJPassword: yup.string().required("请填写密码"),
                        })}
                        children={(FormikProps: any) => (
                            <Formik.Form noValidate onSubmit={FormikProps.handleSubmit}>
                                <FloatingLabel controlId="CYEZOJUsername" label="用户名" className="mb-3">
                                    <Form.Control type="text" placeholder="用户名" isInvalid={!!FormikProps.errors.CYEZOJUsername} {...FormikProps.getFieldProps("CYEZOJUsername")} />
                                    <Form.Control.Feedback type="invalid">{FormikProps.errors.CYEZOJUsername}</Form.Control.Feedback>
                                </FloatingLabel>
                                <FloatingLabel controlId="CYEZOJPassword" label="密码" className="mb-3">
                                    <Form.Control type="password" placeholder="密码" isInvalid={!!FormikProps.errors.CYEZOJPassword} {...FormikProps.getFieldProps("CYEZOJPassword")} />
                                    <Form.Control.Feedback type="invalid">{FormikProps.errors.CYEZOJPassword}</Form.Control.Feedback>
                                </FloatingLabel>
                                <Button variant="primary" type="submit">下一步</Button>
                            </Formik.Form>
                        )}
                    />
                </div>,
                <div>
                    <h1 className="mb-3">CYEZOJ服务器</h1>
                    <p>请设置CYEZOJ服务器地址，如果您不确定，请不要更改此项，保留为默认值</p>
                    <Formik.Formik
                        enableReinitialize
                        onSubmit={() => {
                            const BaseURLInput = document.getElementById("CYEZOJBaseURL") as HTMLInputElement;
                            if (BaseURLInput.value) {
                                this.Settings!.CYEZOJBaseURL = BaseURLInput.value;
                                this.setState({ Step: this.state.Step + 1 });
                            }
                        }}
                        initialValues={{ CYEZOJBaseURL: this.Settings?.CYEZOJBaseURL ?? "https://oj.cyez.cc:18160" }}
                        validationSchema={yup.object().shape({
                            CYEZOJBaseURL: yup.string().url("请输入正确的地址")
                        })}
                        children={(FormikProps: any) => (
                            <Formik.Form noValidate onSubmit={FormikProps.handleSubmit}>
                                <FloatingLabel controlId="CYEZOJBaseURL" label="服务器地址" className="mb-3">
                                    <Form.Control type="text" placeholder="服务器地址" isInvalid={!!FormikProps.errors.CYEZOJBaseURL} {...FormikProps.getFieldProps("CYEZOJBaseURL")} />
                                    <Form.Control.Feedback type="invalid">{FormikProps.errors.CYEZOJBaseURL}</Form.Control.Feedback>
                                </FloatingLabel>
                                <Button variant="primary" type="submit">下一步</Button>
                            </Formik.Form>
                        )}
                    />
                </div>,
                <div>
                    <h1 className="mb-3">大功告成</h1>
                    <p>您已完成配置，现在您可以点击下方按钮开始使用CYEZOJ-Client。</p>
                    <Button variant="primary" onClick={async () => {
                        await API.Request("SetSettings", {
                            Settings: this.Settings
                        }, () => { }, () => { }, () => { }, () => { });
                        ContentRoot.render(<Load />);
                    }}>完成</Button>
                </div>,
            );
        }, () => { }, () => { });
    }

    render() {
        Logger.Output("Rendered: Initial", Logger.LEVEL.DEBUG);
        return (
            <Container className="d-flex justify-content-center flex-column" style={{ height: "100vh" }}>
                {this.Steps[this.state.Step]}
            </Container>
        );
    }
}

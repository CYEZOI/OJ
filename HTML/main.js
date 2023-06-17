var Texts = ["Request failed"];
var Regexes = {
    "Username": /^[0-9a-zA-Z]{4,16}$/,
    "Password": /^([^a-z]+|[^A-Z]+|[^0-9]+|[a-zA-Z0-9]+)$/,
    "Nickname": /^.{4,16}$/,
    "EmailAddress": /^([a-zA-Z0-9_-])+@([a-zA-Z0-9_-])+(.[a-zA-Z0-9_-])+$/,
    "VerifyCode": /^[0-9]{6}$/
};
const GetToken = () => {
    var Token = localStorage.getItem("Token");
    if (Token != null) {
        RequestAPI("CheckTokenAvailable", { "Token": localStorage.getItem("Token") }, () => { }, () => { }, () => {
            localStorage.removeItem("Token");
            Token = null;
        }, () => { Token = null; }, false);
    }
    return Token;
};
const ShowError = (AlertText) => {
    $("#ErrorAlert").text(AlertText);
    $("#ErrorAlert").fadeIn(100);
    setTimeout(() => { $("#ErrorAlert").fadeOut(100); }, 2000);
};
const ShowSuccess = (SuccessText) => {
    $("#SuccessAlert").fadeIn(100);
    $("#SuccessAlert>.weui-toast>.weui-toast__content").text(SuccessText);
    setTimeout(() => { $("#SuccessAlert").fadeOut(100); }, 2000);
};
const ShowLoading = (LoadingText) => {
    $("#LoadingAlert").fadeIn(100);
    $("#LoadingAlert>.weui-toast>.weui-toast__content").text(LoadingText);
};
const HideLoading = () => { $("#LoadingAlert").fadeOut(100); };
const ShowHalfScreenDialog = (Title, Content) => {
    $("#HalfScreenDialog>.weui-half-screen-dialog>.weui-half-screen-dialog__hd>.weui-half-screen-dialog__hd__main>.weui-half-screen-dialog__title").text(Title);
    $("#HalfScreenDialog>.weui-half-screen-dialog>.weui-half-screen-dialog__bd").html(Content);
    $("#HalfScreenDialog").fadeIn(100);
};
$("#HalfScreenDialogCloseButton").click(() => { $("#HalfScreenDialog").fadeOut(100); });
const EnableButton = (Element) => {
    Element.removeClass("weui-btn_disabled");
    Element.removeAttr("disabled");
};
const DisableButton = (Element) => {
    Element.addClass("weui-btn_disabled");
    Element.attr("disabled");
};
const RequestAPI = (Action, Data, CallBack, SuccessCallback, FailCallback, ErrorCallback, AddToken = true) => {
    if (AddToken) {
        Data["Token"] = GetToken();
        if (Data["Token"] == null) {
            CallBack();
            ErrorCallback();
            ShowError("Login expired");
            SwitchPage("Login");
            return;
        }
    }
    var URI = "/api";
    var XHR = new XMLHttpRequest();
    XHR.open("POST", URI);
    XHR.onreadystatechange = () => {
        if (XHR.readyState == 4) {
            CallBack();
            if (XHR.status == 200) {
                var Response = JSON.parse(XHR.responseText);
                if (Response["Success"]) { SuccessCallback(Response["Data"]); } else {
                    ShowError(Response["Message"]);
                    FailCallback();
                }
            } else {
                ShowError(Texts[0]);
                ErrorCallback();
            }
        }
    };
    XHR.send(JSON.stringify({
        "Action": Action,
        "Data": Data
    }));
};
const SwitchPage = (Name, Data) => {
    var PushedURL = new URL(location.href);
    PushedURL.pathname = "/" + Name;
    PushedURL.search = "";
    if (Data) {
        for (const [Key, Value] of Object.entries(Data)) { PushedURL.searchParams.append(Key, Value); }
    }
    history.pushState(Data, Name, PushedURL.toString());
    LoadPage(Name, Data);
};
const MemoryToString = (Memory) => {
    if (Memory < 1024) { return Memory + "B"; } else if (Memory < 1024 * 1024) { return Number(Memory / 1024).toFixed(0) + "KB"; } else { return Number(Memory / 1024 / 1024).toFixed(0) + "MB"; }
};
const TimeToString = (Time) => {
    if (Time < 1000) { return Time + "ms"; } else { return Number(Time / 1000).toFixed(0) + "s"; }
};
const LoadPage = (Name, Data) => {
    $("#Content>div").attr("style", "display: none;");
    $("#" + Name).attr("style", "");
    var LoadPageCallbacks = {
        "Error": () => {
            if (!Data["Message"] || !Data["URL"]) {
                SwitchPage("Error", {
                    "Message": "No error message or url provide",
                    "URL": location.href
                });
            }
            var ErrorTexts = ["Error", "Go home"];
            document.title = ErrorTexts[0];
            $("#ErrorTitle").text(ErrorTexts[0]);
            $("#ErrorDescription").text(Data["Message"]);
            $("#ErrorTips").text(Data["URL"]);
            $("#ErrorButton").text(ErrorTexts[1]);
            $("#ErrorButton").off("click").on("click", (Event) => {
                if (Event.currentTarget.classList.contains("weui-btn_disabled")) { return; }
                location.href = new URL(location.href).origin;
            });
        },
        "Login": () => {
            var LoginTexts = ["Login",
                "Username", "Please fill in the username",
                "Password", "Please fill in the password",
                "No account? Go to register", "Login",
                "The username does not conform to the specification", "The password does not conform to the specification", "Logging in", "Logged in"
            ];
            document.title = LoginTexts[0];
            $("#LoginTitle").text(LoginTexts[0]);
            $("#LoginUsernameLabel>.weui-cell__hd>span").text(LoginTexts[1]);
            $("#LoginUsernameInput").attr("placeholder", LoginTexts[2]);
            $("#LoginPasswordLabel>.weui-cell__hd>span").text(LoginTexts[3]);
            $("#LoginPasswordInput").attr("placeholder", LoginTexts[4]);
            $("#LoginGoToRegister").text(LoginTexts[5]);
            $("#LoginSubmit").text(LoginTexts[6]);
            $("#LoginForm").off("keydown").on("keydown", (Event) => {
                if (Event.keyCode == 13) { $("#LoginSubmit").click(); }
            });
            $("#LoginUsernameInput").off("input").on("input", () => { $("#LoginUsernameLabel").removeClass("weui-cell_warn"); });
            $("#LoginPasswordInput").off("input").on("input", () => { $("#LoginPasswordLabel").removeClass("weui-cell_warn"); });
            $(".LoginInputs").off("input").on("input", () => {
                if ($("#LoginUsernameInput").val() && $("#LoginPasswordInput").val()) { EnableButton($("#LoginSubmit")); } else { DisableButton($("#LoginSubmit")); }
            });
            $("#LoginGoToRegister").off("click").on("click", (Event) => {
                if (Event.currentTarget.classList.contains("weui-btn_disabled")) { return; }
                SwitchPage("Register", null);
            });
            $("#LoginSubmit").off("click").on("click", (Event) => {
                if (Event.currentTarget.classList.contains("weui-btn_disabled")) { return; }
                var Username = $("#LoginUsernameInput").val();
                var Password = $("#LoginPasswordInput").val();
                if (Regexes["Username"].test(Username) == false) {
                    ShowError(LoginTexts[7]);
                    $("#LoginUsernameLabel").addClass("weui-cell_warn");
                } else if (Password.length < 8 || Password.length > 32 || Regexes["Password"].test(Password) == true) {
                    ShowError(LoginTexts[8]);
                    $("#LoginPasswordLabel").addClass("weui-cell_warn");
                } else {
                    ShowLoading(LoginTexts[9]);
                    RequestAPI("Login", {
                        "Username": Username,
                        "Password": Password
                    }, () => { HideLoading(); }, (Response) => {
                        localStorage.setItem("Token", Response["Token"]);
                        ShowSuccess(LoginTexts[10]);
                        setTimeout(() => { SwitchPage("Home", null); }, 1000);
                    }, () => { $("#PasswordLabel").addClass("weui-cell_warn"); }, () => { },
                        false);
                }
            });
            localStorage.getItem("Token") && RequestAPI("CheckTokenAvailable", { "Token": localStorage.getItem("Token") }, () => { }, (Response) => {
                ShowSuccess(LoginTexts[10]);
                setTimeout(() => { SwitchPage("Home", null); }, 1000);
            }, () => { localStorage.removeItem("Token"); }, () => { }, false);
        },
        "Register": () => {
            var RegisterTexts = ["Register",
                "Username", "Please fill in your username",
                "Password", "Please fill in your password",
                "Repeat password", "Please repeat the password above",
                "Email address", "Please fill in your email address",
                "Email verification code", "Please fill in the verification code you received in the email", "Send verification code",
                "Already have an account? Log in directly", "Registration specification", "Register",
                "The username does not conform to the specification", "The password does not conform to the specification", "The password does not match", "The email address does not conform to the specification", "The email verification code does not conform to the specification", "Sending", "Verification code sent", "Registering", "Registered",
                "Username: 4~16 bits, can only contain uppercase and lowercase letters and numbers<br>Password: 8~32 bits, must contain uppercase and lowercase letters, numbers and special characters<br>Nickname: 4~16 bits",
                "Nickname", "Please fill in your nickname", "The nickname does not conform to the specification"
            ];
            document.title = RegisterTexts[0];
            $("#RegisterTitle").text(RegisterTexts[0]);
            $("#RegisterUsernameLabel>.weui-cell__hd>span").text(RegisterTexts[1]);
            $("#RegisterUsernameInput").attr("placeholder", RegisterTexts[2]);
            $("#RegisterPasswordLabel>.weui-cell__hd>span").text(RegisterTexts[3]);
            $("#RegisterPasswordInput").attr("placeholder", RegisterTexts[4]);
            $("#RegisterPasswordLabelAgain>.weui-cell__hd>span").text(RegisterTexts[5]);
            $("#RegisterPasswordInputAgain").attr("placeholder", RegisterTexts[6]);
            $("#RegisterNicknameLabel>.weui-cell__hd>span").text(RegisterTexts[25]);
            $("#RegisterNicknameInput").attr("placeholder", RegisterTexts[26]);
            $("#RegisterEmailAddressLabel>.weui-cell__hd>span").text(RegisterTexts[7]);
            $("#RegisterEmailAddressInput").attr("placeholder", RegisterTexts[8]);
            $("#RegisterVerifyCodeLabel").text(RegisterTexts[9]);
            $("#RegisterVerifyCodeInput").attr("placeholder", RegisterTexts[10]);
            $("#RegisterVerifyCodeButton").text(RegisterTexts[11]);
            $("#RegisterGoToLogin").text(RegisterTexts[12]);
            $("#RegisterShowTips").text(RegisterTexts[13]);
            $("#RegisterSubmit").text(RegisterTexts[14]);
            $("#RegisterForm").off("keydown").on("keydown", (Event) => {
                if (Event.keyCode == 13) { $("#RegisterSubmit").click(); }
            });
            $("#RegisterUsernameInput").off("input").on("input", () => { $("#RegisterUsernameLabel").removeClass("weui-cell_warn"); });
            $("#RegisterUsernameInput").off("blur").on("blur", () => {
                var Username = $("#RegisterUsernameInput").val();
                if (Regexes["Username"].test(Username) == false) {
                    ShowError(RegisterTexts[15]);
                    $("#RegisterUsernameLabel").addClass("weui-cell_warn");
                } else {
                    RequestAPI("CheckUsernameAvailable", { "Username": Username }, () => { }, () => { }, (Response) => {
                        ShowError(Response);
                        $("#RegisterUsernameLabel").addClass("weui-cell_warn");
                    }, () => { },
                        false);
                }
            });
            $("#RegisterPasswordInput").off("input").on("input", () => { $("#RegisterPasswordLabel").removeClass("weui-cell_warn"); });
            $("#RegisterPasswordInput").off("blur").on("blur", () => {
                var Password = $("#RegisterPasswordInput").val();
                if (Password.length < 8 || Password.length > 32 || Regexes["Password"].test(Password) == true) {
                    ShowError(RegisterTexts[16]);
                    $("#RegisterPasswordLabel").addClass("weui-cell_warn");
                }
            });
            $("#RegisterPasswordInputAgain").off("input").on("input", () => { $("#RegisterPasswordLabelAgain").removeClass("weui-cell_warn"); });
            $("#RegisterPasswordInputAgain").off("blur").on("blur", () => {
                var Password = $("#RegisterPasswordInput").val();
                var PasswordAgain = $("#RegisterPasswordInputAgain").val();
                if (Password != PasswordAgain) {
                    ShowError(RegisterTexts[17]);
                    $("#RegisterPasswordLabelAgain").addClass("weui-cell_warn");
                }
            });
            $("#RegisterNicknameInput").off("input").on("input", () => { $("#RegisterNicknameLabel").removeClass("weui-cell_warn"); });
            $("#RegisterNicknameInput").off("blur").on("blur", () => {
                var Nickname = $("#RegisterNicknameInput").val();
                if (Regexes["Nickname"].test(Nickname) == false) {
                    ShowError(RegisterTexts[27]);
                    $("#RegisterNicknameLabel").addClass("weui-cell_warn");
                }
            });
            $("#RegisterEmailAddressInput").off("input").on("input", () => { $("#RegisterEmailAddressLabel").removeClass("weui-cell_warn"); });
            $("#RegisterEmailAddressInput").off("blur").on("blur", () => {
                var EmailAddress = $("#RegisterEmailAddressInput").val();
                if (Regexes["EmailAddress"].test(EmailAddress) == false) {
                    ShowError(RegisterTexts[18]);
                    $("#RegisterEmailAddressLabel").addClass("weui-cell_warn");
                }
            });
            $("#RegisterVerifyCodeInput").off("input").on("input", () => { $("#RegisterVerifyCodeLabel").removeClass("weui-cell_warn"); });
            $("#RegisterVerifyCodeInput").off("blur").on("blur", () => {
                var VerifyCode = $("#RegisterVerifyCodeInput").val();
                if (Regexes["VerifyCode"].test(VerifyCode) == false) {
                    ShowError(RegisterTexts[19]);
                    $("#RegisterVerifyCodeLabel").addClass("weui-cell_warn");
                }
            });
            $("#RegisterVerifyCodeButton").off("click").on("click", (Event) => {
                if (Event.currentTarget.classList.contains("weui-btn_disabled")) { return; }
                var EmailAddress = $("#RegisterEmailAddressInput").val();
                if (Regexes["EmailAddress"].test(EmailAddress) == false) {
                    ShowError(RegisterTexts[18]);
                    $("#RegisterEmailAddressLabel").addClass("weui-cell_warn");
                } else {
                    ShowLoading(RegisterTexts[20]);
                    RequestAPI("SendVerifyCode", { "EmailAddress": EmailAddress }, () => { HideLoading(); }, (Response) => { ShowSuccess(RegisterTexts[21]); }, () => { $("#RegisterEmailAddressLabel").addClass("weui-cell_warn"); }, () => { },
                        false);
                }
            });
            $(".RegisterInputs").off("input").on("input", () => {
                if ($("#RegisterUsernameInput").val() &&
                    $("#RegisterPasswordInput").val() &&
                    $("#RegisterPasswordInputAgain").val() &&
                    $("#RegisterEmailAddressInput").val() &&
                    $("#RegisterVerifyCodeInput").val()) { EnableButton($("#RegisterSubmit")); } else { DisableButton($("#RegisterSubmit")); }
            });
            $("#RegisterShowTips").off("click").on("click", (Event) => {
                if (Event.currentTarget.classList.contains("weui-btn_disabled")) { return; }
                ShowHalfScreenDialog(RegisterTexts[13], RegisterTexts[24]);
            });
            $("#RegisterGoToLogin").off("click").on("click", (Event) => {
                if (Event.currentTarget.classList.contains("weui-btn_disabled")) { return; }
                SwitchPage("Login", null);
            });
            $("#RegisterSubmit").off("click").on("click", (Event) => {
                if (Event.currentTarget.classList.contains("weui-btn_disabled")) { return; }
                var Username = $("#RegisterUsernameInput").val();
                var Password = $("#RegisterPasswordInput").val();
                var PasswordAgain = $("#RegisterPasswordInputAgain").val();
                var Nickname = $("#RegisterNicknameInput").val();
                var EmailAddress = $("#RegisterEmailAddressInput").val();
                var VerifyCode = $("#RegisterVerifyCodeInput").val();
                if (Regexes["Username"].test(Username) == false) {
                    ShowError(RegisterTexts[15]);
                    $("#RegisterUsernameLabel").addClass("weui-cell_warn");
                } else if (Password.length < 8 || Password.length > 32 || Regexes["Password"].test(Password) == true) {
                    ShowError(RegisterTexts[16]);
                    $("#RegisterPasswordLabel").addClass("weui-cell_warn");
                } else if (Password != PasswordAgain) {
                    ShowError(RegisterTexts[17]);
                    $("#RegisterPasswordLabelAgain").addClass("weui-cell_warn");
                } else if (Regexes["EmailAddress"].test(EmailAddress) == false) {
                    ShowError(RegisterTexts[18]);
                    $("#RegisterEmailAddressLabel").addClass("weui-cell_warn");
                } else if (Regexes["VerifyCode"].test(VerifyCode) == false) {
                    ShowError(RegisterTexts[18]);
                    $("#RegisterVerifyCodeLabel").addClass("weui-cell_warn");
                } else {
                    ShowLoading(RegisterTexts[22]);
                    RequestAPI("Register", {
                        "Username": Username,
                        "Password": Password,
                        "Nickname": Nickname,
                        "EmailAddress": EmailAddress,
                        "VerifyCode": VerifyCode
                    }, () => { HideLoading(); }, (Response) => {
                        ShowSuccess(RegisterTexts[23]);
                        localStorage.setItem("Token", Response["Token"]);
                        setTimeout(() => { SwitchPage("Login", null); }, 1000);
                    }, () => { $("#RegisterVerifyCodeLabel").addClass("weui-cell_warn"); }, () => { },
                        false);
                }
            });
        },
        "Submit": () => {
            Data["PID"] && $("#SubmitProblemIDInput").attr("value", Data["PID"]);
            var SubmitTexts = ["Submit",
                "Source code",
                "Problem ID", "Please input the problem ID here",
                "Submit",
                "Input your code here", "Or upload here", "Loading file",
                "O2 optimize",
                "Problem ID is empty", "Source code is empty", "Submitting", "Submitted"
            ];
            document.title = SubmitTexts[0];
            $("#SubmitTitle").text(SubmitTexts[0]);
            $("#SubmitCodeTitle").text(SubmitTexts[1]);
            $("#SubmitProblemIDLabel>.weui-cell__hd>span").text(SubmitTexts[2]);
            $("#SubmitProblemIDInput").attr("placeholder", SubmitTexts[3]);
            $("#SubmitEnableO2Label>.weui-cell__bd").text(SubmitTexts[8]);
            $("#SubmitSubmit").text(SubmitTexts[4]);
            $("#SubmitCodeInputLabel>.weui-cell__hd>span").text(SubmitTexts[5]);
            $("#SubmitCodeUploadLabel>.weui-cell__hd>span").text(SubmitTexts[6]);
            var Editor = CodeMirror.fromTextArea(document.getElementById("SubmitCode"), {
                mode: "text/x-c++src",
                lineNumbers: true,
                matchBrackets: true,
                indentUnit: 4,
                lineWrapping: true,
                foldGutter: true,
                gutters: ["CodeMirror-linenumbers", "CodeMirror-foldgutter"],
                extraKeys: { "Ctrl-Space": "autocomplete" }
            });
            $("#SubmitCodeUploader").off("change").on("change", (Event) => {
                ShowLoading(SubmitTexts[7]);
                var UploadFileReader = new FileReader();
                UploadFileReader.onload = function (event) {
                    HideLoading();
                    Editor.setValue(event.target.result);
                };
                UploadFileReader.readAsText(Event.target.files[0]);
                console.log(Event.target.files[0]);
            });
            Editor.on("change", () => { $(".SubmitInputs").trigger("input"); });
            $(".SubmitInputs").off("input").on("input", () => {
                if ($("#SubmitProblemIDInput").val() &&
                    Editor.getValue()) { EnableButton($("#SubmitSubmit")); } else { DisableButton($("#SubmitSubmit")); }
            });
            $("#SubmitSubmit").off("click").on("click", (Event) => {
                if (Event.currentTarget.classList.contains("weui-btn_disabled")) { return; }
                var PID = $("#SubmitProblemIDInput").val();
                var EnableO2 = $("#SubmitEnableO2Input").val() == "on";
                console.log(EnableO2);
                var Code = Editor.getValue();
                if (PID == "") {
                    ShowError(SubmitTexts[9]);
                    $("#SubmitProblemIDLabel").addClass("weui-cell_warn");
                } else if (Code == "") { ShowError(SubmitTexts[10]); } else {
                    ShowLoading(SubmitTexts[11]);
                    RequestAPI("Submit", {
                        "PID": PID,
                        "EnableO2": EnableO2,
                        "Code": Code
                    }, () => { HideLoading(); }, (Response) => {
                        ShowSuccess(SubmitTexts[12]);
                        SwitchPage("Submission", { "SID": Response["SID"] });
                    }, () => { }, () => { });
                }
            });
        },
        "Submission": () => {
            if (Data["SID"] == null) {
                SwitchPage("Error", {
                    "Message": "No submission id provide",
                    "URL": location.href
                });
            }
            var SubmissionTexts = ["Submit record", "ID", "Status", "Refresh result", "Judging", "Judged"];
            var SubmissionResultShortTexts = ["UKE", "AC", "PE", "WA", "TE", "ME", "OLE", "RE", "RF", "CE", "SE", "WT", "FC", "CP", "CPD", "JG", "JGD", "CMP", "SK", "RJ"];
            $(".TestGroup").off("click").on("click", (Event) => { Event.currentTarget.classList.toggle("HidedTestGroup"); });
            document.title = SubmissionTexts[0];
            $("#SubmissionTitle").text(SubmissionTexts[0]);
            $("#SubmissionIDLabel>.weui-cell__hd>.weui-label").text(SubmissionTexts[1]);
            $("#SubmissionIDInput").attr("value", Data["SID"]);
            $("#SubmissionStatusLabel>.weui-cell__hd>.weui-label").text(SubmissionTexts[2]);
            $("#SubmissionStatusInput").attr("value", SubmissionTexts[4]);
            $("#SubmissionRefresh").text(SubmissionTexts[3]);
            $("#SubmissionRefresh").off("click").on("click", (Event) => {
                if (Event.currentTarget.classList.contains("weui-btn_disabled")) { return; }
                $("#SubmissionRefresh")[0].classList.add("weui-btn_loading");
                $("#SubmissionRefresh").html("<i class=\"weui-mask-loading\"></i>" + SubmissionTexts[3]);
                RequestAPI("GetSubmission", { "SID": Number(Data["SID"]) }, () => {
                    $("#SubmissionRefresh")[0].classList.remove("weui-btn_loading");
                    $("#SubmissionRefresh").text(SubmissionTexts[3]);
                    $("#SubmissionJudgeResult").html("");
                }, (Response) => {
                    $("#SubmissionStatusIcon")[0].className = "weui-icon_msg weui-icon-success";
                    $("#SubmissionJudgeResult")[0].className = "TestGroups JudgeResult" + SubmissionResultShortTexts[Response.Result];
                    $("#SubmissionJudgeResult").append($("<div class=\"TestGroupsResult\">" + SubmissionResultShortTexts[Response.Result] + "</div>"));
                    $("#SubmissionJudgeResult").append($("<div class=\"TestGroupsLimit\">" + Response.Score + "pts</div>"));
                    $("#SubmissionJudgeResult").append($("<div class=\"TestGroupsLimit\">" + TimeToString(Response.TimeSum) + "</div>"));
                    $("#SubmissionJudgeResult").append($("<div class=\"TestGroupsLimit\">" + MemoryToString(Response.Memory) + "</div>"));
                    Response.TestGroups.sort((a, b) => { return (a.TGID > b.TGID ? 1 : -1); });
                    var TestCaseCounter = 1;
                    Response.TestGroups.map((TestGroup) => {
                        var TestGroupElement = $("<div class=\"TestGroup JudgeResult" + SubmissionResultShortTexts[TestGroup.Result] + "\">");
                        TestGroupElement.append($("<div class=\"TestGroupResult\">" + SubmissionResultShortTexts[TestGroup.Result] + "</div>"));
                        TestGroupElement.append($("<div class=\"TestGroupLimit\">" + TestGroup.Score + "pts</div>"));
                        TestGroupElement.append($("<div class=\"TestGroupLimit\">" + TimeToString(TestGroup.TimeSum) + "</div>"));
                        TestGroupElement.append($("<br>"));
                        TestGroup.TestCases.sort((a, b) => { return (a.TCID > b.TCID ? 1 : -1); });
                        TestGroup.TestCases.map((TestCase) => {
                            var TestCaseElement = $("<div class=\"TestCase JudgeResult" + SubmissionResultShortTexts[TestCase.Result] + (TestCase.Description == "" ? "" : " WithDescription") + "\">");
                            TestCaseElement.append($("<div class=\"TestCaseNumber\">" + TestCaseCounter + "</div>"));
                            TestCaseElement.append($("<div class=\"TestCaseResult\">" + SubmissionResultShortTexts[TestCase.Result] + "</div>"));
                            TestCaseElement.append($("<div class=\"TestCaseLimit\"><span>" + TimeToString(TestCase.Time) + "</span><span>" + TimeToString(TestCase.TimeLimit) + "</span></div>"));
                            TestCaseElement.append($("<div class=\"TestCaseLimit\"><span>" + MemoryToString(TestCase.Memory) + "</span><span>" + MemoryToString(TestCase.MemoryLimit) + "</span></div>"));
                            TestCaseElement.append($("<div class=\"TestCaseDescription\"><pre>" + TestCase.Description + "</pre></div>"));
                            TestGroupElement.append(TestCaseElement);
                            TestCaseCounter++;
                        });
                        $("#SubmissionJudgeResult").append(TestGroupElement);
                    });
                    $("#SubmissionJudgeResult").append($("<div class=\"TestGroupsDescription\"><pre>" + Response.Description + "</pre></div>"));
                    if (Response.Result <= 10) {
                        $("#SubmissionStatusInput").attr("value", SubmissionTexts[5]);
                        clearInterval(RefreshInterval);
                    }
                }, () => { clearInterval(RefreshInterval); }, () => { clearInterval(RefreshInterval); });
            });
            var RefreshInterval = setInterval(() => { $("#SubmissionRefresh").click(); }, 1000);
        },
        "Problem": () => {
            if (Data["PID"] == null) {
                SwitchPage("Error", {
                    "Message": "No problem id provide",
                    "URL": location.href
                });
            }
            var ProblemTexts = [". ", "Description", "Input", "Output",
                "Samples", "Sample", "Input", "Output", "Description", "Range", "Hint",
                "Other", "Key", "Value", "Time limit", "Memory limit",
                "Input filename", "Standard input", "Output filename", "Standard output"];
            RequestAPI("GetProblem", { "PID": Data["PID"] }, () => { }, (Response) => {
                var MarkdownData = "# " + Response.PID + ProblemTexts[0] + Response.Title + "\n" +
                    "## " + ProblemTexts[1] + "\n" +
                    Response.Description + "\n" +
                    "## " + ProblemTexts[2] + "\n" +
                    Response.Input + "\n" +
                    "## " + ProblemTexts[3] + "\n" +
                    Response.Output + "\n";
                if (Response.Samples.length > 0) {
                    MarkdownData += "## " + ProblemTexts[4] + "\n";
                    Response.Samples.map((Sample) => {
                        MarkdownData += "### " + ProblemTexts[5] + " " + (Response.Samples.indexOf(Sample) + 1) + "\n" +
                            "#### " + ProblemTexts[6] + "\n```\n" +
                            Sample.Input + "\n```\n" +
                            "#### " + ProblemTexts[7] + "\n```\n" +
                            Sample.Output + "\n```\n";
                        if (Sample.Description != null) {
                            MarkdownData += "#### " + ProblemTexts[8] + "\n" +
                                Sample.Description + "\n";
                        }
                    });
                }
                if (Response.Range != "") {
                    MarkdownData += "## " + ProblemTexts[9] + "\n" +
                        Response.Range + "\n";
                }
                if (Response.Hint != "") {
                    MarkdownData += "## " + ProblemTexts[10] + "\n" +
                        Response.Hint + "\n";
                }
                MarkdownData += "## " + ProblemTexts[11] + "\n" +
                    "| " + ProblemTexts[12] + " | " + ProblemTexts[13] + " |\n" +
                    "|:----|:------|\n";
                var MaxTimeLimit = 0;
                var MaxMemoryLimit = 0;
                Response.TestGroups.map((TestGroup) => {
                    TestGroup.TestCases.map((TestCase) => {
                        MaxTimeLimit = Math.max(MaxTimeLimit, TestCase.TimeLimit);
                        MaxMemoryLimit = Math.max(MaxMemoryLimit, TestCase.MemoryLimit);
                    });
                });
                MarkdownData += "| Time Limit | " + TimeToString(MaxTimeLimit) + " |\n" +
                    "| Memory Limit | " + MemoryToString(MaxMemoryLimit) + " |\n" +
                    "| Input filename | `" + (Response.InputFilename == "" ? Response.InputFilename : "Standard input") + "` |\n" +
                    "| Output filename | `" + (Response.OutputFilename == "" ? Response.OutputFilename : "Standard output") + "` |\n";
                $("#ProblemData").html(marked.parse(MarkdownData));
                $("#ProblemData").append($("<a class=\"weui-btn weui-btn_primary\" id=\"ProblemSubmit\"></a>"));
                $("#ProblemSubmit").text("123");
                renderMathInElement(document.body,
                    {
                        delimiters: [
                            { left: "$$", right: "$$", display: true },
                            { left: "$", right: "$", display: false }
                        ]
                    }
                );
                $("pre>code").off("click").on("click", (Event) => {
                    var InputElement = document.createElement("input");
                    InputElement.value = Event.currentTarget.innerText;
                    InputElement.style.position = "fixed";
                    InputElement.style.left = "-100px";
                    document.body.appendChild(InputElement);
                    InputElement.select();
                    document.execCommand("copy");
                    ShowSuccess("Copied");
                    InputElement.remove();
                });
            }, () => { }, () => { });
        }
    };
    if (LoadPageCallbacks[Name]) { LoadPageCallbacks[Name](); } else {
        SwitchPage("Error", {
            "Message": "Not found",
            "URL": location.href
        });
    }
}
$(() => {
    var PageName = new String(new URL(location.href).pathname).substr(1) || "Login";
    var PageParams = Object();
    new URL(location.href).searchParams.forEach((Value, Key) => { PageParams[Key] = Value; });
    LoadPage(PageName, PageParams);
    window.onselectstart = () => { return false; };
});

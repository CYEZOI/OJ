const NavigateBar = document.getElementById("NavigateBar");
const MainContainer = document.getElementById("MainContainer");
const AlertList = document.getElementById("AlertList");
const ToastList = document.getElementById("ToastList");

const SubmissionResultShortTexts = ["UKE", "AC", "PE", "WA", "TLE", "ME", "OLE", "RE", "RF", "CE", "SE", "WT", "FC", "CP", "CPD", "JG", "JGD", "CMP", "SK", "RJ"];
const SubmissionResultTexts = ["Unknown error", "Accepted", "Presentation error", "Wrong answer", "Time Limit Exceeded", "Memory Limit Exceeded", "Output Limit Exceeded", "Runtime error", "Restricted function", "Compilation error", "System error", "Waiting", "Fetched", "Compiling", "Compiled", "Judging", "Judged", "Comparing", "Skipped", "Rejected"];
const SubmissionResultColors = ["#0000FF", "#49CD32", "#FF778E", "#D3140D", "#0C0066", "#5300A7", "#8300A7", "#1A26C8", "#009182", "#8B7400", "#000000", "#4100D9", "#4C00FF", "#5E19FF", "#7340FF", "#844FFF", "#967FFF", "#A87DFF", "#4B4B4B", "#4E0000"];
const RoleName = ["Admin", "User", "Guest", "Banned"];
const RoleColors = ["#FF0000", "#0000FF", "#00FF00", "#000000"];
const Regexes = {
    "Username": /^[0-9a-zA-Z]{4,16}$/,
    "Password": /^([^a-z]+|[^A-Z]+|[^0-9]+|[a-zA-Z0-9]+|)$/,
    "Nickname": /^.{4,16}$/,
    "EmailAddress": /^([a-zA-Z0-9_-])+@([a-zA-Z0-9_-])+(.[a-zA-Z0-9_-])+$/,
    "EmailVerificationCode": /^[0-9]{6}$/
};
const Requirements = {
    "Username": "Username must be 4-16 characters and only contain numbers and letters",
    "Password": "Password must contain at least one uppercase letter, one lowercase letter, one number and one special character",
    "Nickname": "Nickname must be 4-16 characters",
    "EmailAddress": "Email address is invalid",
    "EmailVerificationCode": "Verification code must contain six numbers"
};

const CreateResultSelect = (Element, Value) => {
    Element.className = "form-select";
    for (let i = 0; i < SubmissionResultShortTexts.length; i++) {
        let Option = document.createElement("option"); Element.appendChild(Option);
        Option.innerText = SubmissionResultTexts[i];
        Option.style.color = "white";
        Option.style.backgroundColor = SubmissionResultColors[i];
    }
    Element.selectedIndex = Value;
    Element.onchange = () => {
        let Result = Element.selectedIndex;
        Element.style.color = "white";
        Element.style.backgroundColor = SubmissionResultColors[Result];
    }
    Element.onchange();
}
const CreateRoleSelect = (Element, Value) => {
    Element.className = "form-select";
    for (let i = 0; i < RoleName.length; i++) {
        let Option = document.createElement("option"); Element.appendChild(Option);
        Option.innerText = RoleName[i];
        Option.style.color = RoleColors[i];
    }
    Element.selectedIndex = Value;
    Element.onchange = () => {
        let Role = Element.selectedIndex;
        Element.style.color = RoleColors[Role];
    }
    Element.onchange();
}
const MemoryToString = (Memory) => {
    if (Memory < 1024) { return Memory + "B"; } else if (Memory < 1024 * 1024) { return Number(Memory / 1024).toFixed(0) + "KB"; } else { return Number(Memory / 1024 / 1024).toFixed(0) + "MB"; }
};
const TimeToString = (Time) => {
    if (Time < 1000) { return Time + "ms"; } else { return Number(Time / 1000).toFixed(0) + "s"; }
};
const CopyTextToClipboard = (Text) => {
    let TextArea = document.createElement("textarea"); document.body.appendChild(TextArea);
    TextArea.value = Text;
    TextArea.style.position = "fixed";
    TextArea.style.top = "0";
    TextArea.style.left = "0";
    TextArea.style.width = "2em";
    TextArea.style.height = "2em";
    TextArea.style.padding = "0";
    TextArea.style.border = "none";
    TextArea.style.outline = "none";
    TextArea.style.boxShadow = "none";
    TextArea.style.background = "transparent";
    TextArea.focus();
    TextArea.select();
    try {
        document.execCommand("copy");
        ShowSuccess("Copied!");
    } catch (e) {
        ShowError("Failed to copy!");
    }
    document.body.removeChild(TextArea);
};
const CreateVditorEditor = (Editor, Content) => {
    return new Vditor(Editor, {
        "cache": { "enable": false },
        "lang": "en_US",
        "placeholder": "Input the markdown here...",
        "preview": { "actions": [] },
        "value": Content,
        "upload": {
            "url": "/api",
            "token": localStorage.getItem("Token")
        }
    })
}
const ShowModal = (Title, Body, PrimaryButtonOnClick, SecondaryButtonOnClick, PrimaryButtonColor = "danger", SecondaryButtonColor = "secondary") => {
    let Modal = document.createElement("div"); document.body.appendChild(Modal);
    Modal.id = "Modal";
    Modal.classList.add("modal");
    Modal.classList.add("fade");
    Modal.setAttribute("data-bs-backdrop", "static");
    Modal.setAttribute("data-bs-keyboard", "false");
    Modal.tabIndex = "-1";
    {
        let ModalDialog = document.createElement("div"); Modal.appendChild(ModalDialog);
        ModalDialog.classList.add("modal-dialog");
        {
            let ModalContent = document.createElement("div"); ModalDialog.appendChild(ModalContent);
            ModalContent.classList.add("modal-content");
            {
                let ModalHeader = document.createElement("div"); ModalContent.appendChild(ModalHeader);
                ModalHeader.classList.add("modal-header");
                {
                    let ModalTitle = document.createElement("h1"); ModalHeader.appendChild(ModalTitle);
                    ModalTitle.classList.add("modal-title");
                    ModalTitle.classList.add("fs-5");
                    ModalTitle.innerText = Title;
                }
                let ModalBody = document.createElement("div"); ModalContent.appendChild(ModalBody);
                ModalBody.classList.add("modal-body");
                ModalBody.innerHTML = Body;
                let ModalFooter = document.createElement("div"); ModalContent.appendChild(ModalFooter);
                ModalFooter.classList.add("modal-footer");
                {
                    let SecondaryButton = document.createElement("button"); ModalFooter.appendChild(SecondaryButton);
                    SecondaryButton.classList.add("btn");
                    SecondaryButton.classList.add("btn-" + SecondaryButtonColor);
                    SecondaryButton.type = "button";
                    SecondaryButton.setAttribute("data-bs-dismiss", "modal");
                    SecondaryButton.innerText = "No";
                    SecondaryButton.onclick = () => {
                        SecondaryButtonOnClick();
                        setTimeout(() => {
                            Modal.remove();
                        }, 500);
                    };
                    let PrimaryButton = document.createElement("button"); ModalFooter.appendChild(PrimaryButton);
                    PrimaryButton.classList.add("btn");
                    PrimaryButton.classList.add("btn-" + PrimaryButtonColor);
                    PrimaryButton.type = "button";
                    PrimaryButton.setAttribute("data-bs-dismiss", "modal");
                    PrimaryButton.innerText = "Yes";
                    PrimaryButton.onclick = () => {
                        PrimaryButtonOnClick();
                        setTimeout(() => {
                            Modal.remove();
                        }, 500);
                    }
                }
            }
        }
    }
    let ModalInstance = new bootstrap.Modal(Modal);
    ModalInstance.show();
}
const ShowAlert = (Text, Type) => {
    for (var i = 0; i < AlertList.children.length; i++) {
        if (AlertList.children[i].innerText == Text) {
            AlertList.children[i].remove();
        }
    }
    var Alert = document.createElement("div"); AlertList.appendChild(Alert);
    Alert.classList.add("alert");
    Alert.classList.add("alert-" + Type);
    Alert.classList.add("alert-dismissible");
    Alert.style.width = "400px";
    Alert.innerHTML = Text;
    {
        var CloseButton = document.createElement("button"); Alert.appendChild(CloseButton);
        CloseButton.classList.add("btn-close");
        CloseButton.type = "button";
        CloseButton.onclick = () => {
            Alert.remove();
        };
    }
    setTimeout(() => {
        Alert.remove();
    }, 3000);
};
const ShowError = (Text) => {
    ShowAlert(Text, "danger");
};
const ShowSuccess = (Text) => {
    ShowAlert(Text, "success");
};
const ShowToast = (Title, Body, Small = "") => {
    let Toast = document.createElement("div"); ToastList.appendChild(Toast);
    Toast.classList.add("toast");
    Toast.classList.add("m-3");
    Toast.role = "alert";
    {
        let ToastHeader = document.createElement("div"); Toast.appendChild(ToastHeader);
        ToastHeader.classList.add("toast-header");
        {
            let ToastTitle = document.createElement("strong"); ToastHeader.appendChild(ToastTitle);
            ToastTitle.classList.add("me-auto");
            ToastTitle.innerHTML = Title;
            let ToastSmall = document.createElement("small"); ToastHeader.appendChild(ToastSmall);
            ToastSmall.innerHTML = Small;
            let ToastClose = document.createElement("button"); ToastHeader.appendChild(ToastClose);
            ToastClose.type = "button";
            ToastClose.classList.add("btn-close");
            ToastClose.setAttribute("data-bs-dismiss", "toast");
        }
    }
    {
        let ToastBody = document.createElement("div"); Toast.appendChild(ToastBody);
        ToastBody.classList.add("toast-body");
        ToastBody.innerHTML = Body;
    }
    bootstrap.Toast.getOrCreateInstance(Toast).show();
}
const AddLoading = (Element) => {
    let Loading = document.createElement("span"); Element.appendChild(Loading);
    Loading.classList.add("ms-2");
    Loading.classList.add("spinner-border");
    Loading.classList.add("spinner-border-sm");
    Loading.role = "status";
};
const RemoveLoading = (Element) => {
    Element.disabled = false;
    if (Element.lastChild.classList.contains("spinner-border"))
        Element.removeChild(Element.lastChild);
};
const SetValid = (Element, Valid = null) => {
    Element.classList.remove("is-valid");
    Element.classList.remove("is-invalid");
    if (Valid === true) {
        Element.classList.add("is-valid");
    }
    else if (Valid === false) {
        Element.classList.add("is-invalid");
    }
}
const CreatePlaceHolder = () => {
    let PlaceHolder = document.createElement("span");
    PlaceHolder.classList.add("placeholder");
    PlaceHolder.classList.add("col-" + (Math.floor(Math.random() * 12) + 1));
    return PlaceHolder;
};
const GetToken = () => {
    var Token = localStorage.getItem("Token");
    if (Token != null) {
        RequestAPI("CheckTokenAvailable", {
            "Token": String(localStorage.getItem("Token"))
        }, () => { }, () => { }, () => {
            localStorage.removeItem("Token");
            Token = null;
        }, () => { Token = null; }, false);
    }
    return Token;
};
const RequestAPI = async (Action, Data, CallBack, SuccessCallback, FailCallback, ErrorCallback, AddToken = true) => {
    if (AddToken) {
        Data.Token = GetToken();
        if (Data.Token == null) {
            CallBack();
            ErrorCallback();
            ShowError("Login expired");
            SwitchPage("Login");
            return;
        }
    }
    // let Timeout = new AbortController();
    // setTimeout(() => {
    //     Timeout.abort();
    //     CallBack();
    //     ErrorCallback();
    //     ShowError("Request timeout");
    // }, 3000);
    await fetch("/api", {
        method: "POST",
        headers: {
            "Content-Type": "application/json"
        },
        // signal: Timeout.signal,
        body: JSON.stringify({
            "Action": Action,
            "Data": Data
        })
    }).then(Response => {
        CallBack();
        if (Response.status == 200) {
            Response.json().then(Response => {
                if (Response.Success) {
                    SuccessCallback(Response.Data);
                } else {
                    ShowError(Response.Message);
                    FailCallback();
                }
            });
        } else {
            ShowError("Request failed: " + Response.status + " " + Response.statusText);
            ErrorCallback();
        }
    }).catch((Error) => {
        CallBack();
        ShowError("Request failed: " + Error);
        ErrorCallback();
    });
};
const SwitchPage = async (Path, Data = {}, PushState = true) => {
    if (PushState) {
        let URLPath = "index.html?Page=" + Path;
        for (let i in Data) {
            URLPath += "&" + encodeURIComponent(i) + "=" + encodeURIComponent(Data[i]);
        }
        history.pushState({
            "Path": Path,
            "Data": Data
        }, Path, URLPath);
    }

    let LoweredPath = "";
    for (let i = 0; i < Path.length; i++) {
        if (i != 0 && Path.charCodeAt(i) >= 65 && Path.charCodeAt(i) <= 90) {
            LoweredPath += " " + Path[i].toLowerCase();
        }
        else {
            LoweredPath += Path[i];
        }
    }
    document.title = LoweredPath;
    for (let i = 0; i < NavigateBar.children[0].children.length; i++) {
        if (NavigateBar.children[0].children[i].innerText == Path) {
            NavigateBar.children[0].children[i].children[0].classList.add("active");
        }
        else {
            NavigateBar.children[0].children[i].children[0].classList.remove("active");
        }
    }

    MainContainer.innerHTML = "";
    let PlaceHolderTitle = document.createElement("h4"); MainContainer.appendChild(PlaceHolderTitle);
    PlaceHolderTitle.appendChild(CreatePlaceHolder());
    let PlaceHolderBody = document.createElement("p"); MainContainer.appendChild(PlaceHolderBody);
    for (let i = 0; i < 10; i++) {
        PlaceHolderBody.appendChild(CreatePlaceHolder());
    }

    await fetch(Path + ".html")
        .then((HTMLResponse) => {
            return HTMLResponse.text();
        }).then(async (HTMLResponse) => {
            await fetch(Path + ".js")
                .then((JSResponse) => {
                    return JSResponse.text();
                }).then((JSResponse) => {
                    MainContainer.innerHTML = "<h4>" + LoweredPath + "</h4>"
                        + HTMLResponse;
                    window.Data = Data;
                    eval(JSResponse);
                });
        });
}
const CheckTokenAvailable = () => {
    var Token = localStorage.getItem("Token");
    if (Token == null) {
        SwitchPage("Login");
        return;
    }
    RequestAPI("CheckTokenAvailable", {
        "Token": String(localStorage.getItem("Token"))
    }, () => { }, () => { }, () => {
        localStorage.removeItem("Token");
        SwitchPage("Login");
    }, () => { }, false);
};
const CreateHorizontalLine = () => {
    let HorizontalLine = document.createElement("hr");
    HorizontalLine.classList.add("border");
    HorizontalLine.classList.add("border-2");
    HorizontalLine.classList.add("border-primary");
    HorizontalLine.classList.add("rounded");
    return HorizontalLine;
};
const CreateMarkdownEditor = (Title, Content, Parent) => {
    let VditorEditor;
    let Container = document.createElement("div"); Parent.appendChild(Container);
    Container.classList.add("form-group");
    {
        let Label = document.createElement("label"); Container.appendChild(Label);
        Label.classList.add("col-form-label");
        Label.innerHTML = Title;
        let Editor = document.createElement("div"); Container.appendChild(Editor);
        VditorEditor = CreateVditorEditor(Editor, Content);
        Container.appendChild(CreateHorizontalLine());
    }
    return VditorEditor;
};

(() => {
    if (localStorage.getItem("IsAdmin") !== null) {
        const AddonStyle = document.getElementById("AddonStyle");
        AddonStyle.innerHTML = ".NotLoginOnly { display: none; }";
        if (localStorage.getItem("IsAdmin") != true) {
            AddonStyle.innerHTML += ".AdminOnly { display: none; }";
        }
    }

    if (SubmissionResultShortTexts.length != SubmissionResultTexts.length || SubmissionResultShortTexts.length != SubmissionResultColors.length) {
        MainContainer.innerHTML = "System error: SubmissionResultShortTexts, SubmissionResultTexts and SubmissionResultColors have different length";
        return;
    }
    for (let i = 0; i < NavigateBar.children[0].children.length; i++) {
        NavigateBar.children[0].children[i].children[0].onclick = () => {
            SwitchPage(NavigateBar.children[0].children[i].children[0].innerText);
        };
    }
    if (new URLSearchParams(window.location.search).get("Page") !== null) {
        let Data = {};
        for (let i of new URLSearchParams(window.location.search).entries()) {
            if (i[0] != "Page") {
                Data[i[0]] = i[1];
            }
        }
        SwitchPage(new URLSearchParams(window.location.search).get("Page"), Data, false);
    }
    else {
        SwitchPage("Home");
    }
})();
onpopstate = (Event) => {
    if (Event.isTrusted && Event.state != null) {
        SwitchPage(Event.state.Path, Event.state.Data, false);
    }
};

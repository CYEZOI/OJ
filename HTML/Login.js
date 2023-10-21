const LoginUsernameInput = document.getElementById("LoginUsernameInput");
const LoginPasswordInput = document.getElementById("LoginPasswordInput");
const LoginButton = document.getElementById("LoginButton");

LoginUsernameInput.oninput = () => {
    SetValid(LoginUsernameInput);
}
LoginUsernameInput.onblur = () => {
    if (!Regexes.Username.test(LoginUsernameInput.value)) {
        SetValid(LoginUsernameInput, false);
        ShowError(Requirements.Username);
    }
    else {
        SetValid(LoginUsernameInput, true);
    }
}
LoginPasswordInput.oninput = () => {
    SetValid(LoginPasswordInput);
}
LoginPasswordInput.onblur = () => {
    if (Regexes.Password.test(LoginPasswordInput.value)) {
        SetValid(LoginPasswordInput, false);
        ShowError(Requirements.Password);
    }
    else {
        SetValid(LoginPasswordInput, true);
    }
}
LoginButton.onclick = () => {
    AddLoading(LoginButton);
    RequestAPI("Login", {
        "Username": String(LoginUsernameInput.value),
        "Password": String(LoginPasswordInput.value)
    }, () => {
        RemoveLoading(LoginButton);
    }, (Response) => {
        localStorage.setItem("Token", Response.Token);
        ShowSuccess("Login success");
        setTimeout(() => {
            SwitchPage("Home");
        }, 1000);
    }, () => {
        SetValid(LoginPasswordInput, false);
    }, () => { }, false);
};
if (localStorage.getItem("Token") != null) {
    RequestAPI("CheckTokenAvailable",
        {
            "Token": String(localStorage.getItem("Token"))
        }, () => { }, () => {
            SwitchPage("Home");
        }, () => {
            localStorage.removeItem("Token");
        }, () => { }, false);
}

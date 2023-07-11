RegisterUsernameInput.oninput = () => {
    RegisterNicknameInput.value = RegisterUsernameInput.value;
    SetValid(RegisterUsernameInput);
}
RegisterUsernameInput.onblur = () => {
    if (!Regexes.Username.test(RegisterUsernameInput.value)) {
        SetValid(RegisterUsernameInput, false);
        ShowError(Requirements.Username);
    }
    else {
        RequestAPI("CheckUsernameAvailable", {
            "Username": String(RegisterUsernameInput.value)
        }, () => { }, () => {
            SetValid(RegisterUsernameInput, true);
        }, (Response) => {
            SetValid(RegisterUsernameInput, false);
        }, () => { }, false);
    }
}
RegisterNicknameInput.oninput = () => {
    SetValid(RegisterNicknameInput);
}
RegisterNicknameInput.onblur = () => {
    if (!Regexes.Nickname.test(RegisterNicknameInput.value)) {
        SetValid(RegisterNicknameInput, false);
        ShowError(Requirements.Nickname);
    }
    else {
        SetValid(RegisterNicknameInput, true);
    }
}
RegisterPasswordInput.oninput = () => {
    SetValid(RegisterPasswordInput);
    RegisterPasswordRepeatInput.value = "";
    SetValid(RegisterPasswordRepeatInput);
}
RegisterPasswordInput.onblur = () => {
    if (Regexes.Password.test(RegisterPasswordInput.value)) {
        SetValid(RegisterPasswordInput, false);
        ShowError(Requirements.Password);
    }
    else {
        SetValid(RegisterPasswordInput, true);
    }
}
RegisterPasswordRepeatInput.oninput = () => {
    SetValid(RegisterPasswordRepeatInput);
}
RegisterPasswordRepeatInput.onblur = () => {
    if (RegisterPasswordRepeatInput.value != RegisterPasswordInput.value) {
        SetValid(RegisterPasswordRepeatInput, false);
        ShowError("Password does not match");
    }
    else {
        SetValid(RegisterPasswordRepeatInput, true);
    }
}
RegisterEmailInput.oninput = () => {
    SetValid(RegisterEmailInput);
    RegisterEmailVerificationCodeButton.disabled = false;
}
RegisterEmailInput.onblur = () => {
    if (!Regexes.EmailAddress.test(RegisterEmailInput.value)) {
        SetValid(RegisterEmailInput, false);
        ShowError(Requirements.EmailAddress);
        RegisterEmailVerificationCodeButton.disabled = true;
    }
    else {
        RequestAPI("CheckEmailAvailable", {
            "EmailAddress": String(RegisterEmailInput.value)
        }, () => { }, () => {
            SetValid(RegisterEmailInput, true);
        }, (Response) => {
            SetValid(RegisterEmailInput, false);
            RegisterEmailVerificationCodeButton.disabled = true;
        }, () => { }, false);
    }
}
RegisterEmailVerificationCodeButton.onclick = () => {
    AddLoading(RegisterEmailVerificationCodeButton);
    RequestAPI("SendVerificationCode", {
        "EmailAddress": String(RegisterEmailInput.value)
    }, () => {
        RemoveLoading(RegisterEmailVerificationCodeButton);
    }, () => {
        ShowSuccess("Verification code has been sent");
        RegisterEmailVerificationCodeInput.focus();
        RegisterEmailVerificationCodeButton.dataset.time = 60;
        RegisterEmailVerificationCodeButton.disabled = true;
        RegisterEmailVerificationCodeButton.innerText = "Resend (" + RegisterEmailVerificationCodeButton.dataset.time + ")";
        RegisterEmailVerificationCodeButton.dataset.time--;
        setInterval(() => {
            RegisterEmailVerificationCodeButton.innerText = "Resend (" + RegisterEmailVerificationCodeButton.dataset.time + ")";
            RegisterEmailVerificationCodeButton.dataset.time--;
            if (RegisterEmailVerificationCodeButton.dataset.time <= 0) {
                RegisterEmailVerificationCodeButton.innerText = "Resend";
                RegisterEmailVerificationCodeButton.disabled = false;
                clearInterval();
            }
        }, 1000);
    }, () => { }, () => { }, false);
}
RegisterEmailVerificationCodeInput.oninput = () => {
    SetValid(RegisterEmailVerificationCodeInput);
}
RegisterEmailVerificationCodeInput.onblur = () => {
    if (!Regexes.EmailVerificationCode.test(RegisterEmailVerificationCodeInput.value)) {
        SetValid(RegisterEmailVerificationCodeInput, false);
        ShowError("Verification code must contain six numbers");
    }
    else {
        SetValid(RegisterEmailVerificationCodeInput, true);
    }
}
RegisterButton.onclick = () => {
    AddLoading(RegisterButton);
    RequestAPI("Register", {
        "Username": String(RegisterUsernameInput.value),
        "Nickname": String(RegisterNicknameInput.value),
        "Password": String(RegisterPasswordInput.value),
        "EmailAddress": String(RegisterEmailInput.value),
        "VerificationCode": String(RegisterEmailVerificationCodeInput.value)
    }, () => {
        RemoveLoading(RegisterButton);
    }, () => {
        ShowSuccess("Register success");
        SwitchPage("Login");
    }, () => { }, () => { }, false);
};

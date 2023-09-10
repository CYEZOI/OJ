CheckTokenAvailable();
AddUserUsernameInput.oninput = () => {
    AddUserNicknameInput.value = AddUserUsernameInput.value;
    SetValid(AddUserUsernameInput);
}
AddUserUsernameInput.onblur = () => {
    if (AddUserUsernameInput.value == "") {
        SetValid(AddUserUsernameInput, false);
        ShowError("Username can't be empty");
    }
    else {
        RequestAPI("CheckUsernameAvailable", {
            "Username": String(AddUserUsernameInput.value)
        }, () => { }, () => {
            SetValid(AddUserUsernameInput, true);
        }, (Response) => {
            SetValid(AddUserUsernameInput, false);
        }, () => { }, false);
    }
}
AddUserNicknameInput.oninput = () => {
    SetValid(AddUserNicknameInput);
}
AddUserNicknameInput.onblur = () => {
    if (AddUserNicknameInput.value == "") {
        SetValid(AddUserNicknameInput, false);
        ShowError("Nickname can't be empty");
    }
    else {
        SetValid(AddUserNicknameInput, true);
    }
}
AddUserPasswordInput.oninput = () => {
    SetValid(AddUserPasswordInput);
}
AddUserPasswordInput.onblur = () => {
    if (AddUserPasswordInput.value == "") {
        SetValid(AddUserPasswordInput, false);
        ShowError("Password can't be empty");
    }
    else {
        SetValid(AddUserPasswordInput, true);
    }
}
AddUserEmailInput.oninput = () => {
    SetValid(AddUserEmailInput);
}
AddUserEmailInput.onblur = () => {
    if (AddUserEmailInput.value == "") {
        SetValid(AddUserEmailInput, false);
        ShowError("Email can't be empty");
    }
    else {
        RequestAPI("CheckEmailAvailable", {
            "EmailAddress": String(AddUserEmailInput.value)
        }, () => { }, () => {
            SetValid(AddUserEmailInput, true);
        }, (Response) => {
            SetValid(AddUserEmailInput, false);
            AddUserEmailVerificationCodeButton.disabled = true;
        }, () => { }, false);
    }
}
AddUserButton.onclick = () => {
    AddLoading(AddUserButton);
    RequestAPI("AddUser", {
        "Username": String(AddUserUsernameInput.value),
        "Nickname": String(AddUserNicknameInput.value),
        "Password": String(AddUserPasswordInput.value),
        "EmailAddress": String(AddUserEmailInput.value),
        "Role": Number(AddUserRoleSelect.selectedIndex)
    }, () => {
        RemoveLoading(AddUserButton);
    }, () => {
        ShowSuccess("AddUser success");
        setTimeout(() => {
            SwitchPage("Users");
        }, 1000);
    }, () => { }, () => { });
};

CreateRoleSelect(AddUserRoleSelect, 1);

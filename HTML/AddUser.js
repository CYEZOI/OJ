const AddUserUsernameInput = document.getElementById("AddUserUsernameInput");
const AddUserNicknameInput = document.getElementById("AddUserNicknameInput");
const AddUserPasswordInput = document.getElementById("AddUserPasswordInput");
const AddUserEmailInput = document.getElementById("AddUserEmailInput");
const AddUserRoleSelect = document.getElementById("AddUserRoleSelect");
const AddUserButton = document.getElementById("AddUserButton");
CheckTokenAvailable();

AddUserUsernameInput.oninput = () => {
    AddUserNicknameInput.value = AddUserUsernameInput.value;
    SetValid(AddUserUsernameInput);
}
AddUserUsernameInput.onblur = () => {
    if (!Regexes.Username.test(AddUserUsernameInput.value)) {
        SetValid(AddUserUsernameInput, false);
        ShowError(Requirements.Username);
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
    if (!Regexes.Nickname.test(AddUserNicknameInput.value)) {
        SetValid(AddUserNicknameInput, false);
        ShowError(Requirements.Nickname);
    }
    else {
        SetValid(AddUserNicknameInput, true);
    }
}
AddUserPasswordInput.oninput = () => {
    SetValid(AddUserPasswordInput);
}
AddUserPasswordInput.onblur = () => {
    if (Regexes.Password.test(AddUserPasswordInput.value)) {
        SetValid(AddUserPasswordInput, false);
        ShowError(Requirements.Password);
    }
    else {
        SetValid(AddUserPasswordInput, true);
    }
}
AddUserEmailInput.oninput = () => {
    SetValid(AddUserEmailInput);
}
AddUserEmailInput.onblur = () => {
    if (!Regexes.EmailAddress.test(AddUserEmailInput.value)) {
        SetValid(AddUserEmailInput, false);
        ShowError(Requirements.EmailAddress);
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
        setTimeout(() => {
            SwitchPage("Users");
        }, 1000);
    }, () => { }, () => { });
};

CreateRoleSelect(AddUserRoleSelect, 1);

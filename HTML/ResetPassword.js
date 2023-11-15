const ResetPasswordEmailInput = document.getElementById("ResetPasswordEmailInput");
const ResetPasswordEmailVerificationCodeButton = document.getElementById("ResetPasswordEmailVerificationCodeButton");
const ResetPasswordEmailVerificationCodeInput = document.getElementById("ResetPasswordEmailVerificationCodeInput");
const ResetPasswordPasswordInput = document.getElementById("ResetPasswordPasswordInput");
const ResetPasswordPasswordRepeatInput = document.getElementById("ResetPasswordPasswordRepeatInput");
const ResetPasswordButton = document.getElementById("ResetPasswordButton");

ResetPasswordEmailInput.oninput = () => {
    SetValid(ResetPasswordEmailInput);
    ResetPasswordEmailVerificationCodeButton.disabled = false;
}
ResetPasswordEmailInput.onblur = () => {
    if (!Regexes.EmailAddress.test(ResetPasswordEmailInput.value)) {
        SetValid(ResetPasswordEmailInput, false);
        ShowError(Requirements.EmailAddress);
        ResetPasswordEmailVerificationCodeButton.disabled = true;
    }
    else {
        SetValid(ResetPasswordEmailInput, true);
    }
}
ResetPasswordEmailVerificationCodeButton.onclick = () => {
    AddLoading(ResetPasswordEmailVerificationCodeButton);
    RequestAPI("SendVerificationCode", {
        "EmailAddress": String(ResetPasswordEmailInput.value)
    }, () => {
        RemoveLoading(ResetPasswordEmailVerificationCodeButton);
    }, () => {
        ShowSuccess("Verification code has been sent");
        ResetPasswordEmailVerificationCodeInput.focus();
        ResetPasswordEmailVerificationCodeButton.dataset.time = 60;
        ResetPasswordEmailVerificationCodeButton.disabled = true;
        ResetPasswordEmailVerificationCodeButton.innerText = "Resend (" + ResetPasswordEmailVerificationCodeButton.dataset.time + ")";
        ResetPasswordEmailVerificationCodeButton.dataset.time--;
        setInterval(() => {
            ResetPasswordEmailVerificationCodeButton.innerText = "Resend (" + ResetPasswordEmailVerificationCodeButton.dataset.time + ")";
            ResetPasswordEmailVerificationCodeButton.dataset.time--;
            if (ResetPasswordEmailVerificationCodeButton.dataset.time <= 0) {
                ResetPasswordEmailVerificationCodeButton.innerText = "Resend";
                ResetPasswordEmailVerificationCodeButton.disabled = false;
                clearInterval();
            }
        }, 1000);
    }, () => { }, () => { }, false);
}
ResetPasswordEmailVerificationCodeInput.oninput = () => {
    SetValid(ResetPasswordEmailVerificationCodeInput);
}
ResetPasswordEmailVerificationCodeInput.onblur = () => {
    if (!Regexes.EmailVerificationCode.test(ResetPasswordEmailVerificationCodeInput.value)) {
        SetValid(ResetPasswordEmailVerificationCodeInput, false);
        ShowError("Verification code must contain six numbers");
    }
    else {
        SetValid(ResetPasswordEmailVerificationCodeInput, true);
    }
}
ResetPasswordPasswordInput.oninput = () => {
    SetValid(ResetPasswordPasswordInput);
    ResetPasswordPasswordRepeatInput.value = "";
    SetValid(ResetPasswordPasswordRepeatInput);
}
ResetPasswordPasswordInput.onblur = () => {
    if (Regexes.Password.test(ResetPasswordPasswordInput.value)) {
        SetValid(ResetPasswordPasswordInput, false);
        ShowError(Requirements.Password);
    }
    else {
        SetValid(ResetPasswordPasswordInput, true);
    }
}
ResetPasswordPasswordRepeatInput.oninput = () => {
    SetValid(ResetPasswordPasswordRepeatInput);
}
ResetPasswordPasswordRepeatInput.onblur = () => {
    if (ResetPasswordPasswordRepeatInput.value != ResetPasswordPasswordInput.value) {
        SetValid(ResetPasswordPasswordRepeatInput, false);
        ShowError("Password does not match");
    }
    else {
        SetValid(ResetPasswordPasswordRepeatInput, true);
    }
}
ResetPasswordButton.onclick = () => {
    AddLoading(ResetPasswordButton);
    RequestAPI("ResetPassword", {
        "EmailAddress": String(ResetPasswordEmailInput.value),
        "VerificationCode": String(ResetPasswordEmailVerificationCodeInput.value),
        "Password": String(ResetPasswordPasswordInput.value)
    }, () => {
        RemoveLoading(ResetPasswordButton);
    }, () => {
        ShowSuccess("Reset password success");
        setTimeout(() => {
            SwitchPage("Login");
        }, 1000);
    }, () => { }, () => { }, false);
};

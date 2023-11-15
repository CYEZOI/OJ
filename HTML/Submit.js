const SubmitCode = document.getElementById("SubmitCode");
const SubmitEnableO2 = document.getElementById("SubmitEnableO2");
const SubmitButton = document.getElementById("SubmitButton");

CheckTokenAvailable();
if (Data.PID == null) {
    SwitchPage("Home");
}
PageTitle.innerHTML += " " + Data.PID;
let SubmitCodeMirror = CreateCodeMirrorSourceEditor(SubmitCode, () => {
    SubmitButton.onclick();
});
SubmitButton.onclick = () => {
    AddLoading(SubmitButton);
    RequestAPI("AddSubmission", {
        PID: String(Data.PID),
        Code: String(SubmitCodeMirror.getValue()),
        EnableO2: Boolean(SubmitEnableO2.checked),
    }, () => {
        RemoveLoading(SubmitButton);
    }, (Response) => {
        ShowSuccess("Submit success");
        setTimeout(() => {
            SwitchPage("Submission", {
                "SID": Response.SID
            });
        }, 1000);
    }, () => { }, () => { });
};

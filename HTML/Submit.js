const SubmitCode = document.getElementById("SubmitCode");
const SubmitEnableO2 = document.getElementById("SubmitEnableO2");
const SubmitButton = document.getElementById("SubmitButton");

CheckTokenAvailable();
if (Data.PID == null) {
    SwitchPage("Home");
}
document.getElementsByTagName("h4")[0].innerHTML += " " + Data.PID;
let SubmitCodeMirror = CodeMirror.fromTextArea(document.getElementById("SubmitCode"), {
    lineNumbers: true,
    foldGutter: true,
    matchBrackets: true,
    mode: "text/x-c++src",
    autofocus: true,
    extraKeys: {
        "Ctrl-Space": "autocomplete",
        "Ctrl-Enter": () => {
            SubmitButton.onclick();
        }
    },
    gutters: ["CodeMirror-linenumbers", "CodeMirror-foldgutter"],
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

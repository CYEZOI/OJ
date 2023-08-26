CheckTokenAvailable();
if (Data.SID == null) {
    SwitchPage("Home");
}

document.getElementsByTagName("h4")[0].innerHTML += " " + Data.SID;
let SubmissionSpinnerCollapse = new bootstrap.Collapse("#SubmissionSpinner", {
    toggle: false
});
const ReloadData = () => {
    SubmissionSpinnerCollapse.show();
    RequestAPI("GetSubmission", {
        "SID": Number(Data.SID)
    }, () => { }, (Response) => {
        if (!Response.IsAdmin) {
            SubmissionRejudgeButton.remove();
            SubmissionEditButton.remove();
            SubmissionDeleteButton.remove();
        }
        SubmissionData.innerHTML = "";
        SubmissionData.className = "";
        SubmissionGoToProblemButton.onclick = () => {
            SwitchPage("Problem", {
                "PID": Response.PID
            });
        };
        SubmissionResubmitButton.onclick = () => {
            SwitchPage("Submit", {
                "PID": Response.PID
            });
        };
        SubmissionRejudgeButton.onclick = () => {
            ShowModal("Rejudge submission", "Are you sure to rejudge it?", () => {
                if (Response.Result > 10) {
                    ShowModal("Rejudge submission", "This submission is still running, are you sure to rejudge it? This may cause the server to crash!", () => {
                        RequestAPI("RejudgeSubmission", {
                            "SID": Number(Data.SID)
                        }, () => { }, () => {
                            ReloadData();
                        }, () => { });
                    }, () => { });
                }
                else {
                    RequestAPI("RejudgeSubmission", {
                        "SID": Number(Data.SID)
                    }, () => { }, () => {
                        ReloadData();
                    }, () => { });
                }
            }, () => { });
        };
        SubmissionEditButton.onclick = () => {
            SwitchPage("EditSubmission", {
                "SID": Data.SID
            });
        };
        SubmissionDeleteButton.onclick = () => {
            ShowModal("Delete submission", "Are you sure to delete this submission?", () => {
                RequestAPI("DeleteSubmission",
                    {
                        "SID": Number(Data.SID)
                    }, () => { }, (Response) => {
                        ShowSuccess("Delete Submission Success");
                        SwitchPage("Submissions");
                    }, () => { }, () => { });
            }, () => { });
        };

        SubmissionData.classList.add("Submission");
        // SubmissionData.classList.add("JudgeResult" + SubmissionResultShortTexts[Response.Result]);
        SubmissionData.style.backgroundColor = SubmissionResultColors[Response.Result];
        {
            let SubmissionResult = document.createElement("div"); SubmissionData.appendChild(SubmissionResult);
            SubmissionResult.classList.add("SubmissionResult");
            SubmissionResult.innerText = SubmissionResultShortTexts[Response.Result];
            let SubmissionScore = document.createElement("div"); SubmissionData.appendChild(SubmissionScore);
            SubmissionScore.classList.add("SubmissionLimit");
            SubmissionScore.innerText = Response.Score + "pts";
            let SubmissionTime = document.createElement("div"); SubmissionData.appendChild(SubmissionTime);
            SubmissionTime.classList.add("SubmissionLimit");
            SubmissionTime.innerText = TimeToString(Response.TimeSum);
            let SubmissionMemory = document.createElement("div"); SubmissionData.appendChild(SubmissionMemory);
            SubmissionMemory.classList.add("SubmissionLimit");
            SubmissionMemory.innerText = MemoryToString(Response.Memory);
            Response.TestGroups = JSON.parse(Response.TestGroups);
            Response.TestGroups.sort((a, b) => {
                return (a.TGID > b.TGID ? 1 : -1);
            });
            Response.TestGroupsLimits.sort((a, b) => {
                return (a.TGID > b.TGID ? 1 : -1);
            });
            var TestCaseCounter = 1;
            Response.TestGroups.map((TestGroup) => {
                let TestGroupElement = document.createElement("div"); SubmissionData.appendChild(TestGroupElement);
                TestGroupElement.classList.add("TestGroup");
                // TestGroupElement.classList.add("JudgeResult" + SubmissionResultShortTexts[TestGroup.Result]);
                TestGroupElement.style.backgroundColor = SubmissionResultColors[TestGroup.Result];
                {
                    let TestGroupResult = document.createElement("div"); TestGroupElement.appendChild(TestGroupResult);
                    TestGroupResult.classList.add("TestGroupResult");
                    TestGroupResult.innerText = SubmissionResultShortTexts[TestGroup.Result];
                    let TestGroupScore = document.createElement("div"); TestGroupElement.appendChild(TestGroupScore);
                    TestGroupScore.classList.add("TestGroupLimit");
                    TestGroupScore.innerText = TestGroup.Score + "pts";
                    let TestGroupTime = document.createElement("div"); TestGroupElement.appendChild(TestGroupTime);
                    TestGroupTime.classList.add("TestGroupLimit");
                    TestGroupTime.innerText = TimeToString(TestGroup.TimeSum);
                    let TestGroupNewLine = document.createElement("br"); TestGroupElement.appendChild(TestGroupNewLine);
                    TestGroup.TestCases.sort((a, b) => { return (a.TCID > b.TCID ? 1 : -1); });
                    TestGroup.TestCases.map((TestCase) => {
                        let TestCaseElement = document.createElement("div"); TestGroupElement.appendChild(TestCaseElement);
                        TestCaseElement.classList.add("TestCase");
                        // TestCaseElement.classList.add("JudgeResult" + SubmissionResultShortTexts[TestCase.Result]);
                        TestCaseElement.style.backgroundColor = SubmissionResultColors[TestCase.Result];
                        if (TestCase.Description != "") {
                            TestCaseElement.classList.add("WithDescription");
                        }
                        {
                            let TestCaseInnerBox1 = document.createElement("div"); TestCaseElement.appendChild(TestCaseInnerBox1);
                            TestCaseInnerBox1.classList.add("TestCaseInnerBox");
                            {
                                let TestCaseNumber = document.createElement("div"); TestCaseInnerBox1.appendChild(TestCaseNumber);
                                TestCaseNumber.classList.add("TestCaseNumber");
                                TestCaseNumber.innerText = TestCaseCounter;
                                let TestCaseResult = document.createElement("div"); TestCaseInnerBox1.appendChild(TestCaseResult);
                                TestCaseResult.classList.add("TestCaseResult");
                                TestCaseResult.innerText = SubmissionResultShortTexts[TestCase.Result];
                                let TestCaseTimeLimit = document.createElement("div"); TestCaseInnerBox1.appendChild(TestCaseTimeLimit);
                                TestCaseTimeLimit.classList.add("TestCaseLimit");
                                {
                                    let TestCaseTimeValue = document.createElement("span"); TestCaseTimeLimit.appendChild(TestCaseTimeValue);
                                    TestCaseTimeValue.innerText = TimeToString(TestCase.Time);
                                    let TestCaseTimeLimitValue = document.createElement("span"); TestCaseTimeLimit.appendChild(TestCaseTimeLimitValue);
                                    TestCaseTimeLimitValue.innerText = TimeToString(Response.TestGroupsLimits[TestGroup.TGID].TestCasesLimits[TestCase.TCID].TimeLimit);
                                }
                                let TestCaseMemoryLimit = document.createElement("div"); TestCaseInnerBox1.appendChild(TestCaseMemoryLimit);
                                TestCaseMemoryLimit.classList.add("TestCaseLimit");
                                {
                                    let TestCaseMemoryValue = document.createElement("span"); TestCaseMemoryLimit.appendChild(TestCaseMemoryValue);
                                    TestCaseMemoryValue.innerText = MemoryToString(TestCase.Memory);
                                    let TestCaseMemoryLimitValue = document.createElement("span"); TestCaseMemoryLimit.appendChild(TestCaseMemoryLimitValue);
                                    TestCaseMemoryLimitValue.innerText = MemoryToString(Response.TestGroupsLimits[TestGroup.TGID].TestCasesLimits[TestCase.TCID].MemoryLimit);
                                }
                            }
                            let TestCaseInnerBox2 = document.createElement("div"); TestCaseElement.appendChild(TestCaseInnerBox2);
                            TestCaseInnerBox2.classList.add("TestCaseInnerBox");
                            {
                                let TestCaseDescription = document.createElement("div"); TestCaseInnerBox2.appendChild(TestCaseDescription);
                                TestCaseDescription.classList.add("TestCaseDescription");
                                {
                                    let TestCaseDescriptionValue = document.createElement("pre"); TestCaseDescription.appendChild(TestCaseDescriptionValue);
                                    TestCaseDescriptionValue.innerText = TestCase.Description;
                                }
                            }
                        }
                        TestCaseCounter++;
                    });
                }
            });
            let SubmissionDescription = document.createElement("div"); SubmissionData.appendChild(SubmissionDescription);
            SubmissionDescription.classList.add("SubmissionDescription");
            {
                let SubmissionDescriptionValue = document.createElement("pre"); SubmissionDescription.appendChild(SubmissionDescriptionValue);
                SubmissionDescriptionValue.innerText = Response.Description;
            }
        }
        if (Response.Code != null && SubmissionCode.value == "") {
            SubmissionCode.value = Response.Code;
            CodeMirror.fromTextArea(SubmissionCode, {
                lineNumbers: true,
                foldGutter: true,
                readOnly: true,
                matchBrackets: true,
                mode: "text/x-c++src",
                extraKeys: {
                    "Ctrl-Space": "autocomplete",
                    "Ctrl-Enter": () => {
                        SubmitButton.click();
                    }
                },
                gutters: ["CodeMirror-linenumbers", "CodeMirror-foldgutter"],
            });
        }
        if (Response.Result <= 10) {
            SubmissionSpinnerCollapse.hide();
        }
        else {
            setTimeout(() => {
                ReloadData();
            }, 1000);
        }
    }, () => { }, () => { })
};
ReloadData();
SubmissionSpinnerCollapse.hide();

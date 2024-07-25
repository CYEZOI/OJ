const SubmissionsProblemResult = document.getElementById("SubmissionsProblemResult");
const SubmissionsUserResult = document.getElementById("SubmissionsUserResult");
const SubmissionsSelectResult = document.getElementById("SubmissionsSelectResult");
const SubmissionsSelectButton = document.getElementById("SubmissionsSelectButton");
const SubmissionsTable = document.getElementById("SubmissionsTable");
const SubmissionsPagination = document.getElementById("SubmissionsPagination");
CheckTokenAvailable();
Data.Page = Data.Page || 1;

SubmissionsProblemResult.value = Data.Problem;
SubmissionsUserResult.value = Data.User;
CreateResultSelect(SubmissionsSelectResult, Data.Result);
SubmissionsSelectButton.onclick = () => {
    SwitchPage("Submissions", {
        "Page": Data.Page,
        "Problem": SubmissionsProblemResult.value,
        "User": SubmissionsUserResult.value,
        "Result": SubmissionsSelectResult.selectedIndex,
    })
};

for (let i = 0; i < 10; i++) {
    let Row = document.createElement("tr"); SubmissionsTable.children[1].appendChild(Row);
    {
        for (let j = 0; j < 8; j++) {
            let Column = document.createElement("td"); Row.appendChild(Column);
            Column.appendChild(CreatePlaceHolder());
        }
    }
}

SubmissionsPagination.children[2].children[0].innerText = Data.Page;
RequestAPI("GetSubmissions", {
    "Page": Number(Data.Page),
    "Problem": Number(Data.Problem || -1),
    "User": Number(Data.User || -1),
    "Result": Number(Data.Result || -1),
}, () => { }, (Response) => {
    SubmissionsPagination.children[0].children[0].setAttribute("data-page-number", 1);
    SubmissionsPagination.children[1].children[0].setAttribute("data-page-number", Math.max(Data.Page - 1, 1));
    SubmissionsPagination.children[2].children[0].setAttribute("data-page-number", Data.Page);
    SubmissionsPagination.children[3].children[0].setAttribute("data-page-number", Math.min(Data.Page + 1, Response.PageCount));
    SubmissionsPagination.children[4].children[0].setAttribute("data-page-number", Response.PageCount);
    for (let i = 0; i < 5; i++) {
        if (SubmissionsPagination.children[i].children[0].getAttribute("data-page-number") ==
            SubmissionsPagination.children[2].children[0].getAttribute("data-page-number")) {
            SubmissionsPagination.children[i].children[0].classList.add("disabled");
        }
        SubmissionsPagination.children[i].children[0].onclick = () => {
            SwitchPage("Submissions", {
                "Page": SubmissionsPagination.children[i].children[0].getAttribute("data-page-number")
            });
        };
    }

    SubmissionsTable.children[1].innerHTML = "";
    Response.Submissions.sort((a, b) => { return a.PID < b.PID; });
    for (let i = 0; i < Response.Submissions.length; i++) {
        let SubmissionRow = document.createElement("tr"); SubmissionsTable.children[1].appendChild(SubmissionRow);
        {
            let SubmissionID = document.createElement("td"); SubmissionRow.appendChild(SubmissionID);
            SubmissionID.innerText = Response.Submissions[i].SID;
            let ProblemID = document.createElement("td"); SubmissionRow.appendChild(ProblemID);
            ProblemID.innerText = Response.Submissions[i].PID;
            ProblemID.role = "button";
            ProblemID.onclick = () => {
                SwitchPage("Problem", {
                    "PID": Response.Submissions[i].PID
                });
            };
            let UserID = document.createElement("td"); SubmissionRow.appendChild(UserID);
            UserID.innerText = Response.Submissions[i].UID;
            UserID.role = "button";
            UserID.onclick = () => {
                SwitchPage("User", {
                    "UID": Response.Submissions[i].UID
                });
            };
            let SubmissionResult = document.createElement("td"); SubmissionRow.appendChild(SubmissionResult);
            let SubmissionResultButton = document.createElement("button"); SubmissionResult.appendChild(SubmissionResultButton);
            SubmissionResultButton.classList.add("btn");
            SubmissionResultButton.innerText = SubmissionResultShortTexts[Response.Submissions[i].Result];
            SubmissionResultButton.style.color = "white";
            SubmissionResultButton.style.backgroundColor = SubmissionResultColors[Response.Submissions[i].Result];
            SubmissionResultButton.onclick = () => {
                SwitchPage("Submission", {
                    "SID": Response.Submissions[i].SID
                });
            };
            let SubmissionTime = document.createElement("td"); SubmissionRow.appendChild(SubmissionTime);
            SubmissionTime.innerText = TimeToString(Response.Submissions[i].Time);
            let SubmissionMemory = document.createElement("td"); SubmissionRow.appendChild(SubmissionMemory);
            SubmissionMemory.innerText = MemoryToString(Response.Submissions[i].Memory);
            let SubmissionCreatedTime = document.createElement("td"); SubmissionRow.appendChild(SubmissionCreatedTime);
            SubmissionCreatedTime.innerText = Response.Submissions[i].CreateTime;
            let SubmissionOperation = document.createElement("td"); SubmissionRow.appendChild(SubmissionOperation);
            {
                let SubmissionOperationButtons = document.createElement("div"); SubmissionOperation.appendChild(SubmissionOperationButtons);
                SubmissionOperationButtons.role = "group";
                {
                    let SubmissionResubmitButton = document.createElement("button"); SubmissionOperationButtons.appendChild(SubmissionResubmitButton);
                    SubmissionResubmitButton.type = "button";
                    SubmissionResubmitButton.classList.add("btn", "btn-secondary", "me-2");
                    SubmissionResubmitButton.innerText = "Resubmit";
                    SubmissionResubmitButton.onclick = () => {
                        SwitchPage("Submit", {
                            "PID": Response.Submissions[i].PID
                        });
                    };
                    let SubmissionRejudgeButton = document.createElement("button"); SubmissionOperationButtons.appendChild(SubmissionRejudgeButton);
                    SubmissionRejudgeButton.type = "button";
                    SubmissionRejudgeButton.classList.add("btn", "btn-warning", "me-2", "AdminOnly");
                    SubmissionRejudgeButton.innerText = "Rejudge";
                    SubmissionRejudgeButton.onclick = () => {
                        ShowModal("Rejudge submission", "Are you sure to rejudge it?", () => {
                            if (Response.Result > 10) {
                                ShowModal("Rejudge submission", "This submission is still running, are you sure to rejudge it? This may cause the server to crash!", () => {
                                    RequestAPI("RejudgeSubmission", {
                                        "SID": Number(Response.Submissions[i].SID)
                                    }, () => { }, () => {
                                        SwitchPage("Submission", {
                                            "SID": Response.Submissions[i].SID
                                        });
                                    }, () => { });
                                }, () => { });
                            }
                            else {
                                RequestAPI("RejudgeSubmission", {
                                    "SID": Number(Response.Submissions[i].SID)
                                }, () => { }, () => {
                                    SwitchPage("Submission", {
                                        "SID": Response.Submissions[i].SID
                                    });
                                }, () => { });
                            }
                        }, () => { });
                    };
                    let SubmissionEditButton = document.createElement("button"); SubmissionOperationButtons.appendChild(SubmissionEditButton);
                    SubmissionEditButton.type = "button";
                    SubmissionEditButton.classList.add("btn", "btn-warning", "me-2", "AdminOnly");
                    SubmissionEditButton.innerText = "Edit";
                    SubmissionEditButton.onclick = () => {
                        SwitchPage("EditSubmission", {
                            "SID": Response.Submissions[i].SID
                        });
                    };
                    let SubmissionDeleteButton = document.createElement("button"); SubmissionOperationButtons.appendChild(SubmissionDeleteButton);
                    SubmissionDeleteButton.type = "button";
                    SubmissionDeleteButton.classList.add("btn", "btn-danger", "me-2", "AdminOnly");
                    SubmissionDeleteButton.innerText = "Delete";
                    SubmissionDeleteButton.onclick = () => {
                        ShowModal("Delete submission", "Are you sure to delete this submission?", () => {
                            RequestAPI("DeleteSubmission", {
                                "SID": Number(Response.Submissions[i].SID)
                            }, () => { }, () => {
                                SubmissionRow.remove();
                            }, () => { });
                        });
                    };
                }
            }
        }
    }
}, () => { }, () => { });

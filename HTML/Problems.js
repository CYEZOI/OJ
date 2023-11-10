const ProblemsAddProblemButton = document.getElementById("ProblemsAddProblemButton");
const ProblemsTable = document.getElementById("ProblemsTable");
const ProblemsPagination = document.getElementById("ProblemsPagination");

CheckTokenAvailable();
if (Data.Page == null) {
    Data.Page = 1;
}
for (let i = 0; i < 10; i++) {
    let Row = document.createElement("tr"); ProblemsTable.children[1].appendChild(Row);
    {
        for (let j = 0; j < 3; j++) {
            let Column = document.createElement("td"); Row.appendChild(Column);
            Column.appendChild(CreatePlaceHolder());
        }
    }
}
ProblemsPagination.children[2].children[0].innerText = Data.Page;
ProblemsAddProblemButton.onclick = () => {
    SwitchPage("UploadProblem", {});
};
RequestAPI("GetProblems", {
    "Page": Number(Data.Page)
}, () => { }, (Response) => {
    ProblemsPagination.children[0].children[0].setAttribute("data-page-number", 1);
    ProblemsPagination.children[1].children[0].setAttribute("data-page-number", Math.max(Data.Page - 1, 1));
    ProblemsPagination.children[2].children[0].setAttribute("data-page-number", Data.Page);
    ProblemsPagination.children[3].children[0].setAttribute("data-page-number", Math.min(Data.Page + 1, Response.PageCount));
    ProblemsPagination.children[4].children[0].setAttribute("data-page-number", Response.PageCount);
    for (let i = 0; i < 5; i++) {
        if (ProblemsPagination.children[i].children[0].getAttribute("data-page-number") ==
            ProblemsPagination.children[2].children[0].getAttribute("data-page-number")) {
            ProblemsPagination.children[i].children[0].classList.add("disabled");
        }
        ProblemsPagination.children[i].children[0].onclick = () => {
            SwitchPage("Problems", {
                "Page": ProblemsPagination.children[i].children[0].getAttribute("data-page-number")
            });
        };
    }

    ProblemsTable.children[1].innerHTML = "";
    Response.Problems.sort((a, b) => { return a.PID < b.PID; });
    for (let i = 0; i < Response.Problems.length; i++) {
        let ProblemRow = document.createElement("tr"); ProblemsTable.children[1].appendChild(ProblemRow);
        {
            let ProblemIDColumn = document.createElement("td"); ProblemRow.appendChild(ProblemIDColumn);
            ProblemIDColumn.innerText = Response.Problems[i].PID;
            ProblemIDColumn.role = "button";
            ProblemIDColumn.onclick = () => {
                SwitchPage("Problem", {
                    "PID": Response.Problems[i].PID
                });
            };
            let ProblemTitleColumn = document.createElement("td"); ProblemRow.appendChild(ProblemTitleColumn);
            ProblemTitleColumn.innerText = Response.Problems[i].Title;
            ProblemTitleColumn.role = "button";
            ProblemTitleColumn.onclick = () => {
                SwitchPage("Problem", {
                    "PID": Response.Problems[i].PID
                });
            };
            let ProblemOperationColumn = document.createElement("td"); ProblemRow.appendChild(ProblemOperationColumn);
            {
                let ProblemOperationButtons = document.createElement("div"); ProblemOperationColumn.appendChild(ProblemOperationButtons);
                ProblemOperationButtons.role = "group";
                {
                    let ProblemOperationSubmitButton = document.createElement("button"); ProblemOperationButtons.appendChild(ProblemOperationSubmitButton);
                    ProblemOperationSubmitButton.classList.add("btn", "btn-primary", "me-2");
                    ProblemOperationSubmitButton.innerText = "Submit";
                    ProblemOperationSubmitButton.onclick = () => {
                        SwitchPage("Submit", {
                            "PID": Response.Problems[i].PID
                        });
                    }
                    let ProblemOperationSubmissionsButton = document.createElement("button"); ProblemOperationButtons.appendChild(ProblemOperationSubmissionsButton);
                    ProblemOperationSubmissionsButton.classList.add("btn", "btn-secondary", "me-2");
                    ProblemOperationSubmissionsButton.innerText = "Submissions";
                    ProblemOperationSubmissionsButton.onclick = () => {
                        SwitchPage("Submissions", {
                            "Filter": [
                                { "PID": [Response.Problems[i].PID] }
                            ]
                        });
                    }
                    let ProblemOperationEditButton = document.createElement("button"); ProblemOperationButtons.appendChild(ProblemOperationEditButton);
                    ProblemOperationEditButton.classList.add("btn", "btn-warning", "me-2", "AdminOnly");
                    ProblemOperationEditButton.innerText = "Edit";
                    ProblemOperationEditButton.onclick = () => {
                        SwitchPage("EditProblem", {
                            "PID": Response.Problems[i].PID
                        });
                    }
                    let ProblemOperationDeleteButton = document.createElement("button"); ProblemOperationButtons.appendChild(ProblemOperationDeleteButton);
                    ProblemOperationDeleteButton.classList.add("btn", "btn-danger", "me-2", "AdminOnly");
                    ProblemOperationDeleteButton.innerText = "Delete";
                    ProblemOperationDeleteButton.onclick = () => {
                        ShowModal("Delete problem", "Are you sure to delete this problem?", () => {
                            RequestAPI("DeleteProblem",
                                {
                                    "PID": String(Response.Problems[i].PID)
                                }, () => { }, (Response) => {
                                    ShowSuccess("Delete Problem Success");
                                    ProblemRow.remove();
                                }, () => { }, () => { });
                        }, () => { });
                    }
                }
            }
        }
    }
}, () => { }, () => { });

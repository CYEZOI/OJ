CheckTokenAvailable();

const CreateResultSelect = (Element, Value) => {
    Element.className = "form-control";
    for (let i = 0; i < SubmissionResultShortTexts.length; i++) {
        let Option = document.createElement("option"); Element.appendChild(Option);
        Option.innerText = SubmissionResultTexts[i];
        Option.style.color = "white";
        Option.style.backgroundColor = SubmissionResultColors[i];
    }
    Element.selectedIndex = Value;
    Element.onchange = () => {
        let Result = Element.selectedIndex;
        Element.style.color = "white";
        Element.style.backgroundColor = SubmissionResultColors[Result];
    }
    Element.onchange();
}

if (Data.SID == null) {
    SwitchPage("Home");
}
for (let i = 0; i < 10; i++) {
    let Row = document.createElement("tr"); SubmissionTestCasesTable.children[1].appendChild(Row);
    {
        for (let j = 0; j < 7; j++) {
            let Column = document.createElement("td"); Row.appendChild(Column);
            Column.appendChild(CreatePlaceHolder());
        }
    }
}

let TitleElement = document.getElementsByTagName("h4")[0];
TitleElement.innerText += " " + Data.SID;

RequestAPI("GetSubmission", {
    "SID": Number(Data.SID)
}, () => { }, (Response) => {
    if (!Response.IsAdmin) {
        SwitchPage("Home");
    }
    EditSubmissionEnableO2.checked = Response.EnableO2;
    EditSubmissionPID.value = Response.PID;
    EditSubmissionUID.value = Response.UID;
    CreateResultSelect(EditSubmissionResult, Response.Result);
    EditSubmissionDescription.value = Response.Description;
    EditSubmissionTime.value = Response.Time;
    EditSubmissionTimeSum.value = Response.TimeSum;
    EditSubmissionMemory.value = Response.Memory;
    EditSubmissionScore.value = Response.Score;
    EditSubmissionCode.value = Response.Code;
    let EditSubmissionCodeEditor = CodeMirror.fromTextArea(EditSubmissionCode, {
        lineNumbers: true,
        foldGutter: true,
        matchBrackets: true,
        mode: "text/x-c++src",
        extraKeys: {
            "Ctrl-Space": "autocomplete"
        },
        gutters: ["CodeMirror-linenumbers", "CodeMirror-foldgutter"]
    });
    EditSubmissionCodeEditor.getWrapperElement().classList.add("col-sm");
    let SubmissionTestGroupsTableBody = SubmissionTestGroupsTable.children[1];
    let SubmissionTestCasesTableBody = SubmissionTestCasesTable.children[1];
    SubmissionTestGroupsTableBody.innerHTML = "";
    SubmissionTestCasesTableBody.innerHTML = "";
    Response.TestGroups = JSON.parse(Response.TestGroups);
    Response.TestGroups.sort((a, b) => {
        return (a.TGID > b.TGID ? 1 : -1);
    });
    for (let i = 0; i < Response.TestGroups.length; i++) {
        let SubmissionTestGroupsTableBodyRow = document.createElement("tr"); SubmissionTestGroupsTableBody.appendChild(SubmissionTestGroupsTableBodyRow);
        {
            let TGID = document.createElement("td"); SubmissionTestGroupsTableBodyRow.appendChild(TGID);
            TGID.innerText = String(i + 1);
            let Result = document.createElement("td"); SubmissionTestGroupsTableBodyRow.appendChild(Result);
            {
                let ResultSelect = document.createElement("select"); Result.appendChild(ResultSelect);
                CreateResultSelect(ResultSelect, Response.TestGroups[i].Result);
            }
            let Time = document.createElement("td"); SubmissionTestGroupsTableBodyRow.appendChild(Time);
            {
                let TimeInputGroup = document.createElement("div"); Time.appendChild(TimeInputGroup);
                TimeInputGroup.className = "input-group";
                {
                    let TimeInputGroupInput = document.createElement("input"); TimeInputGroup.appendChild(TimeInputGroupInput);
                    TimeInputGroupInput.className = "form-control";
                    TimeInputGroupInput.type = "number";
                    TimeInputGroupInput.value = Response.TestGroups[i].Time;
                    let TimeInputGroupUnit = document.createElement("span"); TimeInputGroup.appendChild(TimeInputGroupUnit);
                    TimeInputGroupUnit.className = "input-group-text";
                    TimeInputGroupUnit.innerText = "ms";
                }
            }
            let TimeSum = document.createElement("td"); SubmissionTestGroupsTableBodyRow.appendChild(TimeSum);
            {
                let TimeSumInputGroup = document.createElement("div"); TimeSum.appendChild(TimeSumInputGroup);
                TimeSumInputGroup.className = "input-group";
                {
                    let TimeSumInputGroupInput = document.createElement("input"); TimeSumInputGroup.appendChild(TimeSumInputGroupInput);
                    TimeSumInputGroupInput.className = "form-control";
                    TimeSumInputGroupInput.type = "number";
                    TimeSumInputGroupInput.value = Response.TestGroups[i].TimeSum;
                    let TimeSumInputGroupUnit = document.createElement("span"); TimeSumInputGroup.appendChild(TimeSumInputGroupUnit);
                    TimeSumInputGroupUnit.className = "input-group-text";
                    TimeSumInputGroupUnit.innerText = "ms";
                }
            }
            let Memory = document.createElement("td"); SubmissionTestGroupsTableBodyRow.appendChild(Memory);
            {
                let MemoryInputGroup = document.createElement("div"); Memory.appendChild(MemoryInputGroup);
                MemoryInputGroup.className = "input-group";
                {
                    let MemoryInputGroupInput = document.createElement("input"); MemoryInputGroup.appendChild(MemoryInputGroupInput);
                    MemoryInputGroupInput.className = "form-control";
                    MemoryInputGroupInput.type = "number";
                    MemoryInputGroupInput.value = Math.round(Response.TestGroups[i].Memory / 1024);
                    let MemoryInputGroupUnit = document.createElement("span"); MemoryInputGroup.appendChild(MemoryInputGroupUnit);
                    MemoryInputGroupUnit.className = "input-group-text";
                    MemoryInputGroupUnit.innerText = "KB";
                }
            }
            let Score = document.createElement("td"); SubmissionTestGroupsTableBodyRow.appendChild(Score);
            {
                let ScoreInputGroup = document.createElement("div"); Score.appendChild(ScoreInputGroup);
                ScoreInputGroup.className = "input-group";
                {
                    let ScoreInputGroupInput = document.createElement("input"); ScoreInputGroup.appendChild(ScoreInputGroupInput);
                    ScoreInputGroupInput.className = "form-control";
                    ScoreInputGroupInput.type = "number";
                    ScoreInputGroupInput.value = Response.TestGroups[i].Score;
                    let ScoreInputGroupUnit = document.createElement("span"); ScoreInputGroup.appendChild(ScoreInputGroupUnit);
                    ScoreInputGroupUnit.className = "input-group-text";
                    ScoreInputGroupUnit.innerText = "pts";
                }
            }
        }
        for (let j = 0; j < Response.TestGroups[i].TestCases.length; j++) {
            let SubmissionTestCasesTableBodyRow = document.createElement("tr"); SubmissionTestCasesTableBody.appendChild(SubmissionTestCasesTableBodyRow);
            {
                let TGID = document.createElement("td"); SubmissionTestCasesTableBodyRow.appendChild(TGID);
                TGID.innerText = String(i + 1);
                let TCID = document.createElement("td"); SubmissionTestCasesTableBodyRow.appendChild(TCID);
                TCID.innerText = String(j + 1);
                let Result = document.createElement("td"); SubmissionTestCasesTableBodyRow.appendChild(Result);
                {
                    let ResultSelect = document.createElement("select"); Result.appendChild(ResultSelect);
                    ResultSelect.className = "form-control";
                    CreateResultSelect(ResultSelect, Response.TestGroups[i].TestCases[j].Result);
                }
                let Time = document.createElement("td"); SubmissionTestCasesTableBodyRow.appendChild(Time);
                {
                    let TimeInputGroup = document.createElement("div"); Time.appendChild(TimeInputGroup);
                    TimeInputGroup.className = "input-group";
                    {
                        let TimeInputGroupInput = document.createElement("input"); TimeInputGroup.appendChild(TimeInputGroupInput);
                        TimeInputGroupInput.className = "form-control";
                        TimeInputGroupInput.type = "number";
                        TimeInputGroupInput.value = Response.TestGroups[i].TestCases[j].Time;
                        let TimeInputGroupUnit = document.createElement("span"); TimeInputGroup.appendChild(TimeInputGroupUnit);
                        TimeInputGroupUnit.className = "input-group-text";
                        TimeInputGroupUnit.innerText = "ms";
                    }
                }
                let Memory = document.createElement("td"); SubmissionTestCasesTableBodyRow.appendChild(Memory);
                {
                    let MemoryInputGroup = document.createElement("div"); Memory.appendChild(MemoryInputGroup);
                    MemoryInputGroup.className = "input-group";
                    {
                        let MemoryInputGroupInput = document.createElement("input"); MemoryInputGroup.appendChild(MemoryInputGroupInput);
                        MemoryInputGroupInput.className = "form-control";
                        MemoryInputGroupInput.type = "number";
                        MemoryInputGroupInput.value = Math.round(Response.TestGroups[i].TestCases[j].Memory / 1024);
                        let MemoryInputGroupUnit = document.createElement("span"); MemoryInputGroup.appendChild(MemoryInputGroupUnit);
                        MemoryInputGroupUnit.className = "input-group-text";
                        MemoryInputGroupUnit.innerText = "KB";
                    }
                }
                let Score = document.createElement("td"); SubmissionTestCasesTableBodyRow.appendChild(Score);
                {
                    let ScoreInputGroup = document.createElement("div"); Score.appendChild(ScoreInputGroup);
                    ScoreInputGroup.className = "input-group";
                    {
                        let ScoreInputGroupInput = document.createElement("input"); ScoreInputGroup.appendChild(ScoreInputGroupInput);
                        ScoreInputGroupInput.className = "form-control";
                        ScoreInputGroupInput.type = "number";
                        ScoreInputGroupInput.value = Response.TestGroups[i].TestCases[j].Score;
                        let ScoreInputGroupUnit = document.createElement("span"); ScoreInputGroup.appendChild(ScoreInputGroupUnit);
                        ScoreInputGroupUnit.className = "input-group-text";
                        ScoreInputGroupUnit.innerText = "pts";
                    }
                }
                let Description = document.createElement("td"); SubmissionTestCasesTableBodyRow.appendChild(Description);
                {
                    let DescriptionInput = document.createElement("input"); Description.appendChild(DescriptionInput);
                    DescriptionInput.className = "form-control";
                    DescriptionInput.type = "text";
                    DescriptionInput.value = Response.TestGroups[i].TestCases[j].Description;
                }
            }
        }
    }

    EditSubmissionSaveButton.onclick = () => {
        let SubmitTestGroupsData = [];
        let TestCaseCount = 0;
        for (let i = 0; i < Response.TestGroups.length; i++) {
            let CurrentRow = SubmissionTestGroupsTable.children[1].children[i];
            SubmitTestGroupsData.push({
                "TGID": Response.TestGroups[i].TGID,
                "Result": Number(CurrentRow.children[1].children[0].selectedIndex),
                "Time": Number(CurrentRow.children[2].children[0].children[0].value),
                "TimeSum": Number(CurrentRow.children[3].children[0].children[0].value),
                "Memory": Number(CurrentRow.children[4].children[0].children[0].value * 1024),
                "Score": Number(CurrentRow.children[5].children[0].children[0].value),
                "TestCases": []
            });
            for (let j = 0; j < Response.TestGroups[i].TestCases.length; j++) {
                let CurrentRow = SubmissionTestCasesTable.children[1].children[TestCaseCount];
                TestCaseCount++;
                SubmitTestGroupsData[i].TestCases.push({
                    "TCID": Response.TestGroups[i].TestCases[j].TCID,
                    "Result": Number(CurrentRow.children[2].children[0].selectedIndex),
                    "Time": Number(CurrentRow.children[3].children[0].children[0].value),
                    "Memory": Number(CurrentRow.children[4].children[0].children[0].value * 1024),
                    "Score": Number(CurrentRow.children[5].children[0].children[0].value),
                    "Description": String(CurrentRow.children[6].children[0].value),
                    "Output": String(Response.TestGroups[i].TestCases[j].Output),
                    "StandardOutput": String(Response.TestGroups[i].TestCases[j].StandardOutput),
                    "StandardError": String(Response.TestGroups[i].TestCases[j].StandardError)
                });
            }
        }

        RequestAPI("UpdateSubmission", {
            "SID": Number(Data.SID),
            "PID": String(EditSubmissionPID.value),
            "UID": Number(EditSubmissionUID.value),
            "Code": String(EditSubmissionCodeEditor.getValue()),
            "Result": Number(EditSubmissionResult.selectedIndex),
            "Description": String(EditSubmissionDescription.value),
            "Time": Number(EditSubmissionTime.value),
            "TimeSum": Number(EditSubmissionTimeSum.value),
            "Memory": Number(EditSubmissionMemory.value),
            "Score": Number(EditSubmissionScore.value),
            "EnableO2": Boolean(EditSubmissionEnableO2.checked),
            "TestGroups": JSON.stringify(SubmitTestGroupsData)
        }, () => { }, () => {
            ShowSuccess("Update Submission Success");
            setTimeout(() => {
                SwitchPage("Submission", {
                    "SID": Data.SID
                });
            }, 1000);
        }, () => { }, () => { });
    };
}, () => { }, () => { });

const EditProblemData = document.getElementById("EditProblemData");
const EditProblemSaveButton = document.getElementById("EditProblemSaveButton");
CheckTokenAvailable();
let SamplesData = [];
let TestCasesData = [];

const CreateSampleRow = (SamplesTableBody, Index) => {
    if (Index >= SamplesData.length) {
        SamplesData.push({
            "Input": "",
            "Output": "",
            "Description": "",
            "InputEditor": null,
            "OutputEditor": null,
            "DescriptionEditor": null
        });
    }
    let Sample = SamplesData[Index];
    let SamplesTableBodyRow = document.createElement("tr"); SamplesTableBody.appendChild(SamplesTableBodyRow);
    {
        let SamplesTableBodyRowTitle = document.createElement("td"); SamplesTableBodyRow.appendChild(SamplesTableBodyRowTitle);
        SamplesTableBodyRowTitle.innerHTML = "Sample " + (Index + 1);
        let SamplesTableBodyRowInput = document.createElement("td"); SamplesTableBodyRow.appendChild(SamplesTableBodyRowInput);
        {
            let InputTextArea = document.createElement("textarea"); SamplesTableBodyRowInput.appendChild(InputTextArea);
            InputTextArea.innerText = Sample.Input;
            Sample.InputEditor = CreateCodeMirrorTextEditor(InputTextArea);
            Sample.InputEditor.setSize("100%", "auto");
        }
        let SamplesTableBodyRowOutput = document.createElement("td"); SamplesTableBodyRow.appendChild(SamplesTableBodyRowOutput);
        {
            let OutputTextArea = document.createElement("textarea"); SamplesTableBodyRowOutput.appendChild(OutputTextArea);
            OutputTextArea.innerText = Sample.Output;
            Sample.OutputEditor = CreateCodeMirrorTextEditor(OutputTextArea);
            Sample.OutputEditor.setSize("100%", "auto");
        }
        let SamplesTableBodyRowDescription = document.createElement("td"); SamplesTableBodyRow.appendChild(SamplesTableBodyRowDescription);
        {
            let DescriptionTextArea = document.createElement("textarea"); SamplesTableBodyRowDescription.appendChild(DescriptionTextArea);
            DescriptionTextArea.classList.add("form-control");
            DescriptionTextArea.innerText = Sample.Description;
            Sample.DescriptionEditor = CreateVditorEditor(SamplesTableBodyRowDescription, Sample.Description);
        }
        let SamplesTableBodyRowOperation = document.createElement("td"); SamplesTableBodyRow.appendChild(SamplesTableBodyRowOperation);
        {
            let DeleteButton = document.createElement("button"); SamplesTableBodyRowOperation.appendChild(DeleteButton);
            DeleteButton.classList.add("btn");
            DeleteButton.classList.add("btn-danger");
            DeleteButton.innerHTML = "Delete";
            DeleteButton.onclick = () => {
                ShowModal("Delete sample", "Are you sure to delete this sample?", () => {
                    SamplesTableBody.removeChild(SamplesTableBodyRow);
                    SamplesData.splice(Index, 1);
                    SamplesTableBody.childNodes.forEach((Element, Index) => {
                        Element.childNodes[0].innerHTML = "Sample " + (Index + 1);
                    });
                }, () => { });
            };
        }
    }
}
const CreateTestCaseRow = (TestCasesTableBody, Index) => {
    if (Index >= TestCasesData.length) {
        TestCasesData.push({
            "TGID": 0,
            "Input": "",
            "Answer": "",
            "TimeLimit": 1000,
            "MemoryLimit": 256 * 1024 * 1024,
            "Score": 100,
            "InputEditor": null,
            "AnswerEditor": null
        });
    }
    let TestCase = TestCasesData[Index];
    let TestCasesTableBodyRow = document.createElement("tr"); TestCasesTableBody.appendChild(TestCasesTableBodyRow);
    {
        let TestCasesTableBodyRowTitle = document.createElement("td"); TestCasesTableBodyRow.appendChild(TestCasesTableBodyRowTitle);
        TestCasesTableBodyRowTitle.innerHTML = "Test Case " + (Index + 1);
        let TestCasesTableBodyRowInput = document.createElement("td"); TestCasesTableBodyRow.appendChild(TestCasesTableBodyRowInput);
        {
            let InputTextArea = document.createElement("textarea"); TestCasesTableBodyRowInput.appendChild(InputTextArea);
            InputTextArea.innerText = TestCase.Input;
            TestCase.InputEditor = CreateCodeMirrorTextEditor(InputTextArea);
            TestCase.InputEditor.setSize("100%", "auto");
        }
        let TestCasesTableBodyRowAnswer = document.createElement("td"); TestCasesTableBodyRow.appendChild(TestCasesTableBodyRowAnswer);
        {
            let AnswerTextArea = document.createElement("textarea"); TestCasesTableBodyRowAnswer.appendChild(AnswerTextArea);
            AnswerTextArea.innerText = TestCase.Answer;
            TestCase.AnswerEditor = CreateCodeMirrorTextEditor(AnswerTextArea);
            TestCase.AnswerEditor.setSize("100%", "auto");
        }
        let TestCasesTableBodyRowLimitsAndScore = document.createElement("td"); TestCasesTableBodyRow.appendChild(TestCasesTableBodyRowLimitsAndScore);
        {
            let TimeLimitInputGroup = document.createElement("div"); TestCasesTableBodyRowLimitsAndScore.appendChild(TimeLimitInputGroup);
            TimeLimitInputGroup.classList.add("input-group");
            TimeLimitInputGroup.classList.add("mb-1");
            {
                let TimeLimitInput = document.createElement("input"); TimeLimitInputGroup.appendChild(TimeLimitInput);
                TimeLimitInput.classList.add("form-control");
                TimeLimitInput.type = "number";
                TimeLimitInput.min = 0;
                TimeLimitInput.value = TestCase.TimeLimit;
                let TimeLimitUnit = document.createElement("span"); TimeLimitInputGroup.appendChild(TimeLimitUnit);
                TimeLimitUnit.classList.add("input-group-text");
                TimeLimitUnit.innerHTML = "ms";
                TimeLimitInput.onchange = () => {
                    TestCase.TimeLimit = TimeLimitInput.value;
                }
            }
            let MemoryLimitInputGroup = document.createElement("div"); TestCasesTableBodyRowLimitsAndScore.appendChild(MemoryLimitInputGroup);
            MemoryLimitInputGroup.classList.add("input-group");
            MemoryLimitInputGroup.classList.add("mb-1");
            {
                let MemoryLimitInput = document.createElement("input"); MemoryLimitInputGroup.appendChild(MemoryLimitInput);
                MemoryLimitInput.classList.add("form-control");
                MemoryLimitInput.type = "number";
                MemoryLimitInput.min = 0;
                MemoryLimitInput.value = TestCase.MemoryLimit / 1024 / 1024;
                let MemoryLimitUnit = document.createElement("span"); MemoryLimitInputGroup.appendChild(MemoryLimitUnit);
                MemoryLimitUnit.classList.add("input-group-text");
                MemoryLimitUnit.innerHTML = "MB";
                MemoryLimitInput.onchange = () => {
                    TestCase.MemoryLimit = MemoryLimitInput.value * 1024 * 1024;
                }
            }
            let ScoreInputGroup = document.createElement("div"); TestCasesTableBodyRowLimitsAndScore.appendChild(ScoreInputGroup);
            ScoreInputGroup.classList.add("input-group");
            ScoreInputGroup.classList.add("mb-1");
            {
                let ScoreInput = document.createElement("input"); ScoreInputGroup.appendChild(ScoreInput);
                ScoreInput.classList.add("form-control");
                ScoreInput.type = "number";
                ScoreInput.min = 0;
                ScoreInput.value = TestCase.Score;
                let ScoreUnit = document.createElement("span"); ScoreInputGroup.appendChild(ScoreUnit);
                ScoreUnit.classList.add("input-group-text");
                ScoreUnit.innerHTML = "pts";
                ScoreInput.onchange = () => {
                    TestCase.Score = ScoreInput.value;
                }
            }
        }
        let TestCasesTableBodyRowOperation = document.createElement("td"); TestCasesTableBodyRow.appendChild(TestCasesTableBodyRowOperation);
        {
            let DeleteButton = document.createElement("button"); TestCasesTableBodyRowOperation.appendChild(DeleteButton);
            DeleteButton.classList.add("btn");
            DeleteButton.classList.add("btn-danger");
            DeleteButton.innerHTML = "Delete";
            DeleteButton.onclick = () => {
                ShowModal("Delete test case", "Are you sure to delete this test case?", () => {
                    TestCasesTableBody.removeChild(TestCasesTableBodyRow);
                    TestCasesData.splice(Index, 1);
                    TestCasesTableBody.childNodes.forEach((Element, Index) => {
                        Element.childNodes[0].innerHTML = "Test Case " + (Index + 1);
                    });
                }, () => { });
            };
        }
    }
};

let OldTitle = Data.PID == null ? "Add problem" : "Edit problem";
PageTitle.innerHTML = "";
PageTitle.classList.add("row");
PageTitle.classList.add("align-items-center");
{
    let TitleTextElement = document.createElement("div"); PageTitle.appendChild(TitleTextElement);
    TitleTextElement.classList.add("col-auto");
    TitleTextElement.innerHTML = OldTitle + " ";
    let PIDInputElement = document.createElement("div"); PageTitle.appendChild(PIDInputElement);
    PIDInputElement.classList.add("col-auto");
    {
        let PIDInput = document.createElement("input"); PIDInputElement.appendChild(PIDInput);
        PIDInput.classList.add("form-control");
        PIDInput.id = "PID";
        PIDInput.placeholder = "PID";
        if (Data.PID != null) {
            PIDInput.value = Data.PID;
            PIDInput.readOnly = PIDInput.disabled = true;
        }
    }
    let TitleInputElement = document.createElement("div"); PageTitle.appendChild(TitleInputElement);
    TitleInputElement.classList.add("col-auto");
    {
        let TitleInput = document.createElement("input"); TitleInputElement.appendChild(TitleInput);
        TitleInput.classList.add("form-control");
        TitleInput.id = "Title";
        TitleInput.placeholder = "Title";
    }
}

let IOFilenameInputGroup = document.createElement("div"); EditProblemData.appendChild(IOFilenameInputGroup);
IOFilenameInputGroup.classList.add("input-group");
IOFilenameInputGroup.classList.add("mb-3");
{
    var IOFilenameInput = document.createElement("input"); IOFilenameInputGroup.appendChild(IOFilenameInput);
    IOFilenameInput.classList.add("form-control");
    IOFilenameInput.id = "IOFilename";
    IOFilenameInput.placeholder = "IO Filename";
    let IOFilenameExtension = document.createElement("span"); IOFilenameInputGroup.appendChild(IOFilenameExtension);
    IOFilenameExtension.classList.add("input-group-text");
    IOFilenameExtension.innerHTML = ".in/out";
}
EditProblemData.appendChild(CreateHorizontalLine());

let DescriptionEditor = CreateMarkdownEditor("Description", "", EditProblemData);
let InputEditor = CreateMarkdownEditor("Input", "", EditProblemData);
let OutputEditor = CreateMarkdownEditor("Output", "", EditProblemData);
let RangeEditor = CreateMarkdownEditor("Range", "", EditProblemData);
let HintEditor = CreateMarkdownEditor("Hint", "", EditProblemData);

let SamplesLabel = document.createElement("label"); EditProblemData.appendChild(SamplesLabel);
SamplesLabel.classList.add("col-form-label")
SamplesLabel.innerHTML = "Samples";
let SamplesTable = document.createElement("table"); EditProblemData.appendChild(SamplesTable);
SamplesTable.classList.add("table");
{
    let SamplesTableHead = document.createElement("thead"); SamplesTable.appendChild(SamplesTableHead);
    {
        let SamplesTableHeadRow = document.createElement("tr"); SamplesTableHead.appendChild(SamplesTableHeadRow);
        {
            let SamplesTableHeadRowTitle = document.createElement("th"); SamplesTableHeadRow.appendChild(SamplesTableHeadRowTitle);
            SamplesTableHeadRowTitle.innerHTML = "Sample";
            SamplesTableHeadRowTitle.classList.add("col-2");
            let SamplesTableHeadRowInput = document.createElement("th"); SamplesTableHeadRow.appendChild(SamplesTableHeadRowInput);
            SamplesTableHeadRowInput.innerHTML = "Input";
            SamplesTableHeadRowInput.classList.add("col-2");
            let SamplesTableHeadRowOutput = document.createElement("th"); SamplesTableHeadRow.appendChild(SamplesTableHeadRowOutput);
            SamplesTableHeadRowOutput.innerHTML = "Output";
            SamplesTableHeadRowOutput.classList.add("col-2");
            let SamplesTableHeadRowDescription = document.createElement("th"); SamplesTableHeadRow.appendChild(SamplesTableHeadRowDescription);
            SamplesTableHeadRowDescription.innerHTML = "Description";
            SamplesTableHeadRow.classList.add("col-4");
            let SamplesTableHeadRowOperation = document.createElement("th"); SamplesTableHeadRow.appendChild(SamplesTableHeadRowOperation);
            SamplesTableHeadRowOperation.innerHTML = "Operation";
            SamplesTableHeadRowOperation.classList.add("col-2");
        }
    }
    var SamplesTableBody = document.createElement("tbody"); SamplesTable.appendChild(SamplesTableBody);
}
let AddSampleButton = document.createElement("button"); EditProblemData.appendChild(AddSampleButton);
AddSampleButton.classList.add("btn");
AddSampleButton.classList.add("btn-secondary");
AddSampleButton.innerHTML = "Add sample";
AddSampleButton.onclick = () => {
    CreateSampleRow(SamplesTable.children[1], SamplesTable.children[1].children.length);
};
EditProblemData.appendChild(CreateHorizontalLine());

let TestCasesLabel = document.createElement("label"); EditProblemData.appendChild(TestCasesLabel);
TestCasesLabel.classList.add("col-form-label")
TestCasesLabel.innerHTML = "Test cases";
let TestCasesTable = document.createElement("table"); EditProblemData.appendChild(TestCasesTable);
TestCasesTable.classList.add("table");
{
    let TestCasesTableHead = document.createElement("thead"); TestCasesTable.appendChild(TestCasesTableHead);
    {
        let TestCasesTableHeadRow = document.createElement("tr"); TestCasesTableHead.appendChild(TestCasesTableHeadRow);
        {
            let TestCasesTableHeadRowTitle = document.createElement("th"); TestCasesTableHeadRow.appendChild(TestCasesTableHeadRowTitle);
            TestCasesTableHeadRowTitle.innerHTML = "Test case";
            TestCasesTableHeadRowTitle.classList.add("col-2");
            let TestCasesTableHeadRowInput = document.createElement("th"); TestCasesTableHeadRow.appendChild(TestCasesTableHeadRowInput);
            TestCasesTableHeadRowInput.innerHTML = "Input";
            TestCasesTableHeadRowInput.classList.add("col-3");
            let TestCasesTableHeadRowAnswer = document.createElement("th"); TestCasesTableHeadRow.appendChild(TestCasesTableHeadRowAnswer);
            TestCasesTableHeadRowAnswer.innerHTML = "Answer";
            TestCasesTableHeadRowAnswer.classList.add("col-3");
            let TestCasesTableHeadRowLimitsAndScore = document.createElement("th"); TestCasesTableHeadRow.appendChild(TestCasesTableHeadRowLimitsAndScore);
            TestCasesTableHeadRowLimitsAndScore.innerHTML = "Limits and Score";
            TestCasesTableHeadRowLimitsAndScore.classList.add("col-2");
            let TestCasesTableHeadRowOperation = document.createElement("th"); TestCasesTableHeadRow.appendChild(TestCasesTableHeadRowOperation);
            TestCasesTableHeadRowOperation.innerHTML = "Operation";
            TestCasesTableHeadRowOperation.classList.add("col-2");
        }
    }
    var TestCasesTableBody = document.createElement("tbody"); TestCasesTable.appendChild(TestCasesTableBody);
}
let AddTestCaseButton = document.createElement("button"); EditProblemData.appendChild(AddTestCaseButton);
AddTestCaseButton.classList.add("btn");
AddTestCaseButton.classList.add("btn-secondary");
AddTestCaseButton.innerHTML = "Add Test Case";
AddTestCaseButton.onclick = () => {
    CreateTestCaseRow(TestCasesTable.children[1], TestCasesTable.children[1].children.length);
};
EditProblemData.appendChild(CreateHorizontalLine());

EditProblemSaveButton.onclick = () => {
    let SubmitSamplesData = [];
    for (let SampleCount = 0; SampleCount < SamplesData.length; SampleCount++) {
        SubmitSamplesData.push({
            "Input": String(SamplesData[SampleCount].InputEditor.getValue()),
            "Output": String(SamplesData[SampleCount].OutputEditor.getValue()),
            "Description": String(SamplesData[SampleCount].DescriptionEditor.getValue())
        });
    }

    let SubmitTestGroupsData = [];
    for (let TestCaseCount = 0; TestCaseCount < TestCasesData.length; TestCaseCount++) {
        while (SubmitTestGroupsData.length <= TestCasesData[TestCaseCount].TGID) {
            SubmitTestGroupsData.push({
                "TGID": Number(SubmitTestGroupsData.length),
                "TestCases": []
            });
        }
        SubmitTestGroupsData[TestCasesData[TestCaseCount].TGID].TestCases.push({
            "TCID": Number(SubmitTestGroupsData[TestCasesData[TestCaseCount].TGID].TestCases.length),
            "Input": String(TestCasesData[TestCaseCount].InputEditor.getValue()),
            "Answer": String(TestCasesData[TestCaseCount].AnswerEditor.getValue()),
            "TimeLimit": Number(TestCasesData[TestCaseCount].TimeLimit),
            "MemoryLimit": Number(TestCasesData[TestCaseCount].MemoryLimit),
            "Score": Number(TestCasesData[TestCaseCount].Score)
        });
    }

    RequestAPI(Data.PID != null ? "UpdateProblem" : "AddProblem", {
        "PID": String(PID.value),
        "Title": String(Title.value),
        "IOFilename": String(IOFilename.value.trim()),
        "Description": String(DescriptionEditor.getValue().trim()),
        "Input": String(InputEditor.getValue().trim()),
        "Output": String(OutputEditor.getValue().trim()),
        "Range": String(RangeEditor.getValue().trim()),
        "Hint": String(HintEditor.getValue().trim()),
        "Samples": JSON.stringify(SubmitSamplesData),
        "TestGroups": JSON.stringify(SubmitTestGroupsData)
    }, () => { }, () => {
        ShowSuccess(Data.PID != null ? "Update" : "Add" + " Problem Success");
        setTimeout(() => {
            SwitchPage("Problem", {
                "PID": PID.value
            });
        }, 1000);
    }, () => { }, () => { });
};

if (Data.PID != null) {
    RequestAPI("GetProblem", {
        "PID": String(Data.PID)
    }, () => { }, (Response) => {
        IOFilenameInput.value = Response.IOFilename;
        Title.value = Response.Title;

        let SampleCount = 0;
        Response.Samples.map((Sample) => {
            SamplesData.push({
                "Input": Sample.Input,
                "Output": Sample.Output,
                "Description": Sample.Description,
                "InputEditor": null,
                "OutputEditor": null,
                "DescriptionEditor": null
            });
            CreateSampleRow(SamplesTableBody, SampleCount);
            SampleCount++;
        });

        let TestCaseCount = 0;
        Response.TestGroups.map((TestGroup) => {
            TestGroup.TestCases.map((TestCase) => {
                TestCasesData.push({
                    "TGID": TestGroup.TGID,
                    "Input": TestCase.Input,
                    "Answer": TestCase.Answer,
                    "TimeLimit": TestCase.TimeLimit,
                    "MemoryLimit": TestCase.MemoryLimit,
                    "Score": TestCase.Score,
                    "InputEditor": null,
                    "AnswerEditor": null
                });
                CreateTestCaseRow(TestCasesTableBody, TestCaseCount);
                TestCaseCount++;
            });
        });

        DescriptionEditor.setValue(Response.Description);
        InputEditor.setValue(Response.Input);
        OutputEditor.setValue(Response.Output);
        RangeEditor.setValue(Response.Range);
        HintEditor.setValue(Response.Hint);
    }, () => { }, () => { });
}

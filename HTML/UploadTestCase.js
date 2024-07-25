const UploadTestCaseButton = document.getElementById("UploadTestCaseButton");
const UploadTestCaseData = document.getElementById("UploadTestCaseData");
const UploadTestCaseSubmitButton = document.getElementById("UploadTestCaseSubmitButton");
CheckTokenAvailable();
if (Data.PID == null) {
    SwitchPage("Home");
}
let TestCasesData = [];

const CreateTestCaseRow = (Index, InputFilename = "", AnswerFilename = "") => {
    if (Index >= TestCasesData.length) {
        TestCasesData.push({
            "TGID": 0,
            "InputFilename": InputFilename,
            "AnswerFilename": AnswerFilename,
            "TimeLimit": 1000,
            "MemoryLimit": 256 * 1024 * 1024,
            "Score": 100,
        });
    }
    let TestCase = TestCasesData[Index];
    let TestCasesTableBodyRow = document.createElement("tr"); UploadTestCaseData.appendChild(TestCasesTableBodyRow);
    {
        let TestCasesTableBodyRowInput = document.createElement("td"); TestCasesTableBodyRow.appendChild(TestCasesTableBodyRowInput);
        {
            let InputFilenameTextArea = document.createElement("input"); TestCasesTableBodyRowInput.appendChild(InputFilenameTextArea);
            InputFilenameTextArea.classList.add("form-control");
            InputFilenameTextArea.placeholder = "Input file name";
            InputFilenameTextArea.value = TestCase.InputFilename;
            InputFilenameTextArea.onchange = () => {
                TestCase.InputFilename = InputFilenameTextArea.value;
            }
        }
        let TestCasesTableBodyRowAnswer = document.createElement("td"); TestCasesTableBodyRow.appendChild(TestCasesTableBodyRowAnswer);
        {
            let AnswerFilenameTextArea = document.createElement("input"); TestCasesTableBodyRowAnswer.appendChild(AnswerFilenameTextArea);
            AnswerFilenameTextArea.classList.add("form-control");
            AnswerFilenameTextArea.placeholder = "Answer file name";
            AnswerFilenameTextArea.value = TestCase.AnswerFilename;
            AnswerFilenameTextArea.onchange = () => {
                TestCase.AnswerFilename = AnswerFilenameTextArea.value;
            }
        }
        let TestCasesTableBodyRowLimits = document.createElement("td"); TestCasesTableBodyRow.appendChild(TestCasesTableBodyRowLimits);
        {
            let TestCasesTableBodyRowLimitsInputGroup = document.createElement("div"); TestCasesTableBodyRowLimits.appendChild(TestCasesTableBodyRowLimitsInputGroup);
            TestCasesTableBodyRowLimitsInputGroup.classList.add("input-group");
            let TimeLimitInput = document.createElement("input"); TestCasesTableBodyRowLimitsInputGroup.appendChild(TimeLimitInput);
            TimeLimitInput.classList.add("form-control");
            TimeLimitInput.type = "number";
            TimeLimitInput.min = 0;
            TimeLimitInput.value = TestCase.TimeLimit;
            let TimeLimitUnit = document.createElement("span"); TestCasesTableBodyRowLimitsInputGroup.appendChild(TimeLimitUnit);
            TimeLimitUnit.classList.add("input-group-text");
            TimeLimitUnit.innerHTML = "ms";
            TimeLimitInput.onchange = () => {
                TestCase.TimeLimit = TimeLimitInput.value;
            }
            let MemoryLimitInput = document.createElement("input"); TestCasesTableBodyRowLimitsInputGroup.appendChild(MemoryLimitInput);
            MemoryLimitInput.classList.add("form-control");
            MemoryLimitInput.type = "number";
            MemoryLimitInput.min = 0;
            MemoryLimitInput.value = TestCase.MemoryLimit / 1024 / 1024;
            let MemoryLimitUnit = document.createElement("span"); TestCasesTableBodyRowLimitsInputGroup.appendChild(MemoryLimitUnit);
            MemoryLimitUnit.classList.add("input-group-text");
            MemoryLimitUnit.innerHTML = "MB";
            MemoryLimitInput.onchange = () => {
                TestCase.MemoryLimit = MemoryLimitInput.value * 1024 * 1024;
            }
            let ScoreInput = document.createElement("input"); TestCasesTableBodyRowLimitsInputGroup.appendChild(ScoreInput);
            ScoreInput.classList.add("form-control");
            ScoreInput.type = "number";
            ScoreInput.min = 0;
            ScoreInput.value = TestCase.Score;
            let ScoreUnit = document.createElement("span"); TestCasesTableBodyRowLimitsInputGroup.appendChild(ScoreUnit);
            ScoreUnit.classList.add("input-group-text");
            ScoreUnit.innerHTML = "pts";
            ScoreInput.onchange = () => {
                TestCase.Score = ScoreInput.value;
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
                    UploadTestCaseData.removeChild(TestCasesTableBodyRow);
                    TestCasesData.splice(Index, 1);
                }, () => { });
            };
        }
    }
};

UploadTestCaseButton.onclick = () => {
    AddLoading(UploadTestCaseButton);
    ShowModal("Upload test case", "The modification will overwrite the previous data and take effect immediately! Are you sure to upload the test case?", () => {
        var UploadFileElement = document.createElement("input");
        UploadFileElement.type = "file";
        UploadFileElement.accept = "application/x-zip-compressed";
        UploadFileElement.onchange = (EventData) => {
            var Reader = new FileReader();
            Reader.readAsDataURL(EventData.target.files[0]);
            Reader.onload = (EventData) => {
                RequestAPI("UploadTestCase", {
                    "PID": Data.PID,
                    "Data": EventData.target.result,
                }, () => { }, (Response) => {
                    UploadTestCaseData.innerHTML = "";
                    TestCasesData = [];
                    let TestCaseCount = 0;
                    Response.IOFilenames.map((Sample) => {
                        CreateTestCaseRow(TestCaseCount, Sample.InputFilename, Sample.AnswerFilename);
                        TestCaseCount++;
                    });
                }, () => { }, () => { });
                RemoveLoading(UploadTestCaseButton);
            }
        }
        UploadFileElement.click();
    }, () => {
        RemoveLoading(UploadTestCaseButton);
    });
};

UploadTestCaseSubmitButton.onclick = () => {
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
            "InputFilename": String(TestCasesData[TestCaseCount].InputFilename),
            "AnswerFilename": String(TestCasesData[TestCaseCount].AnswerFilename),
            "TimeLimit": Number(TestCasesData[TestCaseCount].TimeLimit),
            "MemoryLimit": Number(TestCasesData[TestCaseCount].MemoryLimit),
            "Score": Number(TestCasesData[TestCaseCount].Score)
        });
    }
    RequestAPI("UpdateTestCase", {
        "PID": Data.PID,
        "TestCase": JSON.stringify(SubmitTestGroupsData)
    }, () => { }, () => { }, () => { }, () => { });
};

RequestAPI("GetProblem", {
    "PID": String(Data.PID)
}, () => { }, (Response) => {
    let TestCaseCount = 0;
    Response.TestGroups.map((TestGroup) => {
        TestGroup.TestCases.map((TestCase) => {
            debugger
            TestCasesData.push({
                "TGID": TestGroup.TGID,
                "InputFilename": TestCase.InputFilename,
                "AnswerFilename": TestCase.AnswerFilename,
                "TimeLimit": TestCase.TimeLimit,
                "MemoryLimit": TestCase.MemoryLimit,
                "Score": TestCase.Score,
            });
            CreateTestCaseRow(TestCaseCount);
            TestCaseCount++;
        });
    });
}, () => { }, () => { });

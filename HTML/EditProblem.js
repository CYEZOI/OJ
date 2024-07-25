const EditProblemSpinner = document.getElementById("EditProblemSpinner");
const EditProblemData = document.getElementById("EditProblemData");
const EditProblemIOFilename = document.getElementById("EditProblemIOFilename");
const EditProblemDescription = document.getElementById("EditProblemDescription");
const EditProblemInput = document.getElementById("EditProblemInput");
const EditProblemOutput = document.getElementById("EditProblemOutput");
const EditProblemRange = document.getElementById("EditProblemRange");
const EditProblemHint = document.getElementById("EditProblemHint");
const EditProblemSample = document.getElementById("EditProblemSample");
const EditProblemAddSample = document.getElementById("EditProblemAddSample");
const EditProblemUploadTestCase = document.getElementById("EditProblemUploadTestCase");
const EditProblemSaveButton = document.getElementById("EditProblemSaveButton");
CheckTokenAvailable();
let SamplesData = [];

const CreateSampleRow = (Index) => {
    if (Index >= SamplesData.length) {
        SamplesData.push({
            "Input": "",
            "Output": "",
            "Description": "",
            "InputEditor": null,
            "OutputEditor": null,
        });
    }
    let Sample = SamplesData[Index];
    let SamplesTableBodyRow = document.createElement("tr"); EditProblemSample.appendChild(SamplesTableBodyRow);
    {
        let SamplesTableBodyRowInput = document.createElement("td"); SamplesTableBodyRow.appendChild(SamplesTableBodyRowInput);
        {
            let InputTextArea = document.createElement("textarea"); SamplesTableBodyRowInput.appendChild(InputTextArea);
            InputTextArea.innerHTML = Sample.Input;
            Sample.InputEditor = CreateCodeMirrorTextEditor(InputTextArea);
            Sample.InputEditor.setSize("100%", "auto");
        }
        let SamplesTableBodyRowOutput = document.createElement("td"); SamplesTableBodyRow.appendChild(SamplesTableBodyRowOutput);
        {
            let OutputTextArea = document.createElement("textarea"); SamplesTableBodyRowOutput.appendChild(OutputTextArea);
            OutputTextArea.innerHTML = Sample.Output;
            Sample.OutputEditor = CreateCodeMirrorTextEditor(OutputTextArea);
            Sample.OutputEditor.setSize("100%", "auto");
        }
        let SamplesTableBodyRowDescription = document.createElement("td"); SamplesTableBodyRow.appendChild(SamplesTableBodyRowDescription);
        {
            CreateMarkdownEditor(SamplesTableBodyRowDescription, Sample.Description, (Editor) => {
                Sample["Description"] = Editor.value;
            });
        }
        let SamplesTableBodyRowOperation = document.createElement("td"); SamplesTableBodyRow.appendChild(SamplesTableBodyRowOperation);
        {
            let DeleteButton = document.createElement("button"); SamplesTableBodyRowOperation.appendChild(DeleteButton);
            DeleteButton.classList.add("btn");
            DeleteButton.classList.add("btn-danger");
            DeleteButton.innerHTML = "Delete";
            DeleteButton.onclick = () => {
                ShowModal("Delete sample", "Are you sure to delete this sample?", () => {
                    EditProblemSample.removeChild(SamplesTableBodyRow);
                    SamplesData.splice(Index, 1);
                }, () => { });
            };
        }
    }
}

let EditProblemSpinnerCollapse = new bootstrap.Collapse("#EditProblemSpinner", {
    toggle: false
});

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
        PIDInput.type = "number";
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

CreateMarkdownEditor(EditProblemDescription);
CreateMarkdownEditor(EditProblemInput);
CreateMarkdownEditor(EditProblemOutput);
CreateMarkdownEditor(EditProblemRange);
CreateMarkdownEditor(EditProblemHint);
EditProblemAddSample.onclick = () => {
    CreateSampleRow(EditProblemSample.children.length);
};

EditProblemSaveButton.onclick = () => {
    let SubmitSamplesData = [];
    for (let SampleCount = 0; SampleCount < SamplesData.length; SampleCount++) {
        SubmitSamplesData.push({
            "Input": String(SamplesData[SampleCount].InputEditor.getValue()),
            "Output": String(SamplesData[SampleCount].OutputEditor.getValue()),
            "Description": String(SamplesData[SampleCount].Description)
        });
    }

    RequestAPI(Data.PID != null ? "UpdateProblem" : "AddProblem", {
        "PID": String(PID.value),
        "Title": String(Title.value),
        "IOFilename": String(EditProblemIOFilename.value.trim()),
        "Description": String(EditProblemDescription.getValue().trim()),
        "Input": String(EditProblemInput.getValue().trim()),
        "Output": String(EditProblemOutput.getValue().trim()),
        "Range": String(EditProblemRange.getValue().trim()),
        "Hint": String(EditProblemHint.getValue().trim()),
        "Samples": JSON.stringify(SubmitSamplesData)
    }, () => { }, () => {
        setTimeout(() => {
            SwitchPage("Problem", {
                "PID": PID.value
            });
        }, 1000);
    }, () => { }, () => { });
};

if (Data.PID != null) {
    EditProblemSpinnerCollapse.show();
    RequestAPI("GetProblem", {
        "PID": String(Data.PID)
    }, () => { }, (Response) => {
        EditProblemIOFilename.value = Response.IOFilename;
        Title.value = Response.Title;

        let SampleCount = 0;
        Response.Samples.map((Sample) => {
            SamplesData.push({
                "Input": Sample.Input,
                "Output": Sample.Output,
                "Description": Sample.Description,
            });
            CreateSampleRow(SampleCount);
            SampleCount++;
        });

        EditProblemDescription.setValue(Response.Description);
        EditProblemInput.setValue(Response.Input);
        EditProblemOutput.setValue(Response.Output);
        EditProblemRange.setValue(Response.Range);
        EditProblemHint.setValue(Response.Hint);

        EditProblemSpinnerCollapse.hide();
    }, () => { }, () => { });
}
else {
    EditProblemSpinnerCollapse.hide();
}

EditProblemUploadTestCase.onclick = () => {
    if (Data.PID != null) {
        SwitchPage("UploadTestCase", {
            PID: Data.PID,
        });
    }
    else {
        ShowError("Please save problem before uploading test case");
    }
};
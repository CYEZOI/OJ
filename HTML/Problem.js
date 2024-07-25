const ProblemSubmitButton = document.getElementById("ProblemSubmitButton");
const ProblemEditButton = document.getElementById("ProblemEditButton");
const ProblemDeleteButton = document.getElementById("ProblemDeleteButton");
const ProblemData = document.getElementById("ProblemData");
CheckTokenAvailable();

const CreateAccordion = (Title, Body, ID, AutoShow = true) => {
    let Accordion = document.createElement("div"); ProblemData.appendChild(Accordion);
    Accordion.classList.add("accordion-item");
    {
        let AccordionHeader = document.createElement("h2"); Accordion.appendChild(AccordionHeader);
        AccordionHeader.classList.add("accordion-header");
        {
            let AccordionTitle = document.createElement("button"); AccordionHeader.appendChild(AccordionTitle);
            AccordionTitle.classList.add("accordion-button");
            if (!AutoShow) {
                AccordionTitle.classList.add("collapsed");
            }
            AccordionTitle.type = "button";
            AccordionTitle.setAttribute("data-bs-toggle", "collapse");
            AccordionTitle.setAttribute("data-bs-target", "#" + ID);
            AccordionTitle.innerHTML = Title;
        }
        let AccordionData = document.createElement("div"); Accordion.appendChild(AccordionData);
        AccordionData.id = ID;
        AccordionData.classList.add("accordion-collapse");
        AccordionData.classList.add("collapse");
        if (AutoShow) {
            AccordionData.classList.add("show");
        }
        {
            let AccordionBody = document.createElement("div"); AccordionData.appendChild(AccordionBody);
            AccordionBody.classList.add("accordion-body");
            AccordionBody.innerHTML = marked.parse(Body);
            renderMathInElement(AccordionBody, {
                delimiters: [
                    { left: "$$", right: "$$", display: true },
                    { left: "$", right: "$", display: false }
                ]
            });
        }
    }
};
if (Data.PID == null) {
    SwitchPage("Home");
}
ProblemSubmitButton.onclick = () => {
    SwitchPage("Submit", {
        "PID": Data.PID
    });
};
ProblemEditButton.onclick = () => {
    SwitchPage("EditProblem", {
        "PID": Data.PID
    });
};
ProblemDeleteButton.onclick = () => {
    ShowModal("Delete problem", "Are you sure to delete this problem?", () => {
        RequestAPI("DeleteProblem",
            {
                "PID": String(Data.PID)
            }, () => { }, (Response) => {
                SwitchPage("Problems");
            }, () => { }, () => { });
    }, () => { });
};
for (let i = 0; i < 10; i++) {
    CreateAccordion(CreatePlaceHolder().outerHTML, CreatePlaceHolder().outerHTML, "");
}
PageTitle.innerHTML += " " + Data.PID;
RequestAPI("GetProblem", {
    "PID": String(Data.PID)
}, () => { }, (Response) => {
    ProblemData.innerHTML = "";
    PageTitle.innerHTML += ": " + Response.Title;

    CreateAccordion("Description", Response.Description, "Description");
    CreateAccordion("Input", Response.Input, "Input");
    CreateAccordion("Output", Response.Output, "Output");

    let Samples = document.createElement("div");
    Response.Samples.map((Sample) => {
        let SampleData = document.createElement("div"); Samples.appendChild(SampleData);
        SampleData.classList.add("row");
        {
            let SampleTitle = document.createElement("div"); SampleData.appendChild(SampleTitle);
            SampleTitle.classList.add("col-12");
            SampleTitle.innerHTML = "Sample " + (Response.Samples.indexOf(Sample) + 1);
            let SampleInput = document.createElement("div"); SampleData.appendChild(SampleInput);
            SampleInput.classList.add("col-6");
            {
                let InputTextArea = document.createElement("textarea"); SampleInput.appendChild(InputTextArea);
                InputTextArea.classList.add("form-control");
                InputTextArea.innerHTML = Sample.Input;
            }
            let SampleOutput = document.createElement("div"); SampleData.appendChild(SampleOutput);
            SampleOutput.classList.add("col-6");
            {
                let OutputTextArea = document.createElement("textarea"); SampleOutput.appendChild(OutputTextArea);
                OutputTextArea.classList.add("form-control");
                OutputTextArea.innerHTML = Sample.Output;
            }
            if (Sample.Description != "") {
                let SampleDescription = document.createElement("div"); SampleData.appendChild(SampleDescription);
                SampleDescription.innerHTML = Sample.Description;
            }
        }
        let HorizontalLine = document.createElement("hr"); Samples.appendChild(HorizontalLine);
    });
    if (Samples.children.length != 0) {
        Samples.removeChild(Samples.lastChild);
    }
    CreateAccordion("Samples", Samples.outerHTML, "Samples");
    let Textarea = document.querySelectorAll("textarea.form-control");
    for (let i = 0; i < Textarea.length; i++) {
        CreateCodeMirrorText(Textarea[i]).setSize("100%", "auto");
    }

    if (Response.Range != "") {
        CreateAccordion("Range", Response.Range, "Range");
    }
    if (Response.Hint != "") {
        CreateAccordion("Hint", Response.Hint, "Hint", false);
    }
    let MaxTimeLimit = 0;
    let MaxMemoryLimit = 0;
    Response.TestGroups.map((TestGroup) => {
        TestGroup.TestCases.map((TestCase) => {
            MaxTimeLimit = Math.max(MaxTimeLimit, TestCase.TimeLimit);
            MaxMemoryLimit = Math.max(MaxMemoryLimit, TestCase.MemoryLimit);
        });
    });
    let DataTable = document.createElement("table");
    DataTable.classList.add("table");
    DataTable.classList.add("table-bordered");
    DataTable.classList.add("table-striped");
    let TableBody = document.createElement("tbody"); DataTable.appendChild(TableBody);
    {
        const AddRow = (Name, Value) => {
            let TableBodyRow = document.createElement("tr"); TableBody.appendChild(TableBodyRow);
            {
                let TableBodyName = document.createElement("td"); TableBodyRow.appendChild(TableBodyName);
                TableBodyName.innerHTML = Name;
                let TableBodyValue = document.createElement("td"); TableBodyRow.appendChild(TableBodyValue);
                TableBodyValue.innerHTML = Value;
            }
        };
        AddRow("Time limit", TimeToString(MaxTimeLimit));
        AddRow("Memory limit", MemoryToString(MaxMemoryLimit));
        AddRow("Input filename", Response.IOFilename !== "" ? Response.IOFilename + ".in" : "Standard input");
        AddRow("Output filename", Response.IOFilename !== "" ? Response.IOFilename + ".out" : "Standard output");
    }
    CreateAccordion("Other data", DataTable.outerHTML, "OtherData");
}, () => { }, () => { });

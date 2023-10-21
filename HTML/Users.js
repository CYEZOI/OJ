const OriginalPassword = document.getElementById("OriginalPassword");
const HashPassword = document.getElementById("HashPassword");
const HashedPassword = document.getElementById("HashedPassword");
const UsersAddUserButton = document.getElementById("UsersAddUserButton");
const UsersData = document.getElementById("UsersData");

CheckTokenAvailable();
if (Data.Page == null) {
    Data.Page = 1;
}
for (let i = 0; i < 10; i++) {
    let Row = document.createElement("tr"); UsersData.children[1].appendChild(Row);
    {
        for (let j = 0; j < 7; j++) {
            let Column = document.createElement("td"); Row.appendChild(Column);
            Column.appendChild(CreatePlaceHolder());
        }
    }
}
HashPassword.onclick = () => {
    RequestAPI("HashPassword", {
        "OriginalPassword": String(OriginalPassword.value)
    }, () => { }, (Response) => {
        HashedPassword.innerText = Response.HashedPassword;
    });
};
UsersAddUserButton.onclick = () => {
    SwitchPage("AddUser");
};
RequestAPI("GetUsers", {
    "Page": Number(Data.Page)
}, () => { }, (Response) => {
    UsersData.children[1].innerHTML = "";
    if (!Response.IsAdmin) {
        SwitchPage("Home");
    }
    for (let i = 0; i < Response.Users.length; i++) {
        let DataRow = document.createElement("tr"); UsersData.children[1].appendChild(DataRow);
        {
            let DataRowUsernameInput, DataRowNicknameInput, DataRowEmailAddressInput, DataRowPasswordInput, DataRowRoleSelect;
            let DataRowUID = document.createElement("td"); DataRow.appendChild(DataRowUID);
            {
                let DataRowUIDSpan = document.createElement("span"); DataRowUID.appendChild(DataRowUIDSpan);
                DataRowUIDSpan.innerText = Response.Users[i].UID;
            }
            let DataRowUsername = document.createElement("td"); DataRow.appendChild(DataRowUsername);
            {
                DataRowUsernameInput = document.createElement("input"); DataRowUsername.appendChild(DataRowUsernameInput);
                DataRowUsernameInput.type = "text";
                DataRowUsernameInput.classList.add("form-control");
                DataRowUsernameInput.value = Response.Users[i].Username;
            }
            let DataRowNickname = document.createElement("td"); DataRow.appendChild(DataRowNickname);
            {
                DataRowNicknameInput = document.createElement("input"); DataRowNickname.appendChild(DataRowNicknameInput);
                DataRowNicknameInput.type = "text";
                DataRowNicknameInput.classList.add("form-control");
                DataRowNicknameInput.value = Response.Users[i].Nickname;
            }
            let DataRowEmail = document.createElement("td"); DataRow.appendChild(DataRowEmail);
            {
                DataRowEmailAddressInput = document.createElement("input"); DataRowEmail.appendChild(DataRowEmailAddressInput);
                DataRowEmailAddressInput.type = "text";
                DataRowEmailAddressInput.classList.add("form-control");
                DataRowEmailAddressInput.value = Response.Users[i].EmailAddress;
            }
            let DataRowPassword = document.createElement("td"); DataRow.appendChild(DataRowPassword);
            {
                DataRowPasswordInput = document.createElement("input"); DataRowPassword.appendChild(DataRowPasswordInput);
                DataRowPasswordInput.type = "text";
                DataRowPasswordInput.classList.add("form-control");
                DataRowPasswordInput.value = Response.Users[i].Password;
            }
            let DataRowRole = document.createElement("td"); DataRow.appendChild(DataRowRole);
            {
                DataRowRoleSelect = document.createElement("select"); DataRowRole.appendChild(DataRowRoleSelect);
                CreateRoleSelect(DataRowRoleSelect, Response.Users[i].Role);
            }
            let DataRowAction = document.createElement("td"); DataRow.appendChild(DataRowAction);
            {
                let DataRowActionSaveButton = document.createElement("button"); DataRowAction.appendChild(DataRowActionSaveButton);
                DataRowActionSaveButton.innerText = "Save";
                DataRowActionSaveButton.classList.add("btn");
                DataRowActionSaveButton.classList.add("btn-warning");
                DataRowActionSaveButton.classList.add("me-1");
                DataRowActionSaveButton.onclick = () => {
                    RequestAPI("UpdateUser", {
                        "UID": Number(Response.Users[i].UID),
                        "Username": String(DataRowUsernameInput.value),
                        "Nickname": String(DataRowNicknameInput.value),
                        "EmailAddress": String(DataRowEmailAddressInput.value),
                        "Password": String(DataRowPasswordInput.value),
                        "Role": Number(DataRowRoleSelect.selectedIndex)
                    }, () => { }, () => {
                        ShowSuccess("Update User Success");
                    }, () => { }, () => { });
                }
                let DataRowActionDeleteButton = document.createElement("button"); DataRowAction.appendChild(DataRowActionDeleteButton);
                DataRowActionDeleteButton.innerText = "Delete";
                DataRowActionDeleteButton.classList.add("btn");
                DataRowActionDeleteButton.classList.add("btn-danger");
                DataRowActionDeleteButton.classList.add("me-1");
                DataRowActionDeleteButton.onclick = () => {
                    ShowModal("Delete user", "Are you sure to delete this user?", () => {
                        RequestAPI("DeleteUser", {
                            "UID": Number(Response.Users[i].UID)
                        }, () => { }, () => {
                            ShowSuccess("Delete User Success");
                            DataRow.remove();
                        }, () => { }, () => { });
                    }, () => { });
                }
            }
        }
    }
}, () => { }, () => { });

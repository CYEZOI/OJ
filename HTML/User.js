const UserUID = document.getElementById("UserUID");
const UserUsername = document.getElementById("UserUsername");
const UserNickname = document.getElementById("UserNickname");
const UserEmail = document.getElementById("UserEmail");
const UserRole = document.getElementById("UserRole");
CheckTokenAvailable();
if (Data.UID == null) {
    SwitchPage("Home");
}

UserUID.appendChild(CreatePlaceHolder());
UserUsername.appendChild(CreatePlaceHolder());
UserNickname.appendChild(CreatePlaceHolder());
UserEmail.appendChild(CreatePlaceHolder());
UserRole.appendChild(CreatePlaceHolder());

RequestAPI("GetUser", {
    UID: Number(Data.UID)
}, () => { }, (Response) => {
    PageTitle.innerHTML += " " + Response.Nickname;
    UserUID.innerHTML = Data.UID;
    UserUsername.innerHTML = Response.Username;
    UserNickname.innerHTML = Response.Nickname;
    UserEmail.innerHTML = Response.EmailAddress;
    UserRole.innerHTML = RoleName[Response.Role];
}, () => { }, () => { })

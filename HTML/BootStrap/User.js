CheckTokenAvailable();
if (Data.UID == null) {
    SwitchPage("Home");
}
RequestAPI("GetUser", {
    UID: Number(Data.UID)
}, () => { }, (Response) => {
    document.getElementsByTagName("h4")[0].innerHTML += " " + Response.Username;
    UserData.children[0].children[0].children[1].innerHTML = Data.UID;
    UserData.children[0].children[1].children[1].innerHTML = Response.Username;
    UserData.children[0].children[2].children[1].innerHTML = Response.Nickname;
    UserData.children[0].children[3].children[1].innerHTML = Response.EmailAddress;
    UserData.children[0].children[4].children[1].innerHTML = RoleName[Response.Role];
}, () => { }, () => { })

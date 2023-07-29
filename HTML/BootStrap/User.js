CheckTokenAvailable();
if (Data.UID == null) {
    SwitchPage("Home");
}
RequestAPI("GetUser", {
    UID: Number(Data.UID)
}, () => { }, (Response) => {
    document.getElementsByTagName("h4")[0].innerHTML += " " + Response.Username;
    // Response.Email
    // Response.Nickname
}, () => { }, () => { })

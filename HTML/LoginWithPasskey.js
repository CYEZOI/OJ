const LoginWithPasskeyInformation = document.getElementById("LoginWithPasskeyInformation");
const LoginWithPasskeyReady = document.getElementById("LoginWithPasskeyReady");

if (!navigator.credentials) {
    LoginWithPasskeyInformation.innerText = "We are sorry, but your browser does not support passkey.";
    LoginWithPasskeyReady.disabled = true;
}
else {
    LoginWithPasskeyInformation.innerText = "When your passkey is ready, click the button bellow.";
}

LoginWithPasskeyReady.onclick = async () => {
    LoginWithPasskeyInformation.innerText = "Contact the server for information...";
    RequestAPI("CreatePasskeyChallenge", {}, () => { }, async (Response) => {
        Response.challenge = StringToBuffer(Response.challenge);
        LoginWithPasskeyInformation.innerText = "Please input your passkey.";
        await navigator.credentials.get({ publicKey: GetOptions })
            .then((Credential) => {
                LoginWithPasskeyInformation.innerText = "Checking passkey...";
                Credential.response.userHandle = BufferToString(Credential.response.userHandle);
                Credential.response.signature = btoa(BufferToString(Credential.response.signature));
                RequestAPI("LoginWithPasskey", {
                    "Credential": JSON.stringify(Credential),
                }, () => { }, (Response) => {
                    localStorage.setItem("Token", Response.Token);
                    localStorage.setItem("IsAdmin", Response.IsAdmin);
                    localStorage.setItem("UID", Response.UID);
                    localStorage.setItem("Username", Response.Username);
                    const AddonStyle = document.getElementById("AddonStyle");
                    AddonStyle.innerHTML = ".NotLoginOnly { display: none; }";
                    if (!Response.IsAdmin) {
                        AddonStyle.innerHTML += ".AdminOnly { display: none; }";
                    }
                    setTimeout(() => {
                        SwitchPage("Home");
                    }, 1000);
                }, () => { }, () => { }, false);
            }).catch((Error) => {
                LoginWithPasskeyInformation.innerText = Error;
                RequestAPI("DeletePasskeyChallenge", {
                    "Challenge": String(Response.Challenge)
                }, () => { }, () => { }, () => { }, () => { }, false);
            });
    }, (Response) => {
        LoginWithPasskeyInformation.innerText = "Server returned error.";
    }, () => { }, false);
};

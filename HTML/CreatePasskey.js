const CreatePasskeyInformation = document.getElementById("CreatePasskeyInformation");
const CreatePasskeyReady = document.getElementById("CreatePasskeyReady");
CheckTokenAvailable();

if (!navigator.credentials) {
    CreatePasskeyInformation.innerText = "We are sorry, but your browser does not support passkey.";
    CreatePasskeyReady.disabled = true;
}
else {
    CreatePasskeyInformation.innerText = "When your passkey is ready, click the button bellow.";
}

CreatePasskeyReady.onclick = async () => {
    CreatePasskeyInformation.innerText = "Contact the server for information...";
    RequestAPI("CreatePasskeyCreateChallenge", {}, () => { }, async (Response) => {
        Response.challenge = StringToBuffer(Response.challenge);
        Response.user.id = StringToBuffer(Response.user.id);
        CreatePasskeyInformation.innerText = "Please set up your passkey.";
        await navigator.credentials.create({ publicKey: Response })
            .then((Credential) => {
                debugger;
                CreatePasskeyInformation.innerText = "Uploading the passkey to server...";
                RequestAPI("CreatePasskey", {
                    "Credential": JSON.stringify(Credential)
                }, () => { }, () => {
                    CreatePasskeyInformation.innerText = "Passkey created successfully.";
                }, (Response) => {
                    CreatePasskeyInformation.innerText = "Server returned error.";
                }, () => { });
            }).catch((Error) => {
                CreatePasskeyInformation.innerText = Error;
                RequestAPI("DeletePasskeyChallenge", {
                    "Challenge": String(Response.Challenge)
                }, () => { }, () => { }, () => { }, () => { });
            });
    }, (Response) => {
        CreatePasskeyInformation.innerText = "Server returned error.";
    }, () => { });
};

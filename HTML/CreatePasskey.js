const CreatePasskeyInformation = document.getElementById("CreatePasskeyInformation");
const CreatePasskeyReady = document.getElementById("CreatePasskeyReady");

if (!navigator.credentials) {
    CreatePasskeyInformation.innerText = "We are sorry, but your browser does not support passkey.";
    CreatePasskeyReady.disabled = true;
}
else {
    CreatePasskeyInformation.innerText = "When your passkey is ready, click the button bellow.";
}

CreatePasskeyReady.onclick = async () => {
    CreatePasskeyInformation.innerText = "Contact the server for information...";
    RequestAPI("CreatePasskeyChallenge", {}, () => { }, async (Response) => {
        const CreateOptions = {
            "challenge": StringToBuffer(Response.ChallengeID),
            "rp": { "name": "OJ" },
            "user": {
                "id": StringToBuffer(localStorage.getItem("UID")),
                "name": localStorage.getItem("Username"),
                "displayName": localStorage.getItem("Username")
            },
            "pubKeyCredParams": [{ "alg": -7, "type": "public-key" }],
            "timeout": 60000
        };
        CreatePasskeyInformation.innerText = "Please set up your passkey.";
        await navigator.credentials.create({ publicKey: CreateOptions })
            .then((Credential) => {
                CreatePasskeyInformation.innerText = "Uploading the passkey to server...";
                var CredentialIdLength = btoa(BufferToString(Credential.response.getAuthenticatorData()).substr(53, 2)).charCodeAt(1);
                RequestAPI("CreatePasskey", {
                    "ChallengeID": String(Response.ChallengeID),
                    "CredentialID": String(btoa(BufferToString(Credential.response.getAuthenticatorData()).substr(55, CredentialIdLength))),
                    "CredentialPublicKey": String(btoa(BufferToString(Credential.response.getPublicKey())))
                }, () => { }, () => {
                    CreatePasskeyInformation.innerText = "Passkey created successfully.";
                }, (Response) => {
                    CreatePasskeyInformation.innerText = "Server returned error.";
                }, () => { });
            }).catch((Error) => {
                CreatePasskeyInformation.innerText = Error;
                RequestAPI("DeletePasskeyChallenge", {
                    "ChallengeID": String(Response.ChallengeID)
                }, () => { }, () => { }, () => { }, () => { });
            });
    }, (Response) => {
        CreatePasskeyInformation.innerText = "Server returned error.";
    }, () => { });
};

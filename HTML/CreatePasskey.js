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
            "rp": {
                "name": "Passkey Example",
                "id": "example.com"
            },
            "user": {
                "id": "GOVsRuhMQWNoScmh_cK02QyQwTolHSUSlX5ciH242Y4",
                "name": "Michael",
                "displayName": "Michael"
            },
            "pubKeyCredParams": [
                {
                    "alg": -7,
                    "type": "public-key"
                }
            ],
            "timeout": 60000,
            "attestation": "none",
            "excludeCredentials": [
            ],
            "authenticatorSelection": {
                "authenticatorAttachment": "platform",
                "requireResidentKey": true,
                "residentKey": "required"
            },
            "extensions": {
                "credProps": true
            }
        };
        CreateOptions.challenge = StringToBuffer(CreateOptions.challenge);
        CreateOptions.user.id = StringToBuffer(CreateOptions.user.id);
        CreatePasskeyInformation.innerText = "Please set up your passkey.";
        var Credential;
        try {
            Credential = await navigator.credentials.create({ publicKey: CreateOptions });
            CreatePasskeyInformation.innerText = "Uploading the passkey to server...";
            console.log(Credential);
        } catch (e) {
            CreatePasskeyInformation.innerText = e;
        }
    }, (Response) => {
        CreatePasskeyInformation.innerText = "Server returned error.";
    }, () => { });
};

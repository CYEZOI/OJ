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
    RequestAPI("GetPasskeyCreateOption", {}, () => { }, async (Response) => {
        const CreateOptions = Response.CreateOptions;
        CreateOptions.challenge = Base64ToBuffer(CreateOptions.challenge);
        CreateOptions.user.id = Base64ToBuffer(CreateOptions.user.id);

        LoginWithPasskeyInformation.innerText = "Please set up your passkey.";
        const Credential = await navigator.credentials.create({ publicKey: publicKeyCredentialCreationOptions });
        LoginWithPasskeyInformation.innerText = "Passkey set up successfully.";

        console.log(Credential);
    }, (Response) => {
        LoginWithPasskeyInformation.innerText = "Server returned error.";
    }, () => { }, false);
    const publicKeyCredentialCreationOptions = {
        challenge: new Uint8Array([117, 61, 252, 231, 191, 241]),
        rp: {
            name: "Example",
        },
        user: {
            id: new Uint8Array([79, 252, 83, 72, 214, 7, 89, 26]),
            name: "john78",
            displayName: "John",
        },
        pubKeyCredParams: [{ alg: -7, type: "public-key" }],
        timeout: 60000,
    };
};

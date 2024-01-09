localStorage.removeItem("Token");
localStorage.removeItem("IsAdmin");
localStorage.removeItem("UID");
localStorage.removeItem("Username");
const AddonStyle = document.getElementById("AddonStyle");
AddonStyle.innerHTML = ".LoginOnly { display: none; }";
SwitchPage("Login");

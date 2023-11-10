localStorage.removeItem("Token");
localStorage.removeItem("IsAdmin");
const AddonStyle = document.getElementById("AddonStyle");
AddonStyle.innerHTML = ".LoginOnly { display: none; }";
SwitchPage("Login");

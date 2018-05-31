//
//   The HTML PAGE Scan
//

const char api[] PROGMEM = R"=====(
<meta name="viewport" content="width=device-width, initial-scale=1" />
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<link rel="stylesheet" href="style.css" type="text/css" />
<script src="microajax.js"></script> 
<a href="/"  class="btn btn--s"><</a>&nbsp;&nbsp;<strong>Режим отладки</strong>
<hr>
<table border="0"  cellspacing="0" cellpadding="3" style="width:310px" >
<tr><td align="right">433Мгц :</td><td><span id="x_433"></span></td></tr>
<tr><td align="right">FreeHeap</td><td><span id="x_FreeHeap"></span></td></tr>



<tr><td colspan="2" align="center"><a href="javascript:GetState()" class="btn btn--m btn--blue">Обновить</a></td></tr>
</table>
<script>

function GetState()
{
  setValues("/admin/infovalues");
}

window.onload = function ()
{
  load("style.css","css", function() 
  {
    load("function.js","js", function() 
    {
					setValues("/admin/values");
					setInterval(GetState,3000);
    });
  });
}
function load(e,t,n){if("js"==t){var a=document.createElement("script");a.src=e,a.type="text/javascript",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)}else if("css"==t){var a=document.createElement("link");a.href=e,a.rel="stylesheet",a.type="text/css",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)}}



</script>
)=====" ;


//
// FILL WITH INFOMATION
// 

void api() {
  String values ="";
  values += "x_433|" + (String)ReceivedValue +  "|div\n";
  values += "x_FreeHeap|" +  (String)ESP.getFreeHeap() +  "|div\n";
  server.send ( 200, "text/plain", values);
}

/*
const char* www_username = "admin";
const char* www_password = "admin";

void api(){         
    if(!HTTP.authenticate(www_username, www_password))
      return HTTP.requestAuthentication();   
String api = "ESP8266   \n\n";
api += "WiFi RSSI \n";
api += WiFi.RSSI();
api += "\n";
api += "Ram \n";
api += ESP.getFreeHeap();
api += "\n";
api += "Chip ID\n ";
api += ESP.getChipId();
api += "\n";
api += "FlashChip ID\n ";
api += ESP.getFlashChipId();
api += "\n";
api += "\n";
api += "433 MHz \n";
api += ReceivedValue;

HTTP.send(200, "text/plain", api);
if (!handleFileRead("/scan.html")) HTTP.send(404, "text/plain", "ServerNotFound");

 };
*/


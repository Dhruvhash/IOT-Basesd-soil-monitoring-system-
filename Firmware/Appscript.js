function doGet(e) {
  var sheet = SpreadsheetApp.getActiveSpreadsheet().getActiveSheet();

  var time = new Date();

  var soil = e.parameter.soil;
  var temp = e.parameter.temp;
  var hum = e.parameter.hum;
  var rain = e.parameter.rain;
  var motion = e.parameter.motion;

  sheet.appendRow([time, soil, temp, hum, rain, motion]);

  return ContentService.createTextOutput("OK");
}

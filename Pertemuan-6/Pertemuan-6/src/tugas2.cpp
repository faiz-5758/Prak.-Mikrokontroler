#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "NAMA_WIFI";       // Ganti dengan nama Wi-Fi / Hotspot
const char* password = "PASSWORD_WIFI"; // Ganti dengan kata sandi Wi-Fi

WebServer server(80);
const int ldrPin = 34; // Pin AO dari Modul LDR ke GPIO 34
unsigned long waktuSebelumnya = 0;
const long interval = 2000;

// Membaca kecerahan dalam satuan persen (0% = Gelap Total, 100% = Sangat Terang)
int bacaCahayaPersen(int &nilaiADC) {
  nilaiADC = analogRead(ldrPin); // (1) Baca nilai analog dari ldrPin
  
  // Modul LDR standar: Gelap = 4095, Terang = 0. Balik menjadi 0 - 100%
  int persen = map(nilaiADC, 4095, 0, 0, 100); // (2) Rentang ADC: 4095 = 0%, 0 = 100%
  return constrain(persen, 0, 100);
}

String tentukanStatus(int persen) {
  if (persen > 70) return "CERAH";         // (3) Batas persen status CERAH: > 70%
  else if (persen >= 35) return "SEDANG";  // (4) Batas persen status SEDANG: 35% - 70%
  else return "REDUP";                     // Status REDUP: < 35%
}

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>Tugas 2 - Sensor Cahaya LDR</title>
<style>
body { font-family: Arial, sans-serif; text-align: center; margin-top: 50px; }
.badge { padding: 15px 30px; color: white; font-weight: bold; border-radius: 8px; display: inline-block; font-size: 24px; }
.REDUP { background-color: #6c757d; }
.SEDANG { background-color: #28a745; }
.CERAH { background-color: #ffc107; color: black; }
</style>
</head>
<body>
<h2>Pemantauan Intensitas Cahaya (LDR)</h2>
<h3>Intensitas Cahaya: <span id="cahaya">--</span> % (ADC: <span id="adc">--</span>)</h3>
<div id="statusBox" class="badge SEDANG">MEMUAT...</div>
<script>
function ambilData() {
  fetch("/data")
  .then(res => res.json())
  .then(data => {
    document.getElementById("cahaya").innerText = data.cahaya;
    document.getElementById("adc").innerText = data.adc;
    let box = document.getElementById("statusBox");
    box.innerText = data.status;
    box.className = "badge " + data.status;
  });
}
setInterval(ambilData, 2000);
ambilData();
</script>
</body>
</html>
)rawliteral";

void handleData() {
  int adc = 0;
  int persen = bacaCahayaPersen(adc);
  String status = tentukanStatus(persen);
  String json = "{\"cahaya\":" + String(persen) + ",\"adc\":" + String(adc) + ",\"status\":\"" + status + "\"}";
  server.send(200, "application/json", json); // (5) Tipe konten untuk data JSON
}

void setup() {
  Serial.begin(115200);
  analogReadResolution(12);
  
  WiFi.begin(ssid, password);
  Serial.print("Menghubungkan ke WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nTerhubung! IP Address: " + WiFi.localIP().toString());
  
  server.on("/", []() { server.send(200, "text/html", index_html); });
  server.on("/data", handleData); // (6) Path endpoint JSON yang dipanggil fetch()
  server.begin();
}

void loop() {
  server.handleClient();
  
  unsigned long waktuSekarang = millis();
  if (waktuSekarang - waktuSebelumnya >= interval) {
    waktuSebelumnya = waktuSekarang;
    int adc = 0;
    int persen = bacaCahayaPersen(adc);
    String status = tentukanStatus(persen);
    
    Serial.print("ADC LDR: ");
    Serial.print(adc);
    Serial.print(" | Kecerahan: ");
    Serial.print(persen);
    Serial.print("% | Status: ");
    Serial.println(status);
  }
}
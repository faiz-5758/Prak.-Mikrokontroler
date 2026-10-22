#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ESP32Servo.h>

const char* ssid = "NAMA_WIFI";       // Ganti dengan nama Wi-Fi / Hotspot
const char* password = "PASSWORD_WIFI"; // Ganti dengan kata sandi Wi-Fi

WebServer server(80);
Servo myServo;

const int ldrPin = 34;    // Pin AO LDR ke GPIO 34
const int servoPin = 18;  // Pin Signal Servo ke GPIO 18
const int buzzerPin = 23; // Pin Positif Buzzer ke GPIO 23

int currentServoPos = 90;
int adcTerakhir = 0;
int cahayaTerakhir = 0;
String statusTerakhir = "";
String buzzerTerakhir = "OFF";

unsigned long waktuSebelumnya = 0;
const long interval = 2000; // (1) Selang waktu 2 detik (2000 ms)

bool isNumeric(String str) {
  if (str.length() == 0) return false;
  for (unsigned int i = 0; i < str.length(); i++) {
    if (!isDigit(str.charAt(i))) return false;
  }
  return true;
}

int bacaCahayaPersen(int &nilaiADC) {
  nilaiADC = analogRead(ldrPin);
  // Modul LDR umum: Gelap = 4095, Terang = 0
  int persen = map(nilaiADC, 4095, 0, 0, 100);
  return constrain(persen, 0, 100);
}

void cekKondisiLDR() {
  cahayaTerakhir = bacaCahayaPersen(adcTerakhir);
  String statusBaru = "";

  if (cahayaTerakhir > 70) {
    statusBaru = "CERAH";
    buzzerTerakhir = "ON (MENYALA)";
    digitalWrite(buzzerPin, HIGH); // (2) Menyalakan buzzer dengan sinyal HIGH
    currentServoPos = 180;        // Servo ke 180 derajat saat CERAH
    myServo.write(currentServoPos);
  }
  else if (cahayaTerakhir >= 35) {
    statusBaru = "SEDANG";
    buzzerTerakhir = "OFF";
    digitalWrite(buzzerPin, LOW); // Buzzer mati
    
    // (3) Servo hanya digerakkan jika status sebelumnya BUKAN "SEDANG"
    if (statusTerakhir != "SEDANG") {
      currentServoPos = 90;
      myServo.write(currentServoPos);
    }
  }
  else {
    statusBaru = "REDUP";
    buzzerTerakhir = "OFF";
    digitalWrite(buzzerPin, LOW); // Buzzer mati

    // Jika baru saja pindah ke kondisi REDUP, kembalikan servo ke 0 derajat
    if (statusTerakhir != "REDUP") {
      currentServoPos = 0;
      myServo.write(currentServoPos);
    }
  }

  statusTerakhir = statusBaru;

  // Tampilkan di Serial Monitor
  Serial.print("ADC: ");
  Serial.print(adcTerakhir);
  Serial.print(" | Cahaya: ");
  Serial.print(cahayaTerakhir);
  Serial.print("% | Kondisi: ");
  Serial.print(statusTerakhir);
  Serial.print(" | Buzzer: ");
  Serial.print(buzzerTerakhir);
  Serial.print(" | Servo: ");
  Serial.print(currentServoPos);
  Serial.println("°");
}

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>Tugas 3 - LDR + Servo + Buzzer</title>
<style>
body { font-family: Arial, sans-serif; text-align: center; margin-top: 30px; }
.badge { padding: 12px 25px; color: white; font-weight: bold; border-radius: 8px; display: inline-block; font-size: 20px; }
.REDUP { background-color: #6c757d; }
.SEDANG { background-color: #28a745; }
.CERAH { background-color: #dc3545; }
.card { border: 1px solid #ccc; padding: 20px; margin: 15px auto; width: 340px; border-radius: 10px; }
input[type=range] { width: 90%; }
</style>
</head>
<body>
<h2>Sistem Otomatis Cahaya (LDR, Servo & Buzzer)</h2>
<div class="card">
<h3>Status Sensor LDR</h3>
<p>Kecerahan: <span id="cahaya">--</span> % (ADC: <span id="adc">--</span>)</p>
<div id="statusBox" class="badge SEDANG">MEMUAT...</div>
<p>Buzzer: <b id="buzzerStatus">OFF</b></p>
</div>
<div class="card">
<h3>Kendali Servo</h3>
<p>Sudut Servo: <span id="posVal">90</span>&deg;</p>
<input type="range" min="0" max="180" value="90" id="servoSlider" oninput="updateServo(this.value)">
<p id="statusMsg"></p>
</div>
<script>
function updateServo(val) {
  document.getElementById("posVal").innerText = val;
  fetch("/servo?pos=" + val)
  .then(res => res.text().then(text => ({ status: res.status, body: text })))
  .then(res => {
    document.getElementById("statusMsg").innerText = `Status ${res.status}: ${res.body}`;
  });
}

function ambilData() {
  fetch("/data")
  .then(res => res.json())
  .then(data => {
    document.getElementById("cahaya").innerText = data.cahaya;
    document.getElementById("adc").innerText = data.adc;
    let box = document.getElementById("statusBox");
    box.innerText = data.status;
    box.className = "badge " + data.status;
    document.getElementById("buzzerStatus").innerText = data.buzzer;
    document.getElementById("posVal").innerText = data.servo;
    document.getElementById("servoSlider").value = data.servo;
  });
}
setInterval(ambilData, 2000);
ambilData();
</script>
</body>
</html>
)rawliteral";

void handleServo() {
  if (!server.hasArg("pos") || !isNumeric(server.arg("pos"))) {
    server.send(400, "text/plain", "Input tidak valid (harus angka 0-180)!");
    return;
  }
  int pos = server.arg("pos").toInt();
  if (pos < 0 || pos > 180) {
    server.send(400, "text/plain", "Sudut di luar batas (0-180)!");
    return;
  }
  currentServoPos = pos;
  myServo.write(currentServoPos);
  Serial.println("[WEB] Servo diubah manual ke: " + String(currentServoPos) + "°");
  server.send(200, "text/plain", "Servo diatur ke " + String(pos) + " derajat");
}

void handleData() {
  String json = "{\"cahaya\":" + String(cahayaTerakhir) +
                ",\"adc\":" + String(adcTerakhir) +
                ",\"status\":\"" + statusTerakhir + "\"" +
                ",\"buzzer\":\"" + buzzerTerakhir + "\"" +
                ",\"servo\":" + String(currentServoPos) + "}";
  server.send(200, "application/json", json);
}

void setup() {
  Serial.begin(115200);
  analogReadResolution(12);
  
  pinMode(buzzerPin, OUTPUT); // (4) Set pin buzzer sebagai OUTPUT
  digitalWrite(buzzerPin, LOW);
  
  myServo.attach(servoPin);
  myServo.write(currentServoPos);
  
  WiFi.begin(ssid, password);
  Serial.print("Menghubungkan ke WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nTerhubung! IP Address: " + WiFi.localIP().toString());
  
  cekKondisiLDR(); // Baca kondisi awal
  
  server.on("/", []() { server.send(200, "text/html", index_html); });
  server.on("/servo", handleServo); // (5) Endpoint URL untuk mengendalikan slider servo
  server.on("/data", handleData);
  server.begin();
}

void loop() {
  server.handleClient();
  
  unsigned long waktuSekarang = millis();
  if (waktuSekarang - waktuSebelumnya >= interval) {
    waktuSebelumnya = waktuSekarang;
    cekKondisiLDR(); // (6) Memanggil fungsi rutin pengecekan kondisi LDR
  }
}
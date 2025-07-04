#include <SPI.h>             // Library untuk komunikasi SPI (digunakan oleh MFRC522)
#include <MFRC522.h>           // Library untuk modul MFRC522
#include <Wire.h>              // Library untuk komunikasi I2C (digunakan oleh LCD)
#include <LiquidCrystal_I2C.h> // Library untuk LCD I2C

// Definisikan pin SDA (SS) dan RST untuk MFRC522
#define SS_PIN 10
#define RST_PIN 9

// Inisialisasi objek MFRC522
MFRC522 rfid(SS_PIN, RST_PIN);

// Inisialisasi objek LiquidCrystal_I2C
// Parameter: (alamat I2C, jumlah kolom, jumlah baris)
// Alamat I2C umum: 0x27 atau 0x3F. Coba 0x27 dulu. Jika tidak berfungsi, coba 0x3F.
// Untuk LCD 16x2: LiquidCrystal_I2C lcd(0x27, 16, 2);
// Untuk LCD 20x4: LiquidCrystal_I2C lcd(0x27, 20, 4);
LiquidCrystal_I2C lcd(0x27, 16, 2); // Sesuaikan dengan ukuran LCD Anda (contoh: 16 kolom, 2 baris)

void setup() {
  Serial.begin(9600); // Mulai komunikasi serial untuk debugging (opsional)
  while (!Serial);    // Tunggu hingga Serial Monitor siap

  SPI.begin();        // Inisialisasi komunikasi SPI
  rfid.PCD_Init();    // Inisialisasi modul MFRC522

  // --- PERBAIKAN: Menggunakan lcd.begin() ---
  lcd.begin();   // Inisialisasi LCD dengan menentukan jumlah kolom dan baris
                      // Ubah (16, 2) sesuai ukuran LCD Anda (misal: (20, 4) untuk 20x4 LCD)
  // --- END PERBAIKAN ---

  lcd.backlight(); // Nyalakan backlight LCD

  lcd.clear();
  lcd.setCursor(0, 0); // Kolom 0, Baris 0
  lcd.print("RFID Reader");
  lcd.setCursor(0, 1); // Kolom 0, Baris 1
  lcd.print("Tempelkan Tag...");

  Serial.println("RFID Reader Siap!");
  Serial.println("Tempelkan tag RFID pada reader...");
}

void loop() {
  // Mencari tag baru
  if ( ! rfid.PICC_IsNewCardPresent()) {
    return; // Tidak ada tag baru, keluar dari loop()
  }

  // Membaca UID dari tag
  if ( ! rfid.PICC_ReadCardSerial()) {
    return; // Gagal membaca UID, keluar dari loop()
  }

  // Hapus layar LCD dan tampilkan UID
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("UID:");

  lcd.setCursor(0, 1);
  for (byte i = 0; i < rfid.uid.size; i++) {
    if (rfid.uid.uidByte[i] < 0x10) {
      lcd.print("0"); // Tambahkan '0' di depan jika kurang dari 16 (heksadesimal)
    }
    lcd.print(rfid.uid.uidByte[i], HEX); // Cetak byte UID dalam format heksadesimal
    lcd.print(" "); // Spasi antar byte untuk keterbacaan
  }

  // Cetak juga ke Serial Monitor untuk debugging
  Serial.print("UID Tag: ");
  for (byte i = 0; i < rfid.uid.size; i++) {
    Serial.print(rfid.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(rfid.uid.uidByte[i], HEX);
  }
  Serial.println();

  // Menghentikan pembacaan tag saat ini untuk menghindari duplikasi
  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();

  // Beri sedikit jeda sebelum siap membaca tag berikutnya
  delay(1000); // Jeda 1 detik
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Tempelkan Tag...");
  lcd.setCursor(0, 1);
  lcd.print("Lainnya...");
}
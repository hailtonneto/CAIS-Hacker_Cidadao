// Programa: Leitura e gravacao de cartoes RFID com LCD I2C
// Autor original: MakerHero | Adaptado para LCD I2C

#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Pinos Reset e SS módulo MFRC522
#define SS_PIN 10
#define RST_PIN 9
MFRC522 mfrc522(SS_PIN, RST_PIN);

// LCD I2C - Endereço 0x27 (pode ser 0x3F em alguns modelos)
LiquidCrystal_I2C lcd(0x27, 16, 2);

#define pino_botao_le A2
#define pino_botao_gr A3

MFRC522::MIFARE_Key key;

void setup()
{
  pinMode(pino_botao_le, INPUT);
  pinMode(pino_botao_gr, INPUT);
  Serial.begin(9600);
  SPI.begin();
  mfrc522.PCD_Init();

  lcd.init();        // Inicializa o LCD I2C
  lcd.backlight();   // Liga a luz de fundo
  mensageminicial();

  for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF;
}

void loop()
{
  if (digitalRead(pino_botao_le))
  {
    lcd.clear();
    Serial.println("Modo leitura selecionado");
    lcd.setCursor(2, 0);
    lcd.print("Modo leitura");
    lcd.setCursor(3, 1);
    lcd.print("selecionado");
    while (digitalRead(pino_botao_le) == 1) {}
    delay(3000);
    modo_leitura();
  }

  if (digitalRead(pino_botao_gr))
  {
    lcd.clear();
    Serial.println("Modo gravacao selecionado");
    lcd.setCursor(2, 0);
    lcd.print("Modo gravacao");
    lcd.setCursor(3, 1);
    lcd.print("selecionado");
    while (digitalRead(pino_botao_gr) == 1) {}
    delay(3000);
    modo_gravacao();
  }
}

void mensageminicial()
{
  Serial.println("\nSelecione o modo leitura ou gravacao...\n");
  lcd.clear();
  lcd.print("Selecione o modo");
  lcd.setCursor(0, 1);
  lcd.print("leitura/gravacao");
}

void mensagem_inicial_cartao()
{
  Serial.println("Aproxime o seu cartao do leitor...");
  lcd.clear();
  lcd.print(" Aproxime o seu");
  lcd.setCursor(0, 1);
  lcd.print("cartao do leitor");
}

void modo_leitura()
{
  mensagem_inicial_cartao();
  while (!mfrc522.PICC_IsNewCardPresent()) { delay(100); }
  if (!mfrc522.PICC_ReadCardSerial()) return;

  Serial.print("UID da tag : ");
  String conteudo = "";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
    conteudo.concat(String(mfrc522.uid.uidByte[i]<0x10 ? " 0" : " "));
    conteudo.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  Serial.println();

  byte sector = 1, blockAddr = 4, trailerBlock = 7;
  byte buffer[18], size = sizeof(buffer);
  byte status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("PCD_Authenticate() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  status = mfrc522.MIFARE_Read(blockAddr, buffer, &size);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("MIFARE_Read() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  lcd.clear();
  lcd.setCursor(0, 0);
  for (byte i = 1; i < 16; i++) {
    Serial.print(char(buffer[i]));
    lcd.write(char(buffer[i]));
  }
  Serial.println();

  sector = 0; blockAddr = 1; trailerBlock = 3;
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("PCD_Authenticate() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  status = mfrc522.MIFARE_Read(blockAddr, buffer, &size);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("MIFARE_Read() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  lcd.setCursor(0, 1);
  for (byte i = 0; i < 16; i++) {
    Serial.print(char(buffer[i]));
    lcd.write(char(buffer[i]));
  }
  Serial.println();

  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
  delay(3000);
  mensageminicial();
}

void modo_gravacao()
{
  mensagem_inicial_cartao();
  while (!mfrc522.PICC_IsNewCardPresent()) { delay(100); }
  if (!mfrc522.PICC_ReadCardSerial()) return;

  Serial.print(F("UID do Cartao: "));
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
  }

  Serial.print(F("\nTipo do PICC: "));
  byte piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
  Serial.println(mfrc522.PICC_GetTypeName(piccType));

  byte buffer[34], block, status, len;

  Serial.setTimeout(20000L);
  Serial.println(F("Digite o sobrenome, em seguida o caractere #"));
  lcd.clear();
  lcd.print("Digite o sobreno");
  lcd.setCursor(0, 1);
  lcd.print("me + #");
  len = Serial.readBytesUntil('#', (char *) buffer, 30);
  for (byte i = len; i < 30; i++) buffer[i] = ' ';

  block = 1;
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, block, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.println(F("Falha na autenticacao"));
    return;
  }
  status = mfrc522.MIFARE_Write(block, buffer, 16);
  if (status != MFRC522::STATUS_OK) {
    Serial.println(F("Falha na gravacao (bloco 1)"));
    return;
  }

  block = 2;
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, block, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) return;
  status = mfrc522.MIFARE_Write(block, &buffer[16], 16);
  if (status != MFRC522::STATUS_OK) return;

  Serial.println(F("Digite o nome, em seguida o caractere #"));
  lcd.clear();
  lcd.print("Digite o nome e");
  lcd.setCursor(0, 1);
  lcd.print("em seguida #");
  len = Serial.readBytesUntil('#', (char *) buffer, 20);
  for (byte i = len; i < 20; i++) buffer[i] = ' ';

  block = 4;
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, block, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) return;
  status = mfrc522.MIFARE_Write(block, buffer, 16);
  if (status != MFRC522::STATUS_OK) return;

  block = 5;
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, block, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) return;
  status = mfrc522.MIFARE_Write(block, &buffer[16], 16);
  if (status != MFRC522::STATUS_OK) return;

  Serial.println(F("Dados gravados com sucesso!"));
  lcd.clear();
  lcd.print("Gravacao OK!");

  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
  delay(5000);
  mensageminicial();
}

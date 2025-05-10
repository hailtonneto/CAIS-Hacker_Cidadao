#include <Wire.h> //INCLUSÃO DA BIBLIOTECA NECESSÁRIA
#include <SPI.h> //INCLUSÃO DE BIBLIOTECA
#include <MFRC522.h> //INCLUSÃO DE BIBLIOTECA
 
#define SS_PIN 10 //PINO SDA
#define RST_PIN 9 //PINO DE RESET
 
MFRC522 rfid(SS_PIN, RST_PIN); //PASSAGEM DE PARÂMETROS REFERENTE AOS PINOS
 
 
const int ledVerde = 3; //PINO DIGITAL REFERENTE AO LED VERDE
const int ledVermelho = 2; //PINO DIGITAL REFERENTE AO LED VERMELHO
 
void setup(){
 
  Wire.begin(); //INICIALIZA A BIBLIOTECA WIRE
  SPI.begin(); //INICIALIZA O BARRAMENTO SPI
  rfid.PCD_Init(); //INICIALIZA MFRC522
  
  pinMode(ledVerde, OUTPUT); //DEFINE O PINO COMO SAÍDA
  pinMode(ledVermelho, OUTPUT); //DEFINE O PINO COMO SAÍDA
  
  digitalWrite(ledVerde, LOW); //LED INICIA DESLIGADO
  digitalWrite(ledVermelho, LOW); //LED INICIA DESLIGADO
}
 
void loop() {
  leituraRfid(); //CHAMA A FUNÇÃO RESPONSÁVEL PELA VALIDAÇÃO DA TAG RFID
  
  digitalWrite(ledVerde, HIGH); //Início da função para funcionamento dos LED's
  digitalWrite(ledVermelho, LOW);
  digitalWrite(buzzer, LOW); // Garante que o buzzer esteja desligado
  delay(15000);

  // Liga o LED vermelho por 15 segundos
  digitalWrite(ledVerde, LOW);
  digitalWrite(ledVermelho, HIGH);

    // Durante os primeiros 12 segundos, bip a cada quase 2 segundos se o botão estiver pressionado
  for (int i = 0; i < 6; i++) {  
    if (strID.indexOf("27:41:AA:AB") >= 0) {
      tone(buzzer, 1000);   
      delay(100);
      noTone(buzzer);      
      delay(1600);         
    } else {
      noTone(buzzer);
      delay(2000);
    }
  }
  // Últimos 3 segundos com o buzzer desligado
  noTone(buzzer);
  delay(3000);

}
 
//FUNÇÃO DE VALIDAÇÃO DA TAG RFID
void leituraRfid(){
  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) //VERIFICA SE O CARTÃO PRESENTE NO LEITOR É DIFERENTE DO ÚLTIMO CARTÃO LIDO. CASO NÃO SEJA, FAZ
    return; //RETORNA PARA LER NOVAMENTE
 
  /***INICIO BLOCO DE CÓDIGO RESPONSÁVEL POR GERAR A TAG RFID LIDA***/
  String strID = ""; 
  for (byte i = 0; i < 4; i++) {
    strID +=
    (rfid.uid.uidByte[i] < 0x10 ? "0" : "") +
    String(rfid.uid.uidByte[i], HEX) +
    (i!=3 ? ":" : "");
  }
  strID.toUpperCase();
/***FIM DO BLOCO DE CÓDIGO RESPONSÁVEL POR GERAR A TAG RFID LIDA***/
 
  /* CHAVES: DESATIVEI OS COMANDOS QUE RESULTAVAM NA ALTERAÇÃO DO LED -> TUDO QUE ESTÁ COMENTADO ABAIXO

  //O ENDEREÇO "27:41:AA:AB" DEVERÁ SER ALTERADO PARA O ENDEREÇO DA SUA TAG RFID QUE CAPTUROU ANTERIORMENTE
  if (strID.indexOf("27:41:AA:AB") >= 0) { //SE O ENDEREÇO DA TAG LIDA FOR IGUAL AO ENDEREÇO INFORMADO, FAZ
    digitalWrite(ledVerde, HIGH); //LIGA O LED VERDE
    delay(3000); //INTERVALO DE 4 SEGUNDOS
    digitalWrite(ledVerde, LOW); //DESLIGA O LED VERDE
  }else{ //SENÃO, FAZ (CASO A TAG LIDA NÃO SEJÁ VÁLIDA)
    digitalWrite(ledVermelho, HIGH); //LIGA O LED VERMELHO
    delay(3000); ////INTERVALO DE 6 SEGUNDOS
    digitalWrite(ledVermelho, LOW); //DESLIGA O LED VERDE
  }
  */

  rfid.PICC_HaltA(); //PARADA DA LEITURA DO CARTÃO
  rfid.PCD_StopCrypto1(); //PARADA DA CRIPTOGRAFIA NO PCD
  }
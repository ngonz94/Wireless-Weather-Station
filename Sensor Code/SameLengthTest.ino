void setup() {
  // put your setup code here, to run once:

Serial.begin(9600);

}

void loop() {
  // put your main code here, to run repeatedly:
  int b1 = 1;
  int b2 = 1;
  int b3 = 1;
  int b4 = 0;
  int b5 = 0;
  int b6 = 0;
  int b7 = 0;
  int b8 = 1;

  uint8_t dir = b1 * 1 + b2 * 2 + b3 * 4 + b4 * 8 + b5 * 16 + b6 * 32 + b7 * 64 + b8 * 128;



  //  String direct = string(dir);

  int dirlen = 3;
  String direct = String(dir);

  for (int i = direct.length(); i < dirlen; i++) {

    direct += " ";
  }

Serial.println(direct);

}

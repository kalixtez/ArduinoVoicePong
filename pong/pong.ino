#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>
#include <SoftwareSerial.h>

Adafruit_PCD8544 display = Adafruit_PCD8544(7, 6, 5, 4, 3);
//SoftwareSerial wtvSerial(10, 11); // RX, TX pins connected to WTV020M01 module
 class Player
{
  public:
  int pos_x;
  int pos_y;

  int input_x;
  int input_y;

  bool joystick;

  float vertical_speed;

  int score;

  int size_x = 3;
  int size_y = 10;

  int last[3];

  void update_pos(int elapsed_time)
  {

    if(joystick)
      vertical_speed = get_speed();
    else
      vertical_speed = audio_speed();
    if(pos_y>=42 && vertical_speed>0){
      pos_y = 42;
    }
    else if(pos_y<=0 && vertical_speed<0){
      pos_y = 0;
    }
    else
    pos_y += vertical_speed*elapsed_time;


  }

  float audio_speed()
  {
    static int i = 0;

    int a = analogRead(input_y);
    last[i%3] = a;
    i++;

    if(a > 1000)
      return 2.5;
    else if(a <= 1000 && last[0] < 1000 && last[1] < 1000 && last[2] < 1000)
      return -2.5;
  }

  int get_speed()
  {
    
    vertical_speed = analogRead(input_y);
    if(vertical_speed>=824){
      vertical_speed = 2.5;
    }
    else if(vertical_speed<=200){
      vertical_speed = -2.5;
    }

    else if(vertical_speed<=612 && vertical_speed>=412){
      vertical_speed = 0;
    }

    else{
    vertical_speed = ((vertical_speed/1024)*2 - 1)*2.5;
    }
    return vertical_speed;
  }
  Player(int input_x, int input_y, int cord_x, int cord_y, bool joystick){
    this->input_x = input_x;
    this->input_y = input_y;
    this->pos_x = cord_x;
    this->pos_y = cord_y;
    this->joystick = joystick;
  }

  int getPos_x(){
    return pos_x;
  }
  int getPos_y(){
    return pos_y;
  }
  int getSize_x(){
    return size_x;
  }
  int getSize_y(){
    return size_y;
  }

};

bool game = true;
bool last_win = 0;
// Pin del altavoz
const int speakerPin = 11;

// Definición de las frecuencias de las notas musicales
const int noteC = 262;
const int noteE = 330;
const int noteG = 392;
const int noteA = 440;
const int noteB = 494;

// Duración de cada nota musical en milisegundos
const int noteDuration = 250;

class Ball{
  public:
  float pos_x;
  float pos_y;
  int radius;
  float speed_x;
  float speed_y;
  Player& player1;
  Player& player2;
  

  Ball(float posx, float posy, int ballRadius, float speedx, float speedy, Player& p1, Player& p2):
    player1(p1), player2(p2){
    this-> pos_x = posx;
    this-> pos_y = posy;
    this-> radius = ballRadius;
    this-> speed_x = speedx;
    this-> speed_y = speedy;
    
  }
  void update_pos(){
    pos_x += speed_x;
    pos_y += speed_y;

    if(pos_x - radius < 0){
      //Aquí debemos de sumar un punto para el jugador2 y reiniciar el juego
      
      game = false;
      player2.score++;
      last_win = 1;
      deadMusic();
      
    }
    if(pos_x >=display.width()){
      //Aquí debemos de sumar un punto para el jugador1 y reiniciar el juego
      
      game = false;
      player1.score++;
      last_win = 0;
      deadMusic();
      
    }
    if(pos_y - radius < 0 || pos_y + radius >= display.height()){
      speed_y = -speed_y;
      tone(10,noteC,125);
    }
    if(((pos_x >= player1.getPos_x() && pos_x <= player1.getPos_x() + player1.getSize_x() && pos_y >= player1.getPos_y() && pos_y <= player1.getPos_y() +player1.getSize_y())) ||
    (pos_x >= player2.getPos_x() && pos_x <= player2.getPos_x() + player2.getSize_x() && pos_y >= player2.getPos_y() && pos_y <= player2.getPos_y() + player2.getSize_y())){
      speed_x = -speed_x;
      tone(10,noteC,125);
    }
  }
  void ball_draw(){
    display.fillCircle((int)pos_x,(int)pos_y,radius, 1);
  }
  void deadMusic(){
    tone(speakerPin, noteE, noteDuration);
    delay(noteDuration);
    tone(speakerPin, noteG, noteDuration);
    delay(noteDuration);
    tone(speakerPin, noteA, noteDuration);
    delay(noteDuration);
    noTone(speakerPin);
    delay(noteDuration);

  // Pausa entre cada reproducción de la melodía
  delay(1000);
  }

};

Player jugador1(A0,A1,1,21, 1);
Player jugador2(A4,A5,81,21, 1);
Player jugador3(1,A2,1,41, 0);
Player jugador4(1,A3,81,41, 0);

Ball balon(display.width() /2,display.height() /2,2,1,1,jugador1,jugador2);
Ball balon2(display.width() /2,display.height() /2,2,1,1,jugador3,jugador4);

void setup() {
  //Serial.begin(9600);
  display.begin();
  display.clearDisplay();
  randomSeed(analogRead(A0));
  //wtvSerial.begin(9600);
  
}

  bool menu = true;
  int option = 0;

void loop() {

  while(menu)
  {
    display.clearDisplay();

    display.setTextSize(1);
    display.setTextColor(1);
    display.setCursor(display.width()/2 - 20, display.height()/2 - 10);

    display.write("Joystick");

    display.setCursor(display.width()/2 - 20, display.height()/2);
    display.write("Voice");

    display.drawTriangle(17, 14 + option*10, 17, 20 + option*10, 21, 17 + option*10, 1);

    if(analogRead(A0) > 700 && option == 0)
      option = 1;
    else if(analogRead(A0) < 300 && option == 1)
      option = 0;

    if(!digitalRead(8))
    {
      menu = false;
      break;
    }
  
    display.display();
    delay(10);
  }

  if(!option)
  {
    game = true;

    while(game)
    {
      jugador1.update_pos(1);
      jugador2.update_pos(1);

      display.clearDisplay();
      balon.update_pos();
      balon.ball_draw();

      display.fillRect(jugador1.pos_x, jugador1.pos_y, jugador1.size_x, jugador1.size_y,1);
      display.fillRect(jugador2.pos_x, jugador2.pos_y, jugador2.size_x, jugador2.size_y,1);

      display.display();
      delay(10);
    }

      balon.pos_x = display.width()/2;
      balon.pos_y = display.height()/2;

      bool up = random(0, 2);

      float angle;

      if(up)
      {
        angle = random(40, 50);
      }
      else
        angle = random(-50, -40);
        
      balon.speed_x = cos(radians(angle)) * (last_win ? -2 : 2);
      balon.speed_y = sin(radians(angle)) * 2;

      display.clearDisplay();
      balon.ball_draw();
      
      display.fillRect(jugador1.pos_x, jugador1.pos_y, jugador1.size_x, jugador1.size_y,1);
      display.fillRect(jugador2.pos_x, jugador2.pos_y, jugador2.size_x, jugador2.size_y,1);

      display.display();

      delay(1000);
    }
    else if(option){
          game = true;

    while(game)
    {
      jugador3.update_pos(1);
      jugador4.update_pos(1);

      display.clearDisplay();
      balon2.update_pos();
      balon2.ball_draw();

      display.fillRect(jugador3.pos_x, jugador3.pos_y, jugador3.size_x, jugador3.size_y,1);
      display.fillRect(jugador4.pos_x, jugador4.pos_y, jugador4.size_x, jugador4.size_y,1);

      display.display();
      Serial.println(analogRead(jugador3.input_y));
      Serial.println(analogRead(jugador4.input_y));
      delay(10);
    }

      balon2.pos_x = display.width()/2;
      balon2.pos_y = display.height()/2;

      bool up = random(0, 2);

      float angle;

      if(up)
      {
        angle = random(40, 50);
      }
      else
        angle = random(-50, -40);
        
      balon2.speed_x = cos(radians(angle)) * (last_win ? -2 : 2);
      balon2.speed_y = sin(radians(angle)) * 2;

      display.clearDisplay();
      balon2.ball_draw();
      
      display.fillRect(jugador3.pos_x, jugador3.pos_y, jugador3.size_x, jugador3.size_y,1);
      display.fillRect(jugador4.pos_x, jugador4.pos_y, jugador4.size_x, jugador4.size_y,1);

      display.display();

      delay(1000);
    }
}

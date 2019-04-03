
//Драйвер моторов HG7881 (два Н-моста):
byte Leg_R2_pin = 5;  //B-1A  было 6
byte Leg_R1_pin = 6;  //B-1B  было 9
byte Leg_L1_pin = 3;  //A-1B  было 10
byte Leg_L2_pin = 11; //A-1A  было 11

//Ультразвуковые датчики расстояния HC-SR04:
byte EchoPin1 = 7;  //Вывод Echo переднего сенсора
byte TrigPin1 = 8;  //Вывод Trig переднего сенсора
word duration1, cm1; //Переменные для считывания расстояния переднего сенсора
byte EchoPin2 = 12;   //Вывод Echo заднего сенсора
byte TrigPin2 = 13;   //Вывод Trig заднего сенсора
word duration2, cm2;   //Переменные для считывания расстояния заднего сенсора

//"МАКРОСЫ"
byte forvard = 1;
byte back = 0;

byte threshold =     15; //Порог дистанции до препятствия, см
byte threshold_min = 10; //Опасное расстояние до препятствия, см
int turn90_time = 700;   //Время поворота на 90 град, мс

byte AI_state = 0; //Состояние автомата алгоритма Alhoritm_AI()
unsigned long delay_time1 = 0;

void setup() 
{
  Serial.begin(9600);
  randomSeed(analogRead(0));
  
  pinMode(Leg_R1_pin, OUTPUT); 
  pinMode(Leg_R2_pin, OUTPUT);      
  pinMode(Leg_L1_pin, OUTPUT);
  pinMode(Leg_L2_pin, OUTPUT);      

  // Конфигурируем выводы ардуины для работы с датчиками HC-SR04:
  pinMode(TrigPin1, OUTPUT);   // Триггер на выход
  pinMode(EchoPin1, INPUT);    // Эхо на вход
  digitalWrite(TrigPin1, LOW); // Триггер сразу инициализируем в ноль
  pinMode(TrigPin2, OUTPUT);     // Триггер на выход
  pinMode(EchoPin2, INPUT);      // Эхо на вход
  digitalWrite(TrigPin2, LOW);   // Триггер сразу инициализируем в ноль
}

void loop() 
{
  SENSOR_process();
  
  //Alhoritm_RECTANGLE();
  //Alhoritm_FEAR
  Alhoritm_AI(150);
   
  delay(50);
}
//----------------------------------------------------//
//----------------------------------------------------//
//----------------------------------------------------//
void Alhoritm_AI(AI_Speed)
{ //Алгоритм "Искусственный интеллект"
  switch (AI_state)
    {
      case 0: if ((cm1<threshold_min)||(cm2<threshold_min)) {AI_state = 1; break;} //обработка опасного расстояния
              GO_forvard(AI_Speed);    //Движение вперёд  
              if (cm1<threshold) 
                {
                 STOP();
                 if (random(0,2)) {GO_left(AI_Speed);} else {GO_right(AI_Speed);}
                 AI_state = 2; 
                 delay_time1 = millis()+turn90_time;
                } 
              break;
      case 1: if ((cm1<threshold_min)&&(cm2>threshold_min)) {Go_back(AI_Speed); break;} //отойти назад
              else 
              if ((cm2<threshold_min)&&(cm1>threshold_min)) {GO_forvard(AI_Speed); break;} //отойти вперёд
              else
              if ((cm1<threshold_min)&&(cm2<threshold_min))  //Если с обеих сторон опасное расстояние:
                 {
                  if (cm1>cm2)  {GO_back(AI_Speed);}    //отойти назад
                  if (cm2>cm1)  {GO_forvard(AI_Speed);} //отойти вперёд
                 } else {AI_state = 0;}
              break;   
      case 2: if (millis()>=delay_time1) {AI_state = 0;} 
              break;
      case 3:
    }
  
}
//----------------------------------------------------//
void Alhoritm_RECTANGLE(void)
{ //Алгоритм "КВАДРАТ"
  GO(forvard,150,forvard,150);
  delay(3000);
  GO(forvard,150,back,150);
  delay(turn90_time);
}
//----------------------------------------------------//
void Alhoritm_FEAR(void)
{ //Алгоритм ухода от приближающегося препятствия (вперёд/назад)
  if ((cm1<20)&&(cm2>20)) {GO_back(100);} 
  if ((cm2<20)&&(cm1>20)) {GO_forvard(100);} 
  if ((cm1<20)&&(cm2<20)) 
     {
      if (cm1>cm2)  {GO_back(100);}
      if (cm2>cm1)  {GO_forvard(100);}
      if (cm1==cm2) {STOP();}
     }  
  if ((cm1>20)&&(cm2>20)) {STOP();}  
}
//----------------------------------------------------//
void SENSOR_process(void)
{ //Алгоритм опроса сенсоров HC-SR04:::  
  digitalWrite(TrigPin1, HIGH); // Подаём сигнал на тригер...
  delayMicroseconds(10);        // ...длиной 10 мкс
  digitalWrite(TrigPin1, LOW);  
  // Считываем длину "ответного" импульса с ножки Echo:
  duration1 = pulseIn(EchoPin1, HIGH); 
  cm1 = constrain((duration1/58),2,200); // Переводим длину в сантиметры
  //-------------------------//
  digitalWrite(TrigPin2, HIGH); // Подаём сигнал на тригер...
  delayMicroseconds(10);        // ...длиной 10 мкс
  digitalWrite(TrigPin2, LOW);  
  // Считываем длину "ответного" импульса с ножки Echo:
  duration2 = pulseIn(EchoPin2, HIGH); 
  cm2 = constrain((duration2/58),2,200); // Переводим длину в сантиметры
}
//----------------------------------------------------//
void GO(byte L_Direction, byte L_Speed, byte R_Direction, byte R_Speed)
{
  if (L_Direction)
    {
      analogWrite(Leg_L1_pin, L_Speed);
      digitalWrite(Leg_L2_pin, LOW);  
    } else 
    {
      digitalWrite(Leg_L1_pin, LOW);
      analogWrite(Leg_L2_pin, L_Speed);   
    }
  if (R_Direction)
    {
      analogWrite(Leg_R1_pin, L_Speed);
      digitalWrite(Leg_R2_pin, LOW);  
    } else 
    {
      digitalWrite(Leg_R1_pin, LOW);
      analogWrite(Leg_R2_pin, L_Speed);   
    }  
}
//----------------------------------------------------//
void GO_forvard(byte Speed)
{GO(forvard,Speed,forvard,Speed); }
//----------------------------------------------------//
void GO_back(byte Speed)
{GO(back,Speed,back,Speed); }
//----------------------------------------------------//
void GO_left(byte Speed)
{GO(back,Speed,forvard,Speed); }
//----------------------------------------------------//
void GO_right(byte Speed)
{GO(forvard,Speed,back,Speed); }
//----------------------------------------------------//
void STOP(void)
{
    digitalWrite(Leg_L1_pin, LOW);
    digitalWrite(Leg_L2_pin, LOW);
    digitalWrite(Leg_R1_pin, LOW);
    digitalWrite(Leg_R2_pin, LOW);
}
//----------------------------------------------------//


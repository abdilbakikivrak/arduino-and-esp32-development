//rgbledsolision

const int k_led =11;
const int y_led=10;
const int m_led=9;


void setup() {
  pinMode(k_led,OUTPUT);
  pinMode(y_led,OUTPUT);
  pinMode(m_led,OUTPUT);
}

void loop()
{
 digitalWrite(k_led ,LOW);
 digitalWrite(y_led ,HIGH);
  digitalWrite(m_led ,HIGH);
 
   delay(400);
   
    digitalWrite(k_led ,HIGH);
 digitalWrite(y_led ,LOW);
  digitalWrite(m_led ,HIGH);
  
   delay(400);
   
    digitalWrite(k_led ,HIGH);
 digitalWrite(y_led ,HIGH);
  digitalWrite(m_led ,LOW);
  
   delay(400);
   
   
}

#include <math.h>

int blu = 48;
int blm = 50;
int bll = 52;
int bru = 49;
int brm = 51;
int brl = 53;
int led_ru = 26;
int led_rm = 24;
int led_rl = 22;
int led_lu = 28;
int led_lm = 30;
int led_ll = 32;
int Set = 46;
int Display = 47;
int deg_rad = 40;

int NEXT = 36;
int Execute = 42;

int Quadratic = 44;
int Integral = 38;
int Trigonometry = 34; //34 45
int Graph = 43;

int enable = 25;

int buttonPin0 = 35;
int buttonPin1 = 37;
int buttonPin2 = 39;

int fill_graph = 41;
int clear_graph = 45;

String options[5] = {"Sinusoidal", "Polynomial", "Linear", "Conics","Miscellaneous"};
int cur_lev = 0;
int cur_pos[2] = {0,0};
String menu[5][5] = {{"sin(x)","cos(x)","tan(x)","sec(x)","sin(2x)"},{"x^3","x^2","-x^2","x^2-3","x^2+x"},{"x","-x","2x","4x","x+3"},{"Parabola","Ellipse","Circle","Hyperbola","test_conic"},{"ln(x)","e^x","1/x","sqrt(x)","abs(x)"}};

char fdgraph[16][20];
char fdtransfer;

byte fdstart_transmission[]={0x02};
byte fdend_transmission[]={0x03};

void fdgraph_fill()
{
  for(int i=15;i>=2;i--)
  {
    for(int j=3;j<7;j++)
    fdgraph[j][i]='X';
  }
}

//checkerboard
void fdgraph_test1()
{
  for(int i=19;i>=0;i=i-2)
  {
    for(int j=0;j<16;j=j+2)
    fdgraph[j][i]='X';
  }
}

//stripes1
void fdgraph_test2()
{
  for(int i=19;i>=0;i=i-2)
  {
    for(int j=0;j<16;j++)
    fdgraph[j][i]='X';
  }
}

//stripes2
void fdgraph_test3()
{
  for(int i=19;i>=0;i--)
  {
    for(int j=0;j<16;j=j+2)
    fdgraph[j][i]='X';
  }
}

void fdgraph_to_string(){
   unsigned int fdnibble=0x00;
   char fdnibble_char;
   String fddata="1028";
   byte sum=0x30+0x31+0x32+0x38;
   byte checksum=0x00;
   String str_checksum;
   for(int i=19;i>=0;i--){
    for(int j=0;j<16;j+=4){
      fdnibble=0;
      if (fdgraph[j][i]=='X') {fdnibble+=8;}
      if (fdgraph[j+1][i]=='X') {fdnibble+=4;}
      if (fdgraph[j+2][i]=='X') {fdnibble+=2;}
      if (fdgraph[j+3][i]=='X') {fdnibble+=1;}
      switch(fdnibble){
        case 0:
          fdnibble_char = '0';
          break;
        case 1:
          fdnibble_char = '1';
          break;
        case 2:
          fdnibble_char = '2';
          break;
        case 3:
          fdnibble_char = '3';
          break;
        case 4:
          fdnibble_char = '4';
          break;
        case 5:
          fdnibble_char = '5';
          break;
        case 6:
          fdnibble_char = '6';
          break;
        case 7:
          fdnibble_char = '7';
          break;
        case 8:
          fdnibble_char = '8';
          break;
        case 9:
          fdnibble_char = '9';
          break;   
        case 10:
          fdnibble_char='A';
          break;
        case 11:
          fdnibble_char='B';
          break;
         case 12:
          fdnibble_char='C';
          break;  
         case 13:
          fdnibble_char='D';
          break;
        case 14:
          fdnibble_char='E';
          break;
         case 15:
          fdnibble_char='F';
          break;
         default:
          fdnibble_char=fdnibble;
          break; 
        } 
      fddata+=fdnibble_char;
      sum+=fdnibble_char;
    }
    }
   sum+=0x03;
   checksum=sum^0xFF;
   checksum+=0x01;   
   str_checksum=String(checksum,HEX);

   Serial3.write(fdstart_transmission,1);
   Serial3.print(fddata);
   Serial3.write(fdend_transmission,1);
   Serial3.print(str_checksum);
}

void setup() 
{
  pinMode(led_ll, OUTPUT);  
  pinMode(led_lm, OUTPUT);  
  pinMode(led_lu, OUTPUT);  
  pinMode(led_rl, OUTPUT);  
  pinMode(led_rm, OUTPUT);  
  pinMode(led_ru, OUTPUT);   
  pinMode(blu, INPUT); 
  pinMode(blm, INPUT);
  pinMode(bll, INPUT); 
  pinMode(bru, INPUT);
  pinMode(brm, INPUT); 
  pinMode(brl, INPUT);
  pinMode(NEXT, INPUT);
  pinMode(Display, INPUT);
  pinMode(Set, INPUT);
  pinMode(deg_rad, INPUT);
  pinMode(Execute, INPUT);
  pinMode(Quadratic, INPUT);
  pinMode(Integral, INPUT);
  pinMode(Trigonometry, INPUT);
  pinMode(Graph, INPUT);
  //pinMode(NEXT, INPUT);
  pinMode(enable, HIGH);

  String text1 = "Test time :)";
 
  pinMode(buttonPin0, INPUT);
  pinMode(buttonPin1, INPUT);
  pinMode(buttonPin2, INPUT);
  
  pinMode(fill_graph, INPUT);
  pinMode(clear_graph, INPUT);
  
  Serial.begin(9600);
  Serial3.begin(4800);
}

struct BrailleCell
{
  bool lu = LOW;
  bool lm = LOW;
  bool ll = LOW;
  bool ru = LOW;
  bool rm = LOW;
  bool rl = LOW;
};

bool NEXT_STATE = LOW;
bool Set_state = LOW;
bool Display_state = LOW;
bool Execute_state = LOW;
bool Quadratic_state = LOW;
bool Integral_state = LOW;
bool Trigonometry_state = LOW;
bool Graph_state = LOW;
bool deg_rad_state = LOW;
bool fill_graph_state = LOW;
bool clear_graph_state = LOW;

//button state and value transfer objects
BrailleCell state;
BrailleCell trans;

//main equation values
BrailleCell c;
BrailleCell c_array[30];
int n = 0;

//dictionary;
//convert braille cell to number for arithmetic
double Braille_to_number(bool lu, bool lm, bool ll, bool ru, bool rm, bool rl)
{
  if(lu == true && lm == false && ll == false && ru == false && rm == false && rl == false)
  {
    return 1; //'a'
  }
  else if(lu == true && lm == true && ll == false && ru == false && rm == false && rl == false)
  {
    return 2; //'b'
  }
  else if(lu == true && lm == false && ll == false && ru == true && rm == false && rl == false)
  {
    return 3; //'c'
  }
  else if(lu == true && lm == false && ll == false && ru == true && rm == true && rl == false)
  {
    return 4; //'d'
  }
  else if(lu == true && lm == false && ll == false && ru == false && rm == true && rl == false)
  {
    return 5; //'e'
  }
  else if(lu == true && lm == true && ll == false && ru == true && rm == false && rl == false)
  {
    return 6; //'f'
  }
  else if(lu == true && lm == true && ll == false && ru == true && rm == true && rl == false)
  {
    return 7; //'g'
  }
  else if(lu == true && lm == true && ll == false && ru == false && rm == true && rl == false)
  {
    return 8; //'h'
  }
  else if(lu == false && lm == true && ll == false && ru == true && rm == false && rl == false)
  {
    return 9; //'i'
  }
  else if(lu == false && lm == true && ll == false && ru == true && rm == true && rl == false)
  {
    return 0; //'j'
  }
    else if(lu == false && lm == false && ll == false && ru == false && rm == false && rl ==false)
  {
    return 100; // ' ' 
  }
    else if(lu == false && lm == false && ll == true && ru == false && rm == false && rl == true)
  {
    return 101; // '-' 
  }
    else if(lu == false && lm == false && ll == true && ru == true && rm == false && rl == true)
  {
    return 102; // '+' 
  }
    else if(lu == true && lm == false && ll == false && ru == false && rm == false && rl == true)
  {
    return 103; // '*' 
  }
    else if(lu == false && lm == false && ll == true && ru == true && rm == false && rl == false)
  {
    return 104; // '/' 
  }
     else if(lu == true && lm == true && ll == true && ru == true && rm == true && rl == true)
  {
    return 105; // '=' 
  }
     else if(lu == false && lm == false && ll == true && ru == true && rm == true && rl == true)
  {
    return 106; // '#' 
  }
     else if(lu == false && lm == true && ll == false && ru == false && rm == false && rl == false)
  {
    return 111; // 'separator/left marker point .' 
  }
   else if(lu == false && lm == false && ll == false && ru == true && rm == true && rl == false)
  {
    return 113; // '^/upper limit' 
  }
  else if(lu == false && lm == false && ll == true && ru == false && rm == true && rl == true)
  {
    return 114; // '% modulus remainder' 
  }
   else if(lu == false && lm == false && ll == false && ru == true && rm == false && rl == true)
  {
    return 115; // '. decimal point' 
  }
    else if(lu == true && lm == true && ll == true && ru == false && rm == false && rl == false)
  {
    return 131; // 'L' 
  }
    else if(lu == true && lm == false && ll == true && ru == true && rm == true && rl == false)
  {
    return 133; // 'N' 
  }
    else if(lu == true && lm == false && ll == true && ru == false && rm == true && rl == false)
  {
    return 134; // 'O' 
  }
    else if(lu == true && lm == true && ll == true && ru == true && rm == true && rl == false)
  {
    return 136; // 'Q' 
  }
    else if(lu == true && lm == true && ll == true && ru == false && rm == true && rl == false)
  {
    return 137; // 'R' 
  }
    else if(lu == false && lm == true && ll == true && ru == true && rm == false && rl == false)
  {
    return 138; // 'S' 
  }
    else if(lu == false && lm == true && ll == true && ru == true && rm == true && rl == false)
  {
    return 139; // 'T' 
  }
    else if(lu == true && lm == false && ll == true && ru == true && rm == false && rl == true)
  {
    return 143; // 'X' 
  }
    else if(lu == false && lm == true && ll == true && ru == true && rm == false && rl == true)
  {
    return 146; // 'integral sign' 
  }
  /*
    else if(lu == false && lm == false && ll == false && ru == true && rm == true && rl == false)
  {
    return 147; // 'upper limit' 
  }
  */
    else if(lu == false && lm == false && ll == false && ru == false && rm == true && rl == true)
  {
    return 148; // 'lower limit' 
  }  
    else if(lu == false && lm == false && ll == false && ru == false && rm == true && rl == false)
  {
    return 149; // 'right marker point' 
  }
  
}

//translate each number into braille, returning values as a pointer to array
bool carray[6];
bool* Number_to_Braille(int number)
{

  if(number == 1)
  {
    carray[0] = true;
    carray[1] = false;
    carray[2] = false;
    carray[3] = false;
    carray[4] = false;
    carray[5] = false;
  }
  else if(number == 2)
  {
    carray[0] = true;
    carray[1] = true;
    carray[2] = false;
    carray[3] = false;
    carray[4] = false;
    carray[5] = false;
  }
   else if(number == 3)
  {
    carray[0] = true;
    carray[1] = false;
    carray[2] = false;
    carray[3] = true;
    carray[4] = false;
    carray[5] = false;
  }
   else if(number == 4)
  {
    carray[0] = true;
    carray[1] = false;
    carray[2] = false;
    carray[3] = true;
    carray[4] = true;
    carray[5] = false;
  }
    else if(number == 5)
  {
    carray[0] = true;
    carray[1] = false;
    carray[2] = false;
    carray[3] = false;
    carray[4] = true;
    carray[5] = false;
  }
   else if(number == 6)
  {
    carray[0] = true;
    carray[1] = true;
    carray[2] = false;
    carray[3] = true;
    carray[4] = false;
    carray[5] = false;
  }
   else if(number == 7)
  {
    carray[0] = true;
    carray[1] = true;
    carray[2] = false;
    carray[3] = true;
    carray[4] = true;
    carray[5] = false;
  }
    else if(number == 8)
  {
    carray[0] = true;
    carray[1] = true;
    carray[2] = false;
    carray[3] = false;
    carray[4] = true;
    carray[5] = false;
  }
   else if(number == 9)
  {
    carray[0] = false;
    carray[1] = true;
    carray[2] = false;
    carray[3] = true;
    carray[4] = false;
    carray[5] = false;
  }
   else if(number == 0)
  {
    carray[0] = false;
    carray[1] = true;
    carray[2] = false;
    carray[3] = true;
    carray[4] = true;
    carray[5] = false;
  } 

  else if(number == 100) //' '
  {
    carray[0] = false;
    carray[1] = false;
    carray[2] = false;
    carray[3] = false;
    carray[4] = false;
    carray[5] = false;
  } 
    else if(number == 101) //'-'
  {
    carray[0] = false;
    carray[1] = false;
    carray[2] = true;
    carray[3] = false;
    carray[4] = false;
    carray[5] = true;
  } 
   else if(number == 102) //'+'
  {
    carray[0] = false;
    carray[1] = false;
    carray[2] = true;
    carray[3] = true;
    carray[4] = false;
    carray[5] = true;
  } 
    else if(number == 105) //'='
  {
    carray[0] = true;
    carray[1] = true;
    carray[2] = true;
    carray[3] = true;
    carray[4] = true;
    carray[5] = true;
  } 
    else if(number == 116) //'imaginary number (self-invented)
  {
    carray[0] = true;
    carray[1] = false;
    carray[2] = false;
    carray[3] = true;
    carray[4] = true;
    carray[5] = true;
  } 
    else if(number == 143) //'X'
  {
    carray[0] = true;
    carray[1] = false;
    carray[2] = true;
    carray[3] = true;
    carray[4] = false;
    carray[5] = true;
  } 

  

  return carray;
}

//factorial function
int factorial(int x)
{
  int p = 0;
  int res = 1;
  if(x<0)
  {
    res = -99999;
  }
  else if(x=0)
  {
    res = 1;
  }
  if(x>0)
  {
    for(p=x; p>0; p--)
    {
      res = res*p;
    }
  }
  return res; 
}

//power function
int power(int x,int y)
{
  int p = 0;
  int res = 1;
  if(y>0)
  {
    for(p=y;p>0;p--)
    {
      res = res * x;
    }
  }
  return res;
}

////
void display_result(double result)
{
    //display result

    int m = 0;
    int res_trans = 0;
    double temp = 0;
    int num_of_digits =1;
    bool ct_val;
    bool *p_ct_val = &ct_val;

    //determine if negative, if so display negative sign and make positive
    if(result<0)
    {
      digitalWrite(led_lu, LOW);
      digitalWrite(led_lm, LOW);
      digitalWrite(led_ll, HIGH);
      digitalWrite(led_ru, LOW);
      digitalWrite(led_rm, LOW);
      digitalWrite(led_rl, HIGH);
      delay(1000);
      result = result * (-1);
    }

    
    temp = result;
    //count number of digits 
    num_of_digits = 1;
    //count number of digits in result
    while(temp > 0)
    {
      temp = (int)(temp/10);
      num_of_digits++;
    }

    //display all digits, on after the other
    for(m = num_of_digits-2; m>=0; m--)
    {
      res_trans = (int)result/power(10,m);
      
      p_ct_val = Number_to_Braille(res_trans);
      digitalWrite(led_lu, *(p_ct_val));
      digitalWrite(led_lm, *(p_ct_val+1));
      digitalWrite(led_ll, *(p_ct_val+2));
      digitalWrite(led_ru, *(p_ct_val+3));
      digitalWrite(led_rm, *(p_ct_val+4));
      digitalWrite(led_rl, *(p_ct_val+5)); 
  
      result = result - res_trans*power(10,m);
      
      delay(1000);
    }
   

    //take care of the decimal digits
    //check if result is an integer
    if(result != 0)
    {
      //write decimal point
      digitalWrite(led_lu, false);
      digitalWrite(led_lm, false);
      digitalWrite(led_ll, false);
      digitalWrite(led_ru, true);
      digitalWrite(led_rm, false);
      digitalWrite(led_rl, true); 
      delay(1000);
  
      
      for(m=1;m<4;m++)
      {
        if(result != 0)
        {
          res_trans = (int)(result*power(10,m));
          
          p_ct_val = Number_to_Braille(res_trans);
          digitalWrite(led_lu, *(p_ct_val));
          digitalWrite(led_lm, *(p_ct_val+1));
          digitalWrite(led_ll, *(p_ct_val+2));
          digitalWrite(led_ru, *(p_ct_val+3));
          digitalWrite(led_rm, *(p_ct_val+4));
          digitalWrite(led_rl, *(p_ct_val+5)); 
      
          result = result - ((double)res_trans)/power(10,m);
          delay(1000);
        }  
      }   
    }

    delay(1000);
}

void complete()
{
    //signify end
    digitalWrite(led_lu, true);
    digitalWrite(led_lm, false);
    digitalWrite(led_ll, true);
    digitalWrite(led_ru, false);
    digitalWrite(led_rm, true);
    digitalWrite(led_rl, false); 
    delay(300);
    digitalWrite(led_lu, false);
    digitalWrite(led_lm, true);
    digitalWrite(led_ll, false);
    digitalWrite(led_ru, true);
    digitalWrite(led_rm, false);
    digitalWrite(led_rl, true); 
    delay(300);
    digitalWrite(led_lu, false);
    digitalWrite(led_lm, false);
    digitalWrite(led_ll, false);
    digitalWrite(led_ru, false);
    digitalWrite(led_rm, false);
    digitalWrite(led_rl, false); 
}


//transfer pointers for number to braille translator
bool ct_val;
bool *p_ct_val = &ct_val;

void display_digit(int character)
{
  p_ct_val = Number_to_Braille(character);
  digitalWrite(led_lu, *(p_ct_val));
  digitalWrite(led_lm, *(p_ct_val+1));
  digitalWrite(led_ll, *(p_ct_val+2));
  digitalWrite(led_ru, *(p_ct_val+3));
  digitalWrite(led_rm, *(p_ct_val+4));
  digitalWrite(led_rl, *(p_ct_val+5)); 
  delay(1000);
}

//number storage
//float num[5];
//int N = 0;

//operator storage, operator type: [0][~], operator position: [1][~].
int oper[2][5];
int oper_c[2][5];
int o = 0;

//loop counter
int m = 0;
int len = 0;

//button pressed/released
bool b_released = true;

//result, transfer and counter values
double result = 0;
int res_trans = 0;
int temp=0;
int num_of_digits = 0;

//braille cell to store numbers
BrailleCell c_number[10];
void reset_braille_cell()
{
  for(m=0;m<10;m++)
  {
  c_number[m].lu=false;
  c_number[m].lm=false;
  c_number[m].ll=false;
  c_number[m].ru=false;
  c_number[m].rm=false;
  c_number[m].rl=false;
  }
  m = 0;
}

void loop() 
{ 
  //real in dot values
  state.lu = digitalRead(blu);
  trans.lu = state.lu;  
  state.lm = digitalRead(blm);
  trans.lm = state.lm;  
  state.ll = digitalRead(bll);
  trans.ll = state.ll; 
  state.ru = digitalRead(bru);
  trans.ru = state.ru; 
  state.rm = digitalRead(brm);
  trans.rm = state.rm;  
  state.rl = digitalRead(brl);
  trans.rl = state.rl;
  

  //choose to set thaose dot values
  Set_state = digitalRead(Set);
  if(Set_state == HIGH)
  {
    c_array[n].lu = trans.lu;
    c_array[n].lm = trans.lm;
    c_array[n].ll = trans.ll;
    c_array[n].ru = trans.ru;
    c_array[n].rm = trans.rm;
    c_array[n].rl = trans.rl;
  }  


  //display current values of cell
  Display_state = digitalRead(Display);
  if(Display_state == HIGH)
  {
    digitalWrite(led_lu, c_array[n].lu);
    digitalWrite(led_lm, c_array[n].lm);
    digitalWrite(led_ll, c_array[n].ll);
    digitalWrite(led_ru, c_array[n].ru);
    digitalWrite(led_rm, c_array[n].rm);
    digitalWrite(led_rl, c_array[n].rl);  
  }  

  //
  //increment value
  NEXT_STATE = digitalRead(NEXT);
  if(NEXT_STATE == HIGH && b_released == true)
  {
    n++;
    Serial.print("n=");
    Serial.print(n);
    Serial.print(" ");
    b_released = false;
  }

  NEXT_STATE = digitalRead(NEXT);
  if(NEXT_STATE == LOW)
  {
    b_released = true;
  }
  

  fill_graph_state = digitalRead(fill_graph);
  if(fill_graph_state == true)
  {
    for(int my = 0; my < 16; my++)
    {
      for(int mx = 0; mx < 20; mx++)
      {
        fdgraph[my][mx] = 'X';
      }
    }
    
    fdgraph_to_string();
  }

  clear_graph_state = digitalRead(clear_graph);
  if(clear_graph_state == true)
  {
    for(int my = 0; my < 16; my++)
    {
      for(int mx = 0; mx < 20; mx++)
      {
        fdgraph[my][mx] = '.';
      }
    }
    
    fdgraph_to_string();
  }


  
  //graphs
  Graph_state = digitalRead(Graph);
  if(Graph_state == true)
  {
  Serial.print("\nGraph Hello\n");

  Serial.print("First choose your graph\n");
    //option test
  int buttonState0 = 0;
  int buttonState1 = 0;
  int buttonState2 = 0;
  String message = "";
  bool chosen_graph = false;

  while(chosen_graph == false)
  {
      //35 - next
      buttonState0 = digitalRead(buttonPin0);
      if (buttonState0 == HIGH)
      {
        if (cur_lev != 4)
        {
          if(cur_pos[cur_lev] == 4) cur_pos[cur_lev] = 0;
          else cur_pos[cur_lev] ++; 
      
          if(cur_lev == 0) message = options[cur_pos[cur_lev]];
          else message = menu[cur_pos[0]][cur_pos[1]];
          
          Serial.println("Next: " + message);

          if(cur_lev == 0)
          {
          display_digit(cur_pos[0]);
          }
          if(cur_lev != 0)
          {
          display_digit(cur_pos[1]);
          }
        }
        
        delay(200);
        }
        
      //37 - display
      buttonState1 = digitalRead(buttonPin1);
      if (buttonState1 == HIGH)
      {
        if(cur_lev == 1) 
        {
          cur_lev = 2;
          message = menu[cur_pos[0]][cur_pos[1]];
          Serial.println("Displaying: " + message);
          chosen_graph = true;
          }
          
        else if (cur_lev == 0)
        {
          cur_lev = 1;
          cur_pos[1] = 0;
          message = menu[cur_pos[0]][cur_pos[1]];
          Serial.println(options[cur_pos[0]] + " options");
          delay(2000);
          Serial.println(message);
          }

        
        delay(200);
        }
    
      //39 - return home
      buttonState2 = digitalRead(buttonPin2);
      if (buttonState2 == HIGH)
      {
        if(cur_lev != 0)
        {
          cur_lev = 0;
          cur_pos[0] = 0;
          cur_pos[1] = 0;
    
          Serial.println("Back to menu: " + options[0]);
          display_digit(0);
        }    
        delay(200);
      }
   }

    

    //initialise array
    for(int my = 0; my < 16; my++)
    {
      for(int mx = 0; mx < 20; mx++)
      {
        fdgraph[my][mx] = '.';
      }
    }

    double y = 0;
    double x = 0;
    int x_disp = 0;
    int y_disp = 0;

    double hp = 0;

    //y = 8*sin(x-10)/(x-10)+4;
    
    if(message == "sin(x)")
    {
      for(x = 0 - hp; x < 20 - hp; x=x+0.01)
      {
        y = 5*sin(0.5*x)+7;
  
        y_disp = round(y);
        x_disp = round(x);           
        if(y_disp <= 16 && y_disp >=0)
        {
        fdgraph[y_disp][x_disp] = 'X';
        }
      }
    }
    else if(message == "cos(x)")
    {
      for(x = 0 - hp; x < 20 - hp; x=x+0.01)
      {
        y = 5*cos(0.5*x) + 7;
  
        y_disp = round(y);
        x_disp = round(x);           
        if(y_disp <= 16 && y_disp >=0)
        {
        fdgraph[y_disp][x_disp] = 'X';
        }
      }
    }
    else if(message == "tan(x)")
    {
      for(x = 0 - hp; x < 20 - hp; x=x+0.01)
      {
        y = 5*tan(0.5*x-10) + 7;
  
        y_disp = round(y);
        x_disp = round(x);           
        if(y_disp <= 16 && y_disp >=0)
        {
        fdgraph[y_disp][x_disp] = 'X';
        }
      }
    }
    else if(message == "sec(x)")
    {
      for(x = 0 - hp; x < 20 - hp; x=x+0.01)
      {
        y = 1/cos(0.5*x) + 7;
  
        y_disp = round(y);
        x_disp = round(x);           
        if(y_disp <= 16 && y_disp >=0)
        {
        fdgraph[y_disp][x_disp] = 'X';
        }
      }
    }
    else if(message == "sin(2x)")
    {
      for(x = 0 - hp; x < 20 - hp; x=x+0.01)
      {
        y = 5*sin(2*0.5*x) + 7;
  
        y_disp = round(y);
        x_disp = round(x);           
        if(y_disp <= 16 && y_disp >=0)
        {
        fdgraph[y_disp][x_disp] = 'X';
        }
      }
    }
    else if(message == "x^3")
    {
      for(x = 0 - hp; x < 20 - hp; x=x+0.01)
      {
        y = (x-10)*(x-10)*(x-10) + 7;
  
        y_disp = round(y);
        x_disp = round(x);           
        if(y_disp <= 16 && y_disp >=0)
        {
        fdgraph[y_disp][x_disp] = 'X';
        }
      }
    }
    else if(message == "x^2")
    {
      for(x = 0 - hp; x < 20 - hp; x=x+0.01)
      {
        y = (x-10)*(x-10) + 7;
  
        y_disp = round(y);
        x_disp = round(x);           
        if(y_disp <= 16 && y_disp >=0)
        {
        fdgraph[y_disp][x_disp] = 'X';
        }
      }
    }
    else if(message == "-x^2")
    {
      for(x = 0 - hp; x < 20 - hp; x=x+0.01)
      {
        y = (-1)*(x-10)*(x-10) + 7;
  
        y_disp = round(y);
        x_disp = round(x);           
        if(y_disp <= 16 && y_disp >=0)
        {
        fdgraph[y_disp][x_disp] = 'X';
        }
      }
    }
    else if(message == "x^2-3")
    {
      for(x = 0 - hp; x < 20 - hp; x=x+0.01)
      {
        y = (x-10)*(x-10) + 7;
  
        y_disp = round(y);
        x_disp = round(x);           
        if(y_disp <= 16 && y_disp >=0)
        {
        fdgraph[y_disp][x_disp] = 'X';
        }
      }
    }
    else if(message == "x^2+x")
    {
      for(x = 0 - hp; x < 20 - hp; x=x+0.01)
      {
        y = (x-10)*(x-10) + (x-10) + 7;
  
        y_disp = round(y);
        x_disp = round(x);           
        if(y_disp <= 16 && y_disp >=0)
        {
        fdgraph[y_disp][x_disp] = 'X';
        }
      }
    }
    else if(message == "x")
    {
      for(x = 0 - hp; x < 20 - hp; x=x+0.01)
      {
        y = (x-10) + 7;
  
        y_disp = round(y);
        x_disp = round(x);           
        if(y_disp <= 16 && y_disp >=0)
        {
        fdgraph[y_disp][x_disp] = 'X';
        }
      }
    }
    else if(message == "-x")
    {
      for(x = 0 - hp; x < 20 - hp; x=x+0.01)
      {
        y = (-1)*(x-10) + 7;
  
        y_disp = round(y);
        x_disp = round(x);           
        if(y_disp <= 16 && y_disp >=0)
        {
        fdgraph[y_disp][x_disp] = 'X';
        }
      }
    }
    else if(message == "2x")
    {
      for(x = 0 - hp; x < 20 - hp; x=x+0.01)
      {
        y = 2*(x-10) + 7;
  
        y_disp = round(y);
        x_disp = round(x);           
        if(y_disp <= 16 && y_disp >=0)
        {
        fdgraph[y_disp][x_disp] = 'X';
        }
      }
    }
    else if(message == "4x")
    {
      for(x = 0 - hp; x < 20 - hp; x=x+0.01)
      {
        y = 4*(x-10) + 7;
  
        y_disp = round(y);
        x_disp = round(x);           
        if(y_disp <= 16 && y_disp >=0)
        {
        fdgraph[y_disp][x_disp] = 'X';
        }
      }
    }
    else if(message == "x+3")
    {
      for(x = 0 - hp; x < 20 - hp; x=x+0.01)
      {
        y = (x-10) + 7 + 3;
  
        y_disp = round(y);
        x_disp = round(x);           
        if(y_disp <= 16 && y_disp >=0)
        {
        fdgraph[y_disp][x_disp] = 'X';
        }
      }
    }
    else if(message == "Circle")
    {
      for(x = 0 - hp; x < 20 - hp; x=x+0.01)
      {
        y = sqrt(25-(x-10)*(x-10)) + 7;
  
        y_disp = round(y);
        x_disp = round(x);           
        if(y_disp <= 16 && y_disp >=0)
        {
        fdgraph[y_disp][x_disp] = 'X';
        }
      }
      for(x = 0 - hp; x < 20 - hp; x=x+0.01)
      {
        y = (-1)*sqrt(25-(x-10)*(x-10)) + 7;
  
        y_disp = round(y);
        x_disp = round(x);           
        if(y_disp <= 16 && y_disp >=0)
        {
        fdgraph[y_disp][x_disp] = 'X';
        }
      }
    }
    else if(message == "Ellipse")
    {
      for(x = 0 - hp; x < 20 - hp; x=x+0.01)
      {
        y = 1*sqrt(25-(x-10)*(x-10)/2) + 7;
  
        y_disp = round(y);
        x_disp = round(x);           
        if(y_disp <= 16 && y_disp >=0)
        {
        fdgraph[y_disp][x_disp] = 'X';
        }
      }
      for(x = 0 - hp; x < 20 - hp; x=x+0.01)
      {
        y = (-1)*sqrt(25-(x-10)*(x-10)/2) + 7;
  
        y_disp = round(y);
        x_disp = round(x);           
        if(y_disp <= 16 && y_disp >=0)
        {
        fdgraph[y_disp][x_disp] = 'X';
        }
      }
    }
    else if(message == "Hyperbola")
    {
      for(x = 0 - hp; x < 20 - hp; x=x+0.01)
      {
        y = 0.5*sqrt(25+(x-10)*(x-10)*1.5) + 7;
  
        y_disp = round(y);
        x_disp = round(x);           
        if(y_disp <= 16 && y_disp >=0)
        {
        fdgraph[y_disp][x_disp] = 'X';
        }
      }
      for(x = 0 - hp; x < 20 - hp; x=x+0.01)
      {
        y = (-0.5)*sqrt(25+(x-10)*(x-10)*1.5) + 7;
  
        y_disp = round(y);
        x_disp = round(x);           
        if(y_disp <= 16 && y_disp >=0)
        {
        fdgraph[y_disp][x_disp] = 'X';
        }
      }
    }
    else if(message == "Parabola")
    {
      for(x = 0 - hp; x < 20 - hp; x=x+0.01)
      {
        y = 1;
  
        y_disp = round(y);
        x_disp = round(x);           
        if(y_disp <= 16 && y_disp >=0)
        {
        fdgraph[y_disp][x_disp] = 'X';
        }
      }
    }
    else if(message == "abs(x)")
    {
      for(x = 0 - hp; x < 20 - hp; x=x+0.01)
      {
        y = abs(x-10) + 7;
  
        y_disp = round(y);
        x_disp = round(x);           
        if(y_disp <= 16 && y_disp >=0)
        {
        fdgraph[y_disp][x_disp] = 'X';
        }
      }
    }
    else if(message == "sqrt(x)")
    {
      for(x = 0 - hp; x < 20 - hp; x=x+0.01)
      {
        y = sqrt(x-10) + 7;
  
        y_disp = round(y);
        x_disp = round(x);           
        if(y_disp <= 16 && y_disp >=0)
        {
        fdgraph[y_disp][x_disp] = 'X';
        }
      }
    }
    else if(message == "e^x")
    {
      for(x = 0 - hp; x < 20 - hp; x=x+0.01)
      {
        y = exp(x-10) + 7;
  
        y_disp = round(y);
        x_disp = round(x);           
        if(y_disp <= 16 && y_disp >=0)
        {
        fdgraph[y_disp][x_disp] = 'X';
        }
      }
    }
    else if(message == "ln(x)")
    {
      for(x = 0 - hp; x < 20 - hp; x=x+0.01)
      {
        y = log(x-10) + 7;
  
        y_disp = round(y);
        x_disp = round(x);           
        if(y_disp <= 16 && y_disp >=0)
        {
        fdgraph[y_disp][x_disp] = 'X';
        }
      }
    }
    else if(message == "1/x")
    {
      for(x = 0 - hp; x < 20 - hp; x=x+0.01)
      {
        y = 1/(x-10) + 7;
  
        y_disp = round(y);
        x_disp = round(x);           
        if(y_disp <= 16 && y_disp >=0)
        {
        fdgraph[y_disp][x_disp] = 'X';
        }
      }
    }


    //display_graph;
    for(int my = 14; my >= 1; my--)
    {
      Serial.print("\n");
      for(int mx = 0; mx < 20; mx++)
      {     
        Serial.print(fdgraph[my][mx]);
        Serial.print(" ");
      }
    }
    Serial.print("\n");
    

    //swap top and bottom of graph
    for(int mx = 0; mx < 20; mx++)
    {
      for(int my = 0; my < 8; my++)
      {
      fdtransfer = fdgraph[my][mx];
      fdgraph[my][mx] = fdgraph[my+8][mx];
      fdgraph[my+8][mx] = fdtransfer;
      }
    }

    //flip the graph in the x direction
    for(int mx = 0; mx < 10; mx++)
    {
      for(int my = 0; my < 16; my++)
      {
      fdtransfer = fdgraph[my][10+mx];
      fdgraph[my][10+mx] = fdgraph[my][9-mx];
      fdgraph[my][9-mx] = fdtransfer;
      }
    }
    
    //fdgraph_fill();
    //fdgraph_test1();
    //fdgraph_test2();
    //fdgraph_test3();
    fdgraph_to_string();
    delay(1000);
    
    n=0;
  }
  
  Trigonometry_state = digitalRead(Trigonometry);
  if(Trigonometry_state == true)
  {
  Serial.print("\nTrigonometry Hello\n");

  bool neg = false;
  bool dp = false;
  float num = false;
  float result = 0;
  int num_pos = 0;
  int decimal = 0;

  //find number pos
  for(m=0; m<n; m++)
  {
    if(Braille_to_number(c_array[m].lu,c_array[m].lm,c_array[m].ll,c_array[m].ru,c_array[m].rm,c_array[m].rl)==149)
    {
      num_pos = m;
    }
  }

  //find decimal point
  for(m=0; m<n; m++)
  {
    if(Braille_to_number(c_array[m].lu,c_array[m].lm,c_array[m].ll,c_array[m].ru,c_array[m].rm,c_array[m].rl)==115)
    {
      decimal = m;
      dp = true;
    }
  }

   //find negative
   if(Braille_to_number(c_array[num_pos+1].lu,c_array[num_pos+1].lm,c_array[num_pos+1].ll,c_array[num_pos+1].ru,c_array[num_pos+1].rm,c_array[num_pos+1].rl)==101)
   {
      neg = true;
   }
  

   if(neg == true)
   {
     for(m = n-1; m > num_pos+1; m--)
     {
         num = num + (power(10,n-1-m))*Braille_to_number(c_array[m].lu,c_array[m].lm,c_array[m].ll,c_array[m].ru,c_array[m].rm,c_array[m].rl);
     }
     num = num*(-1);
   }
   else
   {
      if(dp == false)
      {
         for(m = n-1; m > num_pos; m--)
         {
             num = num + (power(10,n-1-m))*Braille_to_number(c_array[m].lu,c_array[m].lm,c_array[m].ll,c_array[m].ru,c_array[m].rm,c_array[m].rl);
         }
      }
      else
      {
         for(m = decimal-1; m > num_pos; m--)
         {
             num = num + (power(10,decimal-1-m))*Braille_to_number(c_array[m].lu,c_array[m].lm,c_array[m].ll,c_array[m].ru,c_array[m].rm,c_array[m].rl);
         }
         for(m = decimal+1; m > n; m++)
         {
             num = num + ((double)Braille_to_number(c_array[m].lu,c_array[m].lm,c_array[m].ll,c_array[m].ru,c_array[m].rm,c_array[m].rl))/((double)(pow(10,m-(decimal+1)+1)));
         }

         if(n - decimal == 1)
         {       
           num = num + ((double)Braille_to_number(c_array[decimal+1].lu,c_array[decimal+1].lm,c_array[decimal+1].ll,c_array[decimal+1].ru,c_array[decimal+1].rm,c_array[decimal+1].rl))/((double)10);
         }
         else if(n - decimal == 2)
         {
           num = num + ((double)Braille_to_number(c_array[decimal+1].lu,c_array[decimal+1].lm,c_array[decimal+1].ll,c_array[decimal+1].ru,c_array[decimal+1].rm,c_array[decimal+1].rl))/((double)10);
           num = num + ((double)Braille_to_number(c_array[decimal+2].lu,c_array[decimal+2].lm,c_array[decimal+2].ll,c_array[decimal+2].ru,c_array[decimal+2].rm,c_array[decimal+2].rl))/((double)100);
         }
         else if(n - decimal == 3)
         {
           num = num + ((double)Braille_to_number(c_array[decimal+1].lu,c_array[decimal+1].lm,c_array[decimal+1].ll,c_array[decimal+1].ru,c_array[decimal+1].rm,c_array[decimal+1].rl))/((double)10);
           num = num + ((double)Braille_to_number(c_array[decimal+2].lu,c_array[decimal+2].lm,c_array[decimal+2].ll,c_array[decimal+2].ru,c_array[decimal+2].rm,c_array[decimal+2].rl))/((double)100);
           num = num + ((double)Braille_to_number(c_array[decimal+3].lu,c_array[decimal+3].lm,c_array[decimal+3].ll,c_array[decimal+3].ru,c_array[decimal+3].rm,c_array[decimal+3].rl))/((double)1000);
         }
         else if(n - decimal == 4)
         {
           num = num + ((double)Braille_to_number(c_array[decimal+1].lu,c_array[decimal+1].lm,c_array[decimal+1].ll,c_array[decimal+1].ru,c_array[decimal+1].rm,c_array[decimal+1].rl))/((double)10);
           num = num + ((double)Braille_to_number(c_array[decimal+2].lu,c_array[decimal+2].lm,c_array[decimal+2].ll,c_array[decimal+2].ru,c_array[decimal+2].rm,c_array[decimal+2].rl))/((double)100);
           num = num + ((double)Braille_to_number(c_array[decimal+3].lu,c_array[decimal+3].lm,c_array[decimal+3].ll,c_array[decimal+3].ru,c_array[decimal+3].rm,c_array[decimal+3].rl))/((double)1000);
           num = num + ((double)Braille_to_number(c_array[decimal+4].lu,c_array[decimal+4].lm,c_array[decimal+4].ll,c_array[decimal+4].ru,c_array[decimal+4].rm,c_array[decimal+4].rl))/((double)10000);
         }
      }
   }
  Serial.print(num);

  //sin function
  if(Braille_to_number(c_array[num_pos-1].lu,c_array[num_pos-1].lm,c_array[num_pos-1].ll,c_array[num_pos-1].ru,c_array[num_pos-1].rm,c_array[num_pos+1].rl)==133 &&
     Braille_to_number(c_array[num_pos-2].lu,c_array[num_pos-2].lm,c_array[num_pos-2].ll,c_array[num_pos-2].ru,c_array[num_pos-2].rm,c_array[num_pos-2].rl)==9 &&
     Braille_to_number(c_array[num_pos-3].lu,c_array[num_pos-3].lm,c_array[num_pos-3].ll,c_array[num_pos-3].ru,c_array[num_pos-3].rm,c_array[num_pos-3].rl)==138)
  {
      Serial.print("\nsin\n");

      deg_rad_state = digitalRead(deg_rad);
      if(deg_rad_state == true)
      {
         num = num * (3.1415926/180);
      } 
      
      result = sin(num); 
  }
  //cos function
  if(Braille_to_number(c_array[num_pos-1].lu,c_array[num_pos-1].lm,c_array[num_pos-1].ll,c_array[num_pos-1].ru,c_array[num_pos-1].rm,c_array[num_pos+1].rl)==138 &&
     Braille_to_number(c_array[num_pos-2].lu,c_array[num_pos-2].lm,c_array[num_pos-2].ll,c_array[num_pos-2].ru,c_array[num_pos-2].rm,c_array[num_pos-2].rl)==134 &&
     Braille_to_number(c_array[num_pos-3].lu,c_array[num_pos-3].lm,c_array[num_pos-3].ll,c_array[num_pos-3].ru,c_array[num_pos-3].rm,c_array[num_pos-3].rl)==3)
  {
      Serial.print("\ncos\n");

      deg_rad_state = digitalRead(deg_rad);
      if(deg_rad_state == true)
      {
         num = num * (3.1415926/180);
      } 
      
      result = cos(num); 
  }
  //tan function
  if(Braille_to_number(c_array[num_pos-1].lu,c_array[num_pos-1].lm,c_array[num_pos-1].ll,c_array[num_pos-1].ru,c_array[num_pos-1].rm,c_array[num_pos+1].rl)==133 &&
     Braille_to_number(c_array[num_pos-2].lu,c_array[num_pos-2].lm,c_array[num_pos-2].ll,c_array[num_pos-2].ru,c_array[num_pos-2].rm,c_array[num_pos-2].rl)==1 &&
     Braille_to_number(c_array[num_pos-3].lu,c_array[num_pos-3].lm,c_array[num_pos-3].ll,c_array[num_pos-3].ru,c_array[num_pos-3].rm,c_array[num_pos-3].rl)==139)
  {
      Serial.print("\ntan\n");

      deg_rad_state = digitalRead(deg_rad);
      if(deg_rad_state == true)
      {
         num = num * (3.1415926/180);
      } 
      
      result = tan(num); 
  }
  //asin function
  if(Braille_to_number(c_array[num_pos-1].lu,c_array[num_pos-1].lm,c_array[num_pos-1].ll,c_array[num_pos-1].ru,c_array[num_pos-1].rm,c_array[num_pos+1].rl)==133 &&
     Braille_to_number(c_array[num_pos-2].lu,c_array[num_pos-2].lm,c_array[num_pos-2].ll,c_array[num_pos-2].ru,c_array[num_pos-2].rm,c_array[num_pos-2].rl)==9 &&
     Braille_to_number(c_array[num_pos-3].lu,c_array[num_pos-3].lm,c_array[num_pos-3].ll,c_array[num_pos-3].ru,c_array[num_pos-3].rm,c_array[num_pos-3].rl)==138 &&
     Braille_to_number(c_array[num_pos-4].lu,c_array[num_pos-4].lm,c_array[num_pos-4].ll,c_array[num_pos-4].ru,c_array[num_pos-4].rm,c_array[num_pos-4].rl)==1)
  {
      Serial.print("\nasin\n");
      result = asin(num); 
      
      deg_rad_state = digitalRead(deg_rad);
      if(deg_rad_state == true)
      {
         result = result * (180/3.1415926);
      } 
  }
  //acos function
  if(Braille_to_number(c_array[num_pos-1].lu,c_array[num_pos-1].lm,c_array[num_pos-1].ll,c_array[num_pos-1].ru,c_array[num_pos-1].rm,c_array[num_pos+1].rl)==138 &&
     Braille_to_number(c_array[num_pos-2].lu,c_array[num_pos-2].lm,c_array[num_pos-2].ll,c_array[num_pos-2].ru,c_array[num_pos-2].rm,c_array[num_pos-2].rl)==134 &&
     Braille_to_number(c_array[num_pos-3].lu,c_array[num_pos-3].lm,c_array[num_pos-3].ll,c_array[num_pos-3].ru,c_array[num_pos-3].rm,c_array[num_pos-3].rl)==3 &&
     Braille_to_number(c_array[num_pos-4].lu,c_array[num_pos-4].lm,c_array[num_pos-4].ll,c_array[num_pos-4].ru,c_array[num_pos-4].rm,c_array[num_pos-4].rl)==1)
  {
      Serial.print("\nacos\n");
      result = acos(num); 
      
      deg_rad_state = digitalRead(deg_rad);
      if(deg_rad_state == true)
      {
         result = result * (180/3.1415926);
      } 
  }
  //atan function
  if(Braille_to_number(c_array[num_pos-1].lu,c_array[num_pos-1].lm,c_array[num_pos-1].ll,c_array[num_pos-1].ru,c_array[num_pos-1].rm,c_array[num_pos+1].rl)==133 &&
     Braille_to_number(c_array[num_pos-2].lu,c_array[num_pos-2].lm,c_array[num_pos-2].ll,c_array[num_pos-2].ru,c_array[num_pos-2].rm,c_array[num_pos-2].rl)==1 &&
     Braille_to_number(c_array[num_pos-3].lu,c_array[num_pos-3].lm,c_array[num_pos-3].ll,c_array[num_pos-3].ru,c_array[num_pos-3].rm,c_array[num_pos-3].rl)==139 &&
     Braille_to_number(c_array[num_pos-4].lu,c_array[num_pos-4].lm,c_array[num_pos-4].ll,c_array[num_pos-4].ru,c_array[num_pos-4].rm,c_array[num_pos-4].rl)==1)
  {
      Serial.print("\natan\n");
      result = atan(num); 
      
      deg_rad_state = digitalRead(deg_rad);
      if(deg_rad_state == true)
      {
         result = result * (180/3.1415926);
      } 
  }
  //sqrt function
  if(Braille_to_number(c_array[num_pos-1].lu,c_array[num_pos-1].lm,c_array[num_pos-1].ll,c_array[num_pos-1].ru,c_array[num_pos-1].rm,c_array[num_pos+1].rl)==139 &&
     Braille_to_number(c_array[num_pos-2].lu,c_array[num_pos-2].lm,c_array[num_pos-2].ll,c_array[num_pos-2].ru,c_array[num_pos-2].rm,c_array[num_pos-2].rl)==137 &&
     Braille_to_number(c_array[num_pos-3].lu,c_array[num_pos-3].lm,c_array[num_pos-3].ll,c_array[num_pos-3].ru,c_array[num_pos-3].rm,c_array[num_pos-3].rl)==136 &&
     Braille_to_number(c_array[num_pos-4].lu,c_array[num_pos-4].lm,c_array[num_pos-4].ll,c_array[num_pos-4].ru,c_array[num_pos-4].rm,c_array[num_pos-4].rl)==138)
  {
      Serial.print("\nsqrt\n");
      result = sqrt(num); 
  }
  //log function
  if(Braille_to_number(c_array[num_pos-1].lu,c_array[num_pos-1].lm,c_array[num_pos-1].ll,c_array[num_pos-1].ru,c_array[num_pos-1].rm,c_array[num_pos+1].rl)==7 &&
     Braille_to_number(c_array[num_pos-2].lu,c_array[num_pos-2].lm,c_array[num_pos-2].ll,c_array[num_pos-2].ru,c_array[num_pos-2].rm,c_array[num_pos-2].rl)==134 &&
     Braille_to_number(c_array[num_pos-3].lu,c_array[num_pos-3].lm,c_array[num_pos-3].ll,c_array[num_pos-3].ru,c_array[num_pos-3].rm,c_array[num_pos-3].rl)==131)
  {
      Serial.print("\nlog\n");
      result = log(num); 
  }


  Serial.print("\nRes\n");
  Serial.print(result);
  Serial.print("\n");

  display_result(result);
  complete();
    
  n=0;
  }

  //integration
  Integral_state = digitalRead(Integral);
  if(Integral_state == true)
  {
    Serial.print("\nIntegral Hello\n");

    //positions
    int upper_pos = 0;
    int lower_pos = 0;
    double lower = 0;
    double upper = 0;
    int i_start = 0;
    int i_end = 0;
    bool neg_u = false;
    bool neg_l = false;
    bool u_dete = false;
    
    for(m = 0; m < n; m++)
    {
       //find lower marker
      if(Braille_to_number(c_array[m].lu,c_array[m].lm,c_array[m].ll,c_array[m].ru,c_array[m].rm,c_array[m].rl)==148)
      {
        lower_pos = m;
      }
      //find upper marker
      else if(Braille_to_number(c_array[m].lu,c_array[m].lm,c_array[m].ll,c_array[m].ru,c_array[m].rm,c_array[m].rl)==113)
      {
        //since upper marker and power sign are the same, the following loop causes program only to catch the first symbol, which must mean upper limit
        if(u_dete == false)
        {
          upper_pos = m;
        }
        u_dete = true;
      }
      //find formula start and end
      else if(Braille_to_number(c_array[m].lu,c_array[m].lm,c_array[m].ll,c_array[m].ru,c_array[m].rm,c_array[m].rl)==149)
      {
          i_start = m;
      }
      else if(Braille_to_number(c_array[m].lu,c_array[m].lm,c_array[m].ll,c_array[m].ru,c_array[m].rm,c_array[m].rl)==111)
      {
          i_end = m;
      }
    }

    Serial.print("Positions:\n");
    Serial.print("Lower ");
    Serial.print(lower_pos);
    Serial.print("\n");
    Serial.print("Upper ");
    Serial.print(upper_pos);
    Serial.print("\n");
    Serial.print("Start ");
    Serial.print(i_start);
    Serial.print("\n");
    Serial.print("End ");
    Serial.print(i_end);
    Serial.print("\n");

    Serial.print("Whole sequence\n");
    for(m=0;m<10;m++)
    {
      Serial.print(Braille_to_number(c_array[m].lu,c_array[m].lm,c_array[m].ll,c_array[m].ru,c_array[m].rm,c_array[m].rl));
      Serial.print(" ");
    }
    Serial.print("\n");


    //check for negative
    if(Braille_to_number(c_array[upper_pos+1].lu,c_array[upper_pos+1].lm,c_array[upper_pos+1].ll,c_array[upper_pos+1].ru,c_array[upper_pos+1].rm,c_array[upper_pos+1].rl)==101)
    {
      neg_u=true;
    }
    if(Braille_to_number(c_array[lower_pos+1].lu,c_array[lower_pos+1].lm,c_array[lower_pos+1].ll,c_array[lower_pos+1].ru,c_array[lower_pos+1].rm,c_array[lower_pos+1].rl)==101)
    {
      neg_l=true;
    }
  

   if(neg_l == true)
   {
     for(m = upper_pos-1; m > lower_pos+1; m--)
     {
         lower = lower + (power(10,upper_pos-1-m))*Braille_to_number(c_array[m].lu,c_array[m].lm,c_array[m].ll,c_array[m].ru,c_array[m].rm,c_array[m].rl);
     }
     lower = lower*(-1);
   }
   else
   {
     for(m = upper_pos-1; m > lower_pos; m--)
     {
         lower = lower + (power(10,upper_pos-1-m))*Braille_to_number(c_array[m].lu,c_array[m].lm,c_array[m].ll,c_array[m].ru,c_array[m].rm,c_array[m].rl);
     }
   }

   if(neg_u == true)
   {
     for(m = i_start-1; m > upper_pos+1; m--)
     {
         upper = upper + (power(10,i_start-1-m))*Braille_to_number(c_array[m].lu,c_array[m].lm,c_array[m].ll,c_array[m].ru,c_array[m].rm,c_array[m].rl);
     }
     upper = upper*(-1);
   }
   else
   {
     for(m = i_start-1; m > upper_pos; m--)
     {
         upper = upper + (power(10,i_start-1-m))*Braille_to_number(c_array[m].lu,c_array[m].lm,c_array[m].ll,c_array[m].ru,c_array[m].rm,c_array[m].rl);
     }
   }


    //lower = 0;
    //upper = 1;
    
    Serial.print("Limits:\n");
    Serial.print("下 ");
    Serial.print(lower);
    Serial.print("\n");
    Serial.print("上 ");
    Serial.print(upper);
    Serial.print("\n");

    BrailleCell intergrand[20];
    int Size = i_end - i_start;
    
    //pull out integrand
    for(m = i_start+1; m < i_end; m++)
    {
    intergrand[m-i_start-1] = c_array[m];
    }
     
    //convert braille format into double
    double integrand_num[20] = {0};
    double integrand_num_copy[20] = {0};

    for(m = 0; m < Size-1; m++)
    {
      integrand_num_copy[m] = integrand_num[m] = Braille_to_number(intergrand[m].lu,intergrand[m].lm,intergrand[m].ll,intergrand[m].ru,intergrand[m].rm,intergrand[m].rl);
    }

    //test
    /*
    integrand_num_copy[0]=integrand_num[0]=1.5;
    integrand_num_copy[1]=integrand_num[1]=113;
    integrand_num_copy[2]=integrand_num[2]=2;
    integrand_num_copy[3]=integrand_num[3]=103;
    integrand_num_copy[4]=integrand_num[4]=143;
    integrand_num_copy[5]=integrand_num[5]=102;
    integrand_num_copy[6]=integrand_num[6]=4;
    */

    /*
    integrand_num_copy[0]=integrand_num[0]=2;
    integrand_num_copy[1]=integrand_num[1]=113;
    integrand_num_copy[2]=integrand_num[2]=143;
    integrand_num_copy[3]=integrand_num[3]=103;
    integrand_num_copy[4]=integrand_num[4]=4;
    */
    
    /*
    integrand_num_copy[0]=integrand_num[0]=143;
    integrand_num_copy[1]=integrand_num[1]=113;
    integrand_num_copy[2]=integrand_num[2]=3;
    */
    Size = 5;
      
      
    Serial.print("Integrand\n");
    //display integrand to serial monitor
    for(m = 0; m < 10; m++)
    {
      Serial.print(integrand_num[m]);
      Serial.print(" ");
    }
    Serial.print("\n");
  
    //calculate all Y values
    double X = lower;
    double y_values[51]={0};
    int y_count = 0;
    double h = (upper-lower)/50;
    double result = 0;


    //find all operators in integrand
    o=0;
    for(m = 0; m <= Size; m++)
    {
      if(integrand_num[m]==101 || integrand_num[m]==102 || integrand_num[m]==103 || integrand_num[m]==104 || integrand_num[m]==113)
      {
       //find all operators and their positions
       oper[0][o] = integrand_num[m];
       oper[1][o] = m;
       o++;
      }
     }
    Serial.print("\nNumber of operators: ");
    Serial.print(o);
    Serial.print("\n");
    
    Serial.print("Print operators: ");
    for(m = 0; m < 5;m++)
    {     
     Serial.print(oper[0][m]);
     Serial.print(" ");
     Serial.print(oper[1][m]);
     Serial.print("  ");
    }
    Serial.print("\n");

    
    //sorting operators following BODMAS
    int i = 0, j = 0, tmp;
    for (i = 0; i < o; i++) 
    {   
        for (j = 0; j < o - i - 1; j++) 
        { 
            if (oper[0][j] > oper[0][j+1]) 
            {  
                tmp = oper[0][j];
                oper[0][j] = oper[0][j+1];
                oper[0][j+1] = tmp;

                tmp = oper[1][j];
                oper[1][j] = oper[1][j+1];
                oper[1][j+1] = tmp;
            }
        }
    }

    Serial.print("Print in order ");
    for(m = 0; m < 5;m++)
    {     
     Serial.print(oper[0][m]);
     Serial.print(" ");
     Serial.print(oper[1][m]);
     Serial.print("  ");
    }
    Serial.print("\n\n");

    int prio_oper = 0;
    int p_oper_pos = 0;
    double num_left = 0;
    double num_right = 0;
    double Y = 0;
    double res= 0;
    int tempSize = Size;
    int temp_o = o;
    int m2= 0; 
    
    //Size=Size-1;
    Serial.print("Size:\n");
    Serial.print(Size);
    Serial.print("\n");


    Serial.print("Pre entry integrand\n");
    for(m = 0; m < 10; m++)
    {
    Serial.print(integrand_num_copy[m]);
    Serial.print(" ");
    }
    Serial.print("\n");  

    //X=2.3;
    for(m2 = 0; m2 < 51; m2++)
    {
      //substitute all x_values
      for(m = 0; m < 10; m++)
      {
        if(integrand_num_copy[m] == 143)
        {
          integrand_num_copy[m] = X;
        }
      }
      
      Serial.print("Substitition of values\n");
      for(m = 0; m < 10; m++)
      {
      Serial.print(integrand_num_copy[m]);
      Serial.print(" ");
      }
      Serial.print("\n");         

      //default set at zero
      Y = 0;

      //
      temp_o = o;
      tempSize = Size;
      while(temp_o > 0)
        {
          //find largest operator
          prio_oper = integrand_num_copy[1];
          p_oper_pos = 0;
    
          Serial.print("Pull out operators: \n");     
          for (m = 1; m < Size; m=m+2)
          {
            Serial.print(integrand_num_copy[m]);
            Serial.print(" ");
            if (integrand_num_copy[m - 2] < integrand_num_copy[m])
            {
              prio_oper = integrand_num_copy[m];            
              p_oper_pos = m;
            }
          }
          /*
          if(p_oper_pos > 1)
          {
            p_oper_pos--;
          }
          //float and double only
          else if(p_oper_pos == 1)
          {
            p_oper_pos = 0;
          }
          */
          
          //prio_oper = oper[0][temp_o-1];
          //p_oper_pos = oper[1][temp_o-1];
          
          //Serial.print("Oper Pos");
          //Serial.print(prio_oper);
          //Serial.print(p_oper_pos);
          //Serial.print("\n");
          
          num_left = integrand_num_copy[p_oper_pos-1]; 
          num_right = integrand_num_copy[p_oper_pos+1];

          res = 999;
          //find operator type and calulcate result
          if(prio_oper == 102)//'+'
          {
           res = num_left + num_right;
          }
          else if(prio_oper == 101)//'-'
          {
            res = num_left - num_right;
          }
          else if(prio_oper == 104)//'/'
          {
            res = num_left / num_right;
          }
          else if(prio_oper == 103)//'*'
          {
            res = num_left * num_right;
          }
          else if(prio_oper == 113)//'^'
          {
            res = pow(num_left,num_right);
          }
      
          //re write the equation array
          //shift sequence two numbers to the left
          for(m = 0; m < Size - p_oper_pos; m++)
          {
            integrand_num_copy[p_oper_pos+m] = integrand_num_copy[p_oper_pos+2+m];
            integrand_num_copy[p_oper_pos+2+m] = 0;
          }
      

          //write result into the number before the operator
          integrand_num_copy[p_oper_pos-1] = res;

          /*
          Serial.print("Iteration\n");
          for(m = 0; m < 10; m++)
          {
          Serial.print(integrand_num_copy[m]);
          Serial.print(" ");
          }
          Serial.print("\n");   
          */

          temp_o--;
        }
        Y = integrand_num_copy[0];
      
      
      
      //Y = pow(X,4);
      //Y = 0;

      y_values[y_count] = Y;

      Serial.print(y_values[y_count]);
      Serial.print(" ");
      
      //re-extract equation
      for(m = 0; m < Size; m++)
      {
        integrand_num_copy[m]=integrand_num[m];
      }
      
      y_count++;
      X=X+h;
    }
    Serial.print("\n");
   


    //multiply all values apart from first and last by 2
    for(m = 1; m < 50; m++)
    {
      y_values[m]=2*y_values[m];
      //Serial.print(y_values[m]);
      //Serial.print(" ");
    }
    //Serial.print("\n");


    //add all values together
    for(m = 0; m < 51; m++)
    {
      result = result + y_values[m];
    }
    Serial.print(result);
    Serial.print("\n");

    //multiply by h/2
    result = result * h/2;

    //for integration of a constant only
    if(o=0 && integrand_num[0] != 143)
    {
      result = result*(upper - lower);
    }

    //display result
    Serial.print("Final result:\n");
    Serial.print(result);
    Serial.print("\n");

   
        
    display_result(result);
    complete();
    
    n=0;
    m=0;
    oper[0][0]=0;
    oper[0][1]=0;
    oper[0][2]=0;
    oper[0][3]=0;
    oper[0][4]=0;
    oper[1][0]=0;
    oper[1][1]=0;
    oper[1][2]=0;
    oper[1][3]=0;
    oper[1][4]=0; 

    //reset y values
    for(m = 0; m < 51; m++)
    {
      y_values[m] = 0;
    }
  }

  

  //quadratic formula
  Quadratic_state = digitalRead(Quadratic);
  if(Quadratic_state == true)
  {
  int sep[3] = {0};
  int s = 0;
  int a = 0;
  int b = 0;
  int c = 0;
  bool neg_a = false;
  bool neg_b = false;
  bool neg_c = false;
  
  //search for separator
  for(m = 0; m < n; m++)
  {
    if(Braille_to_number(c_array[m].lu,c_array[m].lm,c_array[m].ll,c_array[m].ru,c_array[m].rm,c_array[m].rl)==111)
    {
      sep[s] = m;
      s++;
    }
  }
  
   Serial.print("\n");
   Serial.print("Point found");
   Serial.print("\n");
   Serial.print(sep[0]);
   Serial.print(" ");
   Serial.print(sep[1]);
   Serial.print("\n");

  
  //pull out 3 numbers
  //check for negatives
  if(Braille_to_number(c_array[0].lu,c_array[0].lm,c_array[0].ll,c_array[0].ru,c_array[0].rm,c_array[0].rl)==101)
  {
    neg_a=true;
  }
  if(Braille_to_number(c_array[sep[0]+1].lu,c_array[sep[0]+1].lm,c_array[sep[0]+1].ll,c_array[sep[0]+1].ru,c_array[sep[0]+1].rm,c_array[sep[0]+1].rl)==101)
  {
    neg_b=true;
  }
  if(Braille_to_number(c_array[sep[1]+1].lu,c_array[sep[1]+1].lm,c_array[sep[1]+1].ll,c_array[sep[1]+1].ru,c_array[sep[1]+1].rm,c_array[sep[1]+1].rl)==101)
  {
    neg_c=true;
  }
  
  Serial.print("\nNegative values at:");
  Serial.print(neg_a);
  Serial.print(" ");
  Serial.print(neg_b);
  Serial.print(" ");
  Serial.print(neg_c);
  Serial.print("\n");
  
   if(neg_a ==true)
   {
     for(m = sep[0]-1; m >= 1; m--)
     {
         a = a + (power(10,oper[1][0]-1-m))*Braille_to_number(c_array[m].lu,c_array[m].lm,c_array[m].ll,c_array[m].ru,c_array[m].rm,c_array[m].rl);
     }
     a = a*(-1);
   }
   else
   {
     for(m = sep[0]-1; m >= 0; m--)
     {
         a = a + (power(10,oper[1][0]-1-m))*Braille_to_number(c_array[m].lu,c_array[m].lm,c_array[m].ll,c_array[m].ru,c_array[m].rm,c_array[m].rl);
     }
   }

   if(neg_b ==true)
   {
    for(m = sep[1]-1; m > sep[0]+1; m--)
    {
       b = b + (power(10,oper[1][0]-1-m))*Braille_to_number(c_array[m].lu,c_array[m].lm,c_array[m].ll,c_array[m].ru,c_array[m].rm,c_array[m].rl);
    }  
    b = b*(-1);
   }
   else
   {
     for(m = sep[1]-1; m > sep[0]; m--)
     {
         b = b + (power(10,oper[1][0]-1-m))*Braille_to_number(c_array[m].lu,c_array[m].lm,c_array[m].ll,c_array[m].ru,c_array[m].rm,c_array[m].rl);
     }
   }

   if(neg_c ==  true)
   {
     for(m = sep[2] - 1; m > sep[1]+1; m--) 
     {
       c = c + (power(10,oper[1][o-1]-1-m))*Braille_to_number(c_array[m].lu,c_array[m].lm,c_array[m].ll,c_array[m].ru,c_array[m].rm,c_array[m].rl);
     }
     c = c*(-1);
   }
   else
   {
      for(m = sep[2] - 1; m > sep[1]; m--) 
      {
       c = c + (power(10,oper[1][o-1]-1-m))*Braille_to_number(c_array[m].lu,c_array[m].lm,c_array[m].ll,c_array[m].ru,c_array[m].rm,c_array[m].rl);
      }
   }
   
  

   Serial.print("Quatratic Polynomial Coeffients: ");
   Serial.print("\n");
   Serial.print(a);
   Serial.print(" ");
   Serial.print(b);
   Serial.print(" ");
   Serial.print(c);
   Serial.print("\n");

   double des = b*b-4*a*c;
   Serial.print("\n");

   double X1 = 0;
   double X2 = 0;
   double rX = 0;
   double iX = 0;

   //real roots
   if(des >= 0)
   {
     X1 = ((-1)*b+sqrt(des))/(2*a);
     X2 = ((-1)*b-sqrt(des))/(2*a);
     Serial.print(X1);
     Serial.print(" ");
     Serial.print(X2);
     Serial.print("\n");
  
  
     //display result 1
     display_digit(143); //'X'
     display_digit(105); //'='
     display_result(X1);
     
     //space
     display_digit(100);
  
     //display result 2
     display_digit(143); //'X'
     display_digit(105); // '='    
     display_result(X2);
     
     complete();
   }
   //imaginary roots
   else
   {
     des = des*(-1);
  
     rX = ((-1)*(double)b)/(2*(double)a);
     iX = (sqrt(des))/(2*a);

     Serial.print(rX);
     Serial.print("+-");
     Serial.print(iX);
     Serial.print("i\n");

     //display result 1
     display_digit(143); //'X'
     display_digit(105); //'='

     display_result(rX); //Re
     display_digit(102); //'+'
     display_digit(101); //'-'
     display_result(iX); //Im1
     display_digit(116); //imagniary sign

     complete();
     
   }

  des = n = m = s = a = b = c = 0;
  X1=X2=rX=iX=0;
  sep[0]=sep[1]=sep[2]=0;
  neg_a = neg_b = neg_c =false;
  }

  
  
  //evaluate equation
  Execute_state = digitalRead(Execute);
  if(Execute_state == true)
  {
    //equation length: n
    len = n;

    //print equation length
     p_ct_val = Number_to_Braille(n);
    digitalWrite(led_lu, *(p_ct_val));
    digitalWrite(led_lm, *(p_ct_val+1));
    digitalWrite(led_ll, *(p_ct_val+2));
    digitalWrite(led_ru, *(p_ct_val+3));
    digitalWrite(led_rm, *(p_ct_val+4));
    digitalWrite(led_rl, *(p_ct_val+5)); 
    delay(1000);
    digitalWrite(led_lu, LOW);
    digitalWrite(led_lm, LOW);
    digitalWrite(led_ll, LOW);
    digitalWrite(led_ru, LOW);
    digitalWrite(led_rm, LOW);
    digitalWrite(led_rl, LOW); 
    delay(1000);



    
    //search for operator
    o=0;
    for(m = 0; m <= n; m++)
    {
      if(Braille_to_number(c_array[m].lu,c_array[m].lm,c_array[m].ll,c_array[m].ru,c_array[m].rm,c_array[m].rl)==101 ||
      Braille_to_number(c_array[m].lu,c_array[m].lm,c_array[m].ll,c_array[m].ru,c_array[m].rm,c_array[m].rl)==102 ||
      Braille_to_number(c_array[m].lu,c_array[m].lm,c_array[m].ll,c_array[m].ru,c_array[m].rm,c_array[m].rl)==103 ||
      Braille_to_number(c_array[m].lu,c_array[m].lm,c_array[m].ll,c_array[m].ru,c_array[m].rm,c_array[m].rl)==104 ||
      Braille_to_number(c_array[m].lu,c_array[m].lm,c_array[m].ll,c_array[m].ru,c_array[m].rm,c_array[m].rl)==113 ||
      Braille_to_number(c_array[m].lu,c_array[m].lm,c_array[m].ll,c_array[m].ru,c_array[m].rm,c_array[m].rl)==114)
      {
       //find all operators and their positions
       oper[0][o] = Braille_to_number(c_array[m].lu,c_array[m].lm,c_array[m].ll,c_array[m].ru,c_array[m].rm,c_array[m].rl);
       oper[1][o] = m;
       o++;
      }
     } 

     
      //convert to array of sign
      double eqn_arr[15] = {0};


      //decimal points
      int dp_pos = 0;
      bool dp_present = false;

      //find decimal point
      for(m = oper[1][0]-1; m >= 0; m--)
      {
        if(Braille_to_number(c_array[m].lu,c_array[m].lm,c_array[m].ll,c_array[m].ru,c_array[m].rm,c_array[m].rl)==115) //115 = decimal point 
        {
          dp_pos = m;
          dp_present = true;
        }
      }

      Serial.print("Decimal Point present in first number: ");
      Serial.print("\n");
      Serial.print(dp_present);
      Serial.print("\n");
      Serial.print(dp_pos);
      Serial.print("\n");

      if(dp_present == false)
      {
        for(m = oper[1][0]-1; m >= 0; m--)
        {
          eqn_arr[0] = eqn_arr[0] + (power(10,oper[1][0]-1-m))*Braille_to_number(c_array[m].lu,c_array[m].lm,c_array[m].ll,c_array[m].ru,c_array[m].rm,c_array[m].rl);
        }
      }
      else if(dp_present == true)
      {
        
        for(m = dp_pos-1; m >= 0; m--)
        {
          eqn_arr[0] = eqn_arr[0] + (power(10,dp_pos-1-m))*Braille_to_number(c_array[m].lu,c_array[m].lm,c_array[m].ll,c_array[m].ru,c_array[m].rm,c_array[m].rl);
        }
        
        
        
        for(m = oper[1][0]-1; m >= dp_pos+1; m--)
        {
          eqn_arr[0] = eqn_arr[0] + (Braille_to_number(c_array[m].lu,c_array[m].lm,c_array[m].ll,c_array[m].ru,c_array[m].rm,c_array[m].rl))/((double)power(10,oper[1][0]-1-m));
        }  
        
      }  
      dp_present = false;   
      
      eqn_arr[1]=oper[0][0];
      

      

      //do for number of operators
      int c = 2;
      int mm = 1; 
      int mmm = 0;     
      
      for(mmm = 0; mmm < o-1; mmm++)
      {
        for(m = oper[1][mm]-1; m > oper[1][mm-1]; m--)
        {
          eqn_arr[c] = eqn_arr[c] + (power(10,oper[1][1]-1-m))*Braille_to_number(c_array[m].lu,c_array[m].lm,c_array[m].ll,c_array[m].ru,c_array[m].rm,c_array[m].rl);
        }      
        eqn_arr[c+1]=oper[0][mmm+1];
      
        c = c + 2;
      }     

      for(m = len-1; m > oper[1][o-1]; m--)
      {
      eqn_arr[c] = eqn_arr[c] + (power(10,oper[1][o-1]-1-m))*Braille_to_number(c_array[m].lu,c_array[m].lm,c_array[m].ll,c_array[m].ru,c_array[m].rm,c_array[m].rl);
      }

      for(m = 0; m < c+1; m++)
      {    
      Serial.print(eqn_arr[m]);
      Serial.print(" ");
      }
      Serial.print("\n");
      



      int prio_oper = 0;
      int p_oper_pos = 0;
      double num_left = 0;
      double num_right = 0;

      while(c > 1)
      {
      //find heaviest operator
      prio_oper = eqn_arr[1];
      p_oper_pos = 0;

      Serial.print("Pull out operators: \n");     
      for (m = 1; m < c+1; m=m+2)
      {
        Serial.print(eqn_arr[m]);
        Serial.print(" ");
        if (eqn_arr[m - 2] < eqn_arr[m])
        {
          prio_oper = eqn_arr[m];            
          p_oper_pos = m;
        }
      }
      if(p_oper_pos > 1)
      {
        p_oper_pos--;
      }
      //float and double only
      else if(p_oper_pos == 1)
      {
        p_oper_pos = 0;
      }
      
      Serial.print("\n");
      Serial.print("Largest operator: "); 
      Serial.print(prio_oper);
      Serial.print("\n");
      Serial.print("Largest operator pos: "); 
      Serial.print(p_oper_pos);
      Serial.print("\n");



    //set two numbers for processing
    num_left = eqn_arr[p_oper_pos];
    num_right = eqn_arr[p_oper_pos+2];  

    result = 999;
    //find operator type and calulcate result
    if(prio_oper == 102)//'+'
    {
     result = num_left + num_right;
    }
    else if(prio_oper == 101)//'-'
    {
      result = num_left - num_right;
    }
    else if(prio_oper == 104)//'/'
    {
      result = num_left / num_right;
    }
    else if(prio_oper == 103)//'*'
    {
      result = num_left * num_right;
    }
    else if(prio_oper == 113)//'^'
    {
      result = power(num_left,num_right);
    }
    else if(prio_oper == 114)//'f modulo remainder'
    {
      result = (int)num_left % (int)num_right;
    }

    //re write the equation array
    //shift sequence two numbers to the left
    for(m = 0; m < len - p_oper_pos; m++)
    {
    eqn_arr[p_oper_pos+m] = eqn_arr[p_oper_pos+2+m];
    eqn_arr[p_oper_pos+2+m] = 0;
    }

    Serial.print("Result ");
    Serial.print(result);
    Serial.print("\n");

    //write result into the number before the operator
    eqn_arr[p_oper_pos] = result;
    
    Serial.print("Print truncated sequence \n");

     //decrease length and c by 2 
     
     len = len - 2;
     c = c - 2;

     //display new sequence
     for(m = 0; m < 15; m++)
     {    
     Serial.print(eqn_arr[m]);
     Serial.print(" ");
     }
     Serial.print("\n");   
    }
     
     Serial.print("\n");

    //for testing, result is now the first element of the array.
    result = eqn_arr[0];


    //display result
    display_result(result);
    complete();
      
    //reset n to 0 for capital equation)
    n = 0;

    //reset variables
    for(m = 0; m < 15; m++)
    {
      eqn_arr[m] = 0;
    }
    m = 0;
    o=0;
    result = 0;
    res_trans = 0;
    num_of_digits = 0;

    oper[0][0]=0;
    oper[0][1]=0;
    oper[0][2]=0;
    oper[0][3]=0;
    oper[0][4]=0;
    oper[1][0]=0;
    oper[1][1]=0;
    oper[1][2]=0;
    oper[1][3]=0;
    oper[1][4]=0;
  }
  
  NEXT_STATE = LOW;

  
 delay(100);
}

#include <SPI.h>
#include <SD.h>
File myFile;

void setup(){
  Serial.begin(9600);
  Serial.print("Initializing SD card...");

  if (!SD.begin()) {
    Serial.println("initialization failed!");
    return;
    }
  
  Serial.println("initialization done.");
}

void loop(){
}


// FUNCTION TO WRITE IN FILE
void write_file(String text, String file_name){
  
  // Open file. If it doesn't exist, creates file
  file_name += ".txt";
  myFile = SD.open(file_name, FILE_WRITE);

  // Write in file
  if (myFile) {
    Serial.print("Writing to test.txt...");
    myFile.print(text);
    myFile.close();
    Serial.println(" done.");
  } else {
    Serial.println("error opening test.txt");
  }
}


// FUNCTION TO READ FILE
void read_file(String file_name){

  file_name += ".txt";
  myFile = SD.open(file_name);
  if (myFile) {
    Serial.println("test.txt:");

    // read from the file until there's nothing else in it:
    while (myFile.available()) {
      Serial.write(myFile.read());
    }
    
    myFile.close();
  } else {
    Serial.println("error opening test.txt");
  }
}
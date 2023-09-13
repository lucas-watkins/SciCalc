//include keypad library
#include <Keypad.h>
//includ lcd library
#include <LiquidCrystal.h>
//include math parser libraries
#include <math.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

//constant pi for math parser
#define PI 3.14159265358979323846
//constant e for math parser
#define E 2.71828182845904523536

//initalize base equation string along with ans variable
String equation = "";
double ans;
String strAns;

//calc uses degrees
bool degreesTrue = true;
//shift active
bool shifted = false;

// setup lcd display
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);

//setup buttons and leds
const int clearBtn = 45;
const int delBtn = 44;
const int leftParentheses = 43;
const int rightParentheses = 42;
const int sqrtBtn = 41;
const int expBtn = 40;
const int shiftBtn = 39;
const int shiftLED = 38;
const int degLED = 37;
const int radLED = 36;

//setup keypad
const byte ROWS = 4;
const byte COLS = 4;

byte rowPins[ROWS] = { 53, 52, 51, 50 };
byte colPins[COLS] = { 49, 48, 47, 46 };

char hexaKeys[4][4] = {
  { '1', '2', '3', '+' },
  { '4', '5', '6', '-' },
  { '7', '8', '9', '*' },
  { '.', '0', '#', '/' }
};
Keypad keypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

//set decimal place to display calculations too
const int floatMode = 6;

void setup() {
  // put your setup code here, to run once:
  //declare lcd resolution
  lcd.begin(16, 2);
  //initalize button pins
  //button 1 - clear; shift = sine function
  pinMode(clearBtn, INPUT_PULLUP);
  //button 2 - delete; shift = cosine function
  pinMode(delBtn, INPUT_PULLUP);
  //button 3 - left parentheses; shift = tangent function
  pinMode(leftParentheses, INPUT_PULLUP);
  //button 4 - right parentheses; shift = toggle rad / deg
  pinMode(rightParentheses, INPUT_PULLUP);
  //button 5 - square root; shift = logarithmic function
  pinMode(sqrtBtn, INPUT_PULLUP);
  //button 6 - exponents
  pinMode(expBtn, INPUT_PULLUP);
  //button 7 - shift button
  pinMode(shiftBtn, INPUT_PULLUP);
  //Shift Led
  pinMode(shiftLED, OUTPUT);
  //splash screen
  lcd.print("SciCalc [1-A]");
  delay(2000);
  lcd.clear();
  //make cursor blink
  lcd.blink();
  //light led depending on mode
  if (degreesTrue) {
    digitalWrite(degLED, HIGH);
  } else {
    digitalWrite(radLED, HIGH);
  }
  //begin serial monitor
  Serial.begin(9600);
  //clear serial monitor
  Serial.println("=========================================");
}

void loop() {
  // put your main code here, to run repeatedly:
  //handle autoscroll
  if(anyKeyPressed() && equation.length() >= 14){
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(equation.substring(equation.length()-14));
    delay(10);

  }
  //handle clear button
  if (digitalRead(clearBtn) == LOW) {
    //handle shift functionality for sine function
    if (shifted) {
      equation += "sin(";
      Serial.println(equation);
      printEquation();
      digitalWrite(shiftLED, LOW);
      shifted = !shifted;
      delay(250);
    } else {
      Serial.println("Clear button pressed");
      lcd.clear();
      lcd.setCursor(0, 0);
      equation = "";
      delay(250);
    }
  }
  //handle delete button
  if (digitalRead(delBtn) == LOW) {
    //handle shift functionality for cosine function
    if (shifted) {
      equation += "cos(";
      Serial.println(equation);
      printEquation();
      digitalWrite(shiftLED, LOW);
      shifted = !shifted;
      delay(250);
    } else {
      equation = equation.substring(0, equation.length() - 1);
      Serial.println("Delete Button Pressed, Equation Substring: " + equation);
      lcd.clear();
      printEquation();
      lcd.setCursor(equation.length(), 0);
      delay(250);
    }
  }
  //handle left parentheses button
  if (digitalRead(leftParentheses) == LOW) {
    //handle shift functionality for tangent function
    if (shifted) {
      equation += "tan(";
      Serial.println(equation);
      printEquation();
      digitalWrite(shiftLED, LOW);
      shifted = !shifted;
      delay(250);
    } else {
      equation += "(";
      Serial.println(equation);
      lcd.print("(");
      delay(250);
    }
  }
  //handle right parentheses button
  if (digitalRead(rightParentheses) == LOW) {
    //handle switch to rad / deg
    if (shifted) {
      if (degreesTrue) {
        digitalWrite(degLED, LOW);
        digitalWrite(radLED, HIGH);
      } else {
        digitalWrite(radLED, LOW);
        digitalWrite(degLED, HIGH);
      }
      degreesTrue = !degreesTrue;
      digitalWrite(shiftLED, LOW);
      shifted = !shifted;
      delay(250);
    } else {
      equation += ")";
      Serial.println(equation);
      lcd.print(")");
      delay(250);
    }
  }
  //handle Square root button 
  if (digitalRead(sqrtBtn) == LOW) {
    //handle log when shifted
    if(shifted){
      equation += "log(";
      printEquation();
      Serial.println(equation);
      digitalWrite(shiftLED, LOW);
      shifted = !shifted;
      delay(250);
    } else{
    equation += "sqrt(";
    Serial.println(equation);
    lcd.print("sqrt(");
    delay(250);
    }
  }
  //handle exponent button
  if (digitalRead(expBtn) == LOW) {
    if (shifted) {
      equation += strAns;
      Serial.println(equation);
      printEquation();
      digitalWrite(shiftLED, LOW);
      shifted = !shifted;
      delay(250);
    } 
    else{
    equation += "^";
    Serial.println(equation);
    lcd.print("^");
    delay(250);
    }
  }
  //handle shift button
  if (digitalRead(shiftBtn) == LOW) {
    shifted = !shifted;
    Serial.println("Shift Button Pressed. Shift State: ");
    Serial.println(shifted);
    if (shifted == true) {
      digitalWrite(shiftLED, HIGH);
    } else {
      digitalWrite(shiftLED, LOW);
    }
    delay(250);
  }
  char keystroke = keypad.getKey();
  //convert keystroke to string
  String stringKeystroke(keystroke);
  //handle any button press but evauluate button
  if (stringKeystroke != NULL && stringKeystroke != "#" && stringKeystroke != "." && stringKeystroke != "+" && stringKeystroke != "-") {
    equation += stringKeystroke;
    Serial.println(equation);
    //lcd.print(keystroke);
    printEquation();
  }
  else if (stringKeystroke == "."){
    if(shifted){
      equation += ",";
      //lcd.print(",");
      digitalWrite(shiftLED, LOW);
      shifted = !shifted;
      printEquation();
    }
    else{
      equation += ".";
      //lcd.print(".");
      printEquation();
    }
    Serial.println(equation);
  }
  else if (stringKeystroke == "+"){
    if(shifted){
      equation += "pi";
      lcd.print("pi");
      digitalWrite(shiftLED, LOW);
      shifted = !shifted;
      printEquation();
    }
    else{
      equation += "+";
      //lcd.print("+");
      printEquation();
    }
    Serial.println(equation);
  }
  else if (stringKeystroke == "-"){
    if(shifted){
      equation += "e";
      //lcd.print("e");
      digitalWrite(shiftLED, LOW);
      shifted = !shifted;
      printEquation();
    }
    else{
      equation += "-";
      //lcd.print("-");
      printEquation();
    }
    Serial.println(equation);
  }
  //handle evaluate button press
  else if (stringKeystroke == "#") {
    //convert string to char for math parser
    char eq[equation.length() + 1];
    equation.toCharArray(eq, equation.length() + 1);
    //serial line debug
    Serial.println("EQ:");
    Serial.println(eq);
    Serial.println("END EQ");

    Serial.println(equation);
    if (degreesTrue) {
      ans = mathParseDeg(eq);
      strAns = String(ans, floatMode);
    } else {
      ans = mathParseRad(eq);
      strAns = String(ans, floatMode);
    }

    if (fmod(ans, 1.0) == 0.0) {
      Serial.println("Rounding to Int32");
      ans = long(ans);
      strAns = String(ans,0);
      lcd.setCursor(16 - String(ans, 0).length(), 1);
      if (ans == -2147483648) {
        lcd.setCursor(4, 1);
        Serial.println("Int Overflow");
        lcd.print("Int Overflow");
      }
      //catch tangent undefined
      else if (ans == -22845570 && equation.indexOf("t") != -1) {
        lcd.setCursor(13, 1);
        lcd.print("nan");
        Serial.println("Tangent Undefined Error");
      } else {
        lcd.print(ans, 0);
      }
    } else {
      lcd.setCursor(16 - String(ans, floatMode).length(), 1);

      lcd.print(ans, floatMode);
    }
    //enter evaluate key to clear previous answer
    while (true) {
      keystroke = keypad.getKey();
      if (keystroke == '#' || digitalRead(clearBtn) == LOW) {
        lcd.clear();
        lcd.setCursor(0, 0);
        equation = "";
        break;
      }
    }
  }
  //delay for stability
  else { delay(10); }
}
double mathParseRad(const char *str) {
  double stack[32];
  int top = -1;
  char opstack[32];
  int optop = -1;
  const char *p = str;
  while (*p) {
    while (isspace(*p)) p++;
    if (isdigit(*p) || *p == '.') {
      double val = strtod(p, (char **)&p);
      stack[++top] = val;
    } else if (*p == '(') {
      opstack[++optop] = *p++;
    } else if (*p == ')') {
      while (optop >= 0 && opstack[optop] != '(') {
        char op = opstack[optop--];
        double b = stack[top--];
        double a = stack[top];
        if (op == '+') stack[top] = a + b;
        else if (op == '-') stack[top] = a - b;
        else if (op == '*') stack[top] = a * b;
        else if (op == '/') stack[top] = a / b;
        else if (op == '^') stack[top] = pow(a, b);
        else if (op == 's') {
          stack[++top] = sin(b * PI / 180);
        } else if (op == 'c') {
          stack[++top] = cos(b * PI / 180);
        } else if (op == 't') {
          stack[++top] = tan(b * PI / 180);
        } else if (op == 'S') {
          stack[++top] = asin(b) * 180 / PI;
        } else if (op == 'C') {
          stack[++top] = acos(b) * 180 / PI;
        } else if (op == 'T') {
          stack[++top] = atan(b) * 180 / PI;
        } else if (op == 'r') {
          stack[++top] = sqrt(b);
        } else if (op == 'l') {
          stack[top] = log(b) / log(a);
        }
      }
      optop--;
      p++;
    } else if (*p == '+' || *p == '-' || *p == '*' || *p == '/' || *p == '^' || *p == 'l') {
      int prec = (*p == '+' || *p == '-') ? 1 : ((*p == '*' || *p == '/') ? 2 : ((*p == '^' || *p == 'l') ? 3 : 4));
      while (optop >= 0 && opstack[optop] != '(') {
        char op = opstack[optop];
        int opprec = (op == '+' || op == '-') ? 1 : ((op == '*' || op == '/') ? 2 : ((op == '^' || op == 'l') ? 3 : 4));
        if (opprec < prec || (opprec == prec && op != '^')) break;
        optop--;
        double b = stack[top--];
        double a = stack[top];
        if (op == '+') stack[top] = a + b;
        else if (op == '-') stack[top] = a - b;
        else if (op == '*') stack[top] = a * b;
        else if (op == '/') stack[top] = a / b;
        else if (op == '^') stack[top] = pow(a, b);
        else if (op == 'l') stack[top] = log(b) / log(a);
      }
      opstack[++optop] = *p++;
    } else if (*p == 's' && *(p + 1) == 'i' && *(p + 2) == 'n' && *(p + 3) == '(') {
      p += 3;
      opstack[++optop] = 's';
    } else if (*p == 'c' && *(p + 1) == 'o' && *(p + 2) == 's' && *(p + 3) == '(') {
      p += 3;
      opstack[++optop] = 'c';
    } else if (*p == 't' && *(p + 1) == 'a' && *(p + 2) == 'n' && *(p + 3) == '(') {
      p += 3;
      opstack[++optop] = 't';
    } else if (*p == 'a' && *(p + 1) == 's' && *(p + 2) == 'i' && *(p + 3) == 'n' && *(p + 4) == '(') {
      p += 4;
      opstack[++optop] = 'S';
    } else if (*p == 'a' && *(p + 1) == 'c' && *(p + 2) == 'o' && *(p + 3) == 's' && *(p + 4) == '(') {
      p += 4;
      opstack[++optop] = 'C';
    } else if (*p == 'a' && *(p + 1) == 't' && *(p + 2) == 'a' && *(p + 3) == 'n' && *(p + 4) == '(') {
      p += 4;
      opstack[++optop] = 'T';
    } else if (*p == 's' && *(p + 1) == 'q' && *(p + 2) == 'r' && *(p + 3) == 't' && *(p + 4) == '(') {
      p += 4;
      opstack[++optop] = 'r';
    } else if (*p == 'l' && *(p + 1) == 'o' && *(p + 2) == 'g' && (*(p + 3) == '(' || isspace(*(p + 3)))) {
      p += 3;
      opstack[++optop] = 'l';
    } else if (*p == 'e') {
      stack[++top] = E;
      p++;
    } else if (*p == 'p' && *(p + 1) == 'i') {
      stack[++top] = PI;
      p += 2;
    } else {
      p++;
    }
  }
  while (optop >= 0) {
    char op = opstack[optop--];
    double b = stack[top--];
    double a = stack[top];
    if (op == '+') stack[top] = a + b;
    else if (op == '-') stack[top] = a - b;
    else if (op == '*') stack[top] = a * b;
    else if (op == '/') stack[top] = a / b;
    else if (op == '^') stack[top] = myPow(a, b);
    else if (op == 's') {
      stack[++top] = sin(b);
    } else if (op == 'c') {
      stack[++top] = cos(b);
    } else if (op == 't') {
      stack[++top] = tan(b);
    } else if (op == 'S') {
      stack[++top] = asin(b);
    } else if (op == 'C') {
      stack[++top] = acos(b);
    } else if (op == 'T') {
      stack[++top] = atan(b);
    } else if (op == 'r') {
      stack[++top] = sqrt(b);
    } else if (op == 'l') {
      stack[top] = log(b) / log(a);
    }
  }
  return top >= 0 ? stack[top] : NAN;
}
double myPow(double a, double b) {
  if (b == 0) return 1;
  if (b < 0) return 1 / myPow(a, -b);
  int intPart = (int)b;
  double fracPart = b - intPart;
  double intResult = 1;
  for (int i = 0; i < intPart; i++) {
    intResult *= a;
  }
  double fracResult = exp(fracPart * log(a));
  return intResult * fracResult;
}


double mathParseDeg(const char *str) {
  double stack[32];
  int top = -1;
  char opstack[32];
  int optop = -1;
  const char *p = str;
  while (*p) {
    while (isspace(*p)) p++;
    if (isdigit(*p) || *p == '.') {
      double val = strtod(p, (char **)&p);
      stack[++top] = val;
    } else if (*p == '(') {
      opstack[++optop] = *p++;
    } else if (*p == ')') {
      while (optop >= 0 && opstack[optop] != '(') {
        char op = opstack[optop--];
        double b = stack[top--];
        double a = stack[top];
        if (op == '+') stack[top] = a + b;
        else if (op == '-') stack[top] = a - b;
        else if (op == '*') stack[top] = a * b;
        else if (op == '/') stack[top] = a / b;
        else if (op == '^') stack[top] = pow(a, b);
        else if (op == 's') {
          stack[++top] = sin(b * PI / 180);
        } else if (op == 'c') {
          stack[++top] = cos(b * PI / 180);
        } else if (op == 't') {
          stack[++top] = tan(b * PI / 180);
        } else if (op == 'S') {
          stack[++top] = asin(b) * 180 / PI;
        } else if (op == 'C') {
          stack[++top] = acos(b) * 180 / PI;
        } else if (op == 'T') {
          stack[++top] = atan(b) * 180 / PI;
        } else if (op == 'r') {
          stack[++top] = sqrt(b);
        } else if (op == 'l') {
          stack[top] = log(b) / log(a);
        }
      }
      optop--;
      p++;
    } else if (*p == '+' || *p == '-' || *p == '*' || *p == '/' || *p == '^' || *p == 'l') {
      int prec = (*p == '+' || *p == '-') ? 1 : ((*p == '*' || *p == '/') ? 2 : ((*p == '^' || *p == 'l') ? 3 : 4));
      while (optop >= 0 && opstack[optop] != '(') {
        char op = opstack[optop];
        int opprec = (op == '+' || op == '-') ? 1 : ((op == '*' || op == '/') ? 2 : ((op == '^' || op == 'l') ? 3 : 4));
        if (opprec < prec || (opprec == prec && op != '^')) break;
        optop--;
        double b = stack[top--];
        double a = stack[top];
        if (op == '+') stack[top] = a + b;
        else if (op == '-') stack[top] = a - b;
        else if (op == '*') stack[top] = a * b;
        else if (op == '/') stack[top] = a / b;
        else if (op == '^') stack[top] = pow(a, b);
        else if (op == 'l') stack[top] = log(b) / log(a);
      }
      opstack[++optop] = *p++;
    } else if (*p == 's' && *(p + 1) == 'i' && *(p + 2) == 'n' && *(p + 3) == '(') {
      p += 3;
      opstack[++optop] = 's';
    } else if (*p == 'c' && *(p + 1) == 'o' && *(p + 2) == 's' && *(p + 3) == '(') {
      p += 3;
      opstack[++optop] = 'c';
    } else if (*p == 't' && *(p + 1) == 'a' && *(p + 2) == 'n' && *(p + 3) == '(') {
      p += 3;
      opstack[++optop] = 't';
    } else if (*p == 'a' && *(p + 1) == 's' && *(p + 2) == 'i' && *(p + 3) == 'n' && *(p + 4) == '(') {
      p += 4;
      opstack[++optop] = 'S';
    } else if (*p == 'a' && *(p + 1) == 'c' && *(p + 2) == 'o' && *(p + 3) == 's' && *(p + 4) == '(') {
      p += 4;
      opstack[++optop] = 'C';
    } else if (*p == 'a' && *(p + 1) == 't' && *(p + 2) == 'a' && *(p + 3) == 'n' && *(p + 4) == '(') {
      p += 4;
      opstack[++optop] = 'T';
    } else if (*p == 's' && *(p + 1) == 'q' && *(p + 2) == 'r' && *(p + 3) == 't' && *(p + 4) == '(') {
      p += 4;
      opstack[++optop] = 'r';
    } else if (*p == 'l' && *(p + 1) == 'o' && *(p + 2) == 'g' && (*(p + 3) == '(' || isspace(*(p + 3)))) {
      p += 3;
      opstack[++optop] = 'l';
    } else if (*p == 'e') {
      stack[++top] = E;
      p++;
    } else if (*p == 'p' && *(p + 1) == 'i') {
      stack[++top] = PI;
      p += 2;
    } else {
      p++;
    }
  }
  while (optop >= 0) {
    char op = opstack[optop--];
    double b = stack[top--];
    double a = stack[top];
    if (op == '+') stack[top] = a + b;
    else if (op == '-') stack[top] = a - b;
    else if (op == '*') stack[top] = a * b;
    else if (op == '/') stack[top] = a / b;
    else if (op == '^') stack[top] = myPow(a, b);
    else if (op == 's') {
      stack[++top] = sin(b * PI / 180);
    } else if (op == 'c') {
      stack[++top] = cos(b * PI / 180);
    } else if (op == 't') {
      stack[++top] = tan(b * PI / 180);
    } else if (op == 'S') {
      stack[++top] = asin(b) * 180 / PI;
    } else if (op == 'C') {
      stack[++top] = acos(b) * 180 / PI;
    } else if (op == 'T') {
      stack[++top] = atan(b) * 180 / PI;
    } else if (op == 'r') {
      stack[++top] = sqrt(b);
    } else if (op == 'l') {
      stack[top] = log(b) / log(a);
    }
  }
  return top >= 0 ? stack[top] : NAN;
}
bool anyKeyPressed(){
  return (digitalRead(clearBtn) == LOW || digitalRead(delBtn) == LOW || digitalRead(leftParentheses) == LOW || digitalRead(rightParentheses) == LOW || digitalRead(sqrtBtn) == LOW || digitalRead(expBtn) == LOW);
}
//check if display needs to scroll and if true do it
void printEquation(){
  if(equation.length() >= 14){
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(equation.substring(equation.length()-14));
    delay(10);
  }
  else{lcd.clear(); lcd.print(equation);}
}
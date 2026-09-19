#include <math.h>

double rx1 = 0;
double ry1 = -0.02;
double rz1 = 0;
double rx2 = -0.01;
double ry2 = 0.015;
double rz2 = 0;
double rx3 = 0.01;
double ry3 = 0.015;
double rz3 = 0;
double b [6] [1] = {
  {0},
  {0},
  {0},
  {0},
  {0},
  {1},
};
double L [6] [6] = {0};
double A [6] [9] = {
  {1, 0, 0, 1, 0, 0, 1, 0, 0},
  {0, 1, 0, 0, 1, 0, 0, 1, 0},
  {0, 0, 1, 0, 0 ,1, 0, 0, 1},
  {0, -rz1, ry1, 0, -rz2, ry2, 0, -rz3, ry3},
  {rz1, 0, -rx1, rz2, 0, -rx2, rz3, 0, -rx3},
  {-ry1, rx1, 0, -ry2, rx2, 0, -ry3, rx3, 0},
};

double gimbalangles [6] [1] = {0};
double Ap [9] [6] = {0};
double Ft_i [3] [1] = {0};
double x [9] [1] = {0};

void setup(){

  Serial.begin(9600);
  if(!Serial){
    while(1);
  }

  //***// MATRIX P CALCULATION (A·A^T) //***//

  double summation;
  double P [6] [6];
  for(int i = 0; i < 6; i++){
    for(int k = 0; k < 6; k++){
      summation = 0;
      for(int j = 0; j < 9; j++){
        summation = summation + A [i] [j] * A [k] [j];
      }
      P [i] [k] = summation;
    }
  }

  //***// MATRIX L CALCULATION (Cholesky)//***//

  double summation1;
  double summation2;

  L [0] [0] = sqrt(P [0] [0]);
  for(int i = 1; i < 6; i++){
    for(int j = 0; j < i; j++){
      summation1 = 0;
      for(int k = 0; k < j; k++){
        summation1 = summation1 + (L [i] [k] * L [j] [k]);
      }
      L [i] [j] = (1.0/L [j] [j]) * (P [i] [j] - summation1); 
    }
    summation2 = 0;
    for(int k = 0; k < i; k++){
      summation2 = summation2 + (L [i] [k] * L [i] [k]);
    }
    double nonNegativeValue = (P [i] [i] - summation2);
    if(nonNegativeValue < 0) nonNegativeValue = 0;
    L [i] [i] = sqrt(nonNegativeValue);
  }

  double max_value [9] [1] = {0};
  double act_value [9] [1] = {0};
    
  //***// L·y = b SOLVING //***//

  double y [6] [1] = {0};
  double summation3;

  for(int i = 0; i < 6; i++){
    summation3 = 0;
    for(int j = 0; j < i; j++){
      summation3 = summation3 + L [i] [j] * y [j] [0];
    }
    y [i] [0] = (b [i] [0] - summation3)/L [i] [i];
  }

  //***// L^T·u = y SOLVING //***//

  double u [6] [1] = {0};
  double summation4;

  for(int i = 5; i >= 0; i--){
    summation4 = 0;
    for(int j = i + 1; j < 6; j++){
      summation4 = summation4 + L [j] [i] * u [j] [0];
    }
    u [i] [0] = (y [i] [0] - summation4)/L [i] [i];
  }

  //***// MATRIX x CALCULATION (A^T·u) //***//

  double summation5;

  for(int i = 0; i < 9; i++){
    summation5 = 0;
    for(int j = 0; j < 6; j++){
      summation5 = summation5 + (A [j] [i] * u [j] [0]);
    }
    x [i] [0] = summation5;
  }

  //***// OBTENTION OF THE PSEUDOINVERSE MATRIX A^+ (Ap) //***//

  double e_i [6] [1] = {0};
  double y_i [6] [1] = {0};
  double u_i [6] [1] = {0};

  for(int i = 0; i < 6; i++){
    for(int j = 0; j < 6; j++){
    y_i [j][0]=0.0;
    u_i [j][0]=0.0;
    }
    for(int j = 0; j < 6; j++) e_i [j] [0] = 0.0;
    e_i [i] [0] = 1.0;
    for(int m = 0; m < 6; m++){ //***// L·y_i = e_i SOLVING //***//
      summation3 = 0;
      for(int n = 0; n < m; n++){
        summation3 = summation3 + L [m] [n] * y_i [n] [0];
      }
      y_i [m] [0] = (e_i [m] [0] - summation3)/L [m] [m];
    }
    for(int m = 5; m >= 0; m--){ //***// L^T·u_i = y_i SOLVING //***//
      summation4 = 0;
      for(int n = m + 1; n < 6; n++){
        summation4 = summation4 + L [n] [m] * u_i [n] [0];
      }
      u_i [m] [0] = (y_i [m] [0] - summation4)/L [m] [m];
    }
    for(int m = 0; m < 9; m++){ //***// MATRIX x_i CALCULATION (A^T·u_i) //***//
      summation5 = 0;
      for(int n = 0; n < 6; n++){
        summation5 = summation5 + (A [n] [m] * u_i [n] [0]);
      }
      Ap [m] [i] = summation5;
    }
  }
}

void printAp(){
  //***// PRINTING OF A^+ (Ap) ON THE SERIAL MONITOR //***//

  Serial.println("Matrix Ap:");
  for(int i = 0; i < 9; i++){
    for(int j = 0; j < 6; j++){
      if(j < 5) Serial.print(Ap [i] [j]) && Serial.print(", ");
      if(j == 5) Serial.println(Ap [i] [j]);
    }
  }
}

void b_print(){
  //***// PRINTING OF b ON THE SERIAL MONITOR //***//

  Serial.println("Matrix b (result):");
  for(int i = 0; i < 6; i++){
    Serial.println(b [i] [0], 6);
  }
}

void x_calc(){
  //***// x CALCULATION (Ap·b) //***//

  double summation;

  for(int i = 0; i < 9; i++){
    summation = 0;
    for(int j = 0; j < 6; j++){
      summation += Ap [i] [j] * b [j] [0];
    }
    x [i] [0] = summation;
  }
}

void x_print(){
  //***// PRINTING OF x ON THE SERIAL MONITOR //***//

  Serial.println("Matrix x (result):");
  for(int i = 0; i < 9; i++){
    Serial.println(x [i] [0], 6);
  }
}

void Ft_i_calc(){
  //***// Ft_i CALCULATION //***//

  for(int j = 0; j < 3; j++){
    int i = j * 3;
    Ft_i [j] [0] = sqrt(x [i] [0] * x [i] [0] + x [i+1] [0] * x [i+1] [0] + x [i+2] [0] * x [i+2] [0]);
  }
}

void Ft_i_print(){
  //***// PRINTING OF Ft_i ON THE SERIAL MONITOR //***//

  Serial.println("Matrix Ft_i:");

  for(int i = 0; i < 3; i++){
    Serial.println(Ft_i [i] [0]);
  }
}

void gimbalangles_calc(){
  //***// servo_x & servo_y CALCULATION //***//

  gimbalangles[0][0] = atan2(x[1][0], sqrt(x[0][0] * x[0][0] + x[2][0] * x[2][0]));
  gimbalangles[1][0] = atan2(-x[0][0], x[2][0]);
  gimbalangles[2][0] = atan2(x[4][0], sqrt(x[3][0] * x[3][0] + x[5][0] * x[5][0]));
  gimbalangles[3][0] = atan2(-x[3][0], x[5][0]);
  gimbalangles[4][0] = atan2(x[7][0], sqrt(x[6][0] * x[6][0] + x[8][0] * x[8][0]));
  gimbalangles[5][0] = atan2(-x[6][0], x[8][0]);
}

void gimbalangles_print(){
  //***// PRINTING OF gimbalangles ON THE SERIAL MONITOR //***//

  Serial.println("Matrix gimbalangles:");

  for(int i = 0; i < 6; i++){
    Serial.println(gimbalangles [i] [0]);
  }
}

void loop(){}
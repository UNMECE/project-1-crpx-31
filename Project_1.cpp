#include<stdio.h>
#include<iostream>

using namespace std;

struct _capacitor
{
  double *time;       // time array
  double *voltage;    // voltage array
  double *current;    // current array
  double C;           // capacitance value
};
typedef struct _capacitor Capacitor;

int main() {
    double delta_t = 1e-10;
    int max_steps = 50000;
    double res = 1000.0;     // = 1 kOhm resistor
    double cap_val = 100e-12; // = 100 pF capacitor
    double V0 = 10.0;
    
    Capacitor cap_cc; // for constant current circuit
    Capacitor cap_cv; // for constant voltage circuit
    
    cap_cc.C = cap_val;
    cap_cv.C = cap_val;

    //dynamically allocate memory & max_steps + 1 to account for the initial t=0 state
    int array_size = max_steps + 1;
    
    cap_cc.time = new double[array_size];
    cap_cc.voltage = new double[array_size];
    cap_cc.current = new double[array_size];
    
    cap_cv.time = new double[array_size];
    cap_cv.voltage = new double[array_size];
    cap_cv.current = new double[array_size];

    //initial conditions at t = 0
    cap_cc.time[0] = 0.0;
    cap_cc.voltage[0] = 0.0;
    cap_cc.current[0] = 1e-2; //0.01 A constant current

    cap_cv.time[0] = 0.0;
    cap_cv.voltage[0] = 0.0; //capacitor is uncharged at start
    cap_cv.current[0] = V0 / res; //initial current is just V0 / R

    //loop to calculate finite differences
    for (int i = 0; i < max_steps; i++) {
        cap_cc.time[i+1] = cap_cc.time[i] + delta_t;
        cap_cv.time[i+1] = cap_cv.time[i] + delta_t;

        //constant current math=> current stays the same
        cap_cc.current[i+1] = cap_cc.current[0];
		 
        //V(t+1) = V(t) + I(t) * dt * (1/C)
        cap_cc.voltage[i+1] = cap_cc.voltage[i] + (cap_cc.current[i] * delta_t * (1.0 / cap_cc.C));


        //constant voltage math => I(t+1) = I(t) - (I(t) / RC) * dt
        cap_cv.current[i+1] = cap_cv.current[i] - ((cap_cv.current[i] / (res * cap_cv.C)) * delta_t);
        
        //KVL means V0 = Vr + Vc, so Vc = V0 - Vr. And Vr = I(t+1) * R
        cap_cv.voltage[i+1] = V0 - (cap_cv.current[i+1] * res); 
    }

    //result
    cout << "Time(s)        CC_Volt(V)     CC_Curr(A)     CV_Volt(V)     CV_Curr(A)\n";
    cout << "----------------------------------------------------------------------\n";
    
    //output every 200 timesteps
    for (int i = 0; i <= max_steps; i++) {
        if (i % 200 == 0) {
            printf("%.4e  %.4e     %.4e     %.4e     %.4e\n", 
                   cap_cc.time[i], 
                   cap_cc.voltage[i], 
                   cap_cc.current[i], 
                   cap_cv.voltage[i], 
                   cap_cv.current[i]);
        }
    }

    //clean up dynamic memory
    delete[] cap_cc.time;
    delete[] cap_cc.voltage;
    delete[] cap_cc.current;
    
    delete[] cap_cv.time;
    delete[] cap_cv.voltage;
    delete[] cap_cv.current;

    return 0;
}
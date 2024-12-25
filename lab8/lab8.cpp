#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <fstream>
#include <climits>
#include <set>
#include <stdint.h>

using namespace std;

typedef size_t size_m;
typedef unsigned long long ull;

inline ull rdtsc(){
    unsigned int lo, hi;
    asm volatile ("rdtsc\n" : "=a" (lo), "=d" (hi));
    return ((ull)hi << 32) | lo;
}

//прогрев процессора
void heatingProcessor(){
    long long itr2 = 2;
    for (int i = 0; i < 10000; i++){
        itr2 *= 2;
    }
    if (itr2 == -12) printf("wow");
}

//тестирование времени обхода
void testMass(size_m *mass, size_m n, ofstream &output){
    size_m i = 0;
    unsigned long long tm = ULLONG_MAX;
    for (; mass[i] != 0; i = mass[i]) {}
    i = 0;
    heatingProcessor();
    for (int itr = 0; itr < 6; itr++){
        unsigned long long t1 = rdtsc();
        for(; mass[i] != 0; i = mass[i]) {}
        unsigned long long t2 = rdtsc();
        if ((t2-t1) < tm){
            tm = (t2-t1);
        }
        i = 0;
    }
    delete mass;
    output << "(" << n << ";" << double(tm) / double(n) << ")" << endl;
    if (i == -12) printf("wow");
}

size_m* createMass(size_t n){
    size_m* mass = new size_m[n];
    for(size_t itr = 0; itr < n - 1; itr++){
        mass[itr] = itr + 1;
    }
    mass[n - 1] = 0;
    return mass;
}
size_m* createReverseMass(size_t n){
    size_m* mass = new size_m[n];
    for (size_t itr = n - 1; itr > 0; itr--){
        mass[itr] = itr - 1;
    }
    mass[0] = n-1;
    return mass;
}


size_m* createRandomMass(size_t n){
    size_m* mass = new size_m[n];
    for (size_t i = 0; i < n; i++) mass[i] = i;
    for (size_t i = n - 1; i > 0; i--){
        size_t j = rand() % i;
        long long tmp = mass[j];
        mass[j] = mass[i];
        mass[i] = tmp;
    }

    return mass;
}

int main(){
    srand(time(NULL));
    ofstream massOutput("streightMass.txt");
    ofstream reverseOutput("reverseMass.txt");
    ofstream randomOutput("randomMass.txt");

    for(size_t i = 10; i < 6205728; i *= 1.2){
        testMass(createMass(i), i, massOutput);
    }

    cout << "Streight array already done!" << endl;

    for(size_t i = 10; i < 6205728; i *= 1.2){
        testMass(createReverseMass(i), i, reverseOutput);
    }
    cout << "Reverse array already done!" << endl;

    for(size_t i = 10; i < 6205728; i *= 1.2){
        testMass(createRandomMass(i), i, randomOutput);
    }

    cout << "Random array already done" << endl;

    massOutput.close();
    reverseOutput.close();
    randomOutput.close();
}
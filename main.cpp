#include <iostream>
#include <string>

using namespace std;

struct Laptop {

string marca;
string color;
int anio;
int ram;
string procesador;


};


void muestrainfo(Laptop lap){

cout << endl << "Información Laptop: " << endl;
cout <<  "Marca: " << lap.marca << endl;
cout <<  "Color: " << lap.color << endl;
cout <<  "Anio: " << lap.anio << endl;
cout <<  "RAM: " << lap.ram << endl;
cout <<  "Procesador: " << lap.procesador << endl;
}
int main(){

Laptop laptop1;

laptop1.marca = "DELL";
laptop1.color = "NEGRO";
laptop1.anio = 2023;
laptop1.ram = 8;
laptop1.procesador = "INTEL i7";

muestrainfo(laptop1);


return 0;
}

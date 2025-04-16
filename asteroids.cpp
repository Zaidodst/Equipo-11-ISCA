/*
PROYECTO FINAL DE PROGRAMACION 1. ASTEROIDES
    INTEGRANTES:
ZAID DE LA ROSA CASTILLO
GARCIA FIGUEROA ALDO MANUEL
MUÑOZ ROMO OSWALDO EMMANUEL    */
//librerias
#include <iostream>
using namespace std;


//declaracion de strcuts

struct Players{
    char nombre[25];
    long score;

};

//declaracion de prototipos
void menu();
void game(/*struct de jugadores, */);
void addPlayer(Players[]);//struct de los jugadores
void showCounter(Players[]);

//main
int main(){
    menu();
    return 0;

}

void menu(){
    int opc=0;
    do{
        cout<<"ASTEROIDS"<<endl;
        cout<<"Start!..................1"<<endl; //por ahora, el menu se maneja como entradas de numeros
        cout<<"Add player..............2"<<endl;
        cout<<"Scores..................3"<<endl;
        cout<<"Exit....................4"<<endl;
        cin>>opc;
        switch (opc)
        {
        case 1:
            // llamar a funcion game
            break;
        case 2:
            //llamar a funcion addPlayer
            break;
        case 3:
            //llamar a funcion showCounter
            break;
        case 4:
            cout<<"Exiting... Thanks 4 playing!!"<<endl;
        default:
            cout<<"Invalid option. Try again."<<endl;
            break;
        }
    }while(opc!=4);
}

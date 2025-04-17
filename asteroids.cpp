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

//Variables globales
#define TAM 5
static int tope=0; //tope sera para definir el maximo de jugadores que podra aceptar la struct players

//main
int main(){
    menu();
    return 0;

}

void menu(){
    Players datos[TAM];
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
            addPlayer(datos);
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
}//menu

void game(){
    cout<<"Prueba :)"<<endl;
}

void addPlayer(Players datos[]){
    if(tope<=TAM){
        cout<<"Introduce your nickname: ";
        cin>>datos[tope].nombre; //tope sirve como auxiliar para que tengamos control sobre la cant de jugadores
        tope++;
    }
    else{
        cout<<"Not enough space. Please try again later."<<endl;
    }
}//addPlayers


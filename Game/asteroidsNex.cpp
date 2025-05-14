/*
PROYECTO FINAL DE PROGRAMACION 1. ASTEROIDES
INTEGRANTES:
ZAID DE LA ROSA CASTILLO
GARCIA FIGUEROA ALDO MANUEL
MUÑOZ ROMO OSWALDO EMMANUEL    
*/


//librerias
#include <SFML/Graphics.hpp>
#include <iostream>
using namespace std;

//Declaracion de prototipos


//Main
int main(){
    sf::RenderWindow ventana(sf::VideoMode({980, 800}), "Asteroids-Nex");

    //Si se presiona una tecla y luego se deja de presionar
    bool isPressed = false;

    //Mantiene la ventana abierta
    while(ventana.isOpen()){

        while(const std::optional event = ventana.pollEvent()){
            //Checa lo que presiona el usiario

            //Letra A=Izquierda
            if(sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::A)){
                cout<<"Izquierda"<<endl;
            }

            //Letra D=Derecha
             if(sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::D)){
                cout<<"Derecha"<<endl;
            }

            //Letra W=Arriba
             if(sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::W)){
                cout<<"Arriba"<<endl;
            }

            //Letra S=Abajo
             if(sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::S)){
                cout<<"Abajo"<<endl;
            }

            //Letra espacio=Fire
             if(sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Space)){
                //Se disparara
                isPressed = true;
                cout<<"Fire";
            }else{
                /*Se supone que recarga pero habra que ver si no genera alguna sobrecarga(Se traba),
                porque en todo momento se estaria recargando si no se presiona el espacio*/
                isPressed = false;
                //cout<<"Recargando";
            }

            //Si se presiona Ctr+O entonces activa un cheat que lo hace inmortal :) capricho de Aldo
            if(auto* key=event->getIf<sf::Event::KeyPressed>()){
                if(key->scancode == sf::Keyboard::Scancode::O){
                    if(key->control){
                        cout<<"Ctrl + O";
                    }
                }
            }

            /*Probablemente deberamos agregar teclas de posiciones en las cuales al presionar 2 teclas a la vez
            se vaya en direccion diagonal dependiendo de la letra, ejemplo A,W = diagonal hacia arriba izquierda*/

            //Fin de manejo de teclas




            //Cuando se presiona el boton de cerrar se cierra la ventana
            if (event->is<sf::Event::Closed>())
                //Cierra la ventana
                ventana.close();
        }
        //Cambia el color de la ventana
        ventana.clear(sf::Color::Black);

        //Muestra el cambio de color
        ventana.display();

    }

    return EXIT_SUCCESS;
}
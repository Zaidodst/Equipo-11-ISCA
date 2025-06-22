/*
PROYECTO FINAL DE PROGRAMACION 1. ASTEROIDES
INTEGRANTES:
ZAID DE LA ROSA CASTILLO
GARCIA FIGUEROA ALDO MANUEL
MUÑOZ ROMO OSWALDO EMMANUEL
NAVARRO REYES DANIEL ISAI
*/

#include <SFML/Graphics.hpp> // Librería para gráficos
#include <SFML/System.hpp>
#include <SFML/Audio.hpp> // Librería para audio
#include <string> // Libreria que nos funciona para las vidas
#include <fstream> //Libreria para el manejo de archivos
#include <iostream>
#include <vector>
#include <cmath>

using namespace std;

//Variables globales
sf::Texture texturaNave;
sf::Texture texturaAsteroide;
int score = 0;

// Constantes de la ventana
const int ANCHO_VENTANA = 1024;
const int ALTO_VENTANA = 768;
const float PI = 3.14159265f;

// Funciones de utilidad

//Lee los scores ya guardados en el archivo txt
string leerScoresGuardados() {
    std::ifstream archivo("scores.txt");
    std::string scores;
    std::string linea;

    if (archivo.is_open()) {
        while (getline(archivo, linea)) {
            scores += linea + "\n";  // Agrega cada línea al string
        }
        archivo.close();
    } else {
        scores = "No hay scores guardados";  // Mensaje si el archivo no existe
    }

    return scores;
}

float gradosARadianes(float grados) {
    return grados * PI / 180.f; // Convierte grados a radianes
} // Convierte grados a radianes

float longitudVector(sf::Vector2f v) {
    return std::sqrt(v.x * v.x + v.y * v.y); // Calcula la longitud de un vector
} // Calcula la longitud de un vector

sf::Vector2f normalizar(sf::Vector2f v) {
    float len = longitudVector(v);
    return (len != 0) ? v / len : v; // Normaliza un vector
} // Normaliza un vector

// Estructura de la Nave Espacial
struct Nave {
    //Creacion de estadisticas de la nave para saber si hubo fin de juego
    int vidas = 2;
    bool gameOver = false;

    sf::Sprite sprite;
    sf::Vector2f posicion;
    float rotacion;
    sf::Vector2f velocidad;
    float velocidadMax = 200.f;
    float velocidadRotacion = 180.f;
    float enfriamientoMisil = 0.25f;
    float temporizadorEnfriamiento = 0.f;

    Nave() : posicion(ANCHO_VENTANA / 2.f, ALTO_VENTANA / 2.f), rotacion(0.f) {
        sprite.setTexture(texturaNave);
        sprite.setOrigin(texturaNave.getSize().x / 2.f, texturaNave.getSize().y / 2.f); // Centro del sprite
        sprite.setPosition(posicion);
        sprite.scale(0.06f,0.06f);
    }

    void actualizar(float dt) {
        if (temporizadorEnfriamiento > 0) temporizadorEnfriamiento -= dt;
        posicion += velocidad * dt;

        // Rebote en bordes
        if (posicion.x < 0) posicion.x += ANCHO_VENTANA;
        if (posicion.x > ANCHO_VENTANA) posicion.x -= ANCHO_VENTANA;
        if (posicion.y < 0) posicion.y += ALTO_VENTANA;
        if (posicion.y > ALTO_VENTANA) posicion.y -= ALTO_VENTANA;

        sprite.setPosition(posicion);
        sprite.setRotation(rotacion);
    } // Actualiza la posición y rotación de la nave

    void rotarIzquierda(float dt) {
        rotacion -= velocidadRotacion * dt;
        if (rotacion < 0) rotacion += 360.f;
    } // Rota la nave a la izquierda

    void rotarDerecha(float dt) {
        rotacion += velocidadRotacion * dt;
        if (rotacion >= 360.f) rotacion -= 360.f;
    } // Rota la nave a la derecha

    void moverAdelante(float dt) {
        sf::Vector2f direccion(std::sin(gradosARadianes(rotacion)), -std::cos(gradosARadianes(rotacion)));
        velocidad += direccion * velocidadMax * dt;
        if (longitudVector(velocidad) > velocidadMax) velocidad = normalizar(velocidad) * velocidadMax;
    } // Mueve la nave hacia adelante

    void moverIzquierda(float dt) {
        velocidad += sf::Vector2f(
            -std::cos(gradosARadianes(rotacion)),
            -std::sin(gradosARadianes(rotacion))  
        ) * velocidadMax * dt * 0.5f;

        if (longitudVector(velocidad) > velocidadMax)
            velocidad = normalizar(velocidad) * velocidadMax;
    } // Mueve la nave hacia la izquierda

    void moverDerecha(float dt) {
        velocidad += sf::Vector2f(
            std::cos(gradosARadianes(rotacion)),
            std::sin(gradosARadianes(rotacion))
        ) * velocidadMax * dt * 0.5f;

        if (longitudVector(velocidad) > velocidadMax)
            velocidad = normalizar(velocidad) * velocidadMax;
    } // Mueve la nave hacia la derecha

    void moverAtras(float dt) {
        sf::Vector2f direccion(-std::sin(gradosARadianes(rotacion)), std::cos(gradosARadianes(rotacion)));
        velocidad += direccion * velocidadMax * dt * 0.5f;
        if (longitudVector(velocidad) > velocidadMax) velocidad = normalizar(velocidad) * velocidadMax;
    } // Mueve la nave hacia atrás

    bool puedeDisparar() {
        //Si a este se le aumenta el numero, ya sea 4.f, se puede disparar con hacks
        return temporizadorEnfriamiento <= 0.f; // Verifica si la nave puede disparar
    } // Verifica si la nave puede disparar

    void disparar(vector<struct Misil>& misiles) {
        if (puedeDisparar()) {
            sf::Vector2f direccion(std::sin(gradosARadianes(rotacion)), -std::cos(gradosARadianes(rotacion)));
            misiles.emplace_back(posicion + direccion * 20.f, direccion * 500.f);
            temporizadorEnfriamiento = enfriamientoMisil;
        }
    } // Dispara un misil
};

// Estructura de Misil
struct Misil {
    sf::CircleShape forma;
    sf::Vector2f posicion;
    sf::Vector2f velocidad;
    float tiempoVida = 2.f;

    Misil(sf::Vector2f pos, sf::Vector2f vel) : posicion(pos), velocidad(vel) {
        forma.setRadius(3);
        forma.setFillColor(sf::Color::Red);
        forma.setOrigin(3, 3);
    }

    void actualizar(float dt) {
        posicion += velocidad * dt;
        tiempoVida -= dt;

        // Rebote en bordes
        if (posicion.x < 0) posicion.x += ANCHO_VENTANA;
        if (posicion.x > ANCHO_VENTANA) posicion.x -= ANCHO_VENTANA;
        if (posicion.y < 0) posicion.y += ALTO_VENTANA;
        if (posicion.y > ALTO_VENTANA) posicion.y -= ALTO_VENTANA;

        forma.setPosition(posicion);
    } // Actualiza la posición del misil

    bool estaMuerto() const {
        return tiempoVida <= 0.f; // Verifica si el misil ha muerto
    } // Verifica si el misil ha muerto
};

// Estructura de Asteroide
struct Asteroide {
    sf::Sprite sprite;
    sf::Vector2f posicion;
    sf::Vector2f velocidad;
    float radio;
    int nivelTamano; // 3=grande, 2=mediano, 1=chico

    Asteroide(sf::Vector2f pos, sf::Vector2f vel, float r, int tamano)
        : posicion(pos), velocidad(vel), radio(r), nivelTamano(tamano) {
        sprite.setTexture(texturaAsteroide);
        sprite.setOrigin(texturaAsteroide.getSize().x / 2.f, texturaAsteroide.getSize().y / 2.f);
        sprite.setPosition(posicion);
        float escala = 0.06f * nivelTamano; //Sirve como apoyo para la division de asteroides
        sprite.setScale(escala, escala);  // Ajusta la escala del asteroide
    }

    void actualizar(float dt) {
        posicion += velocidad * dt;

        // Rebote en bordes
        if (posicion.x < 0) posicion.x += ANCHO_VENTANA;
        if (posicion.x > ANCHO_VENTANA) posicion.x -= ANCHO_VENTANA;
        if (posicion.y < 0) posicion.y += ALTO_VENTANA;
        if (posicion.y > ALTO_VENTANA) posicion.y -= ALTO_VENTANA;

        sprite.setPosition(posicion);
        sprite.rotate(1.f);  // Rotación automática
    } // Actualiza la posición del asteroide

    vector<Asteroide> dividir() {
        vector<Asteroide> hijos;
        if (nivelTamano > 1) {
            float nuevoRadio = radio / 2.f;
            sf::Vector2f velPerp(-velocidad.y, velocidad.x);
            if (longitudVector(velPerp) < 0.1f) velPerp = sf::Vector2f(50.f, -50.f);
            //Aumenta la velocidad de los asteroides divididos (Tienes que aumentar el +)
            velPerp = normalizar(velPerp) * (longitudVector(velocidad) + 150.f);

            //Hace que los asteroides divididos salgan hacia distintos lugares
            hijos.emplace_back(posicion, (velocidad + velPerp) * 0.7f, nuevoRadio, nivelTamano - 1);
            hijos.emplace_back(posicion, (velocidad - velPerp) * 0.7f, nuevoRadio, nivelTamano - 1);
        }
        return hijos;
    } // Divide el asteroide en dos
};

// Clase Botón para el menú
class Boton {
    sf::RectangleShape rectangulo;
    sf::Text texto;
    sf::Color colorNormal = sf::Color(50, 50, 50);
    sf::Color colorHover = sf::Color(70, 130, 180);

public:
    Boton(float porcentajeX, float porcentajeY, sf::Vector2f tam, sf::Font& fuente, string str) {
        rectangulo.setSize(tam);
        // Calcula posición basada en porcentajes:
        float posX = (porcentajeX / 100.0f) * ANCHO_VENTANA - tam.x / 2;
        float posY = (porcentajeY / 100.0f) * ALTO_VENTANA - tam.y / 2;
        rectangulo.setPosition(posX, posY);

        texto.setFont(fuente);
        texto.setString(str);
        texto.setCharacterSize(24);
        texto.setFillColor(sf::Color::White);

        // Centra el texto en el botón
        sf::FloatRect bounds = texto.getLocalBounds();
        texto.setOrigin(bounds.left + bounds.width / 2, bounds.top + bounds.height / 2);
        texto.setPosition(posX + tam.x / 2, posY + tam.y / 2);
    }

    void actualizar(sf::Vector2f mousePos) {
        rectangulo.setFillColor(rectangulo.getGlobalBounds().contains(mousePos) ? colorHover : colorNormal);
    } // Actualiza el color del botón según la posición del mouse

    void dibujar(sf::RenderWindow& ventana) {
        ventana.draw(rectangulo);
        ventana.draw(texto);
    } // Dibuja el botón en la ventana

    bool fueClickeado(sf::Vector2f mousePos) {
        return rectangulo.getGlobalBounds().contains(mousePos); // Verifica si el botón fue clickeado
    } // Verifica si el botón fue clickeado
};

// Estados del juego
enum class EstadoJuego { MENU, PARTIDAS_GUARDADAS, JUGANDO, SALIR };

// Función para inicializar asteroides
void initAsteroides(vector<Asteroide>& asteroides, const sf::Vector2f& posicionNave) {
    asteroides.clear();
    float radioExclusion = 100.0f;
    for (int i = 0; i < 4; i++) {
        sf::Vector2f pos;
        float distanciaANave;
        /*Genera posiciones aleatorias hasta que estén fuera de la nave, esto para evitar
        un Game Over inmediato*/
        do {
            pos.x = rand() % ANCHO_VENTANA;
            pos.y = rand() % ALTO_VENTANA;
            distanciaANave = longitudVector(pos - posicionNave);
        } while (distanciaANave < radioExclusion);

        sf::Vector2f vel((rand() % 200 - 100) / 10.f, (rand() % 200 - 100) / 10.f);
        if (vel == sf::Vector2f(0, 0)) vel = {10.f, 15.f};
        asteroides.emplace_back(pos, vel, 60.f, 3);
    }
} // Inicializa los asteroides


//############################
//############################
//############################
//############################
//Main principal (Importante)
int main() {

    char* nombreJugador = nullptr;
    int capacidadNombre = 0;
    int longitudNombre = 0;
    bool ingresandoNombre = false;

    //Inicializacion de las variables
    capacidadNombre = 20;
    //Uso de memoria dinamica para el vector de nombre
    nombreJugador = new char[capacidadNombre]();
    nombreJugador[0] = '\0';

    //Creacion de la pantalla con ciertas dimensiones
    sf::RenderWindow ventana(sf::VideoMode(ANCHO_VENTANA, ALTO_VENTANA), "Asteroids Space", sf::Style::Close);
    ventana.setFramerateLimit(60);

    //Cargar las imagenes de la nave y los asteroides
    if (!texturaNave.loadFromFile("nave.png") || !texturaAsteroide.loadFromFile("asteroide.png")) {
    std::cerr << "Error cargando texturas\n";
    return -1;
}

    sf::Font fuente;
    if (!fuente.loadFromFile("Doctor Glitch.otf")) {
        return EXIT_FAILURE; // Carga la fuente
    }

    // Elementos del juego
    Boton botonIniciar(50, 30, sf::Vector2f(320, 50), fuente, "Iniciar");  // Permite centrar el boton
    Boton botonGuardadas(50, 45, sf::Vector2f(320, 50), fuente, "Partidas guardadas"); // Permite centrar el boton
    Boton botonSalir(50, 60, sf::Vector2f(320, 50), fuente, "Salir"); // Permite centrar el boton

    EstadoJuego estado = EstadoJuego::MENU;
    Nave nave;

    //Vectores
    vector<Misil> misiles;
    vector<Asteroide> asteroides;
    sf::Clock reloj;

    initAsteroides(asteroides, nave.posicion); // Inicializa asteroides

    // Cargar música
    sf::Music musica;
    if (!musica.openFromFile("A Few Jumps Away.mp3")) {
        return EXIT_FAILURE; // Carga la música
    }
    musica.setLoop(true);
    musica.play(); // Reproduce la música

    // Bucle principal
    while (ventana.isOpen()) {
        float dt = reloj.restart().asSeconds();

        sf::Event evento;
        
        while (ventana.pollEvent(evento)) {
            if (evento.type == sf::Event::Closed) {
                ventana.close();
            }

            //Esto sirve para poder ingresar el nombre del jugador a su score
            if (nave.gameOver && !ingresandoNombre && sf::Keyboard::isKeyPressed(sf::Keyboard::Enter)) {
                // Puedes empezar a editar el nombre si presionas la tecla enter
                ingresandoNombre = true;
                longitudNombre = 0;
                if (nombreJugador) nombreJugador[0] = '\0'; // Resetea el nombre
            }

            if (ingresandoNombre && evento.type == sf::Event::TextEntered) {
                if (evento.text.unicode == '\b' && longitudNombre > 0) {
                    nombreJugador[--longitudNombre] = '\0';
                }
                // Caracteres ASCII válidos (evita Enter/Tab)
                else if (evento.text.unicode >= 32 && evento.text.unicode < 127 && longitudNombre < capacidadNombre - 1) {
                    nombreJugador[longitudNombre++] = static_cast<char>(evento.text.unicode);
                    nombreJugador[longitudNombre] = '\0'; // Null-terminator
                }
            } // Fin de ingresar el nombre del jugador


            //Guardar score y nombre
            if (ingresandoNombre && sf::Keyboard::isKeyPressed(sf::Keyboard::Enter) && longitudNombre > 0) {
                // Guardar en archivo de texto
                std::ofstream archivo("scores.txt", std::ios::app);  // ios::app No sobreescribe los scores anteriores
                if (archivo.is_open()) {
                    archivo << nombreJugador << ": " << score << "\n";  // Ej: "Dani: 350"
                    archivo.close();
                }

                //Mensaje de prueba de que se guardo el scor
                ventana.clear(sf::Color::Black);  // Limpia la pantalla
                sf::Text textoGuardado("Score guardado!", fuente, 30);
                textoGuardado.setFillColor(sf::Color::Green);
                textoGuardado.setOrigin(textoGuardado.getLocalBounds().width / 2, textoGuardado.getLocalBounds().height / 2);
                textoGuardado.setPosition(ANCHO_VENTANA / 2, ALTO_VENTANA / 2);
                ventana.draw(textoGuardado);
                ventana.display();
                sf::sleep(sf::seconds(2));  // Esto sirve para que solo aparezca por ciertos segundos

                // Reiniciar para nuevo juego
                ingresandoNombre = false;
                nave.gameOver = false;
                score = 0;
                estado = EstadoJuego::MENU;  // Esto hace que se regrese al menu
                initAsteroides(asteroides, nave.posicion);  // Reinicia los asteroides
            }
            //Fin guardar score



            if (estado == EstadoJuego::MENU && evento.type == sf::Event::MouseButtonPressed) {
                sf::Vector2f mousePos = ventana.mapPixelToCoords(sf::Mouse::getPosition(ventana));
                if (botonIniciar.fueClickeado(mousePos)) {
                    estado = EstadoJuego::JUGANDO;
                    nave = Nave();
                    misiles.clear();
                    initAsteroides(asteroides, nave.posicion);
                }
                else if (botonGuardadas.fueClickeado(mousePos)) {
                    estado = EstadoJuego::PARTIDAS_GUARDADAS;
                }
                else if (botonSalir.fueClickeado(mousePos)) {
                    estado = EstadoJuego::SALIR;
                }
            }
            else if (evento.type == sf::Event::KeyPressed && evento.key.code == sf::Keyboard::Escape) {
                if (estado == EstadoJuego::PARTIDAS_GUARDADAS || estado == EstadoJuego::JUGANDO) {
                    estado = EstadoJuego::MENU;
                }
            }
        }

        // Actualización del juego
        if (estado == EstadoJuego::MENU) {
            sf::Vector2f mousePos = ventana.mapPixelToCoords(sf::Mouse::getPosition(ventana));
            botonIniciar.actualizar(mousePos);
            botonGuardadas.actualizar(mousePos);
            botonSalir.actualizar(mousePos);
        }
        //Juedo en estado jugando
        else if (estado == EstadoJuego::JUGANDO) {
            // Controles
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q)) nave.rotarIzquierda(dt);
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::E)) nave.rotarDerecha(dt);
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) nave.moverAdelante(dt);
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) nave.moverIzquierda(dt);
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) nave.moverDerecha(dt);
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) nave.moverAtras(dt);
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) nave.disparar(misiles);
            if (nave.gameOver && sf::Keyboard::isKeyPressed(sf::Keyboard::R)) {
                // Reinicia todo el juego
                //Condicion agregada para que no genere error al poner "r" en un nombre
                if(!ingresandoNombre){
                    nave = Nave(); // Reinicia nave (vidas=2, gameOver=false)
                    misiles.clear(); //Limpia los misiles
                    asteroides.clear(); //Limpia los asteriodes
                    score=0; //Resetea el score
                    initAsteroides(asteroides,nave.posicion); // Vuelve a generar asteroides
                }
            }

            nave.actualizar(dt);

            // Actualizar misiles
            for (auto it = misiles.begin(); it != misiles.end();) {
                it->actualizar(dt);
                if (it->estaMuerto()) {
                    it = misiles.erase(it);
                } else {
                    ++it;
                }
            }

            // Actualizar asteroides
            for (auto& a : asteroides) a.actualizar(dt);

            //Deteccion de colision Nave-Asteroide
            for (auto& asteroide : asteroides) {
                if (longitudVector(nave.posicion - asteroide.posicion) < asteroide.radio + 15) {
                    nave.vidas--;
                    if (nave.vidas <= 0) {
                        nave.gameOver = true;
                    } else {
                        // Reiniciar posición de la nave
                        nave.posicion = sf::Vector2f(ANCHO_VENTANA / 2.f, ALTO_VENTANA / 2.f);
                        nave.velocidad = sf::Vector2f(0, 0);
                    }
                    break; // Romper el bucle para evitar múltiples colisiones en un frame
                }
            }
            //Fin de la deteccion de colision Nave-Asteroide


            // Colisiones Misil-Asteroide
            vector<Asteroide> nuevosAsteroides;
            for (auto mit = misiles.begin(); mit != misiles.end();) {
                bool colision = false;
                for (auto ait = asteroides.begin(); ait != asteroides.end();) {
                    if (longitudVector(mit->posicion - ait->posicion) < ait->radio) {
                        //Contador de score
                        //Solo sumar puntos si aun no hay GameOver
                        if(!nave.gameOver){
                            switch (ait->nivelTamano) {
                                case 3: score += 20; break;  // Asteroide grande +20
                                case 2: score += 50; break;  // Asteroide mediano +50
                                case 1: score += 100; break; // Asteroide pequeño +100
                            }//Fin del contador del score
                        }

                        //Condicion para que no se pueda pasar de cierta cantidad de puntos
                        if(score>99990){
                            score=0;
                        }
                        
                        auto hijos = ait->dividir();
                        nuevosAsteroides.insert(nuevosAsteroides.end(), hijos.begin(), hijos.end());
                        ait = asteroides.erase(ait);
                        mit = misiles.erase(mit);
                        colision = true;
                        break;
                    } else {
                        ++ait;
                    }
                }
                if (!colision) ++mit;
            }
            asteroides.insert(asteroides.end(), nuevosAsteroides.begin(), nuevosAsteroides.end());
        }

        // Dibujo de pantalla (Importantisimo)
        ventana.clear(sf::Color(10, 10, 30));

        if (estado == EstadoJuego::MENU) {
            botonIniciar.dibujar(ventana);
            botonGuardadas.dibujar(ventana);
            botonSalir.dibujar(ventana);

            sf::Text titulo("Asteroids Space", fuente, 48);
            titulo.setFillColor(sf::Color::Cyan);
            sf::FloatRect bounds = titulo.getLocalBounds();
            titulo.setOrigin(bounds.left + bounds.width / 2.f, bounds.top + bounds.height / 2.f);
            titulo.setPosition(ANCHO_VENTANA / 2.f, 100);
            ventana.draw(titulo);
        }
        else if (estado == EstadoJuego::PARTIDAS_GUARDADAS) {
            // Título
            sf::Text titulo("Partidas Guardadas", fuente, 48);
            titulo.setFillColor(sf::Color::Cyan);
            titulo.setOrigin(titulo.getLocalBounds().width / 2, 0);
            titulo.setPosition(ANCHO_VENTANA / 2, 50);
            ventana.draw(titulo);

            // Leer y mostrar scores
            sf::Text textoScore(leerScoresGuardados(), fuente, 24);
            textoScore.setFillColor(sf::Color::White);
            textoScore.setPosition(100, 150);  // Posición ajustable
            ventana.draw(textoScore);


        }
        else if (estado == EstadoJuego::JUGANDO) {
            ventana.draw(nave.sprite);
            for (auto& m : misiles) ventana.draw(m.forma);
            for (auto& a : asteroides) ventana.draw(a.sprite);

            // Mostrar vidas
            sf::Text textoVidas("Vidas: " + std::to_string(nave.vidas), fuente, 24);
            textoVidas.setFillColor(sf::Color::White);
            textoVidas.setPosition(20, 20);  // Esquina superior izquierda
            ventana.draw(textoVidas);
            //Fin del contador de vidas

            sf::Text textoScore("Score: " + std::to_string(score), fuente, 24);
            textoScore.setFillColor(sf::Color::Yellow);
            textoScore.setPosition(ANCHO_VENTANA -240, 20); //Pone el score arriba a la derecha
            ventana.draw(textoScore);

            // Pantalla de gameOver
            if (nave.gameOver) {
                //Poner fondo en negro, en la pantalla Game Over
                ventana.clear(sf::Color::Black);

                //Dibujar GameOver
                sf::Text textoGameOver("GAME OVER", fuente, 60);
                textoGameOver.setFillColor(sf::Color::Red);
                textoGameOver.setOrigin(textoGameOver.getLocalBounds().width / 2, textoGameOver.getLocalBounds().height / 2);
                textoGameOver.setPosition(ANCHO_VENTANA / 2, ALTO_VENTANA / 2 - 50);

                //Dibujar Score
                //Usamos el to_string para convertir de numero a string y poder mostrar en pantalla
                sf::Text textoScore("Score: " + std::to_string(score), fuente, 40);
                textoScore.setFillColor(sf::Color::White);
                textoScore.setOrigin(textoGameOver.getLocalBounds().width / 2, textoGameOver.getLocalBounds().height / 2);
                textoScore.setPosition(ANCHO_VENTANA / 2, ALTO_VENTANA / 2 + 60);

                //Dibujar ingresar nombre
                sf::Text instruccion("Presiona Enter para guardar", fuente, 20);
                instruccion.setPosition(ANCHO_VENTANA / 2 - 192, ALTO_VENTANA / 2 + 120);
                ventana.draw(instruccion);
                if (ingresandoNombre) {
                    sf::Text textoInput;
                    textoInput.setFont(fuente);
                    textoInput.setString("Nombre: " + std::string(nombreJugador) + "_");  // Cursor "_"
                    textoInput.setCharacterSize(30);
                    textoInput.setFillColor(sf::Color::Cyan);
                    textoInput.setOrigin(textoInput.getLocalBounds().width / 2, textoInput.getLocalBounds().height/2 - 100);  // Centrado horizontal
                    textoInput.setPosition(ANCHO_VENTANA / 2, ALTO_VENTANA / 2 + 60);
                    ventana.draw(textoInput);
                }
                
                //Dibujar instruccion para reiniciar el juego
                sf::Text textoReiniciar("Presiona R para reiniciar", fuente, 24);
                textoReiniciar.setFillColor(sf::Color::White);
                textoReiniciar.setOrigin(textoReiniciar.getLocalBounds().width / 2, textoReiniciar.getLocalBounds().height / 2);
                textoReiniciar.setPosition(ANCHO_VENTANA / 2, ALTO_VENTANA / 2 + 280);

                ventana.draw(textoGameOver);
                ventana.draw(textoScore);
                ventana.draw(textoReiniciar);
                ventana.display();
                continue;
            }
            // FIN DE PANTALLA GAME OVER
        }
        
        if (estado == EstadoJuego::SALIR) ventana.close();
        ventana.display();
    }

    //Prueba
    delete[] nombreJugador;
    return 0;
} // Fin del main
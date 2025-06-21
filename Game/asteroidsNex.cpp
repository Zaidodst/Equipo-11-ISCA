/*
PROYECTO FINAL DE PROGRAMACION 1. ASTEROIDES
INTEGRANTES:
ZAID DE LA ROSA CASTILLO
GARCIA FIGUEROA ALDO MANUEL
MUÑOZ ROMO OSWALDO EMMANUEL
*/


#include <SFML/Graphics.hpp> // Librería para gráficos
#include <SFML/System.hpp>
#include <SFML/Audio.hpp> // Librería para audio
#include <string> // Libreria que nos funciona para las vidas
#include <iostream>
#include <vector>
#include <cmath>

using namespace std;

// Constantes de la ventana
const int ANCHO_VENTANA = 800;
const int ALTO_VENTANA = 600;
const float PI = 3.14159265f;

// Funciones de utilidad
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

    sf::ConvexShape forma;
    sf::Vector2f posicion;
    float rotacion;
    sf::Vector2f velocidad;
    float velocidadMax = 200.f;
    float velocidadRotacion = 180.f;
    float enfriamientoMisil = 0.25f;
    float temporizadorEnfriamiento = 0.f;

    Nave() : posicion(ANCHO_VENTANA / 2.f, ALTO_VENTANA / 2.f), rotacion(0.f) {
        forma.setPointCount(3);
        forma.setPoint(0, sf::Vector2f(0, -15));
        forma.setPoint(1, sf::Vector2f(10, 10));
        forma.setPoint(2, sf::Vector2f(-10, 10));
        forma.setFillColor(sf::Color::White);
        forma.setOutlineColor(sf::Color::Cyan);
        forma.setOutlineThickness(2);
        forma.setOrigin(0, 0);
    }

    void actualizar(float dt) {
        if (temporizadorEnfriamiento > 0) temporizadorEnfriamiento -= dt;
        posicion += velocidad * dt;

        // Rebote en bordes
        if (posicion.x < 0) posicion.x += ANCHO_VENTANA;
        if (posicion.x > ANCHO_VENTANA) posicion.x -= ANCHO_VENTANA;
        if (posicion.y < 0) posicion.y += ALTO_VENTANA;
        if (posicion.y > ALTO_VENTANA) posicion.y -= ALTO_VENTANA;

        forma.setPosition(posicion);
        forma.setRotation(rotacion);
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
            -std::cos(gradosARadianes(rotacion)),  // -direccion.y (porque direccion.y = -cos)
            -std::sin(gradosARadianes(rotacion))   // direccion.x
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
    sf::CircleShape forma;
    sf::Vector2f posicion;
    sf::Vector2f velocidad;
    float radio;
    int nivelTamano;

    Asteroide(sf::Vector2f pos, sf::Vector2f vel, float r, int tamano)
        : posicion(pos), velocidad(vel), radio(r), nivelTamano(tamano) {
        forma.setRadius(radio);
        forma.setFillColor(sf::Color(150, 150, 150));
        forma.setOutlineColor(sf::Color::White);
        forma.setOutlineThickness(2);
        forma.setOrigin(radio, radio);
    }

    void actualizar(float dt) {
        posicion += velocidad * dt;

        // Rebote en bordes
        if (posicion.x < 0) posicion.x += ANCHO_VENTANA;
        if (posicion.x > ANCHO_VENTANA) posicion.x -= ANCHO_VENTANA;
        if (posicion.y < 0) posicion.y += ALTO_VENTANA;
        if (posicion.y > ALTO_VENTANA) posicion.y -= ALTO_VENTANA;

        forma.setPosition(posicion);
    } // Actualiza la posición del asteroide

    vector<Asteroide> dividir() {
        vector<Asteroide> hijos;
        if (nivelTamano > 1) {
            float nuevoRadio = radio / 2.f;
            sf::Vector2f velPerp(-velocidad.y, velocidad.x);
            if (longitudVector(velPerp) < 0.1f) velPerp = sf::Vector2f(50.f, -50.f);
            velPerp = normalizar(velPerp) * (longitudVector(velocidad) + 20.f);
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
    Boton(sf::Vector2f pos, sf::Vector2f tam, sf::Font& fuente, string str) {
        rectangulo.setSize(tam);
        rectangulo.setPosition(pos);
        rectangulo.setFillColor(colorNormal);

        texto.setFont(fuente);
        texto.setString(str);
        texto.setCharacterSize(24);
        texto.setFillColor(sf::Color::White);

        sf::FloatRect bounds = texto.getLocalBounds();
        texto.setOrigin(bounds.left + bounds.width / 2.f, bounds.top + bounds.height / 2.f);
        texto.setPosition(pos.x + tam.x / 2.f, pos.y + tam.y / 2.f);
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
void initAsteroides(vector<Asteroide>& asteroides) {
    asteroides.clear();
    for (int i = 0; i < 5; i++) {
        sf::Vector2f pos(rand() % ANCHO_VENTANA, rand() % ALTO_VENTANA);
        sf::Vector2f vel((rand() % 200 - 100) / 10.f, (rand() % 200 - 100) / 10.f);
        if (vel == sf::Vector2f(0, 0)) vel = {10.f, 15.f};
        asteroides.emplace_back(pos, vel, 60.f, 3);
    }
} // Inicializa los asteroides

int main() {
    sf::RenderWindow ventana(sf::VideoMode(ANCHO_VENTANA, ALTO_VENTANA), "Asteroids Space", sf::Style::Close);
    ventana.setFramerateLimit(60);

    sf::Font fuente;
    if (!fuente.loadFromFile("Doctor Glitch.otf")) {
        return EXIT_FAILURE; // Carga la fuente
    }

    // Elementos del juego
    Boton botonIniciar({300, 200}, {200, 50}, fuente, "Iniciar");
    Boton botonGuardadas({300, 280}, {200, 50}, fuente, "Partidas guardadas");
    Boton botonSalir({300, 360}, {200, 50}, fuente, "Salir");

    EstadoJuego estado = EstadoJuego::MENU;
    Nave nave;
    vector<Misil> misiles;
    vector<Asteroide> asteroides;
    sf::Clock reloj;

    initAsteroides(asteroides); // Inicializa asteroides

    // Textos
    sf::Text textoGuardadas("No hay partidas guardadas", fuente, 26);
    textoGuardadas.setPosition(200, 250);
    textoGuardadas.setFillColor(sf::Color::White);

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

            if (estado == EstadoJuego::MENU && evento.type == sf::Event::MouseButtonPressed) {
                sf::Vector2f mousePos = ventana.mapPixelToCoords(sf::Mouse::getPosition(ventana));
                if (botonIniciar.fueClickeado(mousePos)) {
                    estado = EstadoJuego::JUGANDO;
                    nave = Nave();
                    misiles.clear();
                    initAsteroides(asteroides);
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
                nave = Nave(); // Reinicia nave (vidas=2, gameOver=false)
                misiles.clear();
                asteroides.clear();
                initAsteroides(asteroides); // Vuelve a generar asteroides
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
                        // Reiniciar posición de la nave (opcional)
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

        // Dibujado
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
            ventana.draw(textoGuardadas);
        }
        else if (estado == EstadoJuego::JUGANDO) {
            ventana.draw(nave.forma);
            for (auto& m : misiles) ventana.draw(m.forma);
            for (auto& a : asteroides) ventana.draw(a.forma);

            //Dibujar vidas en pantalla
            // ====== MOSTRAR VIDAS (PUNTO 4) ======
            sf::Text textoVidas("Vidas: " + std::to_string(nave.vidas), fuente, 24);
            textoVidas.setFillColor(sf::Color::White);
            textoVidas.setPosition(20, 20);  // Esquina superior izquierda
            ventana.draw(textoVidas);
            // ====== FIN DEL CONTADOR DE VIDAS ======


            // ====== PANTALLA DE GAME OVER ======
            if (nave.gameOver) {
                sf::Text textoGameOver("GAME OVER", fuente, 60);
                textoGameOver.setFillColor(sf::Color::Red);
                textoGameOver.setOrigin(textoGameOver.getLocalBounds().width / 2, textoGameOver.getLocalBounds().height / 2);
                textoGameOver.setPosition(ANCHO_VENTANA / 2, ALTO_VENTANA / 2 - 50);

                sf::Text textoReiniciar("Presiona R para reiniciar", fuente, 24);
                textoReiniciar.setFillColor(sf::Color::White);
                textoReiniciar.setOrigin(textoReiniciar.getLocalBounds().width / 2, textoReiniciar.getLocalBounds().height / 2);
                textoReiniciar.setPosition(ANCHO_VENTANA / 2, ALTO_VENTANA / 2 + 50);

                ventana.draw(textoGameOver);
                ventana.draw(textoReiniciar);
            }
            // FIN DE PANTALLA GAME OVER
        }
        
        if (estado == EstadoJuego::SALIR) ventana.close();
        ventana.display();
    }

    //Prueba

    return 0; // Fin del programa
} // Fin del programa

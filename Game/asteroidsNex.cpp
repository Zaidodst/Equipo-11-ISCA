/*
PROYECTO FINAL DE PROGRAMACION 1. ASTEROIDES
INTEGRANTES:
ZAID DE LA ROSA CASTILLO
GARCIA FIGUEROA ALDO MANUEL
MUÑOZ ROMO OSWALDO EMMANUEL
NAVARRO REYES DANIEL ISAI
*/

//ponganos 10 maestra :)


#include <SFML/Graphics.hpp> // Librería para gráficos
#include <SFML/System.hpp>
#include <SFML/Audio.hpp> // Librería para audio
#include <string.h> // Libreria que nos funciona para las vidas
#include <fstream> //Libreria para el manejo de archivos
#include <iostream>
#include <vector>
#include <cmath>
#include <cstdlib> //para aleatorios en los disparos
using namespace std;

//Variables globales
sf::Texture texturaNave;
sf::Texture texturaAsteroide;
int score = 0;
//Variables globales para los generar niveles
int nivelActual = 1;
int asteroidesRestantes = 0;
bool mostrarMensajeNivel = false;
float tiempoMensajeNivel = 0.0f;
const float TIEMPO_MOSTRAR_NIVEL = 3.0f;// 3 segundos para mostrar el mensaje de cambio de nivel
//usamos const porque siempre sera de 3 segundos


// Constantes de la ventana
const int ANCHO_VENTANA = 1024;
const int ALTO_VENTANA = 768;
const float PI = 3.14159265f;

// Funciones de utilidad

//Lee los scores ya guardados en el archivo txt
string leerScoresGuardados() {
    ifstream archivo("scores.txt");
    string scores;
    string linea;

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

//Esta funcion va a dibujar el mensaje que te dice en que nivel vas
void dibujarMensajeNivel(sf::RenderWindow& ventana, sf::Font& fuente, int nivel) {
    sf::Text textoNivel("Nivel " + to_string(nivel), fuente, 72);
    textoNivel.setFillColor(sf::Color::Yellow);
    textoNivel.setStyle(sf::Text::Bold);
    
    // Centrar el texto
    sf::FloatRect bounds = textoNivel.getLocalBounds();
    textoNivel.setOrigin(bounds.width / 2, bounds.height / 2);
    textoNivel.setPosition(ANCHO_VENTANA / 2, ALTO_VENTANA / 2);
    
    ventana.draw(textoNivel);
}


float gradosARadianes(float grados) {
    return grados * PI / 180.f; // Convierte grados a radianes
} // Convierte grados a radianes

float longitudVector(sf::Vector2f v) {
    return sqrt(v.x * v.x + v.y * v.y); // Calcula la longitud de un vector
} // Calcula la longitud de un vector

sf::Vector2f normalizar(sf::Vector2f v) {
    float len = longitudVector(v);
    return (len != 0) ? v / len : v; // Normaliza un vector
} // Normaliza un vector


/*Este estruct esta abajo, pero se pone aqui para que pueda
funcionar la teletransportacion, favor de no quitar
*/
struct Asteroide;


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
    //Variables para enfriamente del hiperespacio
    float enfriamientoTeletransporte = 1.0f;  // 1 segundo de enfriamiento
    float temporizadorTeletransporte = 0.0f;

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
        if (temporizadorTeletransporte > 0) temporizadorTeletransporte -= dt;
    } // Actualiza la posición y rotación de la nave

    void teletransportar(vector<Asteroide>& asteroides);  // Declaración

    void rotarIzquierda(float dt) {
        rotacion -= velocidadRotacion * dt;
        if (rotacion < 0) rotacion += 360.f;
    } // Rota la nave a la izquierda

    void rotarDerecha(float dt) {
        rotacion += velocidadRotacion * dt;
        if (rotacion >= 360.f) rotacion -= 360.f;
    } // Rota la nave a la derecha

    void moverAdelante(float dt) {
        sf::Vector2f direccion(sin(gradosARadianes(rotacion)), -cos(gradosARadianes(rotacion)));
        velocidad += direccion * velocidadMax * dt;
        if (longitudVector(velocidad) > velocidadMax) velocidad = normalizar(velocidad) * velocidadMax;
    } // Mueve la nave hacia adelante

    void moverIzquierda(float dt) {
        velocidad += sf::Vector2f(
            -cos(gradosARadianes(rotacion)),
            -sin(gradosARadianes(rotacion))  
        ) * velocidadMax * dt * 0.5f;

        if (longitudVector(velocidad) > velocidadMax)
            velocidad = normalizar(velocidad) * velocidadMax;
    } // Mueve la nave hacia la izquierda

    void moverDerecha(float dt) {
        velocidad += sf::Vector2f(
            cos(gradosARadianes(rotacion)),
            sin(gradosARadianes(rotacion))
        ) * velocidadMax * dt * 0.5f;

        if (longitudVector(velocidad) > velocidadMax)
            velocidad = normalizar(velocidad) * velocidadMax;
    } // Mueve la nave hacia la derecha

    void moverAtras(float dt) {
        sf::Vector2f direccion(-sin(gradosARadianes(rotacion)), cos(gradosARadianes(rotacion)));
        velocidad += direccion * velocidadMax * dt * 0.5f;
        if (longitudVector(velocidad) > velocidadMax) velocidad = normalizar(velocidad) * velocidadMax;
    } // Mueve la nave hacia atrás

    bool puedeDisparar() {
        //Si a este se le aumenta el numero, ya sea 4.f, se puede disparar con hacks
        return temporizadorEnfriamiento <= 0.f; // Verifica si la nave puede disparar
    } // Verifica si la nave puede disparar

    void disparar(vector<struct Misil>& misiles) {
        if (puedeDisparar()) {
            sf::Vector2f direccion(sin(gradosARadianes(rotacion)), -cos(gradosARadianes(rotacion)));
            misiles.emplace_back(posicion + direccion * 20.f, direccion * 500.f);
            temporizadorEnfriamiento = enfriamientoMisil;
        }
    } // Dispara un misil
};


struct PaginaAyuda {
    char titulo[20];       // Ajuste de tamaño para el titulo
    char lineas[8][50];    // Matriz para la pagina de ayuda
    int numLineas;         // Número de líneas usadas
};
//Nos permite hacer las paginas de ayuda

struct BotonAyuda{ //Crear el boton de ayuda usando matrices
    sf::CircleShape forma;
    sf::Text textoInterrogacion;
    bool visible = false;
    int paginaActual = 0;
    PaginaAyuda paginas[3]; // Matriz de 3 páginas
    sf::Font fuente; // Necesitamos almacenar la fuente

    BotonAyuda() {
        forma.setRadius(25);
        forma.setFillColor(sf::Color(30, 30, 120));
        forma.setOutlineThickness(2);
        forma.setOutlineColor(sf::Color::White); // El circulo lo encierra con una linea blanca
        forma.setPosition(ANCHO_VENTANA - 80, 20); //Posiona el boton arriba a la derecha

        // Configuración del texto
        textoInterrogacion.setString("?");
        textoInterrogacion.setFont(fuente);
        textoInterrogacion.setCharacterSize(30);
        textoInterrogacion.setFillColor(sf::Color::White);

        // Ajusta estos valores para mover la "H" a la izquierda
        float ejeX = -10.0f; // Valor negativo mueve a la izquierda
        float ejeY = 10.0f;  // Ajusta si necesitas mover verticalmente

        // Posición modificada
        textoInterrogacion.setPosition(
            forma.getPosition().x + forma.getRadius() + ejeX, // Mueve el signo o letra para la izquierda
            forma.getPosition().y + forma.getRadius() + ejeY // Mueve el signo o letra para la derecha
        );

        // Centra el signo o la letra que se desee
        sf::FloatRect bounds = textoInterrogacion.getLocalBounds();
        textoInterrogacion.setOrigin(
            bounds.left + bounds.width/2,
            bounds.top + bounds.height/2
        );

        inicializarPaginas();
    }

    void inicializarPaginas() {
        // Página 1: Controles
        strncpy(paginas[0].titulo, "CONTROLES", sizeof(paginas[0].titulo));
        strncpy(paginas[0].lineas[0], "Movimiento:", sizeof(paginas[0].lineas[0]));
        strncpy(paginas[0].lineas[1], "Flecha Arriba - Avanzar", sizeof(paginas[0].lineas[1]));
        strncpy(paginas[0].lineas[2], "Flecha Abajo - Retroceder", sizeof(paginas[0].lineas[2]));
        strncpy(paginas[0].lineas[3], "Flechas Izq/Der - Rotar", sizeof(paginas[0].lineas[3]));
        strncpy(paginas[0].lineas[4], "Espacio - Disparar", sizeof(paginas[0].lineas[4]));
        strncpy(paginas[0].lineas[5], "H - Hiperespacio (Riesgo)", sizeof(paginas[0].lineas[5]));
        paginas[0].numLineas = 6;

        // Página 2: Hiperespacio
        strncpy(paginas[1].titulo, "HIPERESPACIO", sizeof(paginas[1].titulo));
        strncpy(paginas[1].lineas[0], "Teletransportacion", sizeof(paginas[1].lineas[0]));
        strncpy(paginas[1].lineas[1], "Posicion aleatoria", sizeof(paginas[1].lineas[1]));
        strncpy(paginas[1].lineas[2], "Pierdes si caes", sizeof(paginas[1].lineas[2]));
        strncpy(paginas[1].lineas[3], "en un asteroide", sizeof(paginas[1].lineas[3]));
        strncpy(paginas[1].lineas[4], "Enfriamiento: 2 segundos", sizeof(paginas[1].lineas[4]));
        paginas[1].numLineas = 5;

        // Página 3: Puntucaciones
        strncpy(paginas[2].titulo, "Puntuaciones", sizeof(paginas[2].titulo));
        strncpy(paginas[2].lineas[0], "Asteroid Grande - 20", sizeof(paginas[2].lineas[0]));
        strncpy(paginas[2].lineas[1], "Asteroid Mediano - 50", sizeof(paginas[2].lineas[1]));
        strncpy(paginas[2].lineas[2], "Asteroid Chico - 100", sizeof(paginas[2].lineas[2]));
        strncpy(paginas[2].lineas[3], "Ovni Grande - 200", sizeof(paginas[2].lineas[3]));
        strncpy(paginas[2].lineas[4], "Ovni Chico - 1000", sizeof(paginas[2].lineas[4]));
        paginas[2].numLineas = 5;

        /*Si quieren agregar mas paginas deben de modificar aqui y
        el porcentaje en la parte de evento del botonAyuda*/
    }

    void dibujar(sf::RenderWindow& ventana) {
        ventana.draw(forma);
        ventana.draw(textoInterrogacion);
    }

    bool contiene(sf::Vector2f punto) {
        return forma.getGlobalBounds().contains(punto);
    }
};


//Dibuja la ventana de ayuda
void dibujarVentanaAyuda(sf::RenderWindow& ventana, const BotonAyuda& ayuda) {
    // Fondo semitransparente
    sf::RectangleShape fondo(sf::Vector2f(400, 300));
    fondo.setFillColor(sf::Color(0, 0, 0, 200));
    fondo.setPosition(ANCHO_VENTANA/2 - 200, ALTO_VENTANA/2 - 150);
    ventana.draw(fondo);

    const PaginaAyuda& pagina = ayuda.paginas[ayuda.paginaActual];

    // Título
    sf::Text titulo(pagina.titulo, ayuda.fuente, 32);
    titulo.setFillColor(sf::Color::Cyan);
    titulo.setPosition(ANCHO_VENTANA/2 - titulo.getLocalBounds().width/2, ALTO_VENTANA/2 - 120);
    ventana.draw(titulo);

    // Líneas de texto
    for (int i = 0; i < pagina.numLineas; ++i) {
        sf::Text linea(pagina.lineas[i], ayuda.fuente, 22);
        linea.setPosition(ANCHO_VENTANA/2 - 180, ALTO_VENTANA/2 - 70 + i*30);
        ventana.draw(linea);
    }

    // Indicador de página
    sf::Text pagInfo(to_string(ayuda.paginaActual + 1), ayuda.fuente, 20);
    pagInfo.setPosition(ANCHO_VENTANA/2 + 160, ALTO_VENTANA/2 + 120);
    ventana.draw(pagInfo);
}//Fin del dibujo


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


//estructura y enum de los ovnis

enum class tipoOvni{Grande, Chico};

struct Ovni{
    //se definen las caracterisicas, posiciones, velocidades y tiempo de disparo
    tipoOvni tipo;
    sf:: Sprite sprite;//estos dos datos dseran para manejar la imagen del ovni
    sf::Texture textura;
    sf::Vector2f posicion;
    sf::Vector2f velocidad;
    float TdEDisparo;
    float temporizador;
    bool vivo = true;

    //propiedades que tendra el ovni cuando se vea en pantalla (por ahora es prototipo hasta agregar imagenes)
    Ovni(tipoOvni t, sf::Vector2f pos, sf::Vector2f vel, sf::Texture& textura) : tipo(t), posicion(pos), velocidad(vel), temporizador(0.f){
        //para las texturas
        sprite.setTexture(textura);
        sprite.setPosition(posicion);
        sf::Vector2u TAM = textura.getSize();
        sprite.setOrigin(TAM.x / 2.f, TAM.y / 2.f);

        if(tipo == tipoOvni::Grande){
            sprite.setScale(0.2f, 0.2f);
            TdEDisparo = 1.5f; //dispara cada 1 segundo
            velocidad *= 0.5f; // el ovni grande se mueve mas lento
        }
        else{
            //propiedades del ovni chico
            TdEDisparo = 2.f; //dispara cada 2 segundos
            velocidad *= 1.f; // el ovni chico se mueve mas rapido
            sprite.setScale(0.1f, 0.1f);
        }
    }

    void actualizar(float dt){
        if(!vivo){
            return; // si murio el ovni, ya no hace nada
        }
        //en caso de que siga vivo, sigue los siguientes patrones
        posicion += velocidad *dt;
        sprite.setPosition(posicion);
        temporizador += dt;

        //cuando el ovni llega a un borde, cambia de sentido con la velocidad invertida
        if(posicion.x < 0 || posicion.x >ANCHO_VENTANA){
            velocidad.x = -velocidad.x; 
        }
        if(posicion.y <0 || posicion.y >ALTO_VENTANA){
            velocidad.y = -velocidad.y; 
        }
    }//actualizar
    bool Disparar(){
        return temporizador >= TdEDisparo; // si ya paso el TdE, vuelve a disparar  
    }//disparar
    void TdE(){ //TdE es Tiempo de Espera o cooldown del disparo
        temporizador = 0.f;
    }//TdE

};//struct ovni

//struct para el misil del ovni
struct MisilOvni{
    //propiedades del misil (esencialmente, tienen propiedades similares a los misiles del jugador)
    sf::CircleShape forma;
    sf::Vector2f posicion;
    sf::Vector2f velocidad;
    float tiempoVida = 2.f; //tiempo de vida del misil
    float temporizador = 0.f;

    MisilOvni(sf::Vector2f pos, sf::Vector2f vel): posicion(pos), velocidad(vel){
        forma.setRadius(3);
        forma.setFillColor(sf::Color::Green);
        forma.setOrigin(3, 3);
        forma.setPosition(pos);
    }

    void actualizar(float dt){
        posicion += velocidad *dt;
        forma.setPosition(posicion);
        temporizador += dt;
    }
    
    //esta bandera sirve para que saber si el disparo salio de la pantalla
    bool estaFuera() const{ //con const, no se modifican las propiedades del misil
        return temporizador >= tiempoVida
        || posicion.x < 0 || posicion.x > ANCHO_VENTANA
        || posicion.y < 0 || posicion.x >ALTO_VENTANA;
    }
};//misil ovni

// Define la función después de la estructura Asteroide:
void Nave::teletransportar(vector<Asteroide>& asteroides) {
    if (temporizadorTeletransporte <= 0) {
        posicion.x = rand() % ANCHO_VENTANA;
        posicion.y = rand() % ALTO_VENTANA;
        temporizadorTeletransporte = enfriamientoTeletransporte;

        for (auto& asteroide : asteroides) {
            if (longitudVector(posicion - asteroide.posicion) < asteroide.radio + 15) {
                vidas--;
                if (vidas <= 0) gameOver = true;
                break;
            }
        }
    }
}

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
void initAsteroides(vector<Asteroide>& asteroides, const sf::Vector2f& posicionNave, int nivel) {
    asteroides.clear();
    float radioExclusion = 100.0f;
    //Esto hace que aparezcan 4 asteroides por nivel y cuando avanzen de nivel se aumentan 2
    int cantidadAsteroides = 4 + (nivel - 1)*2; 

    for (int i = 0; i < cantidadAsteroides; i++) {
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
    asteroidesRestantes = cantidadAsteroides;
} // Inicializa los asteroides

// Guardar la puntuacion
void guardarPuntuacion(int puntos){
    ofstream archivo("puntuaciones.txt",ios::app);
    if(archivo.is_open()){
        archivo<<"Puntuacion: "<<puntos<<endl;
        archivo.close();
    }
}

// Leer puntuaciones
vector<string> leerPuntuaciones(){
    vector<string> lineas;
    ifstream archivo("puntuaciones.txt");
    string linea;

    if(archivo.is_open()){
        while(getline(archivo, linea)){
            lineas.push_back(linea);
        }
        archivo.close();
    }else{
        lineas.push_back("No se pudo leer el archivo.");
    }
    return lineas;
}

//############################  
//############################
//############################
//############################
//Main principal (Importante)
int main() {
    //Variables para el guardado del nombre
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
    cerr << "Error cargando texturas\n";
    return -1;
}
    //texturas para los ovnis
    sf::Texture OvniGrande, OvniChico;
    if(!OvniGrande.loadFromFile("ovniGrande.png") || !OvniChico.loadFromFile("ovniChico.png")){
        cerr << "Error al cargar las texturas de los ovnis\n";
    }

    sf::Font fuente;
    if (!fuente.loadFromFile("Doctor Glitch.otf")) {
        return EXIT_FAILURE; // Carga la fuente
    }
    
    //Esta fuente aun no se utiliza
    sf::Font fuente2;
    if (!fuente2.loadFromFile("fast99.ttf")) {
        return EXIT_FAILURE; // Carga la fuente
    }

    //Estilo del botonAyuda
    BotonAyuda botonAyuda;
    botonAyuda.fuente = fuente;

    // Elementos del juego
    Boton botonIniciar(50, 30, sf::Vector2f(320, 50), fuente, "Iniciar");  // Permite centrar el boton
    Boton botonGuardadas(50, 45, sf::Vector2f(320, 50), fuente, "Scores guardadas"); // Permite centrar el boton
    Boton botonSalir(50, 60, sf::Vector2f(320, 50), fuente, "Salir"); // Permite centrar el boton

    EstadoJuego estado = EstadoJuego::MENU;
    Nave nave;
    //declaracion de variables de vectores
    vector<Misil> misiles;
    vector<Asteroide> asteroides;
    vector<Ovni> ovnis;
    vector<MisilOvni> disparosOvni;
    vector<string> puntuacionesGuardadads;
    //intervalos y tiempos de espera entre ovnis
    float TdEGrande = 15.f, TdEPeque = 20.f, interG = 0.f, TdEGrandeMuerto = 0.f;
    bool grandeGen = false;
    bool esperarChico = false; // esta bandera es para que, cuando se destruya el ovni grande, espere un tiempo para generar el chico
    int puntuacion=0;

    // variables para el control de tiempo que tarda el jugador en disparar a un asteroide
    bool disparado = false;
    float tiempoSinDisparo = 0.f;
    float tiempoMaxSinDisparo = 20.f; //el tiempo maximo que pasara si no golpea un meteorito es de 20 segundos
    bool disparadoReciente = false;
    int pocosAsteroides = 3;
    bool tiempoDesdeDisparo = 0.f;
    sf::Clock reloj;

    initAsteroides(asteroides, nave.posicion, nivelActual); // Inicializa asteroides

    // Cargar música
    sf::Music musica;
    if (!musica.openFromFile("Dragonspine.mp3")) {
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
                ofstream archivo("scores.txt", ios::app);  // ios::app No sobreescribe los scores anteriores
                if (archivo.is_open()) {
                    archivo << nombreJugador << ": " << score << "\n";  // Ej: "Dani: 350"
                    archivo.close();
                }

                //Mensaje de prueba de que se guardo el score
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
                initAsteroides(asteroides, nave.posicion,nivelActual);  // Reinicia los asteroides
            }
            //Fin guardar score


            //Evento para botonAyuda
            if (evento.type == sf::Event::MouseButtonPressed) {
                sf::Vector2f mousePos = ventana.mapPixelToCoords(sf::Mouse::getPosition(ventana));

                // Si se hace clic en el botón de ayuda
                if (botonAyuda.contiene(mousePos)) {
                    botonAyuda.visible = !botonAyuda.visible;
                }
                // Si se hace clic en cualquier parte con la ayuda visible, cambia de página
                else if (botonAyuda.visible) {
                    botonAyuda.paginaActual = (botonAyuda.paginaActual + 1) % 3;
                }
            }//Fin del evento de el botonAyuda


            if (estado == EstadoJuego::MENU && evento.type == sf::Event::MouseButtonPressed) {
                sf::Vector2f mousePos = ventana.mapPixelToCoords(sf::Mouse::getPosition(ventana));
                if (botonIniciar.fueClickeado(mousePos)) {
                    estado = EstadoJuego::JUGANDO;
                    nave = Nave();
                    misiles.clear();
                    nivelActual=1;
                    score=0;
                    initAsteroides(asteroides, nave.posicion, nivelActual);
                }
                else if (botonGuardadas.fueClickeado(mousePos)) {
                    estado = EstadoJuego::PARTIDAS_GUARDADAS;
                    puntuacionesGuardadads=leerPuntuaciones(); // Cargar las puntuaciones
                }
                else if (botonSalir.fueClickeado(mousePos)) {
                    estado = EstadoJuego::SALIR;
                }
            }
            //Funcion para cuando se presiona el Escape
            else if (evento.type == sf::Event::KeyPressed && evento.key.code == sf::Keyboard::Escape) {
                if (estado == EstadoJuego::PARTIDAS_GUARDADAS || estado == EstadoJuego::JUGANDO) {
                    score=0; // Resetea el score para evitar bugs de score de más.
                    nivelActual=1; // Pone el nivel actual en el primero, igual para evitar problemas
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
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) nave.rotarIzquierda(dt);
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) nave.rotarDerecha(dt);
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) nave.moverAdelante(dt);
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) nave.moverIzquierda(dt);
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) nave.moverDerecha(dt);
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) nave.moverAtras(dt);
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::H)) nave.teletransportar(asteroides);
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) nave.disparar(misiles);
            if (nave.gameOver && sf::Keyboard::isKeyPressed(sf::Keyboard::R)) {
                // Reinicia todo el juego
              
                //Condicion agregada para que no genere error al poner "r" en un nombre
                if(!ingresandoNombre){
                    nave = Nave(); // Reinicia nave (vidas=2, gameOver=false)
                    misiles.clear(); //Limpia los misiles
                    asteroides.clear(); //Limpia los asteriodes
                    score=0; //Resetea el score
                    nivelActual=1; //Reseteal el nivel actual
                    ovnis.clear(); //reinicia los ovnis
                    disparosOvni.clear();//reinicia los disparos de los ovnis
                    initAsteroides(asteroides,nave.posicion,nivelActual); // Vuelve a generar asteroides
                    interG = 0.f; // se reinicia el tiempo de intervalo de los ovnis
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

            // Sistema de niveles
            if (asteroides.empty() && !mostrarMensajeNivel) {
                if (nivelActual < 30) {
                    nivelActual++;
                    mostrarMensajeNivel = true;
                    tiempoMensajeNivel = TIEMPO_MOSTRAR_NIVEL;
                    initAsteroides(asteroides, nave.posicion, nivelActual);

                    //reinicia las propiedades de los ovnis al pasar de nivel
                    ovnis.clear();             // borra ovnis existentes
                    disparosOvni.clear();      // borra disparos de ovnis
                    esperarChico = false;
                    grandeGen = false;
                    interG = 0.f;
                    TdEGrandeMuerto = 0.f;
                } else {
                    // Juego completado
                    nave.gameOver = true;
                }
            }

            // Actualizar temporizador del mensaje de nivel
            if (mostrarMensajeNivel) {
                tiempoMensajeNivel -= dt;
                if (tiempoMensajeNivel <= 0) {
                    mostrarMensajeNivel = false;
                }
            }
            //Fin del sistema de niveles

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
                        //Detecta que se le disparo al asteroide
                        disparado = true;
                        tiempoSinDisparo = 0.f;
                        disparadoReciente = true;
                        tiempoDesdeDisparo = 0.f;
                        //Decrementar el contador de los asteroides restantes
                        asteroidesRestantes--;
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
                        asteroidesRestantes -=1;
                        asteroidesRestantes +=hijos.size();
                        nuevosAsteroides.insert(nuevosAsteroides.end(), hijos.begin(), hijos.end());
                        puntuacion+=10*ait->nivelTamano;//aumenta puntos
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

        // Toma el control del tiempo que pasa sin dispararle a los asteroides
        if (!disparado) {   
            tiempoSinDisparo += dt;
        } else {
            disparado = false;
        }
        if(disparadoReciente){
            tiempoDesdeDisparo += dt;
        }
        if(tiempoDesdeDisparo >= 0.5f){ //TdE para que el ovni aparezca cuando hay pocos meteoritos
            disparadoReciente = false;
        }
        
            // Mostrar los ovnis
        bool chicoVivo = false, grandeVivo = false;
        interG += dt;
        //este ciclo verifica si hay ovnis grandes
        for(auto &o : ovnis){
            if(o.vivo && o.tipo == tipoOvni::Grande){
                grandeVivo = true;
                break;
            }
        }//for ovnis grandes
        //de igual manera, este ciclo verifica si hay ovnis chicos
        for(auto &o : ovnis){
            if(o.vivo && o.tipo == tipoOvni::Chico){
                chicoVivo = true;
                break;
            }
        }//for ovnis chicos

        //generacion de los ovnis
        if(!grandeVivo && !esperarChico && !grandeGen){
            //si muere el ovni chico, reinicia el tiempo del grande
            if(interG >= TdEGrande){
                ovnis.emplace_back(tipoOvni::Grande, sf::Vector2f(ANCHO_VENTANA, 200.f), sf::Vector2f(-50.f, 0.f), OvniGrande);
                interG = 0.f; //reinicia el intervalo de tiempo desde que se genera
                grandeGen = true;
            }
        }
        //comienza el tiempo que espera para generar al ovni chico, una vez el grande muere
        if(grandeGen && !grandeVivo && !esperarChico){
            esperarChico = true;
            TdEGrandeMuerto = 0.f;
        }
        //generacion del ovni chico
        if(esperarChico && !grandeVivo){ //una vez que el grande haya muerto, comienza a contar el tiempo que pasa despues del suceso
            TdEGrandeMuerto += dt;
            if(TdEGrandeMuerto >= TdEPeque){//cuando hayan pasado al menos 10 seg desde que murio el ovni, si no hay uno chico lo genera
                ovnis.emplace_back(tipoOvni::Chico, sf::Vector2f(ANCHO_VENTANA, ALTO_VENTANA / 2.f), sf::Vector2f(-100.f, 0.f), OvniChico);
                esperarChico = false;
                grandeGen = false;
                interG = 0.f; //reinicia el intervalo de tiempo desde que se genera
            }
        }
        // si ya pasaron 10 segundos de que no le dispara a un asteroide, aparece el ovni chiquito
        if(tiempoSinDisparo >= tiempoMaxSinDisparo && !chicoVivo){
            ovnis.emplace_back(tipoOvni::Chico, sf::Vector2f(ANCHO_VENTANA, ALTO_VENTANA / 2.f), sf::Vector2f(-100.f, 0.f), OvniChico);
            tiempoSinDisparo = 0.f; //reinicia el tiempo de disparo
        }
        // si quedan pocos asteroides y no hay un ovni chico vivo
        if(asteroides.size() <= pocosAsteroides && !chicoVivo){
            ovnis.emplace_back(tipoOvni::Chico, sf::Vector2f(ANCHO_VENTANA, ALTO_VENTANA / 2.f), sf::Vector2f(-100.f, 0.f), OvniChico);
        }
        //actualiza los ovnis (los mueve)
        for(auto& o : ovnis){
            o.actualizar(dt);
            //este segmento se encargara de hacer que los ovnis disparen
            if(o.vivo && o.Disparar()){
                sf::Vector2f direccion;
                if(o.tipo == tipoOvni::Grande){// si es un ovni grande, dispara de manera aleatoria
                    /*genera un numero entre 0 a 360 y se convierte a float (sen y cos usan flotantes), 
                    esto se convierte a RADIANES y se genera un vector en una direccion aleatoria*/
                    float angulo = static_cast<float>(rand()%360);
                    direccion = sf::Vector2f(cos(gradosARadianes(angulo)), sin(gradosARadianes(angulo)));
                }
                else{
                    //si es un ovni chico, dispara apuntando al jugador
                    direccion = nave.posicion - o.posicion;
                    direccion = normalizar(direccion);
                }//else
                disparosOvni.emplace_back(o.posicion, direccion * 360.f);
                o.TdE(); //reestablece el TdE del disparo
            }
        }//for (actualizar ovnis)

        for(auto it = disparosOvni.begin(); it !=disparosOvni.end();){// se actualizaran con notacion de punteros
            (*it).actualizar(dt);
            if((*it).estaFuera()){ //si se sale de la pantalla el misil, lo elimina
                it = disparosOvni.erase(it);
            }
            else{
                it++;
            }
        }//for (actualizar disparos de ovnis)

        for(auto it = disparosOvni.begin(); it != disparosOvni.end();){ //colisiones de misil a la nave
            if(longitudVector((*it).posicion - nave.posicion) < 15.f){
                nave.vidas --;
                it = disparosOvni.erase(it); //borra el misil que golpeo al jugador
                if(nave.vidas <= 0){
                    nave.gameOver= true;
                    disparosOvni.clear();
                }
                else{
                    nave.posicion = sf::Vector2f(ANCHO_VENTANA / 2.f, ALTO_VENTANA / 2.f);
                    nave.velocidad = sf::Vector2f(0.f, 0.f);
                }//else
                break;
            }
            else{
                it++;
            }
        }
        //actualiza los ovnis (los mueve)
        for(auto& o : ovnis){
            o.actualizar(dt);
            //este segmento se encargara de hacer que los ovnis disparen
            if(o.vivo && o.Disparar()){
                sf::Vector2f direccion;
                if(o.tipo == tipoOvni::Grande){// si es un ovni grande, dispara de manera aleatoria
                    /*genera un numero entre 0 a 360 y se convierte a float (sen y cos usan flotantes), 
                    esto se convierte a RADIANES y se genera un vector en una direccion aleatoria*/
                    float angulo = static_cast<float>(rand()%360);
                    direccion = sf::Vector2f(cos(gradosARadianes(angulo)), sin(gradosARadianes(angulo)));
                }
                else{
                    //si es un ovni chico, dispara apuntando al jugador
                    direccion = nave.posicion - o.posicion;
                    direccion = normalizar(direccion);
                }//else
                disparosOvni.emplace_back(o.posicion, direccion * 360.f);
                o.TdE(); //reestablece el TdE del disparo
            }
        }//for (actualizar ovnis)

        for(auto it = disparosOvni.begin(); it !=disparosOvni.end();){// se actualizaran con notacion de punteros
            (*it).actualizar(dt);
            if((*it).estaFuera()){ //si se sale de la pantalla el misil, lo elimina
                it = disparosOvni.erase(it);
            }
            else{
                it++;
            }
        }//for (actualizar disparos de ovnis)

        for(auto it = disparosOvni.begin(); it != disparosOvni.end();){ //colisiones de misil a la nave
            if(longitudVector((*it).posicion - nave.posicion) < 15.f){
                nave.vidas --;
                it = disparosOvni.erase(it); //borra el misil que golpeo al jugador
                if(nave.vidas <= 0){
                    nave.gameOver= true;
                    disparosOvni.clear();
                }
                else{
                    nave.posicion = sf::Vector2f(ANCHO_VENTANA / 2.f, ALTO_VENTANA / 2.f);
                    nave.velocidad = sf::Vector2f(0.f, 0.f);
                }//else
                break;
            }
            else{
                it++;
            }
        }

        //colisiones de los misiles hacia los ovnis
        for(auto mit = misiles.begin(); mit != misiles.end();){
            bool colision = false;
            for(auto&o : ovnis){
                //este segmento hace que, mientras la distancia entre el misil y el ovni sea menor a 20 (pixeles)
                //se considere que hay colision y el ovni muere
                if(o.vivo && longitudVector(mit->posicion - o.posicion) < 20.f){
                    o.vivo = false;
                    mit = misiles.erase(mit); //borra el misil
                    colision = true;

                    // Sumar puntos según el tipo de ovni
                    if(!nave.gameOver){  // Solo sumar puntos si el juego no ha terminado
                        if(o.tipo == tipoOvni::Grande){
                            score += 200;  // Ovni grande vale 200 puntos
                        } else {
                            score += 1000;  // Ovni chico vale 1000 puntos
                        }
                        
                        // Asegurarse de que el score no exceda el límite
                        if(score > 99990){
                            score = 0;
                        }
                    }
                    break;
                }
            }//for (ovni)
            if(!colision){// si no colisiona, el ovni sigue su curso
                mit++;
            }
        }//for colisiones misil-ovni

        //colisiones de ovni con nave 
        for(auto& o: ovnis){
            //25.f es para que visualmente coincida que el ovni y la nave colisionan
            if(o.vivo && longitudVector(nave.posicion - o.posicion) < 25.f){ 
                nave.vidas --;
                o.vivo = false;
                if(nave.vidas <= 0){
                    nave.gameOver = true;   
                }else{
                    //si el jugador aun tiene vidas, reincia la posicion de la nave
                    nave.posicion = sf::Vector2f(ANCHO_VENTANA / 2.f, ALTO_VENTANA / 2.f);
                    nave.velocidad = sf::Vector2f(0.f, 0.f);
                }//else
                break;
            }//if (ovni sigue vivo)
        }

        }   // |||||| TERMINA EL ESTADO JUGANDO |||||||||
 
        // Dibujo de pantalla (Importantisimo)
        ventana.clear(sf::Color::Black);

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

            //Dibujo del botonAyuda
            botonAyuda.dibujar(ventana);
            if (botonAyuda.visible) {
                dibujarVentanaAyuda(ventana, botonAyuda);
            }
        }
        else if (estado == EstadoJuego::PARTIDAS_GUARDADAS) {
            // Título
            sf::Text titulo("Scores guardados", fuente, 48);
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
            //Mensaje del cambio de nivel
            if (mostrarMensajeNivel) {
                dibujarMensajeNivel(ventana, fuente, nivelActual);
            }
            //mientras este vivo, se dibuja el ovni (lo muestra en la pantalla)
            for(auto& o : ovnis)
                if(o.vivo) ventana.draw(o.sprite);
            for (auto& d : disparosOvni) ventana.draw(d.forma);

            // Mostrar vidas
            sf::Text textoVidas("Vidas: " + to_string(nave.vidas), fuente, 24);
            textoVidas.setFillColor(sf::Color::White);
            textoVidas.setPosition(20, 20);  // Esquina superior izquierda
            ventana.draw(textoVidas);
            //Fin del contador de vidas

            sf::Text textoScore("Score: " + to_string(score), fuente, 24);
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
                sf::Text textoScore("Score: " + to_string(score), fuente, 40);
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
                    textoInput.setString("Nombre: " + string(nombreJugador) + "_");  // Cursor "_"
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

    delete[] nombreJugador;
    return 0;
} // Fin del main

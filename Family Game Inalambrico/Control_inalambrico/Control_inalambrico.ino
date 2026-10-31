#include <BleGamepad.h>
#include <SPI.h>
#include <TFT_eSPI.h>
#include "EscudoNacional.h"

// =====================================================
// MANDO BLE - TURBO + HISTORIAL + SCROLLING NACIONAL
// =====================================================

TFT_eSPI tft = TFT_eSPI();

BleGamepad bleGamepad("NES Retro Controller", "Tipitochen", 100);
BleGamepadConfiguration config;

// Pines de los botones principales
const int PIN_UP     = 21;
const int PIN_DOWN   = 12;
const int PIN_LEFT   = 25;
const int PIN_RIGHT  = 26;
const int PIN_A      = 27;
const int PIN_B      = 33;
const int PIN_START  = 32;
const int PIN_SELECT = 2;

// Pines para los botones de TURBO independientes
const int PIN_TURBO_A = 17;
const int PIN_TURBO_B = 15;

// Variables para controlar la velocidad del Turbo
unsigned long tiempoUltimoTurbo = 0;
bool estadoTurboPulsos = false;
const int intervaloTurbo = 50; 

// Control de tiempos
unsigned long tiempoUltimaAccion = 0;
const unsigned long tiempoEsperaEscudo = 1000;   // 1 segundo para mostrar el escudo
const unsigned long tiempoEsperaScroll = 10000;  // 10 segundos de inactividad para arrancar el scrolling

// Estado de pantalla y control
bool conectadoAnterior = false;
String ultimoBotonActual = "";

// Historial ampliado a 8 elementos para la izquierda
const int TAM_HISTORIAL = 8;
String historialBotones[TAM_HISTORIAL] = {"", "", "", "", "", "", "", ""};

// =====================================================
// PANTALLA DE ESPERA Y ANIMACIONES
// =====================================================

void mostrarPantallaConexion() {
  tft.fillScreen(TFT_BLACK);
  tft.setCursor(10, 10);
  tft.setTextColor(TFT_YELLOW, TFT_BLACK);
  tft.setTextSize(1);
  tft.println("BUSCANDO");
  
  // "CONECTANDO" en grande y llamativo
  tft.setCursor(10, 35);
  tft.setTextSize(3);
  tft.setTextColor(TFT_CYAN, TFT_BLACK);
  tft.println("CONECTANDO");
  
  tft.drawCircle(120, 95, 12, TFT_CYAN);
  tft.drawCircle(120, 95, 22, TFT_CYAN);
}

void mostrarConexionExitosa() {
  for (int i = 0; i < 3; i++) {
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.setTextSize(2);
    tft.setCursor(35, 55);
    tft.println("CONECTADO!");
    delay(250);
    tft.fillScreen(TFT_BLACK);
    delay(150);
  }
  tft.fillScreen(TFT_BLACK);
  tft.setSwapBytes(true);
  tft.pushImage(56, 5, 128, 128, image_data_esc);
  ultimoBotonActual = "ESCUDO";
}

// =====================================================
// ANIMACIÓN DE TEXTO SCROLLING TIPO STAR WARS (HISTORIA DE NACIONAL)
// =====================================================
bool ejecutarScrollingNacional() {
  const char* lineasHistoria[] = {
    "CLUB NACIONAL",
    "DE FOOTBALL",
    " ",
    "FUNDADO EL",
    "14 DE MAYO",
    "DE 1899",
    " ",
    "EL PRIMER",
    "CLUB CRIOLLO",
    "DE AMERICA",
    "Y DEL MUNDO",
    " ",
    "ORGULLO",
    "Y PASION",
    "INCOMPARABLE",
    " ",
    "3 VECES",
    "CAMPEON",
    "MUNDIAL",
    " ",
    "DECANO",
    "DEL FUTBOL",
    "URUGUAYO",
    " ",
    "GLORIA",
    "ETERNA",
    "NACIONAL"
  };
  int totalLineas = sizeof(lineasHistoria) / sizeof(lineasHistoria[0]);

  for (int yOffset = 135; yOffset > -(totalLineas * 20); yOffset -= 2) {
    
    // Verificamos si se presiona algún botón para cortar la animación de inmediato
    if (digitalRead(PIN_UP) == LOW || digitalRead(PIN_DOWN) == LOW || 
        digitalRead(PIN_LEFT) == LOW || digitalRead(PIN_RIGHT) == LOW || 
        digitalRead(PIN_A) == LOW || digitalRead(PIN_B) == LOW || 
        digitalRead(PIN_START) == LOW || digitalRead(PIN_SELECT) == LOW ||
        digitalRead(PIN_TURBO_A) == LOW || digitalRead(PIN_TURBO_B) == LOW) {
      return false; 
    }

    tft.fillScreen(TFT_BLACK);

    for (int i = 0; i < totalLineas; i++) {
      int currentY = yOffset + (i * 20); 

      if (currentY > -15 && currentY < 145) {
        tft.setTextSize(2);
        
        if (String(lineasHistoria[i]).indexOf("NACIONAL") >= 0 || String(lineasHistoria[i]).indexOf("MUNDIAL") >= 0) {
          tft.setTextColor(TFT_CYAN, TFT_BLACK);
        } else if (String(lineasHistoria[i]).indexOf("1899") >= 0) {
          tft.setTextColor(TFT_YELLOW, TFT_BLACK);
        } else {
          tft.setTextColor(TFT_WHITE, TFT_BLACK);
        }

        int largoTexto = String(lineasHistoria[i]).length() * 12;
        int xPos = (240 - largoTexto) / 2;
        if (xPos < 5) xPos = 5;

        tft.setCursor(xPos, currentY);
        tft.print(lineasHistoria[i]);
      }
    }

    delay(40); 
  }

  // AL TERMINAR EL TEXTO: Mostramos el escudo inmediatamente y dejamos la pantalla ahí
  tft.fillScreen(TFT_BLACK);
  tft.setSwapBytes(true);
  tft.pushImage(56, 5, 128, 128, image_data_esc);
  ultimoBotonActual = "ESCUDO";

  return true; 
}

// Función para actualizar la pantalla principal y la secuencia lateral
void actualizarPantalla(String botonActual, uint16_t colorTexto) {
  if (botonActual != ultimoBotonActual) {
    tft.fillScreen(TFT_BLACK);

    if (botonActual == "ESCUDO") {
      tft.setSwapBytes(true);
      tft.pushImage(56, 5, 128, 128, image_data_esc);
    } else {
      // 1. Mostrar la secuencia histórica a la izquierda (hasta 8 elementos)
      tft.setTextSize(1);
      tft.setCursor(5, 5);
      tft.setTextColor(TFT_DARKGREY, TFT_BLACK);
      tft.println("Secuencia:");
      
      int posY = 18;
      for (int i = TAM_HISTORIAL - 1; i >= 0; i--) {
        if (historialBotones[i] != "") {
          tft.setCursor(5, posY);
          if (i == 0) {
            tft.setTextColor(TFT_YELLOW, TFT_BLACK);
            tft.print("> ");
          } else {
            tft.setTextColor(TFT_DARKGREY, TFT_BLACK);
            tft.print("  ");
          }
          tft.println(historialBotones[i]);
          posY += 13;
        }
      }

      // 2. Mostrar la tecla actual grande
      if (botonActual == "TURBO A" || botonActual == "TURBO B") {
        tft.setTextSize(3);
        tft.setTextColor(colorTexto, TFT_BLACK);
        tft.setCursor(95, 35);  
        tft.print("TURBO");
        
        tft.setTextSize(5); 
        tft.setCursor(135, 70);  
        if (botonActual == "TURBO A") tft.print("A");
        else tft.print("B");
      } 
      else if (botonActual == "ARRIBA" || botonActual == "ABAJO" || botonActual == "START" || botonActual == "SELECT") {
        tft.setTextSize(4);
        tft.setTextColor(colorTexto, TFT_BLACK);
        tft.setCursor(80, 50);   
        tft.print(botonActual);
      }
      else if (botonActual == "A" || botonActual == "B") {
        tft.setTextSize(6);
        tft.setTextColor(colorTexto, TFT_BLACK);
        tft.setCursor(125, 40); 
        tft.print(botonActual);
      }
      else if (botonActual.length() > 5) {
        tft.setTextSize(3);
        tft.setTextColor(colorTexto, TFT_BLACK);
        tft.setCursor(85, 55);   
        tft.print(botonActual);
      } 
      else {
        tft.setTextSize(4);
        tft.setTextColor(colorTexto, TFT_BLACK);
        tft.setCursor(115, 50);          
        tft.print(botonActual);
      }
    }

    ultimoBotonActual = botonActual;
  }
}

// Agrega un nuevo botón al historial desplazando los anteriores
void agregarAlHistorial(String nuevoBoton) {
  if (nuevoBoton != "" && nuevoBoton != historialBotones[0] && nuevoBoton != "ESCUDO") {
    for (int i = TAM_HISTORIAL - 1; i > 0; i--) {
      historialBotones[i] = historialBotones[i - 1];
    }
    historialBotones[0] = nuevoBoton;
  }
}

// =====================================================
// CONFIGURACIÓN
// =====================================================

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println("Iniciando mando BLE con Turbo, Historial y Scrolling de Nacional...");

  tft.init();
  tft.invertDisplay(true);
  tft.setRotation(1);

#ifdef TFT_BL
  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TFT_BL, HIGH);
#endif

  mostrarPantallaConexion();

  pinMode(PIN_UP, INPUT_PULLUP);
  pinMode(PIN_DOWN, INPUT_PULLUP);
  pinMode(PIN_LEFT, INPUT_PULLUP);
  pinMode(PIN_RIGHT, INPUT_PULLUP);

  pinMode(PIN_A, INPUT_PULLUP);
  pinMode(PIN_B, INPUT_PULLUP);
  pinMode(PIN_START, INPUT_PULLUP);
  pinMode(PIN_SELECT, INPUT_PULLUP);
  
  pinMode(PIN_TURBO_A, INPUT_PULLUP);
  pinMode(PIN_TURBO_B, INPUT_PULLUP);

  config.setGamepadMode(GamepadMode::Generic);
  config.setIncludeStart(true);
  config.setIncludeSelect(true);

  bleGamepad.begin(&config);
  Serial.println("Perfil listo.");
}

// =====================================================
// BUCLE PRINCIPAL
// =====================================================

void loop() {
  bool conectado = bleGamepad.isConnected();

  if (conectado && !conectadoAnterior) {
    Serial.println("Mando conectado!");
    mostrarConexionExitosa();
  }

  if (!conectado && conectadoAnterior) {
    Serial.println("Mando desconectado.");
    ultimoBotonActual = "";
    mostrarPantallaConexion();
  }

  conectadoAnterior = conectado;

  if (!conectado) {
    delay(20);
    return;
  }

  // Lectura de botones (LOW = presionado)
  bool up     = digitalRead(PIN_UP)     == LOW;
  bool down   = digitalRead(PIN_DOWN)   == LOW;
  bool left   = digitalRead(PIN_LEFT)   == LOW;
  bool right  = digitalRead(PIN_RIGHT)  == LOW;

  bool a      = digitalRead(PIN_A)      == LOW;
  bool b      = digitalRead(PIN_B)      == LOW;
  bool start  = digitalRead(PIN_START)  == LOW;
  bool select = digitalRead(PIN_SELECT) == LOW;
  
  bool turboA = digitalRead(PIN_TURBO_A) == LOW;
  bool turboB = digitalRead(PIN_TURBO_B) == LOW;

  // ---------------------------------------------------
  // CRUCETA DIGITAL (HAT)
  // ---------------------------------------------------
  int dpadVal = DPAD_CENTERED;

  if (up && right) {
    dpadVal = DPAD_UP_RIGHT;
  } else if (up && left) {
    dpadVal = DPAD_UP_LEFT;
  } else if (down && right) {
    dpadVal = DPAD_DOWN_RIGHT;
  } else if (down && left) {
    dpadVal = DPAD_DOWN_LEFT;
  } else if (up) {
    dpadVal = DPAD_UP;
  } else if (down) {
    dpadVal = DPAD_DOWN;
  } else if (left) {
    dpadVal = DPAD_LEFT;
  } else if (right) {
    dpadVal = DPAD_RIGHT;
  }

  bleGamepad.setHat1(dpadVal);
  bleGamepad.setAxes(0, 0, 0, 0, 0, 0, 0, 0);

  // ---------------------------------------------------
  // TURBO PULSOS
  // ---------------------------------------------------
  if (millis() - tiempoUltimoTurbo >= intervaloTurbo) {
    tiempoUltimoTurbo = millis();
    estadoTurboPulsos = !estadoTurboPulsos;
  }

  bool estadoFinalA = a || (turboA && estadoTurboPulsos);
  if (estadoFinalA) { bleGamepad.press(BUTTON_1); } else { bleGamepad.release(BUTTON_1); }

  bool estadoFinalB = b || (turboB && estadoTurboPulsos);
  if (estadoFinalB) { bleGamepad.press(BUTTON_4); } else { bleGamepad.release(BUTTON_4); }

  if (select) { bleGamepad.press(BUTTON_11); } else { bleGamepad.release(BUTTON_11); }
  if (start)  { bleGamepad.press(BUTTON_12); } else { bleGamepad.release(BUTTON_12); }

  // ---------------------------------------------------
  // LÓGICA DE DETECCIÓN DE BOTÓN ACTUAL Y TIEMPO DE INACTIVIDAD
  // ---------------------------------------------------
  String botonDetectado = "";
  uint16_t colorBoton = TFT_WHITE;

  if (up && right)       { botonDetectado = "UP-RIGHT"; colorBoton = TFT_ORANGE; }
  else if (up && left)   { botonDetectado = "UP-LEFT"; colorBoton = TFT_ORANGE; }
  else if (down && right){ botonDetectado = "DN-RIGHT"; colorBoton = TFT_ORANGE; }
  else if (down && left) { botonDetectado = "DN-LEFT"; colorBoton = TFT_ORANGE; }
  else if (up)           { botonDetectado = "ARRIBA"; colorBoton = TFT_CYAN; }
  else if (down)         { botonDetectado = "ABAJO"; colorBoton = TFT_CYAN; }
  else if (left)         { botonDetectado = "IZQ"; colorBoton = TFT_CYAN; }
  else if (right)        { botonDetectado = "DER"; colorBoton = TFT_CYAN; }
  else if (a)            { botonDetectado = "A"; colorBoton = TFT_RED; }
  else if (b)            { botonDetectado = "B"; colorBoton = TFT_GREEN; }
  else if (turboA)       { botonDetectado = "TURBO A"; colorBoton = TFT_MAGENTA; }
  else if (turboB)       { botonDetectado = "TURBO B"; colorBoton = TFT_MAGENTA; }
  else if (start)        { botonDetectado = "START"; colorBoton = TFT_WHITE; }
  else if (select)       { botonDetectado = "SELECT"; colorBoton = TFT_WHITE; }

  if (botonDetectado != "") {
    tiempoUltimaAccion = millis(); // Reinicia el contador de inactividad global
    agregarAlHistorial(botonDetectado);
    actualizarPantalla(botonDetectado, colorBoton);
  } else {
    unsigned long tiempoInactivo = millis() - tiempoUltimaAccion;

    // Si pasan más de 10 segundos sin tocar nada, ejecutamos el scrolling tipo Star Wars de Nacional
    if (tiempoInactivo >= tiempoEsperaScroll) {
      bool terminado = ejecutarScrollingNacional();
      
      // Reseteamos el temporizador para que ahora empiece a contar desde cero con el escudo en pantalla
      tiempoUltimaAccion = millis(); 
    } 
    // Si pasaron al menos 1 segundo pero menos de 10, mostramos el Escudo
    else if (tiempoInactivo >= tiempoEsperaEscudo) {
      actualizarPantalla("ESCUDO", TFT_BLACK);
    }
  }

  // Diagnóstico por Serial
  static unsigned long ultimaLectura = 0;
  if (millis() - ultimaLectura >= 500) {
    ultimaLectura = millis();
    Serial.printf("A:%d | TurboA:%d | B:%d | TurboB:%d | START:%d | SELECT:%d\n", a, turboA, b, turboB, start, select);
  }

  delay(10);
}
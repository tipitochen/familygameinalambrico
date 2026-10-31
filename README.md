# 🎮 Family Game Inalámbrico & Control Inalámbrico Casero (ESP32)

¡Bienvenido al repositorio oficial del proyecto! Aquí encontrarás dos soluciones independientes pero complementarias para revivir y modernizar consolas clásicas:

1. **Family Game Inalámbrico (BlueRetro):** Adaptación para que tu consola retro de 8 bits (marca Argenvision, CCE, Froggy o similares) acepte controles Bluetooth modernos utilizando el proyecto BlueRetro.
2. **Control Inalámbrico Casero (ESP32):** Código fuente y esquemas para crear tu propio mando de juegos inalámbrico desde cero con un microcontrolador ESP32 y la librería `BleGamepad.h`.

* **ESP32 utilizado:** [Ver modelo de ESP32 en AliExpress](https://www.aliexpress.us/item/3256809703899160.html)
* **Conversor de nivel lógico (Logic Level Shifter 3.3V - 5V):** Necesario para adaptar de forma segura los niveles de voltaje entre el ESP32 (3.3V) y los buses de datos de consolas como el Family Game (5V).  
  👉 [Ver conversor de nivel lógico en AliExpress](https://www.aliexpress.us/item/3256806983586777.html)
  
 
📺 **Pasate por mi canal de youtub:**  
👉 [**Canal de YouTube: @tipitochen**](https://www.youtube.com/@tipitochen)

## 🕹️ Adaptación del Family Game (BlueRetro)

Para lograr que una consola clásica como el **Family Game** (modelos de marcas como Argenvision, CCE, Froggy, etc.) acepte controles inalámbricos modernos de forma nativa, este proyecto utiliza la arquitectura de código abierto **BlueRetro**.

* **¿Qué es BlueRetro?** Es un adaptador Bluetooth multidispositivo para sistemas de videojuegos retro desarrollado por *retro جماعه* / *darthcloud*.
* **Proyecto oficial y código fuente:**  👉 [Repositorio oficial de BlueRetro en GitHub](https://github.com/darthcloud/BlueRetro)
---

## 📂 Estructura del Repositorio

La organización de archivos en el repositorio sigue exactamente esta estructura:

```text
Family Game Inalambrico/
└── Control_inalambrico/
    ├── Control_inalambrico.ino
    └── EscudoNacional.h
```

* **`Family Game Inalambrico/`**: Documentación y esquemas de conexionado hacia la consola Family Game.
* **`Control_inalambrico/`**:
  * **`Control_inalambrico.ino`**: Código ejecutable en Arduino IDE para emular un Gamepad Bluetooth con soporte para botones principales y funciones de Turbo.
  * **`EscudoNacional.h`**: Archivo de cabecera opcional que contiene los datos gráficos del Escudo Nacional (es totalmente opcional y puede eliminarse del proyecto si no se necesita).

---

## 🕹️ Tabla de Conexiones de Pines (Control Casero)

Todos los botones se conectan entre el pin **GPIO** indicado y **GND (Tierra)**, aprovechando las resistencias *pull-up* internas del ESP32 (`INPUT_PULLUP`).

| Función del Botón | Pin GPIO (ESP32) | Etiqueta en el Código (`.ino`) | Ubicación / Descripción |
| :--- | :---: | :---: | :--- |
| **ARRIBA** | `21` | `PIN_UP` | D-Pad Dirección Arriba |
| **ABAJO** | `12` | `PIN_DOWN` | D-Pad Dirección Abajo |
| **IZQUIERDA** | `25` | `PIN_LEFT` | D-Pad Dirección Izquierda |
| **DERECHA** | `26` | `PIN_RIGHT` | D-Pad Dirección Derecha |
| **BOTÓN A** | `27` | `PIN_A` | Botón Principal A |
| **BOTÓN B** | `33` | `PIN_B` | Botón Principal B |
| **START** | `32` | `PIN_START` | Botón Start |
| **SELECT** | `2` | `PIN_SELECT` | Botón Select |
| **TURBO A** | `17` | `PIN_TURBO_A` | Botón físico para ráfaga/turbo de A |
| **TURBO B** | `15` | `PIN_TURBO_B` | Botón físico para ráfaga/turbo de B |

---

## 📸 Diagramas e Imágenes de Referencia

### 1. Tabla gráfica de asignación de pines para conectar un esp32 a un family game Argenvision
![Esquema Family Game Argenvision](Family%20Game%20Inalambrico/Argenvision.png)

### 2. Esquema físico de cableado de pulsadores  para crear tu propio control bluetooh con un esp32. 
![Esquema ESP32 Control](Family%20Game%20Inalambrico/Esquemas%20esp32-%20control.png)

## 💻 Compilación en Arduino IDE

1. Asegúrate de tener instalado el core de **ESP32** en Arduino IDE.
2. Instala la librería **`ESP32-BLE-Gamepad`** (por *lemmingDev*) desde el gestor de librerías.
3. Abre el archivo `Control_inalambrico.ino`. Si no vas a usar la pantalla con el escudo nacional, puedes borrar el archivo `EscudoNacional.h` y quitar la línea `#include "EscudoNacional.h"`.
4. Selecciona tu placa ESP32 y sube el programa.

---

## 📺 Créditos y Canal de YouTube

Proyecto desarrollado por **Tipitochen**.

* **YouTube:** [@tipitochen](https://www.youtube.com/@tipitochen)
* **Temática:** Hardware modding, consolas retro, impresión 3D y proyectos con microcontroladores.

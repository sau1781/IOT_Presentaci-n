#  Tutorial Práctico: Gestión de un Nodo ESP32 con ThingsBoard y Wokwi

**Expositor:** Saúl Quesada Soto  
**Tiempo estimado:** 15 minutos  
**Tecnologías:** ESP32 · Sensor DHT22 · Protocolo MQTT · ThingsBoard Cloud · Simulador Wokwi

---

## Descripción de la Práctica

En este tutorial conectarás un nodo **ESP32** simulado en **Wokwi** (con un sensor de temperatura/humedad **DHT22** en el `GPIO 15` y un **LED** en el `GPIO 13`) hacia la plataforma **ThingsBoard Cloud** mediante el protocolo **MQTT**.

Durante el ejercicio lograrás:
1. Aprovisionar un dispositivo en **ThingsBoard** y obtener su credencial (`Access Token`).
2. Enviar **telemetría en tiempo real** (`temperature`, `humidity`, `led_status`, `free_heap_bytes`, `uptime_ms`).
3. Construir un **Dashboard** con gráficas de series de tiempo y un interruptor de comando remoto (**RPC**) para encender y apagar el LED desde la nube.

---

##  Enlaces Requeridos (Ábrelos en dos pestañas)

1. **Simulación base en Wokwi (Circuito y Código listos):**  
   👉 [https://wokwi.com/projects/480366532077592577](https://wokwi.com/projects/480366532077592577)
2. **Plataforma ThingsBoard Cloud:**  
   👉 [https://thingsboard.cloud](https://thingsboard.cloud) *(Crea una cuenta gratuita o inicia sesión).*

---

## 🚀 Paso 1: Crear el Dispositivo en ThingsBoard y Copiar el Token

1. Inicia sesión en **[ThingsBoard Cloud](https://thingsboard.cloud)**.
2. En el menú lateral izquierdo, haz clic en **Devices & assets** $\rightarrow$ **Devices (Dispositivos)**.
3. Haz clic en el botón **`+ Add device`** (esquina superior derecha) y selecciona **Add new device**.
4. En el campo **Name**, escribe un nombre para tu nodo (por ejemplo: `ESP32_Nodo_Laboratorio`) y haz clic en **Add** (abajo a la derecha). *(Si te aparece una ventana adicional de credenciales, ciérrala o dale a Close).*
5. En la lista de dispositivos, haz clic sobre el dispositivo que acabas de crear para abrir sus detalles y presiona el botón **Copy access token**.

---

## 💻 Paso 2: Vincular tu Token en Wokwi e Iniciar la Simulación

1. Abre el enlace del proyecto en Wokwi: **[https://wokwi.com/projects/480366532077592577]**.
2. En el editor de código (`sketch.ino`), ubica la **línea 17**:
   ```cpp
   const char* token = "-----------"; // <-- REVISA QUE ESTE SEA TU TOKEN ACTUAL
   ```
3. **Reemplaza** el texto dentro de las comillas por el **Access Token** que copiaste de tu propia cuenta de ThingsBoard.
4. Presiona el botón verde de **Play (Start the simulation)** en Wokwi.
5. Verifica en el **Monitor Serial** inferior de Wokwi que aparezcan los mensajes:
   * `¡WiFi conectado exitosamente!`
   * `Conectando a ThingsBoard MQTT...[CONECTADO]`
   * `Telemetría MQTT enviada: {"uptime_ms":...,"temperature":...}`

> 💡 **Tip en Wokwi:** Mientras la simulación está corriendo, puedes hacer clic sobre el sensor **DHT22** en el diagrama y mover los controles deslizantes de **Temperature** y **Humidity** para simular cambios en el ambiente.

---

## 📡 Paso 3: Verificar la Telemetría en Tiempo Real

1. Regresa a la pestaña de **ThingsBoard Cloud**.
2. Abre los detalles de tu dispositivo (`Entities` $\rightarrow$ `Devices` $\rightarrow$ haz clic sobre tu dispositivo).
3. Selecciona la pestaña **Latest telemetry (Última telemetría)**.
4. Verás actualizarse cada 5 segundos las variables que envía el ESP32:
   * `temperature` (Temperatura en °C)
   * `humidity` (Humedad relativa en %)
   * `led_status` (Estado actual del LED: `0` apagado, `1` encendido)
   * `free_heap_bytes` (Memoria RAM libre del ESP32)
   * `uptime_ms` (Tiempo de actividad en milisegundos)

---

## 📊 Paso 4: Crear el Dashboard (Visualizar Temperatura y Estado del LED)

Desde la misma pestaña **Latest telemetry** de tu dispositivo, crearemos el panel visual rápidamente:

1. Marca las casillas a la izquierda de **`temperature`**, **`humidity`** y **`led_status`**.
2. Haz clic en el botón **Show on widget (Mostrar en widget)** que aparece en la parte superior.
3. En **Current bundle**, selecciona **Charts (Gráficos)**.
4. Desliza las opciones hasta encontrar **Timeseries series chart** y haz clic en **Add to dashboard (Agregar al panel)**.
5. Selecciona la opción **Create new dashboard**, escribe como nombre `Dashboard_ESP32_Taller`, marca la casilla **Open dashboard** y haz clic en **Add**.

¡Listo! Se abrirá tu Dashboard mostrando en vivo las curvas de temperatura, humedad y estado del LED.

---

## 🎛️ Paso 5: Agregar el Botón (Switch RPC) para Encender el LED

Ahora agregaremos un interruptor en el mismo Dashboard para enviar el comando remoto `set_led` hacia el ESP32 en Wokwi:

1. Dentro de tu `Dashboard_ESP32_Taller`, haz clic en el botón **Edit mode** (el ícono del **lápiz** en la esquina superior derecha).
2. Haz clic en el botón **`+ Add widget`** en la barra superior.
3. En el buscador de widgets, escribe **`Control widgets`** y selecciona el widget **Single Switch** 
4. Asegúrate de seleccionar tu dispositivo (`ESP32_Nodo_Laboratorio`) para vincular el widget.
5. **Configurar el comportamiento RPC (`set_led`):**
   Baja hasta la sección **Behavior** y edita las opciones haciendo clic en el **ícono del lápiz** a la derecha de cada una:
   * **Initial state:** Cambia la acción a **Get time series** y selecciona Time series key **`led_status`** y selecciona apply (la fila debe verse como `Use time series 'led_status'`).
   * **Turn 'On' (Encender):** Asegúrate de que la acción sea **Execute RPC**, cambia el campo *Method* a **`set_led`** y verifica que el parámetro sea **`true`** y selecciona apply (la fila debe verse como `Execute RPC method 'set_led(true)'`).
   * **Turn 'Off' (Apagar):** Asegúrate de que la acción sea **Execute RPC**, cambia el campo *Method* a **`set_led`** y verifica que el parámetro sea **`false`** y selecciona apply (la fila debe verse como `Execute RPC method 'set_led(false)'`).
6. Haz clic en **Add** (abajo a la derecha) para insertar el interruptor en el panel y luego haz clic en el botón **Save** (el ícono de check naranja en la esquina superior derecha del Dashboard) para guardar todos los cambios.

### 🧪 Prueba de Control Remoto (RPC)
* Activa el interruptor en tu Dashboard de ThingsBoard y observa la pestaña de **Wokwi**:
  * En el Monitor Serial de Wokwi aparecerá al instante: `-> RPC: LED ENCENDIDO` (o `-> RPC: LED APAGADO`).
  * El LED virtual conectado al pin `13` cambiará de estado.

---

## ⚠️ Posibles Errores y Cómo Solucionarlos (Troubleshooting)

### 1. En Wokwi sale `Fallo, rc=-2` o `rc=-4` o `rc=5` al conectar a MQTT
* **`rc=5` (No autorizado):** El `token` en la línea 17 del código no coincide con el de tu dispositivo en ThingsBoard o tiene un espacio en blanco al inicio/final. Vuelve a copiar el **Access Token** en ThingsBoard y pégalo entre las comillas.
* **`rc=-2` (Fallo de red):** Verifica que `mqtt_server` sea exactamente `"thingsboard.cloud"` y que en `WiFi.begin(ssid, password, 6);` no hayas borrado `"Wokwi-GUEST"`.

### 2. Todos los compañeros dejaron el mismo Token del ejemplo
* Si varias personas ejecutan la simulación al mismo tiempo sin cambiar el token de la línea 17 (`"awq1vsunns1hulk3o0dq"`), todos los simuladores enviarán datos al mismo dispositivo y se sobrescribirán las lecturas. **Cada estudiante debe usar su propio Access Token.**

### 3. Enciendo el LED con el botón del Dashboard, pero a los pocos segundos se apaga solo
* **¿Por qué ocurre?** En el `loop()` del código existe una regla de alarma local que se ejecuta cada 5 segundos:
  ```cpp
  if (data.temperature > 35 || data.temperature < 12 || data.humidity > 70 || data.humidity < 40) {
    digitalWrite(LED_PIN, HIGH);
  } else {
    digitalWrite(LED_PIN, LOW);
  }
  ```
* **Efecto:** Si la temperatura del DHT22 en Wokwi está en rango normal (entre $12\text{ }^\circ\text{C}$ y $35\text{ }^\circ\text{C}$) y la humedad entre $40\text{ \%}$ y $70\text{ \%}$, cuando tú enciendes el LED por RPC se prende de inmediato, pero en el siguiente ciclo de 5 segundos la condición `else` lo vuelve a apagar (`LOW`).
* **Cómo verificarlo:** Mira el Monitor Serial de Wokwi en el instante en que presionas el Switch (`-> RPC: LED ENCENDIDO`), o sube la temperatura del DHT22 por encima de $35\text{ }^\circ\text{C}$ haciendo clic sobre el sensor en Wokwi para observar también cómo se activa la alerta local.

### 4. El Switch del Dashboard da error de "Timeout" o no enciende el LED
* **Causa:** Por defecto, el widget *Switch control* de ThingsBoard envía el método `"setValue"`, mientras que el código en Wokwi espera el método `"set_led"` (`if (method == "set_led")`).
* **Solución:** Entra en modo edición en el Dashboard (ícono del lápiz), edita el widget del Switch (ícono del lápiz dentro del widget), busca la opción **Value change method** y cámbiala de `setValue` a **`set_led`**.

### 5. No aparece nada en la pestaña "Latest telemetry"
* Asegúrate de que la simulación en Wokwi esté corriendo (reloj en marcha arriba del diagrama) y espera al menos 5 segundos para que se cumpla el primer ciclo de envío (`if (now - lastMsg > 5000)`).

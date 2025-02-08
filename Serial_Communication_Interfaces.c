// Bibliotecas utilizadas no programa
#include "stdio.h"
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "hardware/timer.h"
#include "inc/ssd1306.h"
#include "inc/font.h"
#include "ws2812.pio.h"

// Definições dos pinos
#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define endereco 0x3C
#define PIN_WS2812 7
#define NUM_PIXELS 25
#define PIN_LED_G 11
#define PIN_LED_B 12
#define BUTTON_A 5
#define BUTTON_B 6

// Variáveis globais para controlar o display, debouncing dos botões e intensidade dos LEDs da matriz (entre 0 e 255 para intensidade)
ssd1306_t ssd;
static volatile bool cor = false;
static volatile uint32_t last_time = 0;
uint8_t led_r = 150;
uint8_t led_g = 0;
uint8_t led_b = 0;

// Matriz para armazenar os números (0 a 9) que serão exibidos
bool numeros[10][NUM_PIXELS] = {
  {
  0, 0, 1, 0, 0, 
  0, 1, 0, 1, 0, 
  0, 1, 0, 1, 0, 
  0, 1, 0, 1, 0, 
  0, 0, 1, 0, 0
  },
  {
  0, 1, 1, 1, 0, 
  0, 0, 1, 0, 0, 
  0, 0, 1, 0, 0, 
  0, 1, 1, 0, 0, 
  0, 0, 1, 0, 0
  },
  {
  0, 1, 1, 1, 0, 
  0, 1, 0, 0, 0, 
  0, 0, 1, 0, 0, 
  0, 1, 0, 1, 0, 
  0, 0, 1, 0, 0
  },
  {
  0, 0, 1, 1, 0, 
  0, 0, 0, 1, 0, 
  0, 1, 0, 0, 0, 
  0, 0, 1, 0, 0, 
  0, 1, 1, 1, 0
  },
  {
  0, 1, 1, 1, 0, 
  0, 0, 1, 0, 0, 
  0, 1, 1, 1, 1, 
  0, 1, 1, 0, 0, 
  0, 0, 1, 0, 0
  },
  {
  0, 0, 1, 1, 0, 
  0, 0, 0, 1, 0, 
  0, 1, 0, 0, 0, 
  0, 1, 1, 0, 0, 
  0, 1, 1, 1, 0
  },
  {
  0, 0, 1, 0, 0, 
  0, 1, 0, 1, 0, 
  0, 1, 1, 1, 0, 
  0, 1, 0, 0, 0, 
  0, 0, 1, 0, 0
  },
  {
  0, 0, 0, 1, 0, 
  0, 0, 1, 0, 0, 
  0, 0, 0, 0, 0, 
  0, 0, 0, 1, 0, 
  0, 1, 1, 1, 0
  },
  {
  0, 0, 1, 0, 0, 
  0, 1, 0, 1, 0, 
  0, 0, 1, 0, 0, 
  0, 1, 0, 1, 0, 
  0, 0, 1, 0, 0
  },
  {
  0, 0, 1, 0, 0, 
  0, 0, 0, 1, 0, 
  0, 1, 1, 1, 0, 
  0, 1, 0, 1, 0, 
  0, 0, 1, 0, 0
  }
};


// Prototipação das rotinas
void initialize_all();
void gpio_irq_handler(uint gpio, uint32_t events);
void read_display_character();
static inline void put_pixel(uint32_t pixel_grb);
static inline uint32_t urgb_u32(uint8_t r, uint8_t g, uint8_t b);
void set_one_led(uint8_t r, uint8_t g, uint8_t b, uint8_t numero);


// Rotina principal
int main()
{
  // Inicialização e configuração dos pinos e periféricos
  initialize_all();

  // Configuração da interrupção com callback para os botões A e B
  gpio_set_irq_enabled_with_callback(BUTTON_A, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
  gpio_set_irq_enabled_with_callback(BUTTON_B, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);

  while (1)
  {
    // Certifica-se de que o USB está conectado
    if (stdio_usb_connected())
    {
      // Chama a rotina para ler e exibir o caractere digitado no display e no Serial Monitor
      read_display_character();
    }

    sleep_ms(40);
  }
}


// Inicializa e configura os pinos e periféricos que serão utilizados no programa
void initialize_all()
{
  stdio_init_all();

  // I2C Initialisation. Using it at 400Khz.
  i2c_init(I2C_PORT, 400 * 1000);

  gpio_set_function(I2C_SDA, GPIO_FUNC_I2C); // Set the GPIO pin function to I2C
  gpio_set_function(I2C_SCL, GPIO_FUNC_I2C); // Set the GPIO pin function to I2C
  gpio_pull_up(I2C_SDA); // Pull up the data line
  gpio_pull_up(I2C_SCL); // Pull up the clock line
  ssd1306_init(&ssd, WIDTH, HEIGHT, false, endereco, I2C_PORT); // Inicializa o display
  ssd1306_config(&ssd); // Configura o display
  ssd1306_send_data(&ssd); // Envia os dados para o display

  // Limpa o display. O display inicia com todos os pixels apagados
  ssd1306_fill(&ssd, false);
  ssd1306_send_data(&ssd);

  // Inicializa o pino do LED verde, define a direção como saída e garante que o LED estará desligado
  gpio_init(PIN_LED_G);
  gpio_set_dir(PIN_LED_G, GPIO_OUT);
  gpio_put(PIN_LED_G, false);

  // Inicializa o pino do LED azul, define a direção como saída e garante que o LED estará desligado
  gpio_init(PIN_LED_B);
  gpio_set_dir(PIN_LED_B, GPIO_OUT);
  gpio_put(PIN_LED_B, false);

  // Inicializa o pino do botão A e define a direção como entrada, com pull-up
  gpio_init(BUTTON_A);
  gpio_set_dir(BUTTON_A, GPIO_IN);
  gpio_pull_up(BUTTON_A);
  
  // Inicializa o pino do botão B e define a direção como entrada, com pull-up
  gpio_init(BUTTON_B);
  gpio_set_dir(BUTTON_B, GPIO_IN);
  gpio_pull_up(BUTTON_B);

  // Inicializa o PIO e configura o estado da máquina e o programa WS2812
  PIO pio = pio0;
  int sm = 0;
  uint offset = pio_add_program(pio, &ws2812_program);

  // Configura o programa WS2812
  ws2812_program_init(pio, sm, offset, PIN_WS2812, 800000, false);
}
  
// Função de interrupção com debouncing
void gpio_irq_handler(uint gpio, uint32_t events)
{
  // Obtém o tempo atual em microssegundos
  uint32_t current_time = to_us_since_boot(get_absolute_time());
    
  // Verifica se passou tempo suficiente desde o último evento
  if (current_time - last_time > 200000) // 200 ms de debouncing
  {
    last_time = current_time; // Atualiza o tempo do último evento

    // Conforme o botão pressionado, o estado do LED é alterado e é exibida uma mensagem referente ao estado tanto no display quanto no Serial Monitor
    if (gpio == 5)
    {
      gpio_put(PIN_LED_G, !gpio_get(PIN_LED_G));
      
      if (gpio_get(PIN_LED_G))
      {
        ssd1306_fill(&ssd, false);
        ssd1306_draw_string(&ssd, "led green on", 1, 30);
        ssd1306_send_data(&ssd);  
        printf("LED verde ligado\n");
      }
      else
      {
        ssd1306_fill(&ssd, false);
        ssd1306_draw_string(&ssd, "led green off", 1, 30);
        ssd1306_send_data(&ssd);  
        printf("LED verde desligado\n");
      }
    }
    else if (gpio == 6) 
    {
      gpio_put(PIN_LED_B, !gpio_get(PIN_LED_B));

      if (gpio_get(PIN_LED_B))
      {
        ssd1306_fill(&ssd, false);
        ssd1306_draw_string(&ssd, "led blue on", 1, 30);
        ssd1306_send_data(&ssd);  
        printf("LED azul ligado\n");
      }
      else
      {
        ssd1306_fill(&ssd, false);
        ssd1306_draw_string(&ssd, "led blue off", 1, 30);
        ssd1306_send_data(&ssd);  
        printf("LED azul desligado\n");
      }
    }
  }
}

// Lê o caractere da entrada e exibe no display e no Serial Monitor
void read_display_character()
{
  char c[2];
  uint8_t num;

  // Lê caractere da entrada
  if (scanf("%c", &c[0]))
  { 
    num = c[0] - '0';

    // Exibe o caractere no Serial Monitor
    printf("Recebido: '%c'\n", c[0]);

    // Exibe o caractere no display
    ssd1306_fill(&ssd, false);
    ssd1306_draw_string(&ssd, c, 50, 30);
    ssd1306_send_data(&ssd);

    // Caso o caractere seja um número, será exibido também na matriz de LEDS
    if (num >= 0 && num <= 9)
    {
      set_one_led(led_r, led_g, led_b, num);
    }
  }
}

// Envia o valor do pixel para a máquina de estado do PIO
static inline void put_pixel(uint32_t pixel_grb)
{
    pio_sm_put_blocking(pio0, 0, pixel_grb << 8u);
}

// Converte os valores RGB para um único valor de 32 bits
static inline uint32_t urgb_u32(uint8_t r, uint8_t g, uint8_t b)
{
    return ((uint32_t)(r) << 8) | ((uint32_t)(g) << 16) | (uint32_t)(b);
}

// Define a cor de um LED específico na matriz
void set_one_led(uint8_t r, uint8_t g, uint8_t b, uint8_t numero)
{
    // Define a cor com base nos parâmetros fornecidos
    uint32_t color = urgb_u32(r, g, b);

    // // Percorre todos os LEDs e define a cor com base no valor da matriz numeros
    for (uint8_t i = 0; i < NUM_PIXELS; i++)
    {
        if (numeros[numero][i])
        {
            put_pixel(color);
        }
        else
        {
            put_pixel(0);
        }
    }
}
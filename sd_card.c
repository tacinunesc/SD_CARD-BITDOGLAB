#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/spi.h"
#include "hardware/i2c.h"
#include "ff.h"
#include "diskio.h"
#include "inc/ssd1306.h"
#include "inc/ssd1306_fonts.h"

// === Definições de pinos ===
#define LED_VERMELHO 13
#define LED_VERDE    11
#define LED_AZUL     12
#define BUZZER       21
#define BOTAO_A      5
#define BOTAO_B      6
#define JOY_X        26  // ADC0
#define JOY_Y        27  // ADC1

#define PIN_MISO     16
#define PIN_MOSI     19
#define PIN_SCK      18
#define PIN_CS       17

#define SDA_I2C      8
#define SCL_I2C      9
#define I2C_PORT     i2c0

FATFS fs;
FIL file;

// === Inicializa cartão SD via SPI ===
void inicializar_sd() {
    spi_init(spi0, 1000 * 1000);
    gpio_set_function(PIN_MISO, GPIO_FUNC_SPI);
    gpio_set_function(PIN_MOSI, GPIO_FUNC_SPI);
    gpio_set_function(PIN_SCK, GPIO_FUNC_SPI);
    gpio_init(PIN_CS);
    gpio_set_dir(PIN_CS, GPIO_OUT);
    gpio_put(PIN_CS, 1);

    FRESULT fr = f_mount(&fs, "", 1);
    if (fr != FR_OK) {
        printf("Erro ao montar SD: %d\n", fr);
    } else {
        printf("Cartão SD montado com sucesso.\n");
        fr = f_open(&file, "bitdoglab.txt", FA_WRITE | FA_OPEN_APPEND);
        if (fr != FR_OK) {
            printf("Erro ao abrir arquivo: %d\n", fr);
        } else {
            f_puts("Evento,Timestamp\n", &file);
            f_sync(&file);
        }
    }
}

// === Registra evento no SD com timestamp ===
void registrar_evento(const char* evento) {
    char linha[64];
    snprintf(linha, sizeof(linha), "%s,%lu\n", evento, to_ms_since_boot(get_absolute_time()));
    UINT bw;
    FRESULT fr = f_write(&file, linha, strlen(linha), &bw);
    if (fr != FR_OK) {
        printf("Erro ao escrever no arquivo: %d\n", fr);
    } else {
        f_sync(&file);
        printf("Evento registrado: %s", linha);
    }
}

// === Exibe evento no OLED ===
void exibir_oled(const char* mensagem) {
    ssd1306_Fill(Black);
    ssd1306_SetCursor(0, 0);
    ssd1306_WriteString("EVENTO DETECTADO", Font_6x8, White);
    ssd1306_SetCursor(0, 16);
    ssd1306_WriteString((char*)mensagem, Font_6x8, White);
    ssd1306_UpdateScreen();
}

// === Função principal ===
int main() {
    stdio_init_all();
    while (!stdio_usb_connected()) sleep_ms(100);

    // Inicializa I2C para OLED
    i2c_init(I2C_PORT, 100 * 1000);
    gpio_set_function(SDA_I2C, GPIO_FUNC_I2C);
    gpio_set_function(SCL_I2C, GPIO_FUNC_I2C);
    gpio_pull_up(SDA_I2C);
    gpio_pull_up(SCL_I2C);

    // Inicializa OLED
    ssd1306_Init();
    ssd1306_Fill(Black);
    ssd1306_UpdateScreen();

    // Inicializa GPIOs
    gpio_init(LED_VERMELHO); gpio_set_dir(LED_VERMELHO, GPIO_OUT);
    gpio_init(LED_VERDE);    gpio_set_dir(LED_VERDE, GPIO_OUT);
    gpio_init(LED_AZUL);     gpio_set_dir(LED_AZUL, GPIO_OUT);
    gpio_init(BUZZER);       gpio_set_dir(BUZZER, GPIO_OUT);

    gpio_init(BOTAO_A); gpio_set_dir(BOTAO_A, GPIO_IN); gpio_pull_up(BOTAO_A);
    gpio_init(BOTAO_B); gpio_set_dir(BOTAO_B, GPIO_IN); gpio_pull_up(BOTAO_B);

    // Inicializa ADC para joystick
    adc_init();
    adc_gpio_init(JOY_X);
    adc_gpio_init(JOY_Y);

    // Inicializa cartão SD
    inicializar_sd();

    // === Loop principal ===
    while (true) {
        bool a_pressionado = !gpio_get(BOTAO_A);
        bool b_pressionado = !gpio_get(BOTAO_B);

        if (a_pressionado && b_pressionado) {
            gpio_put(BUZZER, 1);
            registrar_evento("BUZZER ATIVADO");
            exibir_oled("BUZZER ATIVADO");
            sleep_ms(300);
            gpio_put(BUZZER, 0);
        } else if (a_pressionado) {
            gpio_put(LED_VERMELHO, 1);
            registrar_evento("BOTAO A PRESSIONADO");
            exibir_oled("BOTAO A PRESSIONADO");
            sleep_ms(300);
            gpio_put(LED_VERMELHO, 0);
        } else if (b_pressionado) {
            gpio_put(LED_VERDE, 1);
            registrar_evento("BOTAO B PRESSIONADO");
            exibir_oled("BOTAO B PRESSIONADO");
            sleep_ms(300);
            gpio_put(LED_VERDE, 0);
        }

        adc_select_input(0); uint16_t x = adc_read();
        adc_select_input(1); uint16_t y = adc_read();

        if (x < 1000 || x > 3000 || y < 1000 || y > 3000) {
            gpio_put(LED_AZUL, 1);
            registrar_evento("JOYSTICK MOVIDO");
            exibir_oled("JOYSTICK MOVIDO");
            sleep_ms(300);
            gpio_put(LED_AZUL, 0);
        }

        sleep_ms(200);
    }

    return 0;
}

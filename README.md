# 🔘🟢🔵🔴🕹💾 SD Card atuando com Pico W (BitDogLab)
![Linguagem](https://img.shields.io/badge/Linguagem-C-blue.svg)
![Plataforma](https://img.shields.io/badge/Plataforma-Raspberry%20Pi%20Pico-purple.svg)
![Sensor](https://img.shields.io/badge/SD-Card-blue.svg)

## Sobre o Projeto
Este projeto visa criar um firmware para controlar e vizualizar os LEDs pelo joystick e pelos botões A e B e estes dados são gravados no SD Card, com o display Oled pode ser observado os comandos.


## 🛠️ Estrutura do projeto
- sd_card.c – Programa principal em C que faz leitura dos movimentos dos botões e joystick
- Pasta inc - Onde esta localizada as informações da oled
- Pasta lib - Onde está localizada as bibliotecas do SD Card
- CMakeLists.txt – Configuração do build usando o Pico SDK

- ## 🔌 Requisitos
Hardware:

- Raspberry Pi Pico W
- SD Card
- LEDs
- Buzzer

## ⚙️ Como usar
1- Clone o repositorio

2- Deploy no Pico W
 - Segure o botão BOOTSEL do Pico W e conecte-o ao PC
 - Clique no botão run no Vscode ou arraste o arquivo .u2 para dentro do disco removível que aparecer
 - O Pico irá reiniciar executando o firmware
   
## 🔧 Funcionamento do Código
O programa realiza as seguintes ações:

1. Inicialização dos periféricos
- Gpio: 16 | 19 | 18 | 17 | 22 | para o sd card
- Inicializa o sensor vl53l0x e o display OLED SSD1306
- Configura os pinos dos LEDs RGB e buzzer

2. Leitura de componentes
A cada segundo, o sistema:

   * 🟢 Verde: botão B pressionado o LED VERDE será acionado
   * 🔴 Vermelho: Botão A pressionado o LED VERMELHO será acionado
   * 🔵 Azul: Movimentando o joystick o LED AZUL será acionado
   * 📢 Buzzer: Apertando os dois botões ao mesmo tempo o Buzzer será acionado
 
 
4. SD Card
-  A cada presença detectada é gravado no cartão SD: movimento jostick e botões

## 📦 Dependências

ssd1306.h e ssd1306_fonts.h para o display OLED


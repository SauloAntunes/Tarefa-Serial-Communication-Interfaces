# Interfaces de Comunicação Serial  

## Descrição do Projeto  

Este projeto tem como objetivo explorar a comunicação serial e o controle de dispositivos utilizando o microcontrolador RP2040 com a placa de desenvolvimento BitDogLab. Foram implementadas funcionalidades que envolvem a manipulação de uma matriz de LEDs WS2812, um LED RGB e um display SSD1306 via I2C. Adicionalmente, o projeto abrange o uso de interrupções e debouncing via software para botões, demonstrando a integração entre hardware e software.  

## Pré-requisitos  

Antes de executar o projeto, certifique-se de ter os seguintes softwares e ferramentas instalados:  

- [Visual Studio Code (VS Code)](https://code.visualstudio.com/download)  
- [Extensão Raspberry Pi Pico para VS Code](https://marketplace.visualstudio.com/items?itemName=raspberry-pi.raspberry-pi-pico)  
- [Extensão Wokwi Simulator para VS Code](https://marketplace.visualstudio.com/items?itemName=Wokwi.wokwi-vscode)  
- [Git](https://git-scm.com/downloads)  
- [SDK para Raspberry Pi Pico (Pico SDK)](#instalação-e-configuração-do-ambiente)  

Além disso, será necessário possuir uma conta no [site oficial do Wokwi](https://wokwi.com/).  

## Estrutura do Repositório  

```
├── .vscode/                		    # Configurações específicas do projeto para o VS Code
├── generated/              		    # Biblioteca para configuração do PIO no programa
├── img/                    		    # Imagens utilizadas no README para detalhar o projeto
├── inc/                    		    # Cabeçalhos das fontes (letras e números) e controle do display
├── src/                    		    # Código para configuração e controle do display
├── .gitignore              		    # Arquivos ignorados pelo Git
├── CMakeLists.txt          		    # Configuração do CMake para o projeto
├── README.md               		    # Instruções e detalhes do projeto
├── Serial_Communication_Interfaces.c   # Código-fonte principal do projeto
├── diagram.json            		    # Arquivo de configuração para o simulador Wokwi
├── pico_sdk_import.cmake   		    # Configuração para importar o Pico SDK
├── wokwi.toml              		    # Arquivo de configuração do Wokwi
└── ws2812.pio              		    # Código de controle para LEDs WS2812 usando PIO
```
`OBS.:` o subdiretório `build/` será adicionado ao diretório principal após a configuração automática do CMake.  

## Instalação e Configuração do Ambiente
1. Clone este repositório para seu ambiente local:  
   ```
   git clone https://github.com/SauloAntunes/Tarefa-Serial-Communication-Interfaces.git  
   ```

2. Com o VS Code aberto, configure o ambiente de desenvolvimento do Pico SDK, seguindo as instruções:  
    - O Pico SDK pode ser configurado de forma automática durante a configuração do projeto através da extensão Raspberry Pi Pico no VS Code.  
    ![Configuração do Pico SDK](img/Config-Pico-SDK.png)  
    - Passo a passo:  
    `1º:` acesse a extensão Raspberry Pi Pico;  
     `2º:` selecione a opção `Import Project`;  
    `3º:` adicione o caminho do projeto no seu dispositivo, selecione a versão 2.1.0 do Pico SDK (é importante selecionar essa versão para evitar possíveis incompatibilidades) , e por fim clique em `Import`.  
    `OBS.:` após isso, a própria ferramenta realizará a configuração do Pico SDK. Durante o processo de configuração, notificações serão exibidas no canto inferior direito da tela, indicando cada etapa.  

3. Compile o projeto:  
  ![Compilar projeto](img/Compile-Project.png)
  - Passo a passo:  
    `1º:` com o projeto aberto no VS Code, acesse a extensão Raspberry Pi Pico;  
    `2º:` clique na opção `Compile Project` e aguarde o processo de compilação.   

4. Com o VS Code aberto, configure o ambiente Wokwi, seguindo as instruções:
    - A configuração do Wokwi para VS Code pode ser realizada seguindo as orientações disponíveis na [documentação oficial](https://docs-wokwi-com.translate.goog/vscode/getting-started?_x_tr_sl=en&_x_tr_tl=pt&_x_tr_hl=pt&_x_tr_pto=tc&_x_tr_hist=true).

5. Inicie a simulação do projeto:  
    - Para iniciar a simulação do projeto clique no arquivo `diagram.json`, logo em seguida será aberta uma tela com a simulação do projeto, contendo os componentes como a placa Raspberry Pi Pico W, a matriz de LEDs, o display, entre outros. Após a abertura da simulação do projeto, clique no botão verde de começar.  

## Estrutura de Controle  

- Entrada de caracteres via PC  
	- Utilize o Serial Monitor do VS Code para digitar os caracteres;  
	- Cada caractere digitado no Serial Monitor será exibido no display SSD1306;  
	- Quando um número entre 0 e 9 for digitado, um símbolo correspondente ao número será exibido, também, na matriz 5x5 WS2812.  
- Interação com o Botão A  
	- Pressionar o botão A alterna o estado do LED RGB Verde (ligado/desligado);  
	- A operação será registrada de duas formas:  
		- Uma mensagem informativa sobre o estado do LED será exibida no display SSD1306;  
		- Um texto descritivo sobre a operação será enviado ao Serial Monitor.  
- Interação com o Botão B  
	- Pressionar o botão B alterna o estado do LED RGB Azul (ligado/desligado);  
	- A operação será registrada de duas formas:  
		- Uma mensagem informativa sobre o estado do LED será exibida no display SSD1306;  
		- Um texto descritivo sobre a operação será enviado ao Serial Monitor.  

## Vídeo de Apresentação da Solução

Para mais detalhes sobre a implementação e os resultados, assista ao vídeo da solução: [Link para o vídeo](https://drive.google.com/file/d/1TGt11weTWhNr3kVloiwLFjqAghAaTvQK/view?usp=sharing).  
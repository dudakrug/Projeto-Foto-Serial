# 📸 Projeto Foto Serial 

Projeto desenvolvido durante as aulas técnicas para criar um sistema embarcado capaz de receber dados via porta serial e exibir um slideshow de imagens em um display conectado ao microcontrolador STM32 G0B1RE.

---

## Como Funciona?

A lógica do projeto gira em torno da comunicação entre um programa externo em **C++** rodando no computador e o hardware. 

1. **Comunicação Serial:** Através de um arquivo binário (`.bin`) fornecido pelo professor, estabelecemos a conexão UART com a placa.
2. **Transferência de Imagens:** O programa em C++ gerencia o envio dos pacotes de dados correspondentes às imagens selecionadas.
3. **Exibição (Slideshow):** O **STM32G0B1RE** processa os dados recebidos pela serial e atualiza o display em tempo real, alternando as imagens em formato de apresentação automática (slideshow).

---

## 🛠️ Tecnologias e Hardware

* **Microcontrolador:** STM32G0B1RE
* **Linguagem (PC):** C++ (para a interface e comunicação serial)
* **Periféricos:** Display conectado à placa para renderização das imagens
* **Ferramentas:** Binário fornecido para suporte à comunicação serial

---

## ⚙️ Estrutura do Repositório

* `src/` — Código-fonte principal implementado na placa.
* `bin/` — Binários de suporte e rotinas fornecidas para a comunicação serial.
* `README.md` — Documentação do projeto.

---

## 💡 Sobre o Desenvolvimento

Este projeto exigiu bastante atenção com a sincronização de dados via porta serial, gerenciamento de buffer e o mapeamento correto da taxa de transmissão para garantir que as imagens fossem transferidas sem perda de pacotes e renderizadas de forma fluida no display.

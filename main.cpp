#include <iostream>
#include <cstdint>
#include <windows.h>

// IMAGENS DO ALBUM
// Para adicionar uma imagem:
// 1 - gerar o .h da imagem (vetor de 504 bytes) e colocar na pasta do projeto
// 2 - colocar o #include do arquivo aqui embaixo
// 3 - colocar o nome do vetor dentro de album[]

#include "fotobulba.h"
#include "fotocharmander.h"
#include "fotosquirtle.h"

const uint8_t* album[] = {
    fotobulba,
    fotocharmander,
    fotosquirtle
};

int totalImagens = sizeof(album) / sizeof(album[0]);

// tempo que cada imagem fica na tela (em milissegundos)
#define TEMPO_IMAGEM 3000

// COMANDOS - envia bits especificos para se comunicar com a porta serial
#define SNC 0xD3 // Solicita envio da imagem
#define ATX 0xD8 // Ativar/exibir uma imagem
#define CLR 0xC3 // Comando para limpar a tela

#define ACK 0x41 // Confirmar que o comando foi executado
#define NAK 0x4E // Informar que o comando e invalido
#define MND 0x4D // Avisar que pode enviar os dados da imagem
#define NID 0x49 // Informar que o indice e invalido
#define TOUT 0x44 // Indicar timeout na comunicacao


HANDLE portaSerial;

using namespace std;


bool abrirPortaSerial(){

    portaSerial = CreateFile(
        "\\\\.\\COM10",
        GENERIC_READ | GENERIC_WRITE,
        0,
        NULL,
        OPEN_EXISTING,
        0,
        NULL
    );


    if (portaSerial == INVALID_HANDLE_VALUE) {
        cout << "Erro ao abrir a porta serial" << endl;
        return false;
    }

    else {
        cout << "Porta serial aberta" << endl;
        return true;
    }
}


void configuracoesComunicacaoSerial(){

    DCB configuracao = {};

    configuracao.DCBlength = sizeof(DCB);

    if(!GetCommState(portaSerial, &configuracao)) {
        cout << "Erro ao obter configuracao serial" << endl;
        return;
    }

    configuracao.BaudRate = CBR_115200;
    configuracao.ByteSize = 8;
    configuracao.Parity = EVENPARITY;
    configuracao.StopBits = ONESTOPBIT;
    configuracao.fParity = TRUE;

    if(!SetCommState(portaSerial, &configuracao)) {
        cout << "Erro ao configurar comunicacao serial" << endl;
        return;
    }

    cout << "Comunicacao serial configurada" << endl;
}


void configurarTimeouts(){

    COMMTIMEOUTS timeouts = {};


    timeouts.ReadIntervalTimeout = 50;


    timeouts.ReadTotalTimeoutConstant = 500;


    timeouts.ReadTotalTimeoutMultiplier = 10;


    timeouts.WriteTotalTimeoutConstant = 500;
    timeouts.WriteTotalTimeoutMultiplier = 10;

    if (!SetCommTimeouts(portaSerial, &timeouts)) {
        cout << "Erro ao configurar timeouts!" << endl;
        return;
    }

    cout << "Timeouts configurados" << endl;
}


bool receberResposta(uint8_t& resposta){

    DWORD bytesLidos = 0;

    if (!ReadFile(portaSerial, &resposta, 1, &bytesLidos, NULL)) {
        return false;
    }

    return bytesLidos == 1;
}


bool enviarBytes(const uint8_t dados[], DWORD tamanho){
    DWORD bytesEnviados = 0;

    BOOL sucesso = WriteFile(
        portaSerial,
        dados,
        tamanho,
        &bytesEnviados,
        NULL
    );

    if (!sucesso || bytesEnviados != tamanho) {
        cout << "Erro ao enviar dados!" << endl;
        return false;
    }

    return true;
}


bool enviarImagem(const uint8_t imagem[], int indice){

    unsigned char CID = 0x80 | indice;

    uint8_t comando[2] = {SNC, CID};

    if (!enviarBytes(comando, 2)) {
        return false;
    }

    uint8_t resposta;

    if (!receberResposta(resposta)) {
        cout << "Erro ao receber resposta!" << endl;
        return false;
    }

    if (resposta != MND) {
        cout << "Placa nao autorizou o envio da imagem!" << endl;
        return false;
    }

    if (!enviarBytes(imagem, 504)) {
        return false;
    }

    if (!receberResposta(resposta)) {
        cout << "Erro ao receber confirmacao!" << endl;
        return false;
    }

    if (resposta == ACK) {
        cout << "Imagem enviada com sucesso!" << endl;
        return true;
    }

    cout << "Erro na transferencia da imagem!" << endl;
    return false;
}


bool exibirImagem(int indice){

    unsigned char CID = 0x80 | indice;

    uint8_t comando[2] = {ATX, CID};

    if (!enviarBytes(comando, 2)) {
        return false;
    }

    uint8_t resposta;

    if (!receberResposta(resposta)) {
        cout << "Erro ao receber resposta!" << endl;
        return false;
    }

    if (resposta == ACK) {
        cout << "Imagem exibida com sucesso!" << endl;
        return true;
    }
    else {
        cout << "Erro ao exibir imagem!" << endl;
        return false;
    }
}


bool enviarAlbum(){

    for (int i = 0; i < totalImagens; i++) {
        cout << "Enviando imagem " << i << "..." << endl;

        if (!enviarImagem(album[i], i)) {
            return false;
        }
    }

    return true;
}


bool slideshow(){

    for (int i = 0; i < totalImagens; i++) {

        if (!exibirImagem(i)) {
            return false;
        }

        Sleep(TEMPO_IMAGEM);
    }

    return true;
}

int main()
{
    setlocale(LC_ALL, "portuguese");

    if (totalImagens > 32) {
        cout << "O album deve ter no maximo 32 imagens" << endl;
        return 1;
    }

    if (!abrirPortaSerial()) {
        return 1;
    }

    configuracoesComunicacaoSerial();
    configurarTimeouts();


    if (enviarAlbum()) {


        while (slideshow()) {}
    }

    CloseHandle(portaSerial);

    return 0;
}

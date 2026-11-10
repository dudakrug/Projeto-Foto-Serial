#include <iostream>
#include <cstdint>
#include <windows.h>

// COMANDOS - envia bits especificos para se comunicar com a porta serial
#define SNC 0xD3 // Solicita envio da imagem
#define ATX 0xD8 // Ativar/exibir uma imagem
#define CLR 0xC3 // Comando para limpar a tela

#define ACK 0x41 // Confirmar que o comando foi executado
#define NAK 0x4E // Informar que o comando � inv�lido
#define MND 0x4D // Avisar que pode enviar os dados da imagem
#define NID 0x49 // Informar que o �ndice � inv�lido
#define TOUT 0x44 // Indicar timeout na comunica��o

// Identificador da portal serial - usado nas chamadas de leitura e escrita
HANDLE portaSerial;

using namespace std;

// configurar e abrir a comunica��o
bool abrirPortaSerial(){

    portaSerial = CreateFile(
        "\\\\.\\COM5",           // Nome da porta
        GENERIC_READ | GENERIC_WRITE,
        0,                       // Acesso exclusivo
        NULL,
        OPEN_EXISTING,
        0,
        NULL
    );

    //verificacao se a porta abriu
    if (portaSerial == INVALID_HANDLE_VALUE) {
        cout << "Erro ao abrir a porta serial" << endl;
        return false;
    }

    else {
        cout << "Porta serial aberta" << endl;
        return true;
    }
}

// configura as principais informacoes da Comunicacao Serial definidas pelo Zucco
void configuracoesComunicacaoSerial(){

    DCB configuracao = {}; // criacao da estruta que armazena as informacoes

    configuracao.DCBlength = sizeof(DCB); // informa ao windows o tamanho da estrutura

    if(!GetCommState(portaSerial, &configuracao)) { // informa o estado da porta antes de alterar as configuracoes
        cout << "Erro ao obter configuracao serial" << endl;
        return;
    }

    // requisitos definidos pelo Zucco
    configuracao.BaudRate = CBR_115200;
    configuracao.ByteSize = 8;
    configuracao.Parity = EVENPARITY;
    configuracao.StopBits = ONESTOPBIT;
    configuracao.fParity = TRUE;

    if(!SetCommState(portaSerial, &configuracao)) { // verifica se aplicou as configuracoes acima
        cout << "Erro ao configurar comunicacao serial" << endl;
        return;
    }

    cout << "Comunicacao serial configurada" << endl;
}

// definir quanto tempo o computador espera ao enviar ou receber dados pela porta serial
void configurarTimeouts(){

    COMMTIMEOUTS timeouts = {};

    // Tempo maximo entre leituras de bytes
    timeouts.ReadIntervalTimeout = 50; // intervalo entre bits e bytes

    // Tempo constante para concluir uma leitura
    timeouts.ReadTotalTimeoutConstant = 500; // limite de tempo de espera

    // Tempo adicional por byte solicitado
    timeouts.ReadTotalTimeoutMultiplier = 10; // tempo adicional por byte solicitado

    // Timeouts de escrita
    timeouts.WriteTotalTimeoutConstant = 500; // limite de tempo de escrita
    timeouts.WriteTotalTimeoutMultiplier = 10; // tempo adicional por byte enviado

    if (!SetCommTimeouts(portaSerial, &timeouts)) { // verifica se configurou
        cout << "Erro ao configurar timeouts!" << endl;
        return;
    }

    cout << "Timeouts configurados" << endl;
}

int menuUsuario(){

    int opcao;
    cout << "Selecione uma op��o" << endl;
    cout << "1 - Enviar imagem" << endl;
    cout << "2 - Exibir imagem" << endl;
    cout << "3 - Limpar o display" << endl;
    cout << "4 - Sair" << endl;
    cin >> opcao;

    if(opcao < 1 || opcao > 4){
        cout << "Digite uma resposta v�lida." << endl;
        return 0;
    }
    else{
        return opcao;
    }
}

// ler e interpretar o byte de resposta da placa
bool receberResposta(uint8_t& resposta){ // & permite a funcao trocar o valor da vari�vel

    DWORD bytesLidos = 0;

    if (!ReadFile(portaSerial, &resposta, 1, &bytesLidos, NULL)) {
        return false;
    }

    return bytesLidos == 1;
}

// ela recebe um vetor de bytes e a quantidade que deve transmitir - reutilizavel
bool enviarBytes(const uint8_t dados[], DWORD tamanho){
    DWORD bytesEnviados = 0;

    BOOL sucesso = WriteFile(
        portaSerial, // porta serial
        dados, // bytes que serao enviados
        tamanho, // quantidade de bytes que serao enviados
        &bytesEnviados, // quantidade de bytes que foram enviados
        NULL // sem escrita sobreposta
    );

    if (!sucesso || bytesEnviados != 2) {
        cout << "Erro ao enviar comando!" << endl;
    }
    else {
        cout << "Comando enviado!" << endl;
    }
}

// transmitir o comando, o �ndice e os 504 bytes
bool enviarImagem();

// solicitar a exibi��o de uma imagem armazenada
bool exibirImagem();

// enviar o comando de limpeza
bool limparDisplay(){
    uint8_t comando[2] = {CLR, CLR}; // o comando precisa ser enviado duas vezes para ser aceito

    enviarBytes(comando, 2); // enviando dois bytes do comando de limpeza

    uint8_t resposta; // byte recebido da placa

    if (!receberResposta(resposta)) {
        cout << "Erro ao receber resposta da placa!" << endl;
        return false;
    }

    if(resposta == ACK) {
        cout << "Display limpo!" << endl;
        return true;
    }
    else {
        cout << "Erro ao limpar o display" << resposta << endl;
        return false;
    }
}

int main()
{
    setlocale(LC_ALL, "portuguese");

    unsigned char imagem[504]; // 6 x 84 = 504
	unsigned char indice = 0;// cabe ate 32 imagens
	unsigned char CID = 0x80 | indice; // endereco da comunicacao muda conforme o indice de imagem
	uint8_t resposta; // Byte recebido da placa

    // FAZER VERIFICACAO DA ABERTURA DA PORTA ANTES DE ABRIR AS OUTRAS FUNCOES
	if (!abrirPortaSerial()) {
		return 1;
	}

	configuracoesComunicacaoSerial();
	configurarTimeouts();

    //int opcao = menuUsuario();

    CloseHandle(portaSerial); // fechar a porta serial

    return 0;
}

#include <windows.h>
#include <string>

// --- FUNÇÃO PARA O PROGRAMA INICIAR COM O WINDOWS ---
void ConfigurarAutoRun() {
    HKEY hKey;
    // O caminho no "cartório" do Windows onde ficam os programas que iniciam sozinhos
    const wchar_t* registroCaminho = L"Software\\Microsoft\\Windows\\CurrentVersion\\Run";

    // Pega o local exato onde o seu arquivo .exe está agora
    wchar_t caminhoEXE[MAX_PATH];
    GetModuleFileNameW(NULL, caminhoEXE, MAX_PATH);

    // Abre a chave de registro do usuário atual
    if (RegOpenKeyExW(HKEY_CURRENT_USER, registroCaminho, 0, KEY_SET_VALUE, &hKey) == ERROR_SUCCESS) {
        // Cria o valor "SentinelaBateria" apontando para o seu .exe
        RegSetValueExW(hKey, L"SentinelaBateria", 0, REG_SZ, (BYTE*)caminhoEXE, (wcslen(caminhoEXE) + 1) * sizeof(wchar_t));
        RegCloseKey(hKey);
    }
}

int main() {
    // 1. ESCONDE A JANELA (Roda como um "fantasma")
    HWND hWnd = GetConsoleWindow();
    ShowWindow(hWnd, SW_HIDE);

    // 2. CONFIGURA PARA RODAR SEMPRE QUE LIGAR O PC
    ConfigurarAutoRun();

    SYSTEM_POWER_STATUS status;
    bool jaAvisou = false;

    // 3. LOOP DE VIGILÂNCIA (O Sentinela nunca dorme)
    while (true) {
        // Pede o status da energia pro Windows
        if (GetSystemPowerStatus(&status)) {

            int carga = (int)status.BatteryLifePercent; // Porcentagem
            int naTomada = (int)status.ACLineStatus;    // 1 = No cabo, 0 = Fora do cabo

            // LÓGICA PARA BATERIA VICIADA/MORTA
            // Se a carga estiver baixa (ou der erro 255) e o cabo sair...
            if ((carga < 20 || carga == 255) && naTomada == 0) {
                if (!jaAvisou) {
                    // Toca o som de alerta do Windows
                    MessageBeep(MB_ICONHAND);

                    // Abre a mensagem de pânico na frente de tudo
                    MessageBox(NULL,
                        L"⚠️ CABO DESCONECTADO! ⚠️\n\nSeu PC vai desligar a qualquer segundo!\nConecte o carregador AGORA!",
                        L"SENTINELA DA BATERIA",
                        MB_ICONHAND | MB_OK | MB_SYSTEMMODAL);

                    jaAvisou = true;
                }
            }
            // Se o cabo voltar, ele "arma" o alerta de novo
            else if (naTomada == 1) {
                jaAvisou = false;
            }
        }

        // Checa a cada meio segundo (0.5s) para ser o mais rápido possível
        Sleep(500);
    }

    return 0;
}
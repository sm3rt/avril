// Copyright 2011 Olivier Gillet.
//
// Author: Olivier Gillet (ol.gillet@gmail.com)
// 
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
// -----------------------------------------------------------------------------
//
// FatFS wrappers.

#include "avrlib/fs/file.h"

namespace avrlib {

File::File() 
  : opened_(0) {

}

File::~File() {
  if (opened_) {
    Close();
  }
}

FileSystemStatus File::Open(
    const char* file_name,
    const char* mode,
    uint16_t retry_timeout) {
  if (mode[0] == 'r') {
    return Open(file_name, FA_READ | FA_OPEN_EXISTING, retry_timeout);
  } else if (mode[0] == 'w') {
    return Open(file_name, FA_WRITE | FA_CREATE_ALWAYS, retry_timeout);
  } else {
    return FS_INVALID_PARAMETER;
  }
}

FileSystemStatus File::Open(
    const char* file_name,
    uint8_t attributes,
    uint16_t retry_timeout) {
  if (opened_) {
    Close();
  }
  
  FileSystemStatus s;
  s = static_cast<FileSystemStatus>(f_open(&f_, file_name, attributes));
  if (s == FS_DISK_ERROR && retry_timeout) {
    // If an open fails because of a disk access error, try to reinitialize the
    // disk access layer. This might happen because a process in the background
    // has temporarily disabled the disk access layer (for example to access
    // another device). It is OK to have the disk access layer disabled between
    // file access "sessions" -- but it is not OK to have it disabled during a
    // session.
    FileSystem::Init(retry_timeout);
    s = static_cast<FileSystemStatus>(f_open(&f_, file_name, attributes));
  }

  opened_ = s == FS_OK;
  return s;
}

FileSystemStatus File::Seek(uint32_t position) {
  if (!opened_) {
    return FS_NOT_OPENED;
  }
  
  return static_cast<FileSystemStatus>(f_lseek(&f_, position));
}

FileSystemStatus File::Close() {
  return static_cast<FileSystemStatus>(f_close(&f_));
}

FileSystemStatus File::Truncate() {
  if (!opened_) {
    return FS_NOT_OPENED;
  }
  
  return static_cast<FileSystemStatus>(f_truncate(&f_));
}

FileSystemStatus File::Sync() {
  if (!opened_) {
    return FS_NOT_OPENED;
  }
  
  return static_cast<FileSystemStatus>(f_sync(&f_));
}

FileSystemStatus File::Read(uint8_t* data, uint16_t size, uint16_t* read) {
  if (!opened_) {
    return FS_NOT_OPENED;
  }
  
  return static_cast<FileSystemStatus>(f_read(&f_, data, size, read));
}

FileSystemStatus File::Write(
    const uint8_t* data,
    uint16_t size,
    uint16_t* written) {
  if (!opened_) {
    return FS_NOT_OPENED;
  }

  return static_cast<FileSystemStatus>(f_write(&f_, data, size, written));
}

}  // namespace avrlib

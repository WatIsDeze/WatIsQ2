#include <RmlUi/Core.h>

#include "../shared/shared.h"
#include "../common/common.h"

#include "FileInterface.h"

/*
==================
RmlUi_FileInterface::RmlUi_FileInterface

Constructor.
==================
*/
RmlUi_FileInterface::RmlUi_FileInterface() {

}

/*
==================
RmlUi_FileInterface::RmlUi_FileInterface

Destructor.
==================
*/
RmlUi_FileInterface::~RmlUi_FileInterface() {
    
}

/*
==================
RmlUi_FileInterface::Open

Opens a file.
==================
*/
Rml::FileHandle RmlUi_FileInterface::Open(const Rml::String& path) {
    return 0;
}

/*
==================
RmlUi_FileInterface::Close

Closes a previously opened file.
==================
*/
void RmlUi_FileInterface::Close(Rml::FileHandle file) {
    
}

/*
==================
RmlUi_FileInterface::Read

Reads data from a previously opened file.
==================
*/
size_t RmlUi_FileInterface::Read(void* buffer, size_t size, Rml::FileHandle file) {
    return 0;
}

/*
==================
RmlUi_FileInterface::Seek

Seeks to a point in a previously opened file.
==================
*/
bool RmlUi_FileInterface::Seek(Rml::FileHandle file, long offset, int origin) {
    return false;
}

/*
==================
RmlUi_FileInterface::Tell

Returns the current position of the file pointer.
==================
*/
size_t RmlUi_FileInterface::Tell(Rml::FileHandle file) {
    return 0;
}
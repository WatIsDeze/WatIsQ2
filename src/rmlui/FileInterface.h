#ifndef _RMLUI_FILEINTERFACE_H_
#define _RMLUI_FILEINTERFACE_H_

//==================
//  RmlUi_FileInterface
//  
//  Implements a file interface class for RmlUi.
//  Nothing fancy.
//==================
class RmlUi_FileInterface : public Rml::FileInterface {
public:
    //==================
    // Constructor/Destructor.
    //==================
    RmlUi_FileInterface();
    ~RmlUi_FileInterface();

    //==================
    // Implementation functions.
    //==================
    // File.
    // Opens a file.
    Rml::FileHandle Open(const Rml::String& path);

    // Closes a previously opened file.
    void Close(Rml::FileHandle file);

    // Reads data from a previously opened file.
    size_t Read(void* buffer, size_t size, Rml::FileHandle file);

    // Seeks to a point in a previously opened file.
    bool Seek(Rml::FileHandle file, long offset, int origin);

    // Returns the current position of the file pointer.
    size_t Tell(Rml::FileHandle file);

private:

};

#endif
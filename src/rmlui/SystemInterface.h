#ifndef _RMLUI_SYSTEMINTERFACE_H_
#define _RMLUI_SYSTEMINTERFACE_H_

//==================
//  RmlUi_SystemInterface
//  
//  Implements a system interface class for RmlUi.
//  Nothing fancy.
//==================
class RmlUi_SystemInterface : public Rml::SystemInterface {
public:
    //==================
    // Constructor/Destructor.
    //==================
    RmlUi_SystemInterface();
    ~RmlUi_SystemInterface();

    //==================
    // Implementation functions.
    //==================
    // Logging.
    bool LogMessage(Rml::Log::Type type, const Rml::String& message);

    // Time.
    double GetElapsedTime();
    // Yet to implement:
    // virtual void SetMouseCursor(const Rml::String& cursor_name);

private:

};

#endif
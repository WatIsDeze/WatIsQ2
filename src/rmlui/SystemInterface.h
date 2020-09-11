#ifndef _RMLUI_SYSTEMINTERFACE_H_
#define _RMLUI_SYSTEMINTERFACE_H_

class RmlUi_SystemInterface : public Rml::SystemInterface {
public:
    // Constructor/Destructor.
    RmlUi_SystemInterface();
    ~RmlUi_SystemInterface();

    //
    // Implementation functions.
    //
    double GetElapsedTime();
    
    //
    virtual bool LogMessage(Rml::Log::Type type, const Rml::String& message);
    // Yet to implement:
    // virtual void SetMouseCursor(const Rml::String& cursor_name);

private:

};

#endif
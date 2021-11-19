// cmdjoinery_solver_rhino7.cpp : command file
//

#include "StdAfx.h"
#include "joinery_solver_rhino7PlugIn.h"
#include "compas.h"
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
//
// BEGIN joinery_solver_rhino7 command
//

#pragma region joinery_solver_rhino7 command

// Do NOT put the definition of class CCommandjoinery_solver_rhino7 in a header
// file. There is only ONE instance of a CCommandjoinery_solver_rhino7 class
// and that instance is the static thejoinery_solver_rhino7Command that appears
// immediately below the class definition.

class CCommandjoinery_solver_rhino7 : public CRhinoCommand
{
public:
  // The one and only instance of CCommandjoinery_solver_rhino7 is created below.
  // No copy constructor or operator= is required.
  // Values of member variables persist for the duration of the application.

  // CCommandjoinery_solver_rhino7::CCommandjoinery_solver_rhino7()
  // is called exactly once when static thejoinery_solver_rhino7Command is created.
  CCommandjoinery_solver_rhino7() = default;

  // CCommandjoinery_solver_rhino7::~CCommandjoinery_solver_rhino7()
  // is called exactly once when static thejoinery_solver_rhino7Command is destroyed.
  // The destructor should not make any calls to the Rhino SDK. 
  // If your command has persistent settings, then override 
  // CRhinoCommand::SaveProfile and CRhinoCommand::LoadProfile.
  ~CCommandjoinery_solver_rhino7() = default;

  // Returns a unique UUID for this command.
  // If you try to use an id that is already being used, then
  // your command will not work. Use GUIDGEN.EXE to make unique UUID.
  UUID CommandUUID() override
  {
    // {3118197D-9A40-498D-8419-73D871F06851}
    static const GUID joinery_solver_rhino7Command_UUID =
    { 0x3118197D, 0x9A40, 0x498D, { 0x84, 0x19, 0x73, 0xD8, 0x71, 0xF0, 0x68, 0x51 } };
    return joinery_solver_rhino7Command_UUID;
  }

  // Returns the English command name.
  // If you want to provide a localized command name, then override 
  // CRhinoCommand::LocalCommandName.
  const wchar_t* EnglishCommandName() override { return L"joinery_solver_rhino7"; }

  // Rhino calls RunCommand to run the command.
  CRhinoCommand::result RunCommand(const CRhinoCommandContext& context) override;
};

// The one and only CCommandjoinery_solver_rhino7 object
// Do NOT create any other instance of a CCommandjoinery_solver_rhino7 class.
static class CCommandjoinery_solver_rhino7 thejoinery_solver_rhino7Command;

CRhinoCommand::result CCommandjoinery_solver_rhino7::RunCommand(const CRhinoCommandContext& context)
{
  // CCommandjoinery_solver_rhino7::RunCommand() is called when the user
  // runs the "joinery_solver_rhino7".

  // TODO: Add command code here.

  // Rhino command that display a dialog box interface should also support
  // a command-line, or script-able interface.

  ON_wString str;
  str.Format(L"The \"%s\" command is under construction.\n", EnglishCommandName());
  const wchar_t* pszStr = static_cast<const wchar_t*>(str);
  if (context.IsInteractive())
    RhinoMessageBox(pszStr, joinery_solver_rhino7PlugIn().PlugInName(), MB_OK);
  else
    RhinoApp().Print(pszStr);

  // TODO: Return one of the following values:
  //   CRhinoCommand::success:  The command worked.
  //   CRhinoCommand::failure:  The command failed because of invalid input, inability
  //                            to compute the desired result, or some other reason
  //                            computation reason.
  //   CRhinoCommand::cancel:   The user interactively canceled the command 
  //                            (by pressing ESCAPE, clicking a CANCEL button, etc.)
  //                            in a Get operation, dialog, time consuming computation, etc.

  return CRhinoCommand::success;
}

#pragma endregion

//
// END joinery_solver_rhino7 command
//
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////

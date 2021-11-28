// cmdjoinery_solver_rhino6.cpp : command file
//

#include "StdAfx.h"
#include "joinery_solver_rhino6PlugIn.h"

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
//
// BEGIN joinery_solver_rhino6 command
//

#pragma region joinery_solver_rhino6 command

// Do NOT put the definition of class CCommandjoinery_solver_rhino6 in a header
// file. There is only ONE instance of a CCommandjoinery_solver_rhino6 class
// and that instance is the static thejoinery_solver_rhino6Command that appears
// immediately below the class definition.

class CCommandjoinery_solver_rhino6 : public CRhinoCommand
{
public:
  // The one and only instance of CCommandjoinery_solver_rhino6 is created below.
  // No copy constructor or operator= is required.
  // Values of member variables persist for the duration of the application.

  // CCommandjoinery_solver_rhino6::CCommandjoinery_solver_rhino6()
  // is called exactly once when static thejoinery_solver_rhino6Command is created.
  CCommandjoinery_solver_rhino6() = default;

  // CCommandjoinery_solver_rhino6::~CCommandjoinery_solver_rhino6()
  // is called exactly once when static thejoinery_solver_rhino6Command is destroyed.
  // The destructor should not make any calls to the Rhino SDK. 
  // If your command has persistent settings, then override 
  // CRhinoCommand::SaveProfile and CRhinoCommand::LoadProfile.
  ~CCommandjoinery_solver_rhino6() = default;

  // Returns a unique UUID for this command.
  // If you try to use an id that is already being used, then
  // your command will not work. Use GUIDGEN.EXE to make unique UUID.
  UUID CommandUUID() override
  {
    // {9E39CB07-5A57-4AD4-AFF7-D951CECF025A}
    static const GUID joinery_solver_rhino6Command_UUID =
    { 0x9E39CB07, 0x5A57, 0x4AD4, { 0xAF, 0xF7, 0xD9, 0x51, 0xCE, 0xCF, 0x2, 0x5A } };
    return joinery_solver_rhino6Command_UUID;
  }

  // Returns the English command name.
  // If you want to provide a localized command name, then override 
  // CRhinoCommand::LocalCommandName.
  const wchar_t* EnglishCommandName() override { return L"joinery_solver_rhino6"; }

  // Rhino calls RunCommand to run the command.
  CRhinoCommand::result RunCommand(const CRhinoCommandContext& context) override;
};

// The one and only CCommandjoinery_solver_rhino6 object
// Do NOT create any other instance of a CCommandjoinery_solver_rhino6 class.
static class CCommandjoinery_solver_rhino6 thejoinery_solver_rhino6Command;

CRhinoCommand::result CCommandjoinery_solver_rhino6::RunCommand(const CRhinoCommandContext& context)
{
  // CCommandjoinery_solver_rhino6::RunCommand() is called when the user
  // runs the "joinery_solver_rhino6".

  // TODO: Add command code here.

  // Rhino command that display a dialog box interface should also support
  // a command-line, or scriptable interface.

  ON_wString str;
  str.Format(L"The \"%s\" command is under construction.\n", EnglishCommandName());
  const wchar_t* pszStr = static_cast<const wchar_t*>(str);
  if (context.IsInteractive())
    RhinoMessageBox(pszStr, joinery_solver_rhino6PlugIn().PlugInName(), MB_OK);
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
// END joinery_solver_rhino6 command
//
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////

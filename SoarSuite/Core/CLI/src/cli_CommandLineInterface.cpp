/////////////////////////////////////////////////////////////////
// CommandLineInterface class file.
//
// Author: Jonathan Voigt, voigtjr@gmail.com
// Date  : 2004
//
/////////////////////////////////////////////////////////////////

#include <portability.h>

#include "sml_Utils.h"
#include "cli_CommandLineInterface.h"

#include <assert.h>

#include <iostream>
#include <fstream>

#include "cli_Commands.h"

// SML includes
#include "sml_Connection.h"
#include "sml_TagResult.h"
#include "sml_TagArg.h"
#include "sml_StringOps.h"
#include "sml_KernelSML.h"
#include "sml_AgentSML.h"
#include "XMLTrace.h"
#include "KernelHeaders.h"

#include "agent.h"
#include "xml.h"

using namespace cli;
using namespace sml;
using namespace soarxml;

EXPORT CommandLineInterface::CommandLineInterface() {

	// Map command names to processing function pointers
	m_CommandMap[Commands::kCLIAddWME]						= &cli::CommandLineInterface::ParseAddWME;
	m_CommandMap[Commands::kCLIAlias]						= &cli::CommandLineInterface::ParseAlias;
	m_CommandMap[Commands::kCLIAllocate]					= &cli::CommandLineInterface::ParseAllocate;
	m_CommandMap[Commands::kCLICaptureInput]				= &cli::CommandLineInterface::ParseCaptureInput;
	m_CommandMap[Commands::kCLICD]							= &cli::CommandLineInterface::ParseCD;
	m_CommandMap[Commands::kCLIChunkNameFormat]				= &cli::CommandLineInterface::ParseChunkNameFormat;
	m_CommandMap[Commands::kCLICLog]						= &cli::CommandLineInterface::ParseCLog;
	m_CommandMap[Commands::kCLICommandToFile]				= &cli::CommandLineInterface::ParseCommandToFile;
	m_CommandMap[Commands::kCLIDefaultWMEDepth]				= &cli::CommandLineInterface::ParseDefaultWMEDepth;
	m_CommandMap[Commands::kCLIDirs]						= &cli::CommandLineInterface::ParseDirs;
	m_CommandMap[Commands::kCLIEcho]						= &cli::CommandLineInterface::ParseEcho;
	m_CommandMap[Commands::kCLIEchoCommands]				= &cli::CommandLineInterface::ParseEchoCommands;
	m_CommandMap[Commands::kCLIEditProduction]				= &cli::CommandLineInterface::ParseEditProduction;
	m_CommandMap[Commands::kCLIEpMem]						= &cli::CommandLineInterface::ParseEpMem;
	m_CommandMap[Commands::kCLIExcise]						= &cli::CommandLineInterface::ParseExcise;
	m_CommandMap[Commands::kCLIExplainBacktraces]			= &cli::CommandLineInterface::ParseExplainBacktraces;
	m_CommandMap[Commands::kCLIFiringCounts]				= &cli::CommandLineInterface::ParseFiringCounts;
	m_CommandMap[Commands::kCLIGDSPrint]					= &cli::CommandLineInterface::ParseGDSPrint;
	m_CommandMap[Commands::kCLIGP]							= &cli::CommandLineInterface::ParseGP;
	m_CommandMap[Commands::kCLIGPMax]						= &cli::CommandLineInterface::ParseGPMax;
	m_CommandMap[Commands::kCLIHelp]						= &cli::CommandLineInterface::ParseHelp;
	m_CommandMap[Commands::kCLIIndifferentSelection]		= &cli::CommandLineInterface::ParseIndifferentSelection;
	m_CommandMap[Commands::kCLIInitSoar]					= &cli::CommandLineInterface::ParseInitSoar;
	m_CommandMap[Commands::kCLIInternalSymbols]				= &cli::CommandLineInterface::ParseInternalSymbols;
	m_CommandMap[Commands::kCLILearn]						= &cli::CommandLineInterface::ParseLearn;
	m_CommandMap[Commands::kCLILoadLibrary]					= &cli::CommandLineInterface::ParseLoadLibrary;
	m_CommandMap[Commands::kCLILS]							= &cli::CommandLineInterface::ParseLS;
	m_CommandMap[Commands::kCLIMatches]						= &cli::CommandLineInterface::ParseMatches;
	m_CommandMap[Commands::kCLIMaxChunks]					= &cli::CommandLineInterface::ParseMaxChunks;
	m_CommandMap[Commands::kCLIMaxElaborations]				= &cli::CommandLineInterface::ParseMaxElaborations;
	m_CommandMap[Commands::kCLIMaxGoalDepth]				= &cli::CommandLineInterface::ParseMaxGoalDepth;
	m_CommandMap[Commands::kCLIMaxMemoryUsage]				= &cli::CommandLineInterface::ParseMaxMemoryUsage;
	m_CommandMap[Commands::kCLIMaxNilOutputCycles]			= &cli::CommandLineInterface::ParseMaxNilOutputCycles;
	m_CommandMap[Commands::kCLIMemories]					= &cli::CommandLineInterface::ParseMemories;
	m_CommandMap[Commands::kCLIMultiAttributes]				= &cli::CommandLineInterface::ParseMultiAttributes;
	m_CommandMap[Commands::kCLINumericIndifferentMode]		= &cli::CommandLineInterface::ParseNumericIndifferentMode;
	m_CommandMap[Commands::kCLIOSupportMode]				= &cli::CommandLineInterface::ParseOSupportMode;
	m_CommandMap[Commands::kCLIPopD]						= &cli::CommandLineInterface::ParsePopD;
	m_CommandMap[Commands::kCLIPort]						= &cli::CommandLineInterface::ParsePort;
	m_CommandMap[Commands::kCLIPredict]						= &cli::CommandLineInterface::ParsePredict;
	m_CommandMap[Commands::kCLIPreferences]					= &cli::CommandLineInterface::ParsePreferences;
	m_CommandMap[Commands::kCLIPrint]						= &cli::CommandLineInterface::ParsePrint;
	m_CommandMap[Commands::kCLIProductionFind]				= &cli::CommandLineInterface::ParseProductionFind;
	m_CommandMap[Commands::kCLIPushD]						= &cli::CommandLineInterface::ParsePushD;
	m_CommandMap[Commands::kCLIPWatch]						= &cli::CommandLineInterface::ParsePWatch;
	m_CommandMap[Commands::kCLIPWD]							= &cli::CommandLineInterface::ParsePWD;
	m_CommandMap[Commands::kCLIRand]						= &cli::CommandLineInterface::ParseRand;
	m_CommandMap[Commands::kCLIRemoveWME]					= &cli::CommandLineInterface::ParseRemoveWME;
	m_CommandMap[Commands::kCLIReplayInput]					= &cli::CommandLineInterface::ParseReplayInput;
	m_CommandMap[Commands::kCLIReteNet]						= &cli::CommandLineInterface::ParseReteNet;
	m_CommandMap[Commands::kCLIRL]							= &cli::CommandLineInterface::ParseRL;
	m_CommandMap[Commands::kCLIRun]							= &cli::CommandLineInterface::ParseRun;
	m_CommandMap[Commands::kCLISaveBacktraces]				= &cli::CommandLineInterface::ParseSaveBacktraces;
	m_CommandMap[Commands::kCLISelect]						= &cli::CommandLineInterface::ParseSelect;
	m_CommandMap[Commands::kCLISetLibraryLocation]			= &cli::CommandLineInterface::ParseSetLibraryLocation;
	m_CommandMap[Commands::kCLISMem]						= &cli::CommandLineInterface::ParseSMem;
	m_CommandMap[Commands::kCLISoarNews]					= &cli::CommandLineInterface::ParseSoarNews;
	m_CommandMap[Commands::kCLISource]						= &cli::CommandLineInterface::ParseSource;
	m_CommandMap[Commands::kCLISP]							= &cli::CommandLineInterface::ParseSP;
	m_CommandMap[Commands::kCLISRand]						= &cli::CommandLineInterface::ParseSRand;
	m_CommandMap[Commands::kCLIStats]						= &cli::CommandLineInterface::ParseStats;
	m_CommandMap[Commands::kCLISetStopPhase]				= &cli::CommandLineInterface::ParseSetStopPhase;
	m_CommandMap[Commands::kCLIStopSoar]					= &cli::CommandLineInterface::ParseStopSoar;
	m_CommandMap[Commands::kCLITime]						= &cli::CommandLineInterface::ParseTime;
	m_CommandMap[Commands::kCLITimers]						= &cli::CommandLineInterface::ParseTimers;
	m_CommandMap[Commands::kCLIUnalias]						= &cli::CommandLineInterface::ParseUnalias;
	m_CommandMap[Commands::kCLIVerbose]						= &cli::CommandLineInterface::ParseVerbose;
	m_CommandMap[Commands::kCLIVersion]						= &cli::CommandLineInterface::ParseVersion;
	m_CommandMap[Commands::kCLIWaitSNC]						= &cli::CommandLineInterface::ParseWaitSNC;
	m_CommandMap[Commands::kCLIWarnings]					= &cli::CommandLineInterface::ParseWarnings;
	m_CommandMap[Commands::kCLIWatch]						= &cli::CommandLineInterface::ParseWatch;
	m_CommandMap[Commands::kCLIWatchWMEs]					= &cli::CommandLineInterface::ParseWatchWMEs;
	m_CommandMap[Commands::kCLIWMA]							= &cli::CommandLineInterface::ParseWMA;

	// Indicate which commands should be echoed so that all users can see them when doing a shared debugging session
	// FIXME: missing stuff like GDSPRINT?
	m_EchoMap[Commands::kCLIAddWME]						= true ;
	m_EchoMap[Commands::kCLIAlias]						= true ;
	m_EchoMap[Commands::kCLIAllocate]					= true ;
	m_EchoMap[Commands::kCLICaptureInput]				= true ;
	m_EchoMap[Commands::kCLICD]							= true ;
	m_EchoMap[Commands::kCLIChunkNameFormat]			= true ;
	m_EchoMap[Commands::kCLICLog]						= true ;
	m_EchoMap[Commands::kCLICommandToFile]				= true ;
	m_EchoMap[Commands::kCLIDefaultWMEDepth]			= true ;
	m_EchoMap[Commands::kCLIEcho]						= true ;
	m_EchoMap[Commands::kCLIEchoCommands]				= true ;
	m_EchoMap[Commands::kCLIEpMem]						= true ;
	m_EchoMap[Commands::kCLIExcise]						= true ;
	m_EchoMap[Commands::kCLIGP]							= true ;
	m_EchoMap[Commands::kCLIGPMax]						= true ;
	m_EchoMap[Commands::kCLIIndifferentSelection]		= true ;
	m_EchoMap[Commands::kCLIInitSoar]					= true ;
	m_EchoMap[Commands::kCLILearn]						= true ;
	m_EchoMap[Commands::kCLILoadLibrary]				= true ; // TODO: figure out if we actually want to echo this
	m_EchoMap[Commands::kCLIMaxChunks]					= true ;
	m_EchoMap[Commands::kCLIMaxElaborations]			= true ;
	m_EchoMap[Commands::kCLIMaxNilOutputCycles]			= true ;
	m_EchoMap[Commands::kCLIMultiAttributes]			= true ;
	m_EchoMap[Commands::kCLINumericIndifferentMode]		= true ;
	m_EchoMap[Commands::kCLIOSupportMode]				= true ;
	m_EchoMap[Commands::kCLIPopD]						= true ;
	m_EchoMap[Commands::kCLIPort]						= true ;
	m_EchoMap[Commands::kCLIPreferences]				= true ;
	m_EchoMap[Commands::kCLIPushD]						= true ;
	m_EchoMap[Commands::kCLIRand]						= true ;
	m_EchoMap[Commands::kCLIRemoveWME]					= true ;
	m_EchoMap[Commands::kCLIReplayInput]				= true ;
	m_EchoMap[Commands::kCLIReteNet]					= true ;
	m_EchoMap[Commands::kCLIRL]							= true ;
	m_EchoMap[Commands::kCLIRun]						= true ;
	m_EchoMap[Commands::kCLISelect]						= true ;
	m_EchoMap[Commands::kCLISetLibraryLocation]			= true ;
	m_EchoMap[Commands::kCLISMem]						= true ;
	m_EchoMap[Commands::kCLISource]						= true ;
	m_EchoMap[Commands::kCLISP]							= true ;
	m_EchoMap[Commands::kCLISRand]						= true ;
	m_EchoMap[Commands::kCLISetStopPhase]				= true ;
	m_EchoMap[Commands::kCLIStopSoar]					= true ;
	m_EchoMap[Commands::kCLITimers]						= true ;
	m_EchoMap[Commands::kCLIVerbose]					= true ;
	m_EchoMap[Commands::kCLIWaitSNC]					= true ;
	m_EchoMap[Commands::kCLIWatch]						= true ;
	m_EchoMap[Commands::kCLIWatchWMEs]					= true ;

	// Initialize other members
	m_pLogFile = 0;
	m_LastError = kNoError;
	m_Initialized = true;
	m_TrapPrintEvents = false;
	m_EchoResult = false ;
	m_pAgentSML = 0 ;
	m_pAgentSoar = 0;
	m_VarPrint = false;
	m_GPMax = 20000;

	m_XMLResult = new XMLTrace() ;
}

EXPORT CommandLineInterface::~CommandLineInterface() {
	if (m_pLogFile) {
		(*m_pLogFile) << "Log file closed due to shutdown." << std::endl;
		delete m_pLogFile;
	}

	delete m_XMLResult ;
	m_XMLResult = NULL ;
}

/*************************************************************
* @brief Returns true if the given command should always be echoed (to any listeners)
*        The current implementation doesn't support aliases or short forms of the commands.
* @param pCommandLine	The command line being tested
*************************************************************/
EXPORT bool CommandLineInterface::ShouldEchoCommand(char const* pCommandLine)
{
	if (!pCommandLine)
		return false ;

	std::string command = pCommandLine ;

	char const* pSpace = strchr(pCommandLine, ' ') ;
	if (pSpace)
	{
		// Trim everything from space on
		command.erase(pSpace-pCommandLine, command.length()) ;
	}

	// See if there's an entry in the echo map for this command
	// BADBAD: This won't work for short forms of the command or aliases; but making this test
	// happen later in the command line processing causes too many re-entrancy problem within the command line module.
	return (m_EchoMap.find(command) != m_EchoMap.end()) ;
}

/*************************************************************
* @brief Process a command.  Give it a command line and it will parse
*		 and execute the command using system calls.
* @param pConnection The connection, for communication to the client
* @param pAgent The pointer to the agent interface
* @param pCommandLine The command line string, arguments separated by spaces
* @param echoResults If true send a copy of the result to the echo event
* @param pResponse Pointer to XML response object
*************************************************************/
EXPORT bool CommandLineInterface::DoCommand(Connection* pConnection, sml::AgentSML* pAgent, const char* pCommandLine, bool echoResults, bool rawOutput, ElementXML* pResponse) 
{
	if (!m_pKernelSML) return false;

	PushCall( CallData(pAgent, rawOutput) );

	// Log input
	if (m_pLogFile) 
    {
		if (pAgent) (*m_pLogFile) << pAgent->GetName() << "> ";
		(*m_pLogFile) << pCommandLine << std::endl;
	}

	m_EchoResult = echoResults ;

	SetTrapPrintCallbacks( true );

    m_LastError = kNoError;
    m_LastErrorDetail.clear();

	// Process the command
    soar::tokenizer tokenizer;
    tokenizer.set_handler(this);
    bool ret = tokenizer.evaluate(pCommandLine);

	SetTrapPrintCallbacks( false );

    if (!ret && m_LastError == kNoError)
    {
        SetError(kParseError);
        SetErrorDetail(tokenizer.get_error_string());
    }

	if (pConnection && pResponse)
		GetLastResultSML(pConnection, pResponse);

	PopCall();

	// Always returns true to indicate that we've generated any needed error message already
	return true;
}

void CommandLineInterface::PushCall( CallData callData )
{
	m_CallDataStack.push( callData );

	if (callData.pAgent) 
	{
		m_pAgentSML = callData.pAgent;
		m_pAgentSoar = m_pAgentSML->GetSoarAgent();
		assert( m_pAgentSoar );
	} else {
		m_pAgentSML = 0;
		m_pAgentSoar = 0;
	}

	m_RawOutput = callData.rawOutput;

	// For kernel callback class we inherit
	SetAgentSML(m_pAgentSML) ;
}

void CommandLineInterface::PopCall()
{
	m_CallDataStack.pop();
	sml::AgentSML* pAgent = 0;
	
	if ( m_CallDataStack.size() )
	{
		const CallData& callData = m_CallDataStack.top();
		pAgent = callData.pAgent;
		m_RawOutput = callData.rawOutput;

		// reset these for the next command
		SetAgentSML( pAgent ) ;

		m_pAgentSML = pAgent;
		if (pAgent) 
		{
			m_pAgentSoar = pAgent->GetSoarAgent();
			assert( m_pAgentSoar );
		}
		else 
		{
			m_pAgentSoar = 0;
		}
	}
}

void CommandLineInterface::SetTrapPrintCallbacks(bool setting)
{
	if (!m_pAgentSML)
		return;

	// If we've already set it, don't re-set it
	if ( m_TrapPrintEvents == setting )
		return;

	if (setting)
	{
		// Trap print callbacks
		m_pAgentSML->DisablePrintCallback();
		m_TrapPrintEvents = true;
		if (!m_pLogFile) 
			// If we're logging, we're already registered for this.
			RegisterWithKernel(smlEVENT_PRINT);

		// Tell kernel to collect result in command buffer as opposed to trace buffer
		xml_begin_command_mode( m_pAgentSoar );
	}
	else
	{
		// Retrieve command buffer, tell kernel to use trace buffer again
		ElementXML* pXMLCommandResult = xml_end_command_mode( m_pAgentSoar );

		// The root object is just a <trace> tag.  The substance is in the children
		// Add childrend of the command buffer to response tags
		for ( int i = 0; i < pXMLCommandResult->GetNumberChildren(); ++i )
		{
			ElementXML* pChildXML = new ElementXML();
			pXMLCommandResult->GetChild( pChildXML, i );

			m_ResponseTags.push_back( pChildXML );
		}

		delete pXMLCommandResult;

		if ( !m_RawOutput )
		{
			// Add text result to response tags
			if ( m_Result.str().length() )
			{
				AppendArgTagFast( sml_Names::kParamMessage, sml_Names::kTypeString, m_Result.str() );
				m_Result.str("");
			}
		}

		// Re-enable print callbacks
		if (!m_pLogFile) 
		{
			// If we're logging, we want to stay registered for this
			UnregisterWithKernel(smlEVENT_PRINT);
		}
		m_TrapPrintEvents = false;
		m_pAgentSML->EnablePrintCallback();
	}
}

void CommandLineInterface::GetLastResultSML(sml::Connection* pConnection, soarxml::ElementXML* pResponse) {
	assert(pConnection);
	assert(pResponse);

	if (m_LastError == kNoError) {
        // Log output
        if (m_pLogFile) (*m_pLogFile) << m_Result.str() << std::endl;

        // The command succeeded, so return the result if raw output
		if (m_RawOutput) {
			pConnection->AddSimpleResultToSMLResponse(pResponse, m_Result.str().c_str());
			EchoString(pConnection, m_Result.str().c_str()) ;
		} else {
			// If there are tags in the response list, add them and return
			if (m_ResponseTags.size()) {
				TagResult* pTag = new TagResult();

				ElementXMLListIter iter = m_ResponseTags.begin();
				while (iter != m_ResponseTags.end()) {
					pTag->AddChild(*iter);
					m_ResponseTags.erase(iter);
					iter = m_ResponseTags.begin();
				}

				pResponse->AddChild(pTag);

			} else {
				// Or, simply return true
				pConnection->AddSimpleResultToSMLResponse(pResponse, sml_Names::kTrue);
			}
		}
	} else {
		// The command failed, add the error message
		std::string errorDescription = GenerateErrorString();

		pConnection->AddErrorToSMLResponse(pResponse, errorDescription.c_str(), m_LastError);
		EchoString(pConnection, errorDescription.c_str()) ;

        // Log error
        if (m_pLogFile) (*m_pLogFile) << errorDescription << std::endl;
	}

	// reset state
	m_Result.str("");

	// Delete all remaining xml objects
	for ( ElementXMLListIter cleanupIter = m_ResponseTags.begin(); cleanupIter != m_ResponseTags.end(); ++cleanupIter )
	{
		delete *cleanupIter;
	}
	m_ResponseTags.clear();	

	m_LastError = kNoError;	
	m_LastErrorDetail.clear();			
}

std::string CommandLineInterface::GenerateErrorString()
{
	std::string errorDescription(GetErrorDescription(m_LastError));
	if (m_LastErrorDetail.size()) {
		errorDescription += "\nError detail: ";
		errorDescription += m_LastErrorDetail;
	}
	if (m_Result.str().size()) {
		errorDescription += "\nResult before error happened:\n";
		errorDescription += m_Result.str();
	}
	return errorDescription;
}

const char* CommandLineInterface::GetErrorDescription(CLIError code) 
{
    switch (code) {
        case kNoError:                             return "No Error.";
        case kGetOptError:                         return "GetOpt returned with an error.";
        case kCommandNotImplemented:               return "Command not implemented.";
        case kProductionNotFound:                  return "Production not found.";
        case kNotImplemented:                      return "Not implemented.";
        case kTooManyArgs:                         return "Too many arguments for the specified (or unspecified) options, check syntax.";
        case kTooFewArgs:                          return "Too few arguments for the specified (or unspecified) options, check syntax.";
        case kUnrecognizedOption:                  return "Unrecognized option.";
        case kMissingOptionArg:                    return "Missing option argument.";
        case kgetcwdFail:                          return "Error getting current working directory.";
        case kgettimeofdayFail:                    return "gettimeofday() failed.";
        case kchdirFail:                           return "Error changing to directory.";
        case kAliasNotFound:                       return "Alias not found.";
        case kIntegerExpected:                     return "Integer argument expected.";
        case kIntegerMustBePositive:               return "Integer argument must be positive.";
        case kIntegerMustBeNonNegative:            return "Integer argument must be non-negative.";
        case kIntegerOutOfRange:                   return "Integer argument out of range.";
        case kInvalidOperation:                    return "Invalid operation.";
        case kInvalidNumericIndifferentMode:       return "Invalid numeric indifferent mode.";
        case kInvalidIndifferentSelectionMode:     return "Invalid indifferent selection mode.";
        case kNoProdTypeWhenProdName:              return "Do not specify production type when specifying a production name.";
        case kSourceOnlyOneFile:                   return "Too many arguments: source only one file at a time.  If there are spaces in the path, check that you are using quotes.";
        case kLogAlreadyOpen:                      return "Log already open.";
        case kLogOpenFailure:                      return "Failed to open file for logging.";
        case kLogNotOpen:                          return "Log is not open.";
        case kDirectoryOpenFailure:                return "Unable to open directory for reading.";
        case kDirectoryEntryReadFailure:           return "Unable to read directory entry for reading.";
        case kDirectoryStackEmpty:                 return "Directory stack empty, no directory to change to.";
        case kMissingFilenameArg:                  return "Missing filename argument.";
        case kOpenFileFail:                        return "Failed to open file for reading.";
        case kReteSaveOperationFail:               return "Rete save operation failed.";
        case kReteLoadOperationFail:               return "Rete load operation failed.";
        case kInvalidLearnSetting:                 return "Invalid learn setting, expected noprint, print, fullprint, or 0-2.";
        case kRemoveOrZeroExpected:                return "Invalid argument, expected remove or 0.";
        case kInvalidID:                           return "Unknown or invalid ID.";
        case kInvalidAttribute:                    return "Unknown or invalid attribute.";
        case kInvalidValue:                        return "Unknown or invalid value.";
        case kInvalidWMEFilterType:                return "Invalid WME filter type, expected 'adds' 'removes' or 'both'.";
        case kFilterExpected:                      return "ID/Attribute/Value filter expected, one or more missing.";
        case kDuplicateWMEFilter:                  return "That WME filter already exists.";
        case kInvalidMode:                         return "Invalid mode.";
        case kTypeRequired:                        return "A type (-t adds/removes/both) is required for this command.";
        case kWMEFilterNotFound:                   return "The specified WME filter was not found.";
        case kProductionRequired:                  return "A production is required.";
        case kInvalidConditionNumber:              return "Condition number must be a non-negative integer.";
        case kInvalidPrefix:                       return "Failed to set prefix (does it contain a '*'?).";
        case kCountGreaterThanMaxChunks:           return "Cannot set count greater than the max-chunks sysparam.";
        case kCountLessThanChunks:                 return "Cannot set chunk count less than the current number of chunks.";
        case kAcceptableOrNothingExpected:         return "Expected acceptable preference (+) or nothing, check syntax.";
        case kMustSaveOrLoad:                      return "Must save or load, check command syntax.";
        case kPrintSubOptionsOfStack:              return "Options --operators (-o) and --states (-S) are only valid when printing the stack.";
        case kRunFailed:                           return "Run failed.";
        case kAmbiguousCommand:                    return "Received command is ambiguous, try adding more letters.";
        case kAmbiguousOption:                     return "Ambiguous option.";
        case kInitSoarFailed:                      return "Agent could not be reinitialized.  Probably due to an internal memory leak." ;
        case kPreferencesError:                    return "Preferences command failed." ;
        case kInvalidRunInterleaveSetting:         return "Invalid setting for run interleave option." ;
        case kLoadLibraryError:                    return "Error occurred while loading library";
        case kProductionAddFailed:                 return "Production addition failed.";
        case kSourceDepthExceeded:                 return "Source depth (100) exceeded, possible recursive source.";
        case kCloseFileFail:                       return "File close failed.";
        case kFileOpen:                            return "File already open.";
        case kFileNotOpen:                         return "File is not open.";
        case kRealExpected:                        return "Real number expected.";
        case kValuesError:                         return "gp values error.";
        case kGPMaxExceeded:                       return "gp maximum exceeded. Set it to higher value with gp-max command.";
        case kParseError:                          return "Parse error.";
        case kSMemError:                           return "smem command error.";
        case kWmaError:                            return "wma command error.";
        case kRlError:                             return "rl command error.";
        case kEpMemError:                          return "epmem command error.";
    }
    return "Unknown error code.";
}

/************************************************************* 	 
* @brief Echo the given std::string through the smlEVENT_ECHO event
*		 if the call requested that commands be echoed.
*************************************************************/ 	 
void CommandLineInterface::EchoString(sml::Connection* pConnection, char const* pString)
{
	if (!m_EchoResult)
		return ;

	// BUGBUG: We may need to support this for kernel level commands without an agent
	if (m_pAgentSML)
		m_pAgentSML->FireEchoEvent(pConnection, pString) ;
}

bool CommandLineInterface::PartialMatch(std::vector<std::string>& argv) {

	// Not an alias, check for partial match
	std::list<std::string> possibilities;
	std::list<std::string>::iterator liter;
	bool exactMatch = false;

	for(unsigned index = 0; index < (argv[0].size()); ++index) {

		if (index == 0) {
			// Bootstrap the list of possibilities
			CommandMapConstIter citer = m_CommandMap.begin();

			while (citer != m_CommandMap.end()) {
				if (citer->first[index] == argv[0][index]) {
					possibilities.push_back(citer->first);
				}
				++citer;
			}

		} else {
			// Update the list of possiblities

			// A more efficient search here would be nice.
			liter = possibilities.begin();
			while (liter != possibilities.end()) {
				if ((*liter)[index] != argv[0][index]) {
					// Remove this possibility from the list
					liter = possibilities.erase(liter);
				} else {
					// check for exact match
					if (argv[0] == (*liter)) {
						// Exact match, we're done
						argv[0] = (*liter);
						exactMatch = true;
						break;
					}
					++liter;
				}
			}
			if (exactMatch) break;
		}

		if (!possibilities.size()) {
			// Not implemented
			SetErrorDetail("(No such command: " + argv[0] + ")");
			return SetError(kCommandNotImplemented);

		} 
	}

	if (!exactMatch) {
		if (possibilities.size() != 1) {
			// Ambiguous
			std::stringstream detail;
			detail << "Ambiguous command, possibilities: ";
			liter = possibilities.begin();
			while (liter != possibilities.end()) {
				detail << "'" << (*liter) << "' ";
				++liter;
			}
			SetErrorDetail(detail.str());
			return SetError(kAmbiguousCommand);

		} else {
			// We have a partial match
			argv[0] = (*(possibilities.begin()));
		}
		return true;
	}
	return true;
}

bool CommandLineInterface::handle_command(std::vector<std::string>& argv) {
    if (argv.empty()) return true;

	// Check for help flags
	bool helpFlag = false;
	if (CheckForHelp(argv)) {
		helpFlag = true;
	}

	// Expand aliases
	if (m_Aliases.Translate(argv)) {
		// Is the alias target implemented?
		if (m_CommandMap.find(argv[0]) == m_CommandMap.end()) {
			SetErrorDetail("No such command: " + argv[0]);
			return SetError(kCommandNotImplemented);
		}

	} else {
		if (!PartialMatch(argv)) 
		{
			// error set inside PartialMatch
			return false;
		}
	}

	// Show help if requested
	if (helpFlag) {
		return DoHelp();
	}

	// Process command
	CommandFunction pFunction = m_CommandMap[argv[0]];
	assert(pFunction);
	
	// Initialize option parsing each call
	ResetOptions();

	// Make the Parse call
	return (this->*pFunction)(argv);
}

bool CommandLineInterface::CheckForHelp(std::vector<std::string>& argv) {

	// Standard help check if there is more than one argument
	if (argv.size() > 1) {
		// Is one of the two help strings present?
		if (argv[1] == "-h" || argv[1] == "--help") {
			return true;
		}
	}
	return false;
}

EXPORT void CommandLineInterface::SetKernel(sml::KernelSML* pKernelSML) {
	m_pKernelSML = pKernelSML;
}

bool CommandLineInterface::GetCurrentWorkingDirectory(std::string& directory) {
	// Pull an arbitrary buffer size of 1024 out of a hat and use it
	char buf[1024];
	char* ret = getcwd(buf, 1024);

	// If getcwd returns 0, that is bad
	if (!ret) return SetError(kgetcwdFail);

	// Store directory in output parameter and return success
	directory = buf;
    normalize_separators(directory);
	return true;
}

void CommandLineInterface::AppendArgTag(const char* pParam, const char* pType, const std::string& value) {
	AppendArgTag(pParam, pType, value.c_str());
}

void CommandLineInterface::AppendArgTagFast(const char* pParam, const char* pType, const std::string& value) {
	AppendArgTagFast(pParam, pType, value.c_str());
}

void CommandLineInterface::PrependArgTag(const char* pParam, const char* pType, const std::string& value) {
	PrependArgTag(pParam, pType, value.c_str());
}

void CommandLineInterface::PrependArgTagFast(const char* pParam, const char* pType, const std::string& value) {
	PrependArgTagFast(pParam, pType, value.c_str());
}

void CommandLineInterface::AppendArgTag(const char* pParam, const char* pType, const char* pValue) {
	TagArg* pTag = new TagArg();
	pTag->SetParam(pParam);
	pTag->SetType(pType);
	pTag->SetValue(pValue);
	m_ResponseTags.push_back(pTag);
}

void CommandLineInterface::AppendArgTagFast(const char* pParam, const char* pType, const char* pValue) {
	TagArg* pTag = new TagArg();
	pTag->SetParamFast(pParam);
	pTag->SetTypeFast(pType);
	pTag->SetValue(pValue);
	m_ResponseTags.push_back(pTag);
}

void CommandLineInterface::PrependArgTag(const char* pParam, const char* pType, const char* pValue) {
	TagArg* pTag = new TagArg();
	pTag->SetParam(pParam);
	pTag->SetType(pType);
	pTag->SetValue(pValue);
	m_ResponseTags.push_front(pTag);
}

void CommandLineInterface::PrependArgTagFast(const char* pParam, const char* pType, const char* pValue) {
	TagArg* pTag = new TagArg();
	pTag->SetParamFast(pParam);
	pTag->SetTypeFast(pType);
	pTag->SetValue(pValue);
	m_ResponseTags.push_front(pTag);
}

bool CommandLineInterface::SetError(cli::CLIError code) {
	m_LastError = code;
	return false;
}
bool CommandLineInterface::SetErrorDetail(const std::string detail) {
	m_LastErrorDetail = detail;
	return false;
}

void CommandLineInterface::ResetOptions() {
	m_Argument = 0;
	m_NonOptionArguments = 0;	
}

bool CommandLineInterface::ProcessOptions(std::vector<std::string>& argv, Options* options) {

	// default to indifference
	m_Option = 0;
	m_OptionArgument = "";

	// increment current argument and check bounds
	while (static_cast<unsigned>(++m_Argument) < argv.size()) {

		// args less than 2 characters cannot mean anything to us
		if (argv[m_Argument].size() < 2) {
			++m_NonOptionArguments;
			continue;
		}

		if (argv[m_Argument][0] == '-') {
			if (argv[m_Argument][1] == '-') {
				// possible long m_Option
				if (argv[m_Argument].size() > 2) {
					// long m_Option, save it
					std::string longOption = argv[m_Argument].substr(2);

					// check for partial match
					std::list<Options> possibilities;
					std::list<Options>::iterator liter;
					std::set< std::string > addedLongOptions;

					for(unsigned index = 0; index < longOption.size(); ++index) {

						if (index == 0) {
							// Bootstrap the list of possibilities
							for (int i = 0; options[i].shortOpt != 0; ++i) {
								if (options[i].longOpt[index] == longOption[index]) {
									// don't add duplicates (bug 976)
									if ( addedLongOptions.insert( options[i].longOpt ).second )
									{
										possibilities.push_back(options[i]);
									}
								}
							}

						} else {
							// Update the list of possiblities

							// A more efficient search here would be nice.
							liter = possibilities.begin();
							while (liter != possibilities.end()) {
								if ((*liter).longOpt[index] != longOption[index]) {
									// Remove this possibility from the list
									liter = possibilities.erase(liter);
								} else {
									// check for exact match
									if (longOption == (*liter).longOpt) {
										// exact match, we're done
										m_Option = liter->shortOpt;
										MoveBack(argv, m_Argument, m_NonOptionArguments);
										if (!HandleOptionArgument(argv, liter->longOpt, liter->argument)) return false;
										return true;
									}
									++liter;
								}
							}
						}

						if (!possibilities.size()) {
							SetErrorDetail("No such option: " + longOption);
							return SetError(kUnrecognizedOption);
						} 
					}

					if (possibilities.size() != 1) {
						// Ambiguous
						std::stringstream detail;
						detail << "Ambiguous option, possibilities: ";
						liter = possibilities.begin();
						while (liter != possibilities.end()) {
							detail << "'--" << (*liter).longOpt << "' ";
							++liter;
						}
						SetErrorDetail(detail.str());
						return SetError(kAmbiguousOption);

					}
					// We have a partial match
					m_Option = (*(possibilities.begin())).shortOpt;
					MoveBack(argv, m_Argument, m_NonOptionArguments);
					if (!HandleOptionArgument(argv, (*(possibilities.begin())).longOpt, (*(possibilities.begin())).argument)) return false;
					return true;
				}

				// end of options special flag '--'
				// FIXME: remove -- argument?
				m_Option = -1; // done
				return true; // no error
			}

			// short m_Option(s)
			for (int i = 0; options[i].shortOpt != 0; ++i) {
				if (argv[m_Argument][1] == options[i].shortOpt) {
					if (argv[m_Argument].size() > 2) {
						std::vector<std::string>::iterator target = argv.begin();
						target += m_Argument;

						std::string original = *target;
						*target = (*target).substr(0,2);
						++target;

						argv.insert(target, "-" + original.substr(2));
					}
					m_Option = options[i].shortOpt;
					MoveBack(argv, m_Argument, m_NonOptionArguments);
					if (!HandleOptionArgument(argv, options[i].longOpt, options[i].argument)) return false;
					return true;
				}
			}
			char theOption = argv.at( m_Argument ).at( 1 );
			SetErrorDetail( std::string("No such option: ") + theOption );
			return SetError(kUnrecognizedOption);
		}
		++m_NonOptionArguments;
	}

	// out of arguments
	m_Option = -1;	// done
	return true;	// no error
}

void CommandLineInterface::MoveBack(std::vector<std::string>& argv, int what, int howFar) {

	assert(what >= howFar);
	assert(what > 0);
	assert(howFar >= 0);

	if (howFar == 0) {
		return;
	}

	std::vector<std::string>::iterator target = argv.begin();
	target += what;

	std::vector<std::string>::iterator dest = target - howFar;

	argv.insert(dest, *target);

	target = argv.begin();
	target += what + 1;

	argv.erase(target);
}

bool CommandLineInterface::HandleOptionArgument(std::vector<std::string>& argv, const char* option, eOptionArgument arg) {
	switch (arg) {
		case OPTARG_NONE:
			break;
		case OPTARG_REQUIRED:
			// required argument
			if (static_cast<unsigned>(++m_Argument) >= argv.size()) {
				std::string detail(option);
				SetErrorDetail("Option '" + detail + "' requires an argument.");
				return SetError(kMissingOptionArg);
			}
			m_OptionArgument = argv[m_Argument];
			MoveBack(argv, m_Argument, m_NonOptionArguments);
			break;
		case OPTARG_OPTIONAL:
		default:
			// optional argument
			if (static_cast<unsigned>(++m_Argument) < argv.size()) {
				if (argv[m_Argument].size()) {
					if (argv[m_Argument][0] != '-') {
						m_OptionArgument = argv[m_Argument];
						MoveBack(argv, m_Argument, m_NonOptionArguments);
					} 
				}
			}
			if (!m_OptionArgument.size()) {
				--m_Argument;
			}
			break;
	}
	return true;
}

bool CommandLineInterface::CheckNumNonOptArgs(int min, int max)
{
    if ( m_NonOptionArguments < min )
        return SetError( kTooFewArgs );
    else if ( m_NonOptionArguments > max )
        return SetError( kTooManyArgs );
    return true;
}


/*************************************************************
* @brief Methods to create an XML element by starting a tag, adding attributes and
*		 closing the tag.
*		 These tags are automatically collected into the result of the current command.
*	
* NOTE: The attribute names must be compile time constants -- i.e. they remain in scope
*		at all times (so we don't have to copy them).
*************************************************************/
void CommandLineInterface::XMLBeginTag(char const* pTagName)
{
	m_XMLResult->BeginTag(pTagName) ;
}

void CommandLineInterface::XMLAddAttribute(char const* pAttribute, char const* pValue)
{
	m_XMLResult->AddAttribute(pAttribute, pValue) ;
}

void CommandLineInterface::XMLEndTag(char const* pTagName)
{
	m_XMLResult->EndTag(pTagName) ;
}

/*************************************************************
* @brief	Occassionally it's helpful to be able to back up
*			in the XML and add some extra elements.
*
*			These calls should only be used once a tag has been completed,
*			so the sequence is something like:
*			beginTag() ;
*			...
*			endTag() ;
*			moveToLastChild() ;	// Goes back to previous tag
*			add an extra attribute() ;
*			moveToParent() ;	// Go back to parent
*			... continue on
*************************************************************/
bool CommandLineInterface::XMLMoveCurrentToParent()
{
	return m_XMLResult->MoveCurrentToParent() ;
}

bool CommandLineInterface::XMLMoveCurrentToChild(int index)
{
	return m_XMLResult->MoveCurrentToChild(index) ;
}

bool CommandLineInterface::XMLMoveCurrentToLastChild()
{
	return m_XMLResult->MoveCurrentToLastChild() ;
}

// The copies over the m_XMLResult object to the response XML object and sets the
// tag name to the command that was just executed.
// The result is XML in this format (e.g. for matches):
// <result><matches>...</matches></result>
// where ... contains the XML specific to that command.
void CommandLineInterface::XMLResultToResponse(char const* pCommandName)
{
	// Extract the XML object from the xmlTrace object and
	// add it as a child of this message.  This is just moving a few pointers around, nothing is getting copied.
	ElementXML_Handle xmlHandle = m_XMLResult->Detach() ;
	ElementXML* pXMLResult = new ElementXML(xmlHandle) ;
	pXMLResult->SetTagName(pCommandName) ;

	m_ResponseTags.push_back(pXMLResult) ;

	// Clear the XML result, so it's ready for use again.
	m_XMLResult->Reset() ;
}

void CommandLineInterface::OnKernelEvent(int eventID, AgentSML*, void* pCallData)
{
	if (eventID == smlEVENT_PRINT)
	{
		char const* msg = static_cast<char const*>(pCallData);

		if (m_TrapPrintEvents || m_pLogFile)
		{
			if (m_VarPrint)
			{
				// Transform if varprint, see print command
				std::string message(msg);

				regex_t comp;
				regcomp(&comp, "[A-Z][0-9]+", REG_EXTENDED);

				regmatch_t match;
				memset(&match, 0, sizeof(regmatch_t));

				while (regexec(&comp, message.substr(match.rm_eo, message.size() - match.rm_eo).c_str(), 1, &match, 0) == 0) {
					message.insert(match.rm_so, "<");
					message.insert(match.rm_eo + 1, ">");
					match.rm_eo += 2;
				}  

				regfree(&comp);

				// Simply append to message result
				if (m_TrapPrintEvents) {
					CommandLineInterface::m_Result << message;
					//std::cout << msg;
					//std::cout.flush();
				} else if (m_pLogFile) {
					(*m_pLogFile) << msg;
				}
			} else {
				if (m_TrapPrintEvents) {
					CommandLineInterface::m_Result << msg;
					//std::cout << msg;
					//std::cout.flush();
				} else if (m_pLogFile) {
					(*m_pLogFile) << msg;
				}
			}
		}
	}
    else if (eventID == smlEVENT_BEFORE_PRODUCTION_REMOVED)
    {
		// Only called when source command is active
		production* p = static_cast<production*>(pCallData);
		assert(p);
		assert(p->name->sc.name);
		m_ExcisedDuringSource.push_back(std::string(p->name->sc.name));
    }
	else
	{
		assert(false);
		// unknown event
		// TODO: gracefully (?) deal with this error
	}
} // function


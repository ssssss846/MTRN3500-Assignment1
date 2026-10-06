#include "EmbeddedFunctions.h"

// Constructor 
EmbeddedFunctions::EmbeddedFunctions() 
{
	client = nullptr; 
	stream = nullptr; 
}


// Destructor 
EmbeddedFunctions::~EmbeddedFunctions() 
{
	GClose();
}

/**
* Open a connection to a Galil Controller.
*
* @param address Null-terminated address string. Use direct connection (-d) to connect to hardware or simulator (e.g., "192.168.0.120 -d").
* @param port Integer value for the port to connect to.
*
* @throws error if one occurs.
*/
void EmbeddedFunctions::GOpen(String^ address, const int port)
{
	if (String::IsNullOrWhiteSpace(address)) {
		throw gcnew ArgumentException("IP Address Invalid"); 
	}

	if (port <= 0 || port > 65535) {
		throw gcnew ArgumentOutOfRangeException("port");
	}

	String^ host = address->Trim(); 

	int spacePosition = host->IndexOf(' '); 

	if (spacePosition >= 0) {
		host = host->Substring(0, spacePosition); 
	}

	GClose(); 

	try {
		client = gcnew TcpClient();

		client->SendBufferSize = 64; 
		client->ReceiveBufferSize = 2048; 
		client->SendTimeout = 500; 
		client->ReceiveTimeout = 500; 
		client->NoDelay = true; 

		client->Connect(host, port);
		stream = client->GetStream();
	}
	catch (Exception^ error) {
		GClose(); 

		throw gcnew Exception("Galil connection failed", error); 
	}
}

/**
* Closes a connection to a Galil Controller.
 `GClose()` should be called whenever a program is finished with a controller. This includes when a program closes. A rule of thumb is that for every `GOpen()` call on a given connection, a `GClose()` call should be found on every code path. Failing to call GClose() may cause controller resources to not be released or can hang the process if there are outstanding asynchronous operations. The latter can occur, for example, if a call to GRead() times out and the process exits without calling GClose(). In this case, GRead() still has an outstanding asynchronous read pending. GClose() will terminate this operation allowing the process to exit correctly.
*
*
* @throws error if one occurs.
*/
void EmbeddedFunctions::GClose()
{
	if (stream != nullptr) {
		stream->Close(); 
		stream = nullptr;
	}

	if (client != nullptr) {
		client->Close(); 
		client = nullptr; 
	}
}

/**
* Performs a *command-and-response* transaction on the connection.
* IMPORTANT: Commands being sent to the galil should be in the form of strings based on the command reference (uploaded to Moodle). You should choose commands appropriate for a given task and avoid those that rely on logical expressions. Commands chosen should send data directly to the output or read directly from the inputs.
* IMPORTANT: If the command string supplied is not terminated by a semicolon, you should append one.
*
* @param command Null-terminated command string to send to the controller.
*
* @return The reponse from the Galil.
* @throws error if one occurs.
*/
String^ EmbeddedFunctions::GCommand(String^ command)
{ 
	if (stream == nullptr) {
		throw gcnew InvalidOperationException("Galil connection is not open"); 
	}

	if (String::IsNullOrWhiteSpace(command)) {
		throw gcnew ArgumentException("Command cannot be empty"); 
	}

	command = command->Trim(); 
	if (!command->EndsWith(";")) {
		command += ";";
	}

	command += "\r"; 

	array<Byte>^ sendData = System::Text::Encoding::ASCII->GetBytes(command);

	stream->Write(sendData, 0, sendData->Length); 

	System::Text::StringBuilder^ response = gcnew System::Text::StringBuilder(); 

	bool commandError = false; 

	while (true) {
		int received = stream->ReadByte(); 
		if (received == -1) {
			throw gcnew Exception("Connection closed"); 
		}

		char character = static_cast<char>(received); 

		if (character == '?') {
			commandError = true; 
			continue; 
		}

		if (character == ':') {
			break; 
		}

		response->Append(static_cast<wchar_t>(character)); 
	}

	if (commandError) {
		throw gcnew Exception("Galil command error");
	}

	return response->ToString()->Trim(); 
}
#include "Galil.h"
#include <sstream>
#include <iomanip>

// CONSTRUCTORS 
// Default Constructor 
Galil::Galil() :
	Functions(new EmbeddedFunctions()),
	g(nullptr),
	ControlParameters{ 0.0, 0.0, 0.0 },
	setPoint(0),
	address_("192.168.0.120 -d"),
	ownedFunctions(Functions),
	writeStatus(G_NO_ERROR)
{
	Functions->GOpen("192.168.0.120 -d", &g);
}

// Constructor with EmbeddedFunciton pre-initialised and passed in.
Galil::Galil(EmbeddedFunctions* Funcs, GCStringIn address) :
	Functions(Funcs),
	g(nullptr),
	ControlParameters{0.0, 0.0, 0.0},
	setPoint(0), 
	address_(address),
	ownedFunctions(nullptr), 
	writeStatus(G_NO_ERROR)
{
	Functions->GOpen(address, &g);
}

// Copy Constructor 
Galil::Galil(const Galil& other) :
	Functions(new EmbeddedFunctions()),
	g(nullptr),
	ControlParameters{
		other.ControlParameters[0],
		other.ControlParameters[1],
		other.ControlParameters[2]
	},
	setPoint(other.setPoint),
	address_(other.address_), 
	ownedFunctions(Functions), 
	writeStatus(other.writeStatus)
{
	Functions->GOpen(address_.c_str(), &g);
}

// Default Destructor 
Galil::~Galil()
{
	if (Functions != nullptr && g != nullptr) {
		Functions->GClose(g);
	}

	delete ownedFunctions; 
}

// DIGITAL OUTPUTS 
// Write to all 16 bits of digital output, 1 command to the Galil
void Galil::DigitalOutput(uint16_t value) 
{
	uint8_t lowerByte = value & 0xFF; 
	uint8_t upperByte = (value >> 8) & 0xFF; 

	std::string command =
		"OP " + std::to_string(lowerByte) + 
		"," + std::to_string(upperByte) + ";";

	char buffer[64] = {}; 
	GSize bytesReturned = 0; 

	writeStatus = Functions->GCommand(
		g, 
		command.c_str(),
		buffer, 
		sizeof(buffer), 
		&bytesReturned
	);
}


// Write to one byte, either high or low byte, as specified by user in 'bank'
void Galil::DigitalByteOutput(bool bank, uint8_t value) 
{
	GReturn byteStatus = G_NO_ERROR;

	for (uint8_t i = 0; i < 8; i++) {
		bool bitValue = (value >> i) & 0x01; 
		uint8_t outputBit; 
		
		if (bank == 0) {
			outputBit = i; 
		}
		else {
			outputBit = i + 8; 
		}

		DigitalBitOutput(bitValue, outputBit);

		if (writeStatus != G_NO_ERROR) {
			byteStatus = writeStatus;
		}
	}

	writeStatus = byteStatus;
}

// Write single bit to digital outputs. 'bit' specifies which bit
void Galil::DigitalBitOutput(bool val, uint8_t bit)
{
	if (bit > 15) {
		writeStatus = G_BAD_VALUE_RANGE;
		return;
	}

	std::string command;

	if (val) {
		command = "SB " + std::to_string(bit) + ";";
	}
	else {
		command = "CB " + std::to_string(bit) + ";";
	}

	char buffer[64] = {};
	GSize bytesReturned = 0;

	writeStatus = Functions->GCommand(
		g,
		command.c_str(),
		buffer,
		sizeof(buffer),
		&bytesReturned
	);
}

// DIGITAL INPUTS
// Return the 16 bits of input data
uint16_t Galil::DigitalInput() 
{
	uint16_t inputVal = 0; 

	for (uint8_t i = 0; i < 16; i++) {
		std::string command = "MG @IN[" + std::to_string(i) + "];"; 
		char buffer[64] = {}; 
		GSize bytesReturned = 0; 

		Functions->GCommand(g, command.c_str(), buffer, sizeof(buffer), &bytesReturned); 

		double bitValue = std::stod(buffer); 

		if (bitValue != 0) {
			inputVal |= (1 << i);
		}
	}

	return inputVal;
}


// Read either high or low byte, as specified by user in 'bank'
uint8_t Galil::DigitalByteInput(bool bank)
{
	uint16_t inputs = DigitalInput();

	if (bank == 0) {
		return inputs & 0xFF; 
	} else { 
		return (inputs >> 8) & 0XFF; 
	}
}



// Read single bit from current digital inputs. 
bool Galil::DigitalBitInput(uint8_t bit)					
{
	if (bit > 15) {
		return false; 
	}

	std::string command = "MG @IN[" + std::to_string(bit) + "];";

	char buffer[64] = {}; 
	GSize bytesReturned = 0; 

	Functions->GCommand(g, command.c_str(), buffer, sizeof(buffer), &bytesReturned); 

	return std::stod(buffer) != 0; 
}


// Check the string response from the Galil to check that the last command executed correctly
bool Galil::CheckSuccessfulWrite()
{
	return writeStatus == G_NO_ERROR; 
}


// ANALOG FUNCITONS
// Read Analog channel and return voltage	
float Galil::AnalogInput(uint8_t channel)
{
	if (channel > 7) {
		return 0.0f;
	}

	std::string command = "MG @AN[" + std::to_string(channel) + "];";

	char buffer[64] = {}; 
	GSize bytesReturned = 0; 

	Functions->GCommand(
		g,
		command.c_str(),
		buffer,
		sizeof(buffer),
		&bytesReturned
	); 

	return std::stof(buffer);
}


// Write to any channel of the Galil, send voltages as 2 decimal place in the command string
void Galil::AnalogOutput(uint8_t channel, double voltage)
{
	if (channel > 7 || voltage < -9.99 || voltage > 9.99) {
		writeStatus = G_BAD_VALUE_RANGE; 
		return; 
	}

	std::ostringstream voltageStream; 

	voltageStream << std::fixed << std::setprecision(2) << voltage; 

	std::string command = "AO " + std::to_string(channel) + "," + voltageStream.str() + ";";

	char buffer[64] = {};
	GSize bytesReturned = 0; 

	writeStatus = Functions->GCommand(
		g,
		command.c_str(),
		buffer,
		sizeof(buffer),
		&bytesReturned
	);
}


// Configure the range of the input channel with the desired range code
void Galil::AnalogInputRange(uint8_t channel, uint8_t range)
{
	if (channel > 7 || range < 1 || range > 4) {
		writeStatus = G_BAD_VALUE_RANGE; 
		return; 
	}

	std::string command = "AQ " + std::to_string(channel) + "," + std::to_string(range) + ";";

	char buffer[64] = {}; 
	GSize bytesReturned = 0; 

	writeStatus = Functions->GCommand(
		g,
		command.c_str(),
		buffer,
		sizeof(buffer),
		&bytesReturned
	);
}

// ENCODER
// Manually Set the motor encoder value to zero (encoder channel 0)
void Galil::WriteEncoder()
{
	std::string command = "WE 0;"; 
	char buffer[64] = {};
	GSize bytesReturned = 0;

	writeStatus = Functions->GCommand(
		g,
		command.c_str(),
		buffer,
		sizeof(buffer),
		&bytesReturned
	);
}


// Read from motor Encoder (encoder channel 0)
int Galil::ReadEncoder()
{
	std::string command = "QE 0;";

	char buffer[64] = {};
	GSize bytesReturned = 0;

	Functions->GCommand(
		g,
		command.c_str(),
		buffer,
		sizeof(buffer),
		&bytesReturned
	);

	return std::stoi(buffer);
}

// CONTROL FUNCTIONS
// Set the desired setpoint for control loops, counts or counts/sec
void Galil::setSetPoint(int s)
{
	setPoint = s; 
}


// Gets the current setpoint stored in the class
double Galil::getSetPoint()
{
	return setPoint; 
}


// Set the proportional gain of the controller used in controlLoop() of Position/SpeedControl
void Galil::setKp(double gain)
{
	ControlParameters[0] = gain; 
}


// Gets the current proportional gain stored in the class
double Galil::getKp()
{
	return ControlParameters[0];
}


// Set the integral gain of the controller used in controlLoop()  of Position/SpeedControl
void Galil::setKi(double gain)
{
	ControlParameters[1] = gain;
} 


// Gets the current integral gain stored in the class
double Galil::getKi()
{
	return ControlParameters[1];
}


// Set the derivative gain of the controller used in controlLoop()  of Position/SpeedControl
void Galil::setKd(double gain)
{
	ControlParameters[2] = gain;
}


// Gets the current derivative gain stored in the class
double Galil::getKd()
{
	return ControlParameters[2];
}

// OPERATOR OVERLOADS
// Operator overload for '<<' operator. So the user can say cout << Galil;
std::ostream& operator<<(std::ostream& output, Galil& galil) {
	char info[128] = {}; 
	char version[128] = {}; 

	galil.Functions->GInfo(galil.g, info, sizeof(info)); 
	galil.Functions->GVersion(version, sizeof(version));
	output << info << "\n\n"; 
	output << version << "\n\n";

	return output;
}



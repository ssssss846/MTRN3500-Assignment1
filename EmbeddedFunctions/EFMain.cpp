#include "EmbeddedFunctions.h"

using namespace System;
using namespace System::Threading;

int main(void)
{
    Console::WriteLine("=================================");
    Console::WriteLine("PART B - EmbeddedFunctions TEST");
    Console::WriteLine("=================================\n");

    EmbeddedFunctions funcs;

    // -------------------------------------------------
    // TEST 1: GCommand before GOpen
    // -------------------------------------------------
    Console::WriteLine("TEST 1: GCommand before GOpen");

    try
    {
        funcs.GCommand("MG @IN[0]");
        Console::WriteLine("FAIL - command was allowed without connection");
    }
    catch (Exception^ e)
    {
        Console::WriteLine("PASS - exception correctly thrown");
        Console::WriteLine("Message: " + e->Message);
    }

    Console::WriteLine();


    // -------------------------------------------------
    // TEST 2: Invalid port
    // -------------------------------------------------
    Console::WriteLine("TEST 2: Invalid port");

    try
    {
        funcs.GOpen("192.168.0.120", 0);
        Console::WriteLine("FAIL - invalid port was accepted");
    }
    catch (Exception^ e)
    {
        Console::WriteLine("PASS - invalid port rejected");
        Console::WriteLine("Message: " + e->Message);
    }

    Console::WriteLine();


    // -------------------------------------------------
    // TEST 3: Open real PLC connection
    // -------------------------------------------------
    Console::WriteLine("TEST 3: Connect to physical Galil");

    try
    {
        funcs.GOpen("192.168.0.120", 23);
        Console::WriteLine("PASS - connection opened");
    }
    catch (Exception^ e)
    {
        Console::WriteLine("FAIL - could not connect");
        Console::WriteLine(e->ToString());

        Console::WriteLine("\nCannot continue physical tests.");
        Console::ReadLine();
        return 1;
    }

    Console::WriteLine();


    // -------------------------------------------------
    // TEST 4: Read physical digital input
    // -------------------------------------------------
    Console::WriteLine("TEST 4: Read digital input 0");

    try
    {
        String^ response = funcs.GCommand("MG @IN[0]");

        Console::WriteLine("PASS - command returned");
        Console::WriteLine("Input 0 = [" + response + "]");
    }
    catch (Exception^ e)
    {
        Console::WriteLine("FAIL - read command failed");
        Console::WriteLine(e->Message);
    }

    Console::WriteLine();


    // -------------------------------------------------
    // TEST 5: Turn physical output ON
    // -------------------------------------------------
    Console::WriteLine("TEST 5: Turn output 0 ON");

    try
    {
        funcs.GCommand("SB 0");

        Console::WriteLine("Command sent successfully");
        Console::WriteLine("CHECK: Output 0 LED should now be ON.");

        Thread::Sleep(2000);

        Console::WriteLine("PASS if LED was ON for approximately 2 seconds.");
    }
    catch (Exception^ e)
    {
        Console::WriteLine("FAIL - SB command failed");
        Console::WriteLine(e->Message);
    }

    Console::WriteLine();


    // -------------------------------------------------
    // TEST 6: Turn physical output OFF
    // -------------------------------------------------
    Console::WriteLine("TEST 6: Turn output 0 OFF");

    try
    {
        funcs.GCommand("CB 0");

        Console::WriteLine("Command sent successfully");
        Console::WriteLine("CHECK: Output 0 LED should now be OFF.");
    }
    catch (Exception^ e)
    {
        Console::WriteLine("FAIL - CB command failed");
        Console::WriteLine(e->Message);
    }

    Console::WriteLine();


    // -------------------------------------------------
    // TEST 7: Invalid command
    // -------------------------------------------------
    Console::WriteLine("TEST 7: Invalid Galil command");

    try
    {
        funcs.GCommand("THIS_IS_NOT_A_REAL_COMMAND");

        Console::WriteLine("FAIL - invalid command did not throw exception");
    }
    catch (Exception^ e)
    {
        Console::WriteLine("PASS - Galil command error detected");
        Console::WriteLine("Message: " + e->Message);
    }

    Console::WriteLine();


    // -------------------------------------------------
    // TEST 8: Empty command
    // -------------------------------------------------
    Console::WriteLine("TEST 8: Empty command");

    try
    {
        funcs.GCommand("");

        Console::WriteLine("FAIL - empty command was accepted");
    }
    catch (Exception^ e)
    {
        Console::WriteLine("PASS - empty command rejected");
        Console::WriteLine("Message: " + e->Message);
    }

    Console::WriteLine();


    // -------------------------------------------------
    // TEST 9: Close connection
    // -------------------------------------------------
    Console::WriteLine("TEST 9: GClose");

    try
    {
        funcs.GClose();

        Console::WriteLine("PASS - connection closed");
    }
    catch (Exception^ e)
    {
        Console::WriteLine("FAIL - GClose caused an error");
        Console::WriteLine(e->Message);
    }

    Console::WriteLine();


    // -------------------------------------------------
    // TEST 10: Command after GClose
    // -------------------------------------------------
    Console::WriteLine("TEST 10: GCommand after GClose");

    try
    {
        funcs.GCommand("MG @IN[0]");

        Console::WriteLine("FAIL - command worked after connection closed");
    }
    catch (Exception^ e)
    {
        Console::WriteLine("PASS - command correctly rejected");
        Console::WriteLine("Message: " + e->Message);
    }

    Console::WriteLine();


    // -------------------------------------------------
    // TEST 11: Reconnect
    // -------------------------------------------------
    Console::WriteLine("TEST 11: Reconnect after GClose");

    try
    {
        funcs.GOpen("192.168.0.120", 23);

        Console::WriteLine("Connection reopened");

        String^ response = funcs.GCommand("MG @IN[0]");

        Console::WriteLine("Input 0 = [" + response + "]");

        funcs.GClose();

        Console::WriteLine("PASS - reconnect and command worked");
    }
    catch (Exception^ e)
    {
        Console::WriteLine("FAIL - reconnect failed");
        Console::WriteLine(e->ToString());
    }

    Console::WriteLine();
    Console::WriteLine("=================================");
    Console::WriteLine("TESTING COMPLETE");
    Console::WriteLine("=================================");

    Console::WriteLine("\nPress ENTER to exit");
    Console::ReadLine();

    return 0;
}
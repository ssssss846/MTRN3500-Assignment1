/******************
EmbeddedFunctions.h
ALEXANDER CUNIO 2024
******************/

/*
This class wraps the Galil commands for students to use. Whenever you send or receive from the board in your
Galil class, use the Embeddedfunctions to do it. Documentation within this file taken from Galil library headers.
For more information review the included header files separately.
This file should not be modified.

See spec for usage instructions.
*/

#pragma once

#include "gclib.h"
#include "gclibo.h"
#include "gclib_errors.h"
#include "gclib_record.h"

#include <winsock.h>

class EmbeddedFunctions {
public:
	EmbeddedFunctions();
	EmbeddedFunctions(bool use_simulator);
	~EmbeddedFunctions();

	/**
	* Open a connection to a Galil Controller.
	*
	* @param address Null-terminated address string. Use direct connection (-d) to connect to hardware or simulator (e.g., "192.168.0.120 -d").
	* @param g Pointer to user's `GCon` variable. On success, the library will fill the user's variable with the handle to use for the rest of the connection. A valid `g` value is nonzero.
	*
	* @return The success status or error code of the function. See gclib_errors.h for possible values.
	*/
	virtual GReturn GOpen(GCStringIn address, GCon* g);

	/**
	* Closes a connection to a Galil Controller.
	* *gclib* requires that `GClose()` be called whenever a program is finished with a controller. This includes when a program closes. A rule of thumb is that for every `GOpen()` call on a given connection, a `GClose()` call should be found on every code path. Failing to call GClose() may cause controller resources to not be released or can hang the process if there are outstanding asynchronous operations. The latter can occur, for example, if a call to GRead() times out and the process exits without calling GClose(). In this case, GRead() still has an outstanding asynchronous read pending. GClose() will terminate this operation allowing the process to exit correctly.
	*
	* @param g Connection's handle.
	*
	* @return The success status or error code of the function. See gclib_errors.h for possible values.
	*/
	virtual GReturn GClose(GCon g);

	/**
	* Performs a *command-and-response* transaction on the connection.
	* IMPORTANT: Commands being sent to the galil should be in the form of strings based on the command reference (uploaded to Moodle). You should choose commands appropriate for a given task and avoid those that rely on logical expressions. Commands chosen should send data directly to the output or read directly from the inputs.
	*
	* @param g Connection's handle.
	* @param command Null-terminated command string to send to the controller. The library will append a carriage return to the command string.
	* @param buffer Buffer for the response. Will be filled with the response from the controller. The data will be null terminated unless function returns `G_BAD_LOST_DATA` due to the buffer being too small to hold the data.
	* @param buffer_len The size of the response buffer.
	* @param bytes_returned The size of the data returned from the controller. This does not include null termination. This argument may be null if the value is not desired.
	*
	* @return The success status or error code of the function. See gclib_errors.h for possible values.
	*/
	virtual GReturn GCommand(GCon g, GCStringIn command, GBufOut buffer, GSize buffer_len, GSize* bytes_returned);

	/**
	* Uses GUtility() and `G_UTIL_INFO` [within the underlying galil library] to provide a useful connection string.
	*
	* @param ver Buffer to hold the output string. Buffer will be null terminated, even if the data must be truncated to do so.
	* @param ver_len Length of buffer.
	*
	* @return The success status or error code of the function. See gclib_errors.h for possible values.
	*/
	virtual GReturn GVersion(GCStringOut ver, GSize ver_len);

	/**
	* Uses GUtility() and `G_UTIL_VERSION` [within the underlying galil library] to provide the library version number.
	*
	* @param g Connection's handle.
	* @param info Buffer to hold the output string. Buffer will be null terminated, even if the data must be truncated to do so.
	* @param info_len Length of buffer.
	*
	* @return The success status or error code of the function. See gclib_errors.h for possible values.
	*/
	virtual GReturn GInfo(GCon g, GCStringOut info, GSize info_len);

private:
	bool use_simulator_;
	SOCKET sock; // Socket handle
};

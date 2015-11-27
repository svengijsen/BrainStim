//This example shows how we can send a string of text containing a script command over a tcp/ip connection to another instance of BrainStim.

var sServerIPAddress = "localhost";	//Here we can also use the "localhost" or "127.0.0.1" address for the local host
var sServerPort = "12345";			//Here we need to enter the configured port number
var sScriptCommand = "Log(\"This string is send from the client\")"; //This string contains the script code to be executed at the other instance of BrainStim.

var objTCPNetworkServer = new TCPNetworkServer();
objTCPNetworkServer.ServerDataAvailable.connect(this, this.ServerDataAvailable);

if(objTCPNetworkServer.connectToServer(sServerIPAddress, sServerPort))
{
	objTCPNetworkServer.sendClientData(sScriptCommand);
}
CleanupScript();

/////////////////////////////////////////Local functions////////////////////////////////////////
function ServerDataAvailable(sData)
{
	Log("ServerDataRecieved() called: " + sData);
}

function CleanupScript()
{
	objTCPNetworkServer.ServerDataAvailable.disconnect(this, this.ServerDataAvailable);
	ServerDataAvailable = null;
	ConnectDisconnectScriptFunctions = null;
	objTCPNetworkServer = null;
	ServerAddress = null;
	Log("CleanupScript succeeded!");
	CleanupScript = null;
	BrainStim.cleanupScript();
}
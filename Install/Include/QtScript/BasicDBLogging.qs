var Logging_FirebirdClientobject;
var Logging_FirebirdClientobject_UserName = "user";
var Logging_FirebirdClientobject_Password = "123";
var Logging_FirebirdClientobject_FilePath = "";
var Logging_FirebirdClientobject_StateEnum = 
{
	UNUSED 		: {value: 0, name: "Uninitialized"}, 
	CREATED		: {value: 1, name: "Created"}, 
	OPENED 		: {value: 2, name: "Opened"},
	INITIALIZED 	: {value: 3, name: "Initialized"}
};
var Logging_FirebirdClientobject_CurrentState = Logging_FirebirdClientobject_StateEnum.UNUSED;

function Logging_Construct()
{
	Logging_FirebirdClientobject = new FirebirdClient();
	Log("-Logging_Construct() called.-");
}

function Logging_Destruct()
{
	//Close the database
	if(Logging_FirebirdClientobject_CurrentState.value >= Logging_FirebirdClientobject_StateEnum.OPENED.value)
	{
		var bResult = Logging_FirebirdClientobject.CloseDatabase()
		Log("CloseDatabase() returned: " + bResult);	
	}
	//Destroy the functions
	Logging_Construct = null;
	Logging_Destruct = null;
	Logging_CreateOpenInit = null;	
	Logging_ExecuteDatabaseQuery = null;
	Logging_ShowDatabaseQuery = null;
	Logging_ExportDatabaseQueryToExcel = null;	
	//Destroy the objects
	Logging_FirebirdClientobject = null;
	Log("-Logging_Destruct() called.-");
}

function Logging_CreateOpenInit(databasePath)
{
	var bResult = true;
	if(Logging_FirebirdClientobject_CurrentState.value == Logging_FirebirdClientobject_StateEnum.UNUSED.value)
	{
		var dbFile = new QFile(databasePath);
		if(dbFile.exists())
		{
			Log("CreateDatabase() skipped(already present).");
			Logging_FirebirdClientobject_CurrentState = Logging_FirebirdClientobject_StateEnum.CREATED;
		}
		else
		{
			dbFile = null;
			bResult = Logging_FirebirdClientobject.CreateDatabase(databasePath,Logging_FirebirdClientobject_UserName,Logging_FirebirdClientobject_Password);
			Log("CreateDatabase() returned: " + bResult);
			if(bResult)
				Logging_FirebirdClientobject_CurrentState = Logging_FirebirdClientobject_StateEnum.CREATED;
		}
	}
	
	if(Logging_FirebirdClientobject_CurrentState.value == Logging_FirebirdClientobject_StateEnum.CREATED.value)
	{
		bResult = bResult && Logging_FirebirdClientobject.OpenDatabase(databasePath,Logging_FirebirdClientobject_UserName,Logging_FirebirdClientobject_Password);
		Log("OpenDatabase() returned: " + bResult);
		if(bResult)
		{
			Logging_FirebirdClientobject_FilePath = databasePath;
			Logging_FirebirdClientobject_CurrentState = Logging_FirebirdClientobject_StateEnum.OPENED;
		}
	}

	if(Logging_FirebirdClientobject_CurrentState.value == Logging_FirebirdClientobject_StateEnum.OPENED.value)
	{
		bResult = bResult && Logging_FirebirdClientobject.InitializeDatabase();
		Log("InitializeDatabase() returned: " + bResult);
		if(bResult)
		{
			Logging_FirebirdClientobject_CurrentState = Logging_FirebirdClientobject_StateEnum.INITIALIZED;
		}
	}
	return bResult;	
}

function Logging_ExecuteDatabaseQuery(query)
{
	return Logging_FirebirdClientobject.ExecuteDatabaseQuery(query);
}

function Logging_ShowDatabaseQuery(query)
{
	return Logging_FirebirdClientobject.ShowDatabaseQuery(query);
}

function Logging_ExportDatabaseQueryToExcel(excelPath, query)
{
	return Logging_FirebirdClientobject.ExportDatabasetoExcel(excelPath,query);
}

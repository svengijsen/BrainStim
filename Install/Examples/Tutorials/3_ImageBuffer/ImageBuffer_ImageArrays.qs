var time = QTime.currentTime();
var format = "hhmmsszzz"
var currtimetext = time.toString(format);
var seperator = "?"
var RootDir = StimulGL.getActiveDocumentFileLocation() + "/../Common/Images/Faces_Houses_Objects/";
var tmpString;

var Houses_ImagesPath = Array[];
for (var i=1;i<46;i++)
{
	if(i<10)
	{
		Houses_ImagesPath.push(RootDir + "h_" + "0" + i + ".bmp" + seperator + currtimetext); 
	}
	else
	{
		Houses_ImagesPath.push(RootDir + "h_" + i + ".bmp" + seperator + currtimetext);
	}		
}

var Objects_ImagesPath = Array[];
for (var i=1;i<46;i++)
{
	if(i<10)
	{
		Objects_ImagesPath.push(RootDir + "o_" + "0" + i + ".bmp" + seperator + currtimetext); 
	}
	else
	{
		Objects_ImagesPath.push(RootDir + "o_" + i + ".bmp" + seperator + currtimetext);
	}		
}

var Males_ImagesPath = Array[];
for (var i=1;i<46;i=i+2)
{
	if(i<10)
	{
		Males_ImagesPath.push(RootDir + "m_" + "0" + i + ".bmp" + seperator + currtimetext); 
	}
	else
	{
		Males_ImagesPath.push(RootDir + "m_" + i + ".bmp" + seperator + currtimetext);
	}		
}

var Females_ImagesPath = Array[];
for (var i=2;i<46;i=i+2)
{
	if(i<10)
	{
		Females_ImagesPath.push(RootDir + "f_" + "0" + i + ".bmp" + seperator + currtimetext); 
	}
	else
	{
		Females_ImagesPath.push(RootDir + "f_" + i + ".bmp" + seperator + currtimetext);
	}		
}

var Fixation_Block = Array[];
for (var i=0;i<10;i++)
{
	Fixation_Block.push("fixation"); 
}

var Test_Block = Array[];
for (var i=0;i<10;i++)
{
	Test_Block.push("test"); 
}

var ImagePathArray = Houses_ImagesPath.concat(Objects_ImagesPath,Fixation_Block,Males_ImagesPath,Fixation_Block,Females_ImagesPath,Fixation_Block);
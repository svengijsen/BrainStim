var _imageArray_Time = QTime.currentTime();
var _imageArray_TimeFormat = "hhmmsszzz"
var _imageArray_TimeFormatted = _imageArray_Time.toString(_imageArray_TimeFormat);
var _imageArray_Seperator = "?"
var _imageArray_RootDir = BrainStim.getActiveDocumentFileLocation() + "/../../Common/Images/Faces_Houses_Objects/";
var _imageArray_ImagesPath_Houses = [];
var _imageArray_ImagesPath_Objects = [];
var _imageArray_ImagesPath_Males = [];
var _imageArray_ImagesPath_Females = [];
var _imageArray_ImagesPath_Fixation = [];
var _imageArray_Test = [];
var _imageArray_ImagesPath_Final = [];

for (var i=1;i<46;i++)
{
	if(i<10)
		_imageArray_ImagesPath_Houses.push(_imageArray_RootDir + "h_" + "0" + i + ".bmp" + _imageArray_Seperator + _imageArray_TimeFormatted); 
	else
		_imageArray_ImagesPath_Houses.push(_imageArray_RootDir + "h_" + i + ".bmp" + _imageArray_Seperator + _imageArray_TimeFormatted);		
}

for (var i=1;i<46;i++)
{
	if(i<10)
		_imageArray_ImagesPath_Objects.push(_imageArray_RootDir + "o_" + "0" + i + ".bmp" + _imageArray_Seperator + _imageArray_TimeFormatted); 
	else
		_imageArray_ImagesPath_Objects.push(_imageArray_RootDir + "o_" + i + ".bmp" + _imageArray_Seperator + _imageArray_TimeFormatted);		
}

for (var i=1;i<46;i=i+2)
{
	if(i<10)
		_imageArray_ImagesPath_Males.push(_imageArray_RootDir + "m_" + "0" + i + ".bmp" + _imageArray_Seperator + _imageArray_TimeFormatted); 
	else
		_imageArray_ImagesPath_Males.push(_imageArray_RootDir + "m_" + i + ".bmp" + _imageArray_Seperator + _imageArray_TimeFormatted);		
}

for (var i=2;i<46;i=i+2)
{
	if(i<10)
		_imageArray_ImagesPath_Females.push(_imageArray_RootDir + "f_" + "0" + i + ".bmp" + _imageArray_Seperator + _imageArray_TimeFormatted); 
	else
		_imageArray_ImagesPath_Females.push(_imageArray_RootDir + "f_" + i + ".bmp" + _imageArray_Seperator + _imageArray_TimeFormatted);		
}

for (var i=0;i<10;i++)
	_imageArray_ImagesPath_Fixation.push("fixation"); 

for (var i=0;i<10;i++)
	_imageArray_Test.push("test"); 

//Let's combine everything
_imageArray_ImagesPath_Final = _imageArray_ImagesPath_Houses.concat(_imageArray_ImagesPath_Objects,_imageArray_ImagesPath_Fixation,_imageArray_ImagesPath_Males,_imageArray_ImagesPath_Fixation,_imageArray_ImagesPath_Females,_imageArray_ImagesPath_Fixation);
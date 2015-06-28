var _xmlTreePath;
var _xmlFile;
var _xmlReader;
var _currentXmlTreePathIndex;

function parseXMLContents() 
{
	//Log("parseXMLContents() called.");
	if (!_xmlReader.isStartElement() || _xmlReader.name() != _xmlTreePath[_currentXmlTreePathIndex])
		return "";
	_currentXmlTreePathIndex++;
	return parseXMLTreeItem(_currentXmlTreePathIndex);
}

function parseXMLTreeItem(nTreePathIndex)
{
	//Log("parseXMLTreeItem(" + nTreePathIndex + ") called.");
	if(nTreePathIndex==_xmlTreePath.length)
		return _xmlReader.readElementText(); 
	while (_xmlReader.readNextStartElement()) 
	{
		if (_xmlReader.name() == _xmlTreePath[nTreePathIndex])
		{
			return parseXMLTreeItem(nTreePathIndex+1);
			
//			QString from;
//			QString to;
//			QString conversion;
//			while (xml.readNextStartElement()) {
//			if (xml.name() == "from")
//			    from = readNextText();
//			else if (xml.name() == "to")
//			    to = readNextText();
//			else if (xml.name() == "conversion")
//			    conversion = readNextText();
//			#ifndef USE_READ_ELEMENT_TEXT
//			xml.skipCurrentElement();
//			#endif
//			}
			
		}
		else
		{
			_xmlReader.skipCurrentElement();
		}
	}
}

function getXmlFileItemValue(__xmlFilePath, __xmlTreePath)
{
	var _xmlFilePath = __xmlFilePath;
	_xmlTreePath = __xmlTreePath;

	_xmlFile = new QFile(_xmlFilePath);
	_xmlReader = new QXmlStreamReader();
	_currentXmlTreePathIndex = 0;
	var sRetVal = "";
	
	if(_xmlTreePath.length > 0)
	{
		if(_xmlFile.open(QIODevice.ReadOnly))
		{
			_xmlReader.setDevice(_xmlFile);
			if (_xmlReader.readNextStartElement() && _xmlReader.name() == _xmlTreePath[_currentXmlTreePathIndex])
			{
				sRetVal = parseXMLContents();
			}
			if (_xmlReader.tokenType() == QXmlStreamReader.Invalid)
				_xmlReader.readNext();
			if (_xmlReader.hasError()) 
			{
				_xmlReader.raiseError();
				Log("Error: " + _xmlReader.errorString());
			}
			_xmlFile.close();
		}
	}
	return sRetVal;
}

function cleanupXmlFileReader()
{
	parseXMLContents = null;
	parseXMLTreeItem = null;
	_xmlReader = null;
	_xmlFile = null;
	_xmlTreePath = null;
	getXmlFileItemValue = null;
	cleanupXmlFileReader = null;
}

//Usage example
//Include("../Install/Include/QtScript/ModuleXMLReader.qs");
//Log("Result: " + getXmlFileItemValue("E:\\Projects\\BrainStim\\Source\\base.props", ["Project","PropertyGroup","LIBRARY_DIR"]));
//cleanupXmlFileReader();
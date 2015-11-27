//Includes:
Include("QtScript/BasicQMLExperiment.qs");//Do not use a backslash (\) in a string, use forward slashes for a directory definition!//../../include/
Include("ImageBuffer_ImageArrays.qs"); //Inside this file we prepare a array that is used for storing the imagebuffer elements

//Included variables:
BasicExperiment.sExmlFilePath = BrainStim.getActiveDocumentFileLocation() + "/" + "ImageBuffer.exml";
BasicExperiment.nTestModeTriggerDuration = 2000;

//Custom script references (variables):
var QML2Viewer_Global_QmlFilePath = BrainStim.getActiveDocumentFileLocation() + "/" + "ImageBuffer.qml";

//Local variables:

//Local functions
function preCacheImages()
{
	var nCounter = 0;
	var tmpPixMap = new QPixmap();
	
	for(var i=0;i<_imageArray_ImagesPath_Final.length;i++)
	{
		var tmpString = _imageArray_ImagesPath_Final[i];
		if(tmpString != "fixation")
		{
			try
			{
				tmpString = tmpString.split(_imageArray_Seperator)[0];
				if(tmpPixMap.load(tmpString))
				{					
					tmpString = BasicExperiment.QMLViewerObject.addPixmapToImageBuffer(tmpPixMap);
					if(BasicExperiment.QMLViewerObject.updatePixmapIDFromImageBuffer(tmpString,_imageArray_ImagesPath_Final[i]))
						nCounter++;
					else
						Log("updatePixmapIDFromImageBuffer() failed");
					Log("- pre-Cached :" + _imageArray_ImagesPath_Final[i]);
				}
				else
				{
					Log(".*. Something went wrong loading the Image: " + tmpString);
				}
			} 
			catch(e)
			{
				Log(".*. Something went wrong pre-Caching the Image:" + e);
			}	
		}
	}
	Log("... pre-Caching images done(" + nCounter + " item(s) cached)!");
}

function preProcessImages() 
{
	Log("... pre-Processing images");
	var currLenght = _imageArray_ImagesPath_Final.length;
	var nCounter = 0;
	var tmpPixMap = new QPixmap();
	var transform = new QTransform();
	transform.rotate( -60,Qt.XAxis )
	for(var i=0;i<currLenght;i++)
	{
		if(_imageArray_ImagesPath_Final[i] != "fixation")
		{		
			//Retrieve a pixmap image from the image buffer using the created array element
			if(BasicExperiment.QMLViewerObject.getPixmapFromImageBuffer(tmpPixMap, _imageArray_ImagesPath_Final[i]))
			{
				Log("... getPixmapFromImageBuffer(" + _imageArray_ImagesPath_Final[i] + ") successfully done!");
				//Here we modify the pixmap image
				tmpPixMap = tmpPixMap.scaledToWidth(300);
				tmpPixMap = tmpPixMap.transformed(transform, Qt.FastTransformation);
				//Add the modified pixmap image to the image buffer and store the new element name
				var tmpString = BasicExperiment.QMLViewerObject.addPixmapToImageBuffer(tmpPixMap);
				Log("... addPixmapToImageBuffer returned: " + tmpString);
				//Remove the modified pixmap image from the image buffer
				Log("... removePixmapFromImageBuffer returned: " + BasicExperiment.QMLViewerObject.removePixmapFromImageBuffer(tmpString));
				//Update the original pixmap image from the image buffer
				Log("... updatePixmapFromImageBuffer(" + tmpString + ") returned: " + BasicExperiment.QMLViewerObject.updatePixmapFromImageBuffer(tmpPixMap,_imageArray_ImagesPath_Final[i]));
			}		
			else
			{
				Log("preProcessImages --> Could not getPixmapFromImageBuffer()");
			}
		}
	}
	Log("... pre-Processing images done(" + _imageArray_ImagesPath_Final.length + " item(s) processed)!");	
}

//Override Public Functions from the BasicQMLExperiment.qs include (that includes BasicExperiment):
BasicExperiment.__proto__.NewInitBlockTrial = function()
{
	var _currentBlockID = BasicExperiment.nCurrentExperimentStructureState.CurrentBlock_BlockID;
	var _currentBlockNumber = BasicExperiment.cExperimentStructure_Object.getBlockPointerByID(_currentBlockID).getBlockNumber();
	var _currentTrialNumber = BasicExperiment.nCurrentExperimentStructureState.CurrentBlock_TrialNumber;
	BrainStim.write2OutputWindow("*** Going to initialize a new Block-trial: BlockNr: " + _currentBlockNumber + ", TrialNr: " + _currentTrialNumber);

	if(_currentBlockNumber == 1)
	{
		BasicExperiment.QMLViewerObject.invokeQml2Method("functions", "setImageFunction", _imageArray_ImagesPath_Final[_currentTrialNumber]);
	}
}

BasicExperiment.__proto__.ExperimentStateChanged = function(currentState, sDateTimeStamp)
{
	//This Funtion is called automatically at each time the Experiment State changes
	//See Help documentation (search for "ExperimentManager::ExperimentState") for a description of the different states
	
	if(currentState == ExperimentManager.ExperimentState.ExperimentManager_Initialized)
	{
		BrainStim.write2OutputWindow("*** ExperimentStateChanged to: Initialized at " + sDateTimeStamp);
		preCacheImages();	
		preProcessImages();
	}
	else if(currentState == ExperimentManager.ExperimentState.ExperimentManager_Started)
	{
		BrainStim.write2OutputWindow("*** ExperimentStateChanged to: Started at " + sDateTimeStamp);
	}	
	else if(currentState == ExperimentManager.ExperimentState.ExperimentManager_Stopped)
	{
		BrainStim.write2OutputWindow("*** ExperimentStateChanged to: Stopped at " + sDateTimeStamp);
	}
}

//Run the experiment:
BasicExperiment.RunExperiment();
var sCurrentScriptLocation = BrainStim.getActiveDocumentFileLocation();
var ImageProcessorObj = new ImageProcessor();
var bResult = false;

bResult = ImageProcessorObj.ScalePngFile(sCurrentScriptLocation + "/ExampleImage.png", sCurrentScriptLocation + "/ExampleImage_Scaled.png", 3, 0, 255, true,true);
if(bResult==false)
{
	Log("An Error occured during ScalePngFile(), exiting!");
}
else
{
	bResult = ImageProcessorObj.ConvertPngToDatFile(sCurrentScriptLocation + "/ExampleImage_Scaled.png", sCurrentScriptLocation + "/ExampleImage_Scaled.dat", true);
	if(bResult==false)
	{
		Log("An Error occured during ConvertPngToDatFile(), exiting!");
	}
	else
	{
		bResult = ImageProcessorObj.ConvertDatToPngFile(sCurrentScriptLocation + "/ExampleImage_Scaled.dat", sCurrentScriptLocation + "/ExampleImage_Scaled_FromDat.png", true);
		if(bResult==false)
		{
			Log("An Error occured during ConvertDatToPngFile(), exiting!");
		}		
	}
}

ImageProcessorObj.deleteLater();
ImageProcessorObj = null;